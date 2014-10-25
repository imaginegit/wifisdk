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
#include "interrupt.h"
#include "hw_dma.h"
#include "dma.h"

#if 1

/*------------------------------------ Defines -------------------------------*/

#define pSDIOCFIFOADDR(n)    ((volatile uint32 *)SDC0_FIFO_ADDR)
#define pSDIOCReg(n)         ((pSDC_REG_T)SDC0_ADDR)
#define pSDIOC_ISR(n)        SDC0_ISR 

#define SDIOC_WriteData(fifo, buf, len)       \
while(len--)                                \
{                                           \
    *fifo = *buf++;                         \
}

#define SDIOC_ReadData(buf, fifo, len)        \
while(len--)                                \
{                                           \
    *buf++ = *fifo;                         \
}

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
    uint32     TransLen;	//已经发送或接收的数据长度
    uint32     NeedLen;		//需要传输的数据长度
    uint32    *pBuf;    	//中断数据接收或发送数据用到的buf地址
                          	//指针用32 bit的uint32指针，就可以满足SDMMC FIFO要求的32 bits对齐的要求了，                                    //这样就算你的数据没有4字节对齐，也会因为用了uint32指针，每次向FIFO操作是4字节对齐的。
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



/* ------------------------------- Globals ---------------------------------- */

extern SDMMC_CTRL gSDMMCCtrl[MAX_SDC_NUM];

/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/
int32 SDIOC_SetPower(SDMMC_PORT_E SDCPort, bool enable)
{
    pSDC_REG_T pReg = pSDIOCReg(SDCPort);

    if (enable)
    {
        SDA_SetSrcClk(SDCPort, enable);
        pReg->SDMMC_PWREN = 1;   // power enable
    }
    else
    {

        pReg->SDMMC_PWREN = 0;   // power disable
        SDA_SetSrcClk(SDCPort, enable);
    }
    
    return SDC_SUCCESS;
}

int32 SDIOC_SetBusWidth(SDMMC_PORT_E SDCPort, HOST_BUS_WIDTH width)
{
    uint32       value = 0;
    pSDC_REG_T   pReg = pSDIOCReg(SDCPort);

    switch (width)
    {
        case BUS_WIDTH_1_BIT:
            value = BUS_1_BIT;
            break;
			
        case BUS_WIDTH_4_BIT:
            value = BUS_4_BIT;
            break;
			
        default:
            return SDC_PARAM_ERROR;
    }
    
    SDA_SetIOMux(SDCPort, width);
    pReg->SDMMC_CTYPE = value;
    gSDMMCCtrl[SDCPort].BusWidth = width;
    
    return SDC_SUCCESS;
}

int32 SDIOC_WaitBusy(SDMMC_PORT_E SDCPort)
{
    int32           ret = SDC_SUCCESS;
    pSDC_REG_T      pReg = pSDIOCReg(SDCPort);
    int32 timeout = 0;
    volatile uint32 value;
    
    while ((value = pReg->SDMMC_STATUS) & DATA_BUSY)
    {
        SDA_Delay(1);
        timeout++;
        if (timeout > 250000 * 4) //写最长时间250ms
        {
            ret = SDC_BUSY_TIMEOUT;
            break;
        }
    }
	
    return ret;
}

