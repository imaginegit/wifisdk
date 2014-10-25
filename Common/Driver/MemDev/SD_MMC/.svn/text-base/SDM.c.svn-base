/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: SDM.c
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
#include "sdio.h"


/*------------------------------------ Defines -------------------------------*/


/*----------------------------------- Typedefs -------------------------------*/

/* SDM Port Information */
typedef struct tagSDM_DRIVER
{
    SDM_CARD_INFO CardInfo[MAX_SDC_NUM];
}SDM_DRIVER, *pSDM_DRIVER;


/*-------------------------- Forward Declarations ----------------------------*/



/* ------------------------------- Globals ---------------------------------- */



/*-------------------------------- Local Statics: ----------------------------*/
_ATTR_SD_DATA_ static SDM_DRIVER gSDMDriver;

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/
#if (SDMMC_DRIVER==1)
/*
Name:        _SDMMC_Read
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/

_ATTR_SD_CODE_
static int32 _SDMMC_Read(int32 CardId, uint32 Addr, uint32 BlkCnt, void *pBuf)
{
    int32            ret = SDM_SUCCESS;
    uint32           status = 0;
    pSDM_CARD_INFO pCard = &gSDMDriver.CardInfo[CardId];

    if (1 == BlkCnt)
    {
        ret = SDC_Request(pCard->SDCPort, 
                        (SD_READ_SINGLE_BLOCK | SD_READ_OP | SD_RSP_R1 | WAIT_PREV), 
                        Addr, 
                        &status,
                        512,
                        (BlkCnt << 9), //<<9 == *512
                        pBuf);
    }
    else
    {
        ret = SDC_Request(pCard->SDCPort, 
                        (SD_READ_MULTIPLE_BLOCK | SD_READ_OP | SD_RSP_R1 | WAIT_PREV), 
                        Addr, 
                        &status,
                        512,
                        (BlkCnt << 9), 
                        pBuf);
        if (ret == SDC_SUCCESS
            || ret == SDC_END_BIT_ERROR
            || ret == SDC_DATA_READ_TIMEOUT
            || ret == SDC_DATA_CRC_ERROR)
        {
            SDC_SendCmd(pCard->SDCPort,
                        (SD_STOP_TRANSMISSION | SD_NODATA_OP | SD_RSP_R1B | STOP_CMD | NO_WAIT_PREV),
                        0,
                        &status);
        }
    }

    return ret;
}

/*
Name:        _SDMMC_Write
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_WRITE_CODE_
static int32 _SDMMC_Write(int32 CardId, uint32 Addr, uint32 BlkCnt, void *pBuf)
{
    int32            ret = SDM_SUCCESS;
    uint32           status = 0;
    pSDM_CARD_INFO pCard = &gSDMDriver.CardInfo[CardId];

    if (1 == BlkCnt)
    {
        ret = SDC_Request(pCard->SDCPort, 
                        (SD_WRITE_BLOCK | SD_WRITE_OP | SD_RSP_R1 | WAIT_PREV), 
                        Addr, 
                        &status,
                        512,
                        (BlkCnt << 9), 
                        pBuf);
    }
    else
    {
        ret = SDC_Request(pCard->SDCPort, 
                        (SD_WRITE_MULTIPLE_BLOCK | SD_WRITE_OP | SD_RSP_R1 | WAIT_PREV), 
                        Addr, 
                        &status,
                        512,
                        (BlkCnt << 9), 
                        pBuf);
        
        if (ret == SDC_SUCCESS
            || ret == SDC_END_BIT_ERROR
            || ret == SDC_DATA_CRC_ERROR)
        {
            SDC_SendCmd(pCard->SDCPort,
                        (SD_STOP_TRANSMISSION | SD_NODATA_OP | SD_RSP_R1B | STOP_CMD | NO_WAIT_PREV),
                        0,
                        &status);
        } 
    }
    
    return ret;
}


/*
Name:       _UnRegisterCard
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
static int32 _UnRegisterCard(int32 CardId)
{
    pSDM_CARD_INFO   pCard;

    pCard = &gSDMDriver.CardInfo[CardId];

    if (pCard->CardId == CardId)
    {
        //SDC_SetClock(pCard->SDCPort, FALSE);//关闭卡的时钟
        SDC_SetPower(pCard->SDCPort, FALSE);//关闭卡的电源和时钟源
        
        memset(pCard, 0, sizeof(SDM_CARD_INFO));
        pCard->CardId = -1;
        return SDM_SUCCESS;
    }
    
    return SDM_ERROR;
}


/*
Name:       _IdentifyUnknow
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
static int32 _IdentifyUnknow(pSDM_CARD_INFO pCard)
{
    int32            ret = SDM_FALSE;

#if (CONFIG_SD_SPEC == 1) 
    uint32           status = 0;
    uint32           nf;
    uint32           mp;

	//SD2_SEND_IF_COND(SD_CMD8)	CMD8 is required to support SDHC or SDXC
    ret = SDC_SendCmd(pCard->SDCPort, (SD2_SEND_IF_COND | SD_NODATA_OP | SD_RSP_R7 | WAIT_PREV), 0x1AA, &status);
    if (SDC_SUCCESS == ret)
    {
        /* SDIO-only Card or SDIO-SDHC/SD2.0 Combo Card or SDIO-SDHC/SD2.0 only Card or SD2.0 or SDHC */
        nf = 0;
        mp = 0;
        ret = SDC_SendCmd(pCard->SDCPort, (SDIO_IO_SEND_OP_COND | SD_NODATA_OP | SD_RSP_R4 | WAIT_PREV), 0, &status);
        if (SDC_SUCCESS == ret)
        {
            nf = (status >> 28) & 0x7;
            mp = (status >> 27) & 0x1;
            if ((mp == 1) && (nf > 0) && (status & 0xFFFF00))
            {
                /* SDIO-SDHC/SD2.0 Combo Card */
                //SDIOHC_SD20_ComboInit(&cardInfo);
                pCard->type = UNKNOW_CARD;
            }
            else if ((mp == 0) && (nf > 0) && (status & 0xFFFF00))
            {
                /* SDIO-only Card */
                //SDIO_OnlyInit(&cardInfo);
                pCard->type = UNKNOW_CARD;
            }
            else if(mp == 1)
            {
                /* SDIO-SDHC/SD2.0 only Card */
                ret = SDXX_Init(pCard, TRUE);
            }
            else
            {
                /* unknow card */
            }
        }
        else if (ret == SDC_RESP_TIMEOUT)
        {
            /* SD2.0 or SDHC */
            ret = SDXX_Init(pCard, TRUE);
        }
        else
        {
            /* must be error occured */
        }
    }
    else if (SDC_RESP_TIMEOUT == ret)
    {
        /* SDIO-only Card or SDIO-SD1.X Combo Card or SDIO-SD1.X only Card or SD1.X or MMC or SD2.0 or later with voltage mismatch */
        nf = 0;
        mp = 0;
		//dgl: frist CMD5.
        ret = SDC_SendCmd(pCard->SDCPort, (SDIO_IO_SEND_OP_COND | SD_NODATA_OP | SD_RSP_R4 | WAIT_PREV), 0, &status);
        if (SDC_SUCCESS == ret)
        {
            nf = (status >> 28) & 0x7;//bit31-bit29 : number of I/O functions...
            mp = (status >> 27) & 0x1;//bit28 : memory present...
            if((mp == 1) && (nf > 0) && (status & 0xFFFF00))
            {
                /* SDIO-SD1.X Combo Card */
                //SDIO_SD1X_ComboInit(&cardInfo);
                pCard->type = UNKNOW_CARD;
            }
            else if ((mp == 0) && (nf > 0) && (status & 0xFFFF00))
            {
                pCard->type = UNKNOW_CARD;
            }
            else if (mp == 1)
            {
                /* SDIO-SD1.X only Card */
                ret = SDXX_Init(pCard, FALSE);
            }
            else
            {
                /* unknow card */
            }
        }
        else if (ret == SDC_RESP_TIMEOUT)
        {
            /* SD1.X or MMC or SD2.0 or later with voltage mismatch */
            ret = SDC_SendCmd(pCard->SDCPort, (SD_APP_CMD | SD_NODATA_OP | SD_RSP_R1 | WAIT_PREV), 0, &status);
            if (SDC_SUCCESS == ret)
            {
                /* SD1.X or SD2.0 or later with voltage mismatch */
                ret = SDXX_Init(pCard, FALSE);
            }
            else if (SDC_RESP_TIMEOUT == ret)
            {
                #if (CONFIG_EMMC_SPEC ==1) 
                /* must be MMC */
                ret = MMC_Init(&pCard);
                #endif
            }
            else
            {
                /* must be error occured */
            }
        }
        else
        {
            /* must be error occured */
        }
    }
    else
    {
        /* must be error occured */
    }
