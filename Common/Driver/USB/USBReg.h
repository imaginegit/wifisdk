/*
********************************************************************************
*                   Copyright (c) 2008,CHENFEN
*                         All rights reserved.
*
* File Name£º   Creg.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               CHENFEN      2008-12-04          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _CREG_H_
#define _CREG_H_

/*
--------------------------------------------------------------------------------
  
                        Macro define  
  
--------------------------------------------------------------------------------
*/

#define       USB_REG_BASE            0x60030000

#define       USB_GAHBCFG        (USB_REG_BASE+0x008)
#define       USB_GRSTCTL        (USB_REG_BASE+0x010)
#define       USB_GINTSTS            (USB_REG_BASE+0x014)
#define       USB_GINTMSK        (USB_REG_BASE+0x018)
#define       USB_GRXSTSP        (USB_REG_BASE+0x020)

#define       USB_DEV_BASE         (USB_REG_BASE+0x800)

#define       USB_DCFG           (USB_REG_BASE+0x800)
#define       USB_DCTL           (USB_REG_BASE+0x804)
#define       USB_DSTS           (USB_REG_BASE+0x808)
#define       USB_DIEPMSK        (USB_REG_BASE+0x810)
#define       USB_DOEPMSK        (USB_REG_BASE+0x814)

#define       USB_DAINT          (USB_REG_BASE+0x818)
#define       USB_DAINTMSK         (USB_REG_BASE+0x81C)
#define       USB_DIEPEMPMSK       (USB_REG_BASE+0x834)

#define       USB_DIEPCTL0         (USB_REG_BASE+0x900)
#define       USB_DIEPINT0         (USB_REG_BASE+0x908)
#define       USB_DIEPTSIZ0      (USB_REG_BASE+0x910)
#define       USB_DIEPDMA0         (USB_REG_BASE+0x914)

#define       USB_DOEPCTL0         (USB_REG_BASE+0xB00)
#define       USB_DOEPINT0         (USB_REG_BASE+0xB08)
#define       USB_DOEPTSIZ0      (USB_REG_BASE+0xB10)
#define       USB_DOEPDMA0         (USB_REG_BASE+0xB14)

#define       USB_PCGCCTL        (USB_REG_BASE+0xE00)

#define       USB_EP_FIFO(ep)    (USB_REG_BASE +((ep+1)<<12))

/*
--------------------------------------------------------------------------------
  
                        Struct Define
  
--------------------------------------------------------------------------------
*/

/** DWC_otg Core registers .  
 * The dwc_otg_core_global_regs structure defines the size
 * and relative field offsets for the Core Global registers.
 */
