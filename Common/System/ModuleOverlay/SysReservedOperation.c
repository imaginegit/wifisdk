/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º  SysReservedOperation.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    
********************************************************************************
*/
#define _IN_SYSRESERVED_OP_

#include "SysInclude.h"
#include "FsInclude.h"
#include "SysReservedOperation.h"

#define SYSCONFIGSIZE 	1024

/*
--------------------------------------------------------------------------------
  Function name : uint32 FindIDBlock(void)
  Author        : ZHengYongzhi
  Description   : find IDBlock
                  
  Input         : 
  Return        : IDBlock OR 0xffffffff

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void LoadSysInformation(void)
{
    uint32 i,j=0;
    uint32 Len;
    uint32 SystemDefaultParaAddr;
    uint8  TempBuffer[SYSCONFIGSIZE];
    uint8  FlashBuf[512];
    
    uint16 FirmwareYear;
    uint16 FirmwareDate;
    uint16 MasterVersion;
    uint16 SlaveVersion;
    uint16 SmallVersion;
    uint32 MenuLogicAddress0, MenuLogicAddress1;
    
    SYSCONFIG             *pSysConfig;
    FIRMWARE_INFO_T       *pFirmwareModuleInfo;
    SYSTEM_DEFAULT_PARA_T *pSystemDefaultPara;
    FIRMWARE_HEADER       *pFWHead;

    ////////////////////////////////////////////////////////////////////////////
    //read resource module address.
    pFWHead = (FIRMWARE_HEADER *)FlashBuf;
    MDRead(SysDiskID, 0, 1, FlashBuf);        
    
    CodeLogicAddress   = pFWHead->CodeInfo.CodeInfoTbl[SYS_CODE].ModuleOffset;
    GBKLogicAddress    = pFWHead->CharEncInfo.CharEncInfoTbl[GBK].ModuleOffset ;
    MenuLogicAddress0  = pFWHead->MenuInfo.MenuInfoTbl[0].ModuleOffset;
    MenuLogicAddress1  = pFWHead->MenuInfo.MenuInfoTbl[1].ModuleOffset;
    
    ////////////////////////////////////////////////////////////////////////////
    //read system default parameter.
    MDReadData(SysDiskID, CodeLogicAddress, 512, FlashBuf);
    pFirmwareModuleInfo = (FIRMWARE_INFO_T *)FlashBuf;

    SystemDefaultParaAddr  = CodeLogicAddress + sizeof(pFirmwareModuleInfo->LoadStartBase);
    SystemDefaultParaAddr += sizeof(pFirmwareModuleInfo->ModuleInfo.ModuleNum);
    SystemDefaultParaAddr += pFirmwareModuleInfo->ModuleInfo.ModuleNum * sizeof(CODE_INFO_T);
    MDReadData(SysDiskID, SystemDefaultParaAddr, sizeof(SYSTEM_DEFAULT_PARA_T), FlashBuf);
    pSystemDefaultPara = (SYSTEM_DEFAULT_PARA_T *)FlashBuf;
    
    ///////////////////////////////////////////////////////////////////////
    //read system setting parameters and judge version number.
    ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
    pSysConfig = (SYSCONFIG *)TempBuffer;
    ReadReservedData(SYSDATA_BLK, TempBuffer, SYSCONFIGSIZE);

    GetFirmwareVersion(&FirmwareYear,
                       &FirmwareDate,
                       &MasterVersion,
                       &SlaveVersion,
                       &SmallVersion);

   if (/*(pSysConfig->FirmwareYear  != FirmwareYear)  || 
       (pSysConfig->FirmwareDate  != FirmwareDate)  ||*/ 
       (pSysConfig->MasterVersion != MasterVersion) || 
       (pSysConfig->SlaveVersion  != SlaveVersion)  || 
       (pSysConfig->SmallVersion  != SmallVersion))
    {
		/* use the buffer clear the flash space. */
		printf("WIFI_AP_PW_BLK clear.\n");
		memset(TempBuffer, 0x00, WIFI_APPW_BLKSIZE);
		WriteReservedData(WIFI_AP_PW_BLK, TempBuffer, WIFI_APPW_BLKSIZE);
		file_create_init();
		
        memset(TempBuffer, 0xff, SYSCONFIGSIZE);
    }
    
    gSysConfig.FirmwareYear  = FirmwareYear;
    gSysConfig.FirmwareDate  = FirmwareDate;
    gSysConfig.MasterVersion = MasterVersion;
    gSysConfig.SlaveVersion  = SlaveVersion;
    gSysConfig.SmallVersion  = SmallVersion;

    gSysConfig.SDEnable = (uint8)pSystemDefaultPara->SDEnable; //sd card founction select.
    gSysConfig.FMEnable = (uint8)pSystemDefaultPara->FMEnable; //FM resource select. 
    gSysConfig.KeyNum   = (uint8)pSystemDefaultPara->KeyNumIndex;
    if(gSysConfig.FMEnable)//menu resource address select.
    {
        MenuLogicAddress = MenuLogicAddress0;//FM resource address.
    }
    else
    {
        MenuLogicAddress = MenuLogicAddress1;//FM menu resource address.
    }
	MenuLogicAddress = MenuLogicAddress0;
	printf("MenuLogicAddress=%08x %08x %08x.\n", MenuLogicAddress, MenuLogicAddress0, MenuLogicAddress1);
    ///////////////////////////////////////////////////////////////////////
    //recover system setting parameter
    
    //storage media
    gSysConfig.Memory = pSysConfig->Memory;
    if (gSysConfig.Memory > CARD) 
    {
        gSysConfig.Memory = FLASH0;
    }

    #ifdef _CDROM_
    gSysConfig.bShowCdrom= pSysConfig->bShowCdrom & 0x01;
    #endif

    //system language.
    gSysConfig.SysLanguage = pSysConfig->SysLanguage;