#endif
    return ret;
}

/*
Name:       _IdentifyCard
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
static int32 _IdentifyCard(int32 CardId, uint32 type)
{
    //uint32           status = 0;
    int32            ret = SDC_SUCCESS;
    pSDM_CARD_INFO   pCard;

    if (CardId >= MAX_SDC_NUM || CardId < 0)
    {
        return SDC_PARAM_ERROR;
    }

    pCard = &gSDMDriver.CardInfo[CardId];

    memset(pCard, 0x00, sizeof(SDM_CARD_INFO));
    pCard->CardId = -1;
    pCard->type = UNKNOW_CARD;
    pCard->SDCPort = (SDMMC_PORT_E)CardId;
        
    /* reset SDC */
    SDC_SetPower(pCard->SDCPort, TRUE); //open sdmmc clk gate,and provide power to card.
    SDC_SetBusWidth(pCard->SDCPort, BUS_WIDTH_1_BIT);
    SDC_UpdateFreq(pCard->SDCPort, FOD_FREQ);

    if (eMMC2G == type)
    {
        SDC_ResetEmmc();            
    }
    SDA_Delay(100);  //等待电源和时钟稳定

    ret = SDC_SendCmd(pCard->SDCPort, (SD_GO_IDLE_STATE | SD_NODATA_OP | SD_RSP_NONE | NO_WAIT_PREV | SEND_INIT), 0, NULL);
    if (SDC_SUCCESS != ret)
    {
        return ret;
    }
    
    SDA_Delay(100);  // 27有发现CMD0发送完以后延时一下再发其他命令能提高卡的识别率

    if (UNKNOW_CARD == type)//dgl go this path.
    {
        ret = _IdentifyUnknow(pCard);
    }
