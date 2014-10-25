/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   ModuleOverlay.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _MODULEOVERLAY_H_
#define _MODULEOVERLAY_H_

#undef  EXT
#ifdef _IN_MODULEOVERLAY_
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
#define _ATTR_OVERLAY_CODE_         __attribute__((section("SysCode")))
#define _ATTR_OVERLAY_DATA_         __attribute__((section("SysData")))
#define _ATTR_OVERLAY_BSS_          __attribute__((section("SysBss"),zero_init))

#define _ATTR_OVERLAY_INIT_CODE_    __attribute__((section("SysCode")))
#define _ATTR_OVERLAY_INIT_DATA_    __attribute__((section("SysData")))
#define _ATTR_OVERLAY_INIT_BSS_     __attribute__((section("SysBss"),zero_init))

//ModuleLoad Type define
#define MODULE_OVERLAY_CODE         0x01
#define MODULE_OVERLAY_DATA         0x02
#define MODULE_OVERLAY_BSS          0x04
#define MODULE_OVERLAY_ALL          (MODULE_OVERLAY_CODE | MODULE_OVERLAY_DATA | MODULE_OVERLAY_BSS)

#define MAX_CHAR_ENC_SUPPORT    5
#define MAX_FONT_SUPPORT        10
#define MAX_MENU_SUPPORT        10
#define MAX_UI_SUPPORT          10

/*
*-------------------------------------------------------------------------------
*  
*                           Struct Address
*  
*-------------------------------------------------------------------------------
*/
#define MAX_CHAR_ENC_SUPPORT    5
#define MAX_FONT_SUPPORT        10
#define MAX_MENU_SUPPORT        10
#define MAX_UI_SUPPORT          10
    
typedef enum _ENUM_CODE
{
    USB_BOOT,
    NAND_BOOT1,
    NAND_BOOT2,
    SYS_CODE,
    CODE_NUM
}ENUM_CODE;
    
typedef enum _ENUM_FONT
{
    FONT12,
    FONT16,
    FONT_NUM
}ENUM_FONT;
    
typedef enum _ENUM_CHAR_ENC
{
    GBK,
    CHAR_ENC_NUM
};
    
typedef enum _ENUM_UI
{
    UI1,
    UI_NUM
}ENUM_UI;

          
/***********模块信息表*********/
typedef struct _MODULE_INFO_T
{
    unsigned int ModuleOffset;
    unsigned int ModuleLength;
} MODULE_INFO_T;


/***********代码信息***********/
typedef struct _CODE_INFO
{
    unsigned int    CodeNum;
    MODULE_INFO_T   CodeInfoTbl[CODE_NUM];
} CODE_INFO;
    
/*************字体**************/
typedef struct _FONT_INFO
{
    unsigned int    FontNum;
    MODULE_INFO_T   FontInfoTbl[MAX_FONT_SUPPORT];
} FONT_INFO;
    
/***********字符编码************/
typedef struct _CHAR_ENC_INFO
{
    unsigned int    CharEncNum;
    MODULE_INFO_T   CharEncInfoTbl[MAX_CHAR_ENC_SUPPORT];
} CHAR_ENC_INFO;
    
/************菜单***************/
typedef struct _MENU_INFO
{
    unsigned int    MenuNum;
    MODULE_INFO_T   MenuInfoTbl[MAX_MENU_SUPPORT];
} MENU_INFO;
    
/*************UI****************/
typedef struct _UI_INFO
{
    unsigned int    UiNum;
    MODULE_INFO_T   UiInfoTbl[MAX_UI_SUPPORT];
} UI_INFO;
    
/***********固件头结构**********/
typedef struct _FIRMWARE_HEADER
{
    unsigned short  Year;               // BCD
    unsigned short  Date;               // BCD
    unsigned short  MasterVer;          // BCD
    unsigned short  SlaveVer;           // BCD
    unsigned short  SmallVer;           // BCD
    unsigned short  LoaderVer;          // BCD
    unsigned char   MachineID[4];       // 机器ID,与固件匹配
    unsigned char   VendorName[32];     // 厂商名
    unsigned char   ModelName[32];      // 型号名
    CODE_INFO       CodeInfo;           // 代码信息，包括UsbBoot, NandBoot1, NandBoot2, SysCode  
    FONT_INFO       FontInfo;           // 字库信息
    CHAR_ENC_INFO   CharEncInfo;        // 字符编码信息，目前仅包含GBK编码
    MENU_INFO       MenuInfo;           // 菜单资源信息
    UI_INFO         UiInfo;             // UI资源信息
    unsigned char   Reserved[88];
    unsigned long   FwEndOffset;
    unsigned char   FwSign[8];          // 固件标记，"RkNanoFw"
}FIRMWARE_HEADER, *PFIRMWARE_HEADER;

/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/
//

/*
*-------------------------------------------------------------------------------
*
*                            Function Declaration
*
*-------------------------------------------------------------------------------
*/

uint32 GetResourceAddress(uint32 *CodeLogicAddress,
                          uint32 *GBKLogicAddress,
                          uint32 *MenuLogicAddress0,uint32 *MenuLogicAddress1);
                          
uint32 GetFirmwareVersion(uint16 *Year, 
                          UINT16  *Date, 
                          UINT16 *MasterVer,
                          UINT16 *SlaveVer,
                          UINT16 *SmallVer);

void ModuleOverlayLoadData(uint32 SrcAddr, uint32 DestAddr, uint32 Length);
uint32 ModuleOverlay(uint32 ModuleNum, uint8 Type);
uint32 ReadModuleData(uint32 ModuleNum, uint8 *pBuf, uint32 Offset, uint32 Length);
uint32 ModuleOverlayExt(uint32 ModuleNum, uint8 Type);
/*
********************************************************************************
*
*                         End of ModuleOverlay.h
*
********************************************************************************
*/
#endif
