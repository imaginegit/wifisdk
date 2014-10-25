/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	WiFiMusicBrowWin.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_WIFIMUSICBROWWIN_

#include "SysInclude.h"
#include "OsInclude.h"
#include "ModuleInfoTab.h"
#include "ModuleOverlay.h"
#include "MemDev.h" 
#include "WiFiMusicBrowWin.h"
#include "XmlAnalyze.h"


#define		WINDESTORYTIMEOUT		500

#define 	MAX_WIFI_LCD_BROWNUM     6

#define     WIFIB_UPDATE_ALL   		0
#define     WIFIB_UPDATE_UP    		1
#define     WIFIB_UPDATE_DOWN  		2



typedef struct _WiFiMUISCBROWITEM_STRUCT
{
    struct _WiFiMUISCBROWITEM_STRUCT *pPrev;
	struct _WiFiMUISCBROWITEM_STRUCT *pNext;
	UINT16 LongFileName[ETHNAMELEN];
}WiFiMUSICBROWITEM;

typedef struct _WiFiMUSICBROWLIST_TREE_STRUCT
{
	UINT16  CurId;			//current force item index in all items.
    UINT16  KeyCounter; 	//current force item index in current screen.
    UINT16  PreCounter; 	//last force item index in current screen.
    UINT16  DispTotalItem;	//total item
    WiFiMUSICBROWITEM * pMUSICBro;
}WiFiMUSICBROWLIST_TREE;

/*
********************************************************************************
*
*                         Variable Define
*
********************************************************************************
*/

_ATTR_WiFiMUSICBrow_BSS_ static WiFiMUSICBROWITEM WiFiMusicBrowItem[MAX_WIFI_LCD_BROWNUM];//552
_ATTR_WiFiMUSICBrow_BSS_ static WiFiMUSICBROWLIST_TREE WiFiMusicBrowTreeInf;
_ATTR_WiFiMUSICBrow_BSS_ static UINT16 * WiFiStationPrintBuf;



_ATTR_WiFiMUSICBrow_CODE_
static void WiFiMuisicBrowGetName(void)
{	
	return;
}

_ATTR_WiFiMUSICBrow_CODE_ 
void WiFiMuiscBrowItemInfoInit(void)
{
    int i;

    //MusicBroItem clear zero to long file name and file number of MusicBroItem
    for(i=0; i<MAX_WIFI_LCD_BROWNUM; i++)
    {
        WiFiMusicBrowItem[i].LongFileName[0] = 0;
    }
	
    //the items in MusicBroItem build one double drection list
    for(i=0; i<MAX_WIFI_LCD_BROWNUM-1; i++)
    {
        WiFiMusicBrowItem[i].pNext = &WiFiMusicBrowItem[i+1];
    }
    for(i=1; i<MAX_WIFI_LCD_BROWNUM; i++)
    {
        WiFiMusicBrowItem[i].pPrev = &WiFiMusicBrowItem[i-1];
    }
    WiFiMusicBrowItem[0].pPrev = &WiFiMusicBrowItem[MAX_WIFI_LCD_BROWNUM-1];
    WiFiMusicBrowItem[MAX_WIFI_LCD_BROWNUM-1].pNext = &WiFiMusicBrowItem[0];

    WiFiMusicBrowTreeInf.CurId = 0;
    WiFiMusicBrowTreeInf.PreCounter = 0;
    WiFiMusicBrowTreeInf.KeyCounter = 0 % MAX_WIFI_LCD_BROWNUM;
    WiFiMusicBrowTreeInf.DispTotalItem = 0;
    WiFiMusicBrowTreeInf.pMUSICBro = &WiFiMusicBrowItem[0];
    
    return;
}


_ATTR_WiFiMUSICBrow_CODE_
static void WiFiMusicBrowUpProc(void)
{
    WiFiMUSICBROWITEM *pBro;
    UINT16  StartItem = 0;
    UINT16  i;

    StartItem = WiFiMusicBrowTreeInf.CurId - WiFiMusicBrowTreeInf.KeyCounter; 

    for (i=0; i<MAX_WIFI_LCD_BROWNUM; i++) //clear the items that is diaplay
    {
        WiFiMusicBrowItem[i].LongFileName[0] = 0;
    } 

    WiFiMusicBrowTreeInf.pMUSICBro = &WiFiMusicBrowItem[0]; 
    pBro = WiFiMusicBrowTreeInf.pMUSICBro;            	

    for (i = 0; (i < MAX_WIFI_LCD_BROWNUM) && ((i + StartItem) < WiFiMusicBrowTreeInf.DispTotalItem); i++)
    {    	  
		Xml_Get_Item((void*)pBro->LongFileName, ETHNAMELEN, XML_UML_NAME, i + StartItem);
        pBro = pBro->pNext;
    }
	
	return;
}



