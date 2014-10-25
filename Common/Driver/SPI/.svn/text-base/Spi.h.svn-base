/*
********************************************************************************
*                   Copyright (c) 2008,Yangwenjie
*                         All rights reserved.
*
* File Name£º   spi.h
* 
* Description:  define the master/slave register structure bit marco and related definition\interfaces.
*
* History:      <author>          <time>        <version>       
*             yangwenjie      2009-1-15          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _SPI_H_
#define _SPI_H_

#undef  EXT
#ifdef _IN_SPI_
#define EXT
#else
#define EXT extern
#endif
/*
--------------------------------------------------------------------------------
  
                        Macro define  
  
--------------------------------------------------------------------------------
*/
#define SPI_CTL_TX_SPINOR   ( SPI_MASTER_MODE | TRANSMIT_ONLY | MOTOROLA_SPI \
                            | RXD_SAMPLE_NO_DELAY | APB_BYTE_WR \
                            | MSB_FBIT | LITTLE_ENDIAN_MODE | CS_2_SCLK_OUT_1_CK \
                            | CS_KEEP_LOW | SERIAL_CLOCK_POLARITY_HIGH \
                            | SERIAL_CLOCK_PHASE_START | DATA_FRAME_8BIT)

#define SPI_CTL_RX_SPINOR   ( SPI_MASTER_MODE | RECEIVE_ONLY | MOTOROLA_SPI \
                            | RXD_SAMPLE_NO_DELAY | APB_BYTE_WR \
                            | MSB_FBIT | LITTLE_ENDIAN_MODE | CS_2_SCLK_OUT_1_CK \
                            | CS_KEEP_LOW | SERIAL_CLOCK_POLARITY_HIGH \
                            | SERIAL_CLOCK_PHASE_START | DATA_FRAME_8BIT)
                            
#define SPI_CTL_TXRX_SPINOR ( SPI_MASTER_MODE | TRANSMIT_RECEIVE | MOTOROLA_SPI \
                            | RXD_SAMPLE_NO_DELAY | APB_BYTE_WR \
                            | MSB_FBIT | LITTLE_ENDIAN_MODE | CS_2_SCLK_OUT_1_CK \
                            | CS_KEEP_LOW | SERIAL_CLOCK_POLARITY_HIGH \
                            | SERIAL_CLOCK_PHASE_START | DATA_FRAME_8BIT)
/*
--------------------------------------------------------------------------------
  
                        Struct Define
  
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
  
                        Variable Define
  
--------------------------------------------------------------------------------
*/


/*
--------------------------------------------------------------------------------
  
                        Funtion Declaration
  
--------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM

extern void  SPIDeInit(void);
extern int32 SPIWrite(uint8 *pdata, uint32 size);
extern int32 SPIRead(uint8 *pdata, uint32 size, uint32 dumy);

#else

typedef void  (*pSPIDeInit)(void);
typedef int32 (*pSPIWrite)(uint8 *pdata, uint32 size);
typedef int32 (*pSPIRead)(uint8 *pdata, uint32 size, uint32 dumy);

#define SPIDeInit()                 (((pSPIDeInit)(Addr_SPIDeInit))())
#define SPIWrite(pdata, size)       (((pSPIWrite )(Addr_SPIWrite ))(pdata, size))
#define SPIRead(pdata, size, dumy)  (((pSPIRead  )(Addr_SPIRead  ))(pdata, size, dumy))

#endif

extern int32 SPIInit(uint32 ch, uint32 baudRate, uint32 CtrMode);
extern int32 SPIDmaWrite(uint8 *pdata, uint32 size, pFunc CallBack);
extern int32 SPIDmaRead(uint8 *pdata, uint32 size, pFunc CallBack);

/*
********************************************************************************
*
*                         End of spi.h
*
********************************************************************************
*/
#endif
