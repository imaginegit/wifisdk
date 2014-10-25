/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: FTL.h
Desc: 

Author: chenfen@rock-chip.com
Date: 11-11-17
Notes:

$Log: $
 *
 *
*/

#ifndef _FTL_H
#define _FTL_H

/*-------------------------------- Includes ----------------------------------*/


/*------------------------------ Global Defines ------------------------------*/

#define     FTL_OK                  0
#define     FTL_ERROR               -1


#define     DISK_NAND_CODE          0
#define     DISK_NAND_DATA          1
#define     DISK_NAND_USER          2
#define     DISK_NAND_USER2         3
#define     DISK_NAND_TOTAL         0xff

#define     SYS_PROTECT    
#define     FLASH_PROT_MAGIC        0x444e414e  //NAND


#define     _ATTR_FTL_INIT_CODE_      __attribute__((section("FlashInitCode")))
#define     _ATTR_FTL_INIT_DATA_      __attribute__((section("FlashData")))
#define     _ATTR_FTL_INIT_BSS_       __attribute__((section("FlashBss"),zero_init))

#define     _ATTR_FTL_CODE_           __attribute__((section("FlashCode")))
#define     _ATTR_FTL_DATA_           __attribute__((section("FlashData")))
#define     _ATTR_FTL_BSS_            __attribute__((section("FlashBss"), zero_init))
#if 0
#define     _ATTR_FTL_CACHE_          __attribute__((section("ftlcache"),zero_init))
#define     _ATTR_FTL_CACHE2_         __attribute__((section("ftlcache2"),zero_init))
#else
#define     _ATTR_FTL_CACHE_          __attribute__((section("ftlcache")))
#define     _ATTR_FTL_CACHE2_         __attribute__((section("ftlcache2")))
#endif
#define     _ATTR_FTL_REMAP_TBL_      __attribute__((section("ftlRemapTbl"),zero_init))

#define     _ATTR_FTL_WRITE_CODE_      __attribute__((section("FlashWriteCode")))
#define     _ATTR_FTL_WRITE_DATA_      __attribute__((section("FlashData")))
#define     _ATTR_FTL_WRITE_BSS_       __attribute__((section("FlashBss"),zero_init))

/*------------------------------ Global Typedefs -----------------------------*/


/*----------------------------- External Variables ---------------------------*/


/*------------------------- Global Function Prototypes -----------------------*/



extern void FtlRefreshHook(void);
extern uint32 FtlProgErrHook(uint32 SecAddr, void *DataBuf, uint16 *pSpare, uint16 nSec);

extern void FtlClose(void);
extern void FtlWrBlkFree(void);
extern uint32 MDGetFtlInfo(uint8 LUN, pMEMDEV_INFO pDevInfo);

extern uint16 FtlGetPageSize(void);

#ifndef IN_LOADER
extern uint32 MDFtlWrite(uint8 LUN, uint32 Index, uint32 nSec, void *pbuf);
#else

extern int32 FtlGetStatus(void);
extern int32 FtlGetCurEraseBlk(void);
extern int32 FtlGetBlkNum(void);
extern int32 FtlLowFormatEx(void);
extern uint32 MDFtlWrite(uint32 SecAdrr, uint32 nSec, void *pBuf);
extern uint32 MDFtlRead(uint32 SecAdrr, uint32 nSec, void *pBuf);

#endif

/***************************************************************************
函数描述:设置数据盘大小，在FTLInit之后马上调用
入口参数:数据盘大小
出口参数:
调用函数:
***************************************************************************/
extern void FtlSetDataDskSize(int nMB);

/***************************************************************************
函数描述:设置用户盘2大小，在FTLInit之后马上调用
入口参数:用户盘2大小
出口参数:
调用函数:
***************************************************************************/
extern void FtlSetUsrDsk2Size(int nMB);


/***************************************************************************
函数描述:FTL和FLASH初始化
入口参数:无
出口参数:
        0=正常返回 
        1=映射表出错, 被强制低格
        2=flash初始化失败,可能是硬件错误或要进行固件升级
        其它值:内部错误, 该驱动不可使用
说    明:上电只需调用一次初始化即可
***************************************************************************/
extern int32 FTLInit(void);

/***************************************************************************
函数描述:获取FLASH磁盘容量
入口参数:
        DISK_NAND_CODE:固件盘
        DISK_NAND_DATA:系统参数盘
        DISK_NAND_USER:用户盘
出口参数:磁盘总扇区数
说    明:
***************************************************************************/
extern uint32 FtlGetCapacity(uint8 LUN);

/***************************************************************************
函数描述:读扇区接口
入口参数:LUN=逻辑分区号, Index=起始扇区地址, buf=数据缓冲区, nSec=扇区数
出口参数:0=读取正确; 非0=读取出错, 数据不可用
调用函数:
说    明:
***************************************************************************/
extern int32 FtlRead(uint8 LUN, uint32 index, uint32 nSec, void *buf);

/***************************************************************************
函数描述:写扇区接口
入口参数:LUN=逻辑分区号, Index=起始扇区地址, buf=数据缓冲区, nSec=扇区数
出口参数:0=正确写入; 非0=写操作失败, 数据没有被正确写入
调用函数:
说    明:
***************************************************************************/
extern int32 FtlWrite(uint8 LUN, uint32 index, uint32 nSec, void *buf);

#endif