int32 SDIOC_Reset(SDMMC_PORT_E SDCPort)
{
    volatile uint32  value = 0;
    pSDC_REG_T       pReg = pSDIOCReg(SDCPort);
    int32            timeOut = 0;
    uint32           SrcFreq = SDA_GetSrcClkFreq();
        
    /* reset SDMMC IP */
    SDA_SetSrcClk(SDCPort, TRUE);//open the frequency gate to sdmmc.
    SDA_Delay(10);
    /* reset */

    pReg->SDMMC_CTRL = (FIFO_RESET | SDC_RESET | DMA_RESET);
    timeOut = 1000;
    
    //once set success,then system will clear the bits,so here to wait clear.
    while (((value = pReg->SDMMC_CTRL) & (FIFO_RESET | SDC_RESET | DMA_RESET)) && (timeOut > 0))
    {
        SDA_Delay(1);
        timeOut--;
    }
    
    if(timeOut == 0)
    {
        SDA_SetSrcClk(SDCPort, FALSE);
        return SDC_SDC_ERROR;
    }
    /* config FIFO */
    pReg->SDMMC_FIFOTH = (SD_MSIZE_16 | (RX_WMARK << RX_WMARK_SHIFT) | (TX_WMARK << TX_WMARK_SHIFT));
    pReg->SDMMC_CTYPE = BUS_1_BIT;
    pReg->SDMMC_CLKSRC = CLK_DIV_SRC_0;
    /* config debounce */
    pReg->SDMMC_DEBNCE = (DEBOUNCE_TIME*SrcFreq)&0xFFFFFF;
    /* config interrupt */
    pReg->SDMMC_RINTSTS = 0xFFFFFFFF;
	
    pReg->SDMMC_INTMASK = SDIO_INT;//note: 1:enable(unmask) 0:disable(mask)

    SDA_RegISR(SDCPort, pSDIOC_ISR(SDCPort));
    SDA_EnableIRQ(SDCPort);//dgl:global system interrupt enable.
    
    pReg->SDMMC_CTRL = ENABLE_INT;//dgl:global sdmmc controller interrupt enable.

    #ifdef SDMMC_DMA
    pReg->SDMMC_CTRL |= ENABLE_DMA;
    #endif
    
    return SDC_SUCCESS;
}

static int32 SDIOC_StartCmd(pSDC_REG_T pReg, uint32 cmd)
{
    int32 timeOut = 30000;
    pReg->SDMMC_CMD = cmd;
  
    while (--timeOut &&(pReg->SDMMC_CMD & START_CMD))       
    {                                                            
        DelayUs(2);
    }
    if(timeOut == 0)
    {
        return SDC_SDC_ERROR;
    }
    
    return SDC_SUCCESS;
}

static int32 SDIOC_SetClock(SDMMC_PORT_E SDCPort, bool enable)
{
    volatile uint32 value;
    pSDC_REG_T pReg = pSDIOCReg(SDCPort);

    value = (TRUE == enable)? CCLK_ENABLE : CCLK_DISABLE;// 1:Clock enabled; 0:Clock disable 
    pReg->SDMMC_CLKENA = value;

    if (SDC_SUCCESS != SDIOC_StartCmd(pReg, (START_CMD | UPDATE_CLOCK | WAIT_PREV)))
        return SDC_SDC_ERROR;

    return SDC_SUCCESS;
}