#if (CONFIG_EMMC_SPEC ==1) 
    else if (eMMC2G == type)
    {
        ret = MMC_Init(pCard);
    }
#endif
    else
    {
        ret = SDC_PARAM_ERROR;
    }
    
    /*能识别的卡 pCard->type != UNKNOW_CARD*/    
    if (pCard->type == UNKNOW_CARD)
    {
        SDC_Reset(pCard->SDCPort);
        //SDC_SetClock(pCard->SDCPort, FALSE);//关闭卡时钟
        SDC_SetPower(pCard->SDCPort, FALSE);//关闭卡的电源和时钟源

        return ret;
    }
    else
    {
        pCard->CardId = CardId;
		if (pCard->type != SDIO)
		{
	        pCard->fops.read  = _SDMMC_Read;
	        pCard->fops.write = _SDMMC_Write;
		}
		else
		{
	        pCard->fops.read  = NULL;
	        pCard->fops.write = NULL;
		}
        return SDM_SUCCESS;
    }
}

/*
Name:        _IsCardIdValid
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
bool  _IsCardIdValid(int32 CardId)
{
    pSDM_CARD_INFO pCard;

    if (CardId < MAX_SDC_NUM)
    {
        pCard = &gSDMDriver.CardInfo[CardId];
        if (-1 != pCard->CardId)
        {   
            return TRUE;
        }
    }
    
    return FALSE;    
}

/*
Name:        _IsCardIdValid
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
SDM_CARD_INFO * SDM_GetCardInfo(int32 CardId)
{
    return &gSDMDriver.CardInfo[CardId];
}

/*
Name:        SDM_Init
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
uint32 SDM_Init(void)
{
    SDMMC_PORT_E i;
    pSDM_DRIVER pSDM = &gSDMDriver;

    memset(pSDM, 0, sizeof(SDM_DRIVER));

    SDA_Init();
    for (i = SDC0; i < MAX_SDC_NUM; i++ )
    {
        pSDM->CardInfo[i].CardId = -1;
        SDC_Init((SDMMC_PORT_E) i);
    }
    
    return SDM_SUCCESS;
}





/*
Name:        SDM_Read
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
int32 SDM_Read(int32 CardId, uint32 Addr, uint32 BlkCnt, void *pBuf)
{

    pSDM_CARD_INFO pCard;
    uint32           mul;

    if (!_IsCardIdValid(CardId))
    {
        return SDM_PARAM_ERROR;
    }

    pCard = &gSDMDriver.CardInfo[CardId];
    if ((BlkCnt > (pCard->capability)) || ((Addr + BlkCnt) > (pCard->capability)))
    {
        return SDM_PARAM_ERROR;
    }

    //SDHC地址是以block(512)为单位的，而早期协议地址是以byte为单位
    mul = (pCard->type & (SDHC | eMMC2G))? 0:9;

    return pCard->fops.read(CardId, Addr<<mul, BlkCnt, pBuf); 
}


/*
Name:        SDM_Write
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_WRITE_CODE_
int32 SDM_Write(int32 CardId, uint32 Addr, uint32 BlkCnt, void *pBuf)
{
    pSDM_CARD_INFO pCard;
    uint32           mul;

    if (!_IsCardIdValid(CardId))
    {
        return SDM_PARAM_ERROR;
    }

    pCard = &gSDMDriver.CardInfo[CardId];
    if ((BlkCnt > (pCard->capability)) || ((Addr + BlkCnt) > (pCard->capability)))
    {
        return SDM_PARAM_ERROR;
    }

    if (pCard->WriteProt) //只有SD卡才有写保护,MMC卡这个值总是为0
    {
        return SDM_CARD_WRITE_PROT;
    }

    //SDHC地址是以block(512)为单位的，而早期协议地址是以byte为单位
    mul = (pCard->type & (SDHC | eMMC2G))? 0:9;

    return pCard->fops.write(CardId, Addr<<mul, BlkCnt, pBuf);

}

/*
Name:        SDM_GetCapability
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
uint32 SDM_GetCapability(int32 CardId)
{
    if (!_IsCardIdValid(CardId))
        return 0;

    return gSDMDriver.CardInfo[CardId].capability;
}

/*
Name:        SDM_Open
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
int32 SDM_Open(int32 CardId, uint32 type)
{

    if (CardId >= MAX_SDC_NUM || CardId < 0)
        return SDM_PARAM_ERROR;

    if (_IsCardIdValid(CardId))
         return SDM_SUCCESS;

    return _IdentifyCard(CardId, type);  
}

/*
Name:        SDM_Close
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
int32 SDM_Close(int32 CardId)
{

    if (!_IsCardIdValid(CardId))
        return SDM_PARAM_ERROR;

    return _UnRegisterCard(CardId);
}

/*
Name:        SDM_IOCtrl
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
int32 SDM_IOCtrl(int32 CardId, uint32 cmd, uint32 param)
{
    pSDM_CARD_INFO  pCard;
    int32           ret = SDM_SUCCESS;

    if (!_IsCardIdValid(CardId))
         return SDM_PARAM_ERROR;

    pCard = &gSDMDriver.CardInfo[CardId];
    switch (cmd)
    {
        case SDM_IOCTR_GET_CAPABILITY:
            *(uint32 *)param = pCard->capability;
            break;
            
	#if (CONFIG_EMMC_SPEC == 1) 
        case SDM_IOCTR_GET_BOOT_CAPABILITY:
            *(uint32 *)param = pCard->BootSize;
            break;
        case SDM_IOCTR_PARTITION_CONFIG:
            ret = MMC_PartitionConfig(pCard, (uint8)param);
            break;
	#endif
         
        default:
            ret = SDM_PARAM_ERROR;
            break;
    }

    return ret;

}


/*
Name:        SDM_DeInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
void SDM_DeInit(void)
{

}

#if (CONFIG_SD_SPEC ==1) 
/*
Name:        SDCardInit
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_INIT_CODE_
uint32 SDMCardProbe(void)
{
    uint32 ret=OK;
    /*if (SDM_SUCCESS != SDM_Init())
    {
        return ERROR;
    }*/
    
    if (SDM_SUCCESS != SDM_Open(SDM_DEF_ID, UNKNOW_CARD))
    {
        ret= ERROR;
    }
     
    //MDRegDiskID(MEM_DEV_SD2);
    /*清除控制器报告的卡插拔变化*/
    SDC_CheckCard(SDM_DEF_SDC, MD_EVENT_CHANGE);

    return ret;
}

