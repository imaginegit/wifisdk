/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	WiFiKeyboardWin.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_WIFIKEYBOARDWIN_

#include "SysInclude.h"
//#include "MainMenu.h"
#include "WiFiKeyboardWin.h"
#include "WiFiApWin.h"
#include "messagebox.h"
#include "def.h"
#include "netif.h"


#define IMGKEYBG	IMG_ID_MUSIC_AB_OFF
#define IMGKEYBGXS	15
#define IMGKEYBGYS	14

#define KBXNUM		8
#define KBYNUM		5

_ATTR_KEYBOARD_WIN_DATA_
static UINT16 Keyboard0[] = 
{
	'0', '1', '2', '3', '4', '5', '6', '7', 
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
	'w', 'x', 'y', 'z',  0
};

_ATTR_KEYBOARD_WIN_DATA_
static UINT16 Keyboard1[] = 
{
	'0', '1', '2', '3', '4', '5', '6', '7', 
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 
	'W', 'X', 'Y', 'Z', 0
};

_ATTR_KEYBOARD_WIN_DATA_
static UINT16 Keyboard2[] = 
{
	'~', '@', '#', '$', '%', '^', '&', '*', 
	'(', ')', '_', '-', '+', '=', '[', ']', 
	'{', '}', '|', '"', ':', ';', ',', '<', 
	'.', '>', '?', '/', '`', '!', 0
};

_ATTR_KEYBOARD_WIN_BSS_ static UINT16 * foucsKeyboard;
_ATTR_KEYBOARD_WIN_BSS_ static int foucsKeyboardKeyTotal;
_ATTR_KEYBOARD_WIN_BSS_ static int foucsKeyboardKeyIndex;


_ATTR_KEYBOARD_WIN_CODE_
void WiFiKeyBoardWinInit(void *pArg)
{	
	/* keyboard control initialize..*/
	foucsKeyboard = Keyboard0;
	foucsKeyboardKeyTotal = 36;
	foucsKeyboardKeyIndex = 0;
	
	/* for new password input initialize..*/
	gSysConfig.WiFiConfig.password[0] = 0;
	gSysConfig.WiFiConfig.password_len = 0;

    /* send message paint all..*/
	SendMsg(MSG_WIFI_DIS_ALL);
}

_ATTR_KEYBOARD_WIN_CODE_
void WiFiKeyBoardWinDeInit(void)
{
	return;
}

_ATTR_KEYBOARD_WIN_CODE_
UINT32 WiFiKeyBoardWinService(void)
{
	return RETURN_OK;
}


_ATTR_KEYBOARD_WIN_CODE_
UINT32 WiFiKeyBoardWinKey(void)
{
    uint32  KeyVal, MaxIdx;
    uint32 RetVal = RETURN_OK;
    TASK_ARG TaskArg;
	
    KeyVal =  GetKeyVal();
    switch (KeyVal) 
    {
        case KEY_VAL_MENU_SHORT_UP:
			if (gSysConfig.WiFiConfig.password_len < PASSWORD_LEN)
			{
	            gSysConfig.WiFiConfig.password[gSysConfig.WiFiConfig.password_len] = foucsKeyboard[foucsKeyboardKeyIndex];
				gSysConfig.WiFiConfig.password_len++;
				SendMsg(MSG_WIFI_PASSWORD_DISP);
			}
			break;
			
        case KEY_VAL_ESC_SHORT_UP:
			if (gSysConfig.WiFiConfig.password_len)
			{
				gSysConfig.WiFiConfig.password_len--;
	            gSysConfig.WiFiConfig.password[gSysConfig.WiFiConfig.password_len] = 0;
				SendMsg(MSG_WIFI_PASSWORD_DISP);
			}
			else
			{
	            WinDestroy(&WiFiKeyBoardWin);
				SendMsg(MSG_WIFI_DIS_ALL);
				SendMsg(MSG_BROW_DIS_ALL_ITEM);
			}
			break;
			
        case KEY_VAL_UP_PRESS:
        case KEY_VAL_UP_DOWN: 
            if (foucsKeyboardKeyIndex == 0)
            {
				foucsKeyboardKeyIndex = foucsKeyboardKeyTotal;
            }
			foucsKeyboardKeyIndex--;
			SendMsg(MSG_WIFI_KEYBOARD_UPDATA_FOUCS);
            break;
            
        case KEY_VAL_DOWN_PRESS:
        case KEY_VAL_DOWN_DOWN:
			foucsKeyboardKeyIndex++;
			foucsKeyboardKeyIndex %= foucsKeyboardKeyTotal;
			SendMsg(MSG_WIFI_KEYBOARD_UPDATA_FOUCS);
            break;

		case KEY_VAL_FFW_DOWN:
			if (foucsKeyboard == Keyboard0)
			{
				foucsKeyboard = Keyboard2;
				foucsKeyboardKeyTotal = 30; 
			}
			else if (foucsKeyboard == Keyboard1) 
			{
				foucsKeyboard = Keyboard0;
				foucsKeyboardKeyTotal = 36; 
			}
			else
			{
				foucsKeyboard = Keyboard1;
				foucsKeyboardKeyTotal = 36; 
			}
			foucsKeyboardKeyIndex = 0;
			SendMsg(MSG_WIFI_KEYBOARD_DISP_ALL);
            break;

		case KEY_VAL_FFD_DOWN://next class keyboard
			if (foucsKeyboard == Keyboard0)
			{
				foucsKeyboard = Keyboard1;
				foucsKeyboardKeyTotal = 36; 
			}
			else if (foucsKeyboard == Keyboard1) 
			{
				foucsKeyboard = Keyboard2;
				foucsKeyboardKeyTotal = 30; 
			}
			else
			{
				foucsKeyboard = Keyboard0;
				foucsKeyboardKeyTotal = 36; 
			}
			foucsKeyboardKeyIndex = 0;
			SendMsg(MSG_WIFI_KEYBOARD_DISP_ALL);
            break;

		case KEY_VAL_PLAY_SHORT_UP:
            WinDestroy(&WiFiKeyBoardWin);
			/* start wifi connect. */
			SendMsg(MSG_WIFI_KEYBOARD_DESTORY);
			break;
			
		default:        
			break;
    }
	
	return (RetVal);
}