typedef struct otg_core_regs 
{
  /** OTG Control and Status Register.  <i>Offset: 000h</i> */
  volatile uint32 gotgctl; 
  /** OTG Interrupt Register.  <i>Offset: 004h</i> */
  volatile uint32 gotgint; 
  /**Core AHB Configuration Register.  <i>Offset: 008h</i> */
  volatile uint32 gahbcfg; 

  /**Core USB Configuration Register.  <i>Offset: 00Ch</i> */
  volatile uint32 gusbcfg; 
  /**Core Reset Register.  <i>Offset: 010h</i> */
  volatile uint32 grstctl; 
  /**Core Interrupt Register.  <i>Offset: 014h</i> */
  volatile uint32 gintsts; 
  /**Core Interrupt Mask Register.  <i>Offset: 018h</i> */
  volatile uint32 gintmsk; 
  /**Receive Status Queue Read Register (Read Only).  <i>Offset: 01Ch</i> */
  volatile uint32 grxstsr; 
  /**Receive Status Queue Read & POP Register (Read Only).  <i>Offset: 020h</i>*/
  volatile uint32 grxstsp; 
  /**Receive FIFO Size Register.  <i>Offset: 024h</i> */
  volatile uint32 grxfsiz; 
  /**Non Periodic Transmit FIFO Size Register.  <i>Offset: 028h</i> */
  volatile uint32 gnptxfsiz; 
  /**Non Periodic Transmit FIFO/Queue Status Register (Read
   * Only). <i>Offset: 02Ch</i> */
  volatile uint32 gnptxsts; 
  /**I2C Access Register.  <i>Offset: 030h</i> */
  volatile uint32 gi2cctl; 
  /**PHY Vendor Control Register.  <i>Offset: 034h</i> */
  volatile uint32 gpvndctl;
  /**General Purpose Input/Output Register.  <i>Offset: 038h</i> */
  volatile uint32 ggpio; 
  /**User ID Register.  <i>Offset: 03Ch</i> */
  volatile uint32 guid; 
  /**Synopsys ID Register (Read Only).  <i>Offset: 040h</i> */
  volatile uint32 gsnpsid;
  /**User HW Config1 Register (Read Only).  <i>Offset: 044h</i> */
  volatile uint32 ghwcfg1; 
  /**User HW Config2 Register (Read Only).  <i>Offset: 048h</i> */
  volatile uint32 ghwcfg2;

  /**User HW Config3 Register (Read Only).  <i>Offset: 04Ch</i> */
  volatile uint32 ghwcfg3;
  /**User HW Config4 Register (Read Only).  <i>Offset: 050h</i>*/
  volatile uint32 ghwcfg4;
  /** Reserved  <i>Offset: 054h-0FFh</i> */
  uint32 reserved[43];
  /** Host Periodic Transmit FIFO Size Register. <i>Offset: 100h</i> */
  volatile uint32 hptxfsiz;
  /** Device Periodic Transmit FIFO#n Register if dedicated fifos are disabled, 
    otherwise Device Transmit FIFO#n Register. 
   * <i>Offset: 104h + (FIFO_Number-1)*04h, 1 <= FIFO Number <= 15 (1<=n<=15).</i> */
  volatile uint32 dptxfsiz_dieptxf[15];

  uint32 reserved2[(0x400-0x140)/4];
} otg_core_regs_t;

/**
 * Device Logical IN Endpoint-Specific Registers. <i>Offsets
 * 900h-AFCh</i>
 *
 * There will be one set of endpoint registers per logical endpoint
 * implemented.
 *
 * <i>These registers are visible only in Device mode and must not be
 * accessed in Host mode, as the results are unknown.</i>
 */
typedef struct otg_dev_in_ep_regs 
{
  /** Device IN Endpoint Control Register. <i>Offset:900h +
   * (ep_num * 20h) + 00h</i> */
  volatile uint32 diepctl;
  /** Reserved. <i>Offset:900h + (ep_num * 20h) + 04h</i> */
  uint32 reserved04;  
  /** Device IN Endpoint Interrupt Register. <i>Offset:900h +
   * (ep_num * 20h) + 08h</i> */
  volatile uint32 diepint; 
  /** Reserved. <i>Offset:900h + (ep_num * 20h) + 0Ch</i> */
  uint32 reserved0C;  
  /** Device IN Endpoint Transfer Size
   * Register. <i>Offset:900h + (ep_num * 20h) + 10h</i> */
  volatile uint32 dieptsiz; 
  /** Device IN Endpoint DMA Address Register. <i>Offset:900h +
   * (ep_num * 20h) + 14h</i> */
  volatile uint32 diepdma; 
  /** Device IN Endpoint Transmit FIFO Status Register. <i>Offset:900h +
   * (ep_num * 20h) + 18h</i> */
  volatile uint32 dtxfsts;
  /** Reserved. <i>Offset:900h + (ep_num * 20h) + 1Ch - 900h +
   * (ep_num * 20h) + 1Ch</i>*/
  uint32 reserved18; 
} otg_dev_in_ep_regs_t;

