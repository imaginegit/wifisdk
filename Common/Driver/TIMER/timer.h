/*
********************************************************************************
*                   Copyright (c) 2008,anzhiguo
*                         All rights reserved.
*
* File Name£º   timer.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             anzhiguo      2009-1-14          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#undef  EXT
#ifdef  _IN_TIMER_
#define EXT
#else
#define EXT extern
#endif


extern _ATTR_DRIVER_DATA_ uint32 TimerusTickBak;
extern _ATTR_DRIVER_DATA_ uint32 TimernsTickBak;

/*
--------------------------------------------------------------------------------
  
                        Funtion Declaration
  
--------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM

extern void   TimerEnable(void);
extern void   TimerDisable(void);
extern uint32 TimerPeriodGet(UINT32 PCLK);
extern UINT32 Timer_GetCount(void);
extern void   TimerIntEnable(void);
extern void   TimerIntDisable(void);
extern uint32 TimerClearIntFlag(void);
extern UINT32 TimerIntStateGet(void);

#else

typedef void   (*pTimerEnable)(void);
typedef void   (*pTimerDisable)(void);
typedef uint32 (*pTimerPeriodGet)(UINT32 PCLK);
typedef UINT32 (*pTimer_GetCount)(void);
typedef void   (*pTimerIntEnable)(void);
typedef void   (*pTimerIntDisable)(void);
typedef uint32 (*pTimerClearIntFlag)(void);
typedef UINT32 (*pTimerIntStateGet)(void);

#define TimerEnable()        (((pTimerEnable      )(Addr_TimerEnable      ))())
#define TimerDisable()       (((pTimerDisable     )(Addr_TimerDisable     ))())
#define TimerPeriodGet(PCLK) (((pTimerPeriodGet   )(Addr_TimerPeriodGet   ))(PCLK))
#define Timer_GetCount()     (((pTimer_GetCount   )(Addr_Timer_GetCount   ))())
#define TimerIntEnable()     (((pTimerIntEnable   )(Addr_TimerIntEnable   ))())
#define TimerIntDisable()    (((pTimerIntDisable  )(Addr_TimerIntDisable  ))())
#define TimerClearIntFlag()  (((pTimerClearIntFlag)(Addr_TimerClearIntFlag))())
#define TimerIntStateGet()   (((pTimerIntStateGet )(Addr_TimerIntStateGet ))())

#endif

BOOL TimerPeriodSet( UINT32 usTick, uint32 nsTick);
/*
********************************************************************************
*
*                         End of timer.h
*
********************************************************************************
*/
#endif
