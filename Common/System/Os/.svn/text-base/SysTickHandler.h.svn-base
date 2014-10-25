/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   SystickHandler.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _SYSTICKHANDLER_H_
#define _SYSTICKHANDLER_H_

#undef  EXT
#ifdef _IN_SYSTICK_HANDLER_
#define EXT
#else
#define EXT extern
#endif

/*
*-------------------------------------------------------------------------------
*
*                            Struct Define
*
*-------------------------------------------------------------------------------
*/
typedef struct _SYSTICK_LIST {
    
    struct _SYSTICK_LIST  *pNext;

    uint32 Counter;
    uint32 Reload;
    uint32 Times;
    void   (*pHandler)(void);
    
}SYSTICK_LIST;

typedef UINT32 (*pSysTickStopCallBack)(SYSTICK_LIST *pSystick);

/*
*-------------------------------------------------------------------------------
*
*                            Function Declaration
*
*-------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM

extern UINT32 SystickTimerStartRaw(SYSTICK_LIST *pListHead, SYSTICK_LIST *pSystick);
extern UINT32 SystickTimerStopRaw(SYSTICK_LIST *pListHead, SYSTICK_LIST *pSystick);
extern void   SysTickHandlerRaw(SYSTICK_LIST *pListHead, pSysTickStopCallBack StopCallBack);

#else

typedef UINT32 (*pSystickTimerStartRaw)(SYSTICK_LIST *pListHead, SYSTICK_LIST *pSystick);
typedef UINT32 (*pSystickTimerStopRaw)(SYSTICK_LIST *pListHead, SYSTICK_LIST *pSystick);
typedef void   (*pSysTickHandlerRaw)(SYSTICK_LIST *pListHead, pSysTickStopCallBack StopCallBack);

#define SystickTimerStartRaw(pListHead, pSystick)  (((pSystickTimerStartRaw)(Addr_SystickTimerStartRaw))(pListHead, pSystick))
#define SystickTimerStopRaw(pListHead, pSystick)   (((pSystickTimerStopRaw )(Addr_SystickTimerStopRaw ))(pListHead, pSystick))
#define SysTickHandlerRaw(pListHead, StopCallBack) (((pSysTickHandlerRaw   )(Addr_SysTickHandlerRaw   ))(pListHead, StopCallBack))

#endif

extern uint32 GetSysTick(void);
extern UINT32 SystickTimerStart(SYSTICK_LIST *pSystick);
extern UINT32 SystickTimerStop(SYSTICK_LIST *pSystick);
extern __irq void SysTickHandler(void);

/*
********************************************************************************
*
*                         End of SystickHandler.h
*
********************************************************************************
*/
#endif




