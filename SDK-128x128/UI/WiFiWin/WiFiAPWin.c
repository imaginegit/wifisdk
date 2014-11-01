/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	WiFiAPWin.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_WIFIAPWIN_

#include "SysInclude.h"
#include "WiFiAPWin.h"
#include "wificontrol.h"
#include "messagebox.h"
#include "WiFiKeyboardWin.h"
#include "MainMenu.h"
#include "protocol.h"

/*
********************************************************************************
*
*                         Declaration
*
********************************************************************************
*/

extern int rk_get_scan_ssid(void* ssid, int index, UINT8 bwc);
extern int rk_get_scan_ssid_total(void);

/*
********************************************************************************
*
*                        Macro Define 
*
********************************************************************************
*/

#define 	MAX_AP_NUM_LCD_DISP     6

#define     ALL_BROITEM_UPDATE_AP   0
#define     UP_UPDATE_AP    		1
#define     DOWN_UPDATE_AP  		2

/*
********************************************************************************
*
*                         Structure Define
*
********************************************************************************
*/
typedef struct _WIFIAPITEM_STRUCT
{
    struct _WIFIAPITEM_STRUCT *pPrev;
	struct _WIFIAPITEM_STRUCT *pNext;
    uint8  rssiVal;
	UINT16 LongFileName[ETHNAMELEN + 1];
}WIFIAPITEM;

typedef struct _WIFIAPLIST_TREE_STRUCT
{
	UINT16  CurId;			//current force item index in all items.
    UINT16  KeyCounter; 	//current force item index in current screen.
    UINT16  PreCounter; 	//last force item index in current screen.
    UINT16  DispTotalItem;	//total item
    WIFIAPITEM * pWIFIBro;
}WIFIAPLIST_TREE;

typedef struct _WIFIAPPASSWORD_STRUCT
{
    UINT16      header;
    UINT16      age;
    WIFI_CONFIG APPW;
}WIFIAPPASSWORD;



/*
********************************************************************************
*
*                         Variable Define
*
********************************************************************************
*/

_ATTR_WIFI_AP_WIN_BSS_ WIFIAPITEM WiFiAPItem[MAX_AP_NUM_LCD_DISP];//552
_ATTR_WIFI_AP_WIN_BSS_ WIFIAPLIST_TREE WIFIAPTreeInf;
_ATTR_WIFI_AP_WIN_BSS_ UINT16 * WiFiAPPrintBuf;
_ATTR_WIFI_AP_WIN_BSS_ UINT16 CurScanAPNum;


/*
--------------------------------------------------------------------------------
  Function name : void SSIDItemInfoInit(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WIFI_AP_WIN_CODE_ 
void SSIDItemInfoInit(void)
{
    int i;

    //MusicBroItem clear zero to long file name and file number of MusicBroItem
    for(i=0; i<MAX_AP_NUM_LCD_DISP; i++)
    {
        WiFiAPItem[i].LongFileName[0] = 0;
    }
    //the items in MusicBroItem build one double drection list
    for(i=0; i<MAX_AP_NUM_LCD_DISP-1; i++)
    {
        WiFiAPItem[i].pNext = &WiFiAPItem[i+1];
    }
    for(i=1; i<MAX_AP_NUM_LCD_DISP; i++)
    {
        WiFiAPItem[i].pPrev = &WiFiAPItem[i-1];
    }
    WiFiAPItem[0].pPrev = &WiFiAPItem[MAX_AP_NUM_LCD_DISP-1];
    WiFiAPItem[MAX_AP_NUM_LCD_DISP-1].pNext = &WiFiAPItem[0];

    WIFIAPTreeInf.PreCounter = 0;
    WIFIAPTreeInf.KeyCounter = 0;
    WIFIAPTreeInf.CurId = 0;
    
    WIFIAPTreeInf.pWIFIBro = &WiFiAPItem[0];
    WIFIAPTreeInf.DispTotalItem = CurScanAPNum;
    
}

/*
--------------------------------------------------------------------------------
  Function name :void WiFiSSIDUpProc(UINT16 uiUpdateType)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WIFI_AP_WIN_CODE_
void WiFiSSIDUpProc(UINT16 uiUpdateType)
{
	char APRssi[4] = {30, 50, 65, 80};
	int  rssi;
    WIFIAPITEM *pBro;
    UINT16  StartItem = 0;
    UINT16  i, j;

    StartItem = WIFIAPTreeInf.CurId - WIFIAPTreeInf.KeyCounter; 

    switch(uiUpdateType)
    {
        case ALL_BROITEM_UPDATE_AP:
            for(i=0; i<MAX_AP_NUM_LCD_DISP; i++) //clear the items that is diaplay
            {
                WiFiAPItem[i].LongFileName[0] = 0;
            } 

            WIFIAPTreeInf.pWIFIBro = &WiFiAPItem[0]; 
            pBro = WIFIAPTreeInf.pWIFIBro;            	

            for(i = 0;(i < MAX_AP_NUM_LCD_DISP) && ((i + StartItem) < WIFIAPTreeInf.DispTotalItem);i++)
            {    	  
                //*(pBro->LongFileName) = StartItem + i + 0x30;
                //*(pBro->LongFileName + 1) = 0;
                /* get AP ssid. */
                rk_get_scan_ssid(pBro->LongFileName, StartItem + i, 1);

				/* get ap rssi strengh bmp index */
				rssi = rk_get_ap_rssi(i + StartItem);
				for (j = 0; j < 4; j++)
				{
					if (rssi <= APRssi[j])
					{
						break;
					}
				}
				pBro->rssiVal = 4 - i;

				/* get next object. */	
                pBro = pBro->pNext;
            }
            break;
    }
}