/**
 * Device Logical OUT Endpoint-Specific Registers. <i>Offsets:
 * B00h-CFCh</i>
 *
 * There will be one set of endpoint registers per logical endpoint
 * implemented.
 *
 * <i>These registers are visible only in Device mode and must not be
 * accessed in Host mode, as the results are unknown.</i>
 */
typedef struct otg_dev_out_ep_regs 
{
  /** Device OUT Endpoint Control Register. <i>Offset:B00h +
   * (ep_num * 20h) + 00h</i> */
  volatile uint32 doepctl; 
  /** Device OUT Endpoint Frame number Register.  <i>Offset:
   * B00h + (ep_num * 20h) + 04h</i> */ 
  volatile uint32 doepfn; 
  /** Device OUT Endpoint Interrupt Register. <i>Offset:B00h +
   * (ep_num * 20h) + 08h</i> */
  volatile uint32 doepint; 
  /** Reserved. <i>Offset:B00h + (ep_num * 20h) + 0Ch</i> */
  uint32 reserved0C;  
  /** Device OUT Endpoint Transfer Size Register. <i>Offset:
   * B00h + (ep_num * 20h) + 10h</i> */
  volatile uint32 doeptsiz; 
  /** Device OUT Endpoint DMA Address Register. <i>Offset:B00h
   * + (ep_num * 20h) + 14h</i> */
  volatile uint32 doepdma; 
  /** Reserved. <i>Offset:B00h + (ep_num * 20h) + 18h - B00h +
   * (ep_num * 20h) + 1Ch</i> */
  uint32 unused[2];   
} otg_dev_out_ep_regs_t;

////////////////////////////////////////////
// Device Registers
/**
 * Device Global Registers. <i>Offsets 800h-BFFh</i>
 *
 * The following structures define the size and relative field offsets
 * for the Device Mode Registers.
 *
 * <i>These registers are visible only in Device mode and must not be
 * accessed in Host mode, as the results are unknown.</i>
 */
typedef struct otg_dev_regs 
{
  /** Device Configuration Register. <i>Offset 800h</i> */
  volatile uint32 dcfg; 
  /** Device Control Register. <i>Offset: 804h</i> */
  volatile uint32 dctl; 
  /** Device Status Register (Read Only). <i>Offset: 808h</i> */
  volatile uint32 dsts; 
  /** Reserved. <i>Offset: 80Ch</i> */
  uint32 unused;    
  /** Device IN Endpoint Common Interrupt Mask
   * Register. <i>Offset: 810h</i> */
  volatile uint32 diepmsk; 
  /** Device OUT Endpoint Common Interrupt Mask
   * Register. <i>Offset: 814h</i> */
  volatile uint32 doepmsk;  
  /** Device All Endpoints Interrupt Register.  <i>Offset: 818h</i> */
  volatile uint32 daint;  
  /** Device All Endpoints Interrupt Mask Register.  <i>Offset:
   * 81Ch</i> */
  volatile uint32 daintmsk; 
  /** Device IN Token Queue Read Register-1 (Read Only).
   * <i>Offset: 820h</i> */
  volatile uint32 dtknqr1;  
  /** Device IN Token Queue Read Register-2 (Read Only).
   * <i>Offset: 824h</i> */ 
  volatile uint32 dtknqr2;  
  /** Device VBUS  discharge Register.  <i>Offset: 828h</i> */
  volatile uint32 dvbusdis;   
  /** Device VBUS Pulse Register.  <i>Offset: 82Ch</i> */
  volatile uint32 dvbuspulse;
  /** Device IN Token Queue Read Register-3 (Read Only). /
   *  Device Thresholding control register (Read/Write)
   * <i>Offset: 830h</i> */ 
  volatile uint32 dtknqr3_dthrctl;  
  /** Device IN Token Queue Read Register-4 (Read Only). /
   *  Device IN EPs empty Inr. Mask Register (Read/Write)
   * <i>Offset: 834h</i> */ 
  volatile uint32 dtknqr4_fifoemptymsk;

  uint32 reserved[(0x900-0x838)/4];

  /* Device Logical IN Endpoint-Specific Registers 900h-AFCh */
  otg_dev_in_ep_regs_t in_ep[16];

  /** Device Logical OUT Endpoint-Specific Registers B00h-CFCh */
  otg_dev_out_ep_regs_t out_ep[16];

  uint32 reserved2[(0xe00-0xd00)/4];
  
} otg_dev_regs_t; 

