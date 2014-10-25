/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	WiFiPlayWin.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/
#ifndef _IN_WIFIPLAYWIN_H_
#define _IN_WIFIPLAYWIN_H_


#undef  EXT
#ifdef _IN_WIFIPLAYWIN_
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
#define _ATTR_WIFI_PLAY_WIN_CODE_		__attribute__((section("WiFiPlayWinCode")))
#define _ATTR_WIFI_PLAY_WIN_DATA_		__attribute__((section("WiFiPlayWinData")))
#define _ATTR_WIFI_PLAY_WIN_BSS_		__attribute__((section("WiFiPlayWinBss"),zero_init))


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
EXT void   WiFiPlayWinInit(void *pArg);
EXT void   WiFiPlayWinDeInit(void);

EXT UINT32 WiFiPlayWinService(void);
EXT UINT32 WiFiPlayWinKey(void);
EXT void   WiFiPlayWinDisplay(void);

/*
--------------------------------------------------------------------------------
  
  Description:  window sturcture definition
  
--------------------------------------------------------------------------------
*/
#ifdef _IN_WIFIPLAYWIN_
_ATTR_WIFI_PLAY_WIN_DATA_ EXT WIN WiFiPlayWin = {
    
    NULL,
    NULL,
    
    WiFiPlayWinService,               //window service handle function.
    WiFiPlayWinKey,                   //window key service handle function.
    WiFiPlayWinDisplay,               //window display service handle function.
    
    WiFiPlayWinInit,                  //window initial handle function.
    WiFiPlayWinDeInit                 //window auti-initial handle function.
    
};
#else 
_ATTR_WIFI_PLAY_WIN_DATA_ EXT WIN WiFiPlayWin;
#endif

/*
********************************************************************************
*
*                         End of WiFiPlayWin.h
*
********************************************************************************
*/
#endif

