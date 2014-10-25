/******************************************************************/
/*   Copyright (C) 2008 ROCK-CHIPS FUZHOU . All Rights Reserved.  */
/*******************************************************************
File    :  api_uart.h
Desc    :  UART interface parmeters.

Author  : lhh
Date    : 2008-11-10
Modified:
Revision:           1.00
$Log: api_uart.h,v $
*********************************************************************/

#ifndef _UART_H_
#define _UART_H_

#undef  EXT
#ifdef  _IN_UART_
#define EXT
#else
#define EXT extern
#endif

/*
--------------------------------------------------------------------------------
  
                        Macro define  
  
--------------------------------------------------------------------------------
*/
///uartWorkStatusFlag
#define   UART0_TX_WORK                     (1)
#define   UART0_RX_WORK                     (1<<1)
#define   UART1_TX_WORK                     (1<<2)
#define   UART1_RX_WORK                     (1<<3)
#define   UART2_TX_WORK                     (1<<5)
#define   UART2_RX_WORK                     (1<<6)
#define   UART3_TX_WORK                     (1<<7)
#define   UART3_RX_WORK                     (1<<8)
#define   UART_ERR_RX                       (1<<4)

/*
--------------------------------------------------------------------------------
  
                        Struct Define
  
--------------------------------------------------------------------------------
*/
typedef enum UART_ch
{
    UART_CH0,
    UART_CH1,
    UART_MAX
}eUART_ch_t;

typedef enum UART_BaudRate
{
    UART_BR_110    = 110,
    UART_BR_300    = 300,
    UART_BR_600    = 600,
    UART_BR_1200   = 1200,
    UART_BR_2400   = 2400,
    UART_BR_4800   = 4800,
    UART_BR_9600   = 9600,
    UART_BR_14400  = 14400,
    UART_BR_19200  = 19200,
    UART_BR_38400  = 38400,
    UART_BR_57600  = 57600,
    UART_BR_115200 = 115200,
    UART_BR_230400 = 230400,
    UART_BR_380400 = 380400,
    UART_BR_460800 = 460800,
    UART_BR_921600 = 921600
}eUART_BaudRate;

typedef enum UART_dataLen
{
    UART_DATA_5B = 5,
    UART_DATA_6B,
    UART_DATA_7B,
    UART_DATA_8B
}eUART_dataLen;

typedef enum UART_stopBit
{
    UART_ONE_STOPBIT,
    UART_ONE_AND_HALF_OR_TWO_STOPBIT
}eUART_stopBit;

typedef enum UART_parity_en
{
    UART_ODD_PARITY,
    UART_EVEN_PARITY,
    UART_PARITY_DISABLE
}eUART_parity_en;
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

extern void  UARTRest(void);
extern int32 UARTSetIOP(uint8 useIrDA);
extern int32 UARTSetBaudRate(uint32 clk, uint32 baudRate);
extern int32 UARTSetLcrReg(uint8 byteSize, uint8 parity,uint8 stopBits );
extern void  UARTSetFifoEnabledNumb(uint32 param);
extern void  UARTSetIntEnabled(uint32 uartIntNumb);
extern void  UARTSetIntDisabled(uint32 uartIntNumb);
extern int32 UARTWriteByte(uint8 byte, uint32 uartTimeOut);
extern int32 UARTReadByte(uint8 *pdata, uint32 uartTimeOut);
extern int32 UARTInit(eUART_BaudRate baudRate,eUART_dataLen dataBit,eUART_stopBit stopBit,eUART_parity_en parity);

#else

typedef void  (*pUARTRest)(void);
typedef int32 (*pUARTSetIOP)(uint8 useIrDA);
typedef int32 (*pUARTSetBaudRate)(uint32 clk, uint32 baudRate);
typedef int32 (*pUARTSetLcrReg)(uint8 byteSize, uint8 parity,uint8 stopBits );
typedef void  (*pUARTSetFifoEnabledNumb)(uint32 param);
typedef void  (*pUARTSetIntEnabled)(uint32 uartIntNumb);
typedef void  (*pUARTSetIntDisabled)(uint32 uartIntNumb);
typedef int32 (*pUARTWriteByte)(uint8 byte, uint32 uartTimeOut);
typedef int32 (*pUARTReadByte)(uint8 *pdata, uint32 uartTimeOut);
typedef int32 (*pUARTInit)(eUART_BaudRate baudRate,eUART_dataLen dataBit,eUART_stopBit stopBit,eUART_parity_en parity);

#define UARTRest()                                   (((pUARTRest              )(Addr_UARTRest              ))())
#define UARTSetIOP(useIrDA)                          (((pUARTSetIOP            )(Addr_UARTSetIOP            ))(useIrDA))
#define UARTSetBaudRate(clk, baudRate)               (((pUARTSetBaudRate       )(Addr_UARTSetBaudRate       ))(clk, baudRate))
#define UARTSetLcrReg(byteSize, parity, stopBits)    (((pUARTSetLcrReg         )(Addr_UARTSetLcrReg         ))(byteSize, parity, stopBits))
#define UARTSetFifoEnabledNumb(param)                (((pUARTSetFifoEnabledNumb)(Addr_UARTSetFifoEnabledNumb))(param))
#define UARTSetIntEnabled(uartIntNumb)               (((pUARTSetIntEnabled     )(Addr_UARTSetIntEnabled     ))(uartIntNumb))
#define UARTSetIntDisabled(uartIntNumb)              (((pUARTSetIntDisabled    )(Addr_UARTSetIntDisabled    ))(uartIntNumb))
#define UARTWriteByte(byte, uartTimeOut)             (((pUARTWriteByte         )(Addr_UARTWriteByte         ))(byte, uartTimeOut))
#define UARTReadByte(pdata, uartTimeOut)             (((pUARTReadByte          )(Addr_UARTReadByte          ))(pdata, uartTimeOut))
#define UARTInit(baudRate, dataBit, stopBit, parity) (((pUARTInit              )(Addr_UARTInit              ))(baudRate, dataBit, stopBit, parity))

#endif

extern int32 UartDmaWrite(uint8 *pdata, uint32 size, uint32 uartTimeOut, pFunc Callback);
extern int32 UartDmaRead(uint8 *pdata, uint32 size, uint32 uartTimeOut, pFunc Callback);

/*
********************************************************************************
*
*                         End of uart.h
*
********************************************************************************
*/
#endif