/*
--------------------------------------------------------------------------------
  Function name : void WiFiWinSvcStart(void *pArg)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WIFI_AP_WIN_CODE_
void WiFiWinSvcStart(void *pArg)
{
    //Music backgroud service handle start
    if (TRUE != ThreadCheck(pMainThread, &WiFiThread))
    {
        ModuleOverlay(MODUEL_ID_WIFI_SYS, MODULE_OVERLAY_ALL);
        ThreadCreat(&pMainThread, &WiFiThread, pArg);
    }
}

/*
--------------------------------------------------------------------------------
  Function name :void WiFiAPWinInit(void *pArg)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WIFI_AP_WIN_CODE_
void WiFiAPWinInit(void *pArg)
{
    TASK_ARG TaskArg;

    KeyReset();
    SetPowerOffTimerDisable();
    AutoPowerOffDisable();

    //显示路由器扫描等待界面
    /*Lcd_Clear(0x0000);
    DispPictureWithIDNum(IMG_ID_DIALOGBOX_BACKGROUND);
	LCD_NFDispStringAt(20, 60, L"Scaning the ssid.");*/

    WiFiWinSvcStart(pArg);  

	CurScanAPNum = 0;
	SendMsg(MSG_WIFI_DIS_ALL); 
}

/*
--------------------------------------------------------------------------------
  Function name :void WiFiAPWinDeInit(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WIFI_AP_WIN_CODE_
void WiFiAPWinDeInit(void)
{	
    SetPowerOffTimerEnable();
    AutoPowerOffEnable();
}


_ATTR_WIFI_AP_WIN_CODE_
static int WiFiReadAPPWFrmRecord(void)
{	
	int i, k, rssimin;
	WIFIAPPASSWORD *pAPWD;
	uint8 apPWInfoBlkData[WIFI_APPW_BLKSIZE];

	/* overlay code for ssid and password read and sortout.*/
	ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
	
	ReadReservedData(WIFI_AP_PW_BLK, apPWInfoBlkData, WIFI_APPW_BLKSIZE);
	pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
	ConnectNumber++;
	if(pAPWD->header != WIFI_APPW_ITEM_OK || ConnectNumber > 2) {
		printf("fle--->init connect\n");
		SoftApStart = 1;
		strcpy(gSysConfig.WiFiConfig.ssid, "initap201412");
		strcpy(gSysConfig.WiFiConfig.password, "123456789");
		gSysConfig.WiFiConfig.password_len = 9;
		/*strcpy(gSysConfig.WiFiConfig.ssid, "imagine");
		strcpy(gSysConfig.WiFiConfig.password, "13428725525");
		gSysConfig.WiFiConfig.password_len = 11;*/
		return;
	}

	if (FALSE != CheckMsg(MSG_WIFI_APPW_JUMP))
	{
		printf("fle--->wifi appw jump\n");
		return 0;
	}
	
	rssimin = 0xFFFF;
	for (i = 0; i < WIFI_APPW_ITEMNUM; i++)
	{
		if (pAPWD->header == WIFI_APPW_ITEM_OK)
		{
			k = rk_check_scan_ssid(pAPWD->APPW.ssid);
			if (k >= 0)
			{
				if (rk_get_ap_rssi(k) < rssimin)
				{
					rssimin = rk_get_ap_rssi(k);
					memcpy(&gSysConfig.WiFiConfig, &pAPWD->APPW, sizeof(WIFI_CONFIG));
					//printf("WiFiReadAPPWFrmRecord %s %s\n", pAPWD->APPW.ssid, pAPWD->APPW.password);
				}
			}
		}
		pAPWD++;
	}

	return 0;
}


