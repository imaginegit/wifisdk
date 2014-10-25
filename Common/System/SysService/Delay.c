/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   Delay.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_DELAY_

#include "SysInclude.h"

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : void Delay100cyc(UINT16 count)
  Author        : ZHengYongzhi
  Description   : delay 10 cycle.
                  one while period has x time of cycles.
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void Delay10cyc(UINT32 count)
{
    uint32 cycle;
    
    cycle = count * 2;
    
    while(--cycle);
}

/*
--------------------------------------------------------------------------------
  Function name : void Delay100cyc(UINT16 count)
  Author        : ZHengYongzhi
  Description   : delay 100 time cycle.
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void Delay100cyc(UINT32 count)
{
    uint32 cycle;
    
    cycle = count * 20;
    
    while(--cycle);
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : void DelayMs_nops(UINT32 msec)
  Author        : yangwenjie
  Description   : how many us that software will to delay 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
__asm void ASMDelay(uint32 i)
{
ASMDelayxx
	SUBS    R0, R0, #1
	BHI     ASMDelayxx
	BX      LR
}
_ATTR_SYS_CODE_
void DelayUs(UINT32 us)
{
    uint32 SysFreq;
    volatile uint32 i;

    SysFreq = chip_freq.armclk;
    i = (SysFreq*us)/5;

	if(i)
        ASMDelay(i);
}

/*
--------------------------------------------------------------------------------
  Function name : void DelayMs(UINT32 mSec)
  Author        : yangwenjie
  Description   : how many ms that software will to delay 
                  
  Input         : 
  Return        : ��

  History:     <author>         <time>         <version>       
             yangwenjie       2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void DelayMs(UINT32 ms)
{
    uint32 SysFreq;

    SysFreq = GetPllSysFreq();

    ms *= SysFreq * 1000 / 3;

    while (--ms);
}

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : GetTimeHMS(UINT32 TempSec, UINT16 *pHour, UINT16 *pMin, UINT16 *pSec)
  Author        : ZhengYongzhi
  Description   : get the hour,minute,second according to input second.
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void GetTimeHMS(uint32 TempSec, uint16 *pHour, uint8 *pMin, uint8 *pSec)
{
    *pHour = (uint16)(TempSec / 3600);
    *pMin  = (uint8)((TempSec % 3600) / 60);
    *pSec  = (uint8)(TempSec % 60);
}
#endif
/*
********************************************************************************
*
*                         End of Delay.c
*
********************************************************************************
*/

