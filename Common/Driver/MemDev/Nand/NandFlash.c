/* Copyright (C) 2012 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: NandFlash.c
Desc: 

Author: chenfen
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/

#include "MDConfig.h"


#if (NAND_DRIVER == 1)

#ifndef IN_LOADER
#define NAND_DMA
#else
#define DEBUG(...)
#endif

#ifdef NAND_DMA
#include "hw_dma.h"
#include "dma.h"
#endif
/*------------------------------------ Defines -------------------------------*/


#define DRAM_END_ADDR 0x03018000
#define IRAM_START_ADDR 0x01000000

#define     MIN(x,y) ((x) < (y) ? (x) : (y))
#define     MAX(x,y) ((x) > (y) ? (x) : (y))

/*----------------------------------- Typedefs -------------------------------*/

typedef struct tagNAND_PEND_CMD
{
    uint8   Valid;      //指示是否有效
    uint16  Cmd;        //FLASH命令码
    uint32  ReadSec;    //物理扇区地址
    uint32  ErrSec;     //出错物理扇区地址
    void*  pData;       //缓冲区
    void*  pSysData;    //缓冲区
    uint16  Len;        //扇区数

} NAND_PEND_CMD, *pNAND_PEND_CMD;

typedef  struct tagBCH_CODE_TAB
{
/*    
    uint16  version;
    uint16  HeadSize;
    uint16  TotalSize;
    uint16  TypeNum;
    uint8   type[4];
*/
    uint8   offset[4];
    uint8   num[4];
    uint32  BCHCode[67];  //(32+48+76+112)/4
}BCH_CODE_TAB, *pBCH_CODE_TAB;

typedef  struct tagBCH_CODE_INFO
{
    uint8   mode;                       //mode: 16Bit ECC:mode=0, 24Bit:1, 40Bit:2, 60Bit:3
    uint8   num;                        //num = AlignedSpareSize/4,  16Bit:32/4, 24Bit:48/4, 40Bit:76/4, 60Bit:112/4
    uint32  *BCHCode;                   //point to BCHCodeTab
}BCH_CODE_INFO, *pBCH_CODE_INFO;


/*-------------------------- Forward Declarations ----------------------------*/

/* ------------------------------- Globals ---------------------------------- */

/*-------------------------------- Local Statics: ----------------------------*/

_ATTR_FLASH_BSS_ FLASH_SPEC NandSpec;
_ATTR_FLASH_BSS_ NAND_PEND_CMD NandPendCmd;

_ATTR_FLASH_BSS_ uint16 NandIDBlkNum;

_ATTR_FLASH_BSS_ uint16 NandCurIDBlk;

_ATTR_FLASH_BSS_ BCH_CODE_INFO gBCH;

_ATTR_FLASH_BSS_ uint32 NandRefleshSec[MAX_REFLESH_LOG];

_ATTR_FLASH_INIT_DATA_ uint8 NandLSBPage[128];

_ATTR_FLASH_BSS_  uint32 	FlashSysProtAddr;
_ATTR_FLASH_BSS_  uint32    FlashSysProtMagic;
_ATTR_FLASH_BSS_  uint32 	FlashSysCodeRawAddr;

/*******************************
厂商ID表
********************************/
_ATTR_FLASH_INIT_CODE_  
uint8  const  ManufactureIDTbl[] = 
{
    0xec,					//三星SAMSUNG
    0x98,					//东芝TOSHIBA
    0xad,					//海力士HYNIX
    0xc1,					//英飞凌INFINEON
    0x2c,					//美光MICRON
    0x07,					//瑞萨RENESAS
    0x20,					//意法半导体ST
    0x89,					//英特尔intel
    0x45                    // Sandisk
};

/*******************************
器件ID表
********************************/
_ATTR_FLASH_INIT_CODE_ 
uint8 const  DeviceCode[] =
{
    0x75,                   //small 8bit-32MB
    0x76,                   //small 8bit-64MB
    0x78,                   //small 8bit-128M
    0x79,                   //small 8bit-128MB
    0x71,                   //small 8bit-256MB
    0xF1,                   //large 8bit-128M
    0xD1,                   //large 8bit-128M
    0xDA,                   //large 8bit-256M
    0xDC,                   //large 8bit-512M
    0xD3,                   //large 8bit-1G
    0xD5,                   //large 8bit-2G
    0xD7,                   //large 8bit-4G
    0xD9,                   //large 8bit-8G
    0x48,                   //large 8bit-2G
    0x68,                   //large 8bit-4G
    0x88,                   //large 8bit-8G
    0xA8,                   //large 8bit-16G
    0xDE,                   //PBA 8bit-4GB/CE
    0x3A,                   //PBA 8bit-8GB/CE
    0x3C,                   //PBA 8bit-16GB/CE
    0x64,                   //large 8bit-8G MICRON l84a
    0x84                    //large 8bit-16G MICRON l84a 2 die of ce
};

//器件信息表(Flash容量——以sector为单位)
//phisical sectors
_ATTR_FLASH_INIT_CODE_  
uint32 const DeviceInfo[] =        
{
    0x10000,                    // 0X75 DI_32M,  small page
    0x20000,                    // 0X76 DI_64M,  small page
    0x40000,                    // 0x78 DI_128M, small page
    0x40000,                    // 0x79 DI_128M, small page
    0x80000,                    // 0x71 DI_256M, small page
    0x40000,                    // 0xF1 DI_128M, large page
    0x40000,                    // 0xD1 DI_128M, large page
    0x80000,                    // 0xDA DI_256M, large page
    0x100000,                   // 0xDC DI_512M, large page
    0x200000,                   // 0xD3 DI_1G,   large page
    0x400000,                   // 0xD5 DI_2G,   large page
    0x800000,                   // 0xD7 DI_4G,   large page
    0x1000000,                  // 0xD9 DI_8G,   large page
    0x400000,                   // 0x48 DI_2G,   large page
    0x800000,                   // 0x68 DI_4G,   large page
    0x1000000,                  // 0x88 DI_8G,   large page
    0x2000000,                  // 0xA8 DI_16G,  large page
    0x1000000,                  // 0xDE DI_8G,   large page
    0x2000000,                  // 0x3A DI_16G,  large page
    0x4000000,                  // 0x3C DI_8G,   large page
    0x1000000,                  // 0x64 DI_8G,   large page
    0x2000000                   // 0x84 DI_16G,   large page
};

_ATTR_FLASH_CODE_
BCH_CODE_TAB const BCHCodeTab =
{
/*

    0x0100,             //version
    0x0014,             //HeadSize
    0x0120,             //TotalSize  0x0014+0x010c(67*4)
    0x0004,             //TypeNum
    0x10,               //type[4]
    0x18,                   
    0x28,
    0x3c,
*/    
    0x00,               //offset[4]            
    0x08,               //(14*16/8+4)/4
    0x14,               //(14*24/8+4+3)/4 + 0x08
    0x27,
    0x08,               //num[4]
    0x0c,               //(14*24/8+4+3)/4 =12
    0x13,               //19
    0x1c,               //28
    
    /*1k 16bit*/
    0xffffffff,
    0x90aa66eb,
    0xc43d12f1,
    0xbb97281e,
    0xb69a3f24,
    0x324fb4af,
    0x86c9df3c,
    0xc648a9ae,

    /*1k 24bit*/
    0xffffffff,
    0xdae514e1,
    0x5c5ac305,
    0xc1da0ea5,
    0x574851d3,
    0x00900ca6,
    0x4bf6333d,
    0x1274e07e,
    0x01dede48,
    0x78169b31,
    0xce6d1b59,  //~0xce6d1b59 = 0x3192e4a6
    0xffff6450,

    /*40bit 1k*/
    0xffffffff,
    0x037c5e1c,
    0x6e3f413d,
    0xc7194ac7,
    0x5562151a,
    0xb30b83a3,
    0xcb70c2c4,
    0xf60501f1,
    0x74e2d91c,
    0xc086482a,
    0x85248ca5,
    0xa2d1580e,
    0x479f56b5,
    0x757731a2,
    0x9ff3b819,
    0xd4be761b,
    0x415cc845,
    0xd2983e9b,
    0xffff0989,
    
    /*60bit 1k*/
    0xffffffff,
    0x59080bb1,
    0xc7832059,
    0x56a665cf,
    0xe96292ca,
    0x7690bd85,
    0x9dfb3918,
    0xb2981fa2,
    0x56578b6a,
    0x7c8c9034,
    0x3ff90057,
    0x130643f4,
    0x9e254835,
    0xaa7a576f,
    0xc648a00c,
    0x1cf6168e,
    0x102d2594,
    0xb55a8ae6,
    0xaa536e40,
    0x4e1ce87a,
    0x8acfb9af,
    0x9256ac3c,
    0x99457a31,
    0x1188a7f0,
    0x9ec8140a,
    0x574e10e3,
    0x3d7995e4,
    0xffffff68
} ;

_ATTR_FLASH_CODE_
uint16 const NandActiveRegion[] = 
{
    EMCTL_ECCCTL_ACTIVE_FLASH_0_2,
    EMCTL_ECCCTL_ACTIVE_FLASH_1_3
};

_ATTR_FLASH_CODE_
uint8 const NandCSBit[] = 
{
    EMCTL_FMCTL_SEL_FLASH0, 
    EMCTL_FMCTL_SEL_FLASH1, 
    EMCTL_FMCTL_SEL_FLASH2, 
    EMCTL_FMCTL_SEL_FLASH2
};

/*--------------------------- Local Function Prototypes ----------------------*/
//static uint32 ToshibaReadRetry(uint32 sec, void *pData, void *pSpare, uint8 nSec);
void NandTest(void);