_ATTR_WIFI_AP_WIN_CODE_
static int WiFiCheckAPPWFrmRecord(void)
{	
	int i, k, rssimin;
	WIFIAPPASSWORD *pAPWD;
	uint8 apPWInfoBlkData[WIFI_APPW_BLKSIZE];

	/* overlay code for ssid and password read and sortout.*/
	ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
	
	ReadReservedData(WIFI_AP_PW_BLK, apPWInfoBlkData, WIFI_APPW_BLKSIZE);
	pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
	
	for (i = 0; i < WIFI_APPW_ITEMNUM; i++)
	{
		if (pAPWD->header == WIFI_APPW_ITEM_OK)
		{
			if (strcmp(pAPWD->APPW.ssid, gSysConfig.WiFiConfig.ssid) == 0)
			{
				memcpy(&gSysConfig.WiFiConfig, &pAPWD->APPW, sizeof(WIFI_CONFIG));
				return 0;
			}
		}
		pAPWD++;
	}

	return -1;
}


_ATTR_WIFI_AP_WIN_CODE_
static int WiFiDelFailAPPWFrmRecord(void)
{
	int i;
	WIFIAPPASSWORD *pAPWD;
	uint8 apPWInfoBlkData[WIFI_APPW_BLKSIZE];

	/* overlay code for ssid and password read and sortout.*/
	ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
	ReadReservedData(WIFI_AP_PW_BLK, apPWInfoBlkData, WIFI_APPW_BLKSIZE);
	pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
	for (i = 0; i < WIFI_APPW_ITEMNUM; i++)
	{
		if (strcmp(pAPWD->APPW.ssid, gSysConfig.WiFiConfig.ssid) == 0)
		{
			memset(&gSysConfig.WiFiConfig, 0, sizeof(WIFI_CONFIG));
			pAPWD->header = 0;
			WriteReservedData(WIFI_AP_PW_BLK, apPWInfoBlkData, WIFI_APPW_BLKSIZE);
			printf("WiFiDelFailAPPWFrmRecord ok i=%d\n", i);
			break;
		}
		pAPWD++;
	}
	return 0;
}


