/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: eMMC.c
Desc: 

Author: chenfen
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/

#include "SDConfig.h"
#if  (CONFIG_EMMC_SPEC ==1)

#include "Gpio.h"

#include "eMMC.h"

/*------------------------------------ Defines -------------------------------*/

#define EMMC_IDB_NUM    1

#define  EMMC_BOOT_SIZE   (128*1024/512)

#define ACCESS_NO_BOOT 0x0   //No access to boot partition (default)
#define ACCESS_BOOT1   0x1   //R/W boot partition 1
#define ACCESS_BOOT2   0x2   //R/W boot partition 2

#define NO_BOOT_EN     (0<<3) //Device not boot enabled (default)
#define BOOT1_EN       (1<<3) //Boot partition 1 enabled for boot
#define BOOT2_EN       (2<<3) //Boot partition 2 enabled for boot

#define NO_BOOT_ACK    (0<<6) //No boot acknowledge sent (default)
#define BOOT_ACK       (1<<6) //Boot acknowledge sent during boot operation


/*----------------------------------- Typedefs -------------------------------*/


/*-------------------------- Forward Declarations ----------------------------*/



/* ------------------------------- Globals ---------------------------------- */



/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/

void EmmcTest(void);

/*------------------------ Function Implement --------------------------------*/