/*------------------------ Function Implement --------------------------------*/
#ifdef IN_LOADER
/*
Name:       NandIOMux
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandIOMux(void)
{
    //flash_csn0, flash_csn1, flash_ale, flash_rdy, flash_rdn, flash_wrn, flash_cle 
    GRFReg->GPIO0A_IOMUX = 3<<0 | 3<<2 | 3<<4 | 3<<6 | 3<<8 | 3<<10 | 3<<12 | 3<<16 | 3<<18 | 3<<20| 3<<22| 3<<24| 3<<26 | 3<<28;
    GRFReg->GPIO0C_IOMUX = 2<<14 | 3UL<<30; // flash_csn2
    GRFReg->GPIO0D_IOMUX = 2<<0 | 3<<16;  // flash_csn3  
    GRFReg->IOMUX_CON0 = 0xFFFF0000; //IO_FLASH_DATA[0]~IO_FLASH_DATA[7]
}

/*
Name:       NandIODeMux
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandIODeMux(void)
{
    return;
}
/*
Name:       NandSetClk
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
void NandSetClk(void)
{
    return;
}
#endif

/*
Name:       NandDelay
Desc:       延时
Param:      in:cycle数   
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandDelay(uint32 count)
{
	count *= 4;
    while (count--)
    {
		;
    }
}


#if 0  //def NAND_DMA
_ATTR_FLASH_DATA_ volatile uint8 NandDmaFlag;
/*
Name:       NandDMACB
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandDMACB(void)
{
    NandDmaFlag = 0;
}
#endif


_ATTR_FLASH_CODE_
static void NandWriteDataX8(uint32 *data,uint8 * pbuf,uint32 len)
{
    #ifdef NAND_DMA
    if (len==1024)
    {
        uint32 timeout = 0;
        eDMA_CHN channel;
        DMA_CFGX DmaCfg = {DMA_CTLL_NNAD_WR, DMA_CFGL_NAND_WR, DMA_CFGH_NAND_WR,0};
        
        channel = DmaGetChannel();
        if ((channel != DMA_FALSE)/* && (channel < DMA_CHN2)*/)
        {
            DmaStart((uint32)(channel),(uint32)pbuf,(uint32)data, len,&DmaCfg,NULL);
            while (DMA_SUCCESS != DmaGetState(channel))
    	    {
    	        if (++timeout > len * 1000)
    	        {
    	            printf("Nand Flash DMA write Error!\n");
                    break;
    	        }
    	    }
        }
        else
            goto NO_DMA_WR;
    }
    else
    #endif
    {
NO_DMA_WR:
        while(len--)
        {
            *data = *pbuf++;
        }
    }
}

_ATTR_FLASH_CODE_
static void NandWriteDummy(uint32 *data,uint8 * pbuf,uint32 len)
{
    while(len--)
    {
        *data = *pbuf;
    }
}

_ATTR_FLASH_CODE_
static void NandReadDataX8(uint32 *fifo, uint8 * pbuf, uint32 len)
{
    #ifdef NAND_DMA
    if (len == 1024 || len == 512)
	{
        uint32 timeout = 0;
	    eDMA_CHN channel;
        DMA_CFGX DmaCfg = {DMA_CTLL_NNAD_RD, DMA_CFGL_NAND_RD, DMA_CFGH_NAND_RD,0};
        
	    channel = DmaGetChannel();
	    if ((channel != DMA_FALSE)/* && (channel < DMA_CHN2)*/)
	    {
	        DmaStart((uint32)(channel), (uint32)fifo,(uint32)pbuf,len,&DmaCfg, NULL);
	        while (DMA_SUCCESS != DmaGetState(channel))
    	    {
    	        if (++timeout > len * 1000)
    	        {
    	            printf("Nand Flash DMA write Error!\n");
                    break;
    	        }
    	    }
	    }
        else
            goto NO_DMA_RD;
	}
    else
    #endif
    {
NO_DMA_RD:    
        while(len--)
        {
            *pbuf++ = *fifo;
        }
    }


}

_ATTR_FLASH_CODE_
static void NandReadDummy(uint32 *data,uint8 * pbuf,uint16 len)
{
    while(len--)
        *pbuf = *data;
}

/*
Name:       NandSysProtSet
Desc:       系统区写保护关
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandSysProtSet(uint32 magic)
{
    FlashSysProtMagic=magic;
}

/*
Name:       NandProtStatusGet
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandProtStatusGet(void)
{
    return (FlashSysProtMagic);
}

/*
Name:       NandSysProtChk
Desc:       入口参数:将要擦写的扇区地址
            出口参数:0=非系统保护区, 1=系统保护区
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandSysProtChk(uint32 SecAddr)
{
    uint32 protect=0;

    #ifdef SYS_PROTECT
    if (NandProtStatusGet() != FLASH_PROT_MAGIC)
    {
        if (NandIDBlkNum<3 || SecAddr<=FlashSysProtAddr)    //系统保留区
            protect=1;
    }
    #endif
    return (protect);
}

/*
Name:       NandCs
Desc:       片选FLASH
Param:      in:芯片号  
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandCs(uint8 CS)
{
    NandCtl->FMCtl |= NandCSBit[CS];
}

/*
Name:       NandCs
Desc:       片选FLASH
Param:      in:芯片号  
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandDeCs(uint8 CS)
{
    NandCtl->FMCtl &= (~NandCSBit[CS]);
}

/*
Name:       NandBchDecEn
Desc:       BCH解码使能
Param:      in:芯片号  
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static void NandSetRdmz(uint8 EnRdmz, uint32 sec)
{
    pFLASH_SPEC spec = &NandSpec;

    if ((1== EnRdmz) && (1 == spec->EnRdmz))
    {
        uint32 seed = (sec/spec->SecPerPageRaw)% spec->PagePerBlock;
        NandCtl->RandMC = (EMCTL_RANDMC_RESET |EMCTL_RANDMC_ENABLE|EMCTL_RANDMC_SEED|seed);       
    }
}

/*
Name:       NandBchDecEn
Desc:       BCH解码使能
Param:      in:芯片号  
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static void NandBchDecEn(uint8 CS)
{
    NandCtl->EccCtl=((uint32)gBCH.mode<<EMCTL_ECCCTL_BCHMODE)|NandActiveRegion[CS&0x01]|EMCTL_ECCCTL_DATA_INPUT|EMCTL_ECCCTL_BCHRST;
}

/*
Name:       NandBchEncEn
Desc:       BCH编码使能
Param:      in:芯片号  
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void NandBchEncEn(uint8 CS)
{
    NandCtl->EccCtl=((uint32)gBCH.mode<<EMCTL_ECCCTL_BCHMODE)|NandActiveRegion[CS&0x01] |EMCTL_ECCCTL_BCHRST;
}


/***************************************************************************
函数描述:
入口参数:
出口参数:
调用函数:
***************************************************************************/
/*
Name:       NandBchDisable
Desc:       关闭BCH
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static void NandBchDisable(void)
{
    NandCtl->RandMC = EMCTL_RANDMC_RESET;
    NandCtl->EccCtl = EMCTL_ECCCTL_BCH_POWDN;
}

/*
Name:       NandBchSel
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
static void NandBchSel(uint8 bits)
{
    uint8 size[] = {32, 46, 74, 109};//14*16/8+4=32, 14*24/8+4=46, 14*40/8+4=74, 14*60/8+4=109
    uint8 AlignedSize[] = {32, 48, 76, 112};

    uint32 i, offset;

    i = (bits-1)>>4; //(16-1)>>4 = 0, (24-1)>>4 = 1, (40-1)>>4 = 2, (60-1)>>4 = 3

    NandSpec.EccBits = bits;
    NandSpec.SpareSize = size[i]; 
    NandSpec.AlignedSpareSize = AlignedSize[i];

    gBCH.mode = i;
    gBCH.num = BCHCodeTab.num[i];
    offset = BCHCodeTab.offset[i];
    gBCH.BCHCode = (uint32 *)&(BCHCodeTab.BCHCode[offset]);

}

/*
Name:       NandReadBchEn0
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void NandReadBchEn0(uint32 *pSpare)
{
    uint32 i;
    uint32 n = gBCH.num; 
    uint32 *BCHCode = gBCH.BCHCode;

    for (i=0; i<n; i++) 
    {
        pSpare[i] = NandCtl->BchEn0[i];
        pSpare[i] ^= (~BCHCode[i]);
    }
}

/*
Name:       NandWriteEn
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandWriteEnable(void)
{
    NandCtl->FMCtl &= (~EMCTL_FMCTL_FLASH_WE);
}

/*
Name:       NandWriteDisable
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandWriteDisable(void)
{
    NandCtl->FMCtl |= EMCTL_FMCTL_FLASH_WE;
}

/*
Name:       NandWriteBchEn0
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static void NandWriteBchEn0(uint32 *pSpare)
{
    uint32 i;
    uint32 n = gBCH.num; 
    uint32 *BCHCode = gBCH.BCHCode;
    
    for (i=0; i<n; i++)
        NandCtl->BchEn0[i] = pSpare[i] ^ (~ BCHCode[i]);
}

/*
Name:       NandRefreshRec
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandRefreshRec(uint32 sec)
{
    #ifndef IN_LOADER
    uint32 align;
    uint32 i;
    pFLASH_SPEC spec=&NandSpec;

    align=(sec >= spec->SecPerBlock*NandIDBlkNum) ? spec->SecPerBlock : spec->SecPerBlockRaw;
    sec -= sec % align;
    for (i=0; i<MAX_REFLESH_LOG; i++)   //需要刷新地址初始化
    {
        if (NandRefleshSec[i]==sec)
            break;
        if (NandRefleshSec[i]==(uint32)-1)
        {
            NandRefleshSec[i]=sec;
            break;
        }
    }
    #endif
}

/*
Name:       NandBchCorrect
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static int32 NandBchCorrect(uint8 *pData, uint8 *pSpare, uint32 sec, uint8 mod)
{    
    uint32 i;
    uint16 ErrLocateByte, ErrLocateBit;
    uint16  BchStatus;
    
    for (i = 0; i <50000; i++) 
    {
        BchStatus = NandCtl->BchStatus;
        if ((BchStatus & 0x02) != 0x00)              
            break;                       //Bch Finish
        NandDelay(2);
    }
    
    //Bch not Finish & decode fail
    if (((BchStatus & 0x02) == 0x00) || ((BchStatus & 0x04) != 0x00)) 
    {
        /*if(sec > FlashSysCodeRawAddr && sec <= FlashSysProtAddr)
        {
            NandRefreshRec(sec); 
        }*/
        return (ERROR);
    }
    
    if ((BchStatus & 0x01) != 0x00)   //errf0, error found
    {        
        BchStatus = (BchStatus >> 3) & 0x3f;                       //err num
        if (BchStatus > NandSpec.EccBits)                          //invaild err num            
            return (ERROR);
        
        for (i = 0; i < BchStatus; i++) 
        {
            ErrLocateBit  = (uint16)(NandCtl->BchDe0[i]);               
            ErrLocateByte = (ErrLocateBit >> 4) & 0x7ff;    //error byte

            //error bit in higher half byte, else in lower half byte
            if((ErrLocateBit & 0x8000) != 0)
                ErrLocateBit = (ErrLocateBit<<4) & 0xf0; 
            else
                ErrLocateBit &= 0x0f;
            
            if (ErrLocateByte<1024)
            {
                if (NULL != pData)
                {
                    if ((mod == 2)
                        ||((mod == 0) && (ErrLocateByte < 512))
                        ||((mod == 1) && (ErrLocateByte >= 512)))
                        pData[ErrLocateByte-((mod==1)? 512:0)] ^= ErrLocateBit;
                }
            }
            else
                pSpare[ErrLocateByte-1024] ^= ErrLocateBit;
        }

        return BchStatus;
    }
    
    return(OK);
}