_ATTR_WIFI_AP_WIN_CODE_
static int WiFiSaveOkAPPWToRecord(void)
{
	int i, oki, minAge;
	WIFIAPPASSWORD *pAPWD;
	uint8 apPWInfoBlkData[WIFI_APPW_BLKSIZE];
	uint8 dat;

	if(!strcmp("initap201412", gSysConfig.WiFiConfig.ssid)) return 0;

	/* overlay code for ssid and password read and sortout.*/
	ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
	ReadReservedData(WIFI_AP_PW_BLK, apPWInfoBlkData, WIFI_APPW_BLKSIZE);
	
	/* 1th step: check the ssid had been saved. */
	pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
	for (i = 0; i < WIFI_APPW_ITEMNUM; i++)
	{
		if (pAPWD->header == WIFI_APPW_ITEM_OK)
		{
			if (strcmp(pAPWD->APPW.ssid, gSysConfig.WiFiConfig.ssid) == 0)
			{
				if ((strcmp(pAPWD->APPW.password, gSysConfig.WiFiConfig.password) == 0) 
					 && (pAPWD->APPW.password_len == gSysConfig.WiFiConfig.password_len))
				{
					if (pAPWD->age > 0)
					{
						pAPWD->age = 0;
						goto WRFLASHEND;
					}
					goto DIRECTIONEND;
				}
				else
				{
					pAPWD->header = WIFI_APPW_ITEM_OK;
					pAPWD->age = 0;
					memcpy(&pAPWD->APPW, &gSysConfig.WiFiConfig, sizeof(WIFI_CONFIG));
					goto WRFLASHEND;
				}
			}
		}
		pAPWD++;
	}

	/* 2th older the ssid had been saved. */
	pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
	for (i = 0; i < WIFI_APPW_ITEMNUM; i++)
	{
		if (pAPWD->header == WIFI_APPW_ITEM_OK)
		{
			pAPWD->age++;
		}
	}

	/* 3th step: save the ssid to null item. */
	pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
	for (i = 0; i < WIFI_APPW_ITEMNUM; i++)
	{
		if (pAPWD->header != WIFI_APPW_ITEM_OK)
		{
			pAPWD->header = WIFI_APPW_ITEM_OK;
			pAPWD->age = 0;
			memcpy(&pAPWD->APPW, &gSysConfig.WiFiConfig, sizeof(WIFI_CONFIG));
			goto WRFLASHEND;
		}
		pAPWD++;
	}

	
	/* 4th step: replace the most old. */
	oki = -1;
	minAge = 0xFFFF;
	pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
	for (i = 0; i < WIFI_APPW_ITEMNUM; i++)
	{
		if (pAPWD->age < minAge)
		{
			oki = i;
			minAge = pAPWD->age;
		}
		pAPWD++;
	}
	pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
	pAPWD += oki;
	pAPWD->header = WIFI_APPW_ITEM_OK;
	pAPWD->age = 0;
	memcpy(&pAPWD->APPW, &gSysConfig.WiFiConfig, sizeof(WIFI_CONFIG));
	
WRFLASHEND:
	WriteReservedData(WIFI_AP_PW_BLK, apPWInfoBlkData, WIFI_APPW_BLKSIZE);
	SaveSysInformation();
	
DIRECTIONEND:
	#if 1
	{
		ReadReservedData(WIFI_AP_PW_BLK, apPWInfoBlkData, WIFI_APPW_BLKSIZE);
		pAPWD = (WIFIAPPASSWORD *)apPWInfoBlkData;
		for (i = 0; i < WIFI_APPW_ITEMNUM; i++)
		{
			printf("i=%d %d ap=%s pw=%s pwl=%d\n", i, pAPWD->header, pAPWD->APPW.ssid, pAPWD->APPW.password, pAPWD->APPW.password_len);
			pAPWD++;
		}
	}
	#endif
	
	return 0;
}
/*
--------------------------------------------------------------------------------
  Function name :UINT32 WiFiAPWinService(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WIFI_AP_WIN_CODE_
UINT32 WiFiAPWinService(void)
{
	TASK_ARG TaskArg;
	//uint8 temp[FRAME_TYPE_NAME_LEN];

    //*************************************************
    //  扫描路由器失败，进入本地音乐播放
    //  路由器扫描成功
    //  1. 如果扫描到上次记录的路由器，直接连接路由器
    //  2. 如果没有，进入路由器列表
    //*************************************************
    if (GetMsg(MSG_WIFI_SCAN_ERR))
    {
        ThreadDelete(&pMainThread, &WiFiThread);
        TaskArg.MainMenu.MenuID = MAINMENU_ID_NET_RADIO;
        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        
        return RETURN_FAIL;
    }    

	/* dynamic add the AP list item...*/
	if (GetScanEventOpen())
	{
	    if (GetMsg(MSG_WIFI_SCAN_OK))
	    {   
			gSysConfig.WiFiConfig.ssid[0] = 0xFE;
			gSysConfig.WiFiConfig.ssid[1] = 0;
			gSysConfig.WiFiConfig.password[0] = 0;
			gSysConfig.WiFiConfig.password_len = 0;
			printf("fle\n");
			
			/* try to get last success save ssid and password to auto connect. */
			WiFiReadAPPWFrmRecord();
			
	        if (rk_check_scan_ssid(gSysConfig.WiFiConfig.ssid) >= 0)
	        {
				printf("auto:");

				gSysConfig.WiFiConfig.password[gSysConfig.WiFiConfig.password_len] = 0;
				SetScanEventOpen(FALSE);//set dirver do not receive the scan result event.
	            SendMsg(MSG_WIFI_CONNECT);
				
	            /* display the router connecting waiting interface..*/
		        /*DispPictureWithIDNumAndXY(IMG_ID_MP4_B, 0, 48);
				LCD_NFDispStringAt(5, 58, L"Connecting AP...");*/

				/*do not update the ssid list even if the item number have changed..*/
				CurScanAPNum = rk_get_scan_ssid_total();
	        }
			else
			{
				/*send command scan second..*/
		    	SendMsg(MSG_WIFI_SCAN_START); 
			}
	    }
		
		if (CurScanAPNum != rk_get_scan_ssid_total())
		{
			CurScanAPNum = rk_get_scan_ssid_total();
	        SSIDItemInfoInit();
	    	WiFiSSIDUpProc(ALL_BROITEM_UPDATE_AP);
			SendMsg(MSG_BROW_DIS_ALL_ITEM);
		}
	}

    if (GetMsg(MSG_WIFI_KEYBOARD_DESTORY))
    {
        SendMsg(MSG_WIFI_CONNECT);
		
        //显示路由器连接等待界面
        /*DispPictureWithIDNumAndXY(IMG_ID_MP4_B, 0, 48);
		LCD_NFDispStringAt(5, 58, L"Connecting AP...");*/
    }
	

    //*************************************************
    //  连接失败，进入路由器列表
    //  连接成功，进入音乐播放界面
    //*************************************************
    if (GetMsg(MSG_WIFI_CONN_FAILURE))
    {
        WiFiDelFailAPPWFrmRecord();		
        ThreadDelete(&pMainThread, &WiFiThread);
        TaskArg.MainMenu.MenuID = MAINMENU_ID_NET_RADIO;
        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
        return RETURN_FAIL;
    }
	
    if (GetMsg(MSG_WIFI_CONN_SUCCESS))
    {
		/* save or update the connect success ssid and password.*/
		WiFiSaveOkAPPWToRecord();

        /*DispPictureWithIDNumAndXY(IMG_ID_MP4_B, 0, 48);
		LCD_NFDispStringAt(5, 58, L"Get IP OK!");
        ThreadDelete(&pMainThread, &WiFiMusicThread);*/

        //TaskSwitch(TASK_ID_WIFI_PLAY, NULL);
        SendMsg(MSG_WIFI_TCP_CONNECTING);
	 if(SoftApStart) {
	 	SendMsg(MSG_QPW_ENABLE);
		SoftApStart = 0;
	 }
	 ConnectNumber = 0;
	 //create_active_command_data(temp, C_UPLOAD);
	 eth_service(0);
	 upload_connect(C_UPLOAD);
        return RETURN_FAIL;
    }

    if(GetMsg(MSG_WIFI_SAVE_INFO)) {
	 WiFiSaveOkAPPWToRecord();
    }
    
	return RETURN_OK;
}