/*
Name:       EmmcGetCapacity
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
uint32 EmmcGetCapacity(uint8 lun)
{
    uint32 capacity = 0;
    
    switch (lun)
    {
        case 0:
            capacity = SysProgDiskCapacity;
            break;
        case 1:
            capacity = SysDataDiskCapacity;
            break;
        case 2:
            capacity = SysProgDiskCapacity+SysDataDiskCapacity+SysUserDisk2Capacity;
            capacity = SDM_GetCapability(SDM_DEF_ID) - capacity;
            break;
        case 3:
            capacity = SysUserDisk2Capacity;
            break;
        case 0xff:
            capacity = SDM_GetCapability(SDM_DEF_ID);
            break;
        default:
            break;
    }

    return (capacity);
}

/*
Name:        MDEmmcGetInfo
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
uint32 MDEmmcGetInfo(uint8 lun, pMEMDEV_INFO pDevInfo)
{
    //pDevInfo->Manufacturer = 0;
    pDevInfo->BlockSize = SDM_BOOT_SIZE;  //固定成128K
    pDevInfo->PageSize = 8;
    pDevInfo->capacity= EmmcGetCapacity(lun);

    return OK;
}


/*
Name:        MDEmmcRead
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
uint32 MDEmmcRead(uint8 lun, uint32 LBA, uint32 nSec, void *pBuf)
{
    int32 ret = ERROR;

    DataPortMuxSet(IO_SDMMC);

    switch (lun)
    {
        case 3:
            LBA += EmmcGetCapacity(2);
        case 2:
            LBA += EmmcGetCapacity(1);
        case 1:
            LBA += EmmcGetCapacity(0);
        case 0:
            
            if ((LBA+nSec) > EmmcGetCapacity(0xff))
                break;

            ret = SDM_Read(SDM_DEF_ID, LBA, nSec, pBuf);
            if (SDM_SUCCESS != ret)
            {
                DEBUG("EMMC ERR");
            }
            
            ret = (SDM_SUCCESS == ret)? OK : ERROR;
 
        default:
            break;
    }

    DataPortMuxSet(IO_LCD);
    return (uint32)ret;
}

/*
Name:        MD_SDWrite
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_WRITE_CODE_
uint32 MDEmmcWrite(uint8 lun, uint32 LBA, uint32 nSec, void *pBuf)
{
    int32 ret = ERROR;

    DataPortMuxSet(IO_SDMMC);

    switch (lun)
    {
        case 3:
            LBA += EmmcGetCapacity(2);
        case 2:
            LBA += EmmcGetCapacity(1);
        case 1:
            LBA += EmmcGetCapacity(0);
        case 0:
        #ifdef SYS_PROTECT
            if (LBA < EmmcGetCapacity(0))
                break;
        #endif
            
            if ((LBA+nSec) > EmmcGetCapacity(0xff))
                break;

            ret = SDM_Write(SDM_DEF_ID, LBA, nSec, pBuf);
            ret = (SDM_SUCCESS == ret)? OK : ERROR;
 
        default:
            break;
    }

    DataPortMuxSet(IO_LCD);
    return (uint32)ret;
}

/*
Name:       EmmcInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
uint32 EmmcInit(void)
{
    int32 ret = OK;

    DataPortMuxSet(IO_SDMMC);
    if (SDM_SUCCESS != SDM_Init())
    {
        ret =  ERROR;
        goto END;
    }
    
    if (SDM_SUCCESS != SDM_Open(SDM_EMMC_ID, eMMC2G))
    {
        ret =  ERROR;
        goto END;
    }
    
    if (SDM_SUCCESS != SDM_IOCtrl(SDM_EMMC_ID, SDM_IOCTR_PARTITION_CONFIG, NO_BOOT_ACK|BOOT1_EN|ACCESS_BOOT1))
    {
        ret =  ERROR;
        goto END;
    }

    {
        uint32 i;
        uint8 DataBuf[1024];
        pIDSEC0 IdSec0;
        pIDSEC1 IdSec1;
      
        for (i=0; i<EMMC_IDB_NUM; i++)  //在2个BLOCK里查找ID PAGE
        {
            if (SDM_SUCCESS != SDM_Read(SDM_DEF_ID, (i*SDM_BOOT_SIZE), 2, DataBuf))
            {
                continue;
            }
                
            IdSec0 = (pIDSEC0)DataBuf;
            if (IdSec0->magic == 0xFCDC8C3B)   //0x0ff0aa55 + rc4
            {
                IdSec1=(pIDSEC1)(DataBuf+512);      //IdSec1 NOT RC4
                if (IdSec1->chipSignH==(CHIP_SIGN>>16) && IdSec1->chipSignL==(CHIP_SIGN&0xffff))
                {
                    //spec->PageSize=IdSec1->pageSize;
                    //spec->BlockSize=IdSec1->blockSize;
    
                    LoaderVer    = IdSec1->mainVer;
                    UpgradeTimes = IdSec1->UpgradeTimes;
                    
                    SysProgDiskCapacity = (uint32)(IdSec1->sysProgDiskCapacity)<<11;
                    SysProgRawDiskCapacity = SysProgDiskCapacity>>1;
                    SysDataDiskCapacity = SysUserDisk2Capacity=0;
                    
                    if ((SysProgDiskCapacity) <= SDM_GetCapability(SDM_DEF_ID)) 
                        ret = OK;
    
                    break;
                }
            }
        }
    }

    SDM_IOCtrl(SDM_EMMC_ID, SDM_IOCTR_PARTITION_CONFIG, NO_BOOT_ACK|BOOT1_EN|ACCESS_NO_BOOT);

END:
    DataPortMuxSet(IO_LCD);
    return ret;
}

/*
Name:       EmmcDeInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_ 
uint32 EmmcDeInit(void)
{
    CRUReg->CRU_SOFTRST_CON0 = 1<<12 | 1<<28;
    DelayUs(10);
    CRUReg->CRU_SOFTRST_CON0 = 0<<12 | 1<<28;
    
    return OK;
}

#if 0
/*extern*/ uint32 ProbeReadBuf[PAGE_SIZE];   //FLASH探测时用的PAGE BUF
/*extern*/ uint32 ProbeWriteBuf[PAGE_SIZE];  //FLASH探测时用的PAGE BUF
//uint8 TestEmmcRet;
/*
Name:       EmmcTest
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_FLASH_CODE_
void EmmcTest(void)
{
    uint32 i;

    //TestEmmcRet = 0;
	for (i=0; i<512*4; i++)
	{
	    ProbeWriteBuf[i]=i+1;
	    //ProbeWriteBuf[i] = 0xFFFFFFFF;
	}

    memset(ProbeReadBuf, 0, 512*4);
    if (SDM_SUCCESS != SDM_Read(SDM_EMMC_ID, 0, 4, ProbeReadBuf))
    {
        while(1);
    }

    if (SDM_SUCCESS != SDM_Write(SDM_EMMC_ID, 0, 4, ProbeWriteBuf))
    {
        while(1);
    }

    memset(ProbeReadBuf, 0, 512*4);
    if (SDM_SUCCESS != SDM_Read(SDM_EMMC_ID, 0, 4, ProbeReadBuf))
    {
        while(1);
    }

    for(i=0; i<4*128; i++)
    {
        if (ProbeReadBuf[i] != ProbeWriteBuf[i])
        {
            while(1);
        }
    }
    while(1);
}
#endif

#endif

