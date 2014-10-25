/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   SysService.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _SYSSERVICE_H_
#define _SYSSERVICE_H_

#undef  EXT
#ifdef _IN_SYSSERVICE_
#define EXT
#else
#define EXT extern
#endif

_ATTR_SYS_BSS_ EXT  UINT8  SetPowerOffFlag;
_ATTR_SYS_BSS_ EXT  INT8   AutoPowerOffDisableCounter;

/*-----------------------------------------------------------------------------*/
//Service.c
extern void SysServiceInit(void);
extern UINT32 SysService(void);

extern void AutioPowerOffTimerRest(void);
extern void AutoPowerOffEnable(void);
extern void AutoPowerOffDisable(void);
extern void SetPowerOffTimerEnable(void);
extern void SetPowerOffTimerDisable(void);
/*
********************************************************************************
*
*                         End of SysService.h
*
********************************************************************************
*/
#endif
