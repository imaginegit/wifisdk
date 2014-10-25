/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: Flt.c
Desc: 

Author: chenfen@rock-chip.com
Date: 11-11-17
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/
#include "MDConfig.h"
    
#if (NAND_DRIVER == 1)

#define     IN_FTL
//#define     IN_LOADER


/*------------------------------------ Defines -------------------------------*/
//#define _DEBUG_FTL_


#define UNUSED(x) ( void )(x)

#define     OBTAIN_FTL_SEM
#define     RELEASE_FTL_SEM

#ifdef IN_LOADER
#define     UserIsrDisable()				0                
#define     UserIsrEnable(Tmp)              (Tmp=1)
#else
#define     FTL_POWER_ON_FAST
#define     FTL_READ_REFRESH_EN
#endif

#define     FTL_VERSION                     0x0301      //FTL版本号, 0x100表示ver1.00, 需要低格时修改主版本号, 否则只需修改次版本号
#define     MAX_REMAP_TBL                   (1024*2)    //最大的映射表, 字节单位, 必须>=1024
#define     MAX_BAD_BLK_NUM                 1024        //最大的的坏块交替块数

#define     MAX_FREE_BLK_NUM                512        //用来作扩展块的数量最大限制
#define     MIN_FREE_BLK_NUM                80          //用来作扩展块的数量最小限制
#define     MAX_RSVD_BLK_NUM                4           //保留块数(包含2个坏块表记录块)
#define     MAX_BLK_NUM                     16384

#define     MAX_PAGES_NUM                   256         //一个BLOCK 最大的Page 数
#define     MAX_PAGE_SIZE                   32          //一个PAGE 最大的Sec 数

#define     MAX_SWBLK_NUM                   8           //逻辑顺序写BLOCK 的数目 
#define     MAX_RWBLK_NUM                   8           //逻辑随机写BLOCK 的数目 
#define     MAX_PBA_1_RW                    4           // 1个逻辑随机写 BLOCK 对应的最大物理BLOCK 数目
#define     MAX_PBA_ALL_RW                  16          // 所有逻辑随机写 BLOCK 对应的最大物理BLOCK 数目
#define     MAX_CACHEBLK_NUM                1           // CACHE BLOCK 的最大物理块

#define     ENABLE_DWCACHE                  0           //使能 Delay write cache 目前未完成               
#define     MAX_DWCACHE_NUM                 2           //Delay write cache num


#if (ENABLE_DWCACHE)
#define     FTL_BUF_NUM                     3
#else
#define     FTL_BUF_NUM                     2
#endif

/*按1个block256个page, 1个page16K大小计算
1个RwBlk需要 512+1024Byte空间记录页映射信息*/
#define     RW_LOG_SIZE                     (6*MAX_PAGES_NUM)
#define     RW_LOG_BUF_SIZE                 ((MAX_RWBLK_NUM * RW_LOG_SIZE) / 4)


#define     SIGN_BAD_BLK                    0xf000
#define     SIGN_CACHE_BLK                  0xf100
#define     SIGN_DATA_BLK                   0xf200
#define     SIGN_RCV_BLK                    0xf300
#define     SIGN_REMAP_BLK                  0xf400


#define     SIGN_SW_BLK                     SIGN_DATA_BLK
#define     SIGN_RW_BLK                     (SIGN_DATA_BLK | 0x01)
#define     SIGN_MW_BLK                     (SIGN_DATA_BLK | 0x02)


#define     FTL_OPEN_RD                     0
#define     FTL_OPEN_WR                     1     
#define     FTL_OPEN_SW2RW                  2 

/*----------------------------------- Typedefs -------------------------------*/

typedef struct tagREMAP_INFO
{
    uint16 page[32];
    uint8  num[32];
    uint32 write[2];
    uint16 max;
    uint16 BlkAddr;
    uint16 PageAddr;
    uint16 RcvRemapPageAddr;                        //保留块中记录 page位置，指向下一个用来保存的page(空page)
    uint16 RcvRemapValid;                           //保留块中记录的remap数据有效
    uint16 tbl[MAX_REMAP_TBL/2];
}REMAP_INFO, *pREMAP_INFO;

typedef struct tagBAD_BLK_INFO
{
    uint16 max;
    uint16 cnt;
    uint16 offset;
    uint16 BlkAddr[4];
    //uint16 tbl[MAX_BAD_BLK_NUM];
}BAD_BLK_INFO, *pBAD_BLK_INFO;


typedef struct tagCIR_QUEUE
{
    uint16 max;
	uint16 front;
	uint16 rear;
	uint16 count;
	uint16 arr[MAX_FREE_BLK_NUM];
}CIR_QUEUE, *pCIR_QUEUE;


typedef struct tagRW_BLK_INFO
{
    uint16          ver;
    uint16          LBA;                            //记录RW BLOCK 逻辑地址
    uint16          SrcPBA;                         //记录RW BLOCK 原块物理地址
    uint16          PBA[MAX_PBA_1_RW];              //记录RW BLOCK 对应的物理BLOCK 地址
    uint16          nPage;                          //记录当前写到第几个Page
    uint8           count;                          //用于控制最近访问
    uint8           valid;                          //valid = 1 表示有效, 0无效
    uint8           MaxPBA;                         //一个LBA 对应的物理BLOCK最大数目, 当nPage等于MaxPBA*PagePerBlk时, Merge RW BLOCK

    uint16          *PagePBA;                       //记录一个逻辑页号对应的物理page 地址, 指针指向一个512byte大小的数组        
    uint32          *mask;                          //记录物理页中有效的数据扇区的mask,1有效, 指针指向一个1024byte大小的数组                                                                                    
}RW_BLK_INFO, *pRW_BLK_INFO;

typedef struct tagCACHE_PAGE_INFO
{
    uint16	PagePBA;                                //SW BLOCK 数据 数据所在的PAGE 的物理地址
    uint16  ver;                                    //CACHE 块当前PAGE 保存的 SW BLOCK 数据的版本
    uint32  LBA;                                    //CACHE 块当前PAGE 保存的 SW BLOCK 数据的LBA, 此LBA是PAGE 对齐的
    uint16  nSec;                                   //CACHE 块当前PAGE 保存的 SW BLOCK 数据的长度                           
    uint16  valid;
}CACHE_PAGE_INFO, *pCACHE_PAGE_INFO;

typedef struct tagCACHE_BLK_INFO
{
    uint16	PBA[MAX_CACHEBLK_NUM];                  //CACHE 块的物理地址
    uint16  ver;                                    //CACHE 块的当前版本
    uint16  nPage;                                  //CACHE 块 写到的BLOCK 内的第几个PAGE                         
    CACHE_PAGE_INFO PageInfo[MAX_SWBLK_NUM];        //CACHE 块 页信息              
}CACHE_BLK_INFO, *pCACHE_BLK_INFO;


typedef struct tagSW_BLK_INFO
{
    uint16          i;                              //在 SwBlkInfo[MAX_SWBLK_NUM] 索引, 也用于索引PageInfo[MAX_SWBLK_NUM]       
    uint16          ver;                            //记录SW BLOCK 的版本
    uint16          LBA;                            //记录SW BLOCK 逻辑地址
    uint16          SrcPBA;                         //记录SW BLOCK 原块物理地址
    uint16          PBA;                            //记录SW BLOCK 对应的物理BLOCK 地址                             
    uint16          nSec;                           //记录当前已经写的扇区数, 
    uint8           count;                          //用于控制最近访问
    uint8           valid;                          //valid = 1 表示有效, 0无效                                                    //Cache block信息, (nSec & PageMask)这个区间扇区的数据是写在 CACHE Block 中的
}SW_BLK_INFO, *pSW_BLK_INFO;


//Delay Write Cache Infomation
typedef struct tagFTL_DWCACHE
{
    uint8   LUN;
    uint32  LBA;                                    //Cache 的sec逻辑地址, page 对齐
    uint32  mask;                               
    uint8   valid;                                  //valid = 1 表示有效, 0无效
}FTL_DWCACHE, *pFTL_DWCACHE;


typedef struct tagNEW_FTL_INFO
{
    uint8       valid;                              //FTL有效
    uint16      FtlVer;                             //FTL版本号

    uint16      StartPhyBlk;                        //FLASH作为数据区的起始块
    uint32      MaxPhyBlk;                          //FLASH作为数据区的最大块
    uint16      TotalLogicBlk;

    uint16      SecPerBlk;                          //每个BLOCK的扇区数
    uint16      PagePerBlk;                         //每个BLOCK的PAGE数         
    uint8       SecPerPage;                         //每个PAGE的扇区数
    uint8       LogPageSize;                        //以2的幂次方表示的大小，用一个扇区作为单位, 如8K Page, LogPageSize=4
    uint8       LogBlkSize;                         //以2的幂次方表示的大小，用一个扇区作为单位, 如2M Block LogBlkSize=12, 1M Block LogBlkSize=11
    uint8       LogPageBlk;                         //以2的幂次方表示的大小, 用一个PAGE作为单位, 如256个Page 的Block LogBlkPage=8 
    uint8       PageMask;                           //PageMask = SecPerPage-1        
    uint16      BlockMask;                          //BlockMask = SecPerBlk-1
    uint16      PageBlkMask;                        //BlkPageMask = PagePerBlk-1

    BAD_BLK_INFO BadBlkInfo;                        //坏块信息
    CACHE_BLK_INFO CacheBlkInfo;
    SW_BLK_INFO SwBlkInfo[MAX_SWBLK_NUM];
    RW_BLK_INFO RwBlkInfo[MAX_RWBLK_NUM];
    #if (ENABLE_DWCACHE)
    uint8       EnDWCache;
    FTL_DWCACHE DWCacheInfo;  
    #endif  
    CIR_QUEUE   FreeBlkInfo;                        //空块表, 当磁盘空间满时至少保证有64块可以滚动
    REMAP_INFO  RemapInfo;                          //块映射表, 随容量大小而变, e.g. 32GB/(1MB/blk)=64KB
}FTL_INFO, *pFTL_INFO;


//typedef int32 (*FTL_WRITE) (void *pBlkInfo, uint32 index, uint16 nSec, uint8 *pBuf);
typedef int32 (*FTL_WRITE) (void *, uint32, uint16, uint8 *);
typedef int32 (*FTL_READ) (void *, uint32, uint16, uint8 *);

typedef struct tagFTL_HANDLE
{
    void        *pBlkInfo;
    FTL_WRITE   write;
    FTL_READ    read;
}FTL_HANDLE,  pFTL_HANDLE;

/*-------------------------- Forward Declarations ----------------------------*/

/* ------------------------------- Globals ---------------------------------- */
_ATTR_FTL_BSS_ static FTL_INFO gFtlInfo;

_ATTR_FTL_CACHE_ uint32 gFtlBuf[FTL_BUF_NUM][PAGE_SIZE];
_ATTR_FTL_CACHE2_ uint32 FtlRwLogBuf[RW_LOG_BUF_SIZE];

_ATTR_FTL_BSS_  uint32 *pFtlTmpBuf;

#if (ENABLE_DWCACHE)
_ATTR_FTL_BSS_  uint32 *pFtlCacheBuf;
#endif

_ATTR_FTL_BSS_  uint16 FilSys1stLBA;


#ifdef  FTL_READ_REFRESH_EN
_ATTR_FTL_BSS_  uint32 gFtlReadScanSec; //固件扫描
#endif

#ifdef IN_LOADER
_ATTR_FTL_BSS_ uint32  CurEraseBlk;
#endif

extern uint32 *NandCopyProgBuf;
/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Function Prototypes ----------------------*/
static uint32 FtlGetRemap(uint32 LBA);
static uint16 FtlGetRemapBlk(uint16 LBA, uint16 PBA);
static void FtlSaveRemapTbl(uint32 mod);
static void FtlVarInit(void);
static void FltSysCodeChk(void);
static pSW_BLK_INFO FtlSwBlkOpen(uint32 index, uint16 nSec, uint32 flag);
static void FtlSaveRemap(void);
int32 FtlDoWrite(uint8 LUN, uint32 index, uint32 nSec, void *buf);

void FtlTest(void);
//int32 FtlFormatUsrDsk(uint16 *pBadBlkTbl);

/*------------------------ Function Implement --------------------------------*/

/*
Name:       __Log2
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
int32 __Log2(uint32 num)    
{

    uint32 i;

    for (i=0; i<32; i++)
    {
        if (num == (1<<i))
            return (int32)i;
    }
    return -1;
}

/*
Name:       QueueInit
Desc:       置空队列
Param:      队列Q
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void QueueInit(CIR_QUEUE *Q, uint16 max)
{
    Q->front=Q->rear=0;
    Q->count=0;
    Q->max=max;
    memset(Q->arr, 0x00, Q->max*2);
}

/*
Name:       QueueEmpty
Desc:       判断队列空
Param:      队列Q
Return:     空返回1
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static uint16 QueueEmpty(CIR_QUEUE *Q)
{
    return (Q->count==0 ? 1 : 0);
}

/*
Name:       QueueFull
Desc:       判断队列满
Param:      队列Q
Return:     满返回1     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static uint16 QueueFull(CIR_QUEUE *Q)
{
    return ((Q->count==Q->max) ? 1 : 0);
}

/*
Name:       QueueIn
Desc:       入队
Param:      队列Q,数据x   
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static void QueueIn(CIR_QUEUE *Q, uint16 x)
{
    if (QueueFull(Q)==0)
    {
        uint32 i;

        for (i=0; i<Q->count; i++)
        {
            if (x == Q->arr[(Q->front+i) % Q->max])
                return;
        }
        Q->count++;
        Q->arr[Q->rear]=x;
        Q->rear=(Q->rear+1) % Q->max;
    }
}

/*
Name:       QueueOut
Desc:       出队
Param:      队列Q      
Return:     队头数据
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static uint16 QueueOut(CIR_QUEUE *Q)
{
    uint16 tmp=0;

    if (QueueEmpty(Q) == 0)
    {
        tmp=Q->arr[Q->front];
        Q->count--;
        Q->front=(Q->front + 1) % Q->max;
    }
    return (tmp);
}

/*
Name:       QueueFront
Desc:       取队头数据       
Param:      队列Q      
Return:     队头数据
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static uint16 QueueFront(CIR_QUEUE *Q)
{
    if (QueueEmpty(Q) != 0)
        return ((uint16)-1);

    return (Q->arr[Q->front]);
}

#ifndef IN_LOADER

/*
Name:       FtlReadMskPage
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlReadMskPage(uint32 sec, uint32 mask, uint8 *pData)
{
    uint32 i, start;
    uint16 offset, len = 0;
    int32 result = FTL_OK;
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 PageSize = pFtl->SecPerPage;
    
    sec -= (sec & pFtl->PageMask);
    for (i=0, start = sec; i<PageSize; i++)
    {
        if (mask & (1<<i))
        {
            if (0==len)
                start += i;
            len++;
            if (i+1<PageSize)
                continue;
        }

        if (len>0)
        {
            offset = start & pFtl->PageMask;
            if ( FTL_OK != NandReadEx(start, (uint8*)pData+offset*512, NULL, len, 0))
            {
                result = FTL_ERROR;
            }
            len = 0;
            start = sec;
        }
    } 

    return result;
}


/*
Name:       FtlFindWrPages
Desc:       找出一个block中已写的page数
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static uint16 FtlFindWrPages(uint16 BlkAddr)
{
    pFTL_INFO pFtl = &gFtlInfo;
    int16 low, high, mid;
    uint16 spare[4];
    uint32 PBA = (uint32)BlkAddr<<pFtl->LogBlkSize;

    low=0;
    high=pFtl->PagePerBlk-1;
    NandReadEx(PBA+(high<<pFtl->LogPageSize), NULL, spare, 2, 0);
    if (spare[0]==0xffff)
    {
        while (low <= high)
        {
            mid=(low+high)/2;
            NandReadEx(PBA+(mid<<pFtl->LogPageSize), NULL, spare, 2, 0);
            if (spare[0]==0xffff)
            {
                high=mid-1;
            }
            else
            {
                 low=mid+1;
            }
        }
    }

    return (uint16)(high+1);
}

#if (ENABLE_DWCACHE)
/*
Name:       FtlDwCacheInit
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FtlDwCacheInit(void)
{
    pFTL_INFO pFtl = &gFtlInfo;
    pFTL_DWCACHE pDWCache = &pFtl->DWCacheInfo;

    pFtl->EnDWCache = 0;
    memset(pDWCache, 0, sizeof(FTL_DWCACHE));
}

/*
Name:       FtlDwCacheFree
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlDwCacheFree(void)
{
    int32 ret;
    uint32 mask, nSec, i, start,end;
    uint8* pCacheBuf = (uint8*)pFtlCacheBuf;
    pFTL_INFO pFtl = &gFtlInfo;
    pFTL_DWCACHE pDWCache = &pFtl->DWCacheInfo;
    uint16 PageSize = pFtl->SecPerPage;

    if (!pDWCache->valid)
        return FTL_OK;

    start = 0;
    nSec = PageSize;
    mask = (-1 << PageSize) ^ -1;
    if (mask != pDWCache->mask)
    {
        for (i=0; i<PageSize; i++)
        {
            if (pDWCache->mask & (1<<i))
            {
                end = i;
                if (!start)
                    start = i;
            }
        }
        /*找到连续扇区区间,这个区间的起始扇区是CACHE中第一个有效的扇区,
        结束扇区是CACHE中最后一个有效的扇区, 中间可能有些扇区的数据不在CAHCE中*/
        nSec = end-start+1;                         
        mask = ((-1 << nSec) ^ -1) << start;
        mask = mask ^ pDWCache->mask;               //找出连续扇区区间中不在Cache中扇区

        if (mask)                                   //判断连续区间中是否有数据不在CACHE中
        {
            uint32 len, LBA, offset;
            for (i=start, LBA = pDWCache->LBA; i<start+nSec; i++)
            {
                if (mask & (1<<i))
                {
                    if (0==len)
                        LBA += i;
                    len++;
                    if (i+1<PageSize)   //判断是否已经到最后一个扇区
                        continue;
                }

                if (len>0)  //找到一个连续的区间
                {
                    offset = LBA & pFtl->PageMask; 
                    FtlRead(pDWCache->LUN, LBA, len, pCacheBuf+offset*512);
                    len = 0;
                    LBA = pDWCache->LBA;
                }
            }         
        }
    }

    ret = FtlDoWrite(pDWCache->LUN, pDWCache->LBA+start, nSec, pCacheBuf+start*512);
    memset(pDWCache, 0, sizeof(FTL_DWCACHE));

    return ret;
}

