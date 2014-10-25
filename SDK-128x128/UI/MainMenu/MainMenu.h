/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name:  MainMenu.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _MAINMENU_H_
#define _MAINMENU_H_

#undef  EXT
#ifdef _IN_MAINMENU_
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
//section define
//main menu permanent code
#define _ATTR_MAIN_MENU_CODE_         __attribute__((section("MainMenuCode")))
#define _ATTR_MAIN_MENU_DATA_         __attribute__((section("MainMenuData")))
#define _ATTR_MAIN_MENU_BSS_          __attribute__((section("MainMenuBss"),zero_init))

/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/
typedef enum 
{
    #ifdef _WIFI_
    MAINMENU_ID_NET_RADIO,      //net radio
    #endif
    
    MAINMENU_ID_MAXNUM
    
} MAINMENU_ID;

/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/
#define MainMenuBatteryLevel       gBattery.Batt_Level
#define MainmenuHoldState          HoldState

_ATTR_MAIN_MENU_BSS_ EXT UINT16    MenuId; //moduel id of main menu interface

/*
--------------------------------------------------------------------------------
  
   Functon Declaration 
  
--------------------------------------------------------------------------------
*/
extern void MainMenuInit(void *pArg);
extern void MainMenuDeInit(void);

extern UINT32 MainMenuService(void);
extern UINT32 MainMenuKey(void);
extern void MainMenuDisplay(void);

INT16 MainMenuModeKey(void);


/*
--------------------------------------------------------------------------------
  
  Description:  window sturcture definition
  
--------------------------------------------------------------------------------
*/
#ifdef _IN_MAINMENU_
_ATTR_MAIN_MENU_DATA_ WIN MainMenuWin = {
    
    NULL,
    NULL,
    
    MainMenuService,               //window service handle function.
    MainMenuKey,                   //window key service handle function.
    MainMenuDisplay,               //window display service handle function.
    
    MainMenuInit,                  //window initial handle function.
    MainMenuDeInit                 //window auti-initial handle function.
    
};
#else 
_ATTR_MAIN_MENU_DATA_ EXT WIN MainMenuWin;
#endif

/*
********************************************************************************
*
*                         End of Win.h
*
********************************************************************************
*/
#endif

