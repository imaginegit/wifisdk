/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   interrupt.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#undef  EXT
#ifdef _IN_INTTERUPT_
#define EXT
#else
#define EXT extern
#endif

/*
*-------------------------------------------------------------------------------
*  
*                           Macro define
*  
*-------------------------------------------------------------------------------
*/
//中断服务程序类型定义
typedef void(*ExecFunPtr)(void) __irq;

//section define
#define _ATTR_VECTTAB_              __attribute__((section("vect")))
#define _ATTR_INTRRUPT_CODE_        __attribute__((section("SysCode")))
#define _ATTR_INTRRUPT_DATA_        __attribute__((section("SysData")))
#define _ATTR_INTRRUPT_BSS_         __attribute__((section("SysBss"),zero_init))

//中断优先级定义
#define NUM_PRIORITY_BITS           4

/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/
//中断ID号定义
typedef enum 
{
    //Cortex M3 System Intterupt
    FAULT_ID0_REV = (UINT32)0,                                  //SP Point
    FAULT_ID1_REV,                                              //Reset
    FAULT_ID2_NMI,                                              //NMI fault
    FAULT_ID3_HARD,                                             //Hard fault
    FAULT_ID4_MPU,                                              //MPU fault
    FAULT_ID5_BUS,                                              //Bus fault
    FAULT_ID6_USAGE,                                            //Usage fault
    FAULT_ID7_REV,                                              //                       
    FAULT_ID8_REV,                                              //
    FAULT_ID9_REV,                                              //
    FAULT_ID10_REV,                                             //
    FAULT_ID11_SVCALL,                                          //SVCall
    FAULT_ID12_DEBUG,                                           //Debug monitor
    FAULT_ID13_REV,                                             //
    FAULT_ID14_PENDSV,                                          //PendSV
    FAULT_ID15_SYSTICK,                                         //System Tick
    
    //RKNano Process Intterupt
    INT_ID16_DMA,                                               //00 int_dma            
    INT_ID17_UART,                                              //01 uart     
    INT_ID18_SDMMC,                                             //02 sd mmc
    INT_ID19_PWM1,                                              //03 pwm1               
    INT_ID20_PWM2,                                              //04 pwm2    
    INT_ID21_IMDCT36,                                           //05 imdct36           
    INT_ID22_SYNTHESIZE,                                        //06 synthesize        
    INT_ID23_USB,                                               //07 usb   
    INT_ID24_I2C,                                               //08 i2c  
    INT_ID25_I2S,                                               //09 i2s               
    INT_ID26_GPIO,                                              //10 gpio     
    INT_ID27_SPI,                                               //11 spi              
    INT_ID28_PWM0,                                              //12 pwm0               
    INT_ID29_TIMER,                                             //13 timer  
    INT_ID30_ADC,                                               //14 sar-adc           
    INT_ID31_RESERVED,                                          //15 reserved
    INT_ID32_RESERVED,                                          //16 reserved
    INT_ID33_RESERVED,                                          //17 reserved
    INT_ID34_RTC,                                               //18 rtc
    INT_ID35_RESERVED,                                          //19 reserved
    INT_ID36_OOL_BAT_SNSEN,                                     //20 ool_bat_snsen
    INT_ID37_RESERVED,                                          //21 reserved
    INT_ID38_OOL_PAUSE,                                         //22 ool_pause
    INT_ID39_OOL_PLAYON,                                        //23 ool_PlayOn   
    INT_ID40_PWR_5V_READY,                                      //24 pwr_5v_ready    
    INT_ID41_PWR_UVLO_VDD,                                      //25 pwr_uvlo_vdd
    INT_ID42_PWR_UVP,                                           //26 pwr_uvp
    INT_ID43_PWR_OVP,                                           //27 pwr_ovp    
    INT_ID44_PWR_OT,                                            //28 pwr_ot
    INT_ID45_PWR_OC,                                            //29 pwr_oc
    INT_ID46_PWR_CHARGE,                                        //30 pwr_charge    
    INT_ID47_RESERVED,                                          //31 reserved

    NUM_INTERRUPTS                                              //Interrupts Total Nums

} INTERRUPT_ID;

_ATTR_INTRRUPT_BSS_ EXT uint8 UserIsrDisableCount;
_ATTR_INTRRUPT_BSS_ EXT uint8 UserIsrRequestCount;

/*
*-------------------------------------------------------------------------------
*  
*                           Functon Declaration
*  
*-------------------------------------------------------------------------------
*/

#ifdef DRIVERLIB_IRAM

