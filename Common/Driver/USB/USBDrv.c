/*
********************************************************************************
*                   Copyright (c) 2008, CHENFEN
*                         All rights reserved.
*
* File Name：   D12ci.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             chenfen      2008-12-12          1.0
*    desc:    ORG.
********************************************************************************
*/

#include "USBConfig.h"

#define IN_USBDRV
#include "hw_dma.h"
#include "dma.h"

#ifdef USB

#define USB_DMA

/*
--------------------------------------------------------------------------------
  Function name : ResetOutEpDataToggle
  Author        : chenfen
  Description   : IN端点恢复DATA TOGGLE到DATA0
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void ResetInEpDataToggle(uint8 ep)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    //OtgReg->Device.InEp[ep].DiEpCtl = 1<<28;
    dev_regs->in_ep[ep].diepctl  |= 1<<28;
}

/*
--------------------------------------------------------------------------------
  Function name : ResetOutEpDataToggle
  Author        : chenfen
  Description   : OUT端点恢复DATA TOGGLE到DATA0
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void ResetOutEpDataToggle(uint8 ep)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    //OtgReg->Device.OutEp[ep].DoEpCtl = 1<<28;
    dev_regs->out_ep[ep].doepctl |= 1<<28;
}


/*
--------------------------------------------------------------------------------
  Function name : MUSBHSFC_SetEndpointStatus
  Author        : chenfen
  Description   : 设置端点状态
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void UsbSetEpSts(uint8 dir, uint8 EpNum, uint8 status)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    if (dir!=0)
    {
        if (status != 0)
        {
            volatile uint32 value = dev_regs->in_ep[EpNum].diepctl;
            //value |= ((1<<15)|(2<<18));
            (value & (1ul<<31))? (value |= (1<<30 | 1<<21)) : (value |= (1<<21));
            dev_regs->in_ep[EpNum].diepctl = value;  //In endpoint send stall handshack
        }
        else
            dev_regs->in_ep[EpNum].diepctl &= ~(1<<21);
    }
    else
    {
        if (status != 0)
            dev_regs->out_ep[EpNum].doepctl |= 1<<21;  //Out endpoint send stall handshack
        else
            dev_regs->out_ep[EpNum].doepctl &= ~(1<<21);
    }
}

IRAM_USB
uint8 UsbGetEpSts(uint8 dir, uint8 EpNum)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;
    uint8 status;

    if (dir!=0)
    {
        status = (dev_regs->in_ep[EpNum].diepctl>>21) & 0x1;  
    }
    else
    {   
        status = (dev_regs->out_ep[EpNum].doepctl>>21) & 0x1;  //Out endpoint send stall handshack
    }

    return status;
}


/*
--------------------------------------------------------------------------------
  Function name : UsbWriteEp0
  Author        : chenfen
  Description   : 
            * This function writes a packet into the Tx FIFO associated with the
            * EP. For non-periodic EPs the non-periodic Tx FIFO is written.
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void UsbWriteEp0(uint16 len, void* buf)
{
    uint32 i;
    otg_core_if_t * otg_core = (otg_core_if_t *)USB_REG_BASE;
    uint8 *ptr = (uint8*)buf;
    uint32 *fifo = (uint32 *)USB_EP_FIFO(0);
  
    if (((otg_core->core_regs.gnptxsts & 0xFFFF) >= (len+3)/4) && (((otg_core->core_regs.gnptxsts>>16) & 0xFF) > 0))
    {
        otg_core->dev_regs.in_ep[0].dieptsiz = len | (1<<19);
        otg_core->dev_regs.in_ep[0].diepctl = (1ul<<26) | (1ul<<31);   //Endpoint Enable, Clear NAK

        for (i = 0; i < len; i += 4)
        {
            *fifo = (ptr[3]<<24) | ( ptr[2]<<16) | (ptr[1]<<8) | ptr[0];
            //*fifo = *ptr++;
            ptr += 4;
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : UsbReadEp0
  Author        : chenfen
  Description   : 
            * This function reads a packet from the Rx FIFO into the destination
            * buffer.
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/12/10         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void UsbReadEp0(uint16 len, void *buf)
{
    uint32 i;
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;
    uint32 *ptr = (uint32*)buf;
    uint32 *fifo = (uint32 *)USB_EP_FIFO(0);

    for (i=0; i<len; i+=4)
    {
        *ptr++ =  *fifo;
    }

    dev_regs->out_ep[0].doeptsiz = 0x40 | (1<<19) | (1<<29);
}

/*
--------------------------------------------------------------------------------
  Function name : UsbReadEndpoint
  Author        : chenfen
  Description   : 读取端点数据
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
uint16 UsbReadEndpoint(uint8 endp, uint16 len, void * buf)
{
    uint32 i;
    uint32 *ptr = (uint32*)buf;
    uint32 *fifo = (uint32 *)USB_EP_FIFO(endp);

    for (i=0; i<len; i+=4)
    {
        *ptr++ = *fifo;
    }

    //dev_regs->out_ep[endp].doeptsiz = (1ul<<19) | BulkEndpSize;
    //dev_regs->out_ep[endp].doepctl|= (1ul<<15) | (1ul<<26) | (1ul<<31) | BulkEndpSize; //Active ep, Clr Nak, endpoint enable

    return len;
}

/*
--------------------------------------------------------------------------------
  Function name : MUSBHSFC_WriteEndpoint
  Author        : chenfen
  Description   : 写端点
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
uint16 UsbWriteEndpoint(uint8 endp, uint16 len, void * buf)
{
    uint32 i, pid;
    uint32 *fifo = (uint32 *)USB_EP_FIFO(endp);
    otg_core_if_t * otg_core = (otg_core_if_t *)USB_REG_BASE;
    otg_dev_in_ep_regs_t * in_ep = (otg_dev_in_ep_regs_t *)&otg_core->dev_regs.in_ep[endp];

    if ((otg_core->core_regs.gnptxsts & 0xFFFF) >= ((len+3)>>2))
    {
        in_ep->dieptsiz = len | (1<<19) /*| (1<<29)*/;
        pid = in_ep->diepctl & (1<<16);
        in_ep->diepctl = (1<<15)|(2<<18)/*|(ep<<22)*/|BulkEndpInSize; //endpoint enable
        in_ep->diepctl |= (pid==0)?(1<<28):(1<<29);
        in_ep->diepctl |= (1ul<<26)|(1ul<<31);

        if (!((uint32)buf & 0x3))
        {
            uint32 *ptr = (uint32*)buf;
            for (i=0; i<len; i+=4)
            {
                *fifo = *ptr++;
            }
        }
        else
        {
            uint8 *ptr = (uint8*)buf;
            for (i=0; i<len; i+=4)
            {
                *fifo = (ptr[3]<<24) | ( ptr[2]<<16) | (ptr[1]<<8) | ptr[0];
                ptr += 4;
            }
        }
    }
    
    return (len);
}