/*
Name:       FtlDwCacheOpen
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlDwCacheWrite(uint8 LUN, uint32 LBA, uint32 nSec, void *buf)
{
    uint32 mask;
    uint8* pCacheBuf = (uint8*)pFtlCacheBuf;
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 PageOffset = LBA & pFtl->PageMask;
    pFTL_DWCACHE pDWCache = &pFtl->DWCacheInfo;

    LBA = LBA - PageOffset;
    if (PageOffset+nSec < pFtl->SecPerPage)                                     //判断是否满足进CACHE
    {
        if (pDWCache->valid)
        {
            if (LUN != pDWCache->LUN || LBA != pDWCache->LBA)
               FtlDwCacheFree(); 
        }

        mask = ((-1 << nSec) ^ -1) << PageOffset;                               //计算一个PAGE 内要写扇区的mask
        memcpy(pCacheBuf+PageOffset*512, buf, nSec*512);
        pDWCache->mask |= mask;
        pDWCache->LUN = LUN;
        pDWCache->LBA == LBA;
        pDWCache->valid = 1;
    }
    else
    {
        nSec = 0;
        /*判断是否需要释放, 要写的数据和CACHE是同一个BLOCK就要释放*/
        if (pDWCache->valid && LUN == pDWCache->LUN 
            && (LBA & ~ pFtl->BlockMask)==( pDWCache->LBA & ~ pFtl->BlockMask)) 
        {
            if (LBA == pDWCache->LBA)                                                     
            {
                /*要写的数据和CACHE是同一个PAGE, 先把不对齐的部分写到CACHE, 
                后面数据就是PAGE对齐*/
                nSec = pFtl->SecPerPage - PageOffset;
                mask = ((-1 << nSec) ^ -1) << PageOffset; 
                memcpy(pCacheBuf+PageOffset*512, buf, nSec*512);
                pDWCache->mask |= mask;    
            }
            FtlDwCacheFree();
        }
    }

    return nSec;
}

/*
Name:       FtlDwCacheOpen
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlDwCacheRead(uint8 LUN, uint32 LBA, uint32 nSec, void *buf)
{
    uint32 offset, len, i, mask;
    uint8* pCacheBuf = (uint8*)pFtlCacheBuf;
    uint8* pData = (uint8*)buf;
    pFTL_INFO pFtl = &gFtlInfo;
    pFTL_DWCACHE pDWCache = &pFtl->DWCacheInfo;

    if (!pDWCache->valid || LUN != pDWCache->LUN)
        return FTL_OK;

    if (LBA < pDWCache->LBA && LBA+nSec>pDWCache->LBA)
    {
        offset = pDWCache->LBA - LBA;
        pData += offset*512;
        len = LBA + nSec - pDWCache->LBA;
        len = (len>pFtl->SecPerPage)?  pFtl->SecPerPage : len;
        mask =  (-1 << len) ^ -1;                  //计算要读的数据与(pDWCache->LBA, SecPerPage)区间重叠的扇区
        mask = mask & pDWCache->mask;
    }
    else if (LBA >= pDWCache->LBA  && LBA < pDWCache->LBA+pFtl->SecPerPage)
    {
        offset = LBA & pFtl->PageMask;
        pData -= offset*512;
        len = pFtl->SecPerPage - offset;
        len = (len > nSec)? nSec : len;
        mask = ((-1 << len) ^ -1) << offset;       //计算要读的数据与(pDWCache->LBA, SecPerPage)区间重叠的扇区
        mask = mask & pDWCache->mask;
    }
    else
    {
        mask = 0;
    }
    
    if (mask)                                       
    {
        for (i=0; i<pFtl->SecPerPage; i++)
        {
            if (mask & (1<<i))                      //判断要读数据是否有在CACHE里
            {
                memcpy(pData+i*512, pCacheBuf+i*512, 512);
            }
        }
    }

    return FTL_OK;
}


/*
Name:       FtlDwCachEnble
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
void FtlDwCachEnble(uint8 enble)
{
    pFTL_INFO pFtl = &gFtlInfo;
    pFtl->EnDWCache = enble;
    if (0 == enble)
    {
        FtlDwCacheFree();
    }
}

/*
Name:       FtlBgTask
Desc:       Ftl Background Task
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
void FtlBgTask(void)
{
    #if (ENABLE_DWCACHE)
    FtlDwCacheFree();
    #endif
    //FtlRwBlkFree(1);
}


/*
Name:	FtlCacheWrite
Desc: 	写多个扇区
Param:	LUN=逻辑分区号, Index=扇区号(LBA), buf=数据缓冲区, nSec=扇区数
Return:		
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
int32 FtlWrite(uint8 LUN, uint32 LBA, uint32 nSec, void *buf)
{
    pFTL_INFO pFtl = &gFtlInfo;
    int32  len = 0;
    uint8 *ptr=(uint8*)buf;

    if (pFtl->EnDWCache)
    {
        if (len = FtlDwCacheWrite(LUN, LBA, nSec, buf) > 0)
        {
            LBA += len;
            nSec -= len;
            ptr += 512*len;
            len = 0;
        }

        if (nSec)
        {
            len = pFtl->SecPerPage-(LBA & pFtl->PageMask);
            len  = (nSec>len)? (nSec-len):nSec;
            len = len & pFtl->PageMask;     //计算最后一次写的长度, 若最后一次写是整个Page,则len==0
            nSec -= len;
        }
    }

    if (nSec)
    {
        FtlDoWrite(LUN, LBA, nSec, ptr);
        LBA += nSec;
        nSec -= nSec;
        ptr += 512*nSec;
    }

    if (len)                            //若最后一次写不是整个page,则写到cache
    {
        FtlDwCacheWrite(LUN, LBA, nSec, ptr);
    }

}

#endif

/*
Name:       FtlRwBlkOpen
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FltRwBlkInit(void)
{
    uint32 i;
    pRW_BLK_INFO pRwBlk = gFtlInfo.RwBlkInfo;
    uint8 *pLogBuf  = (uint8 *)&FtlRwLogBuf[0];

    memset (pRwBlk, 0, sizeof(RW_BLK_INFO)*MAX_RWBLK_NUM);
    for (i=0; i<MAX_RWBLK_NUM; i++, pRwBlk++)
    {
        /*按1个block256个page, 1个page16K大小计算
        1个RwBlk需要 512+1024Byte空间记录页映射信息, pRwBlk->PagePBA指向512Byte空间
        pRwBlk->mask 指向1024Byte空间*/
        pRwBlk->PagePBA = (uint16*)&pLogBuf[RW_LOG_SIZE*i];  
        pRwBlk->mask = (uint32*)&pLogBuf[RW_LOG_SIZE*i+512];
        
        //memset(pRwBlk->log, 0, sizeof(pRW_PAGE_INFO)* MAX_PAGES_NUM);   //FtlRwBlkOpen 会执行此操作
        pRwBlk->count = i;
    }
}

/*
Name:       FltRwGetMaxPBA
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static uint32 FltRwGetPBA(pRW_BLK_INFO pRwBlk, uint16 PageLBA)
{
    uint32 i, PBA; 
    uint16 PagePBA;
    pFTL_INFO pFtl = &gFtlInfo;
    
    PagePBA = pRwBlk->PagePBA[PageLBA];
    i = PagePBA>>pFtl->LogPageBlk;
    PBA = ((uint32)pRwBlk->PBA[i]<<pFtl->LogBlkSize) + ((PagePBA & pFtl->PageBlkMask)<<pFtl->LogPageSize);

    return PBA;
}

/*
Name:       FltRwGetMaxPBA
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static uint8 FltRwGetMaxPBA(uint16 BlkLBA)
{
    return (FilSys1stLBA == BlkLBA)? 2/*4*/ : 2;
}

/*
Name:       FtlMergeRwBlk
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static void FtlRwBlkMerge(pRW_BLK_INFO pRwBlk, uint32 NewBlk)
{
    uint16 spare[16];
    uint32 i, PBA, SrcPBA, mask, ReadMask; 
    pFTL_INFO pFtl = &gFtlInfo;
    uint8* pTmpBuf = (uint8*)pFtlTmpBuf;

    if (0==NewBlk)
        NewBlk = QueueOut(&pFtl->FreeBlkInfo);

    NewBlk = NewBlk<<pFtl->LogBlkSize;
    NandBlkErase(NewBlk, NULL);

    spare[0]=SIGN_MW_BLK;            
    spare[1]=pRwBlk->ver+1;
    spare[2]=pRwBlk->LBA;
    spare[3]=pRwBlk->SrcPBA;
    spare[4]=pRwBlk->PBA[(pRwBlk->nPage-1)>>pFtl->LogPageBlk];       //用于识别是合并的目标块, 可用于掉电恢复   
    spare[5]=0;                
    spare[6]=spare[7]=0;

    ReadMask = (-1 << pFtl->SecPerPage) ^ -1;
    SrcPBA = (uint32)pRwBlk->SrcPBA<<pFtl->LogBlkSize;
    for (i = 0; i<pFtl->PagePerBlk; i++)
    {
        mask=0;
        if (pRwBlk->mask[i])
        {
            mask = pRwBlk->mask[i];

            PBA = FltRwGetPBA(pRwBlk, i);
            if (mask == ReadMask)   //数据都在RWBLK
                NandReadEx(PBA, pTmpBuf, NULL, pFtl->SecPerPage, 0);
            else
                FtlReadMskPage(PBA, mask, pTmpBuf);      
        }
        
        mask = mask ^ ReadMask;                     //计算还要从原块读取的扇区 的mask
        if (mask)
        {   
            PBA = SrcPBA + (i<<pFtl->LogPageSize);
            if (mask == ReadMask)                   //数据都在原块
                NandReadEx(PBA, pTmpBuf, NULL, pFtl->SecPerPage, 0);
            else
                FtlReadMskPage(PBA, mask, pTmpBuf);
        }

        NandProg(NewBlk+(i<<pFtl->LogPageSize), pTmpBuf, spare);
    }

    FtlGetRemapBlk(pRwBlk->LBA, NewBlk>>pFtl->LogBlkSize);               //修改映射关系

    QueueIn(&pFtl->FreeBlkInfo, pRwBlk->SrcPBA);   //回收原数据块

    for (i=0; i<pRwBlk->MaxPBA; i++)
    {
        if (!pRwBlk->PBA[i])
            break;
        QueueIn(&pFtl->FreeBlkInfo, pRwBlk->PBA[i]); //回收物理块
        //pRwBlk->PBA[i] = 0;
    }
}


/*
Name:       FtlRwBlkClose
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlRwBlkClose(pRW_BLK_INFO pRwBlk)
{
    if (pRwBlk->valid)
    {
        FtlRwBlkMerge(pRwBlk, 0);
        pRwBlk->valid = 0;        
    }
    return FTL_OK;
}


/*
Name:       FtlRwBlkOpen
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static pRW_BLK_INFO FtlRwBlkOpen(uint32 index, uint32 flag)
{
    uint32 i;
    pRW_BLK_INFO pRwBlk, pTmp;
    uint16 spare[4];
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 BlkLBA = index >> pFtl->LogBlkSize;   

    for(i=0, pRwBlk=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pRwBlk++)
    {
        if (pRwBlk->valid && pRwBlk->LBA == BlkLBA)
        {
            if (FTL_OPEN_RD == flag)
                return pRwBlk;
            else
                break;
        }           
    }

    if (FTL_OPEN_RD == flag)
        return NULL;

    if (i >= MAX_RWBLK_NUM)
    {
        for(i=0, pRwBlk=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pRwBlk++)
        {
            if (0 == pRwBlk->valid)
                break;            
        }

        if (i >= MAX_RWBLK_NUM)
        {
            for(i=0,pRwBlk=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pRwBlk++)
            {
                if (0 == pRwBlk->count)
                {
                    FtlRwBlkClose(pRwBlk);
                    break;
                }
            }
        }

        memset(pRwBlk->PBA, 0, 2*MAX_PBA_1_RW);
        memset(pRwBlk->PagePBA, 0, RW_LOG_SIZE);  
        pRwBlk->LBA = BlkLBA;
        pRwBlk->nPage = 0;
        pRwBlk->SrcPBA = FtlGetRemapBlk(BlkLBA, 0);
        NandReadEx(((uint32)pRwBlk->SrcPBA<<pFtl->LogBlkSize)+1, NULL, spare, 1, 0);
        pRwBlk->ver=spare[0]/*+1*/;
        pRwBlk->MaxPBA = FltRwGetMaxPBA(BlkLBA);
        pRwBlk->valid = 1;
    }

    /*最接近访问块处理, 写的时候才起作用*/
    for(i=0, pTmp=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pTmp++)    
    {
        if (pTmp->count > pRwBlk->count)
        {
            pTmp->count--;
        }
    }
    pRwBlk->count = MAX_RWBLK_NUM-1;

    return pRwBlk;     
}

/*
Name:       FtlRwReadPage
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlRwReadPage(pRW_BLK_INFO pRwBlk, uint32 index, uint16 nSec, uint8 *pData)
{
    int32 result = FTL_OK;
    uint32 mask, ReadMask, ReadMask1;
    uint16 PageOffset, PageLBA;
    pFTL_INFO pFtl = &gFtlInfo;
    uint8* pTmpBuf = (uint8*)pFtlTmpBuf;
    uint16 BlkOffset = (uint16)(index & pFtl->BlockMask);

    PageLBA = BlkOffset>>pFtl->LogPageSize;                         //要读的PAGE 的逻辑地址
    PageOffset = BlkOffset & pFtl->PageMask;
    ReadMask1 = ReadMask = ((-1 << nSec) ^ -1) << PageOffset;       //计算一个PAGE 内要读扇区的mask
    mask = pRwBlk->mask[PageLBA] & ReadMask;    //找出pRwBlk->mask[PageLBA] 中要读的扇区的mask
                        
    if (mask)
    {
        uint32 PBA;

        ReadMask &= ~mask;
        PBA = FltRwGetPBA(pRwBlk, PageLBA);
        if (0 == ReadMask)                      //0 = ReadMask 时log[PageLBA].mask 包含了所有要读的扇区                      
        {
            if (FTL_OK != NandReadEx(PBA+PageOffset, pData, NULL, nSec, 0))
                result = FTL_ERROR;               
        }
        else
        {
            if (FTL_OK != FtlReadMskPage(PBA, mask, pTmpBuf))
                result = FTL_ERROR;
        }
    }

    if (ReadMask)                   //还有要读的扇区没读到
    {
        if (ReadMask == ReadMask1)  //ReadMask == ReadMask1时, 要读的数据都在 原块中
        {
            if (FTL_OK != NandReadEx(((uint32)pRwBlk->SrcPBA<<pFtl->LogBlkSize)+BlkOffset, pData, NULL, nSec, 0))
                result = FTL_ERROR;
        }
        else
        {
            if (FTL_OK != FtlReadMskPage(((uint32)pRwBlk->SrcPBA<<pFtl->LogBlkSize)+BlkOffset, ReadMask, pTmpBuf))
                result = FTL_ERROR;
            
            memcpy(pData, pTmpBuf+PageOffset*512, nSec*512);
        }
    }

    return result;
}

/*
Name:       FtlRwBlkRead
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlRwBlkRead(pRW_BLK_INFO pRwBlk, uint32 index, uint16 nSec, uint8 *pData)
{
    int32 result = FTL_OK;
    uint16 len, PageOffset;
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 PageSize = pFtl->SecPerPage;

    PageOffset = index & pFtl->PageMask;
    if (PageOffset)
    {
        len = PageSize - PageOffset;
        len = (len>nSec)? nSec:len;
        if (FTL_OK != FtlRwReadPage(pRwBlk, index, len, pData))
        {
            result = FTL_ERROR;
        }
        pData += len*512;
        index += len;
        nSec -= len;
    }

    while (nSec >= PageSize)
    {
        if (FTL_OK != FtlRwReadPage(pRwBlk, index, PageSize, pData))
        {
            result = FTL_ERROR;
        }
            
        pData += PageSize*512;
        index += PageSize;
        nSec -= PageSize;
    }

    if (nSec)
    {
        if (FTL_OK != FtlRwReadPage(pRwBlk, index, nSec, pData))
        {
            result = FTL_ERROR;
        }
    }

    return result;
}

/*
Name:       FtlRwWritePage
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlRwWritePage(pRW_BLK_INFO pRwBlk, uint32 index, uint16 nSec, uint8 *pData)
{
    uint32 i, PBA, mask, WriteMask, PageMask;
    uint16 spare[16];
    uint16 PageOffset, PageLBA;
    uint8* pTmpBuf = (uint8*)pFtlTmpBuf;
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 BlkOffset = (uint16)index & pFtl->BlockMask;

    PageLBA = BlkOffset>>pFtl->LogPageSize;
    PageOffset = BlkOffset & pFtl->PageMask;
    WriteMask = ((-1 << nSec) ^ -1) << PageOffset;      //要写的扇区的mask
    PageMask = ((-1 << pFtl->SecPerPage) ^ -1);

    if (WriteMask != PageMask)                          //如果要写的不是整个PAGE
    {   
        mask = 0;
        if (pRwBlk->mask[PageLBA])                      //如果这个逻辑PageLBA之前有被写过                    
        {
            mask = pRwBlk->mask[PageLBA] | WriteMask;
            mask = mask ^ WriteMask;                    //找出PAGE中之前写过, 而当前没写的扇区
            if (mask)
            {
                WriteMask |= mask;                      //记录所有要写的扇区的mask
                PBA = FltRwGetPBA(pRwBlk, PageLBA);
                FtlReadMskPage(PBA, mask, pTmpBuf);        //读出有效的扇区, 准备写到新的PAGE      
            }
        }

        if ( mask || PageOffset )
        {
            memcpy(pTmpBuf+PageOffset*512, pData, nSec*512);
            pData = NULL;
        }
    }       

    i = pRwBlk->nPage>>pFtl->LogPageBlk;                // i 值表示当前 RW BLOCK 写到第i个物理BLOCK
    if (!(pRwBlk->nPage & pFtl->PageBlkMask))           //新写一个BLOCK
    {
        pRwBlk->ver++;
        pRwBlk->PBA[i] = QueueOut(&pFtl->FreeBlkInfo);
        NandBlkErase((uint32)pRwBlk->PBA[i]<<pFtl->LogBlkSize, NULL);
    }

    spare[0]=SIGN_RW_BLK;                             //表示数据块
    spare[1]=pRwBlk->ver;
    spare[2]=pRwBlk->LBA;
    spare[3]=(i>0)? pRwBlk->PBA[i-1]:pRwBlk->SrcPBA;    //用于掉电恢复
    spare[4]=PageLBA;
    spare[5]=(uint16)WriteMask;
    spare[6]=(uint16)(WriteMask>>16);
    spare[7]=0;
    
    PBA = ((uint32)pRwBlk->PBA[i]<<pFtl->LogBlkSize) + ((pRwBlk->nPage&pFtl->PageBlkMask)<<pFtl->LogPageSize);
    NandProg(PBA, (pData != NULL)? pData:pTmpBuf, spare);
    pRwBlk->PagePBA[PageLBA] = pRwBlk->nPage; 
    pRwBlk->mask[PageLBA] = WriteMask;

    pRwBlk->nPage++;
    if ((pRwBlk->nPage>>pFtl->LogPageBlk) >= pRwBlk->MaxPBA)                //写完所有的物理BLOCK
        FtlRwBlkClose(pRwBlk);
    
    return (FTL_OK);    
}

/*
Name:       FtlRwBlkWrite
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlRwBlkWrite(pRW_BLK_INFO pRwBlk, uint32 index, uint16 nSec, uint8 *pData)
{

    uint16 len, PageOffset;
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 PageSize = pFtl->SecPerPage;

    #ifdef FTL_POWER_ON_FAST
    FtlSaveRemapTbl(0x55530000);
    #endif

    PageOffset = index & pFtl->PageMask;
    if (PageOffset)
    {
        len = PageSize - PageOffset;
        len = (len>nSec)? nSec:len;
        FtlRwWritePage(pRwBlk, index, len, pData);
        pData += len*512;
        index += len;
        nSec -= len;
        if ((0 == pRwBlk->valid) && (0 != nSec))
        {
            /*如果RwBlk写满的话,会自动关闭,所以此处需要判断pRwBlk->valid,
            如果还没写完需要重新打开一个RwBlk*/
            pRwBlk = FtlRwBlkOpen(index, FTL_OPEN_WR);
        }
    }

    while (nSec >= PageSize)
    {
        FtlRwWritePage(pRwBlk, index, PageSize, pData);        
        pData += PageSize*512;
        index += PageSize;
        nSec -= PageSize;
        if ((0 == pRwBlk->valid) && (0 != nSec))
        {
            pRwBlk = FtlRwBlkOpen(index, FTL_OPEN_WR);
        }
    }

    if (nSec)
    {
        FtlRwWritePage(pRwBlk, index, nSec, pData);
        //index += nSec;
        //nSec -= nSec;
    }

    return FTL_OK;
}


