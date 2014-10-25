/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   OsHook.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _OSHOOK_H_
#define _OSHOOK_H_

#undef  EXT
#ifdef _IN_OSHOOK_
#define EXT
#else
#define EXT extern
#endif

/*
--------------------------------------------------------------------------------
  
                        Macro define  
  
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
  
                        Struct Define
  
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
  
                        Variable Define
  
--------------------------------------------------------------------------------
*/


/*
--------------------------------------------------------------------------------
  
                        Funtion Declaration
  
--------------------------------------------------------------------------------
*/
extern UINT32 WinPaintCheckPaintAllHook(void);
extern void WinPaintDmaFillImage2Lcd(void);
extern void WinPaintBackLightOnHook(void);
extern uint32 WinPaintBLStatusCheckHook(void);
extern __irq void vbus_wakeup_int(void);
extern __irq void playon_wakeup_int(void);
extern void Os_idle(void);

/*
********************************************************************************
*
*                         End of OsHook.h
*
********************************************************************************
*/
#endif
