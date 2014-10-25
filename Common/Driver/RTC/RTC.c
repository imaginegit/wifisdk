/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name£º   RTC.c
* 
* Description:  CÔ´³ÌÐòÄ£°å
*
* History:      <author>          <time>        <version>       
*             yangwenjie      2008-11-20         1.0
*    desc:    ORG.
********************************************************************************
*/

#include "DriverInclude.h"

/*
--------------------------------------------------------------------------------
  Function name : void RTC_Enable(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

void RTC_Enable(void)
{
    //RTCReg->RTC_CTRL = ((UINT32)0x00000001 << 0);
    RTCReg->RTC_CTRL |= (0x1000 << 4);
}

/*
--------------------------------------------------------------------------------
  Function name : void RTC_Disable(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

void RTC_Disable(void)
{
    //RTCReg->RTC_CTRL |= ((UINT32)0x00000000 << 0);
}
/*
--------------------------------------------------------------------------------
  Function name : void RTC_Start(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

void RTC_Start(void)
{
    UINT32 cmd;
    cmd = RTCReg->RTC_CTRL;
    RTCReg->RTC_CTRL |= ((UINT32)0x00000001 << 2);
 //   RTCReg->RTC_CTRL = cmd;
}
/*
--------------------------------------------------------------------------------
  Function name : void RTC_Init(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

void RTC_Init(void)
{
    RTC_Enable();    
}
/*
--------------------------------------------------------------------------------
  Function name : void RTC_DeInit(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

void RTC_DeInit(void)
{
    
}
/*
--------------------------------------------------------------------------------
  Function name : void RTC_SetDate(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

static void RTC_SetDate(UINT32 year,UINT32 mon,UINT32 day)
{
    UINT32 year1,year2;
    UINT32 mon1,mon2;
    UINT32 day1,day2;
    UINT32 cmd;

    if(year > 99)
        year = 99;
    if(mon > 12)
        mon = 12;
    if(day > 31)
        day = 31;

    year1 = year / 10;
    year2 = year % 10;

    mon1 = mon / 10;
    mon2 = mon % 10;

    day1 = day / 10;
    day2 = day % 10;

    cmd = ((year1 << 15)|(year2 << 11)|(mon1 << 10)|(mon2 << 6)|(day1 << 4)|(day2 <<0));
    RTCReg->RTC_DATE_SET = cmd;
}

/*
--------------------------------------------------------------------------------
  Function name : void RTC_SetTime(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

static void RTC_SetTime(UINT32 hour,UINT32 min,UINT32 sec)
{
    UINT32 hour1,hour2;
    UINT32 min1,min2;
    UINT32 sec1,sec2;
    UINT32 cmd;
     
    if(hour > 24)
        hour = 24;
    if(min > 59)
        min = 59;
    if(sec > 59)
        sec = 59;

    hour1 = hour / 10;
    hour2 = hour % 10;

    min1 = min / 10;
    min2 = min % 10;

    sec1 = sec / 10;
    sec2 = sec % 10;

    cmd = ((hour1 << 18)|(hour2 << 14)|(min1 << 11)|(min2 << 7)|(sec1 << 4)|(sec2 <<0));
    RTCReg->RTC_TIME_SET = cmd;
}
/*
--------------------------------------------------------------------------------
  Function name : void RTC_SetRTC(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

void RTC_SetRTC(UINT32 year,UINT32 mon,UINT32 day,UINT32 hour,UINT32 min,UINT32 sec)
{
    RTC_Init();
    RTC_SetDate(year, mon, day);
    RTC_SetTime(hour,min,sec);
    RTC_Start();
}
/*
--------------------------------------------------------------------------------
  Function name : void RTC_GetDate(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

void RTC_GetDate(UINT32 *year,UINT32 *mon,UINT32 *day)
{
    UINT32 year1,year2;
    UINT32 mon1,mon2;
    UINT32 day1,day2;

    year1 = ((RTCReg->RTC_DATE >> 15) & 0xF);
    year2 = ((RTCReg->RTC_DATE >> 11) & 0xF);
    mon1 =  ((RTCReg->RTC_DATE >> 10) & 0x1);
    mon2 =  ((RTCReg->RTC_DATE >> 6) & 0xF);
    day1 =  ((RTCReg->RTC_DATE >> 4) & 0x3);
    day2 =  ((RTCReg->RTC_DATE >> 0) & 0xF);

    
    *year = (year1*10 + year2);
    *mon  = (mon1*10 + mon2);
    *day  = (day1*10 + day2);

    
}

/*
--------------------------------------------------------------------------------
  Function name : void RTC_GetTime(void)
  Author        : zyz
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                     zyz        2009-3-24         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

void RTC_GetTime(UINT32 *hour,UINT32 *min,UINT32 *sec)
{
    UINT32 hour1,hour2;
    UINT32 min1,min2;
    UINT32 sec1,sec2;
    
    hour1 = ((RTCReg->RTC_TIME >> 18) & 0x3);
    hour2 = ((RTCReg->RTC_TIME >> 14) & 0xF);
    min1 =  ((RTCReg->RTC_TIME >> 11) & 0x7);
    min2 =  ((RTCReg->RTC_TIME >> 7) & 0xF);
    sec1 =  ((RTCReg->RTC_TIME >> 4) & 0x7);
    sec2 =  ((RTCReg->RTC_TIME >> 0) & 0xF);

    *hour = (hour1*10 + hour2);
    *min  = (min1*10 + min2);
    *sec  = (sec1*10 + sec2);

}
void RTC_GetRTC(UINT32 *year,UINT32 *mon,UINT32 *day,UINT32 *hour,UINT32 *min,UINT32 *sec)
{
    RTC_GetDate(year, mon, day);
    RTC_GetTime(hour,min,sec);
}
