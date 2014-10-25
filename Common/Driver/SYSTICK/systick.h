/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   systick.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#undef  EXT
#ifdef _IN_SYSTICK_
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

/*
*-------------------------------------------------------------------------------
*
*                            Function Declaration
*
*-------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM

extern void   SysTickClkSourceSet(UINT32 Source);
extern BOOL   SysTickClkSourceGet(void);
extern void   SysTickEnable(void);
extern void   SysTickDisable(void);
extern UINT32 SysTickPeriodGet(void);
extern UINT32 SysTickValueGet(void);
extern BOOL   SysTickRefClockCheck(void);
extern BOOL   SysTickCalibrationCheck(void);
extern UINT32 SysTickCalibrationTenMsGet(void);

#else

typedef void   (*pSysTickClkSourceSet)(UINT32 Source);
typedef BOOL   (*pSysTickClkSourceGet)(void);
typedef void   (*pSysTickEnable)(void);
typedef void   (*pSysTickDisable)(void);
typedef UINT32 (*pSysTickPeriodGet)(void);
typedef UINT32 (*pSysTickValueGet)(void);
typedef BOOL   (*pSysTickRefClockCheck)(void);
typedef BOOL   (*pSysTickCalibrationCheck)(void);
typedef UINT32 (*pSysTickCalibrationTenMsGet)(void);

#define SysTickClkSourceSet(Source)  (((pSysTickClkSourceSet   )(Addr_SysTickClkSourceSet       ))(Source))
#define SysTickClkSourceGet()        (((pSysTickClkSourceGet   )(Addr_SysTickClkSourceGet       ))())
#define SysTickEnable()              (((pSysTickEnable         )(Addr_SysTickEnable             ))())
#define SysTickDisable()             (((pSysTickDisable        )(Addr_SysTickDisable            ))())
#define SysTickPeriodGet()           (((pSysTickPeriodGet      )(Addr_SysTickPeriodGet          ))())
#define SysTickValueGet()            (((pSysTickValueGet       )(Addr_SysTickValueGet           ))())
#define SysTickRefClockCheck()       (((pSysTickRefClockCheck  )(Addr_SysTickRefClockCheck      ))())
#define SysTickCalibrationCheck()    (((pSysTickCalibrationChec)(Addr_SysTickCalibrationCheck   ))())
#define SysTickCalibrationTenMsGet() (((pSysTickCalibrationTenM)(Addr_SysTickCalibrationTenMsGet))())

#endif

extern void SysTickPeriodSet(UINT32 mstick );

/*
********************************************************************************
*
*                         End of systick.h
*
********************************************************************************
*/
#endif

