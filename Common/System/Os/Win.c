/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name:  Win.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    
********************************************************************************
*/
#define _IN_WIN_

#include "OsInclude.h"
#include "Driverconfig.h"

extern UINT32 SysService(void);
extern UINT32 WinPaintCheckPaintAllHook(void);
extern void WinPaintDmaFillImage2Lcd(void);
extern void WinPaintBackLightOnHook(void);
extern uint32 WinPaintBLStatusCheckHook(void);

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : UINT WinCreat(WIN *pWin, WIN *pFatherWin)
  Author        : ZHengYongzhi
  Description   : create a new window
                  
  Input         : pWin       ---- current window pointer
                  pFatherWin ---- father window pointer
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 WinCreat(WIN *pFatherWin, WIN *pWin, void *pArg)
{
    //link the window to father window
    if (pFatherWin != NULL) {
        pFatherWin->Son = pWin;
    }
    
    pWin->Father = pFatherWin;
    pWin->Son    = NULL;
    
    //call window inital function.
    pWin->pgWinInit(pArg);
    
    return TRUE;
}

/*
--------------------------------------------------------------------------------
  Function name : UINT WinDestroy(WIN *pWin)
  Author        : ZHengYongzhi
  Description   : delete one window
                  
  Input         : pWin ---- current window pointer
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 WinDestroy(WIN *pWin)
{
    WIN *pWinTemp = pWin;
    
    //find the last child window of the window.
    while (pWin->Son != NULL) {         
        pWin = pWin->Son;
    }
    
    //delete all child windows of this window
    while (pWin != pWinTemp) {          
        
        (pWin->pgWinDeInit)();          //call the auti-initial function of the window
        
        pWin = pWin->Father;
        pWin->Son->Father = NULL;
        pWin->Son = NULL;
    }
    
    //delete this window.
    (pWin->pgWinDeInit)();              //call the auti-initial function of this window.
    if (pWin->Father != NULL) {
        pWin->Father->Son = NULL;
    }
    pWin->Son = NULL;
    //pWin->Father = NULL;
    
    return TRUE;
}

/*
--------------------------------------------------------------------------------
  Function name : WIN* WinGetFocus(WIN *pWin)
  Author        : ZHengYongzhi
  Description   : get current window pointer
                  
  Input         : pWin ---- main window pointer
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
WIN* WinGetFocus(WIN *pWin)
{
    WIN *pWinTemp = pWin;
    
    while (pWinTemp->Son != NULL) 
    {         
        pWinTemp = pWinTemp->Son;
    }
    
    return (pWinTemp);
}

/*
--------------------------------------------------------------------------------
  Function name : void ClearMsg(MSG_ID MsgId)
  Author        : ZHengYongzhi
  Description   : clear message
                  
  Input         : message ID
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 WinServiceProc(WIN *pWin)
{
    WIN *pWinTemp = pWin;
    
    while(pWinTemp->Son != NULL) {          //find the last child window.
        pWinTemp = pWinTemp->Son;
    }
    
    while (pWinTemp->Father != NULL) {      //handle the child window serivice program from the last child window.
        
        (pWinTemp->pgWinService)();
        
        pWinTemp = pWinTemp->Father;
        
    } 
    
    return ((pWinTemp->pgWinService)());    //handle main window,it need return value.
    
}

/*
--------------------------------------------------------------------------------
  Function name : void ClearMsg(MSG_ID MsgId)
  Author        : ZHengYongzhi
  Description   : clear message
                  
  Input         : message ID
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 WinKeyProc(WIN *pWin)
{
    UINT RetVal   = RETURN_OK;
    WIN *pWinTemp = pWin;
    
   while(pWinTemp->Son != NULL) {
        pWinTemp = pWinTemp->Son;               //find the last child window
    }
    
    RetVal = (pWinTemp->pgWinKeyScan)();
    
    if (pWinTemp->Father != NULL) {             //only the return value of main window will call task switch.
        RetVal = RETURN_OK;                     //it always ok of child return value.
    }
    printf("RetVal="d%,RetVal);
    return(RetVal);
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : void ClearMsg(MSG_ID MsgId)
  Author        : ZHengYongzhi
  Description   : clear message
                  
  Input         : message ID
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_CODE_
UINT32 WinPaintProc(WIN *pWin)
{
    WIN *pWinTemp = pWin;
    UINT RetVal = RETURN_OK;

    if (WinPaintBLStatusCheckHook())
    {
        return RetVal;
    }
    
    while(pWinTemp->Son != NULL) 
    {
        if (WinPaintCheckPaintAllHook())
        {//it need to refresh basic window and dialog ,if it return from backlight or hold when a dialog being,
            (pWinTemp->pgWinPaint)();
        } //the message will be recived by the child window handle progarm.
        pWinTemp = pWinTemp->Son;
    }

    (pWinTemp->pgWinPaint)();
    
    WinPaintDmaFillImage2Lcd();

    WinPaintBackLightOnHook();

    return (RetVal);
}

/*
--------------------------------------------------------------------------------
  Function name : void OSStart(WIN *pWin, THREAD *pThread)
  Author        : ZHengYongzhi
  Description   : 
                  
  Input         : ID
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_CODE_
void OSStart(WIN **pWin, THREAD **pThread)
{
    void *pWinArg;
#if PRINTF_CPU_USE
    int idletime=0,sec_prev=0,sec_cur=0;
    float cpubusy=0.0;
#endif
    while(1) 
    {
        *pWin = TaskInit(&pWinArg);//task adjust,call new task code,rturn main window pointer.
        WinCreat((WIN *)NULL, *pWin, pWinArg); 
	 ConnectNumber = 0;
	 printf("wifi init start!\n");
        
        while(1) 
        { 
        #if 1
            if (SysService() != RETURN_OK)
            {
                break;
            }
            
            if (ThreadProcess(*pThread) != TRUE)
            {
                break;
            }
            
            if (WinServiceProc(*pWin) != RETURN_OK)
            {
                break;
            }
                
            if (WinKeyProc(*pWin) != RETURN_OK) 
            {
                break;
            }
           	
            WinPaintProc(*pWin);
	 #endif	

	 #if PRINTF_CPU_USE
            idletime ++;
	     sec_cur = GetSysTick()>>9;// 2^7=1.28S
	     if(sec_cur != sec_prev){
		   sec_prev = sec_cur;
		   cpubusy = 7168.0 / idletime;	// ����ʽ����������1400ת/��Ϊ�����ɣ�ͨ��ָ�����ȵó�
		   printf("CPU usage: %f %\n",cpubusy);   
		   idletime = 0;
	     }
	 #endif
		
        }
        //end the main window.
        WinDestroy(*pWin);
    }
    
}

/*
********************************************************************************
*
*                         End of Win.c
*
********************************************************************************
*/