_ATTR_KEYBOARD_WIN_CODE_
static void PasswordPaint(void)
{
/*    UINT16 TempColor, TempCharSize, i;
	UINT16 Password[ETHNAMELEN+2] = {0};
	LCD_RECT rect;
	
	TempColor = LCD_GetColor();
	TempCharSize = LCD_SetCharSize(FONT_12x12);

	LCD_SetColor(COLOR_BLACK);
	LCD_SetTextMode(LCD_DRAWMODE_TRANS);

	DispPictureWithIDNumAndXY(IMG_ID_MP4_B, 0, 16);

    for (i = 0; i < gSysConfig.WiFiConfig.password_len; i++)
	{
		Password[i] = gSysConfig.WiFiConfig.password[i];
	}
	Password[i] = '_';
	Password[i+1] = 0;

	rect.x0 = 0;
	rect.y0 = 16;
	rect.x1 = 128;
	rect.y1 = 48;
	LCD_DispStringInRect(&rect, &rect, Password, LCD_TEXTALIGN_CENTER);
	
    LCD_SetColor(TempColor);
    LCD_SetCharSize(TempCharSize);*/
	return;
}

_ATTR_KEYBOARD_WIN_CODE_
static void KeyOnePaint(int i)

{
/*    int XPos, YPos, width;
	UINT16 KeyValStr[2];

	i %= foucsKeyboardKeyTotal;
	
	XPos = 4 + (i % KBXNUM) * IMGKEYBGXS - 6;
	YPos = 48 + (i / KBXNUM) * IMGKEYBGYS;
	DispPictureWithIDNumAndXYoffset(IMGKEYBG, XPos, YPos);
	
	KeyValStr[0] = foucsKeyboard[i];
	KeyValStr[1] = 0;
	width = LCD_GetCharXSize(foucsKeyboard[i]);
	XPos += ((IMGKEYBGXS - width) >> 1) + 6;
	YPos += ((IMGKEYBGYS - 12) >> 1);
	LCD_NFDispStringAt(XPos, YPos, KeyValStr);*/

	return;
}

_ATTR_KEYBOARD_WIN_CODE_
static void DisplayKeyboardFoucs(void)
{
	//8 * 15 = 120 < 128      5 * 14 = 70 < 80
	KeyOnePaint(foucsKeyboardKeyIndex+1);
	KeyOnePaint(foucsKeyboardKeyIndex+foucsKeyboardKeyTotal-1);

	KeyOnePaint(foucsKeyboardKeyIndex);

	return;
}

_ATTR_KEYBOARD_WIN_CODE_
static void DisplayKeyboardAll(void)
{
    UINT16 i;

	//8 * 15 = 120 < 128      5 * 14 = 70 < 80
	for (i = 0; i < foucsKeyboardKeyTotal; i++)
	{
		KeyOnePaint(i);
	}
	
	KeyOnePaint(foucsKeyboardKeyIndex);
	return;
}

_ATTR_KEYBOARD_WIN_CODE_
void WiFiKeyBoardWinDisplay(void)
{
	
}

/*
********************************************************************************
*
*                         End of WiFiKeyBoardWin.c
*
********************************************************************************
*/