/*
Name:       FtlRwBlkFree
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlRwBlkFree(uint32 param)
{
    uint32 i;
    pRW_BLK_INFO pRwBlk;
    pFTL_INFO pFtl = &gFtlInfo;

    if (0 == param) //释放所有RwBlk
    {
        for(i=0, pRwBlk=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pRwBlk++)
        {
            if (pRwBlk->valid)
            {
                FtlRwBlkClose(pRwBlk);
            }         
        }
    }
    #if 0
    else if (1 == param)    //如果RwBlk 快使用完, 可以在后台任务释放部分老的RwBlk
    {
        uint32 MinCnt = MAX_RWBLK_NUM-1;
        pRW_BLK_INFO tmp = NULL;
        for(i=0, pRwBlk=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pRwBlk++)
        {
            if (pRwBlk->valid)
            {
                if (pRwBlk->count < MinCnt)
                {
                    MinCnt = pRwBlk->count;
                    tmp = pRwBlk;
                }
            }         
        }

        if (tmp && (tmp->count<4))
            FtlRwBlkClose(tmp);
    }
    #endif    
    return FTL_OK;
}


/*
Name:       FtlRwLogRecovery
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FtlRwLogRecovery(void)
{
    uint32 i, PBA, mask, PageMask;
    int32 j;
    uint16 PageLBA, spare[16];
    pRW_BLK_INFO pRwBlk;
    pFTL_INFO pFtl = &gFtlInfo;

    PageMask =  (-1 << pFtl->SecPerPage) ^ -1;
    for(i=0, pRwBlk=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pRwBlk++)
    {
        if (0 == pRwBlk->valid)
            break;
            
        memset(pRwBlk->PagePBA, 0, RW_LOG_SIZE);
        for (j=pRwBlk->nPage-1; j>=0; j--)
        {
            uint32 k = j >> pFtl->LogPageBlk;   //modify 2013-5-17 解决异常掉电, 系统死在这里
            PBA = ((uint32)pRwBlk->PBA[k]<<pFtl->LogBlkSize) + ((j & pFtl->PageBlkMask)<<pFtl->LogPageSize);
            NandReadEx(PBA, NULL, spare, 8, 0);
            mask = spare[5] | ((uint32)spare[6]<<16);
            (0!=mask)? (PageLBA = spare[4]):(PageLBA=j, mask=PageMask); //因为可能SWBLK 2 RWBLK spare[4]记录的可能不是PageLBA

            if (0 == pRwBlk->mask[PageLBA])                             //恢复最后一次写的记录
            {
                pRwBlk->PagePBA[PageLBA] = j; 
                pRwBlk->mask[PageLBA] = mask;
            }
        }            
    }

}

/*
Name:       FtlRwBlkRecovery
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static int32 FtlRwBlkRecovery(pRW_BLK_INFO pRwBlk, uint16 DstBlk, uint16 SrcBlk, uint16 nPage)
{
    int32 ret;
    uint32 i, j, PBA;
    uint16 spare[16];
    pFTL_INFO pFtl=&gFtlInfo;

    pRwBlk->PBA[MAX_PBA_1_RW-1] = DstBlk;
    for (i=1; i<MAX_PBA_1_RW; i++)
    {
        //要检查最后一页, 不能检查第一页, 因为可能SWBLK 2 RWBLK
        PBA = ((uint32)SrcBlk<<pFtl->LogBlkSize) + pFtl->SecPerBlk - pFtl->SecPerPage;  
        if (FTL_OK != NandReadEx(PBA, NULL, spare, 8, 0))
            return FTL_ERROR;
        
        if (SIGN_RW_BLK != spare[0]) //原块的最后一页可能没有被写
            break;

        pRwBlk->PBA[MAX_PBA_1_RW-1-i] = SrcBlk;         //恢复 pRwBlk->PBA[]信息
        SrcBlk = spare[3];
    }
    pRwBlk->SrcPBA = SrcBlk;

    if (i<MAX_PBA_1_RW) //这时i 的值就是 pRwBlk->PBA[] 的数目
    {
        for (j=0; j<i; j++) //把PBA 整理到 pRwBlk->PBA[0] 开始
        {
            pRwBlk->PBA[j] = pRwBlk->PBA[MAX_PBA_1_RW-i+j];
            pRwBlk->PBA[MAX_PBA_1_RW-i+j] = 0;            
        }
    }

    pRwBlk->nPage = ((i-1)<<pFtl->LogPageBlk)+nPage;
    pRwBlk->MaxPBA = MAX_PBA_1_RW;//FltRwGetMaxPBA(pRwBlk->LBA);

    #if 0   //pRwBlk->log 与RemapTbl 用的BUF 有冲突, 延后恢复
    FtlRwLogRecovery(void);
    #endif
    pRwBlk->valid = 1;
    return (FTL_OK);
}

/*
Name:       FtlCacheBlkInit
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static int32 FtlCacheBlkInit(void)
{
    pFTL_INFO pFtl = &gFtlInfo;

    memset(&pFtl->CacheBlkInfo, 0, sizeof(CACHE_BLK_INFO));
}

/*
Name:       FtlCacheBlkRead
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static int32 FtlCacheBlkRead(pSW_BLK_INFO pSwBlk, uint32 LBA, uint16 nSec, void *pData)
{
    int32 ret=FTL_OK;
    uint16 offset;
    pFTL_INFO pFtl = &gFtlInfo;
    pCACHE_BLK_INFO pCacheBlk = &pFtl->CacheBlkInfo;
    pCACHE_PAGE_INFO pCachePage = &pCacheBlk->PageInfo[pSwBlk->i];
    //uint16 BlkOffset = index & pFtl->BlockMask;

    if ((LBA >= pCachePage->LBA) && (LBA+nSec <= pCachePage->LBA+pCachePage->nSec)) //保证要读的扇区一定在CACHE BLOCK里面
    {
        uint32 PBA, i;
        
        offset = LBA-pCachePage->LBA;
        i = pCachePage->PagePBA >> pFtl->LogPageBlk;
        PBA = ((uint32)pCacheBlk->PBA[i]<<pFtl->LogBlkSize)+((pCachePage->PagePBA&pFtl->PageBlkMask)<<pFtl->LogPageSize)+offset;
        if (FTL_OK != NandReadEx(PBA, pData, NULL, nSec, 0))
            ret = FTL_ERROR;
    }
    else
    {	  
        ret = FTL_ERROR;
        //while(1);
    }
    
    return ret;
}


/*
Name:       FtlCacheBlkWrite
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlCacheBlkWrite(pSW_BLK_INFO pSwBlk, uint32 LBA, uint16 nSec, void *pData)
{
    uint32 i, PBA;
    uint16 spare[16];
    pFTL_INFO pFtl = &gFtlInfo;
    pCACHE_BLK_INFO pCacheBlk = &pFtl->CacheBlkInfo;
    pCACHE_PAGE_INFO pCachePage = &pCacheBlk->PageInfo[pSwBlk->i];
    uint8* pTmpBuf = (uint8*)pFtlTmpBuf;      //pData 有可能已经指向pFtlTmpBuf

    i = pCacheBlk->nPage >> pFtl->LogPageBlk;
    if ( !(pCacheBlk->nPage & pFtl->PageBlkMask))   //判断是否新写一个物理BLOCK
    {
        if (!pCacheBlk->PBA[i])
        {
            pCacheBlk->PBA[i] = QueueOut(&pFtl->FreeBlkInfo);
            NandBlkErase((uint32)pCacheBlk->PBA[i]<<pFtl->LogBlkSize, NULL);
        } 
    }
    
    spare[0] = SIGN_CACHE_BLK | i;
    spare[1] = pCacheBlk->ver;
    spare[2] = pCachePage->ver = pSwBlk->ver;
    spare[3] = LBA;
    spare[4] = (uint16)(LBA>>16);
    spare[5] = pCachePage->nSec = nSec;
    spare[6] = spare[7] = 0;
    pCachePage->LBA = LBA;

    PBA = ((uint32 )pCacheBlk->PBA[i]<<pFtl->LogBlkSize)+((pCacheBlk->nPage&pFtl->PageBlkMask)<<pFtl->LogPageSize);
    NandProg(PBA, pData, spare);
    pCachePage->PagePBA = pCacheBlk->nPage; 
    pCachePage->valid = 1;
    
    if (nSec)       //nSec为0时, 用于打无效标志, CACHE BLOCK 恢复时会用到
        pSwBlk->nSec = pSwBlk->nSec-(pSwBlk->nSec&pFtl->PageMask)+nSec; 

    pCacheBlk->nPage++;
    i = pCacheBlk->nPage >> pFtl->LogPageBlk;
    if (i>=MAX_CACHEBLK_NUM)                        //判断是否写满所有的物理BLOCK
    {
        pCACHE_PAGE_INFO tmp;
        uint32 j, nPage=0;
        uint16 blk = QueueOut(&pFtl->FreeBlkInfo);
    
        NandBlkErase((uint32)blk<<pFtl->LogBlkSize, NULL);
        for(j=0, tmp=pCacheBlk->PageInfo; j<MAX_SWBLK_NUM; j++, tmp++)
        {
            if (tmp->valid)         //将有效的CACHE PAGE 写到新的BLOCK
            {
                i = tmp->PagePBA >> pFtl->LogPageBlk;
                PBA = ((uint32 )pCacheBlk->PBA[i]<<pFtl->LogBlkSize)+((tmp->PagePBA&pFtl->PageBlkMask)<<pFtl->LogPageSize);
                NandReadEx(PBA, pTmpBuf, spare, pFtl->SecPerPage,  0);
                spare[0]= SIGN_CACHE_BLK; 
                spare[1]++;         //pCacheBlk->ver++
                NandProg(((uint32)blk<<pFtl->LogBlkSize)+(nPage<<pFtl->LogPageSize), pTmpBuf, spare);
                tmp->PagePBA = nPage;
                nPage++;
            }                
        }
        pCacheBlk->ver++;
        pCacheBlk->nPage = nPage;
        for(j=0; j<MAX_CACHEBLK_NUM; j++)
        {
            QueueIn(&pFtl->FreeBlkInfo, pCacheBlk->PBA[j]);
            pCacheBlk->PBA[j]=0;                               
        }                            
        pCacheBlk->PBA[0] = blk;
    }

    return (FTL_OK);
}

/*
Name:       FtlCacheBlkInvalid
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlCacheBlkInvalid(pSW_BLK_INFO pSwBlk)
{
    pFTL_INFO pFtl = &gFtlInfo;
    CACHE_PAGE_INFO *pPageInfo = &pFtl->CacheBlkInfo.PageInfo[pSwBlk->i];

    if (1==pPageInfo->valid)
    {
        if (!(pPageInfo->LBA & pFtl->BlockMask))    //此种情况, 要置无效标记, 用于掉电恢复
            FtlCacheBlkWrite(pSwBlk, pPageInfo->LBA, 0, NULL); //pPageInfo->nSec =0

        pPageInfo->valid = 0;
    }

    return FTL_OK;
}

/*
Name:       FtlCacheBlkFree
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlCacheBlkFree(pSW_BLK_INFO pSwBlk)
{

    uint16 spare[16];
    pFTL_INFO pFtl = &gFtlInfo;
    uint8 *pTmpBuf = (uint8*)pFtlTmpBuf;
    uint16 PageOffset = pSwBlk->nSec & pFtl->PageMask;

    if (PageOffset)
    {
        uint32 LBA, len;

        LBA = ((uint32)pSwBlk->LBA<<pFtl->LogBlkSize) + pSwBlk->nSec - PageOffset;
        FtlCacheBlkRead(pSwBlk, LBA, PageOffset, pTmpBuf);
        len = pFtl->SecPerPage-PageOffset;
        NandReadEx(((uint32)pSwBlk->SrcPBA<<pFtl->LogBlkSize)+pSwBlk->nSec, pTmpBuf+PageOffset*512, NULL, len, 0);               
        
        spare[0]=SIGN_SW_BLK;            //表示数据块
        spare[1]=pSwBlk->ver;
        spare[2]=pSwBlk->LBA;
        spare[3]=pSwBlk->SrcPBA;
        spare[4]=pSwBlk->PBA;
        spare[5]=spare[6]=spare[7]=0;
        NandProg(((uint32)pSwBlk->PBA<<pFtl->LogBlkSize)+pSwBlk->nSec-PageOffset, pTmpBuf, spare);
        pSwBlk->nSec += len;
        FtlCacheBlkInvalid(pSwBlk);   
    }        

    return FTL_OK;
}

/*
Name:       FtlCacheBlkRecovery
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FtlCacheBlkRecovery(void)
{
    uint32 j, PBA, CacheLBA;
    int32 i;
    uint16 spare[16];
    uint16 nPage, CacheCnt, LBACnt;
    pFTL_INFO pFtl = &gFtlInfo;
    pCACHE_BLK_INFO pCacheBlk = &pFtl->CacheBlkInfo;
    pSW_BLK_INFO pSwBlk;
    CACHE_PAGE_INFO PageInfo;
    //uint32 LBATbl[256*MAX_CACHEBLK_NUM];
    uint32 *LBATbl = pFtlTmpBuf;

    for (i=0; i<MAX_CACHEBLK_NUM; i++)
    {
        if (0==pCacheBlk->PBA[i])
            break;
    }

    if (0==i)
        return;

    i--;    //i--后的值为最后一个写的物理BLOCK 的索引
    nPage = FtlFindWrPages(pCacheBlk->PBA[i]);
    pCacheBlk->nPage = (i<<pFtl->LogPageBlk) + nPage;

    LBACnt=CacheCnt=0;
    for (i=pCacheBlk->nPage-1; i>=0; i--)
    {
		if (CacheCnt >= MAX_SWBLK_NUM)
            break;
        
        j = i>>pFtl->LogPageBlk;
        PBA = ((uint32)pCacheBlk->PBA[j]<<pFtl->LogBlkSize) + ((i & pFtl->PageBlkMask)<<pFtl->LogPageSize);
        if (FTL_OK != NandReadEx(PBA, NULL, spare, 6,  0))
            continue;

        CacheLBA = ((uint32)spare[4]<<16) | spare[3];
        for(j=0; j<LBACnt; j++)
        {
            if (CacheLBA == LBATbl[j])
                break;
        }
        if (j<LBACnt)   //已经记录
            continue;
        LBATbl[LBACnt++] = CacheLBA;

        if (!spare[5])  //PageInfo.nSec ==0,cache 记录blk 第一个page时存在这种情况
            continue;
        
        PageInfo.PagePBA = i;
        PageInfo.ver = spare[2];
        PageInfo.LBA = CacheLBA;
        PageInfo.nSec = spare[5];
        PageInfo.valid = 1;

        if (!(PageInfo.LBA & pFtl->BlockMask))
        {
            pSwBlk = FtlSwBlkOpen(PageInfo.LBA, pFtl->SecPerPage, FTL_OPEN_WR);
            if (pSwBlk)
            {
                memcpy(&pCacheBlk->PageInfo[pSwBlk->i], &PageInfo, sizeof(CACHE_PAGE_INFO));
                pSwBlk->nSec += PageInfo.nSec;
                CacheCnt++;
                continue;
            }
        }            
        
        for (j=0, pSwBlk=pFtl->SwBlkInfo; j<MAX_SWBLK_NUM; j++, pSwBlk++)
        {
            if (pSwBlk->valid && !pCacheBlk->PageInfo[j].valid)
            {
                uint32 LBA = ((uint32)pSwBlk->LBA<<pFtl->LogBlkSize) + pSwBlk->nSec;
                if (LBA==PageInfo.LBA && pSwBlk->ver==PageInfo.ver)
                {
                    memcpy(&pCacheBlk->PageInfo[j], &PageInfo, sizeof(CACHE_PAGE_INFO));
                    pSwBlk->nSec += PageInfo.nSec;
                    CacheCnt++;
                    break;                      //modify 2013-5-17 找到就break 提高效率
                } 
            }
        }
    }
}

/*
Name:       FltSwBlkInit
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FltSwBlkInit(void)
{
    uint32 i;
    pSW_BLK_INFO pSwBlk = gFtlInfo.SwBlkInfo;

    memset (pSwBlk, 0, sizeof(SW_BLK_INFO)*MAX_SWBLK_NUM);
    for (i=0; i<MAX_SWBLK_NUM; i++, pSwBlk++)
    {        
        pSwBlk->i = pSwBlk->count = i;
    }
}

/*
Name:       FtlSwBlk2RwBlk
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlSwBlk2RwBlk(pSW_BLK_INFO pSwBlk)
{
    uint32 i, PageMask;  

    pRW_BLK_INFO pRwBlk;
    pFTL_INFO pFtl = &gFtlInfo;

    FtlCacheBlkFree(pSwBlk);
    
    pRwBlk = FtlRwBlkOpen((uint32)pSwBlk->LBA<<pFtl->LogBlkSize, FTL_OPEN_SW2RW);
    pRwBlk->PBA[0] = pSwBlk->PBA;
    pRwBlk->ver = pSwBlk->ver;
        
    PageMask =  (-1 << pFtl->SecPerPage) ^ -1;
    for(i=0; i<(pSwBlk->nSec>>pFtl->LogPageSize); i++)
    {
        pRwBlk->PagePBA[i] = i;
        pRwBlk->mask[i] = PageMask;
        pRwBlk->nPage++;
    }
    pSwBlk->valid=0;

    return FTL_OK;
}

/*
Name:       FtlSwBlkClose
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlSwBlkClose(pSW_BLK_INFO pSwBlk)
{
    pFTL_INFO pFtl = &gFtlInfo;

    if (!pSwBlk->valid)
        return FTL_ERROR;

    FtlCacheBlkFree(pSwBlk);
   
    if (pSwBlk->nSec != pFtl->SecPerBlk)
    {
        uint16 len = pFtl->SecPerBlk-pSwBlk->nSec;
        uint32 SrcPBA = ((uint32)pSwBlk->SrcPBA<<pFtl->LogBlkSize) + pSwBlk->nSec;
        uint32 DstPBA = ((uint32)pSwBlk->PBA<<pFtl->LogBlkSize) + pSwBlk->nSec;
        
        NandCopyProg(SrcPBA, DstPBA, len, NULL);
        //pSwBlk->nSec += len;        
    }

    FtlGetRemapBlk(pSwBlk->LBA, pSwBlk->PBA);          //修改映射关系
    QueueIn(&pFtl->FreeBlkInfo, pSwBlk->SrcPBA);        //回收原数据块
    pSwBlk->valid=0;

    return FTL_OK;
}


/*
Name:       FtlSwBlkOpen
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static pSW_BLK_INFO FtlSwBlkOpen(uint32 index, uint16 nSec, uint32 flag)
{
    uint32 i;
    uint16 spare[4];
    uint16 BlkOffset, PageOffset, BlkLBA;
    pSW_BLK_INFO pSwBlk, pTmp;
    pFTL_INFO pFtl = &gFtlInfo;

    BlkLBA = index >> pFtl->LogBlkSize;
    BlkOffset = index & pFtl->BlockMask;
    for (i=0, pSwBlk=pFtl->SwBlkInfo; i<MAX_SWBLK_NUM; i++, pSwBlk++)
    {
        if (pSwBlk->valid && pSwBlk->LBA == BlkLBA)
        {
            if (FTL_OPEN_RD == flag)
                return pSwBlk;

            PageOffset = pSwBlk->nSec & pFtl->PageMask;
            if ((BlkOffset >=pSwBlk->nSec) 
                && (BlkOffset <= (pSwBlk->nSec-PageOffset+pFtl->SecPerPage)))   //判断是否还是连续写        
            {
                break;            
            }
            else                                                                //不是连续写
            {
                if (pSwBlk->nSec > (pFtl->SecPerBlk>>2)*3)                      //BLOCK 已经被写了 超过 75% 就关闭释放
                    FtlSwBlkClose(pSwBlk);
                else
                    FtlSwBlk2RwBlk(pSwBlk);                       

                return (NULL);
            }            
        }
    }

    if (FTL_OPEN_RD == flag)
        return (NULL);

    if (i>=MAX_SWBLK_NUM)
    {        
        if (!(index&pFtl->BlockMask) && (nSec>=pFtl->SecPerPage || nSec>=16))    /*判断是否符合打开一个SW BLOCK的条件*/
        {
            pRW_BLK_INFO pRwBlk;
            for(i=0, pRwBlk=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pRwBlk++)
            {
                if (pRwBlk->valid && pRwBlk->LBA == BlkLBA)
                    return (NULL);
            }    
        }
        else
            return (NULL);
            

        for (i=0, pSwBlk=pFtl->SwBlkInfo; i<MAX_SWBLK_NUM; i++, pSwBlk++)
        {
            if (0 == pSwBlk->valid)         //找到一个未使用的  SW BLOCK  
                break;    
        }

        if (i>=MAX_SWBLK_NUM)
        {
            for (i=0, pSwBlk=pFtl->SwBlkInfo; i<MAX_SWBLK_NUM; i++, pSwBlk++)
            {
                if (0 == pSwBlk->count)     //关闭最少使用的 SW BLOCK
                {
                    FtlSwBlkClose(pSwBlk);
                    break;
                }
            }
        }
        
        pSwBlk->LBA = BlkLBA;
        pSwBlk->nSec = 0;
        pSwBlk->PBA = QueueOut(&pFtl->FreeBlkInfo);
        NandBlkErase((uint32)pSwBlk->PBA<<pFtl->LogBlkSize, NULL);
        pSwBlk->SrcPBA = FtlGetRemapBlk(BlkLBA, 0);
        NandReadEx(((uint32)pSwBlk->SrcPBA<<pFtl->LogBlkSize)+1, NULL, spare, 1, 0);
        pSwBlk->ver=spare[0]+1;
        pSwBlk->valid = 1;
    }

    /*最接近访问块处理, 写的时候才起作用*/
    for(i=0, pTmp=pFtl->SwBlkInfo; i<MAX_SWBLK_NUM; i++, pTmp++)    
    {
        if (pTmp->count > pSwBlk->count)
        {
            pTmp->count--;
        }
    }
    pSwBlk->count = MAX_SWBLK_NUM-1;

    return pSwBlk;
}