/*
Name:       NandGetCS
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandGetCS(uint32 sec, uint32 *SecInCS)
{
    uint32 ChipSel;
    pFLASH_SPEC spec=&NandSpec;

    for (ChipSel=0; ChipSel<MAX_FLASH_NUM; ChipSel++)
    {
        if (sec < spec->TotPhySec[ChipSel])
            break;
        sec -= spec->TotPhySec[ChipSel];
    }
    //Assert(CS<MAX_FLASH_NUM, "Addr overflow!!!", CS);

    if (ChipSel < MAX_FLASH_NUM && SecInCS!=NULL)
    {
        *SecInCS=sec;
    }
    return (ChipSel);
}

/*
Name:       NandReadStatus
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static uint8 NandReadStatus(uint8 CS)
{
    uint8 ret=0;
    pNAND_IF NandIF;
    if (CS < MAX_FLASH_NUM)
    {
        NandIF = NandSpec.NandIF[CS];
    
        NandCs(CS);
        NandIF->cmd = NandSpec.ReadStatusCmd;
        NandDelay(4);    //at least 80ns, 100ns @ 500MHz

        ret = NandIF->data;

        NandDeCs(CS);
        NandDelay(4);    //tRHW at least 100ns, 100ns @ 500MHz
    }
    return (ret);
}


/*
Name:       NandWaitBusy
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandWaitBusy(uint32 *SecReplace)
{
    uint32 status = OK;
    uint32 i, CS, tmp;
    pFLASH_SPEC spec = &NandSpec;

    for (i=0; i<2000000; i++)
    {
        NandDelay(5);
        if ((NandCtl->FMCtl & EMCTL_FMCTL_FLASH_RYBY) != 0)		//busy pin
            break;
    }

    if (NandPendCmd.Valid != 0 && NandPendCmd.Cmd != READ_CMD)
    {
        NandPendCmd.Valid=0;
        CS = NandGetCS(NandPendCmd.ErrSec, NULL);
        tmp = NandReadStatus(CS) & 0x01;
        if (tmp != 0 /*&& NandPendCmd.Cmd != BLOCK_ERASE_CMD*/)
        {
            #ifndef IN_LOADER
            tmp = FtlProgErrHook(NandPendCmd.ErrSec, NandPendCmd.pData, NandPendCmd.pSysData, NandPendCmd.Len);
            NandWaitBusy(NULL);
            #endif
            status=(uint32)ERROR;
        }
        
        if (status !=OK && SecReplace != NULL)
        {
            uint32 sec = *SecReplace;
            *SecReplace= tmp-(tmp%spec->SecPerBlock)+(sec%spec->SecPerBlock);
        }  
    }

    return (status);
}