#ifdef DMA_BULKIN_EN

/*
--------------------------------------------------------------------------------
  Function name : UsbWriteEpDma
  Author        : chenfen
  Description   : 写端点
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
uint16 UsbWriteEpDma(uint8 endp, uint16 len, void * buf, pFunc CallBack)
{
    uint32 pid;
    uint32 *fifo = (uint32 *)USB_EP_FIFO(endp);
    otg_core_if_t * otg_core = (otg_core_if_t *)USB_REG_BASE;
    otg_dev_in_ep_regs_t * in_ep = (otg_dev_in_ep_regs_t *)&otg_core->dev_regs.in_ep[endp];
    
    if ((otg_core->core_regs.gnptxsts & 0xFFFF) >= ((len+3)>>2))
    {
        in_ep->dieptsiz = len | (1<<19) /*| (1<<29)*/;
        pid = in_ep->diepctl & (1<<16);
        in_ep->diepctl = (1<<15)|(2<<18)/*|(ep<<22)*/|BulkEndpInSize; //endpoint enable
        in_ep->diepctl |= (pid==0)?(1<<28):(1<<29);
        in_ep->diepctl |= (1ul<<26)|(1ul<<31);

        #ifdef USB_DMA
        {
            uint32 timeout = 0;
    	    eDMA_CHN channel;
            DMA_CFGX DmaCfg = {DMA_CTLL_USB_TX, DMA_CFGL_USB_TX, DMA_CFGH_USB_TX,0};

    	    channel = DmaGetChannel();
    	    if ((channel != DMA_FALSE)/* && (channel < DMA_CHN2)*/)
    	    {
    	        DmaStart((uint32)(channel), (uint32)buf, (uint32)USB_EP_FIFO(endp), len>>2, &DmaCfg, CallBack);
    	        while (DMA_SUCCESS != DmaGetState(channel))
        	    {
        	        if (++timeout > len * 1000)
        	        {
        	            printf("USB DMA write Error: len = %d!\n", len);
                        break;
        	        }
        	    }
    	    }    
        }
        #else
        {
            for (i=0; i<len; i+=4)
            {
                *fifo = *ptr++;
            }
        }
        #endif
    }
    
    return (len);

}
#endif

#ifdef DMA_BULKOUT_EN
/*
--------------------------------------------------------------------------------
  Function name : UsbReadEpDma
  Author        : chenfen
  Description   : 读取端点数据
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
uint16 UsbReadEpDma(uint8 endp, uint16 len, void * buf, pFunc CallBack)
{
    uint32 i;
    uint32 *ptr = (uint32*)buf;
    uint32 *fifo = (uint32 *)USB_EP_FIFO(endp);
    #ifdef USB_DMA
    {
        uint32 timeout = 0;
	    eDMA_CHN channel;
        DMA_CFGX DmaCfg = {DMA_CTLL_USB_RX, DMA_CFGL_USB_RX, DMA_CFGH_USB_RX,0};
    
	    channel = DmaGetChannel();
	    if ((channel != DMA_FALSE)/* && (channel < DMA_CHN2)*/)
	    {
            DmaStart((uint32)(channel), USB_EP_FIFO(endp), (uint32)buf, len>>2, &DmaCfg, NULL);
	        while (DMA_SUCCESS != DmaGetState(channel))
    	    {
    	        if (++timeout > len * 1000)
    	        {
    	            printf("USB DMA Read Error: len = %d!\n", len);
                    break;
    	        }
    	    }
	    }    
    }
    #else
    {
        for (i=0; i<len; i+=4)
        {
            *ptr++ = *fifo;
        }        
    }
    #endif

    return len;
}

#endif


#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

