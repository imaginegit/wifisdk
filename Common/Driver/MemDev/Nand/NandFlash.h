/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:  flash.h
Author:     YIFENG ZHAO
Created:    29/07/2009
Modified:
Revision:   1.00
********************************************************************************
********************************************************************************/
#ifndef _NANDFLASH_H
#define _NANDFLASH_H

//1�����ò���
#define     DUAL_PLANE      //�����Ƿ�ʹ��DUAL PLANE
//#define     INTERLEAVE      //�����Ƿ�ʹ��Interleave

//#define    RECORD_ERASE_COUNT  //����FLASH��������
//#define    POWER_ON_RESUME_ERASE_COUNT //�ϵ�ʱ�ָ��ϴβ�����¼����


#define 	MAX_FLASH_NUM	    4			                /*���֧�ֵ�FLASH��*/
#define     MAX_REFLESH_LOG     10                          //������Ҫ���µĵ�ַ��¼��
#define     DATA_LEN            (1024*16/4)                 //���ݿ鵥λword
#define     SPARE_LEN           (4*16/4)                      //У�����ݳ���
#define     PAGE_SIZE           (DATA_LEN+SPARE_LEN)        //ÿ�����ݵ�λ�ĳ���

#define     CHIP_SIGN           0x4F4E414E                  //NANO
#define     FLASH_MAGIC         0x4E414E44
#define     FLASH_INFO_VER      0x100


#define     FTL_OK              0
#define     FTL_ERROR           ((uint32)-1)


/*******************************************************************
�곣������
*******************************************************************/
//section define
//section define

/*******************************************************************
����ID����
*******************************************************************/
#define     SAMSUNG             0x00		//����SAMSUNG
#define     TOSHIBA             0x01		//��֥TOSHIBA
#define     HYNIX               0x02		//����ʿHYNIX
#define     INFINEON            0x03		//Ӣ����INFINEON
#define     MICRON              0x04		//����MICRON
#define     RENESAS             0x05		//����RENESAS
#define     ST                  0x06		//�ⷨ�뵼��ST
#define     INTEL               0x07		//Ӣ�ض�intel
#define     SANDISK             0x08		//Sandisk

/*******************************************************************
����ID����
*******************************************************************/
#define     Small32M            0x00
#define     Small64M            (Small32M+0x01)
#define     Small128M           (Small32M+0x02)
#define     Small128M_2         (Small32M+0x03)
#define     Small256M           (Small32M+0x04)
#define     SmallPageEnd        Small256M
#define     Large128M           (Small32M+0x05)
#define     Large256M           (Small32M+0x06)
#define     Large512M           (Small32M+0x07)
#define     Large1G             (Small32M+0x08)
#define     Large2G             (Small32M+0x09)


/*******************************************************************
FLASHͨ���������(����)
*******************************************************************/
#define     RESET_CMD               0xff
#define     READ_ID_CMD             0x90
#define     READ_UNIQUE_ID_CMD      0xed
#define     READ_STATUS_CMD         0x70
#define     READ_STATUS_CMD_TOSHIBA 0x71
#define     READ_STATUS_CMD_MICRON  0x78
#define     CHIP1_STATUS_CMD        0xf1
#define     CHIP2_STATUS_CMD        0xf2
#define     PAGE_PROG_CMD           0x8010
#define     PLANE2_PAGE_PROG_CMD    0x8111
#define     INTERLEAVE_PROG_CMD     0x8080
#define     BLOCK_ERASE_CMD         0x60d0
#define     BLOCK_ERASE1_CMD        0x60d1

/*******************************************************************
LARGE PAGE FLASH�������(����)
*******************************************************************/
#define     READ_CMD                0x0030
#define     DUALPLANE_READ_CMD      0x6030
#define     READ_COPY_CMD           0x0035
#define     CACHE_PROG_CMD          0x8015
#define     COPY_PROG_CMD           0x8510
#define     RAND_DATAIN_CMD         0x85
#define     RAND_DATAOUT_CMD        0x05e0
#define     PLANE1_DATAOUT_CMD      0x06e0
/*******************************************************************
SMALL PAGE FLASH�������(����)
*******************************************************************/
#define     READ0_CMD               0x00
#define     READ1_CMD               0x01
#define     READ_SPARE_CMD          0x50
#define     SMALL_COPY_PROG_CMD     0x8A10

//BCHCTL�Ĵ���
#define     BCH_WR                  0x0002
#define     BCH_RST                 0x0001
//FLCTL�Ĵ���
#define     FL_RDY                  0x1000
#define     FL_COR_EN               0x0800
#define     FL_INT_EN               0x0400
#define     FL_XFER_EN              0x0200
#define     FL_INTCLR_EN            0x0100
#define     FL_START                0x0004
#define     FL_WR                   0x0002
#define     FL_RST                  0x0001

#define     FLASH_PROTECT_ON()      //NandWriteDisable()
#define     FLASH_PROTECT_OFF()     //NandWriteEnable()   

/*------------------------------ Global Typedefs -----------------------------*/