/*
Name:       NandSyncIO
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandSyncIO(void)
{
    uint32 ret;
    NandIOMux();
    ret = NandWaitBusy(NULL);
    NandIODeMux();

    return ret;
}

/*
Name:       NandWaitBusy
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
void NandSetRdmzflag(uint8 value)
{
    NandSpec.EnRdmz = value;
}

/*
Name:       NandCpyIn2Spare
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void NandCpyIn2Spare(uint16 *spare, uint16 *SysData, uint32 nSec)
{
    uint32 i, j;
    uint32 SpareSize = NandSpec.AlignedSpareSize>>1; //>>1 因为uint16 SpareSize : 32、48、72、112

    for (i=0, j=0; i<nSec; i++)
    {
        spare[j+(i&0x01)] = SysData[i];
        if (i&0x1)
            j+=SpareSize;   
    }
}

/*
Name:       NandCpySpare2Out
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static void NandCpySpare2Out(uint16 *SysData, uint16 *spare, uint32 nSec, uint8 align)
{
    uint32 i, j;
    uint32 SpareSize = NandSpec.AlignedSpareSize>>1; //>>1 因为uint16 SpareSize : 32、48、72、112

    if (align != 0)
    {
        *SysData = *(spare+1);
        spare += SpareSize;
        nSec--;
    }

    for (i=0, j=0; i<nSec; i++)
    {     
        SysData[i] = spare[j+(i&0x01)];
        if (i&0x1)
            j+=SpareSize;          
    }

}

/*
Name:       NandReadID
Desc:       读FLASH ID,识别FLASH容量等参数
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
void NandReadID(uint8 CS, void *buf)
{
    uint8 *ptr = (uint8*)buf;
    pNAND_IF NandIF;

    NandIOMux();

    NandIF=NandSpec.NandIF[CS];
    NandCs(CS);
    NandIF->cmd=RESET_CMD;
    NandWaitBusy(NULL);

    NandIF->cmd=READ_ID_CMD;
    NandIF->addr=0x00;
    NandDelay(10);    //at least 80ns, 100ns @ 500MHz
	
    ptr[0]=NandIF->data;
    ptr[1]=NandIF->data;
    ptr[2]=NandIF->data;
    ptr[3]=NandIF->data;
    ptr[4]=NandIF->data;
    ptr[5]=NandIF->data;

	NandDeCs(CS);

    NandIODeMux();
}

/*
Name:       NandSkipAddrGap
Desc:       flash跳过片内gap
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
static uint32 NandSkipAddrGap(uint32 row)
{
    pFLASH_SPEC pSpec=&NandSpec;

    if((pSpec->Vonder==TOSHIBA && pSpec->dualPlaneAddrMap == 0) //toshiba 新出的flash，(6D2E,PBA)有gap,由于PBA 8GB内部有2片，所以不能判断
       ||(pSpec->Vonder==SAMSUNG && pSpec->SecPerPageRaw >= 16 && pSpec->ReadRetry == NULL))//page 大于 8KB的MLC有gap, 21nm 没有gap
    {
        if(pSpec->TotPhySec[0] > pSpec->Die2PageAddr * pSpec->SecPerPageRaw)//内置多个die，需要跳过gap
        {
            uint32 dieOffset = (row)/pSpec->Die2PageAddr;
            row +=  dieOffset*pSpec->Die2PageAddr;  //跳过gap
        }
    }
    return row;
}

/*
Name:       Nand2PlaneAddr
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 Nand2PlaneAddr(uint32 row, uint8 EnMulPlane)
{
    #ifdef DUAL_PLANE
    pFLASH_SPEC spec = &NandSpec;
    if (spec->MulPlane > 1 && 1==EnMulPlane)
    {
        uint32 PageAddr, BlkAddr, PlaneAddr, ChipAddr;

        PageAddr=((row>>1) >> spec->Interleave) & (spec->PagePerBlock-1);
        if (spec->dualPlaneAddrMap!=0)
        {
            BlkAddr=((row>>(1+spec->Interleave)) & (spec->Die2PageAddr/2-1)) & (~(spec->PagePerBlock-1));
            PlaneAddr=(row & 0x01)*(spec->Die2PageAddr/2);
        }
        else
        {
            BlkAddr=((row>>spec->Interleave) & (spec->Die2PageAddr-1)) & (~(spec->PagePerBlock*2-1));
            PlaneAddr=(row & 0x01)*spec->PagePerBlock;
        }
        ChipAddr=row&spec->Die2PageAddr;
        row=(ChipAddr | PlaneAddr | BlkAddr | PageAddr);
    }
    #endif
    
    return row;
}

/*
Name:       NandReadSpareCmd
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandReadSpareCmd(uint8 CS, uint32 sec, uint16 SecPerPageRaw)
{
    uint16 col;
    uint32 row;
    //uint16 SecPerPageRaw;
    pFLASH_SPEC spec = &NandSpec;
    pNAND_IF NandIF;
    uint8 EnMulPlane;

    NandIF = spec->NandIF[CS];

    //SecPerPageRaw = (sec<NandIDBlkNum*spec->SecPerBlockRaw)? 4 : spec->SecPerPageRaw;
    col = (SecPerPageRaw << 9) + (((sec & (SecPerPageRaw-1))>>1) * spec->SpareSize);//sec 须为偶数

    row = sec/spec->SecPerPageRaw;
    EnMulPlane = (0==CS && sec<NandIDBlkNum*spec->SecPerBlockRaw)? 0:1;
    row = Nand2PlaneAddr(row, EnMulPlane);
    row =  NandSkipAddrGap(row);

    if (spec->Large != 0)
    {
    	NandIF->cmd  = READ_CMD >> 8;
    	NandIF->addr = col & 0x00ff ;
    	NandIF->addr = col >> 8;
    	NandIF->addr =  row & 0x00ff;
    	NandIF->addr =  row >> 8;
        if (spec->Five != 0)
        	NandIF->addr =  row >> 16;
    	NandIF->cmd = READ_CMD & 0x00ff;
    }
}

/*
Name:       NandProgCmd
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void NandProgCmd(uint8 CS, uint32 SecInCS, uint16 SecPerPageRaw)
{
    uint32 row;
    pFLASH_SPEC spec = &NandSpec;
    pNAND_IF NandIF;

    //SecPerPageRaw = ((0==CS)&&(SecInCS<NandIDBlkNum*spec->SecPerBlockRaw))? 4:spec->SecPerBlockRaw; 
    NandIF = spec->NandIF[CS];
    row = SecInCS/spec->SecPerPageRaw;
    row = NandSkipAddrGap(row);
    if (spec->Large != 0)
    {
    	NandIF->cmd  = PAGE_PROG_CMD >> 8;
    	NandIF->addr = 0;
    	NandIF->addr = 0;
    	NandIF->addr =  row & 0x00ff;
    	NandIF->addr =  row >> 8;
        if (spec->Five != 0)
        	NandIF->addr =  row >> 16;
    }
}

/*
Name:       NandProgCmdPlane2
Desc:       送FLASH编程命令DUAL PLANE
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void NandProgCmdPlane2(uint8 CS, uint32 sec, uint8 plane)
{
    uint32 row;
    uint32 ChipAddr, PageAddr, PlaneAddr, BlkAddr;
    pFLASH_SPEC spec = &NandSpec;
    pNAND_IF NandIF;

    NandIF = spec->NandIF[CS];
    row = sec / spec->SecPerPageRaw;

    PageAddr=((row>>1) >> spec->Interleave) & (spec->PagePerBlock-1);
	if (spec->dualPlaneAddrMap!=0)
	{
        BlkAddr=((row>>(1+spec->Interleave)) & (spec->Die2PageAddr/2-1)) & (~(spec->PagePerBlock-1));
    	PlaneAddr=plane*(spec->Die2PageAddr/2);
	}
	else
	{
        BlkAddr=((row>>spec->Interleave) & (spec->Die2PageAddr-1)) & (~(spec->PagePerBlock*2-1));
    	PlaneAddr=plane*spec->PagePerBlock;
	}
    ChipAddr=row&spec->Die2PageAddr;
	row=ChipAddr | PlaneAddr | BlkAddr | PageAddr;
    if (plane != 0)
    {
        NandIF->cmd=(spec->Vonder==MICRON||spec->Vonder==TOSHIBA)? (PAGE_PROG_CMD>>8):(PLANE2_PAGE_PROG_CMD>>8);
    }
    else
    {
        NandIF->cmd=PAGE_PROG_CMD >> 8;
        row=(spec->fstPlaneAddr != 0) ? row : (ChipAddr);
    }
    
    row = NandSkipAddrGap(row);
    NandIF->addr=0x00;
    NandIF->addr=0x00;
    NandIF->addr=row & 0x00ff;
    NandIF->addr=row >> 8;
    NandIF->addr=row >> 16;
}


/*
Name:       NandBlkEraseCmd
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void NandBlkEraseCmd(uint8 CS, uint32 sec)
{
    pFLASH_SPEC spec = &NandSpec;
    pNAND_IF NandIF;
    uint32 row;
    uint8 EnMulPlane;
    
    NandIF = spec->NandIF[CS];
	row = sec/spec->SecPerPageRaw;
    #ifdef DUAL_PLANE
    EnMulPlane = (0==CS && sec<NandIDBlkNum*spec->SecPerBlockRaw)? 0:1;
    if (spec->MulPlane > 1 && 1==EnMulPlane)
    {
		uint32 BlkAddr, PlaneAddr, ChipAddr;
    	if (spec->dualPlaneAddrMap!=0)
    	{
            BlkAddr=((row>>(1+spec->Interleave)) & (spec->Die2PageAddr/2-1)) & (~(spec->PagePerBlock-1));
        	PlaneAddr=spec->Die2PageAddr/2;
        }
    	else
    	{
            BlkAddr=((row>>spec->Interleave) & (spec->Die2PageAddr-1)) & (~(spec->PagePerBlock*2-1));
        	PlaneAddr=spec->PagePerBlock;
    	}
        ChipAddr= row&spec->Die2PageAddr;
        NandIF->cmd=BLOCK_ERASE_CMD >> 8;

        row=(spec->fstPlaneAddr != 0) ? (ChipAddr | BlkAddr) : (ChipAddr);
        row = NandSkipAddrGap(row);
        NandIF->addr=row & 0x00ff;
        NandIF->addr=row >> 8;
        NandIF->addr=row >> 16;
   
        row= (ChipAddr |PlaneAddr|BlkAddr);
    }
    else
    #endif
    {
        row &= ~(spec->PagePerBlock-1);
    }
    
    row = NandSkipAddrGap(row);    
	NandIF->cmd =  BLOCK_ERASE_CMD >> 8;
	NandIF->addr = row & 0x00ff;
	NandIF->addr = row >> 8;
    if (spec->Five != 0)
    	NandIF->addr = row >> 16; //add by lxs for more than 128MB

    NandIF->cmd = BLOCK_ERASE_CMD & 0x00ff;

}

/*
Name:       NandRead
Desc:       
Param:        
Return:     < 0 : ERROR,  =0 : OK,  >0 : bit error count  
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
int32 NandRead(uint32 sec, uint8 *pData, void *pSysData, uint32 nSec, uint32 NextSec)
{
	int32 ret, status = OK;
    uint32 j, SecInCS;
    uint8  spare[112*16];//16KB page
    uint8 *pSpare;
    pFLASH_SPEC spec = &NandSpec;
    pNAND_IF NandIF;
    uint32 SecPerPageRaw;
    uint8  CS, align, tmp, EnRdmz, mod;

    NandIOMux();
    align = sec & 0x01;        //必须是 1K 对齐的
    sec -= align; 
    NandWaitBusy(&sec);       //wait busy

    (sec<NandIDBlkNum*spec->SecPerBlockRaw)? (SecPerPageRaw=4, EnRdmz=0):(SecPerPageRaw=spec->SecPerPageRaw, EnRdmz=1);

    CS = NandGetCS(sec, &SecInCS);    
    NandIF = spec->NandIF[CS];
    NandCs(CS);
    if (NextSec != -1 && NandPendCmd.Cmd==READ_CMD 
        &&(NandPendCmd.ReadSec& ~(SecPerPageRaw-1))==(sec& ~(SecPerPageRaw-1)))//预读的是在一个PAGE 内
    {
        uint16 col = (SecPerPageRaw << 9)+(((sec & (SecPerPageRaw-1))>>1) * spec->SpareSize);
        NandIF->cmd  = RAND_DATAOUT_CMD >> 8;
        NandIF->addr = col & 0x00ff ;
        NandIF->addr = col >> 8;
        NandIF->cmd  = RAND_DATAOUT_CMD & 0x00ff;
    }
    else
    {
        spec->ReadStatusCmd = READ_STATUS_CMD;
        NandPendCmd.Cmd = READ_CMD;
        NandReadSpareCmd(CS, SecInCS, SecPerPageRaw);
    }
    NandWaitBusy(NULL);
    NandPendCmd.ReadSec = (uint32)((NextSec!=-1)?(sec+align+nSec):-1);    //要预读到地址
       
    for (j = 0; j < (nSec+align+1)>>1; j++) 
    {
        NandReadDataX8(&NandIF->data, spare+j*spec->AlignedSpareSize, spec->SpareSize);
    }    

    NandIF->cmd  = RAND_DATAOUT_CMD >> 8;
    NandIF->addr = 0x00;
    NandIF->addr = (SecInCS & (SecPerPageRaw-1)) << 1;
    NandIF->cmd  = RAND_DATAOUT_CMD & 0x00ff;
    NandWaitBusy(NULL);

    pSpare = spare;
    NandBchDecEn(CS);
    for (j = 0; j < nSec; j++) 
    {
        //NandBchDecEn(CS);
        NandSetRdmz(EnRdmz, SecInCS);
        NandWriteBchEn0((uint32*)pSpare);
        if(pData == NULL)
        {
            NandReadDummy(&NandIF->data, &tmp, 1024);
            j++;
            ret = NandBchCorrect(NULL, pSpare, sec, 2);
            status = (ret >= 0)?  MAX(ret, status) : ERROR;
        }
        else
        {
            if(j==0 && align!=0)
            {
                NandReadDummy(&NandIF->data, &tmp, 512);
                NandReadDataX8(&NandIF->data, pData, 512);
                mod = 1;  //BCH纠data 512-1023
            }
            else if (j+2 > nSec)
            {
                NandReadDataX8(&NandIF->data, pData, 512);
                NandReadDummy(&NandIF->data, &tmp, 512);
                mod = 0; //BCH纠data 0-511
            }
            else
            {
                NandReadDataX8(&NandIF->data, pData, 1024);
                j++;
                mod = 2; //BCH纠data 0-1023
            }
            
            ret = NandBchCorrect((uint8*)pData,  pSpare, sec, mod);
            status = (ret >= 0)?  MAX(ret, ((uint32)status)) : ERROR;

            pData += mod*512;
        }
        pSpare += spec->AlignedSpareSize;
        //NandBchDisable();
    }
    NandBchDisable();
    
    if (pSysData)
        NandCpySpare2Out((uint16*)pSysData, (uint16*)spare, nSec, align);

    NandDeCs(CS);
    
    if (NandPendCmd.ReadSec<spec->TotalPhySec)                  //预读处理
    {
        if (NextSec!=0 && NextSec<spec->TotalPhySec)
            NandPendCmd.ReadSec=NextSec;
        
        if ((NandPendCmd.ReadSec & (SecPerPageRaw-1))== 0)      //页对齐
        {
            CS = NandGetCS(NandPendCmd.ReadSec, &SecInCS);      //注意预读跨片选
            NandReadSpareCmd(CS, SecInCS, SecPerPageRaw);
        }
    }

    NandIODeMux();
	return (status);
}

/*
Name:       NandReadEx
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandReadEx(uint32 sec, void *pData, void *pSysData, uint32 nSec, uint32 NextSec)
{
    uint32 status= OK;
    int32 ret;
    uint32 len;
    uint8 *pDataTmp=(uint8*)pData;
    uint8 *pSpareTmp=(uint8*)pSysData;
    pFLASH_SPEC spec = &NandSpec;

    while (nSec != 0)
    {
        len = spec->SecPerPageRaw-(sec & (spec->SecPerPageRaw-1));
        if (len > nSec)
            len=nSec;
        
        ret = NandRead(sec, pDataTmp, pSpareTmp, len, NextSec);
        if (ret < 0)
        {
            if (spec->EnRdmz)
            {
                spec->EnRdmz = 0;
                ret = NandRead(sec, pDataTmp, pSpareTmp, len, -1);
                spec->EnRdmz = 1;
                if (ret < 0 && spec->ReadRetry == NULL)
                {
                    ret = NandRead(sec, pDataTmp, pSpareTmp, len, -1);
                }
            }
            if ((ret < 0) && (spec->ReadRetry != NULL))
            {
                ret = spec->ReadRetry(sec, pDataTmp, pSpareTmp, len);
            }
        }

        if (ret < 0)
        {
            status = ERROR;
            DEBUG("NRead ERR 0x%x", sec);
        }
        else if(ret >= spec->EccBits-4)
        {
            if (spec->ReadRetry == NULL)
                NandRefreshRec(sec);
            //DEBUG("NRead ERR 0x%x, %d", sec, ret);
        }

        nSec-=len;
        sec+=len;
        if (pData)
            pDataTmp+=len*512;
        if (pSysData)
            pSpareTmp+=len*2;
    }

    return (status);
}

/*
Name:       NandProgWrData
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
static void NandProgWrData(uint8 CS, uint32 sec, uint8* pData, uint8 *pSpare)
{
    uint32 i;
    pNAND_IF NandIF;
    uint8 tmp=0xFF; 
    pFLASH_SPEC spec = &NandSpec;
    uint8 EnRdmz;
    uint16 SecPerPageRaw;
    
    (sec<NandIDBlkNum*spec->SecPerBlockRaw)? (SecPerPageRaw=4, EnRdmz=0)
                                            :(SecPerPageRaw=spec->SecPerPageRaw, EnRdmz=1);

    NandIF = spec->NandIF[CS];
    NandBchEncEn(CS);
    for (i=0; i<(SecPerPageRaw>>1); i++)
    {
        NandSetRdmz(EnRdmz, sec);
        NandCtl->BchEn0[0] = *((uint32*)(pSpare+i*spec->AlignedSpareSize));

        if (((uint32)pData < IRAM_START_ADDR) || ((uint32)pData >= DRAM_END_ADDR)) 
        {
            NandWriteDummy(&NandIF->data, &tmp, 1024);
        } 
        else 
        {
            if((uint32)pData+1024 < DRAM_END_ADDR)//数据空间溢出，这些数据无用，使用其他空间垃圾数据
            {
                NandWriteDataX8(&NandIF->data, pData, 1024);
                pData +=1024;                
            }
            else
            {
                NandWriteDataX8(&NandIF->data, pData, DRAM_END_ADDR-(uint32)pData);
                NandWriteDummy(&NandIF->data, &tmp, (uint32)pData+1024-DRAM_END_ADDR);
                pData = NULL;
            } 
        }
        NandReadBchEn0((uint32*)(pSpare+i*spec->AlignedSpareSize));             //read BCH
    }
    NandBchDisable();
     
    for(i=0; i<(SecPerPageRaw>>1); i++)
    {
        NandWriteDataX8(&NandIF->data, pSpare+i*spec->AlignedSpareSize, spec->SpareSize);
    }
}

/*
Name:       NandProg
Desc:       FLASH编程
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_WRITE_CODE_
uint32 NandProg(uint32 sec, void *pData, void *pSysData)
{
    uint16 SecPerPageRaw;
	uint32 ret=OK;
    uint32 CS, SecInCS;
    uint8 spare[112*16];       //112 > 14*60/8+4 = 109 1K
    pFLASH_SPEC spec = &NandSpec;
    pNAND_IF NandIF;
    uint8 EnMulPlane;

    //return (OK);
    //UNUSED(nPage);
    if (NandSysProtChk(sec))
        return (FTL_OK);

    NandIOMux();
    NandWaitBusy(&sec); 		//wait busy
    (sec<NandIDBlkNum*spec->SecPerBlockRaw)? (SecPerPageRaw=4, EnMulPlane=0)
                                            :(SecPerPageRaw=spec->SecPerPageRaw,EnMulPlane=1); 

    memset(spare, 0xFF, 4); 
    if (pSysData)
        NandCpyIn2Spare((uint16*)spare, (uint16*)pSysData, SecPerPageRaw);
  
    CS = NandGetCS(sec, &SecInCS);
    if (spec->PreProgram)
        spec->PreProgram(CS);
    
    NandIF = spec->NandIF[CS];
    NandCs(CS);
    #ifdef DUAL_PLANE
    if (spec->MulPlane > 1 && 1 == EnMulPlane)
    {
        //plane 1
        NandProgCmdPlane2(CS, SecInCS, 0);
        spec->ReadStatusCmd=(spec->Vonder==TOSHIBA) ? READ_STATUS_CMD_TOSHIBA : READ_STATUS_CMD;

        NandProgWrData(CS, sec, pData, spare);

        NandIF->cmd=PLANE2_PAGE_PROG_CMD & 0x00ff;

        //plane 2
        if(pSysData)          
            NandCpyIn2Spare((uint16*)spare, (uint16*)pSysData +SecPerPageRaw, SecPerPageRaw);

        NandWaitBusy(NULL);

        NandProgCmdPlane2(CS, SecInCS, 1);
        NandProgWrData(CS, sec+SecPerPageRaw, (uint8*)pData+(SecPerPageRaw<<9), spare);

        NandPendCmd.Cmd=PLANE2_PAGE_PROG_CMD;
    }
    else
    #endif
    {
        NandProgCmd(CS, SecInCS, SecPerPageRaw);
        spec->ReadStatusCmd = READ_STATUS_CMD;
        NandProgWrData(CS, sec, pData, spare);
        NandPendCmd.Cmd=PAGE_PROG_CMD;

        #if 0   //支持 2K SLC
        if (spec->SecPerPageRaw == 0x04)
        {
            NandIF->cmd=PAGE_PROG_CMD & 0x00ff;

            if(pSysData)
                NandCpyIn2Spare((uint16*)spare, (uint16*)pSysData +SecPerPageRaw, SecPerPageRaw);
                        
            NandWaitBusy(NULL);        //第一个Page 不考虑编程出错
            NandProgCmd(CS, SecInCS+SecPerPageRaw, SecPerPageRaw);
            NandProgWrData(CS, sec, pData+(SecPerPageRaw<<9), spare);
        }
        #endif
    }

    NandIF->cmd=PAGE_PROG_CMD & 0x00ff;
    NandDeCs(CS);
    
    NandPendCmd.Len = SecPerPageRaw;
    NandPendCmd.ErrSec = sec;
    NandPendCmd.pData = pData;
    NandPendCmd.pSysData = pSysData;
    NandPendCmd.Valid=1;

    NandIODeMux();

	return (ret);
}

/*
Name:       NandBlkErase
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandBlkErase(uint32 sec, uint32 *SecReplace)
{
	uint32 ret;
    uint32 CS, SecInCS;
    pFLASH_SPEC spec=&NandSpec;

    //return (OK);
    if (NandSysProtChk(sec))
        return (OK);

    NandIOMux();
    NandWaitBusy(NULL);

    CS = NandGetCS(sec, &SecInCS);
    if (CS >= MAX_FLASH_NUM)
        return ERROR;

	NandCs(CS);
	NandBlkEraseCmd(CS, SecInCS);

	NandDeCs(CS);

    spec->ReadStatusCmd = READ_STATUS_CMD;
    NandPendCmd.Cmd = BLOCK_ERASE_CMD;
    NandPendCmd.Len=0;
    NandPendCmd.ErrSec=sec;
    NandPendCmd.pData=NULL;
    NandPendCmd.Valid=1;
    ret = NandWaitBusy(SecReplace);
    NandIODeMux();

    return (ret); //FTL_OK

}


/*
Name:        NandInitLSBMap
Desc:       
Param:        
Return:     
Global: 
Note: 
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
static void NandInitLSBTab(void)
{
    uint32 i;
    pFLASH_SPEC spec = &NandSpec;

    for (i=0; i<128; i++)
    {
        NandLSBPage[i] = i;       
    }

    if (spec->Large && spec->MLC)
    {
        spec->EnLSB = 1;
    }
    
    if (spec->EnLSB)
    {
        if (1 == spec->LSBMode) //只有hynix flash get rrt 时用到lsb page
        {
            for (i=0; i<128; i++)
            {
                NandLSBPage[i] = (i>3)? (i*2 - ((i&0x01)? 3:2)): i; // 0 1 2 3 6 7 10 11
            }
        }
        #if 1 
        else if (2 == spec->LSBMode)
        {
            for (i=0; i<128; i++)
            {
                NandLSBPage[i] = (i>1)? (i*2 - 1) : i;  // 0 1 3 5 7 9 11 13
            }
        }
        else if (3 == spec->LSBMode)
        {
            for (i=0; i<128; i++)
            {
                NandLSBPage[i] = (i>5)? (i*2 - ((i&0x01)? 5:4)): i; // 0 1 2 3 4 5 8 9 12 13
            }
        }
        #endif
    }
    
}

/*
Name:        NandLoadRRT
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 NandLoadRRT(uint32 sec)
{
    uint32 ret = ERROR;
    uint32 FlashBuf[512/4];
    pFLASH_INFO pInfo;
    uint8 *pRRT = NULL;
    
    if (OK == NandReadEx(sec, FlashBuf, NULL, 1,  -1))
    {
        pInfo=(pFLASH_INFO)FlashBuf;

        if (pInfo->magic==FLASH_MAGIC 
            && pInfo->ver == FLASH_INFO_VER
            && pInfo->lenth)
        {
            pRRT = pInfo->ReadRetryTab;
        }
    }

    return NandGetRRT(pRRT);
}

/* 
Name: NandSetReadRetry        
Desc:        
Param:        
Return:     
Global: 
Note:   
Author:
Log:
*/
_ATTR_FLASH_INIT_CODE_
void NandSetReadRetry(NREAD_RETRY pRetry)
{
    NandSpec.ReadRetry = pRetry;
}

