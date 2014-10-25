/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��  SysIntHandler.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    
********************************************************************************
*/
#define _IN_SYSTICK_HANDLER_

#include "SysInclude.h"

/*
--------------------------------------------------------------------------------
  Function name : void SysTickTimerRun(void)
  Author        : ZHengYongzhi
  Description   : Default SysTimer, the tick of system
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#ifdef MOUDLE_3LED
 _ATTR_SYS_CODE_
void ClockRun(void)
{
    CurrentTimer++;
    if(CurrentTimer == TIME_MAX) CurrentTimer = 0;
    make_timing();
    make_color();
}

 _ATTR_SYS_CODE_
uint32 GetClock(void)
{
    return CurrentTimer;
}
#endif
 
_ATTR_SYS_CODE_
void SysTickTimerRun(void)
{
    SysTickCounter++;
#ifdef MOUDLE_3LED
    ClockRun();
#endif
}

 _ATTR_SYS_CODE_
uint32 GetSysTick(void)
{
    return SysTickCounter;
}

_ATTR_SYS_DATA_
SYSTICK_LIST SysTickHead = 
{
    NULL,
    0,
    0,
    0,
    SysTickTimerRun,
};

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SystickTimerStart(SYSTICK_LIST *pSystick)
  Author        : ZHengYongzhi
  Description   : Register and Start a Timer
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM
_ATTR_DRIVERLIB_CODE_
UINT32 SystickTimerStartRaw(SYSTICK_LIST *pListHead, SYSTICK_LIST *pSystick)
{
    //SYSTICK_LIST *pListHead = &SysTickHead;
    
    //IntDisable(FAULT_ID15_SYSTICK);
    nvic->SysTick.Ctrl &= ~NVIC_SYSTICKCTRL_TICKINT;
    
    while(pListHead->pNext != NULL)
    {
        pListHead = pListHead->pNext;
    }
    
    pListHead->pNext = pSystick;
    pSystick->Counter = 0;
    pSystick->pNext = NULL;
    
    //IntEnable(FAULT_ID15_SYSTICK);
    nvic->SysTick.Ctrl |= NVIC_SYSTICKCTRL_TICKINT;
    
    return TRUE;
}
#endif

_ATTR_SYS_CODE_
UINT32 SystickTimerStart(SYSTICK_LIST *pSystick)
{
    return (SystickTimerStartRaw(&SysTickHead, pSystick));
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SystickTimerStop(SYSTICK_LIST *pSystick)
  Author        : ZHengYongzhi
  Description   : Stop a Timer
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM
_ATTR_DRIVERLIB_CODE_
UINT32 SystickTimerStopRaw(SYSTICK_LIST *pListHead, SYSTICK_LIST *pSystick)
{
    uint32 tmp;
    //SYSTICK_LIST *pListHead = &SysTickHead;
    
    //tmp = IsrDisable(FAULT_ID15_SYSTICK);
    tmp = (nvic->SysTick.Ctrl & NVIC_SYSTICKCTRL_TICKINT);
    nvic->SysTick.Ctrl &= ~NVIC_SYSTICKCTRL_TICKINT;
        
    while(pListHead->pNext != pSystick)
    {
        pListHead = pListHead->pNext;
    }

    pListHead->pNext = pSystick->pNext;

    //IsrEnable(FAULT_ID15_SYSTICK, tmp);
    if (tmp)
    {
        nvic->SysTick.Ctrl |= NVIC_SYSTICKCTRL_TICKINT;
    }
    
    return TRUE;
}
#endif

_ATTR_SYS_CODE_
UINT32 SystickTimerStop(SYSTICK_LIST *pSystick)
{
    return (SystickTimerStopRaw(&SysTickHead, pSystick));
}


/*
--------------------------------------------------------------------------------
  Function name : void SysTickHandler(void)
  Author        : ZHengYongzhi
  Description   : SysTick Timer Isr
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM
_ATTR_DRIVERLIB_CODE_
void SysTickHandlerRaw(SYSTICK_LIST *pListHead, pSysTickStopCallBack StopCallBack)
{    
    //SYSTICK_LIST *pListHead = &SysTickHead;
    
    (pListHead->pHandler)();
    
    pListHead = pListHead->pNext;
    while(pListHead != NULL)
    {
        if ((pListHead->pHandler) && (pListHead->Reload))
        {
            if (++pListHead->Counter >= pListHead->Reload)
            {
                (pListHead->pHandler)();
                pListHead->Counter = 0;
            
                if (pListHead->Times)
                {
                    if (--pListHead->Times == 0)
                    {
                        //SystickTimerStop(pListHead);
                        StopCallBack(pListHead);
                    }
                }
            }
        }
        pListHead = pListHead->pNext;
    }
}
#endif

_ATTR_SYS_CODE_
__irq void SysTickHandler(void)
{
    SysTickHandlerRaw(&SysTickHead, SystickTimerStop);
}

/*
********************************************************************************
*
*                         End of SysIntHandler.c
*
********************************************************************************
*/