/*
Name:        MD_SDRead
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
uint32 MDSDMRead(uint8 lun, uint32 sec, uint32 nSec, void *pBuf)
{
    if (SDM_SUCCESS != SDM_Read(SDM_DEF_ID, sec, nSec, pBuf))
        return ERROR;

    return OK;
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
uint32 MDSDMWrite(uint8 lun, uint32 sec, uint32 nSec, void *pBuf)
{
    if (SDM_SUCCESS != SDM_Write(SDM_DEF_ID, sec, nSec, pBuf))
        return ERROR;

    return OK;
}

/*
Name:        MDSDMGetInfo
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
uint32 MDSDMGetInfo(uint8 lun, pMEMDEV_INFO pDevInfo)
{
    if (!_IsCardIdValid(SDM_DEF_ID))
    {
        return ERROR;
    }

    //pDevInfo->Manufacturer = 0;
    pDevInfo->BlockSize = SDM_BOOT_SIZE;  //固定成128K
    pDevInfo->PageSize = 8;
    pDevInfo->capacity= SDM_GetCapability(SDM_DEF_ID);

    return OK;
}

/*
Name:        SDM_CheckCard
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
bool SDM_CheckCard(SDMMC_PORT_E SDCPort)
{
	bool present;
	pSDM_CARD_INFO pCard;
    
	pCard = &gSDMDriver.CardInfo[SDM_DEF_ID];
   
    present = SDC_CheckCard(SDCPort, MD_EVENT_PRESENT);
    if (!present)
    {
        pCard->CardId = -1;
    }

    return present;
}

/*
Name:        MDSDMChkEvent
Desc:       
Param:      
Return:     
Global: 
Note:   
Author: 
Log:
*/
_ATTR_SD_CODE_
bool MDSDMChkEvent(uint32 event)
{
    bool present;
    bool ret;

    present = SDM_CheckCard(SDM_DEF_SDC);
    if (event == MD_EVENT_PRESENT)
    {
        ret = present;
    }
    else if (event == MD_EVENT_CHANGE)
    {
        ret = SDC_CheckCard(SDM_DEF_SDC, MD_EVENT_CHANGE);  
    }
    else if (event == MD_EVENT_VAILID)
    {
        ret = _IsCardIdValid(SDM_DEF_ID);
    }
    else
    {
        ret = FALSE;
    }

    return ret;
}