/*
Name:       NandInit
Desc:       FLASH初始化,读FLASH ID,识别FLASH容量等参数
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_INIT_CODE_
uint32 NandInit(void)
{
	uint32 i, DevID, tmp;
	uint8  IDByte[6], lgDiePerCe /*,ChipMask*/;
	uint32 ret = FTL_ERROR;
    pFLASH_SPEC spec=&NandSpec;
    uint32 NandIFBase[4] = {NANDIF_BASE0, NANDIF_BASE1, NANDIF_BASE2, NANDIF_BASE3};

    memset(NandRefleshSec, 0xff, MAX_REFLESH_LOG*4);    //需要刷新地址初始化
    memset(spec, 0, sizeof(NandSpec));
    spec->ReadStatusCmd = READ_STATUS_CMD;
    NandPendCmd.Valid =0;
    
    NandSetClk();
    NandIOMux();
    //NandCtl->RandMC = 0;
    NandCtl->FMWait = 0x1081;			        //csrw=1, rwpw=1, rwcs=1
	NandWriteEnable();

    
    for (i=0; i<MAX_FLASH_NUM; i++)
    {
        spec->TotPhySec[i] = 0;
        spec->NandIF[i] = (NAND_IF *)NandIFBase[i];
    }

    NandReadID(0, IDByte);
    //DEBUG("NAND ID %x %x %x %x", IDByte[0], IDByte[1], IDByte[2], IDByte[3]);
    
    #ifdef IN_LOADER
    if ((0x00==IDByte[0] && 0x00==IDByte[1])
       ||(0xFF==IDByte[0] && 0xFF==IDByte[1]))           //没接NAND
        return ERROR;
    #endif

    //厂商ID
    for (i=0; i<(sizeof(ManufactureIDTbl)); i++)
    {
        if (IDByte[0] == ManufactureIDTbl[i])
        {
            spec->Vonder=i;
            if (INTEL == spec->Vonder)  //因为intel和micron是同一个Die
                spec->Vonder = MICRON;
            else if (SANDISK == spec->Vonder)
                spec->Vonder = TOSHIBA;
                
            break;
        }                
    }

    if (i >= sizeof(ManufactureIDTbl))
    {
        //printf("Nand Flash Manufacture ID error!\n");
        NandIODeMux();
        return ERROR;
    }
    
    //器件ID	
	//ChipMask = 0;
    for (i=0; i<sizeof(DeviceCode); i++)
    {
        if (IDByte[1] == DeviceCode[i]) 
        {
            DevID = i;
            spec->TotalPhySec = spec->TotPhySec[0] = DeviceInfo[DevID];
            //ChipMask = 1;
            break;
        }
    }

    if (i >= sizeof(DeviceCode))
    {
        //printf("Nand Flash Device ID error!\n");
        NandIODeMux();
        return ERROR;
    }

    lgDiePerCe=0;
    spec->MLC=0;
	spec->Large=0;
	spec->Five=1;
	spec->MulPlane=1;
    spec->Interleave=0;
	spec->AccessTime=50;
			
    //小PAGE和大PAGE识别
    if (DevID >= Large128M)
    {
        spec->Large=1;
        //5个地址cyc和4个地址cyc识别
        if (DevID == Large128M)
            spec->Five=0;
    }

    if (0 != spec->Large)
    {
        spec->MLC=(IDByte[2]>>2) & 0x03;   //bit/cell
		spec->SecPerPageRaw = 2 << (IDByte[3] & 0x03);	// 1KB~8KB    
		spec->SecPerBlockRaw = 128 << ((IDByte[3]>>4) & 0x03);	//64KB~512KB
        if(spec->SecPerBlockRaw == 128)//zyf，update block size
        {
            spec->SecPerBlockRaw = spec->SecPerPageRaw* (spec->MLC + 1) * 64;
        }
        lgDiePerCe= IDByte[2] & 0x03;

        #ifdef DUAL_PLANE
        if (spec->Vonder==TOSHIBA || spec->Vonder==MICRON)
        {
            spec->MulPlane = 2;
            if((IDByte[0]==0x98)&&(IDByte[1]==0xD5)&&((IDByte[5]&0x7)==0x6))
            {
                spec->MulPlane = 1; //TOSHIBA 24NM 2GB FLASH NOT SUPPORT DUAL PLANE
            }
        }
        else
        {
            //Mul plane prog num
            spec->MulPlane= 1 << ((IDByte[2]>>4) & 0x03);
            if (spec->MulPlane>2)
                spec->MulPlane=2;  //最多限制2plane

            if (spec->Vonder==SAMSUNG && IDByte[1]==0xDA)
                spec->MulPlane=1;

            if(IDByte[0] == 0x89 &&((IDByte[1]==0x48)||(IDByte[2]==0x68)||(IDByte[3]==0x88)||(IDByte[4]==0xA8)))
                spec->MulPlane = 1;
        }

        if(spec->SecPerPageRaw == 32)   //目前只能支持16K page
            spec->MulPlane = 1;
        #endif   
    }
    else
    {
        spec->SecPerPageRaw=1;     //512byte/sec
        spec->SecPerBlockRaw=32;   //16KB/block
    }

    tmp=spec->MulPlane;
    #ifdef INTERLEAVE
    spec->Interleave=(IDByte[2]>>6) & 0x01;
    tmp <<= spec->Interleave;
    #endif
    spec->SecPerBlock=spec->SecPerBlockRaw*tmp;
    spec->SecPerPage=spec->SecPerPageRaw*tmp;
    if ((0x04==spec->SecPerPage) && (1==tmp))
        spec->SecPerPage = spec->SecPerPage<<1;
    
    spec->PagePerBlock=spec->SecPerBlock/spec->SecPerPage;
    spec->Die2PageAddr=(spec->TotPhySec[0] / spec->SecPerPageRaw)>>lgDiePerCe;

    for (i=1; i<MAX_FLASH_NUM; i++)
	{
        uint8 IDByte2[6];
        NandReadID(i, IDByte2);
        if (IDByte2[1] == IDByte[1])
        {
            spec->TotPhySec[i] = spec->TotPhySec[0];
            spec->TotalPhySec += spec->TotPhySec[0];
            //ChipMask |= 1 << i;
        }
    }

    {
        uint32 j;
        uint8 EccBit[] = {24, 40, 16, 60};
		uint8 DataBuf[1024];
        pIDSEC0 IdSec0;
        pIDSEC1 IdSec1;

        NandIDBlkNum = 100;        
		for (i=0; i<50; i++)	//在50个BLOCK里查找ID PAGE
		{
            for(j=0; j<sizeof(EccBit); j++)
            {
                NandBchSel(EccBit[j]);
                if (OK == NandReadEx(i*spec->SecPerBlockRaw, DataBuf, NULL, 2, 0))
                    break;
            }
            
            IdSec0 = (pIDSEC0)DataBuf;
            if (IdSec0->magic == 0xFCDC8C3B)   //0x0ff0aa55 + rc4
			{
                uint32 sec;
                IdSec1=(pIDSEC1)(DataBuf+512);      //IdSec1 NOT RC4
                if (IdSec1->ChipTag==CHIP_SIGN)
                {
                    if(IdSec1->ECCBits != spec->EccBits)
                        continue;// ecc bits 不对

                    NandCurIDBlk = i;
                    spec->AccessTime=IdSec1->AccessTime;
                    spec->SecPerPageRaw=IdSec1->PageSize;
                    spec->SecPerBlockRaw=IdSec1->BlockSize;
                    if(spec->SecPerPageRaw == 32)
                    {
                        tmp = 1;
                        spec->MulPlane = 1;
                    }
                    
                    spec->SecPerBlock=spec->SecPerBlockRaw*tmp;
                    spec->SecPerPage=spec->SecPerPageRaw*tmp;
                    if ((0x04==spec->SecPerPage) && (1==tmp))
                        spec->SecPerPage = spec->SecPerPage<<1;
                    
                    spec->PagePerBlock=spec->SecPerBlock/spec->SecPerPage;
                    spec->Die2PageAddr=(spec->TotPhySec[0] / spec->SecPerPageRaw)>>lgDiePerCe;

                    tmp=IdSec1->SysAreaBlockRaw;

                    NandVendorInit(IDByte);
                    NandInitLSBTab();
                    if (spec->NeedRRT)
                    {
                        sec = (NandLSBPage[IdSec1->FlashInfoOffset>>2]*spec->SecPerPageRaw) + (IdSec1->FlashInfoOffset&0x3);
                        NandLoadRRT(i*spec->SecPerBlockRaw + sec);
                    }
                    
                    LoaderVer    = IdSec1->LoaderVer;
                    UpgradeTimes = IdSec1->UpgradeTimes;
                    
                    SysProgDiskCapacity = (uint32)(IdSec1->SysProgDiskCapacity)<<11;
                    SysProgRawDiskCapacity = SysProgDiskCapacity>>1;

                    sec = SysProgDiskCapacity & (spec->SecPerBlock-1);
                    SysProgDiskCapacity += ((sec > 0)? (spec->SecPerBlock-sec) : 0); //SysProgDiskCapacity 必须是spec->SecPerBlock整倍数
                    SysDataDiskCapacity = SysUserDisk2Capacity=0;
                    if ((SysProgDiskCapacity+SysDataDiskCapacity) < spec->TotalPhySec)
                        ret=FTL_OK;

                    break;
                }
			}
		}
    }


    spec->fstPlaneAddr=0;   
    spec->dualPlaneAddrMap=0;
    if (spec->Vonder==MICRON || spec->Vonder==TOSHIBA || spec->SecPerPageRaw>=0x10)
    {
        spec->fstPlaneAddr=1;      //第一个PLANE地址不能为0
    }

    if ((spec->Vonder==TOSHIBA) && (spec->SecPerPageRaw<=0x08) && (spec->MLC == 1)/*(IDByte[0][1]!=0xD1) && (IDByte[0][1]!=0xDA) && (IDByte[0][1]!=0xDC)*/)
    {
        spec->dualPlaneAddrMap=1;  //TOSHIBA 4KB/PAGE及以下2PLANE映射方式是DIE的一半容量
    }

    #ifdef DUAL_PLANE
    if (spec->MulPlane>1 && spec->dualPlaneAddrMap==0)
    {
        tmp=(tmp + 1) / 2;
        #ifdef INTERLEAVE
        if (spec->Interleave > 0)
        {
            tmp=(tmp + 1) / 2;
        }
        #endif
    }
    #endif
    NandIDBlkNum = tmp;
    FlashSysProtAddr=spec->SecPerBlock*NandIDBlkNum;

    //NandTest();
    NandIODeMux();
	return (ret);
}

