/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	WiFiAPWin.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _IN_WIFIAPWIN_H_
#define _IN_WIFIAPWIN_H_


#undef  EXT
#ifdef _IN_WIFIAPWIN_
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
#define _ATTR_WIFI_AP_WIN_CODE_		__attribute__((section("WiFiAPWinCode")))
#define _ATTR_WIFI_AP_WIN_DATA_		__attribute__((section("WiFiAPWinData")))
#define _ATTR_WIFI_AP_WIN_BSS_		__attribute__((section("WiFiAPWinBss"),zero_init))

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
EXT void   WiFiAPWinInit(void *pArg);
EXT void   WiFiAPWinDeInit(void);

EXT UINT32 WiFiAPWinService(void);
EXT UINT32 WiFiAPWinKey(void);
EXT void   WiFiAPWinDisplay(void);

/*
--------------------------------------------------------------------------------
  
  Description:  window sturcture definition
  
--------------------------------------------------------------------------------
*/
#ifdef _IN_WIFIAPWIN_
_ATTR_WIFI_AP_WIN_DATA_ EXT WIN WiFiAPWin = {
    
    NULL,
    NULL,
    
    WiFiAPWinService,               //window service handle function.
    WiFiAPWinKey,                   //window key service handle function.
    WiFiAPWinDisplay,               //window display service handle function.
    
    WiFiAPWinInit,                  //window initial handle function.
    WiFiAPWinDeInit                 //window auti-initial handle function.
    
};
#else 
_ATTR_WIFI_AP_WIN_DATA_ EXT WIN WiFiAPWin;
#endif

/*
********************************************************************************
*
*                         End of Win.h
*
********************************************************************************
*/
#endif
