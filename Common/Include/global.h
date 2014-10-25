/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   Main.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#undef  EXT
#ifdef _IN_MAIN_
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
//section define
#define _ATTR_SYS_CODE_         __attribute__((section("SysCode")))
#define _ATTR_SYS_DATA_         __attribute__((section("SysData")))
#define _ATTR_SYS_BSS_          __attribute__((section("SysBss"),zero_init))

#define _ATTR_SYS_INIT_CODE_    __attribute__((section("SysInitCode")))
#define _ATTR_SYS_INIT_DATA_    __attribute__((section("SysInitData")))
#define _ATTR_SYS_INIT_BSS_     __attribute__((section("SysInitBss"),zero_init))

/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/
//system keeping information for music module

#ifdef _WIFI_
#define     ETHNAMELEN			32
#define     PASSWORD_LEN		32 //MAX_PASSWORD_LEN
#define     NETURLLEN			192

#define		WIFI_APPW_BLKSIZE	512		//Context is (1 + 100) * 5 = 505 512 is enough.
#define		WIFI_APPW_ITEMNUM   5   	//item total number.
#define		WIFI_APPW_ITEM_OK	0xAA	//0xAA Item vaild if not is null item.

typedef __packed struct _WIFICONFIG
{
	char ssid[ETHNAMELEN];
	char password[PASSWORD_LEN];
	int  password_len;
}WIFI_CONFIG;
#endif

//
#define ERROR_LEN_NO_ENOUGH   -1
#define ERROR_FRAME_HEADER    -2
#define ERROR_FRAME_TAIL      -3
#define ERROR_NOT_SUPPORT     -4

#define RETURN_OK             0
#define NOT_ANSWER            1
#define NOT_RESPONSE          2

#define GET_OK                0
#define GET_NONE              1
//

#define MOUDLE_3LED
#define TIME_MAX                 8640000
#define TIME_BASE                100
#define TIME_M_BASE            6000
#define MINUTE_TO_SECOND 60
#define HOUR_TO_SECOND           3600

//system setting parameter structure definition.
typedef __packed struct _SYSCONFIG
{
    uint16 FirmwareYear;
    uint16 FirmwareDate;
    uint16 MasterVersion;
    uint16 SlaveVersion;
    uint16 SmallVersion;
    
    uint8  Memory;                  //storage media options.
    UINT8  SysLanguage;             //current system language environment.
    uint8  ImageIndex;

    UINT8  OutputVolume;            //system volume
    UINT8  BLmode;                  //backlight display mode.
    UINT8  BLevel;                  //backlight level
    UINT8  BLtime;                  //backlight time.
    UINT8  LockTime;                //lock keyboard time
    UINT8  ShutTime;                //auto close time
    UINT8  SDEnable;                //sd card select
    UINT8  FMEnable;                //FM setting menu judge.
    UINT8  KeyNum;                  //it is used for 5.6.7 keys judgement.

#ifdef _CDROM_
    UINT8  bShowCdrom;
#endif

#ifdef _WIFI_
	UINT32 XmlSavStartPage;	//the save xml anlyse info frist page position.
	UINT32 WiFiFileNo;
	UINT32 WiFiFileId;
    WIFI_CONFIG WiFiConfig;
#endif

    UINT8   Softin;   //shift bit number,it is used for volume attenuate

    uint32  eFuseSaveData;

} SYSCONFIG,*PSYSCONFIG;

/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/
//Firmware Info
_ATTR_SYS_BSS_  EXT uint32      Font12LogicAddress;
_ATTR_SYS_BSS_  EXT uint32      Font16LogicAddress;

_ATTR_SYS_BSS_  EXT uint32      IDBlockByteAddress;
_ATTR_SYS_BSS_  EXT uint32      CodeLogicAddress;
_ATTR_SYS_BSS_  EXT uint32      FontLogicAddress;
_ATTR_SYS_BSS_  EXT uint32      GBKLogicAddress;
_ATTR_SYS_BSS_  EXT uint32      MenuLogicAddress;
_ATTR_SYS_BSS_  EXT uint32      ImageLogicAddress;

#ifdef _CDROM_ 
_ATTR_SYS_BSS_  EXT BOOL        bShowCdrom;
#endif