/*
Name:       NandDeInit
Desc:      
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandDeInit(void)
{
    NandSyncIO();
    if (1==NandSpec.NeedRRT) 
    {
        HynixSetRRDefault();
    }
}

/***************************************************************************
函数描述:FLASH拷贝编程
入口参数:源PAGE的行地址,列地址,目标PAGE的行地址,列地址
出口参数:FTL_ERROR--失败,FTL_OK--成功
调用函数:
***************************************************************************/
_ATTR_FLASH_DATA_
uint32 *NandCopyProgBuf;    //在FTL代码中被初始化
           
#define     SIGN_SW_BLK                     0xf200

_ATTR_FLASH_WRITE_CODE_
uint32 NandCopyProg(uint32 srcSec, uint32 destSec, uint16 nSec, void *pSpare)
{
    uint32 secPerBlk;
    uint32 status;
    uint8 dataBlk;
    uint8 step;
    pFLASH_SPEC spec=&NandSpec;
    uint32 *buf = NandCopyProgBuf;

    status=(uint32)FTL_ERROR;
    secPerBlk=spec->SecPerBlock;
    if (nSec <= secPerBlk && nSec>0) //参数合法判断
    {
        status=FTL_OK;
        //PRINTF("copy: srcSec=%lx, destSec=%lx, len=%x\n", srcSec, destSec, nSec);
        if (destSec<secPerBlk*NandIDBlkNum)
        {
            step=spec->SecPerPageRaw;
            if (nSec>spec->SecPerBlockRaw)
                nSec=spec->SecPerBlockRaw;
        }
        else
            step=spec->SecPerPage;

        nSec-=nSec % step;
        NandReadEx(srcSec-(srcSec%secPerBlk), NULL, &buf[DATA_LEN], step, 0);
		if ((buf[DATA_LEN]&0xffff)==SIGN_SW_BLK)
        {
            uint16* tmp = (uint16*)&buf[DATA_LEN];
            dataBlk=1;
            tmp[1]++;
            tmp[3]=srcSec/secPerBlk;
            tmp[4]=destSec/secPerBlk;
        }
        else
            dataBlk=0;

        while (nSec != 0)
        {
            NandReadEx(srcSec, buf, (dataBlk!=0) ? NULL : &buf[DATA_LEN], step, 0); //数据块每个Space区都一样，其他块不一样
            NandProg(destSec, buf, (pSpare!=NULL && dataBlk!=0) ? pSpare : &buf[DATA_LEN]); 
            NandWaitBusy(&destSec);
            nSec -= step;
            srcSec += step;
            destSec += step;
        }
    }
    return (status);
}

