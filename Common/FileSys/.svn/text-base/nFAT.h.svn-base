/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   nFAT.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _NFAT_H_
#define _NFAT_H_

#undef  EXT
#ifdef  IN_NFAT
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
--------------------------------------------------------------------------------
  
                           Struct define
  
--------------------------------------------------------------------------------
*/


/***************************************************************************
文件系统FS_CHCHE
***************************************************************************/
typedef struct _FS_CHCHE
{
	uint16	Buf[256];   //数据BUF
	uint8	Flag;		//读写标记
	uint32	Sec;        //扇区地址
}FS_CHCHE, *pFS_CHCHE;

/*
--------------------------------------------------------------------------------
  
                           Variable Define
  
--------------------------------------------------------------------------------
*/
DRAM_FAT EXT        FS_CHCHE    gFatCache;

#if 0//def ENCODE
DRAM_ENCODE EXT     uint32      Fat2CacheSec;           //当前的Cache FAT2表扇区号
DRAM_ENCODE EXT     uint8       Fat2Buf[512];           //FAT2表CACHE
DRAM_ENCODE EXT     uint8       Fat2WriteBuf[4096];     //FAT2K BUF
DRAM_ENCODE EXT     uint32      FatCacheSecOffset;
DRAM_ENCODE EXT     uint32      FatCacheClusIndex;
DRAM_ENCODE EXT     uint16      FatCacheBufOffset;
#endif

/*
*-------------------------------------------------------------------------------
*
*                            Function Declaration
*
*-------------------------------------------------------------------------------
*/
#ifdef ENCODE
extern void FATDelClusChain(uint32 Index);
#endif

extern uint32 FATGetNextClus(uint32 Index, uint32 Count);
extern bool FATReadSector(uint32 LBA, void *pData);
extern bool FATWriteSector(uint32 LBA, void *pData);

#ifdef ENCODE
extern void FATSetNextClus(uint32 Index, uint32 Next);
extern uint32 FATAddClus(uint32 Index);
extern uint32 GetFreeMemFast(uint8 memory);

#endif

extern uint32 GetTotalMem(uint8 memory);
extern uint32 GetFreeMem(uint8 memory);
extern void UpdataFreeMemFast(int32 size);
extern void UpdataFreeMem(int32 size);


/*
********************************************************************************
*
*                         End of nFAT.h
*
********************************************************************************
*/
#endif
