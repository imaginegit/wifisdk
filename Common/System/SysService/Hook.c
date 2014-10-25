/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   Hook.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_HOOK_

#include "SysInclude.h"

/*
--------------------------------------------------------------------------------
  Function name : void UsbBusyHook(void)
  Author        : ZhengYongzhi
  Description   : USB busy status display function
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/

extern uint8 NandIOMuxRef;  //在MemDevInit 初始化此变量


/*
--------------------------------------------------------------------------------
  Function name : void UsbBusyHook(void)
  Author        : ZhengYongzhi
  Description   : USB State Detect
                  
  Input         : 
  Return        : 无

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
bool CheckVbus(void)
{
    return (Scu_Adapter_State_Get());
}

/*
--------------------------------------------------------------------------------
  Function name : void NandIOMux(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void NandIOMux(void)
{
    if (!NandIOMuxRef)
        DataPortMuxSet(IO_FLASH);
    NandIOMuxRef++;
}

/*
--------------------------------------------------------------------------------
  Function name : void NandIODeMux(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void NandIODeMux(void)
{
    if (NandIOMuxRef)
    {
        NandIOMuxRef--;
        if (!NandIOMuxRef)
            DataPortMuxSet(IO_LCD);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void NandSetClk(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void NandSetClk(void)
{
    
}


/*
********************************************************************************
*
*                         End of hook.c
*
********************************************************************************
*/