extern void   IntMasterEnable(void);
extern void   IntMasterDisable(void);
extern void   FaultMasterEnable(void);
extern void   FaultMasterDisable(void);
extern void   IntPriorityGroupingSet(UINT32 ulBits);
extern UINT32 IntPriorityGroupingGet(void);
extern void   IntPrioritySet(UINT32 ulInterrupt, UINT32 ucPriority);
extern UINT32 IntPriorityGet(UINT32 ulInterrupt);
extern void   IntEnable(UINT32 ulInterrupt);
extern void   IntDisable(UINT32 ulInterrupt);
extern void   IntPendingSet(UINT32 ulInterrupt);
extern void   IntPendingClear(UINT32 ulInterrupt);
extern BOOL   IntISRPendingCheck(void);
extern UINT32 IntVectPendingGet(void);
extern UINT32 IntVectActiveGet(void);
extern uint32 IsrDisable(UINT32 ulInterrupt);
extern void   IsrEnable(UINT32 ulInterrupt, uint32 IsEnable);


#else

typedef void   (*pIntMasterEnable)(void);
typedef void   (*pIntMasterDisable)(void);
typedef void   (*pFaultMasterEnable)(void);
typedef void   (*pFaultMasterDisable)(void);
typedef void   (*pIntPriorityGroupingSet)(UINT32 ulBits);
typedef UINT32 (*pIntPriorityGroupingGet)(void);
typedef void   (*pIntPrioritySet)(UINT32 ulInterrupt, UINT32 ucPriority);
typedef UINT32 (*pIntPriorityGet)(UINT32 ulInterrupt);
typedef void   (*pIntEnable)(UINT32 ulInterrupt);
typedef void   (*pIntDisable)(UINT32 ulInterrupt);
typedef void   (*pIntPendingSet)(UINT32 ulInterrupt);
typedef void   (*pIntPendingClear)(UINT32 ulInterrupt);
typedef BOOL   (*pIntISRPendingCheck)(void);
typedef UINT32 (*pIntVectPendingGet)(void);
typedef UINT32 (*pIntVectActiveGet)(void);
typedef uint32 (*pIsrDisable)(UINT32 ulInterrupt);
typedef void   (*pIsrEnable)(UINT32 ulInterrupt, uint32 IsEnable);

#define IntMasterEnable()                       (((pIntMasterEnable       )(Addr_IntMasterEnable       ))())
#define IntMasterDisable()                      (((pIntMasterDisable      )(Addr_IntMasterDisable      ))())
#define FaultMasterEnable()                     (((pFaultMasterEnable     )(Addr_FaultMasterEnable     ))())
#define FaultMasterDisable()                    (((pFaultMasterDisable    )(Addr_FaultMasterDisable    ))())
#define IntPriorityGroupingSet(ulBits)          (((pIntPriorityGroupingSet)(Addr_IntPriorityGroupingSet))(ulBits))
#define IntPriorityGroupingGet()                (((pIntPriorityGroupingGet)(Addr_IntPriorityGroupingGet))())
#define IntPrioritySet(ulInterrupt, ucPriority) (((pIntPrioritySet        )(Addr_IntPrioritySet        ))(ulInterrupt, ucPriority))
#define IntPriorityGet(ulInterrupt)             (((pIntPriorityGet        )(Addr_IntPriorityGet        ))(ulInterrupt))
#define IntEnable(ulInterrupt)                  (((pIntEnable             )(Addr_IntEnable             ))(ulInterrupt))
#define IntDisable(ulInterrupt)                 (((pIntDisable            )(Addr_IntDisable            ))(ulInterrupt))
#define IntPendingSet(ulInterrupt)              (((pIntPendingSet         )(Addr_IntPendingSet         ))(ulInterrupt))
#define IntPendingClear(ulInterrupt)            (((pIntPendingClear       )(Addr_IntPendingClear       ))(ulInterrupt))
#define IntISRPendingCheck()                    (((pIntISRPendingCheck    )(Addr_IntISRPendingCheck    ))())
#define IntVectPendingGet()                     (((pIntVectPendingGet     )(Addr_IntVectPendingGet     ))())
#define IntVectActiveGet()                      (((pIntVectActiveGet      )(Addr_IntVectActiveGet      ))())
#define IsrDisable(ulInterrupt)                 (((pIsrDisable            )(Addr_IsrDisable            ))(ulInterrupt))
#define IsrEnable(ulInterrupt, IsEnable)        (((pIsrEnable             )(Addr_IsrEnable             ))(ulInterrupt, IsEnable))

#endif

extern __irq void IntDefaultHandler(void);
extern void IntRegister(UINT32 ulInterrupt, ExecFunPtr(*pfnHandler)(void));
extern void IntUnregister(UINT32 ulInterrupt);
extern uint32 UserIsrDisable(void);
extern void UserIsrEnable(uint32 data);
extern void UserIsrRequest(void);


/*
********************************************************************************
*
*                         End of interrupt.h
*
********************************************************************************
*/
#endif

