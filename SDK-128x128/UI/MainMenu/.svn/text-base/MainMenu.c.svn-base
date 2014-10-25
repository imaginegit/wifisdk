/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name:  MainMenu.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_MAINMENU_

#include "SysInclude.h"
#include "FsInclude.h"
#include "MainMenu.h"
#include "Hold.h"

_ATTR_SYS_DATA_ UINT8 MainIdFlag = 0;
/*
--------------------------------------------------------------------------------
  Function name : void MainMenuIntInit(void)
  Author        : ZHengYongzhi
  Description   : main menu interrupt initialization
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuIntInit(void)
{
    //MainMenu  interrupt initialization
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuIntDeInit(void)
  Author        : ZHengYongzhi
  Description   : main menu interrupt auti-initial
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuIntDeInit(void)
{
    //MainMenu
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuMsgInit(void)
  Author        : ZHengYongzhi
  Description   : main menu message initial function
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuMsgInit(void)
{
    SendMsg(MSG_MENU_DISPLAY_ALL);
    ClearMsg(MSG_WIFI_APPW_JUMP);
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuMsgDeInit(void)
  Author        : ZHengYongzhi
  Description   : main menu message auti-initialization
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuMsgDeInit(void)
{
 	// ClearMsg(MSG_MENU_DISPLAY_BACK);
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuInit(void)
  Author        : ZHengYongzhi
  Description   : main menu initialization
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuInit(void *pArg)
{
    MenuId = ((MAINMENU_WIN_ARG*)pArg)->MenuID;
    //service initial
    MainMenuMsgInit();      //message initial
    MainMenuIntInit();      //interrupt initial
    KeyReset();             //key initial
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuDeInit(void)
  Author        : ZHengYongzhi
  Description   :
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuDeInit(void)
{
    MainMenuIntDeInit();   
    DEBUG("MainMenu Exit");
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 MainMenuService(void)
  Author        : ZHengYongzhi
  Description   :
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
UINT32 MainMenuService(void)
{
    return(RETURN_OK);
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 MainMenuKey(void)
  Author        : ZHengYongzhi
  Description   : main menu key handle function.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
UINT32 MainMenuKey(void)
{
    uint32 RetVal;
    UINT32 MainMenuKeyVal;
    WIN    *pWin;
	TASK_ARG TaskArg;
    
    RetVal = RETURN_OK;
    
    MainMenuKeyVal =  GetKeyVal();
    
    //printf("key = 0x%x  \n", MainMenuKeyVal);

    //5 key function modification
    if (gSysConfig.KeyNum == KEY_NUM_5)
    {
        switch (MainMenuKeyVal)
        {
        case KEY_VAL_FFD_SHORT_UP:
            MainMenuKeyVal = KEY_VAL_DOWN_SHORT_UP;
            break;
            
        case KEY_VAL_FFD_PRESS:
            MainMenuKeyVal = KEY_VAL_DOWN_PRESS;
            break;
            
        case KEY_VAL_FFW_SHORT_UP:
            MainMenuKeyVal = KEY_VAL_UP_SHORT_UP;
            break;
            
        case KEY_VAL_FFW_PRESS:
            MainMenuKeyVal = KEY_VAL_UP_PRESS;
            break;
            
        default:
            break;
        }
    }

    //6 key function modification
    if (gSysConfig.KeyNum == KEY_NUM_6)
    {
    
    }
    
    //7 key function 
    switch (MainMenuKeyVal) 
    {
    case KEY_VAL_MENU_PRESS:
		SendMsg(MSG_WIFI_APPW_JUMP);
		printf("send msg:MSG_WIFI_APPW_JUMP\n");
	case KEY_VAL_MENU_LONG_UP:
    case KEY_VAL_MENU_SHORT_UP:
       	RetVal = MainMenuModeKey();
        break;
            
    case KEY_VAL_DOWN_SHORT_UP:
    case KEY_VAL_DOWN_PRESS:
        if (++MenuId > (MAINMENU_ID_MAXNUM - 1))
        {
            MenuId = 0;
        }
        SendMsg(MSG_MENU_DISPLAY_ITEM);
        break;
        
    case KEY_VAL_UP_SHORT_UP:
    case KEY_VAL_UP_PRESS:
        if (MenuId == 0)
        {
            MenuId = (MAINMENU_ID_MAXNUM - 1);
        }
        else 
        {
            MenuId -- ;
        }
        SendMsg(MSG_MENU_DISPLAY_ITEM);
        break;

    case KEY_VAL_HOLD_ON:
        TaskArg.Hold.HoldAction = HOLD_STATE_ON;
        WinCreat(&MainMenuWin, &HoldWin, &TaskArg);
        break;
            
    case KEY_VAL_HOLD_OFF:
        TaskArg.Hold.HoldAction = HOLD_STATE_OFF;
        WinCreat(&MainMenuWin, &HoldWin, &TaskArg);
        break;
            
    default:        
        break;
    }
    return (RetVal);
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuDisplay(void)
  Author        : ZHengYongzhi
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MAIN_MENU_CODE_
void MainMenuDisplay(void)
{
/*	UINT16 pictureIDNum = IMG_ID_MAINMENU_ITEMMUSIC;
	
    if (CheckMsg(MSG_NEED_PAINT_ALL) || (GetMsg(MSG_MENU_DISPLAY_ALL)))
    {
        //display backgroud picture,battery and hold icon.
        //send all initial message
        SendMsg(MSG_MENU_DISPLAY_ITEM);
    }
    
    if (TRUE == GetMsg(MSG_MENU_DISPLAY_ITEM))
    {
		switch(MenuId)
		{
			#ifdef _WIFI_
			case MAINMENU_ID_NET_RADIO: pictureIDNum = IMG_ID_MAINMENU_ITEMNETRADIO; break;
			#endif

			default: break;
		}
        DispPictureWithIDNum(pictureIDNum);
    }*/
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuModeKey(void)
  Author        : ZHengYongzhi
  Description   : main menu modules switch
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
extern THREAD MusicThread;
extern THREAD RecordThread;
extern THREAD FMThread;

_ATTR_MAIN_MENU_CODE_
INT16 MainMenuModeKey(void)
{
    INT16  RetVal = RETURN_OK;
    TASK_ARG TaskArg;
    
    switch(MenuId) 
    {
    #ifdef _WIFI_
    case MAINMENU_ID_NET_RADIO:
        ThreadDeleteAll(&pMainThread);
        TaskSwitch(TASK_ID_WIFI_AP, NULL);
		RetVal = RETURN_FAIL;
        break;
    #endif

    default:
        break;
    }
    
    return (RetVal);
}
/*
********************************************************************************
*
*                         End of MainMenu.c
*
********************************************************************************
*/