/*******************************************************************
FLASH��д�ӿ�
*******************************************************************/

//1�ṹ����


#define IOMUX_FLASH_CS1              ((UINT32)1<<1)
#define IOMUX_FLASH_CS2              ((UINT32)1<<2)
#define IOMUX_FLASH_CS3              ((UINT32)1<<4)

typedef  uint32 (*NREAD_RETRY)(uint32 sec, void *pData, void *pSpare, uint32 nSec);

typedef  void (*NPREPROG)(uint32);

typedef struct tagFLASH_SPEC
{
    uint8 	dualPlaneAddrMap;   //=0��ʾ����2BLOCKΪDUAL-PLANE, ����Ϊǰ��һ�������ΪDUAL-PLANE
    uint8 	fstPlaneAddr;       //=0��ʾPLANE0��ַΪ0, ����Ϊʵ�ʵ�ַ
    uint8   CacheProg;          //�Ƿ�֧��cache program
    uint8   MulPlane;           //�Ƿ�֧��MultiPlane
    uint8   Interleave;         //�Ƿ�֧��Interleave
    uint8   Large;              //�Ƿ�LargeBlock
    uint8   Five;               //�Ƿ���3���е�ַ
    uint8   MLC;                //�Ƿ�MLC
    uint8   Vonder;             //����
    uint8   AccessTime;         //cycle time
    uint8   SecPerPage;         //FLASH DualPlaneÿPAGE������
    uint8   SecPerPageRaw;		//FLASHԭʼÿPAGE������
	uint16	SecPerBlock;
	uint16	SecPerBlockRaw;
	uint16	PagePerBlock;
	uint16	PagePerBlockRaw;
    uint8   EccBits;
    uint8   SpareSize;          //32,46,74,109                  
    uint8   AlignedSpareSize;   //32,48,76,112   
    uint8   ReadStatusCmd;
	uint32	Die2PageAddr;
    uint32	TotalPhySec;
    uint32	TotPhySec[MAX_FLASH_NUM];
    pNAND_IF NandIF[MAX_FLASH_NUM];
    uint8  EnRdmz;
    uint8  EnLSB;
    uint8  LSBMode;
    //uint8   RetryNum;
    uint8   NeedRRT;
    NREAD_RETRY ReadRetry;
    NPREPROG PreProgram;
} FLASH_SPEC, *pFLASH_SPEC;


/*ID BLOCK SECTOR 0 INFO*/
typedef __packed struct tagIDSEC0
{
    uint32  magic;              //0x0ff0aa55, MASKROM�޶����ܸ���
    uint8   reserved[56-4];
    uint32  nandboot2offset;
    uint32  nandboot2len;
    uint32  nandboot1offset1;
    uint32  nandboot1len1;
    uint32  nandboot1offset2;
    uint32  nandboot1len2;
    uint8   CRC[16];
    uint8   reserved1[512-96];
} IDSEC0, *pIDSEC0;

typedef __packed struct tagRKNANO_IDB_SEC3
{
    uint16  usSNSize;                        
    uint8   sn[30]; // 2-31
	uint8   reserved[419]; // 32-450
	uint8	uidSize;
	uint8	uid[15];
	uint8   blueToothSize;
	uint8	blueToothAddr[30];
	uint8	macSize;
	uint8	macAddr[6];
}RKNANO_IDB_SEC3, *PRKNANO_IDB_SEC3;

#if 0
/*ID BLOCK SECTOR 1 INFO*/
typedef __packed struct tagIDSEC1
{
    uint16  sysAreaBlockRaw;        //ϵͳ������, ��ԭʼBlockΪ��λ
    uint16  sysProgDiskCapacity;    //ϵͳ�̼�������, ��MBΪ��λ
    uint16  sysDataDiskCapacity;    //ϵͳ����������, ��MBΪ��λ
    uint16  reserved0[2];           //����������0
    uint16  chipSignL;              // 28
    uint16  chipSignH;              // RK
    uint16  MachineUIDL;            //����UID,����ʱƥ��̼���
    uint16  MachineUIDH;
    uint16  year;                   //�м������������
    uint16  data;                   //�м��������������
    uint16  mainVer;                //���汾��
    uint16  FirmMasterVer;
    uint16  FirmSlaveVer;
    uint16  FirmSmallVer;
    uint16  LasterLoader;
    uint16  UpgradeTimes;
    uint8   reserved1[480-30];      //����������0
    uint32  ChipSize;
    uint16  blockSize;              //��SEC��ʾ��BLOCK SIZE
    uint8   pageSize;               //��SEC��ʾ��PAGE SIZE
    uint8   eccBits;                //��Ҫ��ECC BIT��, eg. 8/14
    uint8   accessTime;             //��дcycleʱ��, ns
    uint8   reserved2[498-493];      //����������0
    uint16  bStartRB;
    uint16  bEndRB;
    uint16  idBlk[5];                //ID��λ��
}IDSEC1, *pIDSEC1;
#endif

