/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: SDCtrl.c
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



/*------------------------------------ Defines -------------------------------*/

//#define pSDCReg(n)        ((n == 0) ? ((pSDC_REG_T)SDC0_ADDR) : ( (n==1) ? ((pSDC_REG_T)SDC1_ADDR):(((pSDC_REG_T)SDC2_ADDR)) ) )
//#define pSDCFIFOADDR(n)   ((n == 0) ? ((volatile uint32 *)SDC0_FIFO_ADDR) : ( (n==1)? ((volatile uint32 *)SDC1_FIFO_ADDR): ((volatile uint32 *)SDC2_FIFO_ADDR) ) )
//#define pSDC_DMACB(n)     ((n == SDC0)? SDC0_DMACB : ((n == SDC1)? SDC1_DMACB : SDC2_DMACB));   
//#define pSDC_ISR(n)       ((n ==SDC0)? SDC0_ISR : ((n == SDC1)? SDC1_ISR : SDC2_ISR)

#define pSDCFIFOADDR(n)    ((volatile uint32 *)SDC0_FIFO_ADDR)
#define pSDCReg(n)         ((pSDC_REG_T)SDC0_ADDR)
//#define pSDC_DMACB(n)      SDC0_DMACB
#define pSDC_ISR(n)        SDC0_ISR 


//#define SDC_Start(reg, cmd) reg->SDMMC_CMD = cmd;
#define SDC_EnanbleDMA(reg) reg->SDMMC_CTRL |= ENABLE_DMA
#define SDC_DisableDMA(reg) reg->SDMMC_CTRL &= ~ENABLE_DMA

#define SDC_WriteData(fifo, buf, len)       \
while(len--)                                \
{                                           \
    *fifo = *buf++;                         \
}

#define SDC_ReadData(buf, fifo, len)        \
while(len--)                                \
{                                           \
    *buf++ = *fifo;                         \
}

#define SDC_WaitBitSet(cond)                \
do                                          \
{                                           \
    if (cond)                               \
        break;                              \
    SDA_Delay(1);                           \
}while(1)  

/*----------------------------------- Typedefs -------------------------------*/

/* SDMMC Host Controller register struct */
typedef volatile struct TagSDC_REG
{
    volatile uint32 SDMMC_CTRL;        //SDMMC Control register
    volatile uint32 SDMMC_PWREN;       //Power enable register
    volatile uint32 SDMMC_CLKDIV;      //Clock divider register
    volatile uint32 SDMMC_CLKSRC;      //Clock source register
    volatile uint32 SDMMC_CLKENA;      //Clock enable register
    volatile uint32 SDMMC_TMOUT;       //Time out register
    volatile uint32 SDMMC_CTYPE;       //Card type register
    volatile uint32 SDMMC_BLKSIZ;      //Block size register
    volatile uint32 SDMMC_BYTCNT;      //Byte count register
    volatile uint32 SDMMC_INTMASK;     //Interrupt mask register
    volatile uint32 SDMMC_CMDARG;      //Command argument register
    volatile uint32 SDMMC_CMD;         //Command register
    volatile uint32 SDMMC_RESP0;       //Response 0 register
    volatile uint32 SDMMC_RESP1;       //Response 1 register
    volatile uint32 SDMMC_RESP2;       //Response 2 register
    volatile uint32 SDMMC_RESP3;       //Response 3 register
    volatile uint32 SDMMC_MINTSTS;     //Masked interrupt status register
    volatile uint32 SDMMC_RINTSTS;     //Raw interrupt status register
    volatile uint32 SDMMC_STATUS;      //Status register
    volatile uint32 SDMMC_FIFOTH;      //FIFO threshold register
    volatile uint32 SDMMC_CDETECT;     //Card detect register
    volatile uint32 SDMMC_WRTPRT;      //Write protect register
    volatile uint32 SDMMC_GPIO;        //GPIO register
    volatile uint32 SDMMC_TCBCNT;      //Transferred CIU card byte count
    volatile uint32 SDMMC_TBBCNT;      //Transferred host/DMA to/from BIU_FIFO byte count
    volatile uint32 SDMMC_DEBNCE;      //Card detect debounce register
    volatile uint32 SDMMC_USRID;        //User ID register        
    volatile uint32 SDMMC_VERID;        //Synopsys version ID register
    volatile uint32 SDMMC_HCON;         //Hardware configuration register          
    volatile uint32 SDMMC_UHS_REG;      //UHS-1 register  
    volatile uint32 SDMMC_RST_n;        //Hardware reset register
    volatile uint32 SDMMC_CARDTHRCTL;   //Card Read Threshold Enable
    volatile uint32 SDMMC_BACK_END_POWER; //Back-end Power
}SDC_REG_T,*pSDC_REG_T;