int32 SDIOC_UpdateFreq(SDMMC_PORT_E SDCPort, uint32 FreqKHz)
{
    volatile uint32 value = 0;
    uint32          SuitMmcClkDiv = 0;
    uint32          SuitCclkInDiv = 0;
    uint32          SrcFreq;
    int32           ret = SDC_SUCCESS;
    uint32          SecondFreq;
    pSDC_REG_T      pReg = pSDIOCReg(SDCPort);

    SrcFreq = SDA_GetSrcClkFreq();
    if (FreqKHz == 0)//频率不能为0，否则后面会出现除数为0
        return SDC_PARAM_ERROR;
	
	/*sdmmc clock disable.*/
    ret = SDIOC_SetClock(SDCPort, FALSE);
    if(ret != SDC_SUCCESS)
        return ret;

    //frist ensure the controler of sdmmc cclc_in ls less than 52M hz,if not,the setting for
    //registers will run away in forward.
    //SuitMmcClkDiv = SrcFreq/MMCHS_52_FPP_FREQ + ( ((SrcFreq%MMCHS_52_FPP_FREQ)>0) ? 1: 0 );
    SuitMmcClkDiv = (SrcFreq + MMCHS_52_FPP_FREQ - 1) / MMCHS_52_FPP_FREQ;
    if(SuitMmcClkDiv > MAX_SCU_CLK_DIV)
        SuitMmcClkDiv = MAX_SCU_CLK_DIV;

    SecondFreq = SrcFreq/SuitMmcClkDiv;
    //SuitCclkInDiv = (SecondFreq/FreqKHz) + ( ((SecondFreq%FreqKHz)>0)?1:0 );
    SuitCclkInDiv = (SecondFreq+FreqKHz-1)/FreqKHz;
	////DGL:(24000 + 200 - 1) / 200 = 120 SuitCclkInDiv = 120; 
    if (((SuitCclkInDiv & 0x1) == 1) && (SuitCclkInDiv != 1))
        SuitCclkInDiv++;  //除了1分频，保证是偶数倍

    if(SuitCclkInDiv > 510)
        return SDC_SDC_ERROR;

    //value = (SuitCclkInDiv == 1)? 1 : (SuitCclkInDiv >> 1) ;
    value = SuitCclkInDiv >> 1;
    
    pReg->SDMMC_CLKDIV = value;
    if (SDC_SUCCESS != SDIOC_StartCmd(pReg, (START_CMD | UPDATE_CLOCK | WAIT_PREV)))
        return SDC_SDC_ERROR;
    
    SDA_SetSrcClkDiv(SDCPort, SuitMmcClkDiv);

	/*sdmmc clock enable.*/
    return SDIOC_SetClock(SDCPort, TRUE);
}

int32 SDIOC_Init(SDMMC_PORT_E SDCPort)    
{
    pSDMMC_CTRL pSDC = &gSDMMCCtrl[SDCPort];
	
    memset(pSDC, 0, sizeof(SDMMC_CTRL));
    
    pSDC->BusWidth = BUS_WIDTH_1_BIT;
    pSDC->ConfigWidth = BUS_WIDTH_4_BIT;
    pSDC->pReg = pSDIOCReg(SDCPort);
    pSDC->event = SDA_CreateEvent((uint8)SDCPort);
    SDIOC_Reset(SDCPort);

    if (!SDA_CheckCard(SDCPort))
    {
        SDIOC_SetClock(SDCPort, FALSE);
        SDA_SetSrcClk(SDCPort, FALSE);
    }

    return SDC_SUCCESS;
}

static int SDIOC_Handle_Trans(SDMMC_PORT_E SDCPort, int waitevent)
{
    volatile uint32  value = 0;
    pSDC_REG_T       pReg = pSDIOCReg(SDCPort);
    uint32*          pBuf = 0;
    uint32 i = 0;
    volatile uint32 *pFIFO = pSDIOCFIFOADDR(SDCPort);
    pSDC_TRANS_INFO      pTransInfo = &gSDMMCCtrl[SDCPort].TransInfo;
	int ret = -1;
	
    uint timeout = 5000;
	while(1)
	{
		value = pReg->SDMMC_RINTSTS;//value = pReg->SDMMC_MINTSTS;
        #ifndef SDMMC_DMA  
	    if (value & RXDR_INT)
	    {
	        pBuf = &(pTransInfo->pBuf[pTransInfo->TransLen]);
	        i = (RX_WMARK+1);
	        SDIOC_ReadData(pBuf, pFIFO, i);
	        pTransInfo->TransLen += (RX_WMARK+1);

	        pReg->SDMMC_RINTSTS = RXDR_INT; // clear
	    }
	    if (value & TXDR_INT)
	    {
	        pBuf = &(pTransInfo->pBuf[pTransInfo->TransLen]);
	        i = pTransInfo->NeedLen-pTransInfo->TransLen;
	        (i>(FIFO_DEPTH-TX_WMARK))? (i=(FIFO_DEPTH-TX_WMARK), pTransInfo->TransLen+=(FIFO_DEPTH-TX_WMARK)) : (pTransInfo->TransLen=pTransInfo->NeedLen);
	        
	        SDIOC_WriteData(pFIFO, pBuf, i);
	        
	        pReg->SDMMC_RINTSTS = TXDR_INT; //clear
	    }
        #endif

		if (waitevent == CD_EVENT)
		{
		    if (value & CD_INT)//Command Done
		    {
		        //dgl SDA_SetEvent(pSDC->event, CD_EVENT);
		        pReg->SDMMC_RINTSTS = CD_INT;
				ret = CD_EVENT;
				break;
		    }
		}
		else if (waitevent == DTO_EVENT)
		{
		    if (value & DTO_INT)//Data Transfer Over
		    {
		        //dgl SDA_SetEvent(pSDC->event, DTO_EVENT);
		        pReg->SDMMC_RINTSTS = DTO_INT;
				ret = DTO_EVENT;
				break;
		    }
		    if (value & SBE_INT)//Start Bit Error
		    {
		        //dgl SDA_SetEvent(pSDC->event, DTO_EVENT);
		        pReg->SDMMC_INTMASK &= ~SBE_INT;  //暂时关闭，在中断外部清中断      
		        pReg->SDMMC_RINTSTS = SBE_INT;
				ret = DTO_EVENT;
				break;
		    }
		}
	    
	    if (value & FRUN_INT)//FIFO Underrun/Overrun Error
	    {
	        pReg->SDMMC_RINTSTS = FRUN_INT;
	    }
        
	    if (value & HLE_INT)//Hardware Locked Write Error
	    {
	        pReg->SDMMC_RINTSTS = HLE_INT;
	    }
        
        if(--timeout == 0)
        {
            printf("sd timeout = %08x\n", pReg->SDMMC_STATUS);
            break;
        }
        SDA_Delay(1); 
	}
    
    return ret;
}


