/*
********************************************************************************
*                   Copyright (c) 2009,chenfen
*                         All rights reserved.
*
* File Name£∫   UsbAdapterProbe.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             chenfen      2008-12-12          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_USB_ADAPTER

#include "SysInclude.h"
#include "FsInclude.h"

_ATTR_SYS_BSS_ UINT8  UsbAdapterProbeConnectFlag;
_ATTR_SYS_BSS_ UINT8  UsbAdapterProbeStartFlag;
_ATTR_SYS_BSS_ UINT32 UsbAdapterProbeSystickCounter;
_ATTR_SYS_BSS_ UINT32 UsbAdapterProbeLdoBak;

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
_ATTR_SYS_CODE_
void UsbAdpterProbeISR(void)
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
    }

    if (intr_status & (1<<5)) //Non-periodic TxFIFO Empty
    {
    }
    if(intr_status & (1<<10))    //Early Suspend
    {
        otg_core->core_regs.gintsts = 1<<10;
    }
    if(intr_status & (1<<11))    //USB Suspend
    {
        otg_core->core_regs.gintsts = 1<<11;
    }
    if(intr_status & (1<<12))  //USB Reset
    {
        otg_core->dev_regs.dcfg &= ~0x07f0;    
        otg_core->dev_regs.dctl &= ~0x01;
        otg_core->core_regs.gintsts = 1<<12;
    }
    if(intr_status & (1<<13))  //Enumeration Done
    {
        otg_core->core_regs.gintsts = 1<<13;
        DISABLE_USB_INT;
        UsbAdapterProbeConnectFlag = 1;
    }

    if(intr_status & (1<<30))  //USB VBUS÷–∂œ  this interrupt is asserted when the utmiotg_bvalid signal goes high.
    {
        otg_core->core_regs.gintsts = 1<<30;
    }

    if(intr_status & ((1<<22)|(1<<6)|(1<<7)|(1<<17)))
    {
        otg_core->core_regs.gintsts = intr_status & ((1<<22)|(1<<6)|(1<<7)|(1<<17));
    }
}

_ATTR_SYS_CODE_
void UsbAdpterProbeStart(void)
{
    otg_core_if_t * otg_core = (otg_core_if_t *)USB_REG_BASE;
    int count = 0;

    if (UsbAdapterProbeStartFlag == 0)
    {
        //variable init
        UsbAdapterProbeConnectFlag = 0;
        UsbAdapterProbeStartFlag   = 1;

        //CPU init
        {
            UsbAdapterProbeLdoBak = ScuLDOSet(SCU_LDO_33);
            PMU_EnterModule(PMU_USB);  

            ScuClockGateCtr(CLOCK_GATE_USBPHY, 1);
            ScuClockGateCtr(CLOCK_GATE_USBBUS, 1);

            //interrupt init
            IntRegister(INT_ID23_USB, (void*)UsbAdpterProbeISR);

            Scu_otgphy_suspend(0);
        }

        //USB configer and start probe
        {
            otg_core->dev_regs.dctl |= 0x02;       //soft disconnect
         
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
         
            /* High speed PHY running at high speed */
             otg_core->dev_regs.dcfg &= ~0x03; 
         
            /* Reset Device Address */
            otg_core->dev_regs.dcfg &= ~0x07F0;
         
            otg_core->dev_regs.daint = 0xFFFFFFFF;          //clear all pending intrrupt

            otg_core->core_regs.gintsts = 0xFFFFFFFF;
            otg_core->core_regs.gotgint = 0xFFFFFFFF;
         
            otg_core->core_regs.gintmsk=(1<<4)|/*(1<<5)|*/(1<<10)|(1<<11)|(1<<12)|(1<<13)|(1<<18)|(1<<19)|(1ul<<30)|(1ul<<31);
            otg_core->core_regs.gahbcfg |= 0x01;        //Global interrupt mask
        }
        
        ENABLE_USB_INT;
        otg_core->dev_regs.dctl &= ~0x02;                   //soft connect

        UsbAdapterProbeSystickCounter = GetSysTick();        //start timer
    }
}

_ATTR_SYS_CODE_
void UsbAdpterProbeStop(void)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    if (UsbAdapterProbeStartFlag == 1)
    {
        //Variable deinit
        UsbAdapterProbeStartFlag = 0;

        //Usb Stop
        dev_regs->dctl |= 0x02;       //soft disconnect
        DISABLE_USB_INT;

        //CPU DeInit
        {
            Scu_otgphy_suspend(1);

            IntUnregister(INT_ID23_USB); 
        
            ScuClockGateCtr(CLOCK_GATE_USBPHY, 0);
            ScuClockGateCtr(CLOCK_GATE_USBBUS, 0);
            
            PMU_ExitModule(PMU_USB);
            ScuLDOSet(UsbAdapterProbeLdoBak);
        }
    }
}

_ATTR_SYS_CODE_
int32 UsbAdpterProbe(void)
{
    uint32 SystickTmp;
        
    if (UsbAdapterProbeStartFlag == 1)
    {
        if (UsbAdapterProbeConnectFlag)
        {   
            UsbAdpterProbeStop();
            return 1;
        }
        else
        {
            //probe timeout, it means charge
            SystickTmp = GetSysTick();
            if ((SystickTmp - UsbAdapterProbeSystickCounter) > 100)    //1000ms—” ±
            {
                UsbAdpterProbeStop();
                return 2;
            }
        }
    }
    return 0;
}

/*
********************************************************************************
*
*                         End of UsbAdapterProbe.c
*
********************************************************************************
*/

