/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name：   I2c.c
* 
* Description:  C program template
*
* History:      <author>          <time>        <version>       
*             yangwenjie      2008-11-20         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_I2C_

#include "DriverInclude.h"

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : I2CStart(void)
  Author        : yangwenjie
  Description   : I2C start
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void I2CStart(void)
{
    // Set start to LCMR
    I2cReg->I2C_LCMR |= (I2C_LCMR_RESUME | I2C_LCMR_START);
}

/*
--------------------------------------------------------------------------------
  Function name : I2CStop(void)
  Author        : yangwenjie
  Description   : I2C stop
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void I2CStop(void)
{
    int timeout;

    timeout = 200000;   // timeout must long enogh,that will suit some slow suitation.
	
    I2cReg->I2C_LCMR |=( I2C_LCMR_RESUME | I2C_LCMR_STOP);
    
    while (((I2cReg->I2C_LCMR & I2C_LCMR_STOP) != 0) && (timeout > 0))
    {
        Delay10cyc(1);
        timeout--;
    }
    
    I2cReg->I2C_IER = 0x0;
    
    I2cReg->I2C_ISR = 0x0;
}

/*
--------------------------------------------------------------------------------
  Function name : I2CSendData(UINT8 Data, BOOL StartBit)
  Author        : yangwenjie
  Description   :
                  
  Input         : Data -- data will send(UINT8)
                  
  Return        : 0 -- success
                  non 0 -- failure

  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
int32 I2CSendData(UINT8 Data, BOOL StartBit)
{
    int intstatus;
    int ackcorestatus;
    int timeout;  // 如果I2C设备没有应答也需要超时退出

    timeout = 200000;   // timeout要足够长,以适应个别I2C应答较慢的情况
       
    I2cReg->I2C_CONR &= ~(I2C_CONR_MASTER_MASK); 
    I2cReg->I2C_CONR |= I2C_CONR_MTX;
    // Set MTXR
         
    I2cReg->I2C_MTXR = Data;
    if (StartBit)
        
        I2cReg->I2C_LCMR = (I2C_LCMR_RESUME | I2C_LCMR_START);
    else
      
        I2cReg->I2C_LCMR = I2C_LCMR_RESUME ;
    
    // ACK 释放SDA线,等待从设备应答            
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    // waiting ACK
    do
    {   
        intstatus = I2cReg->I2C_ISR;
        // If  Arbitration Lost, will stop and return
        if ((intstatus & I2C_INT_AL) != 0)
        {
            // Clear INT_AL status
           I2cReg->I2C_ISR &= ~( I2C_INT_AL);
            // stop
            I2CStop();
            
            return ERROR;
        }
        Delay10cyc(1);
        
        ackcorestatus = I2cReg->I2C_LSR;
        timeout--;
    }
    while ((((ackcorestatus & I2C_LSR_NACK) != 0) || ((intstatus& I2C_INT_MACK) == 0)) && (timeout > 0));

    // Clear INT_MACK status
   
    I2cReg->I2C_ISR &= ~(I2C_INT_MACK);

    if (timeout)
    {
        return OK;
    }
    else
    {
        return TIMEOUT;
    }
}


/*
--------------------------------------------------------------------------------
  Function name : I2CReadData(UINT8 *Data)
  Author        : yangwenjie
  Description   : 
                  
  Input         : Data -- to put the read data.
                  
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
int32 I2CReadData(UINT8 *Data)
{
    int intstatus;
    int timeout;  // 如果I2C设备没有应答也需要超时退出

    timeout = 200000;   // timeout要足够长,以适应个别I2C应答较慢的情况
    
    I2cReg->I2C_LCMR |= I2C_LCMR_RESUME;
    // waiting ACK
    do
    {
       
        intstatus = I2cReg->I2C_ISR;
        // Clear INT_MACK status
        
        if ((intstatus & I2C_INT_AL) != 0)
        {
            // Clear INT_AL status
           
            I2cReg->I2C_ISR &= ~(I2C_INT_AL);
            // stop
            I2CStop();

            return ERROR;
        }
        Delay10cyc(1);
        timeout--;
    }
    while (((intstatus & I2C_INT_MACKP) == 0) && (timeout > 0));

    
    *Data = (UINT8)(I2cReg->I2C_MRXR);

    // Clear INT_MACKP status
    
    I2cReg->I2C_ISR &= ~(I2C_INT_MACKP);

    return OK;
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : I2CSetSpeed(UINT16 speed)
  Author        : yangwenjie
  Description   : 
                  
  Input         : speed -- unit is k,for example 400 is mean 400k
                  
  Return        :TRUE -- success
                 FALSE -- failure

  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2CSetSpeed(UINT16 speed)
{
    UINT32 exp;
    UINT32 rem;
    UINT32 scalespeed;
    UINT32 freqbase;

    /*
    SCL Divisor = (I2CCDVR[5:3] + 1) x 2^((I2CCDVR[2:0] + 1)
    SCL = PCLK/ 5*SCLK Divisor
    rem = I2CCDVR[5:3]
    exp = I2CCDVR[2:0]
    */
    
	freqbase   = (GetPclkFreq()/**1000*/) / 5;//by YWJ 04.19
	
    scalespeed = speed << 3;      // speed以K为单位   (rem+1)<=8 ,先以8来确定exp

    for (exp = 0;exp < 7;exp++)
    {
        scalespeed = scalespeed << 1;
        if (scalespeed > freqbase)
        {
            break;
        }
    }

    rem = (freqbase) / speed / (1 << (exp + 1)) - 1;
   
    I2cReg->I2C_OPR &= ~(I2C_OPR_DIV_SPEED_MASK);
    I2cReg->I2C_OPR |= ((rem << 3) | exp);
    return TRUE;
}