static int32 SDIOC_ResetFIFO(pSDC_REG_T pReg)
{
    volatile uint32 value = 0;
    int32 timeOut = 0;

    value = pReg->SDMMC_STATUS;
    if (!(value & FIFO_EMPTY))
    {
        pReg->SDMMC_CTRL |= FIFO_RESET;
        timeOut = 5000;
        while (((value = pReg->SDMMC_CTRL) & (FIFO_RESET)) && (timeOut > 0))
            timeOut--;
        
        if(timeOut == 0)
            return SDC_SDC_ERROR;
    }

    return SDC_SUCCESS;
}

static int32 SDIOC_PrepareWrite(SDMMC_PORT_E SDCPort, void *pDataBuf, uint32 DataLen)
{
	volatile uint32 *pFIFO = pSDIOCFIFOADDR(SDCPort);
    //指针用32 bit的uint32指针，就可以满足SDMMC FIFO要求的32 bits对齐的要求了，这样就算你的数据没有4字节对齐，
    //也会因为用了uint32指针，每次向FIFO操作是4字节对齐的。
    uint32          *pBuf = (uint32 *)pDataBuf;
    //下面对于dataLen的操作，考虑到SDMMC控制器要求32bit对齐的要求，所以看起来对dataLen的操作比较麻烦
    //SDMMC控制器要求32bit对齐，如:要传输13 byte个数据，实际传入FIFO(写时)或从FIFO传出(读时)的数据必须要16个,
    uint32           count = (DataLen >> 2) + ((DataLen & 0x3) ? 1:0);   //用32bit指针来传，因此数据长度要除4
    uint32           len;
    pSDC_TRANS_INFO   pTransInfo = &gSDMMCCtrl[SDCPort].TransInfo;

    pTransInfo->NeedLen = count;
    pTransInfo->pBuf = (uint32 *)pDataBuf;

    //if write, fill FIFO to full before start
    count = (count > FIFO_DEPTH)? FIFO_DEPTH : count;
    len = count;
    SDIOC_WriteData(pFIFO, pBuf, len);
    pTransInfo->TransLen = count;

    return SDC_SUCCESS;
}