/*    if (gSysConfig.SysLanguage > (TOTAL_LANAUAGE_NUM - 1)) 
    {
        gSysConfig.SysLanguage = (uint8)pSystemDefaultPara->gLanguage;
    }*/
    
    for(i=0;i<TOTAL_LANAUAGE_NUM;i++)
    {   
        if(pSystemDefaultPara->LanguagesEnableTbl[i])
        {
            SetMenuLanguageInfo[j]= i;
            if(i == gSysConfig.SysLanguage)
                DefaultLanguage = j;
            j++;
        }
    }
    LanguageNum = j;
    DefaultLanguageID = DefaultLanguage;
    gSysConfig.SysLanguage = SetMenuLanguageInfo[DefaultLanguage];
    
    //picture resource.
    gSysConfig.ImageIndex = pSysConfig->ImageIndex;
/*    if (gSysConfig.ImageIndex >= (IMAGE_ID_MAX)) 
    {
        gSysConfig.ImageIndex = (uint8)pSystemDefaultPara->ImageIndex;
    }*/
    
    //system volume
    gSysConfig.OutputVolume = pSysConfig->OutputVolume;
    if (gSysConfig.OutputVolume > 32) 
    {
        gSysConfig.OutputVolume = (uint8)pSystemDefaultPara->Volume;
    }
    
    //backlight mode
    gSysConfig.BLmode = pSysConfig->BLmode;
    if (gSysConfig.BLmode > 1) 
    {
        gSysConfig.BLmode = (uint8)pSystemDefaultPara->BLMode;
    }
    
    //backlight level
    gSysConfig.BLevel = pSysConfig->BLevel;
    if (gSysConfig.BLevel > 5) 
    {
        gSysConfig.BLevel = (uint8)pSystemDefaultPara->BLevel;
    }
    
    //backlight time
    gSysConfig.BLtime = pSysConfig->BLtime;
    if (gSysConfig.BLtime > 5) 
    {
        gSysConfig.BLtime = (uint8)pSystemDefaultPara->BLtime;
    }
    
    //automatic shutdown time
    gSysConfig.ShutTime = pSysConfig->ShutTime;
    if (gSysConfig.ShutTime > 5) 
    {
        gSysConfig.ShutTime = (uint8)pSystemDefaultPara->SettingPowerOffTime;
    }    
    //efuse
    gSysConfig.eFuseSaveData = pSysConfig->eFuseSaveData;

#if 0//def _WIFI_
	#if 1
	memcpy(&gSysConfig.WiFiConfig, &pSysConfig->WiFiConfig, sizeof(WIFI_CONFIG));
	strcpy(gSysConfig.WiFiConfig.ssid, "rockchip");
	strcpy(gSysConfig.WiFiConfig.password, "1234567890123");
	gSysConfig.WiFiConfig.password_len = 13;
	#endif
	printf("wifi:%d %s:%s\n", gSysConfig.WiFiConfig.password_len, gSysConfig.WiFiConfig.ssid, gSysConfig.WiFiConfig.password);
#endif
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 FindIDBlock(void)
  Author        : ZHengYongzhi
  Description   : find IDBlock
                  
  Input         : 
  Return        : IDBlock or 0xffffffff

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYSRESERVED_OP_CODE_
void SaveSysInformation(void)
{
    uint8  TempBuffer[SYSCONFIGSIZE];
    
    memset(TempBuffer, 0xff, SYSCONFIGSIZE);
    memcpy(TempBuffer, (uint8*)&gSysConfig, sizeof(SYSCONFIG));//dgl SYSCONFIG == 210byte

    ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
    
    WriteReservedData(SYSDATA_BLK, TempBuffer, SYSCONFIGSIZE);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get frimware resource address information
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OVERLAY_CODE_
uint32 ModuleOverlay(uint32 ModuleNum, uint8 Type)
{
    #ifndef _JTAG_DEBUG_
    ModuleOverlayExt(ModuleNum, Type);    
    #endif
    
    return (OK);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get frimware resource address information
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void FlashTotalMemory(void)
{
    //Get flash0 memory Info
    flash0TotalMem = GetTotalMem(FLASH0) / 2;//* 512 /1024;//Sector turn to Kbyte
    if(flash0TotalMem > 0x3e800000) 
    {
        flash0TotalMem -= 1000000;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get frimware resource address information
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void FlashFreeMemory(void)
{
    if (GetMsg(MSG_FLASH_MEM_UPDATE))
    {
        sysFreeMemeryFlash0 = GetFreeMem(FLASH0);
    }
}

/*
********************************************************************************
*
*                         End of SysReservedOperation.c
*
********************************************************************************
*/