#ifndef IN_LOADER
/*
Name:       NandGetProductSn
Desc:      
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
int32 NandGetProductSn(void *pSn)
{
    pFLASH_SPEC spec=&NandSpec;
    uint8 DataBuf[512];
    PRKNANO_IDB_SEC3 pIDBSec3;
        
    if (OK != NandReadEx(NandCurIDBlk*spec->SecPerBlockRaw + 3, DataBuf, NULL, 1, 0))

    {
        return -1;
    }

    pIDBSec3 = (PRKNANO_IDB_SEC3)DataBuf;
    if (pIDBSec3->usSNSize)
    {
        uint16 size = pIDBSec3->usSNSize;
        if (size > sizeof(pIDBSec3->sn)) 
        {
            size = sizeof(pIDBSec3->sn);
        }
        memcpy(pSn, pIDBSec3->sn, size);
    }

    return pIDBSec3->usSNSize;
}

#else
/*
Name:        NandReadSpare
Desc:        读FLASH SPARE 数据
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandReadSpare(uint32 sec, uint8 *pSpare, uint32 len)
{
    uint32 CS, row;
    uint16 col;
    pNAND_IF NandIF;
    pFLASH_SPEC spec = &NandSpec;

    CS = NandGetCS(sec, &row);
    if (CS >= MAX_FLASH_NUM)
        return (ERROR);

    col = (spec->SecPerPageRaw) <<9;
    row /= spec->SecPerPageRaw;

    NandCs(CS);
    NandIF = spec->NandIF[CS];

    row = Nand2PlaneAddr(row, 1);
    row = NandSkipAddrGap(row);

    if (spec->Large != 0)
    {
        NandIF->cmd=(READ_CMD >> 8);
        NandIF->addr=col & 0x00ff;
        NandIF->addr=col >> 8;
        NandIF->addr=row & 0x00ff;
        NandIF->addr=row >> 8;
        if (spec->Five != 0)
            NandIF->addr=row >> 16;
        NandIF->cmd=READ_CMD & 0x00ff;
    }

    NandWaitBusy(NULL); 	
    while(len--)
    {
        *pSpare++ = NandIF->data;
    }
    NandDeCs(CS);
    return (OK);
}

/*
Name:       NandChkBadBlk
Desc:       FLASH坏块检测
Param:      入口参数:sec 地址，需要对齐到 block,对于dualplane ,需要对偶数block
            出口参数:ERROR--坏块,OK--好块  
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 NandChkBadBlk(uint32 sec)
{
	uint8 bad1, bad2;
    uint32 SecPerBlock = NandSpec.SecPerBlock;
	uint32 SecPerPage = NandSpec.SecPerPage;
    uint32 SecPerPageRaw = NandSpec.SecPerPageRaw;
    
#ifdef DUAL_PLANE
    if (NandSpec.MulPlane > 1)
    {
        NandReadSpare(sec, &bad1, 1);  //第一个 page
        NandReadSpare(sec + SecPerPageRaw,  &bad2, 1);
        bad1 &= bad2;

        if (NandSpec.Vonder == HYNIX || NandSpec.Vonder == SAMSUNG ||  NandSpec.Vonder == TOSHIBA)
        {   //最后一个page
            NandReadSpare(sec + SecPerBlock - SecPerPage, &bad2, 1);
            bad1 &= bad2;
            NandReadSpare(sec + SecPerBlock - SecPerPageRaw,  &bad2, 1);
            bad1 &= bad2;
        }
    }
    else
#endif
    {
        NandReadSpare(sec, &bad1, 1); //第一个 page
        if (NandSpec.Vonder == HYNIX || NandSpec.Vonder == SAMSUNG ||  NandSpec.Vonder == TOSHIBA)
        {   //最后一个page
            NandReadSpare(sec+SecPerBlock-SecPerPage, &bad2, 1); // 508
            bad1 &= bad2;
        }
        #if 0
        if(NandSpec.Vonder == HYNIX)
        { //倒数第三个page
            NandReadSpare(sec + SecPerBlock - SecPerPageRaw*3,  &bad2, 1);
            bad1 &= bad2;
        }
        #endif
    }
	
	if (bad1 != 0xff)
		return (ERROR);
	else
		return (OK);
}

/*
Name:       NandSetBadBlk
Desc:       标记坏块
Param:      被标记块内的一个PAGE  
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void NandSetBadBlk(uint32 pageNum)
{
	uint32 BadFlag[16];

	BadFlag[0]=0;
    
	pageNum -= pageNum % NandSpec.SecPerBlockRaw;    //取该块的第一个PAGE
    NandBlkErase(pageNum, NULL);
	NandProg(pageNum, NULL, &BadFlag[0]);
}

/*
Name:       MDNandBlkErase
Desc:       mod 0 为普通擦除； 1为强制擦除
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
uint32 MDNandBlkErase(uint32 ChipSel, uint32 BlkStart, uint32 BlkNum, uint32 mod)
{
    uint32 StartAddr;
	uint32 EndAddr;
    pFLASH_SPEC spec = &NandSpec;
    uint32 ret = OK;

    UNUSED(mod);
    
    if(BlkStart > NandIDBlkNum *spec->MulPlane || ChipSel>0)
    {
        return OK;
    }
    
    if(BlkStart + BlkNum > NandIDBlkNum *spec->MulPlane)
    {
        BlkNum = NandIDBlkNum*spec->MulPlane - BlkStart;
    } 

	StartAddr = BlkStart*spec->SecPerBlockRaw;
	EndAddr= StartAddr+BlkNum*spec->SecPerBlockRaw;

    NandSysProtSet(FLASH_PROT_MAGIC);
	for (; StartAddr < EndAddr; StartAddr += spec->SecPerBlockRaw)
	{
		if (NandBlkErase(StartAddr, NULL) != OK)
		{
		    ret = ERROR;
            NandSetBadBlk(StartAddr);
            if ((StartAddr & spec->SecPerBlockRaw)==spec->SecPerBlockRaw)
                NandSetBadBlk(StartAddr-spec->SecPerBlockRaw);
            else
            {
                StartAddr+=spec->SecPerBlockRaw;
                NandSetBadBlk(StartAddr);
            }
		}
	}
    NandSysProtSet(0);
    
    return ret;
}

/*
Name:       MDGetNandInfo
Desc:       
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
uint32 MDGetNandInfo(pMEMDEV_INFO pDevInfo)
{
    pFLASH_SPEC spec=&NandSpec;

    pDevInfo->Manufacturer = spec->Vonder;
    pDevInfo->ChipMask = 1;
    #ifdef IN_LOADER
    pDevInfo->BlockSize = spec->SecPerBlockRaw;
    pDevInfo->PageSize = spec->SecPerPageRaw;
    #else
    pDevInfo->BlockSize = spec->SecPerBlock;
    pDevInfo->PageSize = spec->SecPerPage;
    #endif
    pDevInfo->ECCBits = spec->EccBits;
    pDevInfo->capacity = spec->TotPhySec[0];

    pDevInfo->AccessTime = spec->AccessTime;
	pDevInfo->IDBlkNum = 50;
    
    return OK;
}

/*
Name:        NandReadIDB
Desc:        sec必须是2K对齐, nSec必须是4
Param:        
Return:     
Global: 
Note:   
Author: 
Log:
*/
uint32 NandReadIDB(uint32 sec, uint32 nSec, uint8 *pData)
{
    uint32 NewSec;
    pFLASH_SPEC spec = &NandSpec;
    uint32 BlkAddr, BlkOffet, page;

    BlkOffet = sec & (spec->SecPerBlockRaw-1);
    BlkAddr = sec & ~(spec->SecPerBlockRaw-1);

    page = BlkOffet>>2;             //IDB 每个page 只用2K
    page = (page<128)? NandLSBPage[page] : page;
    NewSec = BlkAddr | page*spec->SecPerPageRaw;
    
    return NandRead(NewSec, pData, NULL, nSec, -1);
}