/*
--------------------------------------------------------------------------------
  Function name :UINT32 WiFiAPWinKey(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_WIFI_AP_WIN_CODE_
UINT32 WiFiAPWinKey(void)
{
	int ret;
    uint32  WifiApKeyVal;
    uint32 RetVal = RETURN_OK;
    TASK_ARG TaskArg;
	
    WifiApKeyVal =  GetKeyVal();
    switch (WifiApKeyVal) 
    {
        case KEY_VAL_MENU_SHORT_UP:
			printf("Key:");
			SetScanEventOpen(FALSE);//set dirver do not receive the scan result event.
			if (rk_get_scan_ssid_total() > 0)
			{
	            rk_get_scan_ssid(gSysConfig.WiFiConfig.ssid, WIFIAPTreeInf.CurId, 0);
				printf("ssid=%s\n", gSysConfig.WiFiConfig.ssid);

				ClearMsg(MSG_WIFI_NO_ENCODE);
				ret = rk_get_encode_type(WIFIAPTreeInf.CurId);
				if ((0 == WiFiCheckAPPWFrmRecord()) || (0 == ret))
				{
					if (0 == ret)
					{
						SendMsg(MSG_WIFI_NO_ENCODE);
					}
	                SendMsg(MSG_WIFI_KEYBOARD_DESTORY);
	            }
	            else
	            {
	                WinCreat(&WiFiAPWin, &WiFiKeyBoardWin, NULL);
				}
			}
            break;
			
        case KEY_VAL_ESC_SHORT_UP:
            ThreadDelete(&pMainThread, &WiFiThread);
	        TaskArg.MainMenu.MenuID = MAINMENU_ID_NET_RADIO;
	        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
            RetVal = RETURN_FAIL;
			break;

        case KEY_VAL_FFW_SHORT_UP:
		case KEY_VAL_UP_SHORT_UP:
            WIFIAPTreeInf.PreCounter = WIFIAPTreeInf.KeyCounter;
            if (WIFIAPTreeInf.CurId > 0) 
            {
                WIFIAPTreeInf.CurId--;

                if (WIFIAPTreeInf.KeyCounter == 0) 
                {
                    WIFIAPTreeInf.KeyCounter = MAX_AP_NUM_LCD_DISP - 1;
                    WiFiSSIDUpProc(ALL_BROITEM_UPDATE_AP);                    
                    SendMsg(MSG_BROW_DIS_ALL_ITEM);
                }
                else
                {
                    WIFIAPTreeInf.KeyCounter--;
                    SendMsg(MSG_BROW_DIS_SELE_ITEM);
                }
            }
            else
            {
                WIFIAPTreeInf.CurId = WIFIAPTreeInf.DispTotalItem - 1;
                if (WIFIAPTreeInf.DispTotalItem > MAX_AP_NUM_LCD_DISP)
                {
                    if (WIFIAPTreeInf.DispTotalItem % MAX_AP_NUM_LCD_DISP)
                    {
                        WIFIAPTreeInf.KeyCounter = WIFIAPTreeInf.DispTotalItem % MAX_AP_NUM_LCD_DISP - 1;
                    }
                    else
                    {
                        WIFIAPTreeInf.KeyCounter =  MAX_AP_NUM_LCD_DISP - 1;
                    }
                    WiFiSSIDUpProc(ALL_BROITEM_UPDATE_AP);
                    SendMsg(MSG_BROW_DIS_ALL_ITEM);
                }
                else
                {
                    WIFIAPTreeInf.KeyCounter = WIFIAPTreeInf.DispTotalItem - 1;
                    SendMsg(MSG_BROW_DIS_SELE_ITEM);
                }
            }
            break;
            
        case KEY_VAL_FFD_SHORT_UP:
		case KEY_VAL_DOWN_SHORT_UP:
            WIFIAPTreeInf.PreCounter = WIFIAPTreeInf.KeyCounter;
            if(WIFIAPTreeInf.CurId < (WIFIAPTreeInf.DispTotalItem - 1))
            {
                WIFIAPTreeInf.CurId++; 
                if(WIFIAPTreeInf.KeyCounter >= MAX_AP_NUM_LCD_DISP - 1) 
                {
                    WIFIAPTreeInf.KeyCounter = 0;
                    WiFiSSIDUpProc(ALL_BROITEM_UPDATE_AP);                    
                    SendMsg(MSG_BROW_DIS_ALL_ITEM);//display all screen
                }
                else
                {
                    WIFIAPTreeInf.KeyCounter++;
                    SendMsg(MSG_BROW_DIS_SELE_ITEM);
                }  
            }
            else
            {
                WIFIAPTreeInf.CurId = 0;
                WIFIAPTreeInf.KeyCounter = 0;
                if(WIFIAPTreeInf.DispTotalItem > MAX_AP_NUM_LCD_DISP)
                {
                    WiFiSSIDUpProc(ALL_BROITEM_UPDATE_AP);
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
    return (RetVal);
}

_ATTR_WIFI_AP_WIN_CODE_
void WiFiAPWinDisplay(void)
{

}

/*
********************************************************************************
*
*                         End of WiFiAPWin.c
*
********************************************************************************
*/

