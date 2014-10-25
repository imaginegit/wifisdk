/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name：   i2s.c
* 
* Description:  C program template
*
* History:      <author>          <time>        <version>       
*             yangwenjie      2009-1-14         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_I2S_

#include "DriverInclude.h"


int32 I2SInit1(I2S_mode_t mode)
{
    uint32 config;
    uint32 timeout = 20000;
    // clock gate&reset, iomux
    
    // modify I2S XFER, disable i2s
    I2s_Reg->I2S_XFER = (TX_transfer_stop |RX_transfer_stop);
    
    // modify I2S_CLR to reset i2s
    // wait until I2S_CLR become 0
    I2s_Reg->I2S_CLR= CLR_TXC_cleard;
    while(I2s_Reg->I2S_CLR)
    {
	    if (--timeout == 0)
	    {
            break;
	    }
    }

    if(mode == I2S_SLAVE_MODE)
    {
        I2s_Reg->I2S_CKR  = ((CKR_TSD_Value)                   //Fsclk/Ftxlrck TSD=187
                             |(CKR_RSD_Value)                   //Fsclk/Frxlrck RSD=124
                             |(CKR_MDIV_Value)                  //MDIV=6M  Fmclk/Ftxsck-1,panda_FPGA:MCLK=12M
                             |(CKR_TLP_oppsite)                  //rx/tx lrck polarity
                             |(CKR_RLP_normal)
                             |(CKR_CKP_posedge)                 //rx/tx sclk polarity 
                             |(CKR_MSS_slave)                   //slave mode
                             ); 
    }
    else
    {
        I2s_Reg->I2S_CKR  = ((CKR_TSD_Value)                   //Fsclk/Ftxlrck TSD=187
                             |(CKR_RSD_Value)                   //Fsclk/Frxlrck RSD=124
                             |(CKR_MDIV_Value)                  //MDIV=6M  Fmclk/Ftxsck-1,panda_FPGA:MCLK=12M
                             |(CKR_TLP_normal)                  //rx/tx lrck polarity
                             |(CKR_RLP_normal)
                             |(CKR_CKP_posedge)                 //rx/tx sclk polarity 
                             ); 
    }
    
    // modify I2S_TXCR, I2S_TXCKR to config parameter
    I2s_Reg->I2S_TXCR = ((Valid_Data_width16)            	//Valid data width
                         |(TXCR_TFS_I2S)            		//Tranfer format select, 0:I2S, 1:PCM
                         |(TXCR_PCM_no_delay)            	//PCM bus mode
                         |(TXCR_I2S_normal)            		//I2S bus mode
                         |(TXCR_FBM_MSB)           		    //First Bit Mode
                         |(TXCR_SJM_right_justified)        //store justified mode,0:right,1:left
                         |(TXCR_HWT_Bit16)           		//halfword word transform
                         |(TXCR_Channel_0_Enable) 		    //channel select register
                         |(0<<17)						    //RCNT 
                         );
	
    I2s_Reg->I2S_RXCR = ((Valid_Data_width16)            	//Valid data width
                         |(RXCR_TFS_I2S)            		//Tranfer format select, 0:I2S, 1:PCM
                         |(RXCR_PCM_no_delay)            	//PCM bus mode
                         |(RXCR_I2S_normal)            		//I2S bus mode
                         |(RXCR_FBM_MSB)           		    //First Bit Mode
                         |(RXCR_SJM_right_justified)        //store justified mode,0:right,1:left
                         |(RXCR_HWT_Bit16) 			        //halfword word transform
                         );
    
   I2s_Reg->I2S_INTCR = ((INTCR_TX_empty_interrupt_disabled)        // tx empty interrupt. 0:disable,1:enable
                         |(INTCR_TX_underrun_interrupt_disabled)    // tx under run int enable.0:disable,1:enable
                         |(0<<2)                                    // write 1 to clear TX underrun int
                         |(0<<4)                                    // transmit FIFO threshold
                         |(INTCR_RX_full_interrupt_disabled)        // RX Full int
                         |(INTCR_RX_overrun_interrupt_disabled)     // RX overrun
                         |(0<<18)                                   // write 1 to clear RX overrun
                         |(0x1f<<20)                                // Reveive FIFO threshold
                         );

    return 0;
}


