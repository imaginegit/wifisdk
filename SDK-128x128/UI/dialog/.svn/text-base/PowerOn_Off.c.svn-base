/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name£º   PowerOn_Off.c
* 
* Description:   C program template
*
* History:      <author>          <time>        <version>       
*                anzhiguo      2009-2-27         1.0
*    desc:    ORG.
********************************************************************************
*/

#define _IN_POWER_ON_OFF

#include "SysInclude.h"
#include "FsInclude.h"


/*
--------------------------------------------------------------------------------
  Function name : void DisplayPowerOnFlash(void)
  Author        : ZHengYongzhi
  Description   : display power on picture.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
uint8 DisplayPowerOnFlash(void)
{
    uint32 i, j;
	uint32 block;
	uint8  RetVal;
    
    RetVal = OK;
    block = 0;
    
/*    for(i = 0; i < 24; i++)
    {
        DispPictureWithIDNum(IMG_ID_POWERON0 + i);
        DelayMs(50);
    }*/
    
    return(RetVal);
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOff(void)
  Author        : anzhiguo
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             anzhiguo        2009-2-27         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void PowerOff(void)
{
    uint32 i;    
    
    printf("system powerdown!\n");

//    MP4_LCD_DeInit();
            
/*    for(i = 0; i < 12; i++)
    {
        DispPictureWithIDNum(IMG_ID_POWEROFF0 + i);
        DelayMs(100);
    }*/
    
//    LcdStandby();
    
    Codec_DeInitial();

    Scu_OOL_Power_Set(0);
    while(1);
}

/*
********************************************************************************
*
*                         End of PowerOn_Off.c
*
********************************************************************************
*/
