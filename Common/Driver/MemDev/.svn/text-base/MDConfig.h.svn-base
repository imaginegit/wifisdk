/********************************************************************************
*********************************************************************************
			COPYRIGHT (c)   2004 BY ROCK-CHIP FUZHOU
				--  ALL RIGHTS RESERVED  --

File Name:	    config.h
Author:		    XUESHAN LIN
Created:        1st Dec 2008
Modified:
Revision:		1.00
********************************************************************************
********************************************************************************/
#ifndef _MDCONFIG_H
#define  _MDCONFIG_H

#include    <stdio.h>
#include    <string.h>
#include    "SysConfig.h"


#define UNUSED(x)           ( void )(x)

#ifdef _NANDFLASH_
#define     NAND_DRIVER         1
#else
#define     NAND_DRIVER         0
#endif

#if defined(_EMMC_)
#define     SDMMC_DRIVER        0
#else
#define     SDMMC_DRIVER        0
#endif

#ifdef _SPINOR_
#define     SPINOR_DRIVER       1
#else
#define     SPINOR_DRIVER       0
#endif


//SD卡初始化代码、数据段
#define     _ATTR_SD_INIT_CODE_         __attribute__((section("SDInitCode")))
#define     _ATTR_SD_INIT_DATA_         __attribute__((section("SDInitData")))
#define     _ATTR_SD_INIT_BSS_          __attribute__((section("SDInitBss"),zero_init))

//SD卡基本操作及读取操作代码、数据段
#define     _ATTR_SD_CODE_              __attribute__((section("SDCode")))
#define     _ATTR_SD_DATA_              __attribute__((section("SDData")))
#define     _ATTR_SD_BSS_               __attribute__((section("SDBss"),zero_init))
#define     IRAM_SD                     _ATTR_SD_CODE_

//SD卡写操作代码、数据段
#define     _ATTR_SD_WRITE_CODE_        __attribute__((section("SDWriteCode")))
#define     _ATTR_SD_WRITE_DATA_        __attribute__((section("SDWriteData")))
#define     _ATTR_SD_WRITE_BSS_         __attribute__((section("SDWriteBss"),zero_init))


#define     _ATTR_FLASH_INIT_CODE_      __attribute__((section("FlashCode")))//__attribute__((section("FlashInitCode")))
#define     _ATTR_FLASH_INIT_DATA_      __attribute__((section("FlashData")))//__attribute__((section("FlashInitData")))
#define     _ATTR_FLASH_INIT_BSS_       __attribute__((section("FlashBss"),zero_init))//__attribute__((section("FlashInitBss"),zero_init))

#define     _ATTR_FLASH_CODE_           __attribute__((section("FlashCode")))
#define     _ATTR_FLASH_DATA_           __attribute__((section("FlashData")))
#define     _ATTR_FLASH_BSS_            __attribute__((section("FlashBss"),zero_init))

#define     _ATTR_FLASH_WRITE_CODE_      __attribute__((section("FlashCode")))//__attribute__((section("FlashWriteCode")))
#define     _ATTR_FLASH_WRITE_DATA_      __attribute__((section("FlashData")))//__attribute__((section("FlashWriteData")))
#define     _ATTR_FLASH_WRITE_BSS_       __attribute__((section("FlashBss"),zero_init))//__attribute__((section("FlashWriteBss"),zero_init))

#define     IRAM_FLASH_INIT             _ATTR_FLASH_INIT_CODE_



#include    "typedef.h"
#include    "Macro.h"

#include    "hw_memap.h"
#include    "hook.h"
#include     "USBConfig.h" 

#include     "MemDev.h" 

#include     "nand\Hw_nandc.h"
#include     "nand\NandFlash.h"
#include     "nand\ftl.h"       //FLASH定义头文件

#include     "spinor\Hw_spi.h"
#include     "spinor\SpiNor.h"

#include     "sd_mmc\SDConfig.h" 

typedef volatile struct tagCRU_REG
{
    UINT32 CRU_PLL_CON;
    UINT32 CRU_MODE_CON;
    UINT32 CRU_CLKSEL_CPU_CON;         
    UINT32 CRU_CLKSEL_CODEC_CON;        
    UINT32 CRU_CLKSEL_MMC_CON;         
    UINT32 CRU_CLKSEL_SPI_CON;
    UINT32 CRU_CLKSEL_UART_CON;
    UINT32 CRU_CLKSEL_SARADC_CON;   
    UINT32 CRU_CLKGATE_CON0;
    UINT32 CRU_CLKGATE_CON1;
   	UINT32 CRU_SOFTRST_CON0;
    UINT32 CRU_SOFTRST_CON1;
    UINT32 CRU_STCLK_CON;
    UINT32 CRU_STATUS;
    
}CRU_REG, *pCRU_REG;              //将IO mux 放在别处处理

//#define     CRU_BASE                            ((UINT32)0x40000000)
#define     CRUReg                              ((CRU_REG *) CRU_BASE)

typedef volatile struct tagGRF_REG
{
    UINT32 GPIO0A_IOMUX;
    UINT32 GPIO0B_IOMUX;
    UINT32 GPIO0C_IOMUX;         
    UINT32 GPIO0D_IOMUX;        
    UINT32 GPIO0A_PULL;         
    UINT32 GPIO0B_PULL;
    UINT32 GPIO0C_PULL;
    UINT32 GPIO0D_PULL;   
    UINT32 INTER_CON0;
    UINT32 INTER_CON1;
   	UINT32 IOMUX_CON0;
    UINT32 IOMUX_CON1;
    UINT32 OTGPHY_CON0;
    UINT32 OTGPHY_CON1;
    UINT32 SARADC_CON;
    UINT32 PADREN_CON;
    UINT32 MPMU_DAC_CON;
    UINT32 MPMU_HP_CON;
    UINT32 MPMU_PWR_CON;
    UINT32 MPMU_STATE;
    UINT32 RESERVED[(0x60-0x50)/4];
    UINT32 SOFT_DBG0;
    UINT32 SOFT_DBG1;
    UINT32 PRJ_ID;
    UINT32 PRJ_VER;
}GRF_REG, *pGRF_REG;              //将IO mux 放在别处处理

//#define     GRF_BASE                            ((UINT32)0x40010000)
#define     GRFReg                              ((GRF_REG *) GRF_BASE)

#endif


