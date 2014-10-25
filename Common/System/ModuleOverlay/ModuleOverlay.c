/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：  ModuleOverlay.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    
********************************************************************************
*/
#define _IN_MODULEOVERLAY_

#include "SysInclude.h"
#include "FsInclude.h"
#include "SysReservedOperation.h"

uint32 MDReadData(MEMDEV_ID DevID, uint32 offset, uint32 len, void *buf);
/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get firmware resource address information
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OVERLAY_CODE_
uint32 GetFirmwareVersion(uint16 *Year, 
                          UINT16 *Date, 
                          UINT16 *MasterVer,
                          UINT16 *SlaveVer,
                          UINT16 *SmallVer)
{
    uint8  FlashBuf[512];
    
    FIRMWARE_HEADER *pFWHead = (FIRMWARE_HEADER *)FlashBuf;

    MDReadData(SysDiskID, 0, 512,FlashBuf);
    
    *Year      = pFWHead->Year;
    *Date      = pFWHead->Date;
    
    *MasterVer = pFWHead->MasterVer;
    *SlaveVer  = pFWHead->SlaveVer;
    *SmallVer  = pFWHead->SmallVer;
   
    return (OK);
}

_ATTR_OVERLAY_CODE_
uint32 MDReadData(MEMDEV_ID DevID, uint32 offset, uint32 len, void *buf)
{
    uint32 sectorNum;
    uint32 ret = OK; 
    uint32 sectorOffset,ImaskTemp;
    uint8   FlashBuf[512];
    uint8 * pData = (uint8*)buf;
    uint32 RawOffset = 0;
    
    ImaskTemp = UserIsrDisable();  
    sectorNum = offset >> 9;
    sectorOffset = offset & 511;

    #if (NAND_DRIVER==1)    //只有nand 有备份固件
    if (DevID == SysDiskID)
    {
        if(sectorNum < SysProgRawDiskCapacity)
        {
            sectorNum += SysProgRawDiskCapacity; //读备份区
            RawOffset = SysProgRawDiskCapacity;  //原始固件偏移
        }
    }
    #endif
    
	while (len)
	{
		if (sectorOffset != 0 || len < 512)
		{
			uint16 i;
			uint16 count;
			if(OK != MDRead(DevID, sectorNum, 1, FlashBuf))
			{
                if (DevID == SysDiskID)
                {
                    if (OK != MDRead(DevID, sectorNum-RawOffset, 1, FlashBuf))
                       ret = ERROR;
                }
                else
                {
                    ret = ERROR;
                }
			}
            count = 512-sectorOffset;
            count = (count>len)? len : count;
			for (i=sectorOffset; i<count+sectorOffset; i++)
			{
				*pData++=FlashBuf[i];
			}
			len-=count;
			sectorOffset=0;
		}
		else
		{
			if(OK != MDRead(DevID, sectorNum, 1, pData))
			{
			    if (DevID == SysDiskID)
			    {
    			    if (OK != MDRead(DevID, sectorNum-RawOffset, 1, pData))
    			    {
                        ret = ERROR;
    			    }
			    }
                else
                {
                    ret = ERROR;
                }
			}
			pData +=  512;
			len -=  512;
		}
        sectorNum++;
	}
	UserIsrEnable(ImaskTemp);

    return ret;
}



