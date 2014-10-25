/*
********************************************************************************
*                   Copyright (c) 2009,chenfen
*                         All rights reserved.
*
* File Name：   USBCTRL.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             chenfen      2008-12-12          1.0
*    desc:    ORG.
********************************************************************************
*/

#include "USBConfig.h"

#define  IN_USBCTRL

#ifdef USB

DRAM_USB uint32 UsbBulkBuf[USB_BUF_SIZE/4];

/*
--------------------------------------------------------------------------------
  Function name : bool UsbPhyReset(void)
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
bool USBPhyReset(void)
{
    bool VbusOK=FALSE;
 	#if 0
    pSCUReg_t ScuReg = (pSCUReg_t)SCU_BASE;

    if (1)
    {
        ScuReg->SCU_SOFTRST_CON |= 1<<2;     //USB PHY reset
        //DelayUs(10*1000);    //delay 10ms
        Delay100cyc(150*10);
        ScuReg->SCU_SOFTRST_CON &= ~(1<<2);
        //DelayUs(1*1000);     //delay 1ms
        Delay100cyc(150);
        VbusOK=TRUE;
    }
	#endif
    return (VbusOK);
}


/*
--------------------------------------------------------------------------------
  Function name : void usb_driver_Init(void)
  Author        : chenfen
  Description   : USB初始化
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void UDCInit(bool FullSpeed)
{
    otg_core_if_t * otg_core = (otg_core_if_t *)USB_REG_BASE;
    int count = 0;

    /* Wait for AHB master IDLE state. */
    for (count=0; count<10000; count++)
    {
        if ((otg_core->core_regs.grstctl & (1ul<<31))!=0)
            break;
    }
   
        //core soft reset
    otg_core->core_regs.grstctl |= 1<<0;               //Core soft reset
    for (count=0; count<10000; count++)
    {
        if ((otg_core->core_regs.grstctl & (1<<0))==0)
            break;
    }

    otg_core->pcgcctl = 0x00;             /* Restart the Phy Clock */


    //Non-Periodic TxFIFO Empty Level interrupt indicates that the IN Endpoint TxFIFO is completely empty
    otg_core->core_regs.gahbcfg |= 1<<7;

    /* Do device intialization*/
    
    //usb_write_reg32(USB_PCGCCTL, 0);
  
    /* High speed PHY running at high speed */
    if (!FullSpeed)
    {
        otg_core->dev_regs.dcfg &= ~0x03; 
    }
    else
    {
        //#ifdef FORCE_FS
        otg_core->dev_regs.dcfg |= 0x01;   //Force FS
        //#endif
    }
    
    /* Reset Device Address */
    otg_core->dev_regs.dcfg &= ~0x07F0;
  
    /* Flush the FIFOs */
    otg_core->core_regs.grstctl |= ( 0x10<<6) | (1<<5);     //Flush all Txfifo
    for (count=0; count<10000; count++)
    {
        if ((otg_core->core_regs.grstctl & (1<<5))==0)
            break;
    }
  
    otg_core->core_regs.grstctl |= 1<<4;              //Flush all Rxfifo
    for (count=0; count<10000; count++)
    {
        if ((otg_core->core_regs.grstctl & (1<<4))==0)
            break;
    }

    /* Flush the Learning Queue. */
    otg_core->core_regs.grstctl |= 1<<3;

    /* Clear all pending Device Interrupts */
  
    otg_core->dev_regs.in_ep[0].diepctl = (1<<27)|(1<<30);        //IN0 SetNAK & endpoint disable
    otg_core->dev_regs.in_ep[0].dieptsiz = 0;
    //otg_core->dev_regs.in_ep[0].diepdma = 0;
    otg_core->dev_regs.in_ep[0].diepint = 0xFF;
  
    otg_core->dev_regs.out_ep[0].doepctl = (1<<27)|(1<<30);        //OUT0 SetNAK & endpoint disable
    otg_core->dev_regs.out_ep[0].doeptsiz = 0;
    //otg_core->dev_regs.out_ep[0].doepdma = 0;
    otg_core->dev_regs.out_ep[0].doepint = 0xFF;

    
    otg_core->dev_regs.diepmsk = 0x2F;              //device IN interrutp mask
    otg_core->dev_regs.doepmsk = 0x0F;              //device OUT interrutp mask
    otg_core->dev_regs.daint = 0xFFFFFFFF;          //clear all pending intrrupt
    otg_core->dev_regs.daintmsk = 0x00010001;         //device all ep interrtup mask(IN0 & OUT0)
    otg_core->core_regs.gintsts = 0xFFFFFFFF;
    otg_core->core_regs.gotgint = 0xFFFFFFFF;
  
    otg_core->core_regs.gintmsk=(1<<4)|/*(1<<5)|*/(1<<10)|(1<<11)|(1<<12)|(1<<13)|(1<<18)|(1<<19)|(1ul<<30)|(1ul<<31);
    otg_core->core_regs.gahbcfg |= 0x01;        //Global interrupt mask

}