/*
--------------------------------------------------------------------------------
  Function name : void DataPortMuxSet(eDataPortIOMux_t data)
  Author        : anzhiguo
  Description   : 
                  
  Input         : 
                  
  Return        : 

  History:     <author>         <time>         <version>       
             anzhiguo     2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void I2sCs(eI2sCs_t data)
{
    Grf->IOMUX_CON1 = (0x00040000 | (data << 2));
}

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : void I2sStart()
  Author        : yangwenjie
  Description   : start i2s transfer
                  
  Input         : direction，
                            TX:  send to codec
                            RX:  input to inside
  Return        : NULL

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
int32 I2SStart(I2S_Start_t txRx)
{
    // modify I2S_DMACR to enable DMA
    // modify I2S_XFER to enable i2s
    // enable/disable/clear interrupt by modify I2S_INTCR&I2S_INTSR
    // read I2S_FIFOLR to get FIFO status
    if(txRx == I2S_START_TX)
    {
        I2s_Reg->I2S_DMACR |= (0x10 << 0)               //transmit data level
                             |(DMA_Transmit_enabled)    //transmit data enable
                             ;
        I2s_Reg->I2S_XFER |= TX_transfer_start;
    }
    else
    {
        I2s_Reg->I2S_DMACR |= (0x10<<16)                //receive data level
                             |(DMA_Receive_enabled)     //receive data enable
                             ;
        I2s_Reg->I2S_XFER |= RX_transfer_start;
    }

    return 0;
}

/*
--------------------------------------------------------------------------------
  Function name : I2sStop(void)
  Author        : yangwenjie
  Description   : stop transfer
                  
  Input         : 
                           
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void I2SStop(I2S_Start_t txRx)
{
    if(txRx == I2S_START_TX)
    {
        I2s_Reg->I2S_DMACR &= DMA_Transmit_disabled;
        I2s_Reg->I2S_XFER  &= TX_transfer_stop;
    }
    else
    {
        I2s_Reg->I2S_DMACR &= DMA_Receive_disabled;
        I2s_Reg->I2S_XFER  &= RX_transfer_stop;
    }
}
/*----------------------------------------------------------------------
Name      : I2SInit(I2S_mode_t mode, I2S_Start_t TxOrRx)
Desc      : initializatin
Params    : mode：I2S_MASTER_MODE or I2S_SLAVE_MODE master or slave
Return    : 0 ok，-1 fail。
            fs(kHz)\Fmclk(MHz)	    12	    24.576	12.288	8.192	11.2896
                            8	    1500	3072	1536	1024	--
                            11.025	1088*	--	    --	    --	    1024
                            12	    1000	2048	1024	--	    --
                            16	    750	    1536	768	    512	    --
                            22.05	544*	--	    --	    --	    512
                            24	    500	    1024	512	    --	    --
                            32	    375	    768	    384	    256	    --
                            44.1	272*	--	    --	    --	    256
                            48	    250	    512	    256	    --	    --
                            96	    125	    256	    128	    --	    --
                            192	    62.5	128	    64	    --	    --
                            表格 7  Fmclk与Flrck比值表
            Fs=Fmclk/(RMD*RSD);
            Fs=Fmclk/(TMD*TSD);
            Fmclk is the frequency of mclk 
            TMD(RMD) is divider rato of mclk to sclk,TMD(RMD) = Fmclk/Fsclk,TMD(RMD) is even from 2 to 64.
            TSD(RSD) is divider rato of sclk to lrck,TSD(RSD) = Fsclk/Flrck,TSD(RSD) is any number from 32 to 287.
            for keep 32 bit transfer,so the max value of TSD/4 and RSD is needed to bigger than 64(2X32bit).
            the range of TMD(RMD) is get from this,so the smallest drivider is 48.

----------------------------------------------------------------------*/
_ATTR_DRIVERLIB_CODE_
int32 I2SInit(I2S_mode_t mode)
{
    uint32 config;
    uint32 timeout = 20000;
    // clock gate&reset, iomux
    
    // modify I2S XFER, disable i2s
    I2s_Reg->I2S_XFER = (TX_transfer_stop |RX_transfer_stop);
    
    // modify I2S_CLR to reset i2s
    // wait until I2S_CLR become 0
    I2s_Reg->I2S_CLR= CLR_TXC_cleard;
    while(I2s_Reg->I2S_CLR)
    {
	    if (--timeout == 0)
	    {
            break;
	    }
    }

    if(mode == I2S_SLAVE_MODE)
    {
        I2s_Reg->I2S_CKR  = ((CKR_TSD_Value)                   //Fsclk/Ftxlrck TSD=187
                             |(CKR_RSD_Value)                   //Fsclk/Frxlrck RSD=124
                             |(CKR_MDIV_Value)                  //MDIV=6M  Fmclk/Ftxsck-1,panda_FPGA:MCLK=12M
                             |(CKR_TLP_normal)                  //rx/tx lrck polarity
                             |(CKR_RLP_normal)
                             |(CKR_CKP_posedge)                 //rx/tx sclk polarity 
                             |(CKR_MSS_slave)                   //slave mode
                             ); 
    }
    else
    {
        I2s_Reg->I2S_CKR  = ((CKR_TSD_Value)                   //Fsclk/Ftxlrck TSD=187
                             |(CKR_RSD_Value)                   //Fsclk/Frxlrck RSD=124
                             |(CKR_MDIV_Value)                  //MDIV=6M  Fmclk/Ftxsck-1,panda_FPGA:MCLK=12M
                             |(CKR_TLP_normal)                  //rx/tx lrck polarity
                             |(CKR_RLP_normal)
                             |(CKR_CKP_posedge)                 //rx/tx sclk polarity 
                             ); 
    }
    
    // modify I2S_TXCR, I2S_TXCKR to config parameter
    I2s_Reg->I2S_TXCR = ((Valid_Data_width16)            	//Valid data width
                         |(TXCR_TFS_I2S)            		//Tranfer format select, 0:I2S, 1:PCM
                         |(TXCR_PCM_no_delay)            	//PCM bus mode
                         |(TXCR_I2S_normal)            		//I2S bus mode
                         |(TXCR_FBM_MSB)           		    //First Bit Mode
                         |(TXCR_SJM_right_justified)        //store justified mode,0:right,1:left
                         |(TXCR_HWT_Bit16)           		//halfword word transform
                         |(TXCR_Channel_0_Enable) 		    //channel select register
                         |(0<<17)						    //RCNT 
                         );
	
    I2s_Reg->I2S_RXCR = ((Valid_Data_width16)            	//Valid data width
                         |(RXCR_TFS_I2S)            		//Tranfer format select, 0:I2S, 1:PCM
                         |(RXCR_PCM_no_delay)            	//PCM bus mode
                         |(RXCR_I2S_normal)            		//I2S bus mode
                         |(RXCR_FBM_MSB)           		    //First Bit Mode
                         |(RXCR_SJM_right_justified)        //store justified mode,0:right,1:left
                         |(RXCR_HWT_Bit16) 			        //halfword word transform
                         );
    
   I2s_Reg->I2S_INTCR = ((INTCR_TX_empty_interrupt_disabled)        // tx empty interrupt. 0:disable,1:enable
                         |(INTCR_TX_underrun_interrupt_disabled)    // tx under run int enable.0:disable,1:enable
                         |(0<<2)                                    // write 1 to clear TX underrun int
                         |(0<<4)                                    // transmit FIFO threshold
                         |(INTCR_RX_full_interrupt_disabled)        // RX Full int
                         |(INTCR_RX_overrun_interrupt_disabled)     // RX overrun
                         |(0<<18)                                   // write 1 to clear RX overrun
                         |(0x1f<<20)                                // Reveive FIFO threshold
                         );

    return 0;
}

/*
--------------------------------------------------------------------------------
  Function name : I2S_PowerOnInit(I2S_mode_t mode)
  Author        : yangwenjie
  Description   : I2S auti-initial
                  
  Input         : NULL
                           
  Return        : NULL

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  note:        if do not use i2s device,for example,using internal codec,
  			   must call this function after power on.
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void I2SDeInit(void)
{

    I2s_Reg->I2S_XFER = 0;

//    #if(BOARDTYPE == CK610_FPGA)

 //   #else
//        SCUDisableClk(CLK_GATE_I2S0);
 //   #endif
}
#endif
/*
********************************************************************************
*
*                         End of i2s.c
*
********************************************************************************
*/