/*
Name:       FtlSwBlkWrite
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static int32 FtlSwBlkRead(pSW_BLK_INFO pSwBlk, uint32 index, uint16 nSec, uint8 *pData)
{
    int32 ret=FTL_OK;
    uint16 len, PageOffset;
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 BlkOffset = index & pFtl->BlockMask;

    if (BlkOffset<pSwBlk->nSec)         //判断要读的数据是否在 SW BLOCK 或 CACHE BLOCK内
    {
        PageOffset = pSwBlk->nSec & pFtl->PageMask;
        len = pSwBlk->nSec-BlkOffset;
        if (len > PageOffset)           //判断 SW BLOCK中是否有要读的数据
        {
            len = len-PageOffset;
            len = (len > nSec)? nSec : len;
            if (FTL_OK != NandReadEx(((uint32)pSwBlk->PBA<<pFtl->LogBlkSize)+BlkOffset, pData, NULL, len, 0))
            {
                ret = FTL_ERROR;
            }
            index += len;
            nSec -= len;
            pData += 512*len;
        }
        if (nSec>0 && PageOffset)       //判断 CACHE BLOCK是否含有要读的数据
        {
            len = PageOffset - (index & pFtl->PageMask);
            len = (len>nSec)? nSec:len;
            if (FTL_OK != FtlCacheBlkRead(pSwBlk, index, len, pData))
            {
                ret = FTL_ERROR;
            }
            index += len;
            nSec -= len;
            pData += 512*len;
        }        
    }

    if (nSec>0)                         //还有数据未读完, 未读完的数据一定在原块中
    {
        if (FTL_OK != NandReadEx(((uint32)pSwBlk->SrcPBA<<pFtl->LogBlkSize)+(index&pFtl->BlockMask), pData, NULL, nSec, 0))
        {
            ret = FTL_ERROR;
        }
        //index += nSec;
        //nSec -= len;
    }
    
    return ret;
}

/*
Name:       FtlSwBlkWrite
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlSwWritePage(pSW_BLK_INFO pSwBlk, uint8 *pData, uint16 nSec)
{
    uint16 spare[16];
    pFTL_INFO pFtl = &gFtlInfo;
    uint32 PBA;
    
    spare[0]=SIGN_SW_BLK;            //表示数据块
    spare[1]=pSwBlk->ver;
    spare[2]=pSwBlk->LBA;
    spare[3]=pSwBlk->SrcPBA;
    spare[4]=pSwBlk->PBA;               //只有SW BLK spare[4]=pSwBlk->PBA
    spare[5]=spare[6]=spare[7]=0;

    /*若cahce记录的是blk的第一个page, 且执行完后立即掉电, 先CacheBlkInvalid, 此page数据无法恢复
    若先NandProg后CacheBlkInvalid, 恢复代码会比较麻烦*/
    FtlCacheBlkInvalid(pSwBlk);     
    PBA = ((uint32)pSwBlk->PBA<<pFtl->LogBlkSize)+pSwBlk->nSec-(pSwBlk->nSec&pFtl->PageMask);
    NandProg(PBA, pData, spare);
    pSwBlk->nSec += nSec;

    if (pFtl->SecPerBlk==pSwBlk->nSec)
        FtlSwBlkClose(pSwBlk);       

    return (FTL_OK);    
}