/*
--------------------------------------------------------------------------------
  Function name : void usb_driver_Init(void)
  Author        : chenfen
  Description   : USB初始化
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void USBConnect(bool FullSpeed)
{
    
	otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    #if 0
    pSCUReg_t ScuReg = (pSCUReg_t)SCU_BASE;

    ScuReg->SCU_OTGPHY_CON |= (1<<5);     //enable software ctrl phy
    ScuReg->SCU_OTGPHY_CON |= (1<<6);     //don't suspend
    ScuReg->SCU_OTGPHY_CON &= ~(1<<5);    //disable software ctrl phy
    #endif

//    UsbStatus = USB_XFER_IDLE;
    
	G_ucBulkPhase = K_CommandPhase;
	UsbConnected = 0; 	//USB枚举结束标志
//	UsbResetCnt = 0;		//通信过程中总线复位计数
    bEPPflags.bits.control_state = USB_IDLE;

	DISABLE_USB_INT;

//    if (FullSpeed)
//    {
//        Delay100cyc(100000);
//    }

    dev_regs->dctl |= 0x02;       //soft disconnect
    USBPhyReset();
    UDCInit(FullSpeed);

    ENABLE_USB_INT;
    dev_regs->dctl &= ~0x02;        //soft connect
    
}

/*
--------------------------------------------------------------------------------
  Function name : MscReQAdd
  Author        : chenfen
  Description   : 
           
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
int32  MscReqAdd(uint8 cmd, uint32 lun, uint32 LBA, uint16 len, void *buf)
{
    pUSB_MSC_REQ req = &UsbMscReq;

    if (req->cmd)
        return -1;

    req->LUN = lun;
    req->LBA = LBA;
    req->len = len;
    req->buf = buf;
    req->cmd = cmd;

    return 0;
}

IRAM_USB
USB_MSC_REQ* MscReqNext(void)
{
    pUSB_MSC_REQ req = &UsbMscReq;

    if (req->cmd)
        return req;
    else
        return NULL;
}

IRAM_USB
void MscChkPendReq(void)
{
    pUSB_MSC_REQ req = &UsbMscPendReq;

    if (req->cmd == K_SCSICMD_WRITE_10)
    {
        MDWrite(req->LUN, req->LBA, req->len, req->buf); 
    }
    req->cmd = 0;
}

//uint32 ProgCount;
/*
--------------------------------------------------------------------------------
  Function name : void USBInit(void)
  Author        : chenfen
  Description   : USB初始化
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void USBInit(bool ReConnecte)
{
	uint32 i;
    volatile uint32 timer1 = 0;
    volatile uint32 timer2 = 0;

	if (CheckVbus() == 0)
    {   
		return;
    }

    UsbUpdate  = 0;

    memset(&UsbBulkXfer, 0, sizeof(USB_BULK_XFER));
    UsbBulkXfer.buf = (uint8*)UsbBulkBuf;

    memset(&UsbMscReq, 0, sizeof(USB_MSC_REQ));
    memset(&UsbMscPendReq, 0, sizeof(USB_MSC_REQ));

    if (!ReConnecte)
    {
        goto exit;
    }
    
    USBConnect(0);   //先用HING SPEED 连接

    #ifdef USB_IF_TEST
    while (CheckVbus() != 0)    //USB论证测试只要插着USB线就不退出
    #else
	for (i=0; i<20000; i++)//2008-2-28 延长坚持USB1.1的时间，USB连接会可靠一点
    #endif
    {    
		if (UsbConnected != 0)
		{
			break;
		}
		Delay100cyc(750);
	}

exit:
	while (CheckVbus() != 0)
	{
        USB_MSC_REQ *req; 

        #ifndef USB_IF_TEST //USB论证测试只要插着USB线就不退出
        if (UsbConnected == 0)
            break;    
        #endif
        
        req = MscReqNext();             //获取下一个请求
        if (req)
        {
            pUSB_BULK_XFER Xfer = &UsbBulkXfer;
            timer1 = 0;      
            UsbBusyHook();
            
            if (req->cmd == K_SCSICMD_WRITE_10)
            {

                #if (NAND_DRIVER == 1)
                if (MD_MAJOR(req->LUN)==MEM_DEV_NAND)
                {
                    uint16 offset, PageSize = FtlGetPageSize();
                    offset = req->LBA & (PageSize-1);
                    //不足一个Page的数据延迟写, 若下次写的是同一个Page, 速度就可以优化 
                    if (Xfer->TransLen >= Xfer->NendLen && !offset && req->len<PageSize)
                    {
                        memcpy((void*)&UsbMscPendReq, (void*)req, sizeof(USB_MSC_REQ));
                        req->cmd = 0;     
                    }
                }
                #endif

                if (req->cmd)
                {
                    MDWrite(req->LUN, req->LBA, req->len, req->buf);
                    req->cmd = 0;
                }
                ENABLE_USB_INT;

                UsbUpdate  = 1;//media lib update
                if (Xfer->TransLen >= Xfer->NendLen)
                {
                    SendCSW();
                }
            }
            else if (req->cmd == K_SCSICMD_READ_10)
            {
                MscChkPendReq();
                MDRead(req->LUN, req->LBA, req->len, req->buf);
                //ENABLE_USB_INT;
                Xfer->XferLen = 0;
                Xfer->BufLen = req->len<<9;
                req->cmd = 0;
                BulkInPacket();
            }
            else if (req->cmd == K_RKCMD_CHECK_REQUEST)
            {
                MscChkPendReq();
                req->cmd = 0;
                ENABLE_USB_INT;
            }
            else if (req->cmd==K_SCSICMD_PREVENT_ALLOW_MEDIUM_REMOVAL)
            {
                //UsbReadyHook();
                req->cmd = 0;
            }
        }
        else
        {
            timer1++;
            if(timer1 >= 2000000)//200ms
            {
                timer2++;
                timer1 = 0;
                DISABLE_USB_INT;
                MscChkPendReq();
                ENABLE_USB_INT;
            }

            if (timer2 >= 10)
            {
                timer2 = 0;
                DISABLE_USB_INT;
                MDScanDev(1);
                ENABLE_USB_INT;
            }
            
            UsbReadyHook();
		    Delay10cyc(1); //100ns   
            //UsbServiceHook();
        }
	}

    MscChkPendReq();

    #if (NAND_DRIVER == 1)
    FtlWrBlkFree();
    #endif

}

/*
--------------------------------------------------------------------------------
  Function name : code_transmit
  Author        : chenfen
  Description   : 发送端点号1建立代码
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void code_transmit(uint8 * pRomData, uint16 len)
{

	ControlData.pData = pRomData;

    if( len > EP0_PACKET_SIZE)
	{
        len = EP0_PACKET_SIZE;        
		bEPPflags.bits.control_state = USB_TRANSMIT;
	}
	else 
	{
		bEPPflags.bits.control_state = USB_IDLE;
	}
    
    
    UsbWriteEp0(len, ControlData.pData);
    ControlData.wLength -= len;
    ControlData.pData += len;
}

/*
--------------------------------------------------------------------------------
  Function name : void control_handler(void)
  Author        : chenfen
  Description   : 请求处理子程序
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void control_handler(void)
{
	uint8 type, req;

	type = ControlData.DeviceRequest.bmRequestType & USB_REQUEST_TYPE_MASK;
	req = ControlData.DeviceRequest.bRequest & USB_REQUEST_MASK;

	if (type == USB_STANDARD_REQUEST)
	{
		(*StandardDeviceRequest[req])();					//调用标准请求
    }
    else if (type == USB_VENDOR_REQUEST)
    {
		;/*(*VendorDeviceRequest[req])();*/					//调用厂商请求
    }
    else if (type == USB_CLASS_REQUEST) 				//类型请求
	{
		if (ControlData.DeviceRequest.bRequest == 0xfe) 	//0xfe:GET MAX LUN
		{
			UsbConnectedHook();
			type = USBDisk.LunNum-1;
            UsbWriteEp0(1, &type);
            bEPPflags.bits.control_state = USB_IDLE;
		}
        else if (ControlData.DeviceRequest.bRequest == 0xff)
        {
            if (0==ControlData.DeviceRequest.wLength)
            {
                //ResetInEpDataToggle(BULK_IN_EP);
                //UsbSetEpSts(1, BULK_IN_EP, 0);
                G_ucBulkPhase = K_CommandPhase;
                ep0in_ack();
            }
            else
            {
                bEPPflags.bits.control_state = USB_RECEIVE;
                ControlData.wLength = ControlData.DeviceRequest.wLength;
                ControlData.pData = EpBuf;
            }
        }
	}
	else
	{
		stall_ep0();
	}
}
#endif


