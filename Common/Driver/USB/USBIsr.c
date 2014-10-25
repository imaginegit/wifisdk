/*
********************************************************************************
*                   Copyright (c) 2009,chenfen
*                         All rights reserved.
*
* File Name：   isr.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             chenfen      2008-12-12          1.0
*    desc:    ORG.
********************************************************************************
*/

#define	IN_USB_ISR

#include "USBConfig.h"

#ifdef USB


/*
--------------------------------------------------------------------------------
  Function name : usb_handle_intr
  Author        : chenfen
  Description   : 
                  the device interrupts.  Many conditions can cause a
   * device interrupt. When an interrupt occurs, the device interrupt
   * service routine determines the cause of the interrupt and
   * dispatches handling to the appropriate function. These interrupt
   * handling functions are described below.
     *
   * All interrupt registers are processed from LSB to MSB.
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/012/10         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void usb_handle_intr(void)
{
    uint32 intr_status;
    otg_core_if_t * otg_core = (otg_core_if_t *)USB_REG_BASE;
  
    intr_status = otg_core->core_regs.gintsts & otg_core->core_regs.gintmsk;

    if (!intr_status) 
    {
        return ;
    }

    if (intr_status & (1<<4))  //RxFIFO Non-Empty
    {
        usb_rx_level_intr( );
    }
    if (intr_status & (1<<5)) //Non-periodic TxFIFO Empty
    {
        otg_core->core_regs.gintmsk &= ~(1<<5);
        otg_core->dev_regs.dtknqr4_fifoemptymsk = 0;
    }
    if(intr_status & (1<<10))    //Early Suspend
    {
        otg_core->core_regs.gintsts = 1<<10;
    }
    if(intr_status & (1<<11))    //USB Suspend
    {
        otg_core->core_regs.gintsts = 1<<11;

        if (UsbConnected)
		{
			UsbConnected=0;
			//bEPPflags.bits.suspend = 1;
		}
    }
    if(intr_status & (1<<12))  //USB Reset
    {
        usb_reset_intr( );
        otg_core->core_regs.gintsts = 1<<12;
    }
    if(intr_status & (1<<13))  //Enumeration Done
    {
        usb_enum_done_intr( );
        otg_core->core_regs.gintsts = 1<<13;
    }
    if(intr_status & (1<<18))       //IN中断
    {
        usb_in_ep_intr( );
    }
    if(intr_status & (1<<19))       //OUT中断
    {
        usb_out_ep_intr( );
    }
    if(intr_status & (1<<30))  //USB VBUS中断  this interrupt is asserted when the utmiotg_bvalid signal goes high.
    {
        //VbusFlag=GetVbus();
        otg_core->core_regs.gintsts = 1<<30;
    }
    if(intr_status & (1ul<<31))    //resume
    {
        otg_core->core_regs.gintsts = 1ul<<31;
    }
    if(intr_status & ((1<<22)|(1<<6)|(1<<7)|(1<<17)))
    {
        otg_core->core_regs.gintsts = intr_status & ((1<<22)|(1<<6)|(1<<7)|(1<<17));
    }
  
}

/*
--------------------------------------------------------------------------------
  Function name : usb_in_ep_intr
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void usb_in_ep_intr(void)
{
    uint32 ep_intr;
    uint32 diepint;
    uint32 msk;
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    /* Read in the device interrupt bits */
    ep_intr = (dev_regs->daint & dev_regs->daintmsk) & 0xFFFF;
    if ( ep_intr & 0x01)
    {
        ep_intr = 0;     //端点0
    }
    else if ( ep_intr & 0x02)
    {
        ep_intr = 1;       //端点1
    }
    else
    {
        return;
    }

    /* Service the Device IN interrupts for each endpoint */

    msk = dev_regs->diepmsk | ((dev_regs->dtknqr4_fifoemptymsk & 0x01)<<7);   //<<7是因为msk是保留?
    diepint = dev_regs->in_ep[ep_intr].diepint & msk;
                       
    /* Transfer complete */
    if ( diepint & 0x01 ) 
    {
        /* Disable the NP Tx FIFO Empty Interrrupt */
        dev_regs->dtknqr4_fifoemptymsk = 0;

        /* Clear the bit in DIEPINTn for this interrupt */
        dev_regs->in_ep[ep_intr].diepint = 0x01;

        if (ep_intr != 0)
        {
            BulkInPacket();
        }
        /* Complete the transfer */
        if (ep_intr == 0)
        {
            if (bEPPflags.bits.control_state == USB_TRANSMIT)
            {
                code_transmit(ControlData.pData, ControlData.wLength);
            }        
        }

    }
    /* Endpoint disable  */
    if (  diepint & 0x02 ) 
    {
        /* Clear the bit in DIEPINTn for this interrupt */
        dev_regs->in_ep[ep_intr].diepint = 0x02;

    }
    /* AHB Error */
    if ( diepint & 0x04 ) 
    {
        /* Clear the bit in DIEPINTn for this interrupt */
        dev_regs->in_ep[ep_intr].diepint = 0x04;
    }
    /* TimeOUT Handshake (non-ISOC IN EPs) */
    if ( diepint & 0x08 ) 
    { 
        dev_regs->in_ep[ep_intr].diepint = 0x08;
    }
    /** IN Token received with TxF Empty */
    if ( diepint & 0x20 ) 
    {
        dev_regs->in_ep[ep_intr].diepint = 0x20;
    }           
    /** IN EP Tx FIFO Empty Intr */
    if ( diepint & 0x80 )
    {                   
        dev_regs->in_ep[ep_intr].diepint = 0x80;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : usb_out_ep_intr
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void usb_out_ep_intr(void)
{
    uint32 i;
    uint32 ep_intr;
    uint32 doepint;
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    /* Read in the device interrupt bits */
    ep_intr = (dev_regs->daint & dev_regs->daintmsk) >> 16;
    if ( !ep_intr)
    {
        return; 
    }

    for (i=0; i<3; i++)
    {
        if ((1<<i) & ep_intr)
        {
            doepint = dev_regs->out_ep[i].doepint & dev_regs->doepmsk;  

            /* Transfer complete */
            if ( doepint & 0x01 ) 
            {
                /* Clear the bit in DOEPINTn for this interrupt */
                dev_regs->out_ep[i].doepint = 0x01;

                //usb_handle_ep0();
                if (i != 0)
                {
                    dev_regs->out_ep[BULK_OUT_EP].doeptsiz =(1ul<<19) | BulkEndpOutSize; //BulkEndpSize Active ep, Clr Nak, endpoint enable
                    dev_regs->out_ep[BULK_OUT_EP].doepctl |= (1ul<<26) | (1ul<<31); //Active ep, Clr Nak, endpoint enable
                }
            }
            /* Endpoint disable  */
            if ( doepint & 0x02 ) 
            {
                /* Clear the bit in DOEPINTn for this interrupt */
                dev_regs->out_ep[i].doepint = 0x02;
            }
            /* AHB Error */
            if ( doepint & 0x04 ) 
            {
                dev_regs->out_ep[i].doepint = 0x04;
            }
            /* Setup Phase Done (contorl EPs) */
            if ( doepint & 0x08 ) 
            {
                dev_regs->out_ep[i].doepint = 0x08;

                //usb_handle_ep0( );
            }
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : usb_enum_done_intr
  Author        : chenfen
  Description   : 
          * Read the device status register and set the device speed in the
          * data structure. 
          * Set up EP0 to receive SETUP packets by calling dwc_ep0_activate.                
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/12/10         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void usb_enum_done_intr(void)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;      
  
    /* Set the MPS of the IN EP based on the enumeration speed */
    bEPPflags.bits.HS_FS_State = FULL_SPEED;

    BulkEndpInSize = FS_BULK_TX_SIZE;
    BulkEndpOutSize = FS_BULK_RX_SIZE;
    switch ((dev_regs->dsts >> 1) & 0x03) 
    {
        case 0: //HS_PHY_30MHZ_OR_60MHZ:
            BulkEndpInSize = HS_BULK_TX_SIZE;
            BulkEndpOutSize = HS_BULK_RX_SIZE;
            bEPPflags.bits.HS_FS_State = HIGH_SPEED;
        case 1: //FS_PHY_30MHZ_OR_60MHZ:
        case 3: //FS_PHY_48MHZ:
            dev_regs->in_ep[0].diepctl &= ~0x03;  //64bytes MPS
            Ep0PktSize = EP0_PACKET_SIZE_HS;
            break;
        case 2: //LS_PHY_6MHZ
        default:
            dev_regs->in_ep[0].diepctl |= 0x03;   //8bytes MPS
            Ep0PktSize = EP0_PACKET_SIZE_FS;
            break;
    }

    /* setup EP0 to receive SETUP packets */
    dev_regs->out_ep[0].doeptsiz = (1ul << 29) | (1ul << 19) | 0x40; //supcnt = 1, pktcnt = 1, xfersize = 64*1
    /* Enable OUT EP for receive */
    dev_regs->out_ep[0].doepctl |= 1ul<<31;

    dev_regs->daintmsk |= (1<<BULK_IN_EP) | ((1<<BULK_OUT_EP)<<16);
    dev_regs->out_ep[BULK_OUT_EP].doeptsiz = /*(1ul << 29)|*/(1ul << 19) | BulkEndpOutSize; //BulkEndpSize
    dev_regs->out_ep[BULK_OUT_EP].doepctl = (1ul<<31)|(1<<28)|(1<<26)/*|(2<<22)*/|(2<<18)|(1<<15)|BulkEndpOutSize;//BulkEndpSize
    dev_regs->out_ep[BULK_OUT_EP].doepint = 0xFF;

    dev_regs->in_ep[BULK_IN_EP].diepctl = (1<<28)|(2<<18)|(1<<15);
//    dev_regs->in_ep[BULK_IN_EP].diepctl = (1<<15)|(2<<18)/*|(ep<<22)*/|(1ul<<26)|(1ul<<31)|BulkEndpSize;

    dev_regs->dctl |= 1<<8;         //clear global IN NAK
}

/*
--------------------------------------------------------------------------------
  Function name : usb_rx_level_intr
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void usb_rx_level_intr(void  )
{
    otg_core_if_t * otg_core = (otg_core_if_t *)USB_REG_BASE;
    uint32 rxsts;

    /* Disable the Rx Status Queue Level interrupt */
    otg_core->core_regs.gintmsk &= ~(1<<4);
  
    /* Get the Status from the top of the FIFO */
    rxsts = otg_core->core_regs.grxstsp;

    if ((rxsts & 0x0F) == 0) //EP0
    { 
        switch ((rxsts >> 17) & 0x0F) 
        {
            case 0x01: //Global OUT NAK   
                break;
            case 0x02: //OUT data packet received   ;
            //Ep0OutPacket((rxsts >> 4) & 0x7FF);
                break;
            case 0x03: //OUT transfer completed
                otg_core->dev_regs.out_ep[0].doepctl = (1ul<<15) | (1ul<<26) | (1ul<<31);   //Active ep, Clr Nak, endpoint enable
                break;
            case 0x04: //SETUP transaction completed
                otg_core->dev_regs.out_ep[0].doepctl = (1ul<<15) | (1ul<<26) | (1ul<<31);   //Active ep, Clr Nak, endpoint enable
                break;
            case 0x06: //SETUP data packet received
                if (((rxsts >> 4) & 0x7ff) > 0)
                    usb_Setup0((rxsts >> 4) & 0x7ff);
                break;
            default:
                break;
        }
    }
    else if ((rxsts & 0x0F)==BULK_OUT_EP) //EP2
    {
        switch ((rxsts >> 17) & 0x0F)
        {
            case 0x02:      //OUT data packet received
                BulkOutPacket((rxsts>>4) & 0x7ff);
                break;
            case 0x03:      //OUT transfer completed
                BulkOutHandle();
                break;
            case 0x01:      //Global OUT NAK
            case 0x04:      //SETUP transaction completed
            case 0x06:      //SETUP data packet received
            default:
                break;
        }
    }
  
    /* Enable the Rx Status Queue Level interrupt */
    otg_core->core_regs.gintmsk |= (1<<4);

    /* Clear interrupt */
    otg_core->core_regs.gintsts = (1<<4);

}

/*
--------------------------------------------------------------------------------
  Function name : usb_reset_intr
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/12/10         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void usb_reset_intr(void)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;        

    bEPPflags.value = 0;
    G_ucBulkPhase=K_CommandPhase;
    dev_regs->dcfg &= ~0x07f0;    
    /* Clear the Remote Wakeup Signalling */
    dev_regs->dctl &= ~0x01;
  
  /* Clear interrupt */
}

/*
--------------------------------------------------------------------------------
  Function name : usb_Setup0
  Author        : chenfen
  Description   : 
                  * This function handles EP0 Control transfers.  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void usb_Setup0(uint16 len)
{

    if (USB_RECEIVE == bEPPflags.bits.control_state) 
    {
        UsbReadEp0(len, ControlData.pData);
        ControlData.wLength -= len;
        ControlData.pData += len;
        if (ControlData.wLength==0)
        {
            bEPPflags.bits.control_state = USB_IDLE;
            ep0in_ack();
        }
    }
    else 
    {
        if (len == 8)
        {
            UsbReadEp0(len, &ControlData.DeviceRequest);
            control_handler( );                      //调用请求处理子程序
        }
    }
}

#else
/*
IRAM_USB
void usb_handle_intr(void)
{
}
*/
#endif

