/*
********************************************************************************
*                   Copyright (c) 2008,Yangwenjie
*                         All rights reserved.
*
* File Name£º   i2s.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             yangwenjie      2009-1-14          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _I2S_H_
#define _I2S_H_

#undef  EXT
#ifdef _IN_I2S_
#define EXT
#else
#define EXT extern
#endif


/*
--------------------------------------------------------------------------------
  
                        Struct Define
  
--------------------------------------------------------------------------------
*/

typedef enum
{
    I2S_START_RX = 0,
    I2S_START_TX
}I2S_Start_t;

typedef enum
{
    I2S_NORMAL_MODE = 0,
    I2S_LEFT_MODE
}I2S_BUS_MODE_t;

typedef enum
{
    PCM_EARLY_MODE = 0,
    PCM_LATE_MODE
}PCM_BUS_MODE_t;

typedef enum
{
    I2S_BUS_MODE = 0,
    PCM_BUS_MODE
}I2S_PCM_SEL_t;


typedef enum
{   
    I2S_SLAVE_MODE = 0,
    I2S_MASTER_MODE,
    I2S_MASTER_MODE_LOCK,
    I2S_MASTER_MODE_UNLOCK
}I2S_mode_t;

typedef enum
{   
    I2S_EXT = 0,
    I2S_IN  = 1,
    
}eI2sCs_t;

/*
--------------------------------------------------------------------------------
  
                        Funtion Declaration
  
--------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM

extern int32 I2SStart(I2S_Start_t TxOrRx);
extern void  I2SStop(I2S_Start_t TxOrRx);
extern int32 I2SInit(I2S_mode_t mode);
extern void  I2SDeInit(void);

#else

typedef int32 (*pI2SStart)(I2S_Start_t TxOrRx);
typedef void  (*pI2SStop)(I2S_Start_t TxOrRx);
typedef int32 (*pI2SInit)(I2S_mode_t mode);
typedef void  (*pI2SDeInit)(void);

#define I2SStart(TxOrRx) (((pI2SStart )(Addr_I2SStart ))(TxOrRx))
#define I2SStop(TxOrRx)  (((pI2SStop  )(Addr_I2SStop  ))(TxOrRx))
#define I2SInit(mode)    (((pI2SInit  )(Addr_I2SInit  ))(mode))
#define I2SDeInit()      (((pI2SDeInit)(Addr_I2SDeInit))())

#endif

extern void I2sCs(eI2sCs_t data);
extern int32 I2SInit1(I2S_mode_t mode);
/*
********************************************************************************
*
*                         End of i2s.h
*
********************************************************************************
*/
#endif