/*
Name:       FtlSwBlkWrite
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static int32 FtlSwBlkWrite(pSW_BLK_INFO pSwBlk, uint32 index, uint16 nSec, uint8 *pData)
{
    uint16 PageOffset, BlkOffset;
    uint32 LBA, tmp, len = 0;
    pFTL_INFO pFtl = &gFtlInfo;
    uint8* pTmpBuf = (uint8*)pFtlTmpBuf;
    uint16 PageSize = pFtl->SecPerPage;

    #ifdef FTL_POWER_ON_FAST
    FtlSaveRemapTbl(0x55530000);
    #endif
   
    PageOffset = pSwBlk->nSec & pFtl->PageMask;
    BlkOffset = index & pFtl->BlockMask;
    if ((BlkOffset >= pSwBlk->nSec) 
        && (BlkOffset <= (pSwBlk->nSec-PageOffset+PageSize)))   //判断是否连续写
    {
        //LBA = index & ~((uint32)pFtl->PageMask);
        LBA = index-BlkOffset+pSwBlk->nSec-PageOffset;          //计算得到PAGE对齐的LBA
        if (PageOffset)                                         //判断CAHCE BLOCK 是否有数据需要写到新的PAGE中
        {           
            FtlCacheBlkRead(pSwBlk, LBA, PageOffset, pTmpBuf);
            len += PageOffset;    
        }

        if (BlkOffset > pSwBlk->nSec)                           //判断SRC BLOCK 是否有数据需要写到新的PAGE中
        {
            tmp = BlkOffset-pSwBlk->nSec;
            NandReadEx(((uint32)pSwBlk->SrcPBA<<pFtl->LogBlkSize)+pSwBlk->nSec, pTmpBuf+512*len, NULL, tmp, 0);
            len += tmp; 
        }

        if (len == PageSize)                                    //判断要写的数据是否已经有1个PAGE
        {
            FtlSwWritePage(pSwBlk, pTmpBuf, len-PageOffset);
            len -= PageSize;
        }
            
        if (len)                                                //判断CAHCE 或 SRC BLOCK 是否还有数据需要写到新PAGE中                                               
        {
            tmp = PageSize-len;
            tmp = (tmp > nSec)? nSec : tmp;
            memcpy(pTmpBuf+512*len, pData, tmp*512);
            len += tmp;
            
            if (len == PageSize)
                FtlSwWritePage(pSwBlk, pTmpBuf, len-PageOffset);
            else
                FtlCacheBlkWrite(pSwBlk, LBA, len, pTmpBuf);
            
            pData += tmp*512;
            index += tmp;
            nSec -= tmp;
        }

        while (nSec >= PageSize)
        {            
            FtlSwWritePage(pSwBlk, pData, PageSize);
            pData += PageSize*512;
            index += PageSize;
            nSec -= PageSize;
        }

        if (nSec)               //不足一个PAGE 的数据 写到CAHCE BLOCK
        {
            FtlCacheBlkWrite(pSwBlk, index, nSec, pData);
            //BlkOffset += nSec;
            //nSec -= nSec;
        }
    }
    else
    {
        return FTL_ERROR;
    }

    return FTL_OK;
}


/*
Name:       FtlWrBlkRecovery
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static int32 FtlWrBlkRecovery(uint16 *WriteBlk, uint32 *Bitmap, uint16 *RemapTbl)
{
    int32 i, j, RwBlkCnt, SwBlkCnt, ret;
    uint16 nPage, SrcBlk, DstBlk;
    uint16 spare[16];
    pFTL_INFO pFtl=&gFtlInfo;
    pSW_BLK_INFO pSwBlk = pFtl->SwBlkInfo;
    pRW_BLK_INFO pRwBlk = pFtl->RwBlkInfo; 

    RwBlkCnt=SwBlkCnt=0;
    for (i=0; i<MAX_FREE_BLK_NUM; i++)
    {
        DstBlk=WriteBlk[i*2+1];
        if (0 == DstBlk)
            break;

        SrcBlk=WriteBlk[i*2+0];

NEXT:
        nPage = FtlFindWrPages(DstBlk); //找出已经写了多少个PAGE
        do 
        {
            ret = NandReadEx(((uint32)DstBlk<<pFtl->LogBlkSize) + ((nPage-1)<<pFtl->LogPageSize), NULL, spare, 6, 0);
        }while(ret!=FTL_OK && (--nPage>0));
                   
        if (FTL_ERROR == ret)
        {
            #if 0
            NandReadEx(((uint32)SrcBlk<<pFtl->LogBlkSize), NULL, spare, 4, 0); //原块的最后一页可能没有写,所以要读第一页
            Bitmap[SrcBlk/32] |= 1ul<<(SrcBlk % 32);            
            Bitmap[DstBlk/32] &= ~(1ul<<(DstBlk % 32));                 //将没有完成的目标块释放 
            NandBlkErase((uint32)DstBlk<<pFtl->LogBlkSize, NULL);
            RemapTbl[spare[2]] = DstBlk = SrcBlk;           
            if (spare[1])                      //ver=0, 表示都没被写过,没必要恢复
            {
                SrcBlk = spare[3];
                nPage = pFtl->PagePerBlk;
                NandReadEx(((uint32)DstBlk<<pFtl->LogBlkSize) + ((nPage-1)<<pFtl->LogPageSize), NULL, spare, 8, 0);                      
            }
            else
            #endif
                continue;                            
        }

        if (spare[0]  == SIGN_RW_BLK)       
        {
            if (RwBlkCnt < MAX_RWBLK_NUM)   //RWBLk存在 nPage<=pFtl->PagePerBlk 情况,都要恢复
            {
                pRwBlk->ver = spare[1];
                pRwBlk->LBA = spare[2];
                if (FTL_OK != FtlRwBlkRecovery(pRwBlk, DstBlk, SrcBlk, nPage))
                    continue;
                
                for (j=0; j<MAX_PBA_1_RW; j++)
                {
                    if (0 == pRwBlk->PBA[j])
                        break;               
                    Bitmap[pRwBlk->PBA[j]/32] |= 1ul<<(pRwBlk->PBA[j] % 32);
                } 
                Bitmap[pRwBlk->SrcPBA/32] |= 1ul<<(pRwBlk->SrcPBA % 32);
                RemapTbl[pRwBlk->LBA] = pRwBlk->SrcPBA;
                pRwBlk++;
                RwBlkCnt++;
            }
        }
        else if (spare[0]  == SIGN_SW_BLK)
        {
            if (nPage < pFtl->PagePerBlk && SwBlkCnt < MAX_SWBLK_NUM) 
            {
                pSwBlk->ver = spare[1];
                pSwBlk->LBA = spare[2];
                pSwBlk->PBA = DstBlk;
                pSwBlk->SrcPBA = SrcBlk;
                pSwBlk->nSec = nPage<<pFtl->LogPageSize;                    
                pSwBlk->valid = 1;
                Bitmap[SrcBlk/32] |= 1ul<<(SrcBlk % 32);
                RemapTbl[pSwBlk->LBA] = SrcBlk;
                pSwBlk++;
                SwBlkCnt++;
            }
        }
        else if (spare[0] == SIGN_MW_BLK)
        {
            if (nPage < pFtl->PagePerBlk)
            {
                //RWBLK 合并过程中掉电, 合并目标块的spare[4] = pRwBlk->PBA[(pRwBlk->nPage-1)>>pFtl->LogPageBlk]
                SrcBlk = spare[4];
                //nPage = FtlFindWrPages(SrcBlk); 
                if (FTL_OK != NandReadEx(((uint32)SrcBlk<<pFtl->LogBlkSize), NULL, spare, 4, 0))
                    continue;

                Bitmap[SrcBlk/32] |= 1ul<<(SrcBlk % 32);
                Bitmap[DstBlk/32] &= ~(1ul<<(DstBlk % 32));                 //将没有完成的目标块释放
                NandBlkErase((uint32)DstBlk<<pFtl->LogBlkSize, NULL);       //擦除的目的是, 防止下次再被找到

                DstBlk = SrcBlk;
                SrcBlk = spare[3];
                goto NEXT;
            }
        }
    }

    return FTL_OK;
}

/*
Name:       FtlWrBlkFree
Desc:   
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
void FtlWrBlkFree(void)
{
    #if (ENABLE_DWCACHE)
    FtlDwCacheFree();
    #endif

    FtlRwBlkFree(0);
    FtlSaveRemap();
    
    NandSyncIO();
}

#endif

/*
Name:       FtlSaveRemap
Desc:       保存remap表格
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static void FtlSaveRemap(void)
{
    pFTL_INFO pFtl=&gFtlInfo;
    uint32 i, RawBlk, SecAddr;
    uint16 spare[16];
    uint32 BlkMask[2];
    uint16 NeedWritePages = 0; 
    uint16 RemapPageNum = (pFtl->TotalLogicBlk+512-1)/512;
    REMAP_INFO  *pRemapInfo = &pFtl->RemapInfo;
    
    //3查找需要回写的组
    for (i=0; i<pRemapInfo->max; i++)
    {
        if ((pRemapInfo->write[i/32]&(1<<(i%32))))
        {
            NeedWritePages++;
        }
    }
    
    if(NeedWritePages)
    {
        if ((pRemapInfo->PageAddr+(NeedWritePages<<pFtl->LogPageSize))>=pFtl->SecPerBlk)//写满一个块
        {
            RawBlk=pRemapInfo->BlkAddr;
            pRemapInfo->BlkAddr=QueueOut(&pFtl->FreeBlkInfo);//先queueOut 再Queue IN，避免使用同一个block
            QueueIn(&pFtl->FreeBlkInfo, RawBlk);
            NandBlkErase((uint32)pRemapInfo->BlkAddr<<pFtl->LogBlkSize, NULL);
            pRemapInfo->PageAddr=0-pFtl->SecPerPage;
        }
        //把当前在映射表里的BUF回写
        BlkMask[0]=BlkMask[1]=0;
        SecAddr=(uint32)pRemapInfo->BlkAddr<<pFtl->LogBlkSize;
        for (i=0; i<pRemapInfo->max; i++)
        {
            if(pRemapInfo->write[i/32]&(1<<(i%32)))
            {
                pRemapInfo->write[i/32]&=~(1<<(i%32));
                spare[0]=pRemapInfo->num[i];
                BlkMask[spare[0]/32] |= 1<<(spare[0]%32);
                pRemapInfo->PageAddr+=pFtl->SecPerPage;
                pRemapInfo->page[pRemapInfo->num[i]] = pRemapInfo->PageAddr;
                NandProg(SecAddr+pRemapInfo->PageAddr, &pRemapInfo->tbl[i*512], spare);
            }
        }

        //若写满一个BLOCK, 要找出所有最新的映射表并写入新BLOCK
        if (pRemapInfo->PageAddr==((NeedWritePages-1)<<pFtl->LogPageSize))
        {
            RawBlk=RawBlk<<pFtl->LogBlkSize;
            for (i=0; i<RemapPageNum; i++)
            {
                if ((BlkMask[i/32] & (1<<(i%32)))==0)
                {
                    NandReadEx(RawBlk+pRemapInfo->page[i], pRemapInfo->tbl, NULL, 2, 0);
                    pRemapInfo->num[0] = i; //ZYF tbl中的数据已经更新为 i对应的remap表了
                    spare[0] = i;
                    BlkMask[i/32] |= 1<<(i%32);
                    pRemapInfo->PageAddr += pFtl->SecPerPage;
                    pRemapInfo->page[i] = pRemapInfo->PageAddr;
                    NandProg((uint32)SecAddr+pRemapInfo->PageAddr, pRemapInfo->tbl, spare);
                }
            }
        }
    }
}


/*
Name:       FtlGetRemapBlk
Desc:       由逻辑块查找映射表得到物理块
Param:      逻辑块LBA, 设置目标块(0不设置)    
Return:     物理块(设置前)
Global: 
Note:
Author: 
Log:
*/
_ATTR_FTL_CODE_
static uint16 FtlGetRemapBlk(uint16 LBA, uint16 PBA)
{
    uint32 i, num;
    uint16 spare[16];
    uint16 old=0;
    pFTL_INFO pFtl=&gFtlInfo;
    REMAP_INFO  *pRemapInfo = &pFtl->RemapInfo;

    if (LBA < pFtl->TotalLogicBlk)
    {
        num=LBA/512;
        for (i=0; i<pRemapInfo->max; i++)
        {
            if (num==pRemapInfo->num[i])
                break;
        }
        if (i >= pRemapInfo->max)
        {
            //3查找没有打上写标记的组
            for (i=0; i<pRemapInfo->max; i++)
            {
                if ((pRemapInfo->write[i/32]&(1<<(i%32)))==0)
                    break;
            }

            if (i >= pRemapInfo->max)   //这个地方写代码没有在IRAM会死机
            {
                FtlSaveRemap();
                i=0;
            }
            NandReadEx(((uint32)pRemapInfo->BlkAddr<<pFtl->LogBlkSize)+pRemapInfo->page[num], &pRemapInfo->tbl[i*512], spare, 2, 0);
            pRemapInfo->num[i]= (uint8)num;
        }
        old=pRemapInfo->tbl[i*512+(LBA%512)];
        if (PBA!=0 && PBA<pFtl->MaxPhyBlk)
        {
            pRemapInfo->tbl[i*512+(LBA%512)]=PBA;
            pRemapInfo->write[i/32] |= 1<<(i%32);
        }
    }
    return (old);
}

/*
Name:       FtlGetRemap
Desc:       由逻辑扇区LBA查找映射表，找出对应的物理扇区PBA
Param:      LBA--逻辑扇区地址, mod=0表示直接映射, mod!=0表示从交换块里映射
Return:     PBA--物理扇区地址
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static uint32 FtlGetRemap(uint32 LBA)
{
    uint32 PBA;
    uint16 BlkOffset, BlkLBA;
    pFTL_INFO pFtl=&gFtlInfo;

    BlkOffset = LBA & pFtl->BlockMask;
    BlkLBA = LBA >> pFtl->LogBlkSize;

    PBA=FtlGetRemapBlk(BlkLBA, 0);
    if (PBA==0)
    {
        PBA=QueueOut(&pFtl->FreeBlkInfo);
        FtlGetRemapBlk(BlkLBA, PBA);
    }

    PBA = (PBA<<pFtl->LogBlkSize)+BlkOffset;
    return (PBA);
}

/*
Name:       FtlGetUnremap
Desc:       由物理扇区PBA查找映射表，找出对应的逻辑块LBA
Param:      PBA--物理扇区地址
Return:     LBA--逻辑块地址
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static uint32 FtlGetUnremap(uint32 PBA)
{
    uint32 LBA;
    pFTL_INFO pFtl=&gFtlInfo;

    PBA = PBA >>pFtl->LogBlkSize;
    for (LBA=0; LBA<pFtl->TotalLogicBlk; LBA++)
    {
        if (FtlGetRemapBlk(LBA, 0) == PBA)
            break;
    }
    return (LBA);
}


/*
Name:       RemapTblInit
Desc:       建立映射表  
Param:      映射表缓冲区      
Return:     映射表或写入临时块 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FtlRemapTblInit(uint16 *RemapTbl, uint32 flag)
{
    uint32 i, blk;
    uint16 RandCount;
    uint16 spare[16];
    pFTL_INFO pFtl=&gFtlInfo;
    REMAP_INFO  *pRemapInfo = &pFtl->RemapInfo;

    pRemapInfo->write[0]=0;
    pRemapInfo->write[1]=0;
    pRemapInfo->max=MAX_REMAP_TBL/1024;
    for (i=0; i<pRemapInfo->max; i++)
    {
        memcpy(&pRemapInfo->tbl[i*512], &RemapTbl[i*512], 1024);
        pRemapInfo->num[i]=i;
        pRemapInfo->write[i/32] |= 1<<(i%32);
    }

    if (1 == flag)
        return;

#ifndef IN_LOADER
    //3 Init frist free block
    RandCount =  nvic->SysTick.Value;
    if(pFtl->FreeBlkInfo.count)
    {
        RandCount =  RandCount % pFtl->FreeBlkInfo.count;
        while(RandCount--)
        {
            blk = QueueOut(&pFtl->FreeBlkInfo);
            QueueIn(&pFtl->FreeBlkInfo,(uint16)blk);
        }
    }

    pRemapInfo->BlkAddr=0;
#ifndef FTL_POWER_ON_FAST //快速开机时，全部都需要一个remap表格
    if (MAX_REMAP_TBL/2 < pFtl->TotalLogicBlk)
#endif
    {
        blk=QueueOut(&pFtl->FreeBlkInfo);
        if (blk != 0)
        {
            pRemapInfo->BlkAddr=(uint16)blk;
            blk = blk <<pFtl->LogBlkSize;
            NandBlkErase(blk, NULL);
            for (i=0; i<(pFtl->TotalLogicBlk+512-1)/512; i++)
            {
                spare[0]=(uint16)i;
                pRemapInfo->PageAddr=i<<pFtl->LogPageSize;
                pRemapInfo->page[i] = pRemapInfo->PageAddr;
                NandProg(blk+pRemapInfo->PageAddr, &RemapTbl[i*512], spare);
            }
            pRemapInfo->write[0]=0;
            pRemapInfo->write[1]=0;
        }
    }
#endif
}


/*
Name:       FtlLostPwrHandle
Desc:       判断是否坏块并进行替换算法       
Param:
            mod=0x55530000表示清除
            mod=0x55531111表示设置
            mod=0x55532222表示恢复
            srcAddr:需要拷贝的原扇区地址
            destAddr:需要拷贝的目标扇区地址
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void FtlLostPwrHandle(uint32 mod, uint32 SrcAddr, uint32 DstAddr)
{
    pFTL_INFO pFtl=&gFtlInfo;
    uint32 ptr[512/4];
    uint16 spare[16];
    uint32 i, src=0, dst=0;
    uint32 RcvBlkAddr=pFtl->BadBlkInfo.BlkAddr[MAX_RSVD_BLK_NUM-2]<<pFtl->LogBlkSize;

    for (i=0; i<pFtl->SecPerBlk; i+=pFtl->SecPerPage)
    {
        if (FTL_OK == NandReadEx(RcvBlkAddr+i, ptr, spare, 1, 0))
        {
            if (spare[0]==SIGN_RCV_BLK)
            {
                src=ptr[1];
                dst=ptr[2];
            }
            else
                break;
        }
    }

    if (mod == 0x55530000)      //clear
    {
        ptr[0]=0;
        ptr[1]=0;
        ptr[2]=0;
        spare[0]=SIGN_RCV_BLK;
        NandProg(RcvBlkAddr+i, ptr, spare);
        i+=pFtl->SecPerPage;
        if (i >= pFtl->SecPerBlk)
        {
            NandBlkErase(RcvBlkAddr, NULL);   //擦除标记
        }
    }
    else if (mod == 0x55531111) //set
    {
        ptr[0]=0x55531111;
        ptr[1]=SrcAddr;
        ptr[2]=DstAddr;
        spare[0]=SIGN_RCV_BLK;

        if (i+pFtl->SecPerPage >= pFtl->SecPerBlk)
        {
            NandBlkErase(RcvBlkAddr, NULL);   //擦除标记
            i = 0;
        }
        NandProg(RcvBlkAddr+i, ptr, spare);
        i+=pFtl->SecPerPage;
    }
    else if (mod == 0x55532222) //check
    {
        if ((i > 0) && (src != dst))
        {
            NandSysProtSet(FLASH_PROT_MAGIC);
            NandBlkErase(dst, NULL);
            NandCopyProg(src, dst, pFtl->SecPerBlk, NULL);     //数据恢复
            NandBlkErase(RcvBlkAddr, NULL);   //擦除标记
            NandSysProtSet(0);
        }
    }
}


/*
Name:       GetRemapTbl
Desc:       上电时恢复映射块/交换块/CACHE块
Param:      
Return:     FTL_OK正常,FTL_ERROR找不到映射块 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static int32 FtlGetRemapTbl(uint16 *pBadBlkTbl, uint32 flag)
{
    int32 ret = FTL_OK;
    uint32 i, j, WriteBlkCnt;
    uint16 ver, CacheVer, nPage, LBA, PBA, SrcBlk, DstBlk, CodeEndLBA;
    uint16 spare[16];
    pFTL_INFO pFtl=&gFtlInfo;
    
    uint16 WriteBlk[MAX_FREE_BLK_NUM*2];
    uint32 Bitmap[MAX_BLK_NUM/32];
    uint16 *RemapTbl = (uint16*)gFtlBuf[0];   //最大占用4条CACHE 32kb(@16K block)
    pBAD_BLK_INFO pBadBlk=&pFtl->BadBlkInfo;

    WriteBlkCnt = CacheVer = 0;
    memset (Bitmap, 0x00, pFtl->MaxPhyBlk/8);
    memset(RemapTbl, 0x00, pFtl->TotalLogicBlk*2);
    memset (WriteBlk, 0x00, sizeof(WriteBlk));
    CodeEndLBA = (uint16)(FtlGetCapacity(DISK_NAND_CODE)>>pFtl->LogBlkSize);
    
    FtlLostPwrHandle(0x55532222, 0, 0);

    //2以下这段恢复映射块和CACHE块, 2GB/580ms(@4KB/page)
    for (i=pBadBlk->BlkAddr[MAX_RSVD_BLK_NUM-1]+1; i<pFtl->MaxPhyBlk; i++)
    {
        for (j=0; j<pBadBlk->cnt; j++)   //不能是坏块
        {
            if (i==pBadBlkTbl[j])
            {
                Bitmap[i/32] |= 1ul<<(i % 32);
                break;
            }
        }
        if (j < pBadBlk->cnt)
            continue;

        if (FTL_OK == NandReadEx(i<<pFtl->LogBlkSize, NULL, spare, 4, (i+1)<<pFtl->LogBlkSize))
        {
            if ((spare[0] & 0xff00) == SIGN_CACHE_BLK)   //CACHE块的恢复
            {
                if (spare[1] >= CacheVer)
                {
                    if (spare[1] > CacheVer)
                    {
                        pFtl->CacheBlkInfo.ver = CacheVer = spare[1];
                        for (j=0; j<MAX_CACHEBLK_NUM; j++)
                        {
                            PBA = pFtl->CacheBlkInfo.PBA[j];
                            if (PBA)
                            {
                                Bitmap[PBA/32] &= ~(1ul<<(PBA % 32));   //释放旧块
                                pFtl->CacheBlkInfo.PBA[j] = 0;     
                            }
                        }
                    }                                                           
                    pFtl->CacheBlkInfo.PBA[spare[0]&0xff] = i;
                    Bitmap[i/32] |= 1ul<<(i % 32);        
                }                                  
            }
            else if ((spare[0] & 0xff00) == SIGN_DATA_BLK)
            {
                LBA=spare[2];// | ((uint32)spare[5*8]<<16);
                if (LBA < pFtl->TotalLogicBlk)
                {
                    PBA=RemapTbl[LBA];
                    if (PBA == 0)  //还没有记录, 不作判断直接记录
                    {
                        Bitmap[i/32] |= 1ul<<(i % 32);
                        RemapTbl[LBA]=i;
#ifdef IN_LOADER
                        if (1==flag && LBA >= CodeEndLBA)    //在loader里只收到系统盘即可退出
                        {
                            j = i;
                            //根据bitMap表, BLOCK号对就的比特值为0的进FREE块,升级失败时，有可能remap表丢失，从free块里面恢复
                            for (i=pBadBlk->BlkAddr[MAX_RSVD_BLK_NUM-1]+1; i<j; i++)
                            {
                                if ((Bitmap[i/32] & (1<<(i%32)))==0)
                                    QueueIn(&pFtl->FreeBlkInfo, i);
                            }
                            
                            for(i=0; i<CodeEndLBA; i++)
                            {
                                //升级过程SIGN_DATA_BLK 标记被擦除, 但第一页的数据还没来得及写下去就断电咯, 就会出现RemapTbl[i] == 0
                                //FtlGetRemap 会处理 RemapTbl[i] == 0 情况, 会从Free blk 取出一块
                                if(RemapTbl[i] == 0 && pFtl->FreeBlkInfo.count == 0)
                                    return FTL_ERROR;
                            }
                            FtlRemapTblInit(RemapTbl, flag);
                            
                            return (FTL_OK);
                        }  
#endif					   
                    }
                    else
                    {
                        if(LBA < CodeEndLBA)
                        {//zyf 20091110 固件区不能有交换块
                            continue;
                        }
                        ver = spare[1];
                        SrcBlk = spare[3];
                        DstBlk = (uint16)i/*spare[4]*/;   //注意RwBlk spare[4] 记录的不是DstBlk
                        NandReadEx((uint32)PBA<<pFtl->LogBlkSize, NULL, spare, 4, (i+1)<<pFtl->LogBlkSize);
                        if (ver>spare[1] )//|| (uint16)(ver+0x8000)>(uint16)(spare[1]+0x8000)) //注意VER 大于OxFFFF时
                        {
                            Bitmap[PBA/32] &= ~(1ul<<(PBA % 32));   //释放旧块
                            Bitmap[i/32] |= 1ul<<(i % 32);          //记录新块
                            RemapTbl[LBA]=i;                        //更新映射块
                        }
                        else
                        {
                            SrcBlk=spare[3];                        //已记录的版本号更新, 用它来恢复交换块
                            DstBlk=PBA/*spare[4]*/;
                        }

                        for (j=0; j<WriteBlkCnt; j++)
                        {
                            if (PBA==WriteBlk[j*2+1])    //找到已记录的交换块
                            {
                                WriteBlk[j*2+0]=SrcBlk;
                                WriteBlk[j*2+1]=DstBlk;
                                break;
                            }
                        }
                        if (j >= WriteBlkCnt)
                        {
                            if (WriteBlkCnt<MAX_FREE_BLK_NUM)
                            {
                                WriteBlk[WriteBlkCnt*2+0]=SrcBlk;
                                WriteBlk[WriteBlkCnt*2+1]=DstBlk;
                                WriteBlkCnt++;
                            }
                        }
                    }
                }
            }
        }
    }

    #ifndef IN_LOADER
    FtlWrBlkRecovery(WriteBlk, Bitmap, RemapTbl);
    #endif
    //根据bitMap表, BLOCK号对就的比特值为0的进FREE块
    for (i=pFtl->BadBlkInfo.BlkAddr[MAX_RSVD_BLK_NUM-1]+1; i<pFtl->MaxPhyBlk; )
    {
        if (Bitmap[i/32] != -1)
        {
            do
            {
                if ((Bitmap[i/32] & (1ul<<(i%32)))==0)
                    QueueIn(&pFtl->FreeBlkInfo, i);
                i++;
            }
            while ((i & 0x1f) != 0);
        }
        else
            i=(i | 0x1f)+1;
    }
      
    FtlRemapTblInit(RemapTbl, 0);

    #ifndef IN_LOADER
    FtlCacheBlkRecovery();  //FtlCacheBlkRecovery 会用到RemapTbl, 所以必须在FtlRemapTblInit之后
    FtlRwLogRecovery();  //pRwBlk->log 与RemapTbl 用的BUF 有冲突, 所以在RemapTbl释放后在恢复
    #endif
    return (ret);
}

