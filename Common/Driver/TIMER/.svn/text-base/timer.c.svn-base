/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name£º   Timer.c
* 
* Description:  C program template
*
* History:      <author>          <time>        <version>       
*               anzhiguo        2009-1-14         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_TIMER_

#include "DriverInclude.h"

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : TimerEnable(void)
  Author        : anzhiguo
  Description   : enable timer
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void TimerEnable(void)
{
    Timer->ControlReg |= TIMR_EN ;      //enable timer
}
/*
--------------------------------------------------------------------------------
  Function name : TimerDisable(void)
  Author        : anzhiguo
  Description   : disable timer
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
                anzhiguo     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void TimerDisable(void)
{
    Timer->ControlReg &= (~TIMR_EN) ;      //disable timer
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : TimerPeriodSet()
  Author        : anzhiguo
  Description   : set timer timing time,
                  
  Input         : usTick -- timer timing£¬unit:us
                  PCLK   -- current apb bus frequency unit:MHz
                  
  Return        : 

  History:     <author>         <time>         <version>       
                anzhiguo     2009-1-14         Ver1.0
  desc:        usTick * PCLK <= 0x00ffffff
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_DATA_ uint32 TimerusTickBak = 0x00ffffff;
_ATTR_DRIVER_DATA_ uint32 TimernsTickBak = 0x00ffffff;
_ATTR_DRIVER_CODE_
BOOL TimerPeriodSet( UINT32 usTick, uint32 nsTick)
{
    UINT32 count,Pclk;
    BOOL   Retval = TRUE;
    
    TimerusTickBak = usTick;
    TimernsTickBak = nsTick;
    
    Pclk  = GetPclkFreq();
    count =  (Pclk * usTick + Pclk * nsTick / 1000)/1000;
    if(count > 0x00ffffff) 
	{
        count = 0x00ffffff;
        Retval = FALSE; 
	}

   /* Disable the timer ,usermode,mask*/
    TimerDisable();

    Timer->ControlReg |= TIMR_USER_MODE;
    
    Timer->LoadCountRegister = count;   

   /* free mode and enable the timer  £¬not mask*/
    TimerEnable();
   
    return Retval;
}

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : TimerPeriodSet()
  Author        : anzhiguo
  Description   : get timer timing time
                  
  Input         : PCLK   -- current APB bus frequency.unit is MHZ
  Return        :        -- timer timing time unit:us

  History:     <author>         <time>         <version>       
               anzhiguo     2009-1-14         Ver1.0
  desc:        mstick max value= 0x00ffffff/clk; 
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
uint32 TimerPeriodGet(UINT32 PCLK)
{
    return(Timer->LoadCountRegister / (PCLK));
}
/*
--------------------------------------------------------------------------------
  Function name : Timer_GetCount(void)
  Author        : anzhiguo
  Description   : get current timer count value.
                  
  Input         : null
                  null
  Return        : current timer count value.

  History:     <author>         <time>         <version>       
               anzhiguo     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 Timer_GetCount(void)
{
    UINT32 count;
    
    count = Timer->CurrentValue;        
    return count;
}

/*
--------------------------------------------------------------------------------
  Function name : TimerIntEnable
  Author        : anzhiguo
  Description   : timer interrupt enable
                  
  History:     <author>         <time>         <version>       
               anzhiguo     2009-1-14         Ver1.0
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void TimerIntEnable(void)
{
    Timer->ControlReg &= (~TIMR_IMASK ) ; 
}
/*
--------------------------------------------------------------------------------
  Function name : TimerIntDIsable
  Author        : anzhiguo
  Description   : timer disable
                  

  History:     <author>         <time>         <version>       
               anzhiguo     2009-1-14         Ver1.0
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void TimerIntDisable(void)
{
    Timer->ControlReg |= (TIMR_IMASK ) ; 
}

/*
--------------------------------------------------------------------------------
  Function name : Timer_ClearIntrFlag()
  Author        : anzhiguo
  Description   : clear timer interrupt flag
                  
  Input         : null
                  null
  Return        : 

  History:     <author>         <time>         <version>       
              anzhiguo     2009-1-14         Ver1.0
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
uint32 TimerClearIntFlag(void)
{
    UINT32 tmp;

    tmp = Timer->TimerEOI;

	return(tmp);
}

/*
--------------------------------------------------------------------------------
  Function name : TimerIntStateGet
  Author        : anzhiguo
  Description   : get timer interrupt status.
                  
  Return        : timerinterrupt status.

  History:     <author>         <time>         <version>       
               anzhiguo     2009-1-14         Ver1.0
  desc:       
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 TimerIntStateGet(void)
{
    UINT32 tmp;
    
    tmp = Timer->TimerIntStatus;
    
    return(tmp);
}
#endif
/*
********************************************************************************
*
*                         End of timer.c
*
********************************************************************************
*/

