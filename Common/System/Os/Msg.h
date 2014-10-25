/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   Msg.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _MSG_H_
#define _MSG_H_

#undef  EXT
#ifdef _IN_TASK_
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

/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/
typedef enum
{
    //system message
    MSG_KEYSCAN_PROCESSED = (UINT32)0x00,
    MSG_KEY_VOL_STATE,
    MSG_KEY_VOL_STATE_UPDATE,
    MSG_KEY_VOL_SHOWSTATE,
    MSG_KEYSCAN_HOLD,

    MSG_USB_INSERT,                                                 //USB insert status
    MSG_SDCARD_EJECT,                                               //SD card insert status

    MSG_SYS_RESUME,                                                 //backlight restore
    MSG_POWER_DOWN,                                                 //shutdown message
    MSG_CHARGE_ENABLE,
    MSG_CHARGE_START,
    MSG_NEED_PAINT_ALL,
    
    //mian menu UI message
    MSG_MENU_DISPLAY_ALL,                                          //main menu backgroud pictrue display
    MSG_MENU_DISPLAY_ITEM,
    MSG_MENU_DISPLAY_BATT,                                          //main menu display battery
    MSG_MENU_DISPLAY_HOLD,                                          //main menu hold picture display
    
    //audio playing  UI message
    MSG_MUSIC_DISPFLAG_CURRENT_TIME,                                    //Current Time
    MSG_MUSIC_DISPFLAG_STATUS,                                          //PLAY/PAUSE
    
    //recording UI message
    MSG_RECORDWIN_DISPLAY_ALL,
    MSG_RECORDWIN_DISPFLAG_REFRESH,                                    
    MSG_RECORDWIN_DISPFLAG_BACKGROUND,                              //record base map
    MSG_RECORDWIN_DISPFLAG_HOLD,                                    //Hold
    MSG_RECORDWIN_DISPFLAG_BATT,                                    //Battery
    MSG_RECORDWIN_DISPFLAG_VOL,                                     //VOl
    MSG_RECORDWIN_DISPFLAG_RECQUALITY,                              //record quality(High,Nor)
    MSG_RECORDWIN_DISPFLAG_TIME,                                    //record time(current record time and total left time)
    MSG_RECORDWIN_DISPFLAG_TOTALTIME,
    MSG_RECORDWIN_DISPFLAG_FILENAME,                                //playing recording file name
    MSG_RECORDWIN_DISPFLAG_BITRATE,                                 //playing recording baud rate
    MSG_RECORDWIN_DISPFLAG_SAMPLERATE,                              //playing recording sampling
    
    //recording backgroud message
    MSG_RECORD_STATUS_PREPARE,                                      //recording ready
    MSG_RECORD_STATUS_PLAY,                                         //playing recording
    MSG_RECORD_STATUS_PAUSE,                                        //recording pause
    MSG_RECORD_STATUS_SAVE ,                                        //save recording file
    MSG_RECORD_STATUS_TIME_UPDATE,                                  //update time
    MSG_RECORD_STATUS_GETFILE_FAIL,                                 //fail to display record file name
    MSG_RECORD_STATUS_CREATEFILE_FAIL,                             // fail to create file
    MSG_RECORD_STATUS_DISK_FULL,                                    //disk full(before and in recording)               
    MSG_RECORD_STATUS_WRITEFILE_FAIL,                               //write file fail when recording
    MSG_RECORD_STATUS_CLOSEFILE_FAIL,                               //fail to close recording file 
                           
    MSG_RECORD_INITUI,                                              //initial recording UI
    MSG_RECORD_SERVICESTART,                                        //start recording backgroud service.
    MSG_RECORD_GETRECORDTYPE,                                       //select recording type
    MSG_RECORD_ENCODESTART,                                         //start coding
    MSG_RECORD_WAVENCODESTOP,                                       //stop coding
    MSG_RECORD_TYPE,                                                //get coding type from backgroud.
    MSG_RECORD_GETINFO,                                             //start recording from bakcgroud
    MSG_RECORD_INIT,                                                //start coding in backgroud
    MSG_RECORD_STARTE,                                              //codeing start,write file head, open timer interrupt.
    MSG_RECORD_PAUSE,                                               //codeing pause
    MSG_RECORD_RESUME,                                              //turn suspend status to recording status,resume recording
    MSG_RECORD_STOP,                                                //stop record,write file,close file
    MSG_RECORD_WRITEFILE,                                           //write file
    
    //FM  UI message
    MSG_RADIOWIN_DISPLAY_ALL,
    MSG_RADIOWIN_DISPLAY_NULL,                                      //
    MSG_RADIOWIN_DISPLAY_BACK,                                      //FM backgroud image
    MSG_RADIOWIN_DISPLAY_BATT,                                      //Battery
    MSG_RADIOWIN_DISPLAY_HOLD,                                      //
    MSG_RADIOWIN_DISPLAY_FREQ,                                      //FM main window digit frequency display
    MSG_RADIOWIN_DISPLAY_GUAGE,                                     //FM main window progress bar
    MSG_RADIOWIN_DISPLAY_SENS,                                      //
    MSG_RADIOWIN_DISPLAY_STEREO,                                    //STEREO
    MSG_RADIOWIN_DISPLAY_CH,                                        //CH
    MSG_RADIOWIN_DISPLAY_STEREO_UPDATA,
    MSG_RADIOWIN_DISPLAY_VOL,                                       //VOl
    MSG_RADIOWIN_DISPLAY_REGION,                                    //area
	MSG_RADIOWIN_DISPLAY_SETTING,
	MSG_RADIOWIN_DISPLAY_BASICFREQ,

    //FM child windows  UI message
    MSG_RADIOWIN_DISPLAY_SUB_ALL,
    MSG_RADIOWIN_DISPLAY_SUB_BK,                                    //
    MSG_RADIOWIN_DISPLAY_SUB_SEL,                                   //
    MSG_RADIOWIN_DISPLAY_SUB_LIST_BK,                               //
    MSG_RADIOWIN_DISPLAY_SUB_LIST_SEL,                              //
    MSG_RADIOWIN_DISPLAY_SUB_NULL,                                  //
    MSG_RADIOSUBWIN_DISPLAY_FLAG,                                   //
    MSG_RADIOSUBWIN_DIALOG,                                         //

    //FM backgroud service message.
    MSG_FM_INIT,
    MSG_FM_START,
    MSG_FM_HANDSEARCH,
    MSG_FM_AUTOSEARCH,
    MSG_FM_STEROSWITCH,
    MSG_FM_STEPFREQ,
    MSG_FM_STEPSTATION,
    MSG_FM_GETSTEROSTATUS,
    MSG_FM_DEINIT,
    MSG_RADIO_VOLUMESET,                                           //FM volum
    MSG_RADIO_VOLUMRESET,

    //setting menu message
    MSG_SETDIALOG_FLAG,                                             //set menu dialog
    MSG_USEREQ_SET_INITFLAG ,                                       //user EQ setting initial
    MSG_USEREQ_SET_TITLE,                                           //user EQ setting title display
    MSG_USEREQ_SET_DISPFLAG,                                        //user EQ setting display flag
    MSG_SERVICE_EQ_UPDATE,                                          //user EQ setting update
    MSG_PRODUCTINFO,                                                //check produce information
    MSG_FIRMWAREUPGRADE,                                            //frimware update
    MSG_ENTER_LOADER_USB,                                            //enter loader usb
    MSG_MES_FIRMWAREUPGRADE,                                        //frimware update check
    MSG_SYSDEFAULTVALSET,                                           //restore system default setting
    MSG_SYS_SET_DISPLAY_NULL ,                                      //do not refresh                        
    MSG_SYS_SET_DISPLAY_BATT  ,                                     //                  
    MSG_SYS_SET_DISPLAY_HOLD,                                       //   
    MSG_SYS_SET_DISPLAY,                                            //setting refresh screen
    MSG_SYS_SET_DISPLAY_ALL,                                        //refresh full screen.
    MSG_SYS_SET_DISPLAY_SCR,                                        //fresh scroll item
    MSG_SYS_SET_DISPLAY_TITLE,                                      //title fresh message
    MSG_SYS_SET_DISP_REFRESH_CUR_PREV_ITEM ,                        //cursor scroll to up
    MSG_SYS_SET_DISP_REFRESH_CUR_NEXT_ITEM ,                        //cursor scroll to dowm
    MSG_SYS_SET_INFO_SCROLL,
	MSG_SYS_SET_MUSIC_HOLD,                                          //EQ ylz++
    //USB & charge message
    MSG_USB_DISPLAY_CHARGE,                                         //display usb charge

    //DialogBox
    MSG_DIALOG_DISPLAY_ALL,                                         //
    MSG_DIALOG_DISPLAY_BUTTON,                                      //
    MSG_DIALOG_KEY_OK,                                              //
    MSG_DIALOG_KEY_CANCEL,                                          //

    //MessageBox
    MSG_MESSAGE_DISPLAY_ALL,
    MSG_MESSAGEBOX_DESTROY,                                         //messagebox destory

    //Hold
    MSG_HOLD_DISPLAY_ALL,
    MSG_HOLDDIALOG_DESTROY,

    //LowPower
    MSG_LOWPOWER_DISPLAY_ALL,
    MSG_LOWPOWER_DESTROY,
    
    MSG_AUDIO_GETBITRATE,                                           //
    MSG_AUDIO_GETFILENAME,                                          // 
    MSG_AUDIO_GETSAMPLERATE,                                        //
    MSG_AUDIO_GETLENGTH,                                            //
    MSG_AUDIO_GETPOSI,                                              //
    MSG_AUDIO_UPDATETIME,                                           //
    MSG_AUDIO_REPORTERR,                                            //
    MSG_AUDIO_NEXTFILE,                                             //
    MSG_AUDIO_UNMUTE,                                               //
    MSG_AUDIO_EXIT,
    MSG_SERVICE_MUSIC_MODE_UPDATE,  //music play mode update.it will be sent when setting menu,music receive.
    MSG_SERVICE_MUSIC_ORDER_UPDATE, //music order play update,it will be sent when setting menu,music receive.

    MSG_XVID_INIT,
    MSG_XVID_OPEN,
    MSG_XVID_START,
    MSG_XVID_DECODE,
    MSG_XVID_CLOSE,
    MSG_XVID_STOP,
    MSG_XVID_PAUSE,                                                
    MSG_XVID_RESUME,                                               
    MSG_XVID_NEXTFILE,
    MSG_XVID_VOLUMESET,
    MSG_XVID_FFD,
    MSG_XVID_FFW,
    MSG_XVID_NOFILE,
    MSG_XVID_DISPFLAG_STATUS,                                          //PLAY/PAUSE
    MSG_XVID_DISPLAY_ALL,
    MSG_VIDEO_DISPFLAG_BATTERY,
    MSG_VIDEO_DISPFLAG_CURRENT_TIME,
    MSG_VIDEO_DISPFLAG_TOTAL_TIME,
    MSG_VIDEO_DISPFLAG_FILENUM,
    MSG_VIDEO_DISPFLAG_SCHED,
    MSG_VIDEO_DISPFLAG_VOL,
    MSG_VIDEO_CARD_CHECK,
    MSG_VIDEO_FILENOTSUPPORT,
    MSG_VIDEO_EXIT_BROWSER,
    MSG_VIDEO_DISPLAY_HOLD,
    MSG_VIDEO_DISPLAY_UNHOLD,
	MSG_VIDEO_VERTICAL_DISPLAY,  //ylz++

    //ebook message
    MSG_BOOK_DISPLAY_ALL,
    MSG_BOOK_DISPLAY_HOLD,
    MSG_BOOK_DISPLAY_BATT,
    MSG_BOOK_DISPLAY_BACKGROUND,
    MSG_BOOK_DISPLAY_TITLE,
    MSG_BOOK_DISPLAY_TITLE_TOTAIPAGE,
    MSG_BOOK_DISPLAY_TITLE_CURRENTPAGE,
    MSG_BOOK_DISPLAY_TITLE_SCROLL,
    MSG_BOOK_DISPLAY_UPDATE,
    MSG_BOOK_DISPLAY_PLAYSTATUS,
	MSG_BOOK_DISPLAY_WIN_FALG,   //YLZ++
    
    MSG_BOOK_MARK_DISPLAY_ALL,
    MSG_BOOK_MARK_DISPLAY_HOLD,
    MSG_BOOK_MARK_DISPLAY_BATT,
    MSG_BOOK_MARK_DISPLAY_BACKGROUND,
    MSG_BOOK_MARK_DISPLAY_TITLE,
    MSG_BOOK_MARK_DISPLAY_ITEM,
    MSG_BOOK_MARK_UPDATE,
    MSG_BOOK_MARK_SCROLL_UPDATE,

    //explorer ui   
    MSG_BROW_DIS_ALL,               
    MSG_BROW_DIS_BAR,              
    MSG_BROW_DIS_ALL_ITEM,          
    MSG_BROW_DIS_SELE_ITEM,         
    MSG_BROW_DIS_BATT,              
    MSG_BROW_DIS_HOLD,
    MSG_BROW_DIS_SCLLO_ITEM,  
    BROWSER_DISPFLAG_SCROLL_FILENAME,
    MSG_BROW_DELETE_FILE,
    MSG_BROW_DELETEING_FILE,//it is deleting file.
    MSG_BROW_FROM_MAINMENU,
    MSG_BROW_CHECK_FILE,
    
    //media libary message
    MSG_MEDIAWIN_DISPLAY_ALL,
    MSG_MEDIAWIN_ALL_ITEM,
    MSG_MEDIAWIN_FRESH_ITEM,

    MSG_MEDIABRO_DISPLAY_ALL,
    MSG_MEDIABRO_ALL_ITEM,
    MSG_MEDIABRO_FRESH_ITEM,
    MSG_MEDIABRO_DISPFLAG_SCROLL_FILENAME,
    MSG_MEDIABRO_NOFIND_FILE,

    MSG_MEDIABROSUB_DISPLAY_ALL,
    MSG_MEDIABROSUB_ALL_ITEM,
    MSG_MEDIABROSUB_FRESH_ITEM,

    MSG_MEDIAFAVOSUB_DISPLAY_ALL,
    MSG_MEDIAFAVOSUB_ALL_ITEM,
    MSG_MEDIAFAVOSUB_FRESH_ITEM,
    MSG_MEDIA_BREAKPOINT_PLAY,
    MSG_MEDIA_SCROLL_PAINT,
    MSG_MEDIALIB_SCROLL_PAINT,
    MSG_MEDIA_TO_BROWSER,//media libary enter explorer message,it is use to disguish whether exit to media or not.
    MSG_MEDIALIB_UPDATA,
    MSG_FILE_IN_FAVO,
    MSG_MEDIAFAVO_DELALL, //clear favourite

    MSG_SYS_REBOOT,

    MSG_FLASH_MEM_UPDATE,
    MSG_SDCARD_MEM_UPDATE,

    //wifi
    MSG_WIFI_SDIO_ERR,
	MSG_WIFI_SCAN_START,
    MSG_WIFI_SCAN,
    MSG_WIFI_SCAN_ERR, 
    MSG_WIFI_SCAN_OK,
	MSG_WIFI_KEYBOARD_DESTORY,
	MSG_WIFI_CONNECT,
	MSG_WIFI_CONN_FAILURE, 
	MSG_WIFI_CONN_SUCCESS,
	MSG_WIFI_TCP_CONNECTING,
	MSG_WIFI_SEEK_ERROR,
    MSG_WIFI_PASSWORD_DISP,
	MSG_WIFI_KEYBOARD_DISP_ALL, 
	MSG_WIFI_KEYBOARD_UPDATA_FOUCS,
    MSG_WIFI_GO_STATIONS_WIN,
    MSG_WIFI_STATIONS_WIN_DESTORY,
    MSG_WIFI_DIS_ALL,
    MSG_WIFI_AUDIO_OPENERROR,
    MSG_WIFI_AUDIO_FILE_ERROR,
    MSG_WIFI_MUSIC_SWITCH,
    MSG_WIFI_PLAY_FINSH,
    MSG_WIFI_APPW_JUMP,
    MSG_WIFI_NO_ENCODE,
		
		MSG_QPW_ENABLE,
		MSG_WIFI_RECONNECT,
		MSG_WIFI_SAVE_INFO,

    MSG_ID_MAX	//max message number
}MSG_ID;

/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/
_ATTR_OS_BSS_  EXT UINT32  Msg[(MSG_ID_MAX / 32) + 1];

/*
*-------------------------------------------------------------------------------
*  
*                           Functon Declaration
*  
*-------------------------------------------------------------------------------
*/
EXT void SendMsg(MSG_ID MsgId);
EXT BOOL GetMsg(MSG_ID MsgId);
EXT void ClearMsg(MSG_ID MsgId);
EXT BOOL CheckMsg(MSG_ID MsgId);

/*
********************************************************************************
*
*                         End of Msg.h
*
********************************************************************************
*/
#endif
