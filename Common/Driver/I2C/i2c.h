/*
********************************************************************************
*                   Copyright (c) 2008,yangwenjie
*                         All rights reserved.
*
* File Name£º   I2c.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             yangwenjie      2009-1-14          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _I2C_H_
#define _I2C_H_

#undef  EXT
#ifdef _IN_I2C_
#define EXT
#else
#define EXT extern
#endif

/*
--------------------------------------------------------------------------------
  
                        Macro define  
  
--------------------------------------------------------------------------------
*/
// Interrupt status bit
#define I2C_INT_MACK            ((UINT32)0x00000001 << 0)   //Master receives ACK interrupt status bit
#define I2C_INT_MACKP           ((UINT32)0x00000001 << 1)   //Master ACK period interrupt status bit
#define I2C_INT_SACK            ((UINT32)0x00000001 << 2)   //Slave receives ACK interrupt status bit
#define I2C_INT_SACKP           ((UINT32)0x00000001 << 3)   //Slave ACK period interrupt status bit
#define I2C_INT_SAM             ((UINT32)0x00000001 << 4)   //Slave address matches status bit
#define I2C_INT_SBA             ((UINT32)0x00000001 << 5)   //Broadcast address 
#define I2C_INT_SAS             ((UINT32)0x00000001 << 6)   //Abnormal stop occurs
#define I2C_INT_AL              ((UINT32)0x00000001 << 7)   //Arbitration Lost

// I2C operation bit
#define I2C_OPR_DIV_SPEED_MASK   0x3F
#define I2C_OPR_ENABLE          ((UINT32)0x00000001 << 6)      //I2C core enable bit
#define I2C_OPR_RESET           ((UINT32)0x00000001 << 7)      //I2C state machine (both master/slave) reset bit
#define I2C_OPR_S10ADDR         ((UINT32)0x00000001 << 8)       //Slave 10 bits address mode
#define I2C_OPR_S7ADDR          ((UINT32)0x00000000 << 8)       //Slave 10 bits address mode

//I2C control register
#define I2C_LCMR_START          ((UINT32)0x00000001 << 0)
#define I2C_LCMR_STOP           ((UINT32)0x00000001 << 1)
#define I2C_LCMR_RESUME         ((UINT32)0x00000001 << 2)

//I2C line status register
#define I2C_LSR_BUSY            ((UINT32)0x00000001 << 0)   //I2C core busy status bit
#define I2C_LSR_NACK            ((UINT32)0x00000001 << 1)   //I2C receives NACK status bit

// I2C operation mode bit
#define I2C_CON_MASTER_TX       ((UINT32)0x00000003 << 2)
#define I2C_CON_MASTER_RX       ((UINT32)0x00000001 << 2)
#define I2C_CON_ACK             ((UINT32)0x00000000 << 4)   //I2C bus acknowledge enable register
#define I2C_CON_NACK            ((UINT32)0x00000001 << 4)   //I2C bus acknowledge enable register
#define I2C_CONR_MASTER_MASK    ((UINT32)0x00000003 << 2)
#define I2C_CONR_MRX            ((UINT32)0x00000001 << 2)  //Master receive mode
#define I2C_CONR_MTX            ((UINT32)0x00000003 << 2)  //Master transmit mode

#define I2C_CON_MASTER_TX       ((UINT32)0x00000003 << 2)
#define I2C_CON_MASTER_RX       ((UINT32)0x00000001 << 2)

#define I2C_READ_BIT            (1)
#define I2C_WRITE_BIT           (0)

#define I2C_10ADDR_PREFIX       (0x1e)
/*
--------------------------------------------------------------------------------
  
                        Struct Define
  
--------------------------------------------------------------------------------
*/
 typedef enum I2C_slaveaddr
{
    tvp5145 = 0x5C,
    WM8987codec = 0x34,
    RDA5820AD = 0x22,
    FM5767 = 0xC0,
    FM5800 = 0x20,
    RTCM41 = 0xD0,
    InterCodec = 0x4E,
    AR1010 = 0x20,
    FM5802 = 0x20
}eI2C_slaveaddr_t;

typedef enum I2C_mode
{
    NormalMode,
    DirectMode,
    RDA_5820Mode
}eI2C_mode_t;

/*
--------------------------------------------------------------------------------
  
                        Variable Define
  
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_BSS_ EXT UINT8 I2C_slaveaddr ;
_ATTR_DRIVER_BSS_ EXT UINT16 I2C_oldspeed ;

/*
--------------------------------------------------------------------------------
  
                        Funtion Declaration
  
--------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM

extern void  I2CStart(void);
extern void  I2CStop(void);
extern int32 I2CSendData(UINT8 Data, BOOL StartBit);
extern int32 I2CReadData(UINT8 *Data);

#else

typedef void  (*pI2CStart)(void);
typedef void  (*pI2CStop)(void);
typedef int32 (*pI2CSendData)(UINT8 Data, BOOL StartBit);
typedef int32 (*pI2CReadData)(UINT8 *Data);

#define I2CStart()                  (((pI2CStart   )(Addr_I2CStart   ))())
#define I2CStop()                   (((pI2CStop    )(Addr_I2CStop    ))())
#define I2CSendData(Data, StartBit) (((pI2CSendData)(Addr_I2CSendData))(Data, StartBit))
#define I2CReadData(Data)           (((pI2CReadData)(Addr_I2CReadData))(Data))

#endif

extern BOOL I2C_Init(UINT8 SlaveAddress, UINT16 nKHz);
extern BOOL I2CSetSpeed(UINT16 speed);
extern BOOL I2C_Write(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode);
extern BOOL I2C_Read(UINT8 RegAddr, UINT8 *pData, UINT16 size, eI2C_mode_t mode);
extern BOOL I2C_Deinit(void);

/*
********************************************************************************
*
*                         End of Example.h
*
********************************************************************************
*/
#endif