/*
--------------------------------------------------------------------------------
  Function name : void MusicWinInit(void)
  Author        : ZHengYongzhi
  Description   : main menu message initial function
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WiFiMUSICBrow_CODE_
void WiFiMusicBrowWinInit(void *pArg)
{
	WiFiMuiscBrowItemInfoInit();
	WiFiMusicBrowUpProc();
	SendMsg(MSG_WIFI_DIS_ALL); 
}

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinDeInit(void)
  Author        : ZHengYongzhi
  Description   :  main menu message auti-initial function
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WiFiMUSICBrow_CODE_
void WiFiMusicBrowWinDeInit(void)
{	
	SendMsg(MSG_WIFI_STATIONS_WIN_DESTORY);
}


/*
--------------------------------------------------------------------------------
  Function name : UINT32 MusicWinService(void)
  Author        : ZHengYongzhi
  Description   :  main menu message service function
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WiFiMUSICBrow_CODE_
UINT32 WiFiMusicBrowWinService(void)
{
	return RETURN_OK;
}

/*
--------------------------------------------------------------------------------
  Function name : void MusicWinPaint(void)
  Author        : Chenwei
  Description   : main menu display program
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               Chenwei        2009/03/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WiFiMUSICBrow_CODE_
void WiFiMusicBrowWinPaint(void)
{
	
}

_ATTR_WiFiMUSICBrow_CODE_
UINT32 WiFiMusicBrowWinKeyProc(void)
{

    uint32  WifiApKeyVal;
    uint32 RetVal = RETURN_OK;
    TASK_ARG TaskArg;
	
    WifiApKeyVal =  GetKeyVal();
    switch (WifiApKeyVal) 
    {
        case KEY_VAL_MENU_SHORT_UP:
			if (0 != WiFiMusicBrowTreeInf.CurId)
			{
				WiFiMuisicBrowGetName();
				SendMsg(MSG_WIFI_MUSIC_SWITCH);
			}
            WinDestroy(&WiFiMusicBrowWin);
            break;
			
        case KEY_VAL_ESC_SHORT_UP:
            WinDestroy(&WiFiMusicBrowWin);
			break;

		case KEY_VAL_UP_SHORT_UP:
            WiFiMusicBrowTreeInf.PreCounter = WiFiMusicBrowTreeInf.KeyCounter;
            if (WiFiMusicBrowTreeInf.CurId > 0) 
            {
                WiFiMusicBrowTreeInf.CurId--;

                if (WiFiMusicBrowTreeInf.KeyCounter == 0) 
                {
                    WiFiMusicBrowTreeInf.KeyCounter = MAX_WIFI_LCD_BROWNUM - 1;
                    WiFiMusicBrowUpProc();                    
                    SendMsg(MSG_BROW_DIS_ALL_ITEM);
                }
                else
                {
                    WiFiMusicBrowTreeInf.KeyCounter--;
                    SendMsg(MSG_BROW_DIS_SELE_ITEM);
                }
            }
            else
            {
                WiFiMusicBrowTreeInf.CurId = WiFiMusicBrowTreeInf.DispTotalItem - 1;
                if (WiFiMusicBrowTreeInf.DispTotalItem > MAX_WIFI_LCD_BROWNUM)
                {
                    if (WiFiMusicBrowTreeInf.DispTotalItem % MAX_WIFI_LCD_BROWNUM)
                    {
                        WiFiMusicBrowTreeInf.KeyCounter = WiFiMusicBrowTreeInf.DispTotalItem % MAX_WIFI_LCD_BROWNUM - 1;
                    }
                    else
                    {
                        WiFiMusicBrowTreeInf.KeyCounter =  MAX_WIFI_LCD_BROWNUM - 1;
                    }
                    WiFiMusicBrowUpProc();
                    SendMsg(MSG_BROW_DIS_ALL_ITEM);
                }
                else
                {
                    WiFiMusicBrowTreeInf.KeyCounter = WiFiMusicBrowTreeInf.DispTotalItem - 1;
                    SendMsg(MSG_BROW_DIS_SELE_ITEM);
                }
            }
            break;
            
		case KEY_VAL_DOWN_SHORT_UP:
            WiFiMusicBrowTreeInf.PreCounter = WiFiMusicBrowTreeInf.KeyCounter;
            if(WiFiMusicBrowTreeInf.CurId < (WiFiMusicBrowTreeInf.DispTotalItem - 1))
            {
                WiFiMusicBrowTreeInf.CurId++; 
                if(WiFiMusicBrowTreeInf.KeyCounter >= MAX_WIFI_LCD_BROWNUM - 1) 
                {
                    WiFiMusicBrowTreeInf.KeyCounter = 0;
                    WiFiMusicBrowUpProc();                    
                    SendMsg(MSG_BROW_DIS_ALL_ITEM);//display all screen
                }
                else
                {
                    WiFiMusicBrowTreeInf.KeyCounter++;
                    SendMsg(MSG_BROW_DIS_SELE_ITEM);
                }  
            }
            else
            {
                WiFiMusicBrowTreeInf.CurId = 0;
                WiFiMusicBrowTreeInf.KeyCounter = 0;
                if(WiFiMusicBrowTreeInf.DispTotalItem > MAX_WIFI_LCD_BROWNUM)
                {
                    WiFiMusicBrowUpProc();
                    SendMsg(MSG_BROW_DIS_ALL_ITEM);
                }
                else
                {
                   SendMsg(MSG_BROW_DIS_SELE_ITEM);
                }                
            }
            break;
            
		default:        
			break;
    }
    return RetVal;
}

/*
********************************************************************************
*
*                         End of WiFiMusicBrowWin.c
*
********************************************************************************
*/

