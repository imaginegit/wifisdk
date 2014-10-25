/*
********************************************************************************
*                   Copyright (c) 2008,anzhiguo
*                         All rights reserved.
*
* File Name��  Gpio.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             anzhiguo      2009-1-14          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _GPIO_H_
#define _GPIO_H_

#undef  EXT
#ifdef  _IN_GPIO_
#define EXT
#else
#define EXT extern
#endif

/*
--------------------------------------------------------------------------------
  
                ENUMERATIONS AND STRUCTURES
  
--------------------------------------------------------------------------------
*/
typedef enum eGPIOPinLevel
{
    GPIO_LOW,
    GPIO_HIGH
    
}eGPIOPinLevel_t;

typedef enum eGPIOPinDirection
{
    GPIO_IN,
    GPIO_OUT
    
}eGPIOPinDirection_t;

typedef enum eGPIOPinNum
{
    GPIOPortA_Pin0,
    GPIOPortA_Pin1,
    GPIOPortA_Pin2,
    GPIOPortA_Pin3,
    GPIOPortA_Pin4,
    GPIOPortA_Pin5,
    GPIOPortA_Pin6,
    GPIOPortA_Pin7,
    GPIOPortB_Pin0,
    GPIOPortB_Pin1,
    GPIOPortB_Pin2,
    GPIOPortB_Pin3,
    GPIOPortB_Pin4,
    GPIOPortB_Pin5,
    GPIOPortB_Pin6,
    GPIOPortB_Pin7,
    GPIOPortC_Pin0,
    GPIOPortC_Pin1,
    GPIOPortC_Pin2,
    GPIOPortC_Pin3,
    GPIOPortC_Pin4,
    GPIOPortC_Pin5,
    GPIOPortC_Pin6,
    GPIOPortC_Pin7,
    GPIOPortD_Pin0,
    GPIOPortD_Pin1,
    GPIOPortD_Pin2,
    GPIOPortD_Pin3,
    GPIOPortD_Pin4,
    GPIOPortD_Pin5,
    GPIOPortD_Pin6,
    GPIOPortD_Pin7
    
}eGPIOPinNum_t;

// Constants for gpio interrupt type definition
typedef enum GPIOIntrType
{
    IntrTypeRisingEdge,      // Rising edge triggered
    IntrTypeFallingEdge,      // Falling edge triggered
    IntrTypeHighLevel,         // Level high triggered
    IntrTypeLowLevel         // Level low triggered
    
} GPIOIntrType_c;

typedef enum eGPIOPinIOMux
{
    Type_Gpio = 0x0ul,  
    Type_Mux1 = 0x1ul,  
    Type_Mux2 = 0x2ul,
    Type_Mux3 = 0x3ul,
    
} eGPIOPinIOMux_t;

typedef enum eGPIOPinPull
{
    ENABLE = ((uint32)(0)),  
    DISABLE,
    
} eGPIOPinPull_t;

typedef enum eDataPortIOMux
{
    IO_FLASH  = 0x0000ul,  
    IO_REV    = 0x5555ul,  
    IO_LCD    = 0xaaaaul,
    IO_SDMMC  = 0xfffful,
    
} eDataPortIOMux_t;

typedef enum eSDMMCPortIOMux
{
    SDMMC_PORT0  = 0,  
    SDMMC_PORT1  = 1,      
    
} eSDMMCPortIOMux_t;

typedef enum eSPIPortIOMux
{
    SPI_PORT0  = 0,  
    SPI_PORT1  = 2,      
    
} eSPIPortIOMux_t;

/*
--------------------------------------------------------------------------------
  
                iomux define macro define
  
--------------------------------------------------------------------------------
*/
//
#define IOMUX_GPIOA0_IO                 ((uint32)(0))
#define IOMUX_GPIOA0_SDMMC_PWREN        ((uint32)(1))
#define IOMUX_GPIOA0_DUMY2              ((uint32)(2))
#define IOMUX_GPIOA0_FLAS_CS0           ((uint32)(3))

#define IOMUX_GPIOA1_IO                 ((uint32)(0))
#define IOMUX_GPIOA1_DUMY1              ((uint32)(1))
#define IOMUX_GPIOA1_DUMY2              ((uint32)(2))
#define IOMUX_GPIOA1_FLAS_CS1           ((uint32)(3))

#define IOMUX_GPIOA2_IO                 ((uint32)(0))
#define IOMUX_GPIOA2_DUMY1              ((uint32)(1))
#define IOMUX_GPIOA2_LCDRS              ((uint32)(2))
#define IOMUX_GPIOA2_FLAS_ALE           ((uint32)(3))

#define IOMUX_GPIOA3_IO                 ((uint32)(0))
#define IOMUX_GPIOA3_SDMMC_CCLKP1       ((uint32)(1))
#define IOMUX_GPIOA3_SPI_RXDP0          ((uint32)(2))
#define IOMUX_GPIOA3_FLAS_RDY           ((uint32)(3))

