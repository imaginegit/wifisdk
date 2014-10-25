/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  Task.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    
********************************************************************************
*/
#define _IN_TASK_

#include "SysInclude.h"
#include "mainmenu.h"
#include "FsInclude.h"
#include "FunUSB.h"
#include "WiFiAPWin.h"
#include "WiFiPlayWin.h"

/*
--------------------------------------------------------------------------------
  Function name : 
  Author        : ZHengYongzhi
  Description   : system stucture initial value
                  
  Input         : pWin —— function return window pointer.返回的窗口指针
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_DATA_
TASK Task = 
{
    (TASK_ID)(TASK_ID_WIFI_AP),
    
    {
        &MainMenuWin,

        #ifdef USB
        &FunUSBWin,        
        #else
        0,
        #endif

        #ifdef _WIFI_
        &WiFiAPWin,
        &WiFiPlayWin,
        #else
        0,
        0,
        #endif      
    },
    
    {
        MODULE_ID_MAINMENU,
        MODULE_ID_USB,
        MODUEL_ID_WIFI_AP_WIN,
        MODUEL_ID_WIFI_PLAY_WIN,
    },
    
    2
};

/*
--------------------------------------------------------------------------------
  Function name : void TaskInit(WIN *pWin)
  Author        : ZHengYongzhi
  Description   : task initial,tasts switch code,get task main window pointer.
                  
  Input         : pWin —— window pointer.
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_CODE_
WIN* TaskInit(void **pArg)
{
    ModuleOverlay(Task.CodeIDList[TASK_ID_WIFI_AP], MODULE_OVERLAY_ALL);

    #if 1
    *pArg = &Task.Arg;
    #else
    
    switch(Task.TaskID)
    {
    case TASK_ID_MAINMENU:
        *pArg = &Task.Arg.MainMenu;
        break;
        
    case TASK_ID_MEDIALIB:
        *pArg = &Task.Arg.Medialib;
        break;
    case TASK_ID_MEDIABRO:
        *pArg = &Task.Arg.MediaBro;
        break;

    case TASK_ID_MUSIC:
        *pArg = &Task.Arg.Music;
        break;
        
     case TASK_ID_VIDEO:
        *pArg = &Task.Arg.Video;
        break;

    case TASK_ID_RADIO:
        *pArg = &Task.Arg.Radio;
        break;
        
    case TASK_ID_RECORD:
        *pArg = &Task.Arg.Record;
        break;
        
    case TASK_ID_EBOOK:
        *pArg = &Task.Arg.Text;
        break;
        
    case TASK_ID_SYSSET:
        *pArg = &Task.Arg.Sysset;
        break;        
        
      case TASK_ID_GAME:
        
        break;      

    case TASK_ID_USB:
        *pArg = &Task.Arg.Usb;
        break;
        
    default:
        break;
    }
    #endif
   return(Task.TaskMainWinList[TASK_ID_WIFI_AP]);
}

/*
--------------------------------------------------------------------------------
  Function name : void TaskSwitch(TASK_ID TaskId, TASK_ARG *pTaskArg)
  Author        : ZHengYongzhi
  Description   : task switch,call by application.
                  
  Input         : TaskId   —— new task id.
                  pTaskArg —— task parameters.
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_CODE_
void TaskSwitch(TASK_ID TaskId, TASK_ARG *pTaskArg)
{
    Task.TaskID = TaskId;

    //clear parameter table message
    memset((uint8*)&Task.Arg, 0, sizeof(Task.Arg));

    if (pTaskArg == (TASK_ARG *)NULL)
    {
        return;
    }

    #if 1
    memcpy((uint8*)&Task.Arg, (uint8*)pTaskArg, sizeof(TASK_ARG));
    #else
    
    //参数赋值
    switch(TaskId) 
    {
    case TASK_ID_MAINMENU:
        Task.Arg.MainMenu.MenuID = pTaskArg->MainMenu.MenuID;
        break;
        
    case TASK_ID_MEDIALIB:
        Task.Arg.Medialib.CurId= pTaskArg->Medialib.CurId;
        break;
        
    case TASK_ID_MEDIABRO:
        Task.Arg.MediaBro.CurId= pTaskArg->MediaBro.CurId;
        Task.Arg.MediaBro.TitleAdd= pTaskArg->MediaBro.TitleAdd;
        break;
        
    case TASK_ID_MUSIC:
        Task.Arg.Music.FileNum = pTaskArg->Music.FileNum;
        Task.Arg.Music.MediaTitleAdd= pTaskArg->Music.MediaTitleAdd;
        break;

    case TASK_ID_VIDEO:
        Task.Arg.Video.FileNum = pTaskArg->Video.FileNum;
        break;
        
    case TASK_ID_PICTURE:
        Task.Arg.Pic.FileNum = pTaskArg->Pic.FileNum;
        break;
        
    case TASK_ID_RADIO:
        break;
        
    case TASK_ID_RECORD:
        Task.Arg.Record.RecordType  = pTaskArg->Record.RecordType;
        Task.Arg.Record.RecordFirst = pTaskArg->Record.RecordFirst;
        break;
        
    case TASK_ID_EBOOK:
        Task.Arg.Text.FileNum = pTaskArg->Text.FileNum;
        break;
        
    case TASK_ID_SYSSET:
        break;

     case TASK_ID_GAME:
        break;
        
    case TASK_ID_USB:
		Task.Arg.Usb.FunSel = pTaskArg->Usb.FunSel;
        break;
        
    default:
        break;
    }
    #endif
}
/*
********************************************************************************
*
*                         End of Task.c
*
********************************************************************************
*/

