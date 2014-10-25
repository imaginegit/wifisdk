/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: SDAdapt.c
Desc: 

Author: chenfen
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/

#include "SDConfig.h"

#if 1//(SDMMC_DRIVER==1)
#include "SysInclude.h"

/*------------------------------------ Defines -------------------------------*/


/*----------------------------------- Typedefs -------------------------------*/


/*-------------------------- Forward Declarations ----------------------------*/


/* ------------------------------- Globals ---------------------------------- */


/*-------------------------------- Local Statics: ----------------------------*/
SDOAM_EVENT_T gEvent[SDC_MAX];

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

/*
Name:       SDA_Delay
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
void SDA_Delay(uint32 us)
{
    DelayUs(us);
}


/*
Name:       SDA_CreateEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
pEVENT SDA_CreateEvent(uint8 nSDCPort)
{
    gEvent[nSDCPort].event = 0;
    return (&gEvent[nSDCPort].event);
}

/*
Name:       SDA_SetEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
void SDA_SetEvent(pEVENT handle, uint32 event)
{
    *handle |= event;
}

/*
Name:       SDOAM_GetEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
int32 SDOAM_GetEvent(pEVENT handle, uint32 event, uint32 timeout)
{
    if (NO_TIMEOOUT == timeout)
    {
        while (!(*((volatile uint32 *)handle) & event))
        {
             SDA_Delay(2);
        }
    }
    else if (0 == timeout) 
    {
        if (!(*((volatile uint32 *)handle) & event))
        {
            return SDC_NO_EVENT;
        }     
    }
    else
    {
        while ((!(*((volatile uint32 *)handle) & event)))
        {
            if (timeout == 0)
            {
                return SDC_NO_EVENT;
            }
            timeout--;
            SDA_Delay(1); 
        }
    }
    
    *handle &= ~(event);
    
    return SDC_SUCCESS;
}


/*
Name:       SDA_DMAStart
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
bool SDA_DMAStart(SDMMC_PORT_E nSDCPort, uint32 DstAddr, uint32 SrcAddr, uint32 size, bool rw, pFunc CallBack)
{
    return TRUE;
}

/*
Name:       SDA_DMAStop
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
bool SDA_DMAStop(SDMMC_PORT_E nSDCPort)
{
    return TRUE;
}

/*
Name:       SDA_GetSrcClkFreq
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
uint32 SDA_GetSrcClkFreq(void)
{
    return 24000;  //usbplus = 24MHz
}


/*
Name:       SDA_SetSrcClkDiv
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
int32 SDA_SetSrcClkDiv(SDMMC_PORT_E nSDCPort, uint32 div)
{
	return SDC_SUCCESS;
}

/*
Name:       SDA_SetSrcClk
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
void SDA_SetSrcClk(SDMMC_PORT_E nSDCPort, bool enable)
{
    CRUReg->CRU_CLKGATE_CON1 = ((TRUE == enable)? 0:1)<<2 | 1<<18; //clk_sdmmc_gate
}

/*
Name:       SDA_RegISR
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
bool SDA_RegISR(SDMMC_PORT_E nSDCPort, pFunc Routine)
{
    IntRegister(INT_ID18_SDMMC, (void*)Routine);
    return TRUE;

}

/*
Name:       SDA_EnableIRQ
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
void SDA_EnableIRQ(SDMMC_PORT_E nSDCPort)
{
	//EnableCPUInt();
    nvic->Irq.Enable[0] = 0x00000004;
}


/*
Name:       SDA_SetIOMux
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
bool SDA_SetIOMux(SDMMC_PORT_E SDCPort, HOST_BUS_WIDTH width)
{
    return TRUE;
}

/*
Name:       SDA_SetIOMux
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_DRIVERLIB_CODE_
bool SDA_CheckCard(SDMMC_PORT_E         SDCPortd)
{
    #if (CONFIG_EMMC_SPEC==1)
    return TRUE;
    #else
    //return (0 == Gpio_GetPinLevel(SD_DET))? TRUE:FALSE;
    return FALSE;
    #endif
}

/*
Name:       SDA_Init
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
void SDA_Init(void)
{
    //=========================================================
    //WIFI Power Initial: PowerDown, SDMMC Low Level
    //WIFI Power Down
    Gpio_SetPinDirection(WIFI_POWER, GPIO_OUT);
    Gpio_SetPinLevel(WIFI_POWER, GPIO_HIGH);
    DelayMs(500);
    
    //WIFI RegOn: hold low level after powerup
    GpioMuxSet(WIFI_REG_ON,IOMUX_GPIOD2_IO);
    Gpio_SetPinDirection(WIFI_REG_ON,GPIO_OUT);
    Gpio_SetPinLevel(WIFI_REG_ON,GPIO_HIGH);
    DelayMs(200);
    
    //WIFI Host wakeup: hold low level after powerup
    GpioMuxSet(WIFI_HOST_WAKE,IOMUX_GPIOC7_IO);
    Gpio_SetPinDirection(WIFI_HOST_WAKE,GPIO_IN);

    //SDMMC
    #if (CONFIG_EMMC_SPEC==1)
	GpioMuxSet(GPIOPortA_Pin0,IOMUX_GPIOA0_SDMMC_PWREN);
	GpioMuxSet(GPIOPortA_Pin3,IOMUX_GPIOA3_SDMMC_CCLKP1);
    GpioMuxSet(GPIOPortA_Pin4,IOMUX_GPIOA4_SDMMC_RSTNP1);
    GpioMuxSet(GPIOPortA_Pin6,IOMUX_GPIOA6_SDMMC_CMDP1);
    DataPortMuxSet(IO_SDMMC);
	SDMMCPortMuxSet(SDMMC_PORT1);
    #else
	GpioMuxSet(SDMMC_PORT0_CMD,IOMUX_GPIOC1_SDMMC_CMDP0);
    GpioMuxSet(SDMMC_PORT0_CLK,IOMUX_GPIOC2_SDMMC_CLKP0);
    GpioMuxSet(SDMMC_PORT0_DATA0,IOMUX_GPIOC3_SDMMC_DATA0P0);
    GpioMuxSet(SDMMC_PORT0_DATA1,IOMUX_GPIOC4_SDMMC_DATA1P0);
    GpioMuxSet(SDMMC_PORT0_DATA2,IOMUX_GPIOC5_SDMMC_DATA2P0);
    GpioMuxSet(SDMMC_PORT0_DATA3,IOMUX_GPIOC6_SDMMC_DATA3P0);
	SDMMCPortMuxSet(SDMMC_PORT0);
    #endif

    SetSdmmcClkSel(MMC_CLK_SEL_24M);
    SetSdmmcFreq(24);
}

/*
Name:       SDA_DeInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
void SDA_DeInit(void)
{
    //=========================================================
    //WIFI Power Initial: PowerDown, SDMMC Low Level
    //SDMMC
    GpioMuxSet(SDMMC_PORT0_CMD,IOMUX_GPIOC1_IO);
    GpioMuxSet(SDMMC_PORT0_CLK,IOMUX_GPIOC2_IO);
    GpioMuxSet(SDMMC_PORT0_DATA0,IOMUX_GPIOC3_IO);
    GpioMuxSet(SDMMC_PORT0_DATA1,IOMUX_GPIOC4_IO);
    GpioMuxSet(SDMMC_PORT0_DATA2,IOMUX_GPIOC5_IO);
    GpioMuxSet(SDMMC_PORT0_DATA3,IOMUX_GPIOC6_IO);

    Gpio_SetPinDirection(SDMMC_PORT0_CMD,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_CLK,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_DATA0,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_DATA1,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_DATA2,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_DATA3,GPIO_OUT);

    Gpio_SetPinLevel(SDMMC_PORT0_CMD,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_CLK,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_DATA0,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_DATA1,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_DATA2,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_DATA3,GPIO_LOW); 

    //WIFI Host wakeup: hold low level after powerup
    GpioMuxSet(WIFI_HOST_WAKE,IOMUX_GPIOC7_IO);
    Gpio_SetPinDirection(WIFI_HOST_WAKE,GPIO_OUT);
    Gpio_SetPinLevel(WIFI_HOST_WAKE,GPIO_LOW);

    //WIFI RegOn: hold low level after powerup
    GpioMuxSet(WIFI_REG_ON,IOMUX_GPIOD2_IO);
    Gpio_SetPinDirection(WIFI_REG_ON,GPIO_OUT);
    Gpio_SetPinLevel(WIFI_REG_ON,GPIO_LOW);

    //WIFI Power Down
    Gpio_SetPinDirection(WIFI_POWER, GPIO_OUT);
    Gpio_SetPinLevel(WIFI_POWER, GPIO_LOW);
    
}

#endif