/**
 * The Host Global Registers structure defines the size and relative
 * field offsets for the Host Mode Global Registers.  Host Global
 * Registers offsets 400h-7FFh.
*/
typedef struct otg_host_regs 
{
  /** Host Configuration Register.   <i>Offset: 400h</i> */
  volatile uint32 hcfg;     
  /** Host Frame Interval Register. <i>Offset: 404h</i> */
  volatile uint32 hfir;     
  /** Host Frame Number / Frame Remaining Register. <i>Offset: 408h</i> */
  volatile uint32 hfnum; 
  /** Reserved. <i>Offset: 40Ch</i> */
  uint32 reserved40C;
  /** Host Periodic Transmit FIFO/ Queue Status Register. <i>Offset: 410h</i> */
  volatile uint32 hptxsts;    
  /** Host All Channels Interrupt Register. <i>Offset: 414h</i> */
  volatile uint32 haint;    
  /** Host All Channels Interrupt Mask Register. <i>Offset: 418h</i> */
  volatile uint32 haintmsk;

  uint32 reserved[(0x400-0x1c)/4];
} otg_host_regs_t;


typedef struct otg_core_if 
{
  otg_core_regs_t core_regs;
  otg_host_regs_t host_regs;
  otg_dev_regs_t dev_regs;
  volatile uint32 pcgcctl;
  
}  otg_core_if_t;

#define       OTGReg            ((otg_core_if_t *)USB_REG_BASE)
//------------------------------------------------
// C MACROS
//------------------------------------------------ 
#ifndef ASM_CODE

#define enable_interrupts()  	//asm volatile ("bits %%imask, %0" : : "n" (GIE_BIT) : "%imask");
#define disable_interrupts() 	//asm volatile ("bitc %%imask, %0" : : "n" (GIE_BIT) : "%imask");

#define read_creg(creg,val)         //asm volatile ("mov\t%0, %"#creg"": "=r" (val))
    //  val must be a variable
    //  Example:
    //      read_creg(%smode,smode_value);          

#ifdef ZSP_G2
#define write_creg(creg,val)      	//asm volatile ("mov\tr0, %0\n\tmov\t%"#creg", r0": : "rn" (val) : "r0", #creg)
#else
#define write_creg(creg,val)      	//asm volatile ("mov\tr13, %0\n\tmov\t%"#creg", r13": : "rn" (val) : #creg)
#endif
    //  val may be a variable or constant
    //  Example:
    //      write_creg(%fmode,SAT|Q15);
    //      write_creg(%fmode,new_fmode_value);
    //  Do not directly move value into creg with "mov creg, %0" because
    //  some control registers do not have a mov cX, imm instruction

#define bitset_creg(creg,bitnum)	//asm volatile ("bits\t%"#creg", %0": : "n" (bitnum) : #creg)
    //  bitnum must be a constant
    //  Example:
    //      bitset_creg(%fmode,Q15_BIT);

#define bitclear_creg(creg,bitnum)	//asm volatile ("bitc\t%"#creg", %0": : "n" (bitnum) : #creg)
    //  bitnum must be a constant
    //  Example:
    //      bitclear_creg(%fmode,Q15_BIT);

#define bitinvert_creg(creg,bitnum)	//asm volatile ("biti\t%"#creg", %0": : "n" (bitnum) : #creg)
    //  bitnum must be a constant
    //  Example:
    //      bitinvert_creg(%smode,DCT_BIT);

#endif // not ASM_CODE


#endif // _CREG_H_
