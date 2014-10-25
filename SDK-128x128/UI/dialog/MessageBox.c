/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name£º   Message.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                anzhiguo      2009-2-27         1.0
*    desc:    ORG.
********************************************************************************
*/

#define _IN_MESSAGE_BOX

#include "SysInclude.h"
#include "MessageBox.h"
#include "Hold.h"

/*
--------------------------------------------------------------------------------
  Function name : void MessageBoxInit(void *pArg)
  Author        : ZhengYongzhi
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2009-4-3         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MESSAGEBOX_CODE_
void MessageBoxInit(void *pArg)
{
    MessageBoxTitleID   = ((MESSAGE_WIN_ARG*)pArg)->TitleID;
    MessageBoxContentID = ((MESSAGE_WIN_ARG*)pArg)->ContentID;
    MessageBoxHoldTime  = ((MESSAGE_WIN_ARG*)pArg)->HoldTime;
    
    KeyReset();

    MessageBoxSystickCounterBack = SysTickCounter;    
    //initial regular time
    SendMsg(MSG_MESSAGE_DISPLAY_ALL);
}

/*
--------------------------------------------------------------------------------
  Function name : void MessageBoxDeInit(void)
  Author        : ZhengYongzhi
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2009-4-3         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MESSAGEBOX_CODE_
void MessageBoxDeInit(void)
{
    KeyReset();
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 MessageBoxService(void)
  Author        : ZhengYongzhi
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2009-4-3         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MESSAGEBOX_CODE_
uint32 MessageBoxService(void)
{
    INT16 Retval = 0;
    UINT32 Count;
    //check regular time,regular time come,message bos return
    Count = SysTickCounter - MessageBoxSystickCounterBack;
    if(Count > MessageBoxHoldTime * 100)
    {
       WinDestroy(&MessageBoxWin);
       SendMsg(MSG_MESSAGEBOX_DESTROY);
    }
    
    return(Retval);
}
   
/*
--------------------------------------------------------------------------------
  Function name : UINT32 MessageBoxKey(void)
  Author        : ZhengYongzhi
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2009-4-3         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MESSAGEBOX_CODE_
uint32 MessageBoxKey(void)
{
    UINT32 RetVal = 0;
    UINT32 KeyTemp;
    TASK_ARG TaskArg;

#if 0    
    KeyTemp =  GetKeyVal();
    
    switch (KeyTemp) 
    {
    case KEY_VAL_MENU_DOWN:
    case KEY_VAL_PLAY_DOWN:
    case KEY_VAL_FFD_DOWN:
    case KEY_VAL_FFW_DOWN:
    case KEY_VAL_UP_DOWN:
    case KEY_VAL_DOWN_DOWN:
    case KEY_VAL_VOL_DOWN:
        break;
        
    case KEY_VAL_HOLD_ON:
        TaskArg.Hold.HoldAction = HOLD_STATE_ON;
        WinCreat(&MessageBoxWin, &HoldWin, &TaskArg);
        break;
            
    case KEY_VAL_HOLD_OFF:
        TaskArg.Hold.HoldAction = HOLD_STATE_OFF;
        WinCreat(&MessageBoxWin, &HoldWin, &TaskArg);
        break;
        
    default:
        break;
    }
#endif    
    return (RetVal);    
}    

/*
--------------------------------------------------------------------------------
  Function name : void MessageBoxDisplay(void)
  Author        : ZhengYongzhi
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2009-4-3         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_MESSAGEBOX_CODE_
void MessageBoxDisplay(void)
{
/*    UINT16              color_tmp, TempCharSize;
    PICTURE_INFO_STRUCT PicInfo;    ////it will be used when operating text that is related with brower position
    
    color_tmp = LCD_GetColor();     
    LCD_SetColor(COLOR_BLACK);
    TempCharSize = LCD_SetCharSize(FONT_12x12);
    
    if(CheckMsg(MSG_NEED_PAINT_ALL) || GetMsg(MSG_MESSAGE_DISPLAY_ALL))
    {
        //display backgroud picture and message box content
        DispPictureWithIDNum(IMG_ID_MESSAGEBOX_BACKGROUND);
        DisplayMenuStrWithIDNum(MESSAGE_TEXT_TITLE_X, MESSAGE_TEXT_TITLE_Y, MESSAGE_TEXT_TITLE_XSIZE, MESSAGE_TEXT_TITLE_YSIZE, LCD_TEXTALIGN_CENTER, MessageBoxTitleID);                  //Title
        DisplayMenuStrWithIDNum(MESSAGE_TEXT_CONTENT_X, MESSAGE_TEXT_CONTENT_Y, MESSAGE_TEXT_CONTENT_XSIZE, MESSAGE_TEXT_CONTENT_YSIZE, LCD_TEXTALIGN_CENTER, MessageBoxContentID);        //Content
        //send other picture display message
    }

    LCD_SetColor(color_tmp); 
    LCD_SetCharSize(TempCharSize);*/
}

/*
********************************************************************************
*
*                         End of MessageBox.c
*
********************************************************************************
*/

