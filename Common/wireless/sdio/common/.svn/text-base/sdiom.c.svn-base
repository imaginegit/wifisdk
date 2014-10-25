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
#include "Gpio.h"

#if 1

/*------------------------------------ Defines -------------------------------*/


/*----------------------------------- Typedefs -------------------------------*/

/* SDM Port Information */
typedef struct SDIOM_DRIVER_tag
{
    SDM_CARD_INFO CardInfo[MAX_SDC_NUM];
}SDIOM_DRIVER;


/*-------------------------- Forward Declarations ----------------------------*/



/* ------------------------------- Globals ---------------------------------- */



/*-------------------------------- Local Statics: ----------------------------*/
static SDIOM_DRIVER gSDIOMDriver;

/*--------------------------- Local Function Prototypes ----------------------*/

/*--------------------------- Extern Function Prototypes ----------------------*/

/*------------------------ Function Implement --------------------------------*/

void * SDIOM_GetCardInfo(int32 CardId)
{
    return &gSDIOMDriver.CardInfo[CardId];
}


static int32 SDIO_Module_Attach(SDM_CARD_INFO* pCard)
{
    int32  ret = SDM_FALSE;
    uint32 status = 0;
    uint32 nf;
    uint32 mp;

    ret = SDIOC_SendCmd(pCard->SDCPort, (SDIO_IO_SEND_OP_COND | SD_NODATA_OP | SD_RSP_R4 | WAIT_PREV), 0, &status);
    if (SDC_SUCCESS != ret)
		return SDM_FALSE;
	
    nf = (status >> 28) & 0x7;//bit31-bit29 : number of I/O functions...
    mp = (status >> 27) & 0x1;//bit28 : memory present...
	printf("SDIO_Module_Attach nf:0x%x mp:0x%x status:0x%x\n",nf, mp,status);
    if ((mp == 0) && (nf > 0) && (status & 0xFFFF00))
    {
		//bit27-bit26 : stuff bits
		//bit25 : S18A
		//bit24-bit0 : The supported minimum and maximum values for VDD
		  //bit0-bit7 : reserved bits no use.
		  //bit8 : 2.0-2.1
		  //bit9 : 2.1-2.2
		  //.......
		  //bit23: 3.5-3.6
        /* SDIO-only Card */
		pCard->ocr = status;
		
        ret = mmc_attach_sdio(pCard);
        if (ret == SDC_SUCCESS)
        {
			ret = sdio_function_init(pCard);;
			if (ret != SDC_SUCCESS)
			{
				return SDM_FALSE;
			}
        }
    }

	return SDM_SUCCESS;
}

int32 SDIO_Module_Init(void)
{
    int32 ret = SDC_SUCCESS;
    SDIOM_DRIVER* pSDM = &gSDIOMDriver;
    SDM_CARD_INFO*  pCard;	
   
    SDA_Init();
     
    memset(pSDM, 0, sizeof(SDIOM_DRIVER));
    pSDM->CardInfo[SDC0].CardId = -1;
	
    SDIOC_Init(SDC0);
	
	pCard = &gSDIOMDriver.CardInfo[SDC0];

    memset(pCard, 0x00, sizeof(SDM_CARD_INFO));

    pCard->CardId = -1;
    pCard->type = UNKNOW_CARD;
    pCard->SDCPort = SDC0;
    pCard->fops.read  = NULL;
    pCard->fops.write = NULL;

    /* reset SDC */
    SDIOC_SetPower(pCard->SDCPort, TRUE); //open sdmmc clk gate,and provide power to card.
    SDIOC_SetBusWidth(pCard->SDCPort, BUS_WIDTH_1_BIT);
    SDIOC_UpdateFreq(pCard->SDCPort, FOD_FREQ);

    SDA_Delay(1000);  //等待电源和时钟稳定

	sdio_reset(pCard);
	
    ret = SDIOC_SendCmd(SDC0, (SD_GO_IDLE_STATE | SD_NODATA_OP | SD_RSP_NONE | NO_WAIT_PREV | SEND_INIT), 0, NULL);
    if (SDC_SUCCESS != ret)
        return ret;
    
    SDA_Delay(100);  // 27有发现CMD0发送完以后延时一下再发其他命令能提高卡的识别率
    
    ret = SDIO_Module_Attach(pCard);
    
	return ret;
}

void SDIO_Module_Deinit(void)
{
	SDM_CARD_INFO*  pCard;	

	pCard = &gSDIOMDriver.CardInfo[SDC0];
	
    SDIOC_Reset(pCard->SDCPort);
    SDIOC_SetPower(pCard->SDCPort, FALSE);//关闭卡的电源和时钟源

    SDA_DeInit();
}

#endif