#define IOMUX_GPIOA4_IO                 ((uint32)(0))
#define IOMUX_GPIOA4_SDMMC_RSTNP1       ((uint32)(1))
#define IOMUX_GPIOA4_SPI_TXDP0          ((uint32)(2))
#define IOMUX_GPIOA4_FLAS_RDN           ((uint32)(3))

#define IOMUX_GPIOA5_IO                 ((uint32)(0))
#define IOMUX_GPIOA5_DUMY1              ((uint32)(1))
#define IOMUX_GPIOA5_SPI_CLKP0          ((uint32)(2))
#define IOMUX_GPIOA5_FLAS_WRN           ((uint32)(3))

#define IOMUX_GPIOA6_IO                 ((uint32)(0))
#define IOMUX_GPIOA6_SDMMC_CMDP1        ((uint32)(1))
#define IOMUX_GPIOA6_SPI_CSN0           ((uint32)(2))
#define IOMUX_GPIOA6_FLAS_CLE           ((uint32)(3))

#define IOMUX_GPIOA7_IO                 ((uint32)(0))
#define IOMUX_GPIOA7_DUMY1              ((uint32)(1))
#define IOMUX_GPIOA7_DUMY2              ((uint32)(2))
#define IOMUX_GPIOA7_LCD_CSN            ((uint32)(3))

#define IOMUX_GPIOB0_IO                 ((uint32)(0))
#define IOMUX_GPIOB0_LCD_WRN            ((uint32)(1))

#define IOMUX_GPIOB1_IO                 ((uint32)(0))
#define IOMUX_GPIOB1_SPI_CSN1           ((uint32)(1))

#define IOMUX_GPIOB2_IO                 ((uint32)(0))
#define IOMUX_GPIOB2_SPI_CLKP1          ((uint32)(1))

#define IOMUX_GPIOB3_IO                 ((uint32)(0))
#define IOMUX_GPIOB3_SPI_TXDP1          ((uint32)(1))

#define IOMUX_GPIOB4_IO                 ((uint32)(0))
#define IOMUX_GPIOB4_SPI_RXDP1          ((uint32)(1))

#define IOMUX_GPIOB5_IO                 ((uint32)(0))
#define IOMUX_GPIOB5_UART_TXD           ((uint32)(1))

#define IOMUX_GPIOB6_IO                 ((uint32)(0))
#define IOMUX_GPIOB6_UART_RXD           ((uint32)(1))

#define IOMUX_GPIOB7_IO                 ((uint32)(0))
#define IOMUX_GPIOB7_UART_CTS           ((uint32)(1))

#define IOMUX_GPIOC0_IO                 ((uint32)(0))
#define IOMUX_GPIOC0_UART_SIR_I         ((uint32)(2))
#define IOMUX_GPIOC0_UART_RTS           ((uint32)(3))

#define IOMUX_GPIOC1_IO                 ((uint32)(0))
#define IOMUX_GPIOC1_SDMMC_CMDP0        ((uint32)(1))

#define IOMUX_GPIOC2_IO                 ((uint32)(0))
#define IOMUX_GPIOC2_I2S_CLK            ((uint32)(2))
#define IOMUX_GPIOC2_SDMMC_CLKP0        ((uint32)(3))

#define IOMUX_GPIOC3_IO                 ((uint32)(0))
#define IOMUX_GPIOC3_I2S_LRCK           ((uint32)(2))
#define IOMUX_GPIOC3_SDMMC_DATA0P0      ((uint32)(3))

#define IOMUX_GPIOC4_IO                 ((uint32)(0))
#define IOMUX_GPIOC4_I2S_SCLK           ((uint32)(2))
#define IOMUX_GPIOC4_SDMMC_DATA1P0      ((uint32)(3))

#define IOMUX_GPIOC5_IO                 ((uint32)(0))
#define IOMUX_GPIOC5_I2S_SDO            ((uint32)(2))
#define IOMUX_GPIOC5_SDMMC_DATA2P0      ((uint32)(3))

#define IOMUX_GPIOC6_IO                 ((uint32)(0))
#define IOMUX_GPIOC6_I2S_SDI            ((uint32)(2))
#define IOMUX_GPIOC6_SDMMC_DATA3P0      ((uint32)(3))

#define IOMUX_GPIOC7_IO                 ((uint32)(0))
#define IOMUX_GPIOC7_FLASH_CSN2         ((uint32)(2))
#define IOMUX_GPIOC7_I2C_SCL            ((uint32)(3))

#define IOMUX_GPIOD0_IO                 ((uint32)(0))
#define IOMUX_GPIOD0_FLASH_CSN3         ((uint32)(2))
#define IOMUX_GPIOD0_I2C_SDA            ((uint32)(3))

#define IOMUX_GPIOD1_IO                 ((uint32)(0))
#define IOMUX_GPIOD1_PWM0               ((uint32)(1))

