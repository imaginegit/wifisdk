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
��������:���������̴�С����FTLInit֮�����ϵ���
��ڲ���:�����̴�С
���ڲ���:
���ú���:
***************************************************************************/
extern void FtlSetDataDskSize(int nMB);

/***************************************************************************
��������:�����û���2��С����FTLInit֮�����ϵ���
��ڲ���:�û���2��С
���ڲ���:
���ú���:
***************************************************************************/
extern void FtlSetUsrDsk2Size(int nMB);


/***************************************************************************
��������:FTL��FLASH��ʼ��
��ڲ���:��
���ڲ���:
        0=�������� 
        1=ӳ������, ��ǿ�Ƶ͸�
        2=flash��ʼ��ʧ��,������Ӳ�������Ҫ���й̼�����
        ����ֵ:�ڲ�����, ����������ʹ��
˵    ��:�ϵ�ֻ�����һ�γ�ʼ������
***************************************************************************/
extern int32 FTLInit(void);

/***************************************************************************
��������:��ȡFLASH��������
��ڲ���:
        DISK_NAND_CODE:�̼���
        DISK_NAND_DATA:ϵͳ������
        DISK_NAND_USER:�û���
���ڲ���:������������
˵    ��:
***************************************************************************/
extern uint32 FtlGetCapacity(uint8 LUN);

/***************************************************************************
��������:�������ӿ�
��ڲ���:LUN=�߼�������, Index=��ʼ������ַ, buf=���ݻ�����, nSec=������
���ڲ���:0=��ȡ��ȷ; ��0=��ȡ����, ���ݲ�����
���ú���:
˵    ��:
***************************************************************************/
extern int32 FtlRead(uint8 LUN, uint32 index, uint32 nSec, void *buf);

/***************************************************************************
��������:д�����ӿ�
��ڲ���:LUN=�߼�������, Index=��ʼ������ַ, buf=���ݻ�����, nSec=������
���ڲ���:0=��ȷд��; ��0=д����ʧ��, ����û�б���ȷд��
���ú���:
˵    ��:
***************************************************************************/
extern int32 FtlWrite(uint8 LUN, uint32 index, uint32 nSec, void *buf);

#endif