#endif

#if 0
/*extern*/ uint32 ProbeReadBuf[PAGE_SIZE];   //FLASH探测时用的PAGE BUF
/*extern*/ uint32 ProbeWriteBuf[PAGE_SIZE];  //FLASH探测时用的PAGE BUF
//static uint8 TestRet;
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
void SDM_Test(void)
{
    uint32 i;
    
    //TestRet = 0;
	for (i=0; i<512*4; i++)
	{
	    ProbeWriteBuf[i]=i+1;
	    //ProbeWriteBuf[i] = 0xFFFFFFFF;
	}

    if (SDM_SUCCESS != SDM_Init())
    {
        while(1);
    }
    
    if (SDM_SUCCESS != SDM_Open(SDM_CARD_ID, UNKNOW_CARD))
    {
        while(1);
    }


    memset(ProbeReadBuf, 0, 512*4);
    if (SDM_SUCCESS != SDM_Read(SDM_CARD_ID, 4, 4, ProbeReadBuf))
    {
         while(1);
    }

   
    if (SDM_SUCCESS != SDM_Write(SDM_CARD_ID, 0, 4, ProbeWriteBuf))
    {
        while(1);
    }

    memset(ProbeReadBuf, 0, 512*4);
    if (SDM_SUCCESS != SDM_Read(SDM_CARD_ID, 0, 4, ProbeReadBuf))
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


_ATTR_SD_CODE_
bool SDM_CheckCard(SDMMC_PORT_E SDCPort)
{
	bool present;
	pSDM_CARD_INFO pCard;
    
	pCard = &gSDMDriver.CardInfo[SDM_DEF_ID];
   
    present = SDC_CheckCard(SDCPort, MD_EVENT_PRESENT);
    if (!present)
    {
        pCard->CardId = -1;
    }

    return present;
}