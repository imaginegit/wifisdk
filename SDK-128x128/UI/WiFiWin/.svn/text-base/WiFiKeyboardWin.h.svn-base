/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	WiFiKeyboardWin.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/
#ifndef _IN_WIFIKEYBOARDWIN_H_
#define _IN_WIFIKEYBOARDWIN_H_


#undef  EXT
#ifdef _IN_WIFIKEYBOARDWIN_
#define EXT
#else
#define EXT extern
#endif

/*
*-------------------------------------------------------------------------------
*  
*                           Macro define
*  
*-------------------------------------------------------------------------------
*/
#define _ATTR_KEYBOARD_WIN_CODE_	__attribute__((section("KeyboardWinCode")))
#define _ATTR_KEYBOARD_WIN_DATA_	__attribute__((section("KeyboardWinData")))
#define _ATTR_KEYBOARD_WIN_BSS_		__attribute__((section("KeyboardWinBss"),zero_init))

/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/


/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
  
   Functon Declaration 
  
--------------------------------------------------------------------------------
*/
EXT void   WiFiKeyBoardWinInit(void *pArg);
EXT void   WiFiKeyBoardWinDeInit(void);

EXT UINT32 WiFiKeyBoardWinService(void);
EXT UINT32 WiFiKeyBoardWinKey(void);
EXT void   WiFiKeyBoardWinDisplay(void);

/*
--------------------------------------------------------------------------------
  
  Description:  window sturcture definition
  
--------------------------------------------------------------------------------
*/
#ifdef _IN_WIFIKEYBOARDWIN_
_ATTR_KEYBOARD_WIN_DATA_ WIN WiFiKeyBoardWin = {
    
    NULL,
    NULL,
    
    WiFiKeyBoardWinService,               //window service handle function.
    WiFiKeyBoardWinKey,                   //window key service handle function.
    WiFiKeyBoardWinDisplay,               //window display service handle function.
    
    WiFiKeyBoardWinInit,                  //window initial handle function.
    WiFiKeyBoardWinDeInit                 //window auti-initial handle function.
    
};
#else 
_ATTR_KEYBOARD_WIN_DATA_ EXT WIN WiFiKeyBoardWin;
#endif

/*
********************************************************************************
*
*                         End of Win.h
*
********************************************************************************
*/
#endif

