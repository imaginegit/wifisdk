/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   driverlib_def.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             anzhiguo      2009-1-14          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _DRIVERLIB_DEF_H_
#define _DRIVERLIB_DEF_H_

/*
*-------------------------------------------------------------------------------
*
*                            driverlib addr define
*
*-------------------------------------------------------------------------------
*/
//driverlib fun addr define
#define    Addr_NvicSystemReset                          (0x00002d75)
#define    Addr_NvicCpuReset                             (0x00002d7d)
#define    Addr_NvicClearActive                          (0x00002d87)
#define    Addr_NvicVectorTableSet                       (0x00002d91)
#define    Addr_DmaGetState                              (0x00002da1)
#define    Addr_DmaGetChannel                            (0x00002db7)
#define    Addr_DmaConfig                                (0x00002dd7)
#define    Addr_UARTRest                                 (0x00002f59)
#define    Addr_UARTSetIOP                               (0x00002f6b)
#define    Addr_UARTSetBaudRate                          (0x00002f81)
#define    Addr_UARTSetLcrReg                            (0x00002fa9)
#define    Addr_UARTSetFifoEnabledNumb                   (0x00003011)
#define    Addr_UARTSetIntEnabled                        (0x00003029)
#define    Addr_UARTSetIntDisabled                       (0x00003035)
#define    Addr_UARTWriteByte                            (0x00003041)
#define    Addr_UARTReadByte                             (0x0000305d)
#define    Addr_UARTInit                                 (0x0000307b)
#define    Addr_I2CStart                                 (0x000030b1)
#define    Addr_I2CStop                                  (0x000030bd)
#define    Addr_I2CSendData                              (0x000030e7)
#define    Addr_I2CReadData                              (0x00003159)
#define    Addr_SPIDeInit                                (0x000031ad)
#define    Addr_SPIWrite                                 (0x000031b7)
#define    Addr_SPIRead                                  (0x000031f7)
#define    Addr_I2SStart                                 (0x0000325d)
#define    Addr_I2SStop                                  (0x00003285)
#define    Addr_I2SInit                                  (0x00003293)
#define    Addr_I2SDeInit                                (0x000032c7)
#define    Addr_PWM_Start                                (0x000032dd)
#define    Addr_PWM_Stop                                 (0x000032ed)
#define    Addr_PwmRegReset                              (0x000032fd)
#define    Addr_PwmPrescalefFctorSet                     (0x0000330d)
#define    Addr_PwmPrescalefFctorGet                     (0x0000331d)
#define    Addr_Gpio_SetPinDirection                     (0x00003331)
#define    Addr_Gpio_GetPinDirection                     (0x00003353)
#define    Addr_Gpio_SetPinLevel                         (0x00003371)
#define    Addr_Gpio_GetPinLevel                         (0x00003393)
#define    Addr_Gpio_SetIntMode                          (0x000033ad)
#define    Addr_Gpio_GetIntMode                          (0x000033e7)
#define    Addr_Gpio_ClearInt_Level                      (0x000033f5)
#define    Addr_Gpio_EnableInt                           (0x00003403)
#define    Addr_Gpio_DisableInt                          (0x00003411)
#define    Addr_SysTickClkSourceSet                      (0x00003425)
#define    Addr_SysTickClkSourceGet                      (0x0000343d)
#define    Addr_SysTickEnable                            (0x00003449)
#define    Addr_SysTickDisable                           (0x00003457)
#define    Addr_SysTickPeriodGet                         (0x00003465)
#define    Addr_SysTickValueGet                          (0x0000346f)
#define    Addr_SysTickRefClockCheck                     (0x00003477)
#define    Addr_SysTickCalibrationCheck                  (0x00003485)
#define    Addr_SysTickCalibrationTenMsGet               (0x00003493)
#define    Addr_TimerEnable                              (0x000034a1)
#define    Addr_TimerDisable                             (0x000034ad)
#define    Addr_TimerPeriodGet                           (0x000034b9)
#define    Addr_Timer_GetCount                           (0x000034c3)
#define    Addr_TimerIntEnable                           (0x000034c9)
#define    Addr_TimerIntDisable                          (0x000034d5)
#define    Addr_TimerClearIntFlag                        (0x000034e1)
#define    Addr_TimerIntStateGet                         (0x000034e7)
#define    Addr_Delay10cyc                               (0x000034f1)
#define    Addr_Delay100cyc                              (0x000034f9)
#define    Addr_GetTimeHMS                               (0x00003503)
#define    Addr_IsArabicLetter                           (0x00003527)
#define    Addr_IsHebrewLetter                           (0x0000354f)
#define    Addr_IsBidirectionalString                    (0x00003573)
#define    Addr_IsLetter                                 (0x00003593)
#define    Addr_GetAsciiLength                           (0x000035af)
#define    Addr_BidirectionalStringPro                   (0x000035e1)
#define    Addr_ConTime2String                           (0x00003639)
#define    Addr_GetGbkCodeType                           (0x000036a3)
#define    Addr_Check_Special_TaiWen                     (0x000036bf)
#define    Addr_LCD_GetStringNum                         (0x000036d5)
#define    Addr_Lcdchang                                 (0x000036e7)
#define    Addr_WinCreat                                 (0x00003707)
#define    Addr_WinDestroy                               (0x0000371d)
#define    Addr_WinGetFocus                              (0x00003751)
#define    Addr_WinServiceProc                           (0x0000375d)
#define    Addr_WinKeyProc                               (0x0000377d)
#define    Addr_ThreadCreat                              (0x00003795)
#define    Addr_ThreadDelete                             (0x000037c9)
#define    Addr_ThreadDeleteAll                          (0x0000381d)
#define    Addr_ThreadCheck                              (0x00003857)
#define    Addr_ThreadProcess                            (0x00003869)
#define    Addr_IntMasterEnable                          (0x0000387d)
#define    Addr_IntMasterDisable                         (0x00003881)
#define    Addr_FaultMasterEnable                        (0x00003885)
#define    Addr_FaultMasterDisable                       (0x00003889)
#define    Addr_IntPriorityGroupingSet                   (0x0000388d)
#define    Addr_IntPriorityGroupingGet                   (0x0000389d)
#define    Addr_IntPrioritySet                           (0x000038ad)
#define    Addr_IntPriorityGet                           (0x000038d5)
#define    Addr_IntEnable                                (0x000038f1)
#define    Addr_IntDisable                               (0x0000394b)
#define    Addr_IntPendingSet                            (0x000039a5)
#define    Addr_IntPendingClear                          (0x000039f1)
#define    Addr_IntISRPendingCheck                       (0x00003a2f)
#define    Addr_IntVectPendingGet                        (0x00003a39)
#define    Addr_IntVectActiveGet                         (0x00003a47)
#define    Addr_IsrDisable                               (0x00003a53)
#define    Addr_IsrEnable                                (0x00003ab3)
#define    Addr_SystickTimerStartRaw                     (0x00003ac9)
#define    Addr_SystickTimerStopRaw                      (0x00003af3)
#define    Addr_SysTickHandlerRaw                        (0x00003b23)
#define    Addr___WFI                                    (0x00003b61)
#define    Addr___WFE                                    (0x00003b65)
#define    Addr___SEV                                    (0x00003b69)
#define    Addr___ISB                                    (0x00003b6d)
#define    Addr___DSB                                    (0x00003b73)
#define    Addr___DMB                                    (0x00003b79)
#define    Addr___SVC                                    (0x00003b7f)
#define    Addr___MRS_CONTROL                            (0x00003b83)
#define    Addr___MSR_CONTROL                            (0x00003b89)
#define    Addr___MRS_PSP                                (0x00003b93)
#define    Addr___MSR_PSP                                (0x00003b99)
#define    Addr___MRS_MSP                                (0x00003b9f)
#define    Addr___MSR_MSP                                (0x00003ba5)
#define    Addr___SETPRIMASK                             (0x00003bab)
#define    Addr___RESETPRIMASK                           (0x00003baf)
#define    Addr___SETFAULTMASK                           (0x00003bb3)
#define    Addr___RESETFAULTMASK                         (0x00003bb7)
#define    Addr___BASEPRICONFIG                          (0x00003bbb)
#define    Addr___GetBASEPRI                             (0x00003bc1)
#define    Addr___REV_HalfWord                           (0x00003bc7)
#define    Addr___REV_Word                               (0x00003bcb)

/*
********************************************************************************
*
*                         End of driverlib_def.h
*
********************************************************************************
*/
#endif