#define IOMUX_GPIOD2_IO                 ((uint32)(0))
#define IOMUX_GPIOD2_PWM1               ((uint32)(1))   //PWM2 ?

#define IOMUX_GPIOD3_IO                 ((uint32)(0))
#define IOMUX_GPIOD3_CLK_OBS            ((uint32)(2))
#define IOMUX_GPIOD3_PWM2               ((uint32)(3))

/*
--------------------------------------------------------------------------------
  
                        Funtion Declaration
  
--------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM

extern void   Gpio_SetPinDirection(eGPIOPinNum_t GPIOPinNum, eGPIOPinDirection_t direction);
extern UINT8  Gpio_GetPinDirection(eGPIOPinNum_t GPIOPinNum);
extern void   Gpio_SetPinLevel(eGPIOPinNum_t GPIOPinNum, eGPIOPinLevel_t level);
extern UINT8  Gpio_GetPinLevel(eGPIOPinNum_t GPIOPinNum);
extern void   Gpio_SetIntMode(eGPIOPinNum_t GPIOPinNum, GPIOIntrType_c type);
extern UINT32 Gpio_GetIntMode(eGPIOPinNum_t GPIOPinNum);
extern void   Gpio_ClearInt_Level(eGPIOPinNum_t GPIOPinNum);   //��ƽ�����ж�״̬���
extern void   Gpio_EnableInt(eGPIOPinNum_t GPIOPinNum);
extern void   Gpio_DisableInt(eGPIOPinNum_t GPIOPinNum);

#else

typedef void   (*pGpio_SetPinDirection)(eGPIOPinNum_t GPIOPinNum, eGPIOPinDirection_t direction);
typedef UINT8  (*pGpio_GetPinDirection)(eGPIOPinNum_t GPIOPinNum);
typedef void   (*pGpio_SetPinLevel)(eGPIOPinNum_t GPIOPinNum, eGPIOPinLevel_t level);
typedef UINT8  (*pGpio_GetPinLevel)(eGPIOPinNum_t GPIOPinNum);
typedef void   (*pGpio_SetIntMode)(eGPIOPinNum_t GPIOPinNum, GPIOIntrType_c type);
typedef UINT32 (*pGpio_GetIntMode)(eGPIOPinNum_t GPIOPinNum);
typedef void   (*pGpio_ClearInt_Level)(eGPIOPinNum_t GPIOPinNum);
typedef void   (*pGpio_EnableInt)(eGPIOPinNum_t GPIOPinNum);
typedef void   (*pGpio_DisableInt)(eGPIOPinNum_t GPIOPinNum);

#define Gpio_SetPinDirection(GPIOPinNum, direction) (((pGpio_SetPinDirection)(Addr_Gpio_SetPinDirection))(GPIOPinNum, direction))
#define Gpio_GetPinDirection(GPIOPinNum)            (((pGpio_GetPinDirection)(Addr_Gpio_GetPinDirection))(GPIOPinNum))
#define Gpio_SetPinLevel(GPIOPinNum, level)         (((pGpio_SetPinLevel    )(Addr_Gpio_SetPinLevel    ))(GPIOPinNum, level))
#define Gpio_GetPinLevel(GPIOPinNum)                (((pGpio_GetPinLevel    )(Addr_Gpio_GetPinLevel    ))(GPIOPinNum))
#define Gpio_SetIntMode(GPIOPinNum, type)           (((pGpio_SetIntMode     )(Addr_Gpio_SetIntMode     ))(GPIOPinNum, type))
#define Gpio_GetIntMode(GPIOPinNum)                 (((pGpio_GetIntMode     )(Addr_Gpio_GetIntMode     ))(GPIOPinNum))
#define Gpio_ClearInt_Level(GPIOPinNum)             (((pGpio_ClearInt_Level )(Addr_Gpio_ClearInt_Level ))(GPIOPinNum))
#define Gpio_EnableInt(GPIOPinNum)                  (((pGpio_EnableInt      )(Addr_Gpio_EnableInt      ))(GPIOPinNum))
#define Gpio_DisableInt(GPIOPinNum)                 (((pGpio_DisableInt     )(Addr_Gpio_DisableInt     ))(GPIOPinNum))

#endif

extern void  GpioMuxSet(eGPIOPinNum_t GPIOPinNum, eGPIOPinIOMux_t data);
extern int32 GpioMuxGet(eGPIOPinNum_t GPIOPinNum);
extern void  DataPortMuxSet(eDataPortIOMux_t data);
extern int32 DataPortMuxGet(void);
extern int32 GpioIsrRegister(uint32 pin, pFunc CallBack);
extern int32 GPIOIsrUnRegister(uint32 pin);
extern void  GpioInt(void);


/*
********************************************************************************
*
*                         End of Gpio.h
*
********************************************************************************
*/
#endif

