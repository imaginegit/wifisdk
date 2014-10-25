/*
********************************************************************************
*                   Copyright (c) 2008,Huweiguo
*                         All rights reserved.
*
* File Name£º   Hw_memap.h
* 
* Description:  
*                      
*
* History:      <author>          <time>        <version>       
*               HuWeiGuo        2009-01-05         1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _HW_MEMAP_H_
#define _HW_MEMAP_H_

#if 1
#define CRU_BASE            ((UINT32)0x40000000)
#define GRF_BASE            ((UINT32)0x40010000)
#define I2S_BASE            ((UINT32)0x40020000)
#define I2C_BASE            ((UINT32)0x40030000)
#define SPI_BASE            ((UINT32)0x40040000)
#define UART_BASE           ((UINT32)0x40050000)
#define ADC_BASE            ((UINT32)0x40060000)
#define TIMER_BASE          ((UINT32)0x40070000)
#define PWM_BASE            ((UINT32)0x40080000)
#define EFUSE_BASE          ((UINT32)0x40090000)
#define GPIO_BASE           ((UINT32)0x400a0000)
#define RTC_BASE            ((UINT32)0x400b0000)

#define DMA_BASE            ((UINT32)0x60000000) 
#define IMDCT_BASE          ((UINT32)0x60010000)
#define SYNTH_BASE          ((UINT32)0x60020000)
#define USBOTG20_BASE       ((UINT32)0x60030000)
#define SDMMC_BASE          ((UINT32)0x60070000)
#define NANDC_BASE          ((UINT32)0x60080000)
#define LCD_BASE            ((UINT32)0x60090000)

#else

///////////////////////////////////////////
//       BOOT_ROM
///////////////////////////////////////////
#define      BOOT_ROM_BASE                 0x00000000


///////////////////////////////////////////
//       IRAM
///////////////////////////////////////////
#define      IRAM_BASE                     0x01000000
#define      IRAM0_BASE                    0x01000000
#define      IRAM1_BASE                    0x01008000
#define      IRAM2_BASE                    0x01010000
#define      IRAM3_BASE                    0x01018000

///////////////////////////////////////////
//       TABLE_ROM
///////////////////////////////////////////
#define      TABLE_ROM_BASE                0x02000000


///////////////////////////////////////////
//       DRAM
///////////////////////////////////////////
#define        DRAM_BASE                   0x03000000
#define        DRAM0_BASE                  0x03000000
#define        DRAM1_BASE                  0x03008000
#define        DRAM2_BASE                  0x03010000

///////////////////////////////////////////
//       CRU
///////////////////////////////////////////
#define      CRU_BASE                      0x40000000

#define      CRU_PLL_CON                   0x00
#define      CRU_MODE_CON                  0x04
#define      CRU_CLKSEL_CPU_CON            0x08
#define      CRU_CLKSEL_CODEC_CON          0x0c
#define      CRU_CLKSEL_MMC_CON            0x10
#define      CRU_CLKSEL_SPI_CON            0x14
#define      CRU_CLKSEL_UART_CON           0x18
#define      CRU_CLKSEL_PERI_CON           0x1c
#define      CRU_CLKGATE_CON0              0x20 
#define      CRU_CLKGATE_CON1              0x24
#define      CRU_SOFTRST_CON0              0x28
#define      CRU_SOFTRST_CON1              0x2c
#define      CRU_STCLK_CON                 0x30
#define      CRU_STATUS                    0x34


///////////////////////////////////////////
//       GRF
///////////////////////////////////////////
#define      GRF_BASE                      0x40010000

#define      GRF_GPIO0A_IOMUX              0x00
#define      GRF_GPIO0B_IOMUX              0x04
#define      GRF_GPIO0C_IOMUX              0x08
#define      GRF_GPIO0D_IOMUX              0x0c
#define      GRF_GPIO0A_PULL               0x10
#define      GRF_GPIO0B_PULL               0x14
#define      GRF_GPIO0C_PULL               0x18
#define      GRF_GPIO0D_PULL               0x1c

#define      GRF_INTER_CON0                0x20
#define      GRF_INTER_CON1                0x24
#define      GRF_IOMUX_CON0                0x28
#define      GRF_IOMUX_CON1                0x2c

#define      GRF_OTGPHY_CON0               0x30
#define      GRF_OTGPHY_CON1               0x34
#define      GRF_SARADC_CON                0x38

#define      GRF_MPMU_DAC_CON              0x40
#define      GRF_MPMU_HP_CON               0x44
#define      GRF_MPMU_PWR_CON              0x48
#define      GRF_MPMU_STATE                0x4c

#define      GRF_SOFT_CON0                 0x60
#define      GRF_SOFT_CON1                 0x64
#define      GRF_SOFT_CON2                 0x68
#define      GRF_SOFT_CON3                 0x6c

///////////////////////////////////////////
//       I2S
///////////////////////////////////////////
#define      I2S_BASE                      0x40020000

#define      I2S_TXCR                      0x0000
#define      I2S_RXCR                      0x0004
#define      I2S_CKR                       0x0008
#define      I2S_FIFOLR                    0x000c
#define      I2S_DMACR                     0x0010
#define      I2S_INTCR                     0x0014
#define      I2S_INTSR                     0x0018
#define      I2S_XFER                      0x001c
#define      I2S_CLR                       0x0020
#define      I2S_TXDR                      0x0024
#define      I2S_RXDR                      0x0028


///////////////////////////////////////////
//       I2C
///////////////////////////////////////////
#define      I2C_BASE                      0x40030000

#define      I2C_MTXR  	                   0x00
#define      I2C_MRXR  	                   0x04
#define      I2C_STXR  	                   0x08
#define      I2C_SRXR  	                   0x0c
#define      I2C_SADDR	                   0x10
#define      I2C_IER 	                   0x14
#define      I2C_ISR  	                   0x18
#define      I2C_LCMR 	                   0x1c
#define      I2C_LSR 	                   0x20
#define      I2C_CONR  	                   0x24
#define      I2C_OPR  	                   0x28


///////////////////////////////////////////
//       SPI
///////////////////////////////////////////
#define      SPI_BASE                     0x40040000

#define      SPI_CTRLR0                    0x0000
#define      SPI_CTRLR1                    0x0004
#define      SPI_ENR                       0x0008
#define      SPI_SER                       0x000c
#define      SPI_BAUDR                     0x0010
#define      SPI_TXFTLR                    0x0014
#define      SPI_RXFTLR                    0x0018
#define      SPI_TXFLR                     0x001c
#define      SPI_RXFLR                     0x0020
#define      SPI_SR                        0x0024
#define      SPI_IPR                       0x0028
#define      SPI_IMR                       0x002c
#define      SPI_ISR                       0x0030
#define      SPI_RISR                      0x0034
#define      SPI_ICR                       0x0038
#define      SPI_DMACR                     0x003c
#define      SPI_DMATDLR                   0x0040
#define      SPI_DMARDLR                   0x0044
#define      SPI_TXDR                      0x0400
#define      SPI_RXDR                      0x0800


///////////////////////////////////////////
//       UART
///////////////////////////////////////////
#define      UART_BASE                    0x40050000

#define      UART_RBR                      0x00    
#define      UART_THR                      0x00
#define      UART_DLL                      0x00
#define      UART_DLH                      0x04
#define      UART_IER                      0x04
#define      UART_IIR                      0x08
#define      UART_FCR                      0x08
#define      UART_LCR                      0x0c
#define      UART_MCR                      0x10
#define      UART_LSR                      0x14
#define      UART_MSR                      0x18
#define      UART_SCR                      0x1c
#define      UART_LPDLL                    0x20 
#define      UART_LPDLH                    0x24 
#define      UART_SPBR1                    0x30 
#define      UART_SPBR2                    0x34
#define      UART_SPBR3                    0x38 
#define      UART_SPBR4                    0x3c
#define      UART_SPBR5                    0x40
#define      UART_SPBR6                    0x44
#define      UART_SPBR7                    0x48
#define      UART_SPBR8                    0x4c
#define      UART_STHR1                    0x50
#define      UART_STHR2                    0x54
#define      UART_STHR3                    0x58
#define      UART_STHR4                    0x5c
#define      UART_STHR5                    0x60 
#define      UART_STHR6                    0x64
#define      UART_STHR7                    0x68
#define      UART_STHR8                    0x6c
#define      UART_FAR                      0x70
#define      UART_TFR                      0x74
#define      UART_RFW                      0x78
#define      UART_USR                      0x7c
#define      UART_TFL                      0x80
#define      UART_RFL                      0x84
#define      UART_SRR                      0x88
#define      UART_SRTS                     0x8c
#define      UART_SBCR                     0x90
#define      UART_SDMAM                    0x94
#define      UART_SFE                      0x98
#define      UART_SRT                      0x9c
#define      UART_STET                     0xa0
#define      UART_HTX                      0xa4
#define      UART_DMASA                    0xa8
#define      UART_UCV                      0xf8
#define      UART_CTR                      0xfc


///////////////////////////////////////////
//       SARADC
///////////////////////////////////////////
#define      SARADC_BASE                   0x40060000


#define      SARADC_DATA                   0x0000
#define      SARADC_STAS                   0x0004
#define      SARADC_CTRL                   0x0008
#define      SARADC_INTER                  0x000c


///////////////////////////////////////////
//       TIMER
///////////////////////////////////////////
#define      TIMER_BASE                   0x40070000

#define      TIMER_TIMER1_LOAD_COUNT       0x0000
#define      TIMER_TIMER1_CURRENT_VALUE    0x0004
#define      TIMER_TIMER1_CONTROLREG       0x0008
#define      TIMER_TIMER1_EOI              0x000c
#define      TIMER_TIMER1_INTSTATUS        0x0010
#define      TIMER_TIMERS_INTSTATUS        0x00a0
#define      TIMER_TIMERS_EOI              0x00a4
#define      TIMER_TIMERS_RAWSTATUS        0x00a8


///////////////////////////////////////////
//       PWM
///////////////////////////////////////////
#define      PWM_BASE                      0x40080000

#define      PWM_CON  		           0x00
#define      PWM_CMD 		           0x04
#define      PWM_DATA0  	           0x08
#define      PWM_DATA1	  	           0x0C
#define      PWM_CNT  		           0x10


///////////////////////////////////////////
//       EFUSE      
///////////////////////////////////////////
#define      EFUSE_BASE                    0x40090000
                                           
#define      EFUSE_CON                     0x00
#define      EFUSE_DOUT                    0x04
#define      EFUSE_DOUTH                   0x08


///////////////////////////////////////////
//       GPIO
///////////////////////////////////////////
#define      GPIO_BASE                     0x400a0000

#define      GPIO_SWPORTA_DR               0x0000
#define      GPIO_SWPORTA_DDR              0x0004
#define      GPIO_INTEN                    0x0030
#define      GPIO_INTMASK                  0x0034
#define      GPIO_INTTYPE_LEVEL            0x0038
#define      GPIO_INT_POLARITY             0x003c
#define      GPIO_INT_STATUS               0x0040
#define      GPIO_INT_RAWSTATUS            0x0044
#define      GPIO_DEBOUNCE                 0x0048
#define      GPIO_PORTA_EOI                0x004c
#define      GPIO_EXT_PORTA                0x0050
#define      GPIO_LS_SYNC                  0x0060


///////////////////////////////////////////
//       RTC
///////////////////////////////////////////
#define      RTC_BASE                      0x400b0000

#define      RTC_CTRL                      0x00
#define      RTC_TIME                      0x04
#define      RTC_DATE                      0x08
#define      RTC_TIME_SET                  0x0c
#define      RTC_DATE_SET                  0x10
#define      RTC_COMP                      0x14


///////////////////////////////////////////
//       DMAC
///////////////////////////////////////////
#define      DWDMA_BASE                    0x60000000

#define      DWDMA_SAR0                    0x0000
#define      DWDMA_DAR0                    0x0008
#define      DWDMA_LLP0                    0x0010
#define      DWDMA_CTL0                    0x0018
#define      DWDMA_SSTAT0                  0x0020
#define      DWDMA_DSTAT0                  0x0028
#define      DWDMA_SSTATAR0                0x0030
#define      DWDMA_DSTATAR0                0x0038
#define      DWDMA_CFG0                    0x0040
#define      DWDMA_SGR0                    0x0048
#define      DWDMA_DSR0                    0x0050
#define      DWDMA_CFG0_H                  0x0054
#define      DWDMA_SAR1                    0x0058
#define      DWDMA_DAR1                    0x0060
#define      DWDMA_LLP1                    0x0068
#define      DWDMA_CTL1                    0x0070
#define      DWDMA_SSTAT1                  0x0078
#define      DWDMA_DSTAT1                  0x0080
#define      DWDMA_SSTATAR1                0x0088
#define      DWDMA_DSTATAR1                0x0090
#define      DWDMA_CFG1                    0x0098
#define      DWDMA_SGR1                    0x00a0
#define      DWDMA_DSR1                    0x00a8
#define      DWDMA_SAR2                    0x00b0
#define      DWDMA_DAR2                    0x00b8
#define      DWDMA_LLP2                    0x00c0
#define      DWDMA_CTL2                    0x00c8
#define      DWDMA_SSTAT2                  0x00d0
#define      DWDMA_DSTAT2                  0x00d8
#define      DWDMA_SSTATAR2                0x00e0
#define      DWDMA_DSTATAR2                0x00e8
#define      DWDMA_CFG2                    0x00f0
#define      DWDMA_SGR2                    0x00f8
#define      DWDMA_DSR2                    0x0100
#define      DWDMA_RAWTFR                  0x02c0
#define      DWDMA_RAWBLOCK                0x02c8
#define      DWDMA_RAWSRCTRAN              0x02d0
#define      DWDMA_RAWDSTTRAN              0x02d8
#define      DWDMA_RAWERR                  0x02e0
#define      DWDMA_STATUSTFR               0x02e8
#define      DWDMA_STATUSBLOCK             0x02f0
#define      DWDMA_STATUSSRCTRAN           0x02f8
#define      DWDMA_STATUSDSTTRAN           0x0300
#define      DWDMA_STATUSERR               0x0308
#define      DWDMA_MASKTFR                 0x0310
#define      DWDMA_MASKBLOCK               0x0318
#define      DWDMA_MASKSRCTRAN             0x0320
#define      DWDMA_MASKDSTTRAN             0x0328
#define      DWDMA_MASKERR                 0x0330
#define      DWDMA_CLEARTFR                0x0338
#define      DWDMA_CLEARBLOCK              0x0340
#define      DWDMA_CLEARSRCTRAN            0x0348
#define      DWDMA_CLEARDSTTRAN            0x0350
#define      DWDMA_CLEARERR                0x0358
#define      DWDMA_STATUSINT               0x0360
#define      DWDMA_DMACFGREG               0x0398
#define      DWDMA_CHENREG                 0x03a0


///////////////////////////////////////////
//      imdct36
///////////////////////////////////////////
#define      IMDCT36_BASE                  0x60010000

#define      IMDCT36_CTRL  		   0x800
#define      IMDCT36_STAT 		   0x804
#define      IMDCT36_INT	  	   0x808
#define      IMDCT36_EOI	  	   0x80C


///////////////////////////////////////////
//     synthsize 
///////////////////////////////////////////
#define      SYNTH_BASE                    0x60020000

#define      SYNTH_CTRL  		   0x800
#define      SYNTH_CFG	 		   0x804
#define      SYNTH_STAT 		   0x808
#define      SYNTH_INT	  		   0x80C
#define      SYNTH_EOI	  		   0x810


///////////////////////////////////////////
//       USB
///////////////////////////////////////////
#define      USBOTG20_BASE                 0x60030000

#define      USB_GOTGCTL                   0x000
#define      USB_GOTGINT                   0x004
#define      USB_GAHBCFG                   0x008
#define      USB_GUSBCFG                   0x00c
#define      USB_GRSTCTL                   0x010
#define      USB_GINTSTS                   0x014
#define      USB_GINTMSK                   0x018
#define      USB_GRXSTSR                   0x01c
#define      USB_GRXSTSP                   0x020
#define      USB_GRXFSIZ                   0x024
#define      USB_GNPTXFSIZ                 0x028
#define      USB_GNPTXSTS                  0x02c
#define      USB_GI2CCTL                   0x030
#define      USB_GPVNDCTL                  0x034
#define      USB_GGPIO                     0x038
#define      USB_GUID                      0x03c
#define      USB_GSNPSID                   0x040
#define      USB_GHWCFG1                   0x044
#define      USB_GHWCFG2                   0x048
#define      USB_GHWCFG3                   0x04c
#define      USB_GHWCFG4                   0x050
#define      USB_GLPMCFG                   0x054
#define      USB_GPWRDN                    0x058
#define      USB_GDFIFOCFG                 0x05c
#define      USB_GADPCTL                   0x060
#define      USB_HPTXFSIZ                  0x100
#define      USB_DIEPTXF1                  0x104
#define      USB_DIEPTXF2                  0x108
#define      USB_DIEPTXF3                  0x10c
#define      USB_DIEPTXF4                  0x110
#define      USB_DIEPTXF5                  0x114
#define      USB_DIEPTXF6                  0x118


#define      USB_HCFG                      0x400
#define      USB_HFIR                      0x404
#define      USB_HFNUM                     0x408
#define      USB_HPTXSTS                   0x410 
#define      USB_HAINT                     0x414
#define      USB_HAINTMSK                  0x418   //0x440~0x47c
#define      USB_HPRT                      0x440
#define      USB_HCCHAR0                   0x500
#define      USB_HCSPLT0                   0x504
#define      USB_HCINT0                    0x508
#define      USB_HCINTMSK0                 0x50c
#define      USB_HCTSIZ0                   0x510
#define      USB_HCDMA0                    0x514
#define      USB_HCCHAR1                   0x520
#define      USB_HCSPLT1                   0x524
#define      USB_HCINT1                    0x528
#define      USB_HCINTMSK1                 0x52c
#define      USB_HCTSIZ1                   0x530
#define      USB_HCDMA1                    0x534
#define      USB_HCCHAR2                   0x540
#define      USB_HCSPLT2                   0x544
#define      USB_HCINT2                    0x548
#define      USB_HCINTMSK2                 0x54c
#define      USB_HCTSIZ2                   0x550
#define      USB_HCDMA2                    0x554
#define      USB_HCCHAR3                   0x560
#define      USB_HCSPLT3                   0x564
#define      USB_HCINT3                    0x568
#define      USB_HCINTMSK3                 0x56c
#define      USB_HCTSIZ3                   0x570
#define      USB_HCDMA3                    0x574
#define      USB_HCCHAR4                   0x580
#define      USB_HCSPLT4                   0x584
#define      USB_HCINT4                    0x588
#define      USB_HCINTMSK4                 0x58c
#define      USB_HCTSIZ4                   0x590
#define      USB_HCDMA4                    0x594
#define      USB_HCCHAR5                   0x5a0
#define      USB_HCSPLT5                   0x5a4
#define      USB_HCINT5                    0x5a8
#define      USB_HCINTMSK5                 0x5ac
#define      USB_HCTSIZ5                   0x5b0
#define      USB_HCDMA5                    0x5b4
#define      USB_HCCHAR6                   0x5c0
#define      USB_HCSPLT6                   0x5c4
#define      USB_HCINT6                    0x5c8
#define      USB_HCINTMSK6                 0x5cc
#define      USB_HCTSIZ6                   0x5d0
#define      USB_HCDMA6                    0x5d4


#define      USB_HCCHAR7                   0x5e0
#define      USB_HCSPLT7                   0x5e4
#define      USB_HCINT7                    0x5e8
#define      USB_HCINTMSK7                 0x5ec
#define      USB_HCTSIZ7                   0x5f0
#define      USB_HCDMA7                    0x5f4
#define      USB_HCCHAR8                   0x600
#define      USB_HCSPLT8                   0x604
#define      USB_HCINT8                    0x608
#define      USB_HCINTMSK8                 0x60c
#define      USB_HCTSIZ8                   0x610
#define      USB_HCDMA8                    0x614
#define      USB_HCCHAR9                   0x620
#define      USB_HCSPLT9                   0x624
#define      USB_HCINT9                    0x628
#define      USB_HCINTMSK9                 0x62c
#define      USB_HCTSIZ9                   0x630
#define      USB_HCDMA9                    0x634
#define      USB_HCCHAR10                  0x640
#define      USB_HCSPLT10                  0x644
#define      USB_HCINT10                   0x648
#define      USB_HCINTMSK10                0x64c
#define      USB_HCTSIZ10                  0x650
#define      USB_HCDMA10                   0x654
#define      USB_HCCHAR11                  0x660
#define      USB_HCSPLT11                  0x664
#define      USB_HCINT11                   0x668
#define      USB_HCINTMSK11                0x66c
#define      USB_HCTSIZ11                  0x670
#define      USB_HCDMA11                   0x674
#define      USB_HCCHAR12                  0x680
#define      USB_HCSPLT12                  0x684
#define      USB_HCINT12                   0x688
#define      USB_HCINTMSK12                0x68c
#define      USB_HCTSIZ12                  0x690
#define      USB_HCDMA12                   0x694
#define      USB_HCCHAR13                  0x6a0
#define      USB_HCSPLT13                  0x6a4
#define      USB_HCINT13                   0x6a8
#define      USB_HCINTMSK13                0x6ac
#define      USB_HCTSIZ13                  0x6b0
#define      USB_HCDMA13                   0x6b4
#define      USB_HCCHAR14                  0x6c0
#define      USB_HCSPLT14                  0x6c4
#define      USB_HCINT14                   0x6c8
#define      USB_HCINTMSK14                0x6cc
#define      USB_HCTSIZ14                  0x6d0
#define      USB_HCDMA14                   0x6d4
#define      USB_HCCHAR15                  0x6e0
#define      USB_HCSPLT15                  0x6e4
#define      USB_HCINT15                   0x6e8
#define      USB_HCINTMSK15                0x6ec
#define      USB_HCTSIZ15                  0x6f0
#define      USB_HCDMA15                   0x6f4


#define      USB_DCFG                      0x800
#define      USB_DCTL                      0x804
#define      USB_DSTS                      0x808
#define      USB_DIEPMSK                   0x810
#define      USB_DOEPMSK                   0x814
#define      USB_DAINT                     0x818
#define      USB_DAINTMSK                  0x81c
#define      USB_DTKNQR1                   0x820
#define      USB_DTKNQR2                   0x824
#define      USB_DVBUSDIS                  0x828
#define      USB_DVBUSPULSE                0x82c
#define      USB_DTHRCTL                   0x830
#define      USB_DIEPEMPMSK                0x834
#define      USB_DEACHINT                  0x838
#define      USB_DEACHINTMSK               0x83c
#define      USB_DIEPEACHMSKN              0x840
#define      USB_DOEPEACHMSKN              0x880

#define      USB_DIEPCTL0                  0x900
#define      USB_DIEPINT0                  0x908
#define      USB_DIEPTSIZ0                 0x910
#define      USB_DIEPDMA0                  0x914
#define      USB_DTXFSTS0                  0x918
#define      USB_DIEPDMAB0                 0x91c
#define      USB_DIEPCTL1                  0x920
#define      USB_DIEPINT1                  0x928
#define      USB_DIEPTSIZ1                 0x930
#define      USB_DIEPDMA1                  0x934
#define      USB_DTXFSTS1                  0x938
#define      USB_DIEPDMAB1                 0x93c
#define      USB_DIEPCTL2                  0x940
#define      USB_DIEPINT2                  0x948
#define      USB_DIEPTSIZ2                 0x950
#define      USB_DIEPDMA2                  0x954
#define      USB_DTXFSTS2                  0x958
#define      USB_DIEPDMAB2                 0x95c
#define      USB_DIEPCTL3                  0x960
#define      USB_DIEPINT3                  0x968
#define      USB_DIEPTSIZ3                 0x970
#define      USB_DIEPDMA3                  0x974
#define      USB_DTXFSTS3                  0x978
#define      USB_DIEPDMAB3                 0x97c
#define      USB_DIEPCTL4                  0x980
#define      USB_DIEPINT4                  0x988
#define      USB_DIEPTSIZ4                 0x990
#define      USB_DIEPDMA4                  0x994
#define      USB_DTXFSTS4                  0x998
#define      USB_DIEPDMAB4                 0x99c
#define      USB_DIEPCTL5                  0x9a0
#define      USB_DIEPINT5                  0x9a8
#define      USB_DIEPTSIZ5                 0x9b0
#define      USB_DIEPDMA5                  0x9b4
#define      USB_DTXFSTS5                  0x9b8
#define      USB_DIEPDMAB5                 0x9bc
#define      USB_DIEPCTL6                  0x9c0
#define      USB_DIEPINT6                  0x9c8
#define      USB_DIEPTSIZ6                 0x9d0
#define      USB_DIEPDMA6                  0x9d4
#define      USB_DTXFSTS6                  0x9d8
#define      USB_DIEPDMAB6                 0x9dc
#define      USB_DIEPCTL7                  0x9e0
#define      USB_DIEPINT7                  0x9e8
#define      USB_DIEPTSIZ7                 0x9f0
#define      USB_DIEPDMA7                  0x9f4
#define      USB_DTXFSTS7                  0x9f8
#define      USB_DIEPDMAB7                 0x9fc
#define      USB_DIEPCTL8                  0xa00
#define      USB_DIEPINT8                  0xa08
#define      USB_DIEPTSIZ8                 0xa10
#define      USB_DIEPDMA8                  0xa14
#define      USB_DTXFSTS8                  0xa18
#define      USB_DIEPDMAB8                 0xa1c
#define      USB_DIEPCTL9                  0xa20
#define      USB_DIEPINT9                  0xa28
#define      USB_DIEPTSIZ9                 0xa30
#define      USB_DIEPDMA9                  0xa34
#define      USB_DTXFSTS9                  0xa38
#define      USB_DIEPDMAB9                 0xa3c
#define      USB_DIEPCTL10                 0xa40
#define      USB_DIEPINT10                 0xa48
#define      USB_DIEPTSIZ10                0xa50
#define      USB_DIEPDMA10                 0xa54
#define      USB_DTXFSTS10                 0xa58
#define      USB_DIEPDMAB10                0xa5c
#define      USB_DIEPCTL11                 0xa60
#define      USB_DIEPINT11                 0xa68
#define      USB_DIEPTSIZ11                0xa70
#define      USB_DIEPDMA11                 0xa74
#define      USB_DTXFSTS11                 0xa78
#define      USB_DIEPDMAB11                0xa7c
#define      USB_DIEPCTL12                 0xa80
#define      USB_DIEPINT12                 0xa88
#define      USB_DIEPTSIZ12                0xa90
#define      USB_DIEPDMA12                 0xa94
#define      USB_DTXFSTS12                 0xa98
#define      USB_DIEPDMAB12                0xa9c
#define      USB_DIEPCTL13                 0xaa0
#define      USB_DIEPINT13                 0xaa8
#define      USB_DIEPTSIZ13                0xab0
#define      USB_DIEPDMA13                 0xab4
#define      USB_DTXFSTS13                 0xab8
#define      USB_DIEPDMAB13                0xabc
#define      USB_DIEPCTL14                 0xac0
#define      USB_DIEPINT14                 0xac8
#define      USB_DIEPTSIZ14                0xad0
#define      USB_DIEPDMA14                 0xad4
#define      USB_DTXFSTS14                 0xad8
#define      USB_DIEPDMAB14                0xadc
#define      USB_DIEPCTL15                 0xae0
#define      USB_DIEPINT15                 0xae8
#define      USB_DIEPTSIZ15                0xaf0
#define      USB_DIEPDMA15                 0xaf4
#define      USB_DTXFSTS15                 0xaf8
#define      USB_DIEPDMAB15                0xafc


#define      USB_DOEPCTL0                  0xb00
#define      USB_DOEPINT0                  0xb08
#define      USB_DOEPTSIZ0                 0xb10
#define      USB_DOEPDMA0                  0xb14
#define      USB_DOEPDMAB0                 0xb1c
#define      USB_DOEPCTL1                  0xb20
#define      USB_DOEPINT1                  0xb28
#define      USB_DOEPTSIZ1                 0xb30
#define      USB_DOEPDMA1                  0xb34
#define      USB_DOEPDMAB1                 0xb3c
#define      USB_DOEPCTL2                  0xb40
#define      USB_DOEPINT2                  0xb48
#define      USB_DOEPTSIZ2                 0xb50
#define      USB_DOEPDMA2                  0xb54
#define      USB_DOEPDMAB2                 0xb5c
#define      USB_DOEPCTL3                  0xb60
#define      USB_DOEPINT3                  0xb68
#define      USB_DOEPTSIZ3                 0xb70
#define      USB_DOEPDMA3                  0xb74
#define      USB_DOEPDMAB3                 0xb7c
#define      USB_DOEPCTL4                  0xb80
#define      USB_DOEPINT4                  0xb88
#define      USB_DOEPTSIZ4                 0xb90
#define      USB_DOEPDMA4                  0xb94
#define      USB_DOEPDMAB4                 0xb9c
#define      USB_DOEPCTL5                  0xba0
#define      USB_DOEPINT5                  0xba8
#define      USB_DOEPTSIZ5                 0xbb0
#define      USB_DOEPDMA5                  0xbb4
#define      USB_DOEPDMAB5                 0xbbc
#define      USB_DOEPCTL6                  0xbc0
#define      USB_DOEPINT6                  0xbc8
#define      USB_DOEPTSIZ6                 0xbd0
#define      USB_DOEPDMA6                  0xbd4
#define      USB_DOEPDMAB6                 0xbdc
#define      USB_DOEPCTL7                  0xbe0
#define      USB_DOEPINT7                  0xbe8
#define      USB_DOEPTSIZ7                 0xbf0
#define      USB_DOEPDMA7                  0xbf4
#define      USB_DOEPDMAB7                 0xbfc
#define      USB_DOEPCTL8                  0xc00
#define      USB_DOEPINT8                  0xc08
#define      USB_DOEPTSIZ8                 0xc10
#define      USB_DOEPDMA8                  0xc14
#define      USB_DOEPDMAB8                 0xc1c
#define      USB_DOEPCTL9                  0xc20
#define      USB_DOEPINT9                  0xc28
#define      USB_DOEPTSIZ9                 0xc30
#define      USB_DOEPDMA9                  0xc34
#define      USB_DOEPDMAB9                 0xc3c


#define      USB_EPBUF0                    0x1000
#define      USB_EPBUF1                    0x2000
#define      USB_EPBUF2                    0x3000
#define      USB_EPBUF3                    0x4000
#define      USB_EPBUF4                    0x5000
#define      USB_EPBUF5                    0x6000
#define      USB_EPBUF6                    0x7000
#define      USB_EPBUF7                    0x8000

#define      USB_HSTCH0_FF                 0x1000
#define      USB_HSTCH1_FF                 0x2000

///////////////////////////////////////////
//       SD
///////////////////////////////////////////
#define      SDMMC_BASE                   0x60070000

#define      SDMMC_CTRL                    0x0000
#define      SDMMC_PWREN                   0x0004
#define      SDMMC_CLKDIV                  0x0008
#define      SDMMC_CLKSRC                  0x000c
#define      SDMMC_CLKENA                  0x0010
#define      SDMMC_TMOUT                   0x0014
#define      SDMMC_CTYPE                   0x0018
#define      SDMMC_BLKSIZ                  0x001c
#define      SDMMC_BYTCNT                  0x0020
#define      SDMMC_INTMASK                 0x0024
#define      SDMMC_CMDARG                  0x0028
#define      SDMMC_CMD                     0x002c
#define      SDMMC_RESP0                   0x0030
#define      SDMMC_RESP1                   0x0034
#define      SDMMC_RESP2                   0x0038
#define      SDMMC_RESP3                   0x003c
#define      SDMMC_MINTSTS                 0x0040
#define      SDMMC_RINTSTS                 0x0044
#define      SDMMC_STATUS                  0x0048
#define      SDMMC_FIFOTH                  0x004c
#define      SDMMC_CDETECT                 0x0050
#define      SDMMC_WRTPRT                  0x0054
#define      SDMMC_GPIO                    0x0058
#define      SDMMC_TCBCNT                  0x005c
#define      SDMMC_TBBCNT                  0x0060
#define      SDMMC_DEBNCE                  0x0064
#define      SDMMC_USRID                   0x0068
#define      SDMMC_VERID                   0x006c
#define      SDMMC_HCON                    0x0070
#define      SDMMC_UHS_REG                 0x0074
#define      SDMMC_RST_N                   0x0078
#define      SDMMC_CARDTHRCTL              0x0100
#define      SDMMC_BACK_END_POWER          0x0104


///////////////////////////////////////////
//       NANDC
///////////////////////////////////////////
#define      NANDC_BASE                    0x60080000

#define      NANDC_ECC_CTL                 0x0000
#define      NANDC_FLASH_CTL               0x0004
//#define      NANDC_NORWAIT                 0x0008
#define      NANDC_FLASH_WAIT              0x000c
//#define      NANDC_LCDWAIT                 0x0010
#define      NANDC_EMICTL                  0x0014
#define      NANDC_SYSD_REG_0              0x0018
#define      NANDC_BCHEN0_1                0x001c
#define      NANDC_BCHEN0_2                0x0020
#define      NANDC_BCHEN0_3                0x0024
#define      NANDC_BCHEN0_4                0x0028
#define      NANDC_BCHEN0_5                0x002c
#define      NANDC_BCHEN0_6                0x0030
#define      NANDC_BCHEN0_7                0x0034
#define      NANDC_BCHEN0_8                0x0038
#define      NANDC_BCHEN0_9                0x003c
#define      NANDC_BCHEN0_10               0x0040
#define      NANDC_BCHEN0_11               0x0044
#define      NANDC_BCHEN0_12               0x0048
#define      NANDC_BCHEN0_13               0x004c
#define      NANDC_BCHEN0_14               0x0050
#define      NANDC_BCHEN0_15               0x0054
#define      NANDC_BCHEN0_16               0x0058
#define      NANDC_BCHEN0_17               0x005c
#define      NANDC_BCHEN0_18               0x0060
#define      NANDC_BCHEN0_19               0x0064
#define      NANDC_BCHEN0_20               0x0068
#define      NANDC_BCHEN0_21               0x006c
#define      NANDC_BCHEN0_22               0x0070
#define      NANDC_BCHEN0_23               0x0074
#define      NANDC_BCHEN0_24               0x0078
#define      NANDC_BCHEN0_25               0x007c
#define      NANDC_BCHEN0_26               0x0080
#define      NANDC_BCHEN0_27               0x0084

#define      NANDC_BCHDE0_0                0x0088
#define      NANDC_BCHDE0_1                0x008c
#define      NANDC_BCHDE0_2                0x0090
#define      NANDC_BCHDE0_3                0x0094
#define      NANDC_BCHDE0_4                0x0098
#define      NANDC_BCHDE0_5                0x009c
#define      NANDC_BCHDE0_6                0x00a0
#define      NANDC_BCHDE0_7                0x00a4
#define      NANDC_BCHDE0_8                0x00a8
#define      NANDC_BCHDE0_9                0x00ac
#define      NANDC_BCHDE0_10               0x00b0
#define      NANDC_BCHDE0_11               0x00b4
#define      NANDC_BCHDE0_12               0x00b8
#define      NANDC_BCHDE0_13               0x00bc
#define      NANDC_BCHDE0_14               0x00c0
#define      NANDC_BCHDE0_15               0x00c4
#define      NANDC_BCHDE0_16               0x00c8
#define      NANDC_BCHDE0_17               0x00cc
#define      NANDC_BCHDE0_18               0x00d0
#define      NANDC_BCHDE0_19               0x00d4
#define      NANDC_BCHDE0_20               0x00d8
#define      NANDC_BCHDE0_21               0x00dc
#define      NANDC_BCHDE0_22               0x00e0
#define      NANDC_BCHDE0_23               0x00e4
#define      NANDC_BCHDE0_24               0x00e8
#define      NANDC_BCHDE0_25               0x00ec
#define      NANDC_BCHDE0_26               0x00f0
#define      NANDC_BCHDE0_27               0x00f4
#define      NANDC_BCHDE0_28               0x00f8
#define      NANDC_BCHDE0_29               0x00fc
#define      NANDC_BCHDE0_30               0x0100
#define      NANDC_BCHDE0_31               0x0104
#define      NANDC_BCHDE0_32               0x0108
#define      NANDC_BCHDE0_33               0x010c
#define      NANDC_BCHDE0_34               0x0110
#define      NANDC_BCHDE0_35               0x0114
#define      NANDC_BCHDE0_36               0x0118
#define      NANDC_BCHDE0_37               0x011c
#define      NANDC_BCHDE0_38               0x0120
#define      NANDC_BCHDE0_39               0x0124
#define      NANDC_BCHDE0_40               0x0128
#define      NANDC_BCHDE0_41               0x012c
#define      NANDC_BCHDE0_42               0x0130
#define      NANDC_BCHDE0_43               0x0134
#define      NANDC_BCHDE0_44               0x0138
#define      NANDC_BCHDE0_45               0x013c
#define      NANDC_BCHDE0_46               0x0140
#define      NANDC_BCHDE0_47               0x0144
#define      NANDC_BCHDE0_48               0x0148
#define      NANDC_BCHDE0_49               0x014c
#define      NANDC_BCHDE0_50               0x0150
#define      NANDC_BCHDE0_51               0x0154
#define      NANDC_BCHDE0_52               0x0158
#define      NANDC_BCHDE0_53               0x015c
#define      NANDC_BCHDE0_54               0x0160
#define      NANDC_BCHDE0_55               0x0164
#define      NANDC_BCHDE0_56               0x0168
#define      NANDC_BCHDE0_57               0x016c
#define      NANDC_BCHDE0_58               0x0170
#define      NANDC_BCHDE0_59               0x0174

#define      NANDC_BCHST                   0x0178
#define      NANDC_RND_CTL                 0x017c

#define      NANDC_FLASH0                  0x0200
#define      NANDC_FLASH0_ALE              0x0204
#define      NANDC_FLASH0_CLE              0x0208
#define      NANDC_FLASH1                  0x0400
#define      NANDC_FLASH1_ALE              0x0404
#define      NANDC_FLASH1_CLE              0x0408
#define      NANDC_FLASH2                  0x0600
#define      NANDC_FLASH2_ALE              0x0604
#define      NANDC_FLASH2_CLE              0x0608
#define      NANDC_FLASH3                  0x0800
#define      NANDC_FLASH3_ALE              0x0804
#define      NANDC_FLASH3_CLE              0x0808



///////////////////////////////////////////
//       LCDC
///////////////////////////////////////////
#define      LCDC_BASE                    0x60090000

#define      LCD_WAIT                      0x00
#define      LCD_MODE                      0x04
#define      LCD_CMD                       0x08
#define      LCD_DAT                       0x0c


///////////////////////////////////////////
//       NVIC  
///////////////////////////////////////////
#define      NVIC_BASE                     0xe000e000

#define      NVIC_ICTR                     0x04
#define      NVIC_SYSTIC_CTLST             0x10
#define      NVIC_SYSTIC_RLOAD             0x14
#define      NVIC_SYSTIC_CURRV             0x18
#define      NVIC_SYSTIC_REFV              0x1c
#define      NVIC_INT_SETEN                0x100
#define      NVIC_INT_CLREN                0x180
#define      NVIC_INT_SETPEND              0x200
#define      NVIC_INT_CLRPEND              0x280
#define      NVIC_INT_ACTST                0x380

#define      NVIC_INT_PRI0                 0x400
#define      NVIC_INT_PRI1                 0x404
#define      NVIC_INT_PRI2                 0x408
#define      NVIC_INT_PRI3                 0x40c
#define      NVIC_INT_PRI4                 0x410
#define      NVIC_INT_PRI5                 0x414
#define      NVIC_INT_PRI6                 0x418
#define      NVIC_INT_PRI7                 0x41c
#define      NVIC_INT_PRI8                 0x420
#define      NVIC_INT_PRI9                 0x424
#define      NVIC_INT_PRI10                0x428
#define      NVIC_INT_PRI11                0x42c
#define      NVIC_INT_PRI12                0x430
#define      NVIC_INT_PRI13                0x434
#define      NVIC_INT_PRI14                0x438
#define      NVIC_INT_PRI15                0x43c

#define      NVIC_ICSR                     0xd04
#define      NVIC_VTOR                     0xd08
#define      NVIC_AIRCR                    0xd0c
#define      NVIC_SYSCTL                   0xd10
#define      NVIC_CFGCTL                   0xd14


///////////////////////////////////////////
//       ARM PASS OR FAIL      
///////////////////////////////////////////
#define      ARM_FAILED                    0x02013f04
#define      ARM_SUCESS                    0x02013f08
#define      ARM_DABORT                    0x02013f14
#define      ARM_PABORT                    0x02013f64
#define      ARM_UNDEF                     0x02013f74
#define      ARM_EXP                       0x02013f84
#define      ARM_HARD_FAULT_EXP            0x02013f24
#define      ARM_MEM_MANAGE_EXP            0x02013f34
#define      ARM_BUS_FAULT_EXP             0x02013f44
#define      ARM_USAGE_FAULT_EXP           0x02013f54
#endif
/*
********************************************************************************
*
*                         End of Hw_memap.h
*
********************************************************************************
*/

#endif