static int32 SDIOC_PrepareRead(SDMMC_PORT_E SDCPort, void *pDataBuf, uint32 DataLen)
{
    pSDC_TRANS_INFO      pTransInfo = &gSDMMCCtrl[SDCPort].TransInfo;
    
    pTransInfo->NeedLen = (DataLen >> 2);
    pTransInfo->TransLen = 0;
    pTransInfo->pBuf = (uint32 *)pDataBuf;

    return SDC_SUCCESS;
}

static int32 SDIOC_ReadRemain(SDMMC_PORT_E SDCPort, uint32 OriginalLen, void *pDataBuf)
{
    volatile uint32  value = 0;
    volatile uint32 *pFIFO = pSDIOCFIFOADDR(SDCPort);
    pSDC_REG_T       pReg = pSDIOCReg(SDCPort);
    uint32           i = 0;
    //指针用32 bit的uint32指针，就可以满足SDMMC FIFO要求的32 bits对齐的要求了，这样就算你的数据没有4字节对齐，
    //也会因为用了uint32指针，每次向FIFO操作是4字节对齐的。
    uint32          *pBuf = (uint32 *)pDataBuf;
    uint8           *pByteBuf = (uint8 *)pDataBuf;
    uint32           lastData = 0;
    //下面对于dataLen的操作，考虑到SDMMC控制器要求32bit对齐的要求，所以看起来对dataLen的操作比较麻烦
    //SDMMC控制器要求32bit对齐，如:要传输13 byte个数据，实际传入FIFO(写时)或从FIFO传出(读时)的数据必须要16个
    uint32           count = (OriginalLen >> 2) + ((OriginalLen & 0x3) ? 1:0);   //用32bit指针来传，因此数据长度要除4
    pSDC_TRANS_INFO      pTransInfo = &gSDMMCCtrl[SDCPort].TransInfo;

    value = pReg->SDMMC_STATUS;
    if (value & FIFO_EMPTY)
        return SDC_SUCCESS;

    //中断传输时，当dataLen/4小于等于RX_WMARK+1,或者dataLen没4字节对齐的，或者最后剩余的数据达不到RX_WMARK+1都会有剩余数据
    while ((pTransInfo->TransLen < pTransInfo->NeedLen) && (!(value & FIFO_EMPTY)))
    {
        pBuf[pTransInfo->TransLen++] = *pFIFO;
        value = pReg->SDMMC_STATUS;
    }

    if (count > (OriginalLen >> 2))
    {
        if(value & FIFO_EMPTY)
        {
            return SDC_SDC_ERROR;
        }
        lastData = *pFIFO;
        //填充剩余的1-3个字节
        for (i=0; i<(OriginalLen & 0x3); i++)
        {
            pByteBuf[(OriginalLen & 0xFFFFFFFC) + i] = (uint8)((lastData >> (i << 3)) & 0xFF);  //只考虑CPU为小端，little-endian
        }
    }

    return SDC_SUCCESS;
}

#ifdef SDMMC_DMA
uint8 SDIO_Dma_ReadRequest(uint8 * pbTrgBuf, uint32 dwLen)
{
    
    uint32 timeout = 0;
    eDMA_CHN channel;
    DMA_CFGX DmaCfg = {DMA_CTLL_SDMMC_RD, DMA_CFGL_SDMMC_RD, DMA_CFGH_SDMMC_RD, 0};                
    channel = DmaGetChannel();
    if ((channel != DMA_FALSE)/* && (channel < DMA_CHN2)*/)
    {
        DmaStart((uint32)(channel), (uint32)SDC0_FIFO_ADDR, (uint32)pbTrgBuf, dwLen >> 2, &DmaCfg, NULL);
        while (DMA_SUCCESS != DmaGetState(channel))
	    {
	        if (++timeout > (dwLen) * 1000)
	        {
	            printf("SDIO DMA Read Error: len = %d!\n", (dwLen));
                return SDC_SDC_ERROR;
	        }
	    }
        printf("SDIO DMA Read OK: len = %d!\n", (dwLen));
        return SDC_SUCCESS;
    }
    else
    {
        printf("SDIO DMA Read Error: len = %d!\n", (dwLen));
        return SDC_SDC_ERROR;
    }
}

