/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	WiFiPlayWin.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_WIFIPLAYWIN_

#include "SysInclude.h"
#include "MainMenu.h"
#include "wificontrol.h"
#include "WiFiPlayWin.h"
#include "WiFiMusicBrowWin.h"


_ATTR_WIFI_PLAY_WIN_BSS_ UINT32 WiFiCurrentTimeSecBk;

extern BOOLEAN bWiFiAudioPause;

_ATTR_WIFI_PLAY_WIN_CODE_
void WiFiPlayWinInit(void *pArg)
{
	WiFiCurrentTimeSecBk = 0;

	printf("WiFiPlayWinInit\n");
	/* Create the wifi music thread. */
	SendMsg(MSG_WIFI_DIS_ALL);
	
	return;
}


_ATTR_WIFI_PLAY_WIN_CODE_
void WiFiPlayWinDeInit(void)
{
    ThreadDelete(&pMainThread, &WiFiThread);
	return;
}


_ATTR_WIFI_PLAY_WIN_CODE_
UINT32 WiFiPlayWinService(void)
{
    TASK_ARG TaskArg;

	if (GetMsg(MSG_WIFI_AUDIO_OPENERROR) || GetMsg(MSG_WIFI_PLAY_FINSH))
	{
        TaskArg.MainMenu.MenuID = MAINMENU_ID_NET_RADIO;
        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        return RETURN_FAIL;
	}

	/* Create Ethernet radio stations list. */
    if (GetMsg(MSG_WIFI_GO_STATIONS_WIN))
    {
		ClearMsg(MSG_MUSIC_DISPFLAG_CURRENT_TIME);
		WinCreat(&WiFiPlayWin, &WiFiMusicBrowWin, NULL);
		return RETURN_OK;
	}

	if (GetMsg(MSG_WIFI_STATIONS_WIN_DESTORY))
	{
		SendMsg(MSG_WIFI_DIS_ALL);
	}

	return RETURN_OK;
}


_ATTR_WIFI_PLAY_WIN_CODE_
UINT32 WiFiPlayWinKey(void)
{
    uint32  KeyVal;
    uint32 RetVal = RETURN_OK;
    TASK_ARG TaskArg;
	
    KeyVal =  GetKeyVal();
    switch (KeyVal) 
    {
        case KEY_VAL_MENU_SHORT_UP:
			SendMsg(MSG_WIFI_GO_STATIONS_WIN);
            break;
			
        case KEY_VAL_ESC_SHORT_UP:
	        TaskArg.MainMenu.MenuID = MAINMENU_ID_NET_RADIO;
	        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
            RetVal = RETURN_FAIL;
			break;

        case KEY_VAL_PLAY_SHORT_UP:                             // play pause 
			if (bWiFiAudioPause == FALSE)
			{
				printf("wifi pause .\n");
				bWiFiAudioPause = TRUE;
			}
			else
			{
				printf("wifi play.\n");
				bWiFiAudioPause = FALSE;
			}
            SendMsg(MSG_MUSIC_DISPFLAG_STATUS);
            break;
            
        case KEY_VAL_UP_DOWN:                                   //volume increse
        case KEY_VAL_UP_PRESS:  
            
            break;    
            
        case KEY_VAL_DOWN_DOWN:                                 //volume reduce
        case KEY_VAL_DOWN_PRESS:
            
            break;

        default: 
            break;

    }
	
    return (RetVal);
}


_ATTR_WIFI_PLAY_WIN_CODE_
void WiFiPlayWinDisplay(void)
{
	
}
/*
********************************************************************************
*
*                         End of WiFiPlayWin.c
*
********************************************************************************
*/