/*控制器传输信息*/
typedef struct tagSDC_TRANS_INFO
{
    uint32     TransLen;                   //已经发送或接收的数据长度
    uint32     NeedLen;                  //需要传输的数据长度
    uint32    *pBuf;                    //中断数据接收或发送数据用到的buf地址                                        //指针用32 bit的uint32指针，就可以满足SDMMC FIFO要求的32 bits对齐的要求了，                                    //这样就算你的数据没有4字节对齐，也会因为用了uint32指针，每次向FIFO操作是4字节对齐的。
}SDC_TRANS_INFO, *pSDC_TRANS_INFO;


/* SDMMC Host Controller Information */
typedef struct tagSDMMC_CTRL
{
    uint32              LowPower;
    pSDC_REG_T          pReg;           //SDMMC Host Controller Register, only for debug
    HOST_BUS_WIDTH      BusWidth;       //SDMMC Host Controller Bus Width，
                                      //控制器支持的总线宽度是由硬件设计的IO共用情况决定的，
                                      //插在该控制器上的卡设置总线宽度时受到该宽度的限制
    HOST_BUS_WIDTH      ConfigWidth;  
    SDC_TRANS_INFO      TransInfo;      //控制器中断要用到的信息
    pEVENT              event;          //SDMMC Host Controller对应的事件
    uint32              CardFreq;       //卡的工作频率,单位KHz，这个频率只有SDM层有权修改
    bool                UpdatedFreq;  //表示是否需要更新卡的频率，用于由于AHB频率改变而导致需要更新卡的频率,TRUE:要更新，FALSE:不用更新

    bool                present;          //记录目前插拔的状态
    bool                changed;          //用于指示卡是否被插拔过  

} SDMMC_CTRL, *pSDMMC_CTRL;

/*-------------------------- Forward Declarations ----------------------------*/

int sdio_int_num = 0;

/* ------------------------------- Globals ---------------------------------- */

_ATTR_SD_DATA_ SDMMC_CTRL gSDMMCCtrl[MAX_SDC_NUM];

extern int process_sdio_pending_irqs(void);
_ATTR_SD_CODE_
static void SDIOC_HandleIRQ(SDMMC_PORT_E SDCPort)
{
    volatile uint32  value = 0;
    SDMMC_CTRL *     pSDC = &gSDMMCCtrl[SDCPort];
    pSDC_REG_T       pReg = pSDCReg(SDCPort);

    value = pReg->SDMMC_MINTSTS;
    
    /*sdio interrupt.*/
    if (value & SDIO_INT)
	{	
		pReg->SDMMC_RINTSTS = SDIO_INT;
        
	    #if 1
		/* loop get data..*/
        SDA_SetEvent(pSDC->event, SDIO_EVENT);
	    #else
		/* interput get data..*/
		process_sdio_pending_irqs();
	    #endif
	}
    return;
}


_ATTR_SD_CODE_
bool SDC_CheckCard(SDMMC_PORT_E SDCPort, uint32 event)
{
    bool present, ret;
    pSDMMC_CTRL pSDC = &gSDMMCCtrl[SDCPort];

    present = SDA_CheckCard(SDCPort);
    if (present != pSDC->present)
    {
        pSDC->changed = 1;
        pSDC->present = present;
        //SDC_NotifyClient(pSDC, EV_HOTPLUG, present);
    }
    if (event == 1)
    {
        ret = present;
    }
    else if (event == 2)//MD_EVENT_CHANGE
    {
        ret = pSDC->changed;
        pSDC->changed = 0;
    }
    else
    {
        ret = FALSE;
    }
    return ret;
}


_ATTR_SD_CODE_
int32  SDC_GetBusWidth(SDMMC_PORT_E SDCPort, HOST_BUS_WIDTH *pWidth)
{
    *pWidth = gSDMMCCtrl[SDCPort].ConfigWidth;
    return SDC_SUCCESS;
}


_ATTR_SD_CODE_
void SDC0_ISR(void)
{
	SDIOC_HandleIRQ(SDC0);
}

_ATTR_SD_CODE_
int32 SDC_GetSdioEvent(uint32 timeout)
{
    int32 ret = SDC_SUCCESS;

    if (SDC_SUCCESS != SDOAM_GetEvent(gSDMMCCtrl[SDC0].event, SDIO_EVENT, timeout))
    {
        ret = SDC_BUSY_TIMEOUT;
    }
    
    return ret;
}