/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get resource address information of firmware.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OVERLAY_CODE_
void ModuleOverlayLoadData(uint32 SrcAddr, uint32 DestAddr, uint32 Length)
{
    uint32 i, j;
    uint32 StartSector, EndSector;
    uint32 StartOffset, EndOffset;
    uint8  FlashBuf[512];
    uint8  *pFirmWareDest;
    
    pFirmWareDest = (uint8*)DestAddr;
    
    StartSector = (SrcAddr / 512);
    EndSector   = ((SrcAddr + Length + 511) / 512);
    
    StartOffset = SrcAddr % 512;
    EndOffset   = (SrcAddr + Length) % 512;
    EndOffset = 512;
    
    for (i = StartSector; i < EndSector; i++)
    {
		MDReadData(SysDiskID, i * 512 , 512, FlashBuf);

        if (i == (EndSector - 1))
        {
            if (((SrcAddr + Length) % 512) != 0)
            {
                EndOffset = (SrcAddr + Length) % 512;
            }
        }
        for (j = StartOffset; j < EndOffset; j++)
        {
            *pFirmWareDest++ = FlashBuf[j];
        }
        
        StartOffset = 0;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetResourceAddress()
  Author        : ZHengYongzhi
  Description   : get resource address information of firmware.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OVERLAY_CODE_
uint32 ModuleOverlayExt(uint32 ModuleNum, uint8 Type)
{
    uint32 i, Len;
    uint32 CodeInfoAddr;
    uint32 LoadStartBase;
    uint32 LoadBase;
    uint32 ImageBase;
    uint32 ImageLength;
    uint8  FlashBuf[512];
    uint8  *pBss;
	uint8  tempIsr;
    CODE_INFO_T Module;
    
    FIRMWARE_INFO_T *pFirmwareModuleInfo;
    
    tempIsr = UserIsrDisable();
    
    //先读取固件相对起始地址
    MDReadData(SysDiskID, CodeLogicAddress, 512, FlashBuf);
    pFirmwareModuleInfo = (FIRMWARE_INFO_T *)FlashBuf;
    LoadStartBase = pFirmwareModuleInfo -> LoadStartBase;

    //读取模块信息CODE_INFO_T
    CodeInfoAddr  = CodeLogicAddress + sizeof(pFirmwareModuleInfo -> LoadStartBase);
    CodeInfoAddr  = CodeInfoAddr + sizeof(pFirmwareModuleInfo -> ModuleInfo.ModuleNum) + ModuleNum * sizeof(CODE_INFO_T);
    MDReadData(SysDiskID, CodeInfoAddr, sizeof(CODE_INFO_T), FlashBuf);
    Module = *(CODE_INFO_T *)FlashBuf;
    
    //读取代码、数据；清除BSS段
    if (Type & MODULE_OVERLAY_CODE)
    {
        LoadBase    = Module.CodeLoadBase - LoadStartBase + CodeLogicAddress;
        ImageBase   = Module.CodeImageBase;
        ImageLength = Module.CodeImageLength;
        
        ModuleOverlayLoadData(LoadBase, ImageBase, ImageLength);
    }
    
    if (Type & MODULE_OVERLAY_DATA)
    {
        LoadBase    = Module.DataLoadBase - LoadStartBase + CodeLogicAddress;
        ImageBase   = Module.DataImageBase;
        ImageLength = Module.DataImageLength;
        
        ModuleOverlayLoadData(LoadBase, ImageBase, ImageLength);
    }
    
    if (Type & MODULE_OVERLAY_BSS)
    {
        ImageBase   =  Module.BssImageBase;
        ImageLength =  Module.BssImageLength;
        
        pBss = (uint8 *)ImageBase;
        for (i = 0; i < ImageLength; i++) 
        {
            *pBss++ = 0;
        }
    }

    UserIsrEnable(tempIsr);
    
    return (OK);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 ReadModuleData(uint32 ModuleNum, uint8 *pBuf, uint32 Offset, uint32 Length)
  Author        : ZHengYongzhi
  Description   : get resource address information of firmware.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OVERLAY_CODE_
uint32 ReadModuleData(uint32 ModuleNum, uint8 *pBuf, uint32 Offset, uint32 Length)
{
    uint16 i, Len;
    uint32 CodeInfoAddr;
    uint32 LoadStartBase;
    uint32 LoadBase;
    uint32 ImageBase;
    uint32 ImageLength;
    uint8  FlashBuf[512];
    uint8  *pBss;
	uint8  tempIsr;
    
    CODE_INFO_T     Module;
    FIRMWARE_INFO_T *pFirmwareModuleInfo;
    
    tempIsr = UserIsrDisable();
    
    //先读取固件相对起始地址
    MDReadData(SysDiskID, CodeLogicAddress, 512, FlashBuf);
    pFirmwareModuleInfo = (FIRMWARE_INFO_T *)FlashBuf;
    LoadStartBase = pFirmwareModuleInfo -> LoadStartBase;

    //读取模块信息CODE_INFO_T
    CodeInfoAddr  = CodeLogicAddress + sizeof(pFirmwareModuleInfo -> LoadStartBase);
    CodeInfoAddr  = CodeInfoAddr + sizeof(pFirmwareModuleInfo -> ModuleInfo.ModuleNum) + ModuleNum * sizeof(CODE_INFO_T);
    MDReadData(SysDiskID, CodeInfoAddr, sizeof(CODE_INFO_T), FlashBuf);
    Module = *(CODE_INFO_T *)FlashBuf;

    //读取想要的数据
    LoadBase    = Module.DataLoadBase - LoadStartBase + CodeLogicAddress;
    ModuleOverlayLoadData(LoadBase + Offset, (uint32)pBuf, Length);

    UserIsrEnable(tempIsr);
    
    return (OK);
}


/*
--------------------------------------------------------------------------------
  Function name : uint32 GetSysReservedNextPage(uint32 block)
  Author        : ZHengYongzhi
  Description   : search a page that has not been worte,it start form specified block address add a offset 
                  inside of the block,this offset is left for forture use,if found,return this page.
  Input         : 
  Return        : Page address.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYSRESERVED_OP_CODE_
uint32 GetSysReservedNextPage(uint32 block)
{
    uint32 i, Page;
    uint32 SysSecPerPage;
    uint32 SysPagePerBlock;
    uint8  FlashBuf[512];
    uint32 LBA;
    MEMDEV_INFO DevInfo;
   
    if (OK != MDGetInfo(DataDiskID, &DevInfo))
    {
        return 0;
    }
    
    Page = 0;
    #ifdef _NANDFLASH_
    {
        SysSecPerPage   = DevInfo.PageSize;
        SysPagePerBlock = DevInfo.BlockSize/SysSecPerPage;
        
        LBA = DevInfo.BlockSize * block;

        //从Block的起始Page + 偏移地址 开始搜索空的Page
        for(Page = 0; Page < SysPagePerBlock; Page++)
        {
            MDRead(DataDiskID, LBA + Page * SysSecPerPage, 1, FlashBuf);
            for (i = 0; i < 16; i++)
            {
                if (FlashBuf[i] != 0xff)
                {
                    break;
                }
            }
            if (i == 16)
            {
                break;
            }
        }
    }
    #endif
    
    return Page;
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetSysReservedNextPage(uint32 block)
  Author        : ZHengYongzhi
  Description   : search a page that has not been worte,it start form specified block address add a offset 
                  inside of the block,this offset is left for forture use,if found,return this page.
  Input         : 
  Return        : Page地址

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYSRESERVED_OP_CODE_
void ReadReservedData(uint32 block , uint8 *Buf, int16 Length)
{
    uint32 i, Page, PageLen, Sectors;
    uint32 SysAreaSecPerPage;
    uint32 SysAreaPagePerBlock;
    uint32 tempIsr;
    uint32 LBA;
    MEMDEV_INFO DevInfo;
    
    if (OK != MDGetInfo(DataDiskID, &DevInfo))
        return;
    
    tempIsr = UserIsrDisable();

    SysAreaSecPerPage   = DevInfo.PageSize;
    SysAreaPagePerBlock = DevInfo.BlockSize/ SysAreaSecPerPage;
    
    PageLen = (Length + (SysAreaSecPerPage<<9) - 1) / (SysAreaSecPerPage<<9);
    Page = GetSysReservedNextPage(block);

    if (Page >= PageLen)
    {
        Page -= PageLen;
    }

    LBA = DevInfo.BlockSize * block;
    LBA += Page * SysAreaSecPerPage;
    Sectors = (Length + 512 - 1) / 512;

    MDRead(DataDiskID, LBA, Sectors, Buf);

    UserIsrEnable(tempIsr);
}
 
/*
--------------------------------------------------------------------------------
  Function name : void WriteReservedData(uint32 block , uint8 *Buf, uint16 Length)
  Author        : ZHengYongzhi
  Description   : write system reserved area,find a page that has not been wrote in specified page,
                  write data to this page.
  Input         : 
  Return        : Page address.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYSRESERVED_OP_CODE_
void WriteReservedData(uint32 block , uint8 *Buf, uint16 Length)
{
    uint32 Page, PageLen;
    uint32 SysAreaSecPerPage;
    uint32 SysAreaPagePerBlock;
    uint32 tempIsr;
    uint32 LBA;
    MEMDEV_INFO DevInfo;

    if (OK != MDGetInfo(DataDiskID, &DevInfo))
        return;
    
    tempIsr = UserIsrDisable();

    SysAreaSecPerPage   = DevInfo.PageSize;
    SysAreaPagePerBlock = DevInfo.BlockSize / SysAreaSecPerPage;
    
    PageLen = (Length + (SysAreaSecPerPage<<9) - 1) / (SysAreaSecPerPage<<9);
    Page = GetSysReservedNextPage(block);
    
    if (SysAreaPagePerBlock - Page < PageLen)
    {
        Page = 0;
    }

    LBA = DevInfo.BlockSize * block;
    LBA += Page * SysAreaSecPerPage;
    MDWrite(DataDiskID, LBA , PageLen * SysAreaSecPerPage, Buf);
    
    UserIsrEnable(tempIsr);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 GetSysReservedNextPage(uint32 block)
  Author        : ZHengYongzhi
  Description   : 
  Input         : 
  Return        : Page Addr

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
void ReadResourceData(uint32 Addr, uint8 *pData, uint16 length)
{
    MDReadData(SysDiskID, Addr,length,pData);
}


/*
********************************************************************************
*
*                         End of ModuleOverlay.c
*
********************************************************************************
*/

