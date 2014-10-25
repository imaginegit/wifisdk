/*
********************************************************************************
*                   Copyright (c) 2008,Yangwenjie
*                         All rights reserved.
*
* File Name：   spi.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             yangwenjie      2009-1-15          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_SPI_


#include "DriverInclude.h"

_ATTR_DRIVER_DATA_ pFunc SpiDmaCallBack = NULL;

/*
--------------------------------------------------------------------------------
  Function name : int32 SPIWrite(uint32 ch, uint32 baudRate, uint32 CtrMode, uint8 *pdata, uint32 size)
  Author        : 
  Description   : 
                  
  Input         : baudRate:   unit:HZ
                  ch：        spi cs
  Return        : 

  History:     <author>         <time>         <version>    
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
int32 SPIInit(uint32 ch, uint32 baudRate, uint32 CtrMode)
{
    uint32 clk;
    
    if (SPICtl->SPI_SER)
    {
        return BUSY;
    }     
    
    Grf->IOMUX_CON1 = 0x00020000 | (ch << 1);
    
    clk = GetSpiFreq();
    if ((baudRate > 12*1000*1000) && (clk > 24))
    {
        //Need Configer spi clk to other
        Scu->CLKSEL_SPI = ((SPI_CLK_SEL_MASK << 16) | MMC_CLK_SEL_PLL);
        
        SPICtl->SPI_BAUDR = clk * 1000 * 1000 / baudRate;
    }
    else
    {
        //Need Configer spi clk to 24M
        Scu->CLKSEL_SPI = ((SPI_CLK_SEL_MASK << 16) | SPI_CLK_SEL_24M);
        
        SPICtl->SPI_BAUDR = 24 * 1000 * 1000 / baudRate;
    }
    
    SPICtl->SPI_CTRLR0 = CtrMode;
    SPICtl->SPI_SER    = ((uint32)(0x01) << ch);
    
}

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : int32 SPIWrite(uint32 ch, uint32 baudRate, uint32 CtrMode, uint8 *pdata, uint32 size)
  Author        : 
  Description   : 
                  
  Input         : baudRate:   unit:HZ
                  ch：        spi cs
  Return        : 

  History:     <author>         <time>         <version>    
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void SPIDeInit(void)
{
    SPICtl->SPI_ENR = 0;    
    SPICtl->SPI_SER = 0;    
}

/*
--------------------------------------------------------------------------------
  Function name : int32 SPIWrite(uint32 ch, uint32 baudRate, uint32 CtrMode, uint8 *pdata, uint32 size)
  Author        : 
  Description   : 
                  
  Input         : baudRate:   以Hz为单位
                  ch：        spi cs
  Return        : 

  History:     <author>         <time>         <version>    
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
int32 SPIWrite(uint8 *pdata, uint32 size)
{
    int32  ret;
    uint32 timeout = 200000;
    
    SPICtl->SPI_ENR = SPI_DISABLE;
    SPICtl->SPI_CTRLR0 &= ~(TRANSMIT_RECEIVE_MASK);
    SPICtl->SPI_CTRLR0 |= SPI_CTL_TX_SPINOR;
    SPICtl->SPI_ENR = SPI_ENABLE;
    
    while (size)
    {
        if ((SPICtl->SPI_SR & TRANSMIT_FIFO_FULL) != TRANSMIT_FIFO_FULL)
        {
            SPICtl->SPI_TXDR[0] = *pdata++;
            size--;
            timeout = 200000;
        }
        if (--timeout == 0)
        {
            ret = TIMEOUT;
            break;
        }
    }

    return OK;
}

/*
--------------------------------------------------------------------------------
  Function name : int32 SPIRead(uint32 ch, uint32 baudRate, uint32 CtrMode, uint8 *pdata, uint32 size)
  Author        : 
  Description   : 
                  
  Input         : baudRate:   unit:HZ
                  ch：        spi cs
  Return        : 

  History:     <author>         <time>         <version>    
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
int32 SPIRead(uint8 *pdata, uint32 size, uint32 dumy)
{
    int32  i, ret;
    uint32 timeout = 200000;

    SPICtl->SPI_ENR = SPI_DISABLE;
    SPICtl->SPI_CTRLR0 &= ~(TRANSMIT_RECEIVE_MASK);
    SPICtl->SPI_CTRLR0 |= SPI_CTL_RX_SPINOR;
    SPICtl->SPI_CTRLR1 = size - 1;
    SPICtl->SPI_ENR = SPI_ENABLE;
    
    while (size)
    {
        for (i = 0; i < 200; i++)
        {
            if ((SPICtl->SPI_SR & RECEIVE_FIFO_EMPTY) != RECEIVE_FIFO_EMPTY)
            {
                if (dumy > 0)   //发送完命令后, 可能要等几个时钟,数据才出来
                {
                    *pdata = (uint8)(SPICtl->SPI_RXDR[0] & 0xFF);
                    dumy--;
                }
                else
                {
                    *pdata++ = (uint8)(SPICtl->SPI_RXDR[0] & 0xFF);  //无效数据
                    size--;
                }
                timeout = 200000;
                break;
            }
        }
        
        if (--timeout == 0)
        {
            ret = TIMEOUT;
            break;
        }
    }

    return OK;
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : int32 SPIDmaWrite(uint32 ch, uint32 baudRate, uint32 CtrMode, uint8 *pdata, uint32 size, pFunc CallBack)
  Author        : 
  Description   : 
                  
  Input         : baudRate:   unit:HZ
                  ch：        spi cs
  Return        : 

  History:     <author>         <time>         <version>    
  desc:         ORG
  Note:        After finish spi transfer,need call SPIDeInit function to release bus.
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
int32 SPIDmaWrite(uint8 *pdata, uint32 size, pFunc CallBack)
{
    int32  ret;
    int32 channel;    
    uint32 timeout = 200000;
    DMA_CFGX DmaCfg = {DMA_CTLL_SPI8_MTX,DMA_CFGL_SPI8_MTX,DMA_CFGH_SPI8_MTX,0};
    
    SPICtl->SPI_ENR = SPI_DISABLE;
    SPICtl->SPI_CTRLR0 &= ~(TRANSMIT_RECEIVE_MASK);
    SPICtl->SPI_CTRLR0 |= SPI_CTL_TX_SPINOR;
    SPICtl->SPI_DMATDLR = 8;
    SPICtl->SPI_DMACR   = TRANSMIT_DMA_ENABLE;
    SPICtl->SPI_ENR = SPI_ENABLE;

    channel = DmaGetChannel();
    if (channel != DMA_FALSE)
    {
        DmaStart((uint32)(channel), (UINT32)(pdata),(uint32)(&(SPICtl->SPI_TXDR[0])),size, &DmaCfg, CallBack);
        return channel;
    }
    
    return (-1);
}

/*
--------------------------------------------------------------------------------
  Function name : int32 SPIDmaRead(uint32 ch, uint32 baudRate, uint32 CtrMode, uint8 *pdata, uint32 size, pFunc CallBack)
  Author        : 
  Description   : 
                  
  Input         : baudRate:   unit:HZ
                  ch：        spi cs
  Return        : 

  History:     <author>         <time>         <version>    
  desc:         ORG
  Note:        After finish spi transfer,need call SPIDeInit function to release bus.
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
int32 SPIDmaRead(uint8 *pdata, uint32 size, pFunc CallBack)
{
    int32  ret;
    int32 channel;    
    uint32 timeout = 200000;
    DMA_CFGX DmaCfg = {DMA_CTLL_SPI8_MRX,DMA_CFGL_SPI8_MRX,DMA_CFGH_SPI8_MRX,0};

    SPICtl->SPI_ENR = SPI_DISABLE;
    SPICtl->SPI_CTRLR0 &= ~(TRANSMIT_RECEIVE_MASK);
    SPICtl->SPI_CTRLR0 |= SPI_CTL_RX_SPINOR;
    SPICtl->SPI_CTRLR1 = size - 1;
    SPICtl->SPI_DMARDLR = 7;
    SPICtl->SPI_DMACR   = RECEIVE_DMA_ENABLE;
    SPICtl->SPI_ENR = SPI_ENABLE;
    
    channel = DmaGetChannel();
    if (channel != DMA_FALSE)
    {   
        DmaStart((uint32)(channel),(uint32)(&(SPICtl->SPI_TXDR[0])),(UINT32)(pdata),size, &DmaCfg, CallBack);
        return channel;
    }
    
    return (-1);
}

/*
********************************************************************************
*
*                         End of spi.c
*
********************************************************************************
*/ 