uint8 SDIO_Dma_WriteRequest(uint8 * pbSrcBuf, uint32 dwLen)
{


    uint32 timeout = 0;
    eDMA_CHN channel;
    DMA_CFGX DmaCfg = {DMA_CTLL_SDMMC_WR, DMA_CFGL_SDMMC_WR, DMA_CFGH_SDMMC_WR, 0};

    channel = DmaGetChannel();
    if ((channel != DMA_FALSE)/* && (channel < DMA_CHN2)*/)
    {
        DmaStart((uint32)(channel), (uint32)pbSrcBuf, (uint32)SDC0_FIFO_ADDR, dwLen>>2, &DmaCfg, NULL);
        while (DMA_SUCCESS != DmaGetState(channel))
	    {
	        if (++timeout > dwLen * 1000)
	        {
	            printf("SDIO DMA Write Error: len = %d!\n", (dwLen));
                return SDC_SDC_ERROR;
	        }
	    }
        printf("SDIO DMA Write OK: len = %d!\n", (dwLen));
        return SDC_SUCCESS;
    }
    else
    {
        printf("SDIO DMA Write Error: len = %d!\n", (dwLen));
        return SDC_SDC_ERROR;
    }
    
}
#endif


int32 SDIOC_Request(SDMMC_PORT_E SDCPort,
                        uint32  cmd,
                        uint32  CmdArg,
                        uint32 *RespBuf,
                        uint32  BlkSize,
                        uint32  DataLen,
                        void   *pDataBuf)
{
    volatile uint32 value = 0;
    int32           ret = SDC_SUCCESS;
    pSDMMC_CTRL     pSDC= &gSDMMCCtrl[SDCPort];
    pSDC_REG_T      pReg = pSDC->pReg;

	IntDisable(INT_ID18_SDMMC);
	
    if (cmd & WAIT_PREV)
    {
        if (pReg->SDMMC_STATUS & DATA_BUSY)
        {
			ret = SDC_BUSY_TIMEOUT;
			goto cmddone;
        }
    }

    value = pReg->SDMMC_CMD;
    if(value & START_CMD)
    {
		ret = SDC_SDC_ERROR;
		goto cmddone;
	}

    //ensure the card is not busy due to any previous data transfer command
    if (!(cmd & STOP_CMD))
    {         
        SDIOC_ResetFIFO(pReg);
    }

    #ifndef SDMMC_DMA 
    if (cmd & DATA_EXPECT)
    {              
        if((cmd & SD_OP_MASK) == SD_WRITE_OP)
        {
            SDIOC_PrepareWrite(SDCPort, pDataBuf, DataLen);
        }
        else
        {
            SDIOC_PrepareRead(SDCPort, pDataBuf, DataLen);
        }
        
        pReg->SDMMC_BLKSIZ = BlkSize;
        pReg->SDMMC_BYTCNT = DataLen;  //这个寄存器的长度一定要设置为需要的长度，不用考虑SDMMC控制器的32bit对齐
    }
    #endif

    pReg->SDMMC_CMDARG = CmdArg;
    //set start bit,CMD/CMDARG/BYTCNT/BLKSIZ/CLKDIV/CLKENA/CLKSRC/TMOUT/CTYPE were locked
    if (SDC_SUCCESS != SDIOC_StartCmd(pReg, (cmd & ~(RSP_BUSY)) | START_CMD | USE_HOLD_REG))
    {
		ret = SDC_SDC_ERROR;
		goto cmddone;
    }

    if (cmd & STOP_CMD)
    {
        //随着STOP命令，可能还会有数据进来(如mutiple read时)，要再清除FIFO
        SDIOC_ResetFIFO(pReg);
    }

    //check response error, or response crc error, or response timeout
    value = pReg->SDMMC_RINTSTS;
    if (value & RTO_INT)
    {
        pReg->SDMMC_RINTSTS = (~SDIO_INT);//0xFFFFFFFF;  //如果response timeout，数据传输 被中止，清除所有中断
		ret = SDC_RESP_TIMEOUT;
		goto cmddone;
    }

	SDIOC_Handle_Trans(SDCPort, CD_EVENT);

    #ifdef SDMMC_DMA
    if (cmd & DATA_EXPECT)
    {
        pReg->SDMMC_BLKSIZ = BlkSize;
        pReg->SDMMC_BYTCNT = DataLen;  //这个寄存器的长度一定要设置为需要的长度，不用考虑SDMMC控制器的32bit对齐
        
        printf("sd timeout = %08x\n", pReg->SDMMC_STATUS);
        
        if((cmd & SD_OP_MASK) == SD_WRITE_OP)
        {
            ret = SDIO_Dma_WriteRequest(pDataBuf, DataLen);
        }
        else
        {
            ret = SDIO_Dma_ReadRequest(pDataBuf, DataLen);
        }

        SDIOC_Handle_Trans(SDCPort, DTO_EVENT);
    }    
    
    #else   

    if (cmd & DATA_EXPECT)
    {
		SDIOC_Handle_Trans(SDCPort, DTO_EVENT);
        value = pReg->SDMMC_RINTSTS;
        if((cmd & SD_OP_MASK) == SD_WRITE_OP)
        {
            if (value & DCRC_INT)
            {
                ret = SDC_DATA_CRC_ERROR;
            }
            else if (value & EBE_INT)
            {
                ret = SDC_END_BIT_ERROR;
            }
            else
            {
                ret = SDIOC_WaitBusy(SDCPort);

                if (ret != SDC_SUCCESS)
					goto cmddone;

                ret = SDC_SUCCESS;
            }
        }
        else
        {
            if(value & (SBE_INT | EBE_INT | DRTO_INT | DCRC_INT))
            {
                if (value & SBE_INT)
                {
                    ret = SDC_START_BIT_ERROR;
                }
                else if (value & EBE_INT)
                {
                    ret = SDC_END_BIT_ERROR;
                }
                else if (value & DRTO_INT)
                {
                    ret = SDC_DATA_READ_TIMEOUT;
                }
                else if (value & DCRC_INT) 
                {
                    ret = SDC_DATA_CRC_ERROR;
                }
            }
            else
            {
                ret = SDIOC_ReadRemain(SDCPort, DataLen, pDataBuf);
            }
        }
    }
    
    #endif

    value = pReg->SDMMC_RINTSTS;
    pReg->SDMMC_RINTSTS = (~SDIO_INT);//0xFFFFFFFF;
    if (ret == SDC_SUCCESS)
    {
        if (cmd & RSP_BUSY) //R1b
        {
            ret = SDIOC_WaitBusy(SDCPort);
            if (ret != SDC_SUCCESS)
				goto cmddone;
        }

        //if need, get response
        if ((cmd & R_EXPECT) && (RespBuf != NULL))
        {
            RespBuf[0] = pReg->SDMMC_RESP0;
            if (cmd & LONG_R)
            {                
                RespBuf[1] = pReg->SDMMC_RESP1;
                RespBuf[2] = pReg->SDMMC_RESP2;
                RespBuf[3] = pReg->SDMMC_RESP3;
            }
        }
    }
	
cmddone:
	IntEnable(INT_ID18_SDMMC);
	
	if (ret)
		printf("err SDIOC_Request = 0x%04x\n", ret);
	
    return ret;
}

#if 1
int32 SDIOC_SendCmd(SDMMC_PORT_E SDCPort, uint32 cmd, uint32  CmdArg, uint32 *RespBuf)
{
    int32 err = SDC_SUCCESS;
	int   sdioc_try = 2;

	while (sdioc_try)
	{
		err = SDIOC_Request(SDCPort, cmd, CmdArg, RespBuf, 0, 0, NULL);
	    if (SDC_SUCCESS == err)
	    {           
            break;
	    }        
        sdioc_try--;
	}
    return err;
}
#endif

#endif