/*
Name:       NandWriteIDB
Desc:       sec必须是2K对齐, nSec必须是4
Param:        
Return:     
Global: 
Note:   一次只写2K
Author: 
Log:
*/
uint32 NandWriteIDB(uint32 sec, uint32 nSec, uint8 *pData)
{
    uint32 ret;
    uint32 SysData[2];
    uint32 NewSec, NextSec;
    pFLASH_SPEC spec = &NandSpec;
    uint32 BlkAddr, BlkOffet, page;

    BlkOffet = sec & (spec->SecPerBlockRaw-1);
    BlkAddr = sec & ~(spec->SecPerBlockRaw-1);

    NextSec = (BlkOffet!=4)? (BlkOffet+4):20; //boot1 实际大小不能超过2K, maskrom读完boot1的前2K, 就要从boot2的起始读
    page = NextSec>>2;             //IDB 每个page 只用2K
    page = (page<128)? NandLSBPage[page] : page;
    SysData[1] = SysData[0] = page<<2;     

    page = BlkOffet>>2;             //IDB 每个page 只用2K
    page = (page<128)? NandLSBPage[page] : page;
    NewSec = BlkAddr | page*spec->SecPerPageRaw;

    NandSysProtSet(FLASH_PROT_MAGIC);
    #if 0   //use for debug
    if (OK != NandProg(NewSec, pData, SysData))
    {
        DEBUG("prog err sec=%x\n", sec);
        while(1);
    }

    if (OK == NandReadIDB(sec, nSec, (uint8*)NandCopyProgBuf))
    {
        uint32 i;
        uint32 *pTmpData = (uint32*)pData;
        
        for (i=0; i<nSec*128; i++)
        {
            if (pTmpData[i] != NandCopyProgBuf[i])
            {
                DEBUG("cmp err i=%x\n", i);
                while(1);
            }
        }
    }
    else
    {
        DEBUG("read err sec=%x\n", sec);
        while(1);
    }
    return OK;
    #else
    ret = NandProg(NewSec, pData, SysData);
    #endif
    NandSysProtSet(0);
    return ret;
}

#endif

#if 0

#define     TRET_READ_ERR       (1<<1)
#define     TRET_PROG_ERR       (1<<2)
#define     TRET_ERASE_ERR      (1<<3)
#define     TRET_DATA_ERR       (1<<4)


extern uint32 gFtlBuf[2][PAGE_SIZE];

uint8 TestRet;
uint16 TestERRCnt;

_ATTR_FLASH_CODE_
void NandTest(void)
{
    uint32 TestBlk/*, TestBlk2*/;
    uint32 i, j;
    pFLASH_SPEC spec=&NandSpec;
    uint32 *ReadBuf = &gFtlBuf[0][0]; 
    uint32 *WriteBuf = &gFtlBuf[1][0]; 
    uint16 ReadSpare[32];       // 2*32byte 每个sec 2个byte
    uint16 WriteSpare[32];

    //NandIOMux();

	for (i=0; i<DATA_LEN; i++)
	{
	    WriteBuf[i]=i+1;
	}

	/*for (i=0; i<8; i++)
	{
	    WriteSpare[i]=i;
	}*/

    //NandIDBlkNum = 0;
    //NandBchSel(24);
    TestRet = 0;
    TestERRCnt = 0;
    //NandReadEx(0x00FFA080, (uint8*)ReadBuf, NULL/*ReadSpare*/, spec->SecPerPage, 0);
    //while(1);

    TestBlk = 0x00772000;//500*spec->SecPerBlock;
    if (OK != NandBlkErase(TestBlk, NULL))
    {
        TestRet |= TRET_ERASE_ERR;
        while(1);
    }

    for(i=0; i</*=0x50*/spec->SecPerBlock; i+=spec->SecPerPage)
    {
        /*if (i==0x40)
        {
            Delay100cyc(1);
        }*/

         //if (OK != NandReadEx(TestBlk+i, (uint8*)ReadBuf, NULL/*ReadSpare*/, spec->SecPerPage, 0))
         //{
         //    TestRet |= TRET_ERASE_ERR;
         //}

        if (OK != NandProg(TestBlk+i, (uint8*)WriteBuf, NULL/*WriteSpare*/))
        {
            TestRet |= TRET_PROG_ERR;
            while(1);
        }
        #if 1
        //j = (i==0x50)? 0x40:i;
        if (OK != NandReadEx(TestBlk+i, (uint8*)ReadBuf, NULL/*ReadSpare*/, spec->SecPerPage, -1))
        {
             TestRet |= TRET_READ_ERR;
             TestERRCnt++;
             //while(1);
        }
         
        /*for (j=0; j<8; j++)
        {
            if (WriteSpare[j] != ReadSpare[j])
            {
                TestRet = j;
                break;
            }
        }*/

        #if 0
        for (j=0; j<spec->SecPerPage*128; j++)
        {
            if (WriteBuf[j] != ReadBuf[j])
            {
                 TestRet |= TRET_DATA_ERR;
                 while(1);
            }
        }

        if (j<spec->SecPerPage*128)
        {
         while(1);
        }
        #endif
        #endif

    }

 #if 0       
    for(i=0; i<spec->SecPerBlock; i+=spec->SecPerPage)
    {
         if (OK != NandReadEx(TestBlk+i, (uint8*)ReadBuf, NULL/*ReadSpare*/, spec->SecPerPage, 0))
         {
             TestRet |= TRET_READ_ERR;
             TestERRCnt++;
         }  
         
         for (j=0; j<spec->SecPerPage*128; j++)
         {
             if (WriteBuf[j] != ReadBuf[j])
             {
                 TestRet = j;
                 while(1);
             }
         }
         
         if (j<spec->SecPerPage*128)
         {
             while(1);
         }
    }
 #endif        

    while(1);

    //NandIODeMux();
}
#endif

#endif
