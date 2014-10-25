/*
********************************************************************************
*                   Copyright (c) 2009,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   ModuleInfoTab.h
* 
* Description:  define module struct information
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2009-02-06         1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _MODULEINFO_H
#define _MODULEINFO_H

#undef  EXT
#ifdef _IN_MODULEINFO_
#define EXT
#else
#define EXT extern
#endif

/*
*-------------------------------------------------------------------------------
*  
*                           Module Info
*  
*-------------------------------------------------------------------------------
*/
//Module Section ID Define
typedef enum 
{
    MODULE_ID_SYSCODE = (UINT32)0,	//00
    MODULE_ID_SYSINIT,				//01
    MODULE_ID_FLASH_PROG,			//02
    MODULE_ID_LCD_1,				//03
    MODULE_ID_LCD_2,				//04
    MODULE_ID_USB,					//05
    MODULE_ID_MAINMENU,				//06
    MODULE_ID_MP3_DECODE,			//07
    MODULE_ID_MP3_INIT,				//08
    MODULE_ID_MP3_COMMON,			//09
    MODULE_ID_FILE_ENCODE,			//10
    MODULE_ID_FILE_FIND,			//11
    MODULE_ID_CHARGE_WIN,			//12
    MODUEL_ID_WIFI_AP_WIN,			//13
    MODUEL_ID_WIFI_PLAY_WIN,		//14
	MODUEL_ID_WIFI_SYS,				//15
    MODULE_ID_WIFI_SCAN,			//16		
    MODULE_ID_WIFI_LWIP,			//17
    MODULE_ID_WIFI_DHCP,			//18
    MODULE_ID_NETFILE_FIND,			//19
    MODULE_ID_WIFI_WPA,				//20
    MODULE_ID_WIFI_AUDIOC,			//21
    MODULE_ID_XML,					//22
    MAX_MODULE_NUM
    
} MODULE_ID;

//Code Info Table
typedef	struct _CODE_INFO_T 
{
    UINT32 CodeLoadBase;
    UINT32 CodeImageBase;
    UINT32 CodeImageLength;
    
    UINT32 DataLoadBase;
    UINT32 DataImageBase;
    UINT32 DataImageLength;
    
    UINT32 BssImageBase;
    UINT32 BssImageLength;
    
} CODE_INFO_T;

//Module Info
typedef	struct _CODE_INFO_TABLE 
{
    UINT32                  ModuleNum;
    #ifndef _JTAG_DEBUG_
    CODE_INFO_T             Module[MAX_MODULE_NUM];
    #endif
    
} CODE_INFO_TABLE;

/*
*-------------------------------------------------------------------------------
*  
*                           FM Driver Info
*  
*-------------------------------------------------------------------------------
*/
//FM Driver Index
typedef enum 
{
    MODULE_ID_FM_1    = (UINT32)0,   
    MODULE_ID_FM_2,
    
    FM_DRIVER_MAX
    
} FM_DRIVER_ID;

//FM driver
typedef	struct _FM_DRIVER_TABLE_T 
{    
    void   (*Tuner_SetInitArea)(UINT8 );
    void   (*Tuner_SetFrequency)(UINT32 , UINT8 , BOOL ,UINT16 );
    void   (*Tuner_SetStereo)(BOOL );
    void   (*Tuner_SetVolume)(uint8);
    
    void   (*Tuner_PowerOffDeinit)(void);
    UINT16 (*Tuner_SearchByHand)(UINT16 , UINT32 * );
    void   (*Tuner_PowerDown)(void);
    void   (*Tuner_MuteControl)(bool );
    BOOLEAN (*GetStereoStatus)(void);
  
} FM_DRIVER_TABLE_T;

typedef	struct _FM_DRIVER_INFO_T 
{
    //FM function list
    UINT32 FMDriverIndex;
    UINT32 FMDriverMax;
    FM_DRIVER_TABLE_T FmDriver[FM_DRIVER_MAX];
    
} FM_DRIVER_INFO_T;