_ATTR_SYS_BSS_  EXT uint32      DCDCLastLevel;
_ATTR_SYS_BSS_  EXT uint32      LDOLastLevel;
_ATTR_SYS_BSS_  EXT uint32      EfuseReadData;
_ATTR_SYS_BSS_  EXT uint32      SysTickCounter;  //system clock counter,add one every 10 ms.
_ATTR_SYS_BSS_  EXT SYSCONFIG   gSysConfig;
_ATTR_SYS_BSS_  EXT BOOL        IsSetmenu;
_ATTR_SYS_BSS_  EXT BOOL        BroswerFlag;//the flag that is distinguish return to media or explorer
_ATTR_SYS_BSS_  EXT UINT16      GlobalFilenum;

_ATTR_SYS_BSS_  EXT INT8       OutputVolOffset;//it is use to modify volume after eq setting.
_ATTR_SYS_BSS_  EXT UINT8       MaxShuffleAllCount;      
#define  MemorySelect           gSysConfig.Memory
#define  gbTextFileNum          gSysConfig.TextConfig.FileNum
#define  gbAudioFileNum         gSysConfig.MusicConfig.FileNum
#define  gbVideoFileNum         gSysConfig.VideoConfig.FileNum
#define  gbPicFileNum           gSysConfig.PicConfig.FileNum //an 4.27 picture mode.
#ifdef _IN_MAIN_
_ATTR_SYS_DATA_ EXT char        FileOpenStringR[]      = "R";
_ATTR_SYS_DATA_ EXT char        ALLFileExtString[]     = "*";
_ATTR_SYS_DATA_ EXT char        AudioFileExtString[]   = "MP1MP2MP3WMAWAVAPEFLAAACM4A";
_ATTR_SYS_DATA_ EXT char        TextFileExtString[]    = "TXTLRC";
_ATTR_SYS_DATA_ EXT char        VideoFileExtString[]   = "AVI";
_ATTR_SYS_DATA_ EXT char        PictureFileExtString[] = "JPGJPEBMP";//JPE it is used judge jpeg picture.
#else
_ATTR_SYS_DATA_ EXT char        FileOpenStringR[];
_ATTR_SYS_DATA_     EXT char    ALLFileExtString[];
_ATTR_SYS_DATA_ EXT char        AudioFileExtString[];
_ATTR_SYS_DATA_ EXT char        TextFileExtString[];
_ATTR_SYS_DATA_ EXT char        VideoFileExtString[];
_ATTR_SYS_DATA_ EXT char        PictureFileExtString[];

#endif

//set language information group of real support in menu.this information is the offset of language in explorer.
_ATTR_SYS_BSS_  EXT UINT8       SetMenuLanguageInfo[LANGUAGE_MAX_COUNT];
_ATTR_SYS_BSS_  EXT UINT8       DefaultLanguage;//default language item number it is used to locate default language cursor in menu display.
_ATTR_SYS_BSS_  EXT UINT8       LanguageNum;//set the real suppot language number.
_ATTR_SYS_BSS_  EXT UINT8       DefaultLanguageID;
_ATTR_SYS_BSS_  EXT UINT8       ConnectNumber;
_ATTR_SYS_BSS_  EXT UINT32     QpwNumber;

#ifdef MOUDLE_3LED
_ATTR_SYS_BSS_  EXT UINT8       ColorSave[0x03];
_ATTR_SYS_BSS_  EXT UINT8       FreqSave[0x03];
// color action
_ATTR_SYS_BSS_  EXT UINT8       ColorMark;
_ATTR_SYS_BSS_  EXT UINT32     StartColor;
_ATTR_SYS_BSS_  EXT UINT32     EndColor;
_ATTR_SYS_BSS_  EXT UINT32     ColorSpace;
_ATTR_SYS_BSS_  EXT UINT32     MarkTime;
// timing mode
_ATTR_SYS_BSS_  EXT UINT8       TimingMark;
_ATTR_SYS_BSS_  EXT UINT32     CurrentTimer;
_ATTR_SYS_BSS_  EXT UINT32     OpenTimer;
_ATTR_SYS_BSS_  EXT UINT32     CloseTimer;
#endif

/*
********************************************************************************
*
*                         End of Main.h
*
********************************************************************************
*/
#endif
