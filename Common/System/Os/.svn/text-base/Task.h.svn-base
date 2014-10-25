/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   Task.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _TASK_H_
#define _TASK_H_

#undef  EXT
#ifdef _IN_TASK_
#define EXT
#else
#define EXT extern
#endif

/*
--------------------------------------------------------------------------------
  
  Description:  task ID definition
  
--------------------------------------------------------------------------------
*/
typedef enum 
{
    TASK_ID_MAINMENU = (UINT32)0,
    TASK_ID_USB,
    TASK_ID_WIFI_AP,
    TASK_ID_WIFI_PLAY,
    TASK_ID_MAX
} TASK_ID;

/*
--------------------------------------------------------------------------------
  
  Description:  task initial parameter.
  
--------------------------------------------------------------------------------
*/
//parameter structure that is for main menu window tasks switch.
typedef __packed struct _MAINMENU_WIN_ARG_
{
    uint8 MenuID;
    
} MAINMENU_WIN_ARG;

//music playing window task switch parameters.
typedef __packed struct _MUSIC_WIN_ARG_
{
    uint16 FileNum;
    uint16 MediaTitleAdd;
    
} MUSIC_WIN_ARG;

typedef __packed struct _MEDIAWIN_WIN_ARG_
{
    uint16 CurId;
    
} MEDIAWIN_WIN_ARG;

typedef __packed struct _MEDIABRO_WIN_ARG_
{
    uint16 CurId;
    uint16 TitleAdd;
    
} MEDIABRO_WIN_ARG;


//video playing window task switch parameters.
typedef __packed struct _VIDEO_WIN_ARG_
{
    uint16 FileNum;
} VIDEO_WIN_ARG;

//Radio window task switch parameters.
typedef __packed struct _RADIO_WIN_ARG_
{
    uint16 Freq;
    
} RADIO_WIN_ARG;

//RecordWin window task switch parameters.
typedef __packed struct _RECORD_WIN_ARG_
{
    uint16 RecordType;
    uint8  RecordFirst;
    
} RECORD_WIN_ARG;

typedef __packed struct _PICTURE_WIN_ARG_
{
    uint16 FileNum;
    
} PICTURE_WIN_ARG;

//TextWin window task switch parameters.
typedef __packed struct _TEXT_WIN_ARG_
{
    uint16 FileNum;
    
} TEXT_WIN_ARG;

//BrowserWin窗口任务切换参数
typedef __packed struct _BROWSER_WIN_ARG_
{
    uint8  FileType;
    uint16 FileNum;
    
} BROWSER_WIN_ARG;

//SysSetWin window task switch parameters.
typedef __packed struct _SYSSET_WIN_ARG_
{
    uint16 FileNum;
    
} SYSSET_WIN_ARG;

//UsbWin parameter window.
typedef __packed struct _USB_WIN_ARG_
{
    uint16 FunSel;
    
} USB_WIN_ARG;

//dialog  parameter window..
typedef __packed struct _DIALOG_WIN_ARG_
{
    uint8  Button;
    uint16 TitleID;
    uint16 ContentID;
    
} DIALOG_WIN_ARG;

//message box  parameter window.
typedef __packed struct _MESSAGE_WIN_ARG_
{
    uint16 TitleID;
    uint16 ContentID;
    uint16 HoldTime;
    
} MESSAGE_WIN_ARG;

//Hold dialog  parameters
typedef __packed struct _HOLD_WIN_ARG_
{
    uint16 HoldAction;
    
} HOLD_WIN_ARG;

//wifi parameter
typedef __packed struct _WI_FI_ARG_
{
	UINT16 CurId;     //global foucs item index..
    UINT16 KeyCounter;//Cousor postion..
}WIFI_ARG;

typedef union {
    
    MAINMENU_WIN_ARG    MainMenu;
    MEDIAWIN_WIN_ARG    Medialib;
    MEDIABRO_WIN_ARG    MediaBro;
    MUSIC_WIN_ARG       Music;
    VIDEO_WIN_ARG       Video;
    RADIO_WIN_ARG       Radio;
	PICTURE_WIN_ARG     Pic;
    RECORD_WIN_ARG      Record;
    TEXT_WIN_ARG        Text;
    BROWSER_WIN_ARG     Browser;
    SYSSET_WIN_ARG      Sysset;
    USB_WIN_ARG         Usb;
    DIALOG_WIN_ARG      Dialog;
    MESSAGE_WIN_ARG     Message;
    HOLD_WIN_ARG        Hold;
    WIFI_ARG            WiFi;
}TASK_ARG;

/*
--------------------------------------------------------------------------------
  
  Description:  任务结构体参数定义
  
--------------------------------------------------------------------------------
*/
typedef struct TASK_STRUCK {
    
    TASK_ID TaskID;                         //task ID
    
    WIN     *TaskMainWinList[TASK_ID_MAX];  //main window pointer table.
    UINT32  CodeIDList[TASK_ID_MAX];        //task modules code id table.
    
    TASK_ARG Arg;
    
}TASK;

/*
--------------------------------------------------------------------------------
  
  Description:  variables redefinition
  
--------------------------------------------------------------------------------
*/
extern TASK Task;

/*-----------------------------------------------------------------------------*/
extern WIN* TaskInit(void **pArg);
extern void TaskSwitch(TASK_ID TaskId, TASK_ARG *pTaskArg);
/*
********************************************************************************
*
*                         End of Task.h
*
********************************************************************************
*/
#endif