#ifdef FTL_POWER_ON_FAST
/*
Name:       FtlLoadRemapTbl
Desc:       上电时恢复映射块/交换块/CACHE块
Param:    
Return:     FTL_OK正常,FTL_ERROR找不到映射块
Global: 
Note:       需要先调用函数GetBadBlkTbl后才能调用   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static int32 FtlLoadRemapTbl(void)
{
    int32 i, result = FTL_OK;
    pFTL_INFO pFtl=&gFtlInfo;
    uint16 spare[16];
    uint16 RcvRemapPageAddr;
    uint32 RcvBlkAddr;
    uint16 *ptr=(uint16*)gFtlBuf[1];   //只需要 4KB
    REMAP_INFO  *pRemapInfo = &pFtl->RemapInfo;

    pRemapInfo->RcvRemapValid = 0;
    RcvBlkAddr=(uint32)pFtl->BadBlkInfo.BlkAddr[MAX_RSVD_BLK_NUM-1]<<pFtl->LogBlkSize;
    for (i=0; i<pFtl->SecPerBlk; i+=pFtl->SecPerPage)
    {
        if (FTL_OK == NandReadEx(RcvBlkAddr+i, NULL, spare, 1, 0))
        {
            if (spare[0] == 0xFFFF)
                break;
        }
        else
        {
            result = FTL_ERROR;
            break;
        }
    }

    if((i > 0) && (result != FTL_ERROR))
    {
        RcvRemapPageAddr = (uint16)i;
        //pRemapInfo->RcvRemapPageAddr = RcvRemapPageAddr;
        if (FTL_OK == NandReadEx(RcvBlkAddr + i - pFtl->SecPerPage, ptr, spare, 8, 0))
        {
            if (spare[0] == SIGN_REMAP_BLK)
            {
                pFTL_INFO pInfo;
                pInfo = (pFTL_INFO)ptr;
                //3 判断数据有效
                if(pInfo->FtlVer == pFtl->FtlVer && pInfo->TotalLogicBlk == pFtl->TotalLogicBlk 
                    && pInfo->SecPerPage == pFtl->SecPerPage && pInfo->SecPerBlk == pFtl->SecPerBlk)
                {
                    //3 读4 K gFtlInfo 信息，坏块表和remap表可能读不全
                    //if (FTL_OK == NandReadEx(rcvBlkAddr + i - pFtlInfo->secPerPage, (uint16*)pFtlInfo, spare, 8, 0))
                    {
                        uint16 ReadPage, tmp = sizeof(FTL_INFO);
                        
                        tmp = (tmp > 512*8)? 512*8 : tmp;
                        memcpy(pFtl, ptr,  tmp);//读4 K gFtlInfo 信息，坏块表和部分remap表
                        
                        pRemapInfo->RcvRemapValid = 1;
                        pRemapInfo->RcvRemapPageAddr = RcvRemapPageAddr;
                        
                        ReadPage = (MAX_REMAP_TBL/2) /512;
                        if (MAX_REMAP_TBL/2 >= pFtl->TotalLogicBlk)
                        {
                            ReadPage = (pFtl->TotalLogicBlk+512-1)/512;
                        }
                        //3 read remap tbl
                        for (i=0; i < ReadPage; i++)
                        {
                            pRemapInfo->num[i] = i;
                            if(FTL_OK != NandReadEx(((uint32)pRemapInfo->BlkAddr<<pFtl->LogBlkSize)+pRemapInfo->page[i], &pRemapInfo->tbl[i*512], spare, 2, 0))
                            {
                                FtlVarInit();
                                pRemapInfo->RcvRemapValid = 0;
                                return FTL_ERROR;
                            }
                            if(spare[0] != i)
                            {
                                FtlVarInit();
                                pRemapInfo->RcvRemapValid = 0;
                                return FTL_ERROR;
                            }
                        }
                        if(i == ReadPage)
                        {
                            FltRwBlkInit();
                            return FTL_OK;
                        }
                    }
                    //else
                    //{
                    //     gFtlInfo.remapInfo.rcvRemapPageAddr = rcvRemapPageAddr;
                    //}
                }
            }
            else
            {   //remap表无效
                return FTL_ERROR;
            }
        }
        else
        {   // 读数据出错
            result = FTL_ERROR;
        }
    }
    
    if(result == FTL_ERROR)
    {
        NandSysProtSet(FLASH_PROT_MAGIC);
        NandBlkErase(RcvBlkAddr, NULL);   //擦除标记
        NandSysProtSet(0);
        pRemapInfo->RcvRemapPageAddr = 0;
    }
    return FTL_ERROR;
}

/*
Name:       FtlSaveRemapTbl
Desc:       关机时保存remap表
Param:      mod=0x55530000表示清除
            mod=0x55531111表示设置
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static void FtlSaveRemapTbl(uint32 mod)
{
    int32 result=FTL_OK;
    pFTL_INFO pFtl=&gFtlInfo;
    uint16 spare[16];
    uint16 blk;
    uint32 i, RcvBlkAddr;
    REMAP_INFO  *pRemapInfo = &pFtl->RemapInfo;

    if(mod == 0x55530000 && pRemapInfo->RcvRemapValid == 0)    //clear
    {
        return;
    }
    
    RcvBlkAddr=(uint32)pFtl->BadBlkInfo.BlkAddr[MAX_RSVD_BLK_NUM-1]<<pFtl->LogBlkSize;

    if(pRemapInfo->RcvRemapPageAddr >= pFtl->SecPerBlk)
    {
        NandSysProtSet(FLASH_PROT_MAGIC);
        NandBlkErase(RcvBlkAddr, NULL);   //擦除标记
        NandSysProtSet(0);
        pRemapInfo->RcvRemapPageAddr = 0;
    }
    

    if(mod == 0x55531111 &&(pRemapInfo->RcvRemapValid == 0))//set
    {
        FtlSaveRemap();        
    }

    spare[0] = SIGN_REMAP_BLK;
    if(mod == 0x55530000)    //clear
    {
        spare[0] = 0;
        pRemapInfo->RcvRemapValid = 0;
    }

    //3 无效时才回写
    if(pRemapInfo->RcvRemapValid == 0)
    {
        if(mod == 0x55531111)
        {
            pRemapInfo->RcvRemapValid  = 1;
        }        
        NandSysProtSet(FLASH_PROT_MAGIC);
        NandProg(RcvBlkAddr + pRemapInfo->RcvRemapPageAddr, (uint16*)pFtl, spare);
        NandSysProtSet(0);
        pRemapInfo->RcvRemapPageAddr += pFtl->SecPerPage;
    }
}
#endif


/*
Name:       FtlGetBadBlkTbl
Desc:       获取坏块信息表      
Param:      注意大小必须>=512B
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static int32 FtlGetBadBlkTbl(uint16 *BadTbl, uint32 flag)
{
    uint32 i, j;
    uint16 spare[16];
    int8 cnt;
    uint32 addr[2];
    pFTL_INFO pFtl=&gFtlInfo;
    pBAD_BLK_INFO pBadBlk=&pFtl->BadBlkInfo;
    uint16 *pBuf = (uint16 *)gFtlBuf[0];

    addr[0] = addr[1] = -1;
    cnt=(int8)-1;

    memset(pBadBlk, 0, sizeof(BAD_BLK_INFO));
    pBadBlk->max=MAX_BAD_BLK_NUM;
    memset(BadTbl, 0xff, MAX_BAD_BLK_NUM*2);
    
    for (i=pFtl->StartPhyBlk; i<pFtl->StartPhyBlk+50; i++)
    {
        for (j=0; j<pFtl->SecPerBlk; j+=pFtl->SecPerPage)
        {
            if (FTL_OK != NandReadEx((i<<pFtl->LogBlkSize)+j, NULL, spare, 1, 0))
            {
                continue;                
            }
            else
            {
                if (spare[0]==SIGN_BAD_BLK)   //找到坏块记录表
                {
                    if (j==0)
                        cnt++;
                    addr[cnt]=(i<<pFtl->LogBlkSize)+j;
                }
                else
                    break;
            }
        }
        if (cnt >= 1)
        {
            break;
        }
    }

    if (cnt<0)     //两个都没有找到失败
        return (FTL_ERROR);

    if ((addr[0]&pFtl->BlockMask) != (addr[1]&pFtl->BlockMask))   //正常情况2个映射表一样,取映射表0
    {
        if (addr[1] == -1)	        //当写表1且当擦完还没写时断电,取表0
        {
            i=addr[0];
        }
        else if (addr[0] == -1)
        {
            i=addr[1];
        }
        else
        {
            i=((addr[0]&pFtl->BlockMask) > (addr[1]&pFtl->BlockMask)) ? addr[0] : addr[1];
        }
    }
    else
        i=addr[0];

    NandReadEx(i, pBuf, spare, 8, 0);	//读好的一份坏块表
    
    for (j=0; j<pBadBlk->max; j++)
    {
        if (pBuf[j] != 0xffff)
            pBadBlk->cnt++;
        else
            break;
    }

    if (pBadBlk->cnt > (pFtl->MaxPhyBlk>>4))    //坏块数超出总block的1/16
    {
        return (FTL_ERROR);
    }

    memcpy(BadTbl, pBuf, pBadBlk->max*2);
    pBadBlk->BlkAddr[0]=spare[4];           //坏块表地址
    pBadBlk->BlkAddr[1]=spare[5];
    pBadBlk->BlkAddr[2]=spare[6];           //保存保留块地址
    pBadBlk->BlkAddr[3]=spare[7];

    pFtl->FtlVer=spare[1];
    pFtl->TotalLogicBlk=spare[2]+((uint32)spare[3]<<16);

    if (1==flag)
        return (FTL_OK);

    if ((pFtl->FtlVer>>8) != (FTL_VERSION>>8)) //主版本号不一致需要低格
    {
        return (FTL_ERROR);
    }
    
    if(pFtl->TotalLogicBlk > pFtl->MaxPhyBlk
    || pFtl->TotalLogicBlk < pFtl->MaxPhyBlk/2)
    {
        return (FTL_ERROR);
    }

    /*异常恢复*/
    if ((addr[0]&pFtl->BlockMask) != (addr[1]&pFtl->BlockMask))
    {
        if ((i>>pFtl->LogBlkSize)==spare[4])   //判断哪一份是好的, 先恢复坏的那一块
        {
            addr[0]=(uint32)spare[5]<<pFtl->LogBlkSize;     //第一份是好的, 先恢复第二份
            addr[1]=(uint32)spare[4]<<pFtl->LogBlkSize;
        }
        else
        {
            addr[0]=(uint32)spare[4]<<pFtl->LogBlkSize;
            addr[1]=(uint32)spare[5]<<pFtl->LogBlkSize;
        }
        NandSysProtSet(FLASH_PROT_MAGIC);
        NandBlkErase(addr[0], NULL);
        NandProg(addr[0], pBuf, spare);
        NandBlkErase(addr[1], NULL);
        NandProg(addr[1], pBuf, spare);
        NandSysProtSet(0);
    }

    pBadBlk->offset=addr[0] & pFtl->BlockMask;
    return (FTL_OK);
}