/*
*-------------------------------------------------------------------------------
*  
*                           LCD Driver Info
*  
*-------------------------------------------------------------------------------
*/
//LCD Driver Index
typedef enum 
{
    LCD_DRIVER_LCD_1 = (UINT32)0,
    LCD_DRIVER_LCD_2,
    LCD_DRIVER_MAX
    
} LCD_DRIVER_ID;

//Lcd driver
typedef	struct _LCD_DRIVER_TABLE_T 
{ 
    //Lcd function list
    void(*pLcd_WriteRAM_Prepare)(void);
    void(*pLcd_Init)(void);
    void(*pLcd_SendData)(UINT16);
    void(*pLcd_SetWindow)(UINT16 ,UINT16 ,UINT16,INT16);
    void(*pLcd_Clear)(UINT16);
    void(*pLcd_DMATranfer)(UINT8,UINT8,UINT8,UINT8,UINT16*);
    void(*pLcd_Standby)(void);
    void(*pLcd_WakeUp)(void);
    void(*pLcd_MP4_Init)(void);
    void(*pLcd_MP4_DeInit)(void);
    void(*pLcd_SetPixel)(UINT16 x, UINT16 y, UINT16 data);
    void(*pLcd_Buffer_Display1)(unsigned  int x0,unsigned int y0,unsigned int x1,unsigned int y1);
    void(*pLcd_ClrSrc)(void);
    void(*pLcd_ClrRect)(int x0, int y0, int x1, int y1);
    void(*LCDDEV_FillRect)(int x0, int y0, int x1, int y1);
     // UINT32 reserve;
    
} LCD_DRIVER_TABLE_T;

typedef	struct _LCD_DRIVER_INFO_T 
{
    //Lcd driver function
    UINT32 LcdDriverIndex;
    UINT32 LcdDriverMax;
    LCD_DRIVER_TABLE_T LcdDriver[LCD_DRIVER_MAX];
    
} LCD_DRIVER_INFO_T;

/*
*-------------------------------------------------------------------------------
*  
*                           Default Para
*  
*-------------------------------------------------------------------------------
*/
//System Default Para
typedef	struct _SYSTEM_DEFAULT_PARA_T 
{
    //system default setting
    UINT32 SDEnable;
    UINT32 FMEnable;
    UINT32 KeyNumIndex;
    
    UINT32 gLanguage;
    UINT32 Volume;
    
    UINT32 BLMode;
    UINT32 BLevel;
    UINT32 BLtime;
    
    UINT32 SettingPowerOffTime;
    UINT32 IdlePowerOffTime;
    
    //Music default setting
    UINT32 MusicRepMode;
    UINT32 MusicPlayOrder;
    UINT32 MusicEqSel;
    UINT32 MusicPlayFxSel;
    
    //Video default setting
    
    //Radio default setting
    UINT32 FMArea;
    UINT32 FMStereo;
    
    //Record default setting
    
    //Picture default setting
    UINT32 PicAutoPlayTime;
    
    //Text default setting
    UINT32 TextAutoPlayTime;

    //Image ID
    uint32 ImageIndex;

    //mutiple language
    uint32 MenuMaxItem;
    uint32 LanguageTotle;   //TOTAL_LANAUAGE_NUM,LANGUAGE_MAX_COUNT
    uint32 LanguagesEnableTbl[64];//support 256 languange
    
} SYSTEM_DEFAULT_PARA_T;

/*
*-------------------------------------------------------------------------------
*  
*                           Module Info
*  
*-------------------------------------------------------------------------------
*/
//Firmware Info
typedef	struct _FIRMWARE_INFO_T 
{
    UINT32                  LoadStartBase;
    
    CODE_INFO_TABLE         ModuleInfo;

    SYSTEM_DEFAULT_PARA_T   SysDefaultPara;

    FM_DRIVER_INFO_T        FMDriverTable;

    LCD_DRIVER_INFO_T       LCDDriverTable;
    
} FIRMWARE_INFO_T;

/*
********************************************************************************
*
*                         End of ModuleInfoTab.h
*
********************************************************************************
*/
#endif