/*ID BLOCK SECTOR 1 INFO*/
typedef __packed struct tagIDSEC1
{
    uint16  SysAreaBlockRaw;        //ϵͳ������, ��ԭʼBlockΪ��λ
    uint16  SysProgDiskCapacity;    //ϵͳ�̼�������, ��MBΪ��λ			
	uint16  SysDataDiskCapacity;			
	uint16  Disk2Size;				
	uint16  Disk3Size;				
	uint32	ChipTag;
	uint32	MachineId;
    
	uint16	LoaderYear;
	uint16	LoaderDate;
	uint16	LoaderVer;
	uint16	FirmMasterVer;		// (24-25)	BCD���� �̼����汾
    uint16	FirmSlaveVer;			// (26-27)	BCD���� �̼��Ӱ汾
    uint16  FirmSmallVer;			// (28-29)	BCD���� �̼��Ӱ汾  
	uint16  LastLoaderVer;
	uint16  UpgradeTimes;
	uint16  MachineInfoLen;
	uint8	MachineInfo[30];
	uint16	ManufactoryInfoLen;
	uint8	ManufactoryInfo[30];
	uint16	FlashInfoOffset;
	uint16	FlashInfoLen;
    uint8	Reserved2[382];				// (102-483)����
	uint32	FlashSize;				    //��sectorΪ��λ
	uint16  BlockSize;                  //��SEC��ʾ��BLOCK SIZE
	uint8   PageSize;                  //��SEC��ʾ��PAGE SIZE
	uint8   ECCBits;
	uint8   AccessTime;                //��дcycleʱ��, ns
    uint8   reserved2[5];
	uint16  FirstReservedBlock;		// (498-499)Idblock֮���һ��������ĵ�λ��		(��λ:block)
    uint16  LastReservedBlock;		// (500-501)Idblock֮�����һ��������ĵ�λ��	(��λ:block)
	uint16  IdBlock0;
	uint16  IdBlock1;
	uint16  IdBlock2;
	uint16  IdBlock3;
	uint16  IdBlock4;
}IDSEC1, *pIDSEC1;


typedef __packed struct _FLASH_INFO
{
    uint32  FlashSize;          //(SectorΪ��λ)  4Byte
    uint16  BlockSize;          //(SectorΪ��λ)  2Byte
    uint8   PageSize;           //(SectorΪ��λ)  1Byte
    uint8   ECCBits;            //(bitsΪ��λ)    1Byte
    uint8   AccessTime;
    uint8   ManufacturerName;   // 1Byte
    uint8   FlashMask;          //ÿһbit�����Ǹ�Ƭѡ�Ƿ���FLASH
    int8    reserved0[5+48];        //������չ�ã���� 0
    // flash ����  ��ʼ64
    uint32   magic;              //NAND  0x4E414E44
    uint16   ver;                //�汾  1.00
    uint16   lenth;              //��Ч���ݳ���
    uint8    ReadRetryTab[512-64-4];
} FLASH_INFO, *pFLASH_INFO;


/*----------------------------- External Variables ---------------------------*/

extern FLASH_SPEC NandSpec;
extern uint16 NandIDBlkNum;

extern uint32 	FlashSysProtAddr;
extern uint32 	FlashSysCodeRawAddr;
extern  uint32  NandRefleshSec[];

/*------------------------- Global Function Prototypes -----------------------*/

//1����ԭ������

//flash.c
extern	uint32  NandInit(void);
extern	void    NandReadID(uint8 CS, void *buf);
extern	int32 	NandRead(uint32 sec, uint8 *pData, void *pSysData, uint32 nSec, uint32 NextSec);
extern	uint32 	NandReadEx(uint32 sec, void *pData, void *pSysData, uint32 nSec, uint32 NextSec);

extern	uint32 	NandProg(uint32 sec, void *pData, void *pSysData);

extern	uint32  NandCopyProg(uint32 srcSec, uint32 destSec, uint16 nSec, void *pSpare);

extern	uint32 NandBlkErase(uint32 sec, uint32 *SecReplace);

extern	uint32  NandChkBadBlk(uint32 blk);

extern	uint32 NandWaitBusy(uint32 *SecReplace);
extern	uint32 MDGetNandInfo(pMEMDEV_INFO pDevInfo);
extern	uint32 MDNandBlkErase(uint32 ChipSel, uint32 BlkStart, uint32 BlkNum, uint32 mod);
extern	void NandSetBadBlk(uint32 pageNum);
extern	uint32 NandDeInit(void);


extern	void NandDelay(uint32 count);
extern	void NandCs(uint8 CS);
extern	void NandDeCs(uint8 CS);
extern	uint32 NandWaitBusy(uint32 *SecReplace);
extern	uint32 NandSyncIO(void);

extern	uint32 NandGetCS(uint32 sec, uint32 *SecInCS);
extern	void NandSetRdmzflag(uint8 value);
extern	void NandRefreshRec(uint32 sec);
extern	int32 NandGetProductSn(void *pSn);
extern	uint32 NandReadIDB(uint32 sec, uint32 nSec, uint8 *pData);
extern	uint32 NandWriteIDB(uint32 sec, uint32 nSec, uint8 *pData);

#endif
