/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  OsHook.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    
********************************************************************************
*/
#define _IN_OSHOOK_

//#include "OsInclude.h"
#include "SysInclude.h"

/*
--------------------------------------------------------------------------------
  Function name : UINT32 WinPaintCheckPaintAllHook(void)
  Author        : ZHengYongzhi
  Description   : check whethe is full screen refresh.
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
UINT32 WinPaintCheckPaintAllHook(void)
{
#ifdef LCD_MEMORY_DEV
#else
    return (CheckMsg(MSG_NEED_PAINT_ALL));
#endif
}

/*
--------------------------------------------------------------------------------
  Function name : void WinPaintDmaFillImage2Lcd(void)
  Author        : ZHengYongzhi
  Description   : 
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void WinPaintDmaFillImage2Lcd(void)
{
#ifdef LCD_MEMORY_DEV

#endif
}

/*
--------------------------------------------------------------------------------
  Function name : void WinPaintBackLightOnHook(void)
  Author        : ZHengYongzhi
  Description   : open backlight
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void WinPaintBackLightOnHook(void)
{
    ClearMsg(MSG_NEED_PAINT_ALL);
}

/*
--------------------------------------------------------------------------------
  Function name : void WinPaintBackLightOnHook(void)
  Author        : ZHengYongzhi
  Description   : open backlight
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
uint32 WinPaintBLStatusCheckHook(void)
{
    return 1;    
}

/*
--------------------------------------------------------------------------------
  Function name : void vbus_wakeup_int(void)
  Author        : ZHengYongzhi
  Description   : Vbus interrupt wake
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
__irq void vbus_wakeup_int(void)
{
    IntDisable(INT_ID40_PWR_5V_READY);
}

/*
--------------------------------------------------------------------------------
  Function name : void playon_wakeup_int(void)
  Author        : ZHengYongzhi
  Description   : Play key interrupt wake
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
__irq void playon_wakeup_int(void)
{
    IntDisable(INT_ID39_OOL_PLAYON);
}

/*
--------------------------------------------------------------------------------
  Function name : void Os_idle(void)
  Author        : ZHengYongzhi
  Description   : system enter idle
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void Os_idle(void)
{
    uint32 LDOBack;
    uint32 SysClkBack;
    
    //if (pMainThread != NULL) return;
    //if (1 == CheckVbus())    return;
    //zyz: 当系统允许自动关机时，一般就允许进入待机模式
    if (AutoPowerOffDisableCounter != 0)
    {
        return;
    }
    
    DEBUG("Enter");
    DelayMs(10);
    
    //Enter Idle
    SysTickDisable();
    AdcPowerDown();
    ScuClockGateCtr(CLOCK_GATE_ADC, 0);
    ScuClockGateCtr(CLOCK_GATE_DMA, 0);
    ScuClockGateCtr(CLOCK_GATE_LCDC, 0);

    ScuClockGateCtr(CLOCK_GATE_BCH, 0);
    ScuClockGateCtr(CLOCK_GATE_NANDC, 0);
    ScuClockGateCtr(CLOCK_GATE_SDMMC, 0);
    ScuClockGateCtr(CLOCK_GATE_UART, 0);
    ScuClockGateCtr(CLOCK_GATE_TIMER, 0);

    //wakeup intrrupt        
    IntPendingClear(INT_ID39_OOL_PLAYON);
    IntPendingClear(INT_ID40_PWR_5V_READY);
    IntEnable(INT_ID39_OOL_PLAYON);
    IntEnable(INT_ID40_PWR_5V_READY);
    
    SysClkBack = chip_freq.armclk;
    SysFreqSet(0);
    //LDOBack = ScuLDOSet(SCU_LDO_27);      //LDO 降低会影响到eMMC
    if (Grf->MPMU_HP_CON & MPMU_HP_POWER_DOWM)
    {
        ScuDCoutSet(SCU_DCOUT_080);
    }
    
    __WFI();

    //wakeup Idle
    //ScuLDOSet(LDOBack);                   //LDO 降低会影响到eMMC
    ScuDCoutSet(SCU_DCOUT_120);
    SysFreqSet(SysClkBack);
    PMU_EnterModule(PMU_BLON);   
    
    ScuClockGateCtr(CLOCK_GATE_UART, 1);
    #ifdef _NANDFLASH_
    ScuClockGateCtr(CLOCK_GATE_BCH, 1);
    ScuClockGateCtr(CLOCK_GATE_NANDC, 1);
    #endif

    ScuClockGateCtr(CLOCK_GATE_LCDC, 1);
    ScuClockGateCtr(CLOCK_GATE_ADC, 1);
    ScuClockGateCtr(CLOCK_GATE_DMA, 1);
    ScuClockGateCtr(CLOCK_GATE_TIMER, 1);
    AdcPowerUp();
    SysTickEnable();

    DEBUG("Exit");    
}

/*
********************************************************************************
*
*                         End of OsHook.c
*
********************************************************************************
*/