/*
Name:       FtlSetBadBlkTbl
Desc:       增加坏块到坏块表      
Param:      BlkNum=坏块号
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
static void FtlSetBadBlkTbl(uint16 blkNum)
{
    uint32 i;
    uint16 spare[16];
    uint16 BadBlkTbl[MAX_BAD_BLK_NUM]; 
    pFTL_INFO pFtl=&gFtlInfo;
    pBAD_BLK_INFO pBadBlk=&pFtl->BadBlkInfo;

    if (pBadBlk->cnt >= pBadBlk->max)
        return;

    NandReadEx(((uint32)pBadBlk->BlkAddr[0]<<pFtl->LogBlkSize)+pBadBlk->offset, BadBlkTbl,NULL, (MAX_BAD_BLK_NUM*2)>>9, 0);

    for (i=0; i<pBadBlk->cnt; i++)
    {
        if (BadBlkTbl[i]==blkNum)
            return;
    }

    spare[0]=SIGN_BAD_BLK;
    spare[1]=pFtl->FtlVer;
    spare[2]=pFtl->TotalLogicBlk & 0xffff;
    spare[3]=(pFtl->TotalLogicBlk>>16) & 0xffff;
    for (i=0; i<MAX_RSVD_BLK_NUM; i++)
        spare[(i+4)]=pBadBlk->BlkAddr[i];

    BadBlkTbl[pBadBlk->cnt]=blkNum;
    pBadBlk->cnt++;

    NandSysProtSet(FLASH_PROT_MAGIC);
    if (pBadBlk->offset >= pFtl->SecPerBlk)
    {
        pBadBlk->offset=0;
        NandBlkErase((uint32)pBadBlk->BlkAddr[0]<<pFtl->LogBlkSize, NULL);
        NandProg((uint32)pBadBlk->BlkAddr[0]<<pFtl->LogBlkSize, BadBlkTbl, spare);

        NandBlkErase((uint32)pBadBlk->BlkAddr[1]<<pFtl->LogBlkSize, NULL);
        NandProg((uint32)pBadBlk->BlkAddr[1]<<pFtl->LogBlkSize, BadBlkTbl, spare);
    }
    else
    {
        pBadBlk->offset+=pFtl->SecPerPage;
        NandProg(((uint32)pBadBlk->BlkAddr[0]<<pFtl->LogBlkSize)+pBadBlk->offset, BadBlkTbl, spare);
        NandProg(((uint32)pBadBlk->BlkAddr[1]<<pFtl->LogBlkSize)+pBadBlk->offset, BadBlkTbl, spare);
    }
    NandSysProtSet(0);
}

#ifdef IN_LOADER
/*
Name:       FtlChkBadBlk
Desc:       
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FtlChkBadBlk(uint16 * BadBlkTbl , uint16 * pBadBlkCnt)
{
    pFTL_INFO pFtl=&gFtlInfo;
    uint32 i, blk;
    uint32 BadBlkCnt = 0;

    *pBadBlkCnt = 0;
    #ifdef IN_LOADER
    for(blk=pFtl->StartPhyBlk; blk< pFtl->MaxPhyBlk; blk++)
    {
        if(NandChkBadBlk(blk<<pFtl->LogBlkSize) == FTL_ERROR)
        {
            BadBlkTbl[BadBlkCnt++] = blk;
        }

        //3  如果512 个blk里面超过256个坏块，不再检查坏块
        if(BadBlkCnt > 128 && blk < 512)
        {
            memset(BadBlkTbl, 0xFF, 2*BadBlkCnt);
            return;
        }
    }
    //3 如果坏块数超过总块数的1/16 , 那么说明flash可能被用过，不检查原厂坏块
    if(BadBlkCnt > (pFtl->MaxPhyBlk >> 4))
    {
        memset(BadBlkTbl, 0xFF, 2*BadBlkCnt);
        BadBlkCnt = 0;
    }
   // printf("\n");
    *pBadBlkCnt = BadBlkCnt;
    #endif
}



/*
Name:       FtlMakeBadBlkTbl
Desc:       建立坏块表     
Param:      
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FtlMakeBadBlkTbl(uint16 *BlkAddr, void *buf)
{
    uint32 i;
    uint16 spare[16];
    pFTL_INFO pFtl=&gFtlInfo;

    NandSysProtSet(FLASH_PROT_MAGIC);
    spare[0]=SIGN_BAD_BLK;
    spare[1]=FTL_VERSION;
    for (i=0; i<MAX_RSVD_BLK_NUM+2; i++)
        spare[(i+2)]=BlkAddr[i];

    NandBlkErase((uint32)BlkAddr[5]<<pFtl->LogBlkSize, NULL); //保留块可能存有快速开机的信息,也清除
    NandBlkErase((uint32)BlkAddr[2]<<pFtl->LogBlkSize, NULL);
    NandProg((uint32)BlkAddr[2]<<pFtl->LogBlkSize, buf, spare);
    
    NandBlkErase((uint32)BlkAddr[3]<<pFtl->LogBlkSize, NULL);
    NandProg((uint32)BlkAddr[3]<<pFtl->LogBlkSize, buf, spare);
    NandSysProtSet(0);

}

/*
Name:       LowFormat
Desc:       低级格式化      
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static int32 FtlLowFormat(uint16 *BadBlkTbl, uint16 BlkPBA, uint16 BlkLBA)
{
    uint32 i,j, BlkAddr, LBA, TotBlks;
    uint16 BadBlk[MAX_RSVD_BLK_NUM+2];
    uint16 spare[16];
    pFTL_INFO pFtl=&gFtlInfo;
    uint32 *pReadBuf = gFtlBuf[0];
    uint32 *pWriteBuf = gFtlBuf[1];
    pBAD_BLK_INFO pBadBlk=&pFtl->BadBlkInfo;
    uint8 BadTblFlag = TRUE;                    //表示是否成功获取到坏块表

    FlashSysProtAddr = 0;
    TotBlks=pFtl->MaxPhyBlk-pFtl->StartPhyBlk-pFtl->FreeBlkInfo.max-MAX_RSVD_BLK_NUM;
    memset(spare, 0xff, sizeof(spare));
    //memset(BadBlkTbl, 0xff, MAX_BAD_BLK_NUM*2); //在FtlGetBadBlkTbl 会执行此操作

    for (i=0; i<pFtl->SecPerPage*128; i++)
    {
        pWriteBuf[i] = i;
    }

    if (FTL_OK != FtlGetBadBlkTbl(BadBlkTbl, 1))   //已经成功获取了坏块表
    {
        BadTblFlag = FALSE;
        FtlChkBadBlk(BadBlkTbl, &pBadBlk->cnt);
    }
    TotBlks -= pBadBlk->cnt;
    if (TRUE == BadTblFlag)
        memcpy(&BadBlk[2], pBadBlk->BlkAddr, 2*MAX_RSVD_BLK_NUM);

    (BlkPBA == 0)? (BlkAddr=pFtl->StartPhyBlk, LBA=-MAX_RSVD_BLK_NUM): (BlkAddr = BlkPBA, LBA = BlkLBA);    

    for ( ;BlkAddr<pFtl->MaxPhyBlk; BlkAddr++)
    {
        for (j=0; j<pBadBlk->cnt; j++)   //不能是坏块
        {
            if (BlkAddr==BadBlkTbl[j])
            {
                break;
            }
        }
        if( j< pBadBlk->cnt)
            continue;
    
        CurEraseBlk = BlkAddr+1;
        if(LBA >= -MAX_RSVD_BLK_NUM)
        {
            if(BadTblFlag == FALSE)
            {
                if (FTL_OK != NandBlkErase(BlkAddr<<pFtl->LogBlkSize, NULL))
                {
                    TotBlks--;
                    if (pBadBlk->cnt < pBadBlk->max)
                        BadBlkTbl[pBadBlk->cnt++]=(uint16)BlkAddr;
                    continue;
                }
            }
            BadBlk[LBA+MAX_RSVD_BLK_NUM+2]=(uint16)BlkAddr;
            LBA++;
        }
        else
        {
            if (FTL_OK != NandBlkErase(BlkAddr<<pFtl->LogBlkSize, NULL))
            {
                TotBlks--;
                if (pBadBlk->cnt < pBadBlk->max)
                    BadBlkTbl[pBadBlk->cnt++]=(uint16)BlkAddr;
                continue;
            }
            else
            {
                if(LBA < TotBlks)
                {
                    spare[0]=SIGN_DATA_BLK;   //表示数据块
                    spare[1]=0;               //ver
                    spare[2]=LBA;
                    //spare[5*8]=LBA>>16;
                    NandProg(BlkAddr<<pFtl->LogBlkSize, pWriteBuf, spare);
                    if (FTL_OK!=NandReadEx(BlkAddr<<pFtl->LogBlkSize, pReadBuf, NULL, pFtl->SecPerPage, 0)
                            || memcmp(pReadBuf, pWriteBuf, pFtl->SecPerPage*512)!=0)
                    {
                        TotBlks--;
                        if (pFtl->BadBlkInfo.cnt < pFtl->BadBlkInfo.max)
                            BadBlkTbl[pFtl->BadBlkInfo.cnt++]=BlkAddr;
                        NandBlkErase(BlkAddr<<pFtl->LogBlkSize, NULL);
                        LBA--;
                    }                    

                }
                else    //free块也做读写测试
                {
                    spare[0]=0xFFFF;   //表示数据块
                    spare[1]=0xFFFF;   //ver
                    spare[2]=0xFFFF;
                    NandProg(BlkAddr<<pFtl->LogBlkSize, pWriteBuf, spare);
                    if (FTL_OK!=NandReadEx(BlkAddr<<pFtl->LogBlkSize, pReadBuf, NULL, pFtl->SecPerPage, 0)
                            || memcmp(pReadBuf, pWriteBuf, pFtl->SecPerPage*512)!=0)
                    {
                        TotBlks--;
                        if (pFtl->BadBlkInfo.cnt < pFtl->BadBlkInfo.max)
                            BadBlkTbl[pFtl->BadBlkInfo.cnt++]=BlkAddr;
                    }
                }
                LBA++;            
            }
        }
    }

    BadBlk[0]=TotBlks & 0xffff;
    BadBlk[1]=(TotBlks>>16) & 0xffff;
    FtlMakeBadBlkTbl(BadBlk, BadBlkTbl);
    return (FTL_OK);
}

/*
Name:       FtlLowFormatEx
Desc:       低级格式化      
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
int32 FtlLowFormatEx(void)
{
    uint16 BadBlkTbl[MAX_BAD_BLK_NUM];
    pFTL_INFO pFtl=&gFtlInfo;

    pFtl->valid = 0;
    CurEraseBlk = 0;

    FtlLowFormat(BadBlkTbl, 0, 0);
    FtlGetBadBlkTbl(BadBlkTbl, 0);
    FtlGetRemapTbl(BadBlkTbl, 1);

    FlashSysProtAddr = FtlGetRemap(FtlGetCapacity(DISK_NAND_CODE)-1);  //获取系统保留区地址
    pFtl->valid = 1;
    return FTL_OK;
}

#endif


/*
Name:       FtlVarInit
Desc:   
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FtlVarInit(void)
{
    uint32 tmp;
    pFLASH_SPEC pFS =&NandSpec;
    pFTL_INFO pFtl = &gFtlInfo;

    pFtl->SecPerPage = pFS->SecPerPage;
    pFtl->SecPerBlk = (uint16)pFS->SecPerBlock;
    pFtl->PagePerBlk = (uint16)pFS->PagePerBlock;

    pFtl->LogPageSize = (uint8)__Log2(pFtl->SecPerPage);
    pFtl->LogBlkSize = (uint8)__Log2(pFtl->SecPerBlk);
    pFtl->LogPageBlk = (uint8)__Log2(pFtl->PagePerBlk);

    pFtl->PageMask = pFtl->SecPerPage-1;
    pFtl->BlockMask = pFtl->SecPerBlk-1;
    pFtl->PageBlkMask = pFtl->PagePerBlk-1;

    pFtl->StartPhyBlk = NandIDBlkNum;
    pFtl->MaxPhyBlk = pFS->TotalPhySec>>pFtl->LogBlkSize;

    tmp=pFtl->MaxPhyBlk/32;
    tmp=(tmp>MAX_FREE_BLK_NUM) ? MAX_FREE_BLK_NUM : ((tmp<MIN_FREE_BLK_NUM) ? MIN_FREE_BLK_NUM : tmp);
    QueueInit(&pFtl->FreeBlkInfo, tmp);
    
    FilSys1stLBA = 0;
    pFtlTmpBuf = gFtlBuf[0];
    NandCopyProgBuf = gFtlBuf[1];

    #if (ENABLE_DWCACHE)
    pFtlCacheBuf = gFtlBuf[2];
    #endif

    #ifndef IN_LOADER
    FltSwBlkInit();
    FltRwBlkInit();
    FtlCacheBlkInit();
    #if (ENABLE_DWCACHE)
    FtlDwCacheInit();
    #endif
    #endif
}

/*
Name:       FTLInitExt
Desc:   
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FTLInitExt(void)
{
    uint16 BadBlkTbl[MAX_BAD_BLK_NUM];

    //FtlFormatUsrDsk(BadBlkTbl);
#ifdef IN_LOADER
    if (FTL_OK != FtlGetBadBlkTbl(BadBlkTbl, 0))
    {
FTLLOWFORMAT:
        CurEraseBlk = 0;
        USBBoot(); //先连接USB，在低格

        FtlLowFormat(BadBlkTbl, 0, 0);

        FtlGetBadBlkTbl(BadBlkTbl, 0);
        FtlGetRemapTbl(BadBlkTbl, 1);
        gFtlInfo.valid=1;
        FlashSysProtAddr=FtlGetRemap(FtlGetCapacity(DISK_NAND_CODE)-1);  //获取系统保留区地址
        while(1)
        {
            SysLowFormatCheck();        
            FW_HandleReq();
        };
    }

    if(FtlGetRemapTbl(BadBlkTbl, 1) == FTL_ERROR)
    {
        goto FTLLOWFORMAT;
    }
#else

    FtlGetBadBlkTbl(BadBlkTbl, 0);
    #ifdef FTL_POWER_ON_FAST
    if(FtlLoadRemapTbl()!= FTL_OK)
    #endif
	{
        FtlGetRemapTbl(BadBlkTbl, 0);
    }
	
    FtlRwBlkFree(0);
#endif
}

/*
Name:       FTLInit
Desc:       FTL和FLASH初始化
Param:  
Return:     0=正常返回
            -1=flash初始化失败或映射表出错,可能是硬件错误或要进行固件升级        
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
int32 FTLInit(void)
{
    uint32 ret=FTL_ERROR;
    pFTL_INFO pFtl = &gFtlInfo;

	#ifndef IN_LOADER    
    nvic->SysTick.Reload = 0x10000;
    nvic->SysTick.Ctrl |= NVIC_SYSTICKCTRL_CLKIN | NVIC_SYSTICKCTRL_ENABLE;

    OBTAIN_FTL_SEM;
	#endif    
    pFtl->valid=0;
    if (FTL_OK == NandInit())
    {
        #ifdef IN_LOADER
        MDSetCurDev(MEM_DEVID_NAND);
        #endif
		FtlVarInit();
        
        FTLInitExt();                
        #ifdef FTL_POWER_ON_FAST
        FtlSaveRemapTbl(0x55531111);
        #endif     
        pFtl->valid=1;

        FlashSysProtAddr=FtlGetRemap(FtlGetCapacity(DISK_NAND_CODE)-1);  //获取系统保留区地址
        #ifndef IN_LOADER
        FltSysCodeChk();
        #endif
        ret=FTL_OK;
    }

    #ifdef  FTL_READ_REFRESH_EN
    //固件区扫描,避免hynix flash只读block内部分数据，其他数据会出错
    gFtlReadScanSec = SysProgRawDiskCapacity;
    #endif

    //FtlTest();

    RELEASE_FTL_SEM;
    return (ret);
}

/*
Name:   FtlGetCapacity
Desc:   获取FLASH磁盘容量
Param:  盘符
Return: 容量,以扇区为单位
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
uint32 FtlGetCapacity(uint8 LUN)
{
    uint32 capacity, SysCap;

    capacity = SysCap=0;
    switch (LUN)
    {
        case DISK_NAND_CODE:
            capacity = SysProgDiskCapacity;
            break;
        case DISK_NAND_DATA:
            capacity = SysDataDiskCapacity;
            break;

        case DISK_NAND_USER2:
            capacity = SysUserDisk2Capacity;
            break;

        case DISK_NAND_USER:
            SysCap = SysProgDiskCapacity+SysDataDiskCapacity+SysUserDisk2Capacity;
        case DISK_NAND_TOTAL:
            capacity = (gFtlInfo.TotalLogicBlk<<gFtlInfo.LogBlkSize)-SysCap;
            break;
        default:
            break;
    }
    return (capacity);
}

/*
Name:	FtlSrcBlkRead
Desc: 	
Param:	
Return:		
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static int32 FtlSrcBlkRead(void *pSrcBlk, uint32 index, uint16 nSec, uint8 *pData)
{
    return NandReadEx((uint32)pSrcBlk, pData, NULL, nSec, 0);
}

/*
Name:	FtlOpenWrBlk
Desc: 	
Param:	
Return:		
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
static int32 FtlOpenBlk(FTL_HANDLE *pHandle, uint32 index, uint16 nSec, uint32 flag)
{
    pSW_BLK_INFO pSwBlk=NULL;
    pRW_BLK_INFO pRwBlk=NULL;
    pFTL_INFO pFtl = &gFtlInfo;

    #ifndef IN_LOADER
    pSwBlk = FtlSwBlkOpen(index, nSec, flag);
    if (pSwBlk)
    {
        pHandle->write = (FTL_WRITE)FtlSwBlkWrite;
        pHandle->read = (FTL_READ)FtlSwBlkRead;
        pHandle->pBlkInfo = pSwBlk;
        return FTL_OK;
    }
        
    pRwBlk = FtlRwBlkOpen(index, flag);
    if (pRwBlk)
    {
        pHandle->write = (FTL_WRITE)FtlRwBlkWrite;
        pHandle->read = (FTL_READ)FtlRwBlkRead;
        pHandle->pBlkInfo = pRwBlk;
        return FTL_OK;        
    }
    #endif

    if (FTL_OPEN_RD == flag)
    {
        pHandle->write = (FTL_WRITE)NULL;
        pHandle->read = (FTL_READ)FtlSrcBlkRead;
        pHandle->pBlkInfo = (void *)FtlGetRemap(index);
        return FTL_OK;
    }

    return FTL_ERROR;
}


/*
Name:   FtlRead
Desc:   读多扇区
Param:  LUN=逻辑分区号, Index=扇区号(LBA), nSec=扇区数
Return: 读到的数据在缓冲区buf中    
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
int32 FtlRead(uint8 LUN, uint32 LBA, uint32 size, void *buf)
{
    uint16 BlkOffset, len = 0;
    int32 result = FTL_OK;
    uint32 ImaskTemp;
    uint8 *ptr=(uint8*)buf;
    FTL_HANDLE handle;
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 BlkSize = pFtl->SecPerBlk;
    uint32 index = LBA;
    uint32 nSec = size;

    OBTAIN_FTL_SEM;
    ImaskTemp = UserIsrDisable();

    switch (LUN)
    {
        case DISK_NAND_USER2:
            index += FtlGetCapacity(DISK_NAND_USER);
        case DISK_NAND_USER:
            index += FtlGetCapacity(DISK_NAND_DATA);
        case DISK_NAND_DATA:
            index += FtlGetCapacity(DISK_NAND_CODE);
        case DISK_NAND_CODE:
            if ((index+nSec) > FtlGetCapacity(DISK_NAND_TOTAL))
                break;

            BlkOffset = (uint16)index & pFtl->BlockMask;                    
            if (BlkOffset)
            {
                len = BlkSize-BlkOffset;
                len = (len>nSec)? nSec:len;

                FtlOpenBlk(&handle, index, len, FTL_OPEN_RD);
                if (FTL_OK != handle.read(handle.pBlkInfo, index, len, ptr))
                {
                    result=FTL_ERROR;
                }
                index += len;
                nSec -= len;
                ptr += 512*len;
            }

            while (nSec >= BlkSize)
            {
                FtlOpenBlk(&handle, index, BlkSize, FTL_OPEN_RD);
                if (FTL_OK != handle.read(handle.pBlkInfo, index, BlkSize, ptr))
                {
                    result=FTL_ERROR;
                }
                index += BlkSize;
                nSec -= BlkSize;
                ptr += 512*BlkSize;
            }

            if (nSec != 0)
            {                               
                FtlOpenBlk(&handle, index, nSec, FTL_OPEN_RD);
                if (FTL_OK != handle.read(handle.pBlkInfo, index, nSec, ptr))
                {
                    result=FTL_ERROR;
                }
                //index += nSec;
                //nSec -= nSec;
            }
            
            #if (ENABLE_DWCACHE)
            if (pFtl->EnDWCache)
                FtlDwCacheRead(LUN, LBA, size, buf);
            #endif
            
            break;
        default:
            break;
    }
    UserIsrEnable(ImaskTemp);
    RELEASE_FTL_SEM;

    return (result);
}

/*
Name:	FtlWriteImage
Desc: 	写一个大PAGE
Param:	LUN=逻辑分区号, Index=扇区号(LBA), buf=数据缓冲区, nSec=扇区数
Return:		
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
int32 FtlWriteImage(uint8 LUN, uint32 Index, uint32 nSec, void *pData)
{
    uint32 i, PBA;
    int32 ret=FTL_ERROR;
    uint16 spare[16];
    pFTL_INFO pFtl = &gFtlInfo;

    memset(spare, 0xff, sizeof(spare));
    switch (LUN)
    {
        case DISK_NAND_DATA:
            Index += FtlGetCapacity(DISK_NAND_CODE);
         
        case DISK_NAND_CODE:
            if (DISK_NAND_CODE==LUN)
                NandSysProtSet(FLASH_PROT_MAGIC);
            
            for (i=0; i<nSec; i+=pFtl->SecPerPage)
            {
                PBA=FtlGetRemap(Index+i);
                spare[0]=SIGN_DATA_BLK;            //表示数据块
                spare[1]=0;
                spare[2]=(uint16)((Index+i)>>pFtl->LogBlkSize);
                
                if ((PBA & pFtl->BlockMask) == 0)
                    NandBlkErase(PBA, NULL);
                NandProg(PBA, (uint8*)pData+i*512, spare);                
            }

            NandSysProtSet(0);            
            ret=FTL_OK;
            break;
            
        default:
            ret=FTL_ERROR;
            break;
    }

    return (ret);
}

#ifndef IN_LOADER

/*
Name:	FtlWrite
Desc: 	写多个扇区
Param:	LUN=逻辑分区号, Index=扇区号(LBA), buf=数据缓冲区, nSec=扇区数
Return:		
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
#if (ENABLE_DWCACHE)
int32 FtlDoWrite(uint8 LUN, uint32 index, uint32 nSec, void *buf)
#else
int32 FtlWrite(uint8 LUN, uint32 index, uint32 nSec, void *buf)
#endif
{
    int32  len, result=FTL_ERROR;
    uint16 BlkOffset;
    uint32 ImaskTemp;
    uint8 *ptr=(uint8*)buf;
    FTL_HANDLE handle;
    pFTL_INFO pFtl = &gFtlInfo;
    uint16 BlkSize = pFtl->SecPerBlk;
    //uint32 index = LBA;
    
    OBTAIN_FTL_SEM;
    ImaskTemp = UserIsrDisable();

    switch (LUN)
    {
        case DISK_NAND_USER2:
            index += FtlGetCapacity(DISK_NAND_USER);
        case DISK_NAND_USER:
            index += FtlGetCapacity(DISK_NAND_DATA);
        case DISK_NAND_DATA:
            index += FtlGetCapacity(DISK_NAND_CODE);
        case DISK_NAND_CODE:
        #ifdef SYS_PROTECT
            if (index < FtlGetCapacity(DISK_NAND_CODE))
                break;
        #endif

            if ((index+nSec) > FtlGetCapacity(DISK_NAND_TOTAL))
                break;
            
            BlkOffset = (uint16)index & pFtl->BlockMask;                    
            if (nSec && BlkOffset)
            {
                len = BlkSize-BlkOffset;
                len = (len>nSec)? nSec:len;

                FtlOpenBlk(&handle, index, len, FTL_OPEN_WR);
                handle.write(handle.pBlkInfo, index, len, ptr);                
                index += len;
                nSec -= len;
                ptr += 512*len;
            }

            while (nSec >= BlkSize)
            {
                FtlOpenBlk(&handle, index, BlkSize, FTL_OPEN_WR);
                handle.write(handle.pBlkInfo, index, BlkSize, ptr); 

                index += BlkSize;
                nSec -= BlkSize;
                ptr += 512*BlkSize;
            }

            if (nSec != 0)
            {                               
                FtlOpenBlk(&handle, index, nSec, FTL_OPEN_WR);
                handle.write(handle.pBlkInfo, index, nSec, ptr); 
                //index += nSec;
                //nSec -= nSec;
            }

            result=FTL_OK;
            break;
        default:
            break;
    }

    UserIsrEnable(ImaskTemp);
    RELEASE_FTL_SEM;
    return (result);
}

/*
Name:       FltSysCodeChk
Desc:   
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
static void FltSysCodeChk(void)
{
    uint16 src[512];
    uint16 des[512];
    uint32 i,j;
    
    FtlRead(0, 0, 2, src);
    for(j=0; j<4; j++)
    {
        FtlRead(0, SysProgRawDiskCapacity, 2, des);
        for(i=0;i<512;i++)
        {
            if(src[i]!=des[i])
            {
                SysProgRawDiskCapacity -= 2048; //每次减1M
                break;
            }
        }
        if(i==512)
        {
            FlashSysCodeRawAddr = FtlGetRemap(SysProgRawDiskCapacity-1);  //获取系统RAW保留区地址
            return;
        }
    }
    SysProgRawDiskCapacity = 0;
    FlashSysCodeRawAddr = FlashSysProtAddr;  //获取系统RAW保留区地址
    return;
}

/*
Name:	FtlClose
Desc: 	处理一些flash cache回写等操作
Param:	
Return:		
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
void FtlClose(void)
{
    FtlWrBlkFree();

    #ifdef FTL_POWER_ON_FAST
    FtlSaveRemapTbl(0x55531111);
    #endif
    NandSyncIO();
}

/*
Name:   FtlSetDataDskSize
Desc:   设置数据盘大小，在FTLInit之后马上调用
Param:  数据盘大小
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
void FtlSetDataDskSize(int nMB)
{
    SysDataDiskCapacity =  (uint32)(nMB)<<11;

    FilSys1stLBA = (uint16)((SysProgDiskCapacity+SysDataDiskCapacity)>>gFtlInfo.LogBlkSize);
}

/*
Name:   SetUserDisk2Size
Desc:   设置数据盘大小，在FTLInit之后马上调用
Param:  数据盘大小
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_INIT_CODE_
void FtlSetUsrDsk2Size(int nMB)
{
    SysUserDisk2Capacity =  (uint32)(nMB)<<11;
}

/*
Name:       FtlProgErrHook
Desc:       FLASH写或擦除出错   
Param:      secAddr--出错块中的任一个页
            DataBuf--出错页的数据缓冲区
            nSec--数据扇区数(只能是SecPerPageRaw或SecPerPage)  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
uint32 FtlProgErrHook(uint32 SecAddr, void *DataBuf, uint16 *pSpare, uint16 nSec)
{
    uint32 i, offset, NewSecAddr;
    uint16 DstBlk, BadBlock;
    pFTL_INFO pFtl=&gFtlInfo;
    pSW_BLK_INFO pSwBlk=NULL;
    pRW_BLK_INFO pRwBlk=NULL;
    pCACHE_BLK_INFO pCacheBlk = NULL;
    //PRINTF("usb prog error at addr=%x\n", secAddr);
    if (pFtl->valid==0)
        return (SecAddr);

    BadBlock = SecAddr>>pFtl->LogBlkSize;
    if (BadBlock <= (FlashSysProtAddr>>pFtl->LogBlkSize)) //固件区 没有替换块
    {
        #if 0
        if (BadBlock <= pFtlInfo->badBlkInfo.blkAddr[MAX_RSVD_BLK_NUM-1] 
            && BadBlock >= pFtlInfo->badBlkInfo.blkAddr[0])
        {        
            NandBlkErase(BadBlock, NULL);
            pFtlInfo->valid = 0;    //防止再次替换
            NandProg(BadBlock, DataBuf, pSpare);
            pFtlInfo->valid = bakup;
        }
        #endif
        return (SecAddr);
    }

    DstBlk=QueueOut(&pFtl->FreeBlkInfo);
    if (0==DstBlk)  //没有可用的块可替换
    {
        return (SecAddr);
    }

    offset = FtlGetUnremap(SecAddr);
    if (offset < pFtl->TotalLogicBlk)
        FtlGetRemapBlk(offset, DstBlk);
        
    for (i=0, pSwBlk=pFtl->SwBlkInfo; i<MAX_SWBLK_NUM; i++, pSwBlk++)
    {
        if (pSwBlk->valid)
        {
            if (pSwBlk->PBA == BadBlock)  //查找坏块是否在交换块里
            {
                pSwBlk->PBA = DstBlk;
                break;
            }
        }
    }
    
    if (i>=MAX_SWBLK_NUM)
    {
        for (i=0, pRwBlk=pFtl->RwBlkInfo; i<MAX_RWBLK_NUM; i++, pRwBlk++)
        {
            if (pRwBlk->valid)
            {
                if (pRwBlk->PBA[pRwBlk->nPage>>pFtl->LogPageBlk] == BadBlock)
                {
                    pRwBlk->PBA[pRwBlk->nPage>>pFtl->LogPageBlk] = DstBlk;
                    break;
                }
            }
        }
    }

    if (i>= MAX_RWBLK_NUM)
    {
        for (i=0; i<MAX_CACHEBLK_NUM; i++)
        {
            if (pCacheBlk->PBA[i] == BadBlock)
            {
                pCacheBlk->PBA[i] = DstBlk;
                break;
            }
        }
    }

    if (pFtl->RemapInfo.BlkAddr == BadBlock)
    {
        pFtl->RemapInfo.BlkAddr = DstBlk;
    }
    
    NewSecAddr=(uint32)DstBlk<<pFtl->LogBlkSize;
    NandBlkErase(NewSecAddr,NULL);
    if (DataBuf != NULL)
    {
        uint16 spare[16];
        offset = SecAddr & pFtl->BlockMask;
        //从出错块中拷贝数据到替换块
        NandReadEx(SecAddr-offset, NULL, spare, 8, 0);
        NandCopyProg(SecAddr-offset, NewSecAddr, offset, NULL);
        NewSecAddr += offset;
        if (nSec != 0)
        {
            NandProg(NewSecAddr, DataBuf, (pSpare!=NULL)? pSpare:spare);
            NewSecAddr+=pFtl->SecPerPage;
        }
        //NandBlkErase(SecAddr);
    }
    FtlSetBadBlkTbl(BadBlock);

    return (NewSecAddr);
}

/*
Name:       FlashRefresh
Desc:       
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void FtlFlashRefresh(uint32 row)
{
    uint32 LBA;
    pFTL_INFO pFtl=&gFtlInfo;
    pSW_BLK_INFO pSwBlk;
    // TODO: loader区和固件区原地写
    if (row<=FlashSysProtAddr)
    {
        if(row<=FlashSysCodeRawAddr)    //第一份固件区
        {
            LBA= (uint32)QueueFront(&pFtl->FreeBlkInfo)<<pFtl->LogBlkSize;
            NandBlkErase(LBA, NULL);
            NandCopyProg(row, LBA, pFtl->SecPerBlk, NULL);
            NandSysProtSet(FLASH_PROT_MAGIC);
            FtlLostPwrHandle(0x55531111, LBA, row);
            NandBlkErase(row, NULL);
            NandCopyProg(LBA, row, pFtl->SecPerBlk, NULL);
            NandBlkErase(LBA, NULL); //将目标块擦除掉
            FtlLostPwrHandle(0x55530000, 0, 0);
            NandSysProtSet(0);
        }
        else
        {    
            uint16 spare[16];
            uint32 FreeBlk, RawLBA, RawPBA;
            
            LBA = FtlGetUnremap(row);
            spare[0]=SIGN_DATA_BLK;            //表示数据块
            spare[1]=0;
            spare[2]=LBA;

            LBA = LBA<<pFtl->LogBlkSize;
            RawLBA = LBA - SysProgRawDiskCapacity;  //用第一份固件的数据, 来恢复第二份固件
            RawPBA = FtlGetRemap(RawLBA);

            FreeBlk = (uint32)QueueFront(&pFtl->FreeBlkInfo)<<pFtl->LogBlkSize;
            NandBlkErase(FreeBlk, NULL);
            NandCopyProg(RawPBA, FreeBlk, pFtl->SecPerBlk, spare);
            NandSysProtSet(FLASH_PROT_MAGIC);
            FtlLostPwrHandle(0x55531111, FreeBlk, row);
            NandBlkErase(row, NULL);
            NandCopyProg(RawPBA, row, pFtl->SecPerBlk, spare);
            NandBlkErase(FreeBlk, NULL); //将目标块擦除掉
            FtlLostPwrHandle(0x55530000, 0, 0);
            NandSysProtSet(0);
        }
    }
    else
    {
        #ifdef FTL_POWER_ON_FAST
        FtlSaveRemapTbl(0x55530000);
        #endif
        // TODO: 由PBA匹配到对应的LBA, 作映射处理
        LBA = FtlGetUnremap(row);    //row 和 LBA都是BLOCK 对齐的
        if (LBA < pFtl->TotalLogicBlk)      //数据在原块
        {            
            pSwBlk = FtlSwBlkOpen(LBA, pFtl->SecPerBlk, FTL_OPEN_WR);
            NandCopyProg(row, (uint32)pSwBlk->PBA<<pFtl->LogBlkSize, pFtl->SecPerBlk, NULL); //记录的时候ROW 是BLOCK对齐的
            pSwBlk->nSec += pFtl->SecPerBlk;
            FtlSwBlkClose(pSwBlk);
        }
        else
        {
            uint8 j;
            //3如果在交换块的源块里面，关闭交换块
            row = row>>pFtl->LogBlkSize;
            for (j=0, pSwBlk=pFtl->SwBlkInfo; j<MAX_SWBLK_NUM; j++, pSwBlk++)
            {
                if (pSwBlk->valid != 0)
                {
                    if (pSwBlk->PBA == row)  //查找坏块是否在交换块里
                    {
                        FtlSwBlkClose(pSwBlk);
                        break;
                    }
                }
            }
            //数据在cache 块
        }
    }
}

/*
Name:       FlashRefreshHook
Desc:       MLC数据刷新
Param:      
Return:
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
void FtlRefreshHook(void)
{
    uint32 row, num;
    uint32 ImaskTemp;
    pFTL_INFO pFtl=&gFtlInfo;

    OBTAIN_FTL_SEM;
    ImaskTemp = UserIsrDisable();
    for (num=0; num<MAX_REFLESH_LOG; num++)
    {
        if (NandRefleshSec[num]==-1)
            continue;

        row = NandRefleshSec[num];   
        FtlFlashRefresh(row);
        NandRefleshSec[num]=-1;
    }
    
#ifdef  FTL_READ_REFRESH_EN
    //固件区扫描,避免hynix flash只读block内部分数据，其他数据会出错
    if (gFtlReadScanSec >= SysProgDiskCapacity)
       gFtlReadScanSec = SysProgRawDiskCapacity;

    row = FtlGetRemap(gFtlReadScanSec);
    gFtlReadScanSec += 128;//gFtlInfo.secPerBlk ，固件8MB，播放音乐时，差不多6小时扫描一次固件
    for(num = 0; num<128; num+= pFtl->SecPerPage)//gFtlInfo.secPerBlk
    {
        NandReadEx(row+num, NULL, NULL, pFtl->SecPerPage, 0);
    }
#endif

    UserIsrEnable(ImaskTemp);
    RELEASE_FTL_SEM;
}

/*
Name:	FtlGetPageSize
Desc: 	
Param:	
Return:		
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
uint16 FtlGetPageSize(void)
{
    return gFtlInfo.SecPerPage;
}

/*
Name:       MDGetFtlInfo
Desc:   
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
uint32 MDGetFtlInfo(uint8 LUN, pMEMDEV_INFO pDevInfo)
{
    pDevInfo->BlockSize = gFtlInfo.SecPerBlk;
    pDevInfo->PageSize = gFtlInfo.SecPerPage;

    pDevInfo->capacity = FtlGetCapacity(LUN);

    return FTL_OK;
}

/*
Name:       MDFtlWrite
Desc:   
Param:  
Return: 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_WRITE_CODE_
uint32 MDFtlWrite(uint8 LUN, uint32 Index, uint32 nSec, void *pbuf)
{
    if (LUN < 2)
    {
        return (uint32)FtlWriteImage(LUN, Index, nSec, pbuf);
    }
    else
    {
        return (uint32)FtlWrite(LUN, Index, nSec, pbuf);
    }
}

#else

/*
Name:       FtlGetStatus
Desc:       获取 FTL 状态
Param:      
Return:     1:valid,0:not valid 
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
int32 FtlGetStatus(void)
{
    return gFtlInfo.valid;
}

/*
Name:       FtlGetCurEraseBlk
Desc:       获取 FTL 正常擦除的 block号
Param:      
Return:  
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
int32 FtlGetCurEraseBlk(void)
{
    return CurEraseBlk;
}

/*
Name:       FtlGetBlkNum
Desc:       获取FTL 总block数
Param:      
Return:  
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
int32 FtlGetBlkNum(void)
{
    return gFtlInfo.MaxPhyBlk;
}

/*
Name:       FtlSetBadBlkByLBA
Desc:       增加LBA对应的块到坏块表
Param:      BlkNum=坏块号 
Return:  
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
void FtlSetBadBlkByLBA(uint32 LBA)
{
    uint32 PBA;

    PBA=FtlGetRemap(LBA);
    FtlSetBadBlkTbl(PBA>>gFtlInfo.LogBlkSize);
}

/*
Name:       MDFtlRead
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
uint32 MDFtlRead(uint32 SecAdrr, uint32 nSec, void *pBuf)
{
    uint32 ret;

    ret = (uint32)FtlRead(0, SecAdrr, nSec, pBuf);
    if(ret  !=  FTL_OK)
    {
        FtlSetBadBlkByLBA(SecAdrr);
        FWLowFormatEn = 1;
    }
    return ret;
}

/*
Name:       MDFtlWrite
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
uint32 MDFtlWrite(uint32 SecAdrr, uint32 nSec, void *pBuf)
{
    return (uint32)FtlWriteImage(0, SecAdrr, nSec, pBuf);
}

#endif

#if 0

/*
Name:   FtlTest
Desc:       
Param:      
Return:  
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FTL_CODE_
void FtlTest(void)
{

    pFTL_INFO pFtl=&gFtlInfo;
    uint32* pTmpBuf = (uint32*)pFtlTmpBuf;
    
    FtlSetDataDskSize(SYS_DATA_DISK_SIZE);
    
    FtlRead(2, 53328, 16,  pTmpBuf);
    while(1);
  
}
#endif

#endif