/*
--------------------------------------------------------------------------------
  Function name : I2C_Init(UINT8 SlaveAddress, UINT16 nKHz)
  Author        : yangwenjie
  Description   : 
                  
  Input         : SlaveAddress :I2C device address
                  nKHz         :I2C max speed
                  
  Return        :TRUE -- ok
                 FALSE -- fail

  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2C_Init(UINT8 SlaveAddress, UINT16 nKHz)
{
    UserIsrDisable();
    
    ScuClockGateCtr(CLOCK_GATE_I2C, 1);

    GpioMuxSet(GPIOPortC_Pin7,IOMUX_GPIOC7_I2C_SCL);
    GpioMuxSet(GPIOPortD_Pin0,IOMUX_GPIOD0_I2C_SDA);

    I2cReg->I2C_OPR &= ~(I2C_OPR_RESET);
    I2cReg->I2C_IER = 0x0;
    
    I2C_slaveaddr = SlaveAddress & 0xfe;
    
    if (I2C_slaveaddr == (0x55 << 1))
    {
        //IoMuxSetI2C(I2C_Internal);
    }
    else
    {
        //IoMuxSetI2C(I2C_External);
    }

    if (I2C_oldspeed != nKHz)
    {
        I2cReg->I2C_OPR &= ~( I2C_OPR_ENABLE);
        I2CSetSpeed(nKHz);
        I2C_oldspeed = nKHz;
    }
    
    I2cReg->I2C_OPR |= I2C_OPR_ENABLE;
    
    return TRUE;

}

/*
--------------------------------------------------------------------------------
  Function name : I2C_Write(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode)
  Author        : yangwenjie
  Description   :
                  
  Input         : RegAddr -- I2C device address.
                  *pData -- data pointer.
                  size -- 一write size
                  mode -- NormalMode/DirectMode
                  
  Return        :TRUE --ok
                 FALSE -- fail

  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2C_Write(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode)
{
    BOOL ret = TRUE;
    
    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x0;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;
    
    if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
    {
        printf("I2C_Write: Send slaveaddr(%d) Error!\n",I2C_slaveaddr);
        ret = FALSE;
        goto error;
    }
    
    if (mode == NormalMode)
    {
        if (I2CSendData(RegAddr, FALSE) != OK)
        {
            printf("I2C_Write: Send Command(%d) Error!\n",RegAddr);
            ret = FALSE;
            goto error;
        }
    }
    // Write data
    while (size)
    {
        if (I2CSendData(*pData, FALSE) != OK)
        {
            printf("I2C_Write: Send Data(%d) Error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        pData++;
        size --;
    }
    
error:
    // Set CONR ACK    释放SDA总线
    I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
    I2CStop();
    
    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name : I2C_Read(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode)
  Author        : yangwenjie
  Description   : 
                  
  Input         : RegAddr 
                  size -- read size
                  mode -- NormalMode/DirectMode
                  
  Return        :TRUE -- ok
                 FALSE --fail

  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2C_Read(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode)
{
    BOOL ret = TRUE;
    
    // Set I2C_IER to disable all kind of I2C’s interrupt type.
    I2cReg->I2C_IER = 0x00;
    // Clear status register
    I2cReg->I2C_ISR = 0x0;
    
    if ((mode == NormalMode)||(mode ==RDA_5820Mode)) // 为了以后扩展
    {
        if (I2CSendData(I2C_slaveaddr | I2C_WRITE_BIT, TRUE) != OK)
        {
            printf("I2C_Read: Send slaveaddr(%d) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
        
        if (I2CSendData(RegAddr, FALSE) != OK)
        {
            printf("I2C_Read: Send cmd(%d) error!\n",RegAddr);
            ret = FALSE;
            goto error;
        }

        if(mode != RDA_5820Mode)
        {
            I2CStop(); // I2C_CONR = 0x1C , I2C_CONR = 0xC
        }

        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            printf("I2C_Read: Send read slaveaddr(%d) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }
    else if (mode == DirectMode) // FM5767
    {
        if (I2CSendData(I2C_slaveaddr | I2C_READ_BIT, TRUE) != OK)
        {
            printf("I2C_Read: Send read slaveaddr(%d) error!\n",I2C_slaveaddr);
            ret = FALSE;
            goto error;
        }
    }
    
    // 设置为主设备接收
    // MaskRegBits32(&pI2CReg->I2C_CONR, I2C_CONR_MASTER_MASK, I2C_CONR_MRX);
    I2cReg->I2C_CONR &= ~( I2C_CONR_MASTER_MASK);
    I2cReg->I2C_CONR |= I2C_CONR_MRX;
    while (size)
    {
        if (OK != I2CReadData(pData))
        {
            printf("I2C_Read: read data(%d) error!\n",*pData);
            ret = FALSE;
            goto error;
        }
        
        //  the last read (size = 1)is NACK , other is ACK
        if (1 == size)
            //SetRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR |= I2C_CON_NACK;
        else
            //ClrRegBits32(&pI2CReg->I2C_CONR, I2C_CON_NACK);
            I2cReg->I2C_CONR &= ~(I2C_CON_NACK);
        pData++;
        
        size --;
    }
    
error:
    
    I2CStop();
    
    return ret;
}

/*
--------------------------------------------------------------------------------
  Function name : I2C_Deinit(void)
  Author        : yangwenjie
  Description   : 
  History:     <author>         <time>         <version>       
             yangwenjie     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
BOOL I2C_Deinit(void)
{
    //IoMuxSetI2C(I2C_CE);

    I2cReg->I2C_OPR &= ~(I2C_OPR_ENABLE);
    I2cReg->I2C_OPR |= I2C_OPR_RESET;

    UserIsrEnable(0);

    GpioMuxSet(GPIOPortC_Pin7,IOMUX_GPIOC7_FLASH_CSN2);
    GpioMuxSet(GPIOPortD_Pin0,IOMUX_GPIOD0_FLASH_CSN3);

    ScuClockGateCtr(CLOCK_GATE_I2C, 0);
    
    return TRUE;
}

/*
********************************************************************************
*
*                         End of I2S.c
*
********************************************************************************
*/
