/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   interrupt.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-7-21          1.0
*    desc:    
********************************************************************************
*/
#define _IN_INTTERUPT_

#include "OsInclude.h"
#include "DriverInclude.h"

extern int Main(void);
extern UINT32 Image$$ARM_LIB_STACK$$ZI$$Limit;

/*
--------------------------------------------------------------------------------
  Function name :  
  Author        : ZHengYongzhi
  Description   : CortexM3 interrupt vector table
                  
  Input         :  
  Return        :  

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_VECTTAB_
ExecFunPtr exceptions_table[NUM_INTERRUPTS] = {
    
    /* Core Fixed interrupts start here...*/
    
    (ExecFunPtr)&Image$$ARM_LIB_STACK$$ZI$$Limit,               
    (ExecFunPtr)Main,                                           
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                          
    IntDefaultHandler,                                         
    IntDefaultHandler,                                          
    
    //RKNano Process Intterupt
    IntDefaultHandler,                                          //00 int_dma             
    IntDefaultHandler,                                          //01 uart                
    IntDefaultHandler,                                          //02 sd mmc              
    IntDefaultHandler,                                          //03 pwm1                
    IntDefaultHandler,                                          //04 pwm2                
    IntDefaultHandler,                                          //05 imdct36             
    IntDefaultHandler,                                          //06 synthesize          
    IntDefaultHandler,                                          //07 usb                 
    IntDefaultHandler,                                          //08 i2c                 
    IntDefaultHandler,                                          //09 i2s                 
    IntDefaultHandler,                                          //10 gpio                 
    IntDefaultHandler,                                          //11 spi                 
    IntDefaultHandler,                                          //12 pwm0                   
    IntDefaultHandler,                                          //13 timer               
    IntDefaultHandler,                                          //14 sar-adc             
    IntDefaultHandler,                                          //15 reserved            
    IntDefaultHandler,                                          //16 reserved            
    IntDefaultHandler,                                          //17 reserved            
    IntDefaultHandler,                                          //18 rtc                 
    IntDefaultHandler,                                          //19 reserved            
    IntDefaultHandler,                                          //20 ool_bat_snsen       
    IntDefaultHandler,                                          //21 reserved            
    IntDefaultHandler,                                          //22 ool_pause           
    IntDefaultHandler,                                          //23 ool_PlayOn          
    IntDefaultHandler,                                          //24 pwr_5v_ready        
    IntDefaultHandler,                                          //25 pwr_uvlo_vdd        
    IntDefaultHandler,                                          //26 pwr_uvp             
    IntDefaultHandler,                                          //27 pwr_ovp             
    IntDefaultHandler,                                          //28 pwr_ot              
    IntDefaultHandler,                                          //29 pwr_oc              
    IntDefaultHandler,                                          //30 pwr_charge          
    IntDefaultHandler,                                          //31 reserved            
};                                                                                       
                                                                //Interrupts Total Nums  
/*
--------------------------------------------------------------------------------
  Function name :  IntDefaultHandler(void)
  Author        :  ZHengYongzhi
  Description   :  default interrupt service program.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_INTRRUPT_CODE_
__asm uint32 __MSR_XPSR(void)
{
    MRS     R0, XPSR
    BX      R14
}        
#if 1
_ATTR_INTRRUPT_CODE_
__irq void IntDefaultHandler(void)
{
    uint32 *pStack;
    uint32 i, j;

    (uint32)pStack = __MRS_MSP();
    while((uint32)pStack < 0x03018000)
    {
        printf("        Stack = 0x%x: ", (uint32)pStack);
        for (i = 0 ; i < 8; i++)
        {
            printf("0x%x ", *(uint32*)pStack++);
            if ((uint32)pStack >= 0x03018000)
            {
                break;
            }
        }
        printf("\n");
    }
    printf("IntDefaultHandler: control_reg = 0x%x, MSP = 0x%x, PSP = 0x%x, XPSR = 0x%x!\n", __MRS_CONTROL(), __MRS_MSP(), __MRS_PSP(),__MSR_XPSR());

    while(1);
}
#else
_ATTR_INTRRUPT_CODE_
__asm __irq void IntDefaultHandler(void)
{        
	TST LR, #0x4    		; EXC_RETURN.2=0? 
    ITTEE 	EQ     			;  
    MRSEQ 	R0, MSP    		; MSPR0 
    LDREQ 	R0,[R0,#24]   	; MSPPC 
    MRSNE 	R0, PSP    		; PSPR0 
    LDRNE 	R0,[R0,#24]   	; PSPPC 
HERE
	B		HERE
}
#endif

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name :  IntMasterEnable(void)
  Author        :  ZHengYongzhi
  Description   :  Enable processor interrupts.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG											
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_//_ATTR_INTRRUPT_CODE_
void IntMasterEnable(void)
{
    __SETPRIMASK();
}

/*
--------------------------------------------------------------------------------
  Function name :  IntMasterEnable(void)
  Author        :  ZHengYongzhi
  Description   :  Disable processor interrupts.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void IntMasterDisable(void)
{
    __RESETPRIMASK();
}

/*
--------------------------------------------------------------------------------
  Function name :  IntMasterEnable(void)
  Author        :  ZHengYongzhi
  Description   :  Enable processor interrupts.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG											
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_//_ATTR_INTRRUPT_CODE_
void FaultMasterEnable(void)
{
    __RESETFAULTMASK();
}

/*
--------------------------------------------------------------------------------
  Function name :  IntMasterEnable(void)
  Author        :  ZHengYongzhi
  Description   :  Disable processor interrupts.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void FaultMasterDisable(void)
{    
    __SETFAULTMASK();
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : IntRegister(UINT32 ulInterrupt, void (*pfnHandler)(void))
  Author        : ZHengYongzhi
  Description   : interrupts register
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_INTRRUPT_CODE_
void IntRegister(UINT32 ulInterrupt, ExecFunPtr(*pfnHandler)(void))
{
    UINT32 ulIdx;

    // Check the arguments.
    ASSERT(ulInterrupt < NUM_INTERRUPTS);

    // Make sure that the RAM vector table is correctly aligned.
    ASSERT(((UINT32)exceptions_table & 0x000003ff) == 0);

    // See if the RAM vector table has been initialized.
    if(nvic->VectorTableOffset != (UINT32)exceptions_table)
    {
        // Initiale the RAM vector table.
        exceptions_table[0] = (ExecFunPtr)&Image$$ARM_LIB_STACK$$ZI$$Limit;
        exceptions_table[1] = (ExecFunPtr)Main;
        for(ulIdx = 2; ulIdx < NUM_INTERRUPTS; ulIdx++)
        {
            exceptions_table[ulIdx] = (ExecFunPtr)IntDefaultHandler;
        }

        // Point NVIC at the RAM vector table.
        nvic->VectorTableOffset = (UINT32)exceptions_table;
    }

    // Save the interrupt handler.
    exceptions_table[ulInterrupt] = (ExecFunPtr)pfnHandler;
}

/*
--------------------------------------------------------------------------------
  Function name : IntUnregister(UINT32 ulInterrupt)
  Author        : ZHengYongzhi
  Description   : interrupts Unregister
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_INTRRUPT_CODE_
void IntUnregister(UINT32 ulInterrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ulInterrupt < NUM_INTERRUPTS);

    //
    // Reset the interrupt handler.
    //
    exceptions_table[ulInterrupt] = (ExecFunPtr)IntDefaultHandler;
}

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : void IntPriorityGroupingSet(UINT32 ulBits)
  Author        : ZHengYongzhi
  Description   : priority is grouping setting
                  This function specifies the split between preemptable priority
                  levels and subpriority levels in the interrupt priority speci-
                  fication.  The range of the grouping values are dependent upon 
                  the hardware implementation.
                   
  Input         : ulBits specifies the number of bits of preemptable priority.
  Return        :  

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_//_ATTR_INTRRUPT_CODE_
void IntPriorityGroupingSet(UINT32 ulBits)
{
    //
    // Set the priority grouping.
    //
    nvic->APIntRst = NVIC_APINTRST_VECTKEY | ((7 - ulBits) << 8);
}

/*
--------------------------------------------------------------------------------
  Function name : void IntPriorityGroupingSet(UINT32 ulBits)
  Author        : ZHengYongzhi
  Description   : get priority grouping.
                  This function returns the split between preemptable priority 
                  levels and subpriority levels in the interrupt priority spe-
                  cification.
  Input         :
  Return        : The number of bits of preemptable priority.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 IntPriorityGroupingGet(void)
{
    UINT32 ulValue;

    //
    // Read the priority grouping.
    //
    ulValue = nvic->APIntRst & NVIC_APINTRST_PRIGROUP_MASK;
    
    //
    // Return the number of priority bits.
    //
    return(7 - (ulValue >> 8));
}

/*
--------------------------------------------------------------------------------
  Function name : void IntPrioritySet(UINT32 ulInterrupt, unsigned char ucPriority)
  Author        : ZHengYongzhi
  Description   : Sets the priority of an interrupt.
                  This function is used to set the priority of an interrupt.
  Input         : ulInterrupt -- INT_ID;   ucPriority -- Priority Num.        
  Return        :

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void IntPrioritySet(UINT32 ulInterrupt, UINT32 ucPriority)
{
    UINT32 ulBitOffset;
    UINT32 *pRegister;
    
    //
    // Set the interrupt priority.
    //
    ulBitOffset   = (ulInterrupt & 0x03) * 8;
    
    if (ulInterrupt < 16 ) {
        pRegister = (UINT32*)((uint32)nvic->SystemPriority - 4);
    } else {
        pRegister = (UINT32 *)nvic->Irq.Priority;
    }
    pRegister = pRegister + (ulInterrupt >> 2);
    
    *pRegister = *pRegister & (~(0xFF << ulBitOffset)) | (ucPriority << ulBitOffset);
}

/*
--------------------------------------------------------------------------------
  Function name :  UINT32 IntPriorityGet(UINT32 ulInterrupt)
  Author        :  ZHengYongzhi
  Description   :  Gets the priority of an interrupt.
                   This function gets the priority of an interrupt.
  Input         :  ulInterrupt -- INT_ID
  Return        :  Returns the interrupt priority.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 IntPriorityGet(UINT32 ulInterrupt)
{
    UINT32 ulBitOffset;
    UINT32 *pRegister;

    //
    // Return the interrupt priority.
    //
    ulBitOffset   = (ulInterrupt & 0x03) * 8;
    
    if (ulInterrupt < 16 ) {
        pRegister = (UINT32 *)(nvic->SystemPriority - 4);
    } else {
        pRegister = (UINT32 *)nvic->Irq.Priority;
    }
    pRegister = pRegister + (ulInterrupt & 0x03);
    
    return((*pRegister >> ulBitOffset) & 0xFF);
}

/*
--------------------------------------------------------------------------------
  Function name : void IntEnable(UINT32 ulInterrupt)
  Author        : ZHengYongzhi
  Description   : Enables an interrupt.
                  The specified interrupt is enabled in the interrupt controller.
  Input         : ulInterrupt -- INT_ID
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void IntEnable(UINT32 ulInterrupt)
{
    //
    // Determine the interrupt to enable.
    //
    if(ulInterrupt == FAULT_ID4_MPU)
    {
        //
        // Enable the MemManage interrupt.
        //
        nvic->SystemHandlerCtrlAndState |= NVIC_SYSHANDCTRL_MEMFAULTENA;
    }
    else if(ulInterrupt == FAULT_ID5_BUS)
    {
        //
        // Enable the bus fault interrupt.
        //
        nvic->SystemHandlerCtrlAndState |= NVIC_SYSHANDCTRL_BUSFAULTENA;
    }
    else if(ulInterrupt == FAULT_ID6_USAGE)
    {
        //
        // Enable the usage fault interrupt.
        //
        nvic->SystemHandlerCtrlAndState |= NVIC_SYSHANDCTRL_USGFAULTENA;
    }
    else if(ulInterrupt == FAULT_ID15_SYSTICK)
    {
        //
        // Enable the System Tick interrupt.
        //
        nvic->SysTick.Ctrl |= NVIC_SYSTICKCTRL_TICKINT;
    }
    else if (ulInterrupt >= 16)
    {
        //
        // Enable the general interrupt.
        //
        nvic->Irq.Enable[(ulInterrupt - 16) / 32] = 1 << (ulInterrupt - 16) % 32;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void IntDisable(UINT32 ulInterrupt)
  Author        : ZHengYongzhi
  Description   : Disables an interrupt.
                  The specified interrupt is Disabled in the interrupt controller.
  Input         : ulInterrupt -- INT_ID
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void IntDisable(UINT32 ulInterrupt)
{
    //
    // Determine the interrupt to Disable.
    //
    if(ulInterrupt == FAULT_ID4_MPU)
    {
        //
        // Disable the MemManage interrupt.
        //
        nvic->SystemHandlerCtrlAndState &= ~NVIC_SYSHANDCTRL_MEMFAULTENA;
    }
    else if(ulInterrupt == FAULT_ID5_BUS)
    {
        //
        // Disable the bus fault interrupt.
        //
        nvic->SystemHandlerCtrlAndState &= ~NVIC_SYSHANDCTRL_BUSFAULTENA;
    }
    else if(ulInterrupt == FAULT_ID6_USAGE)
    {
        //
        // Disable the usage fault interrupt.
        //
        nvic->SystemHandlerCtrlAndState &= ~NVIC_SYSHANDCTRL_USGFAULTENA;
    }
    else if(ulInterrupt == FAULT_ID15_SYSTICK)
    {
        //
        // Disable the System Tick interrupt.
        //
        nvic->SysTick.Ctrl &= ~NVIC_SYSTICKCTRL_TICKINT;
    }
    else if (ulInterrupt >= 16)
    {
        //
        // Disable the general interrupt.
        //
        nvic->Irq.Disable[(ulInterrupt - 16) / 32] = 1 << (ulInterrupt - 16) % 32;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void IntPendingSet(UINT32 ulInterrupt)
  Author        : ZHengYongzhi
  Description   : Set pending bit of the interrupt.
  
  Input         : 
  Return        : Pending IRQ Channel Identifier.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void IntPendingSet(UINT32 ulInterrupt)
{
    //
    // Determine the interrupt pend to Set.
    //
    if(ulInterrupt == FAULT_ID2_NMI)
    {
        //
        // Set the NMI interrupt pend.
        //
        nvic->INTcontrolState |= NVIC_INTCTRLSTA_NMIPENDSET;
    }
    else if(ulInterrupt == FAULT_ID14_PENDSV)
    {
        //
        // Set the PendSV interrupt pend.
        //
        nvic->INTcontrolState |= NVIC_INTCTRLSTA_PENDSVSET;
    }
    else if(ulInterrupt == FAULT_ID15_SYSTICK)
    {
        //
        // Set the System Tick interrupt pend.
        //
        nvic->INTcontrolState |= NVIC_INTCTRLSTA_PENDSTSET;
    }
    else if (ulInterrupt >= 16)
    {
        //
        // Set the general interrupt pend.
        //
        nvic->Irq.SetPend[(ulInterrupt - 16) / 32] = 1 << (ulInterrupt - 16) % 32;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void IntPendClear(UINT32 ulInterrupt)
  Author        : ZHengYongzhi
  Description   : Clear pending bit of the irq.
  
  Input         : 
  Return        : Pending IRQ Channel Identifier.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void IntPendingClear(UINT32 ulInterrupt)
{
    //
    // Determine the interrupt pend to Set.
    //
    if(ulInterrupt == FAULT_ID14_PENDSV)
    {
        //
        // Set the PendSV interrupt pend.
        //
        nvic->INTcontrolState |= NVIC_INTCTRLSTA_PENDSVCLR;
    }
    else if(ulInterrupt == FAULT_ID15_SYSTICK)
    {
        //
        // Set the System Tick interrupt pend.
        //
        nvic->INTcontrolState |= NVIC_INTCTRLSTA_PENDSTCLR;
    }
    else if (ulInterrupt >= 16)
    {
        //
        // Set the general interrupt pend.
        //
        nvic->Irq.ClearPend[(ulInterrupt - 16) / 32] = 1 << (ulInterrupt - 16) % 32;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : BOOL IntISRPendingCheck(void)
  Author        : ZHengYongzhi
  Description   : Check interrupt pending or not.
  
  Input         : 
  Return        : pending or not.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
BOOL IntISRPendingCheck(void)
{
    //
    // Check the arguments.
    //
    return ((BOOL)(nvic->INTcontrolState & NVIC_INTCTRLSTA_ISRPENDING));
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 IntVectPendingGet(void)
  Author        : ZHengYongzhi
  Description   : Get interrupt number of the highest priority pending ISR.
  
  Input         : 
  Return        : Return interrupt number of the highest priority pending ISR.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 IntVectPendingGet(void)
{
    //
    // Check the arguments.
    //
    return ((nvic->INTcontrolState & NVIC_INTCTRLSTA_ISRPENDING) >> 12);
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 IntVectActiveGet(void)
  Author        : ZHengYongzhi
  Description   : Get interrupt number of the currently running ISR.
  
  Input         : 
  Return        : Return interrupt number of the currently running ISR.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
UINT32 IntVectActiveGet(void)
{
    //
    // Check the arguments.
    //
    return (nvic->INTcontrolState & NVIC_INTCTRLSTA_VECTACTIVE_MASK);
}

/*
--------------------------------------------------------------------------------
  Function name : uint32 IsrDisable(UINT32 ulInterrupt)
  Author        : ZHengYongzhi
  Description   : close interrupt temporarily,go to use with IsrEnable.
  
  Input         : 
  Return        : current interrupt status.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
uint32 IsrDisable(UINT32 ulInterrupt)
{
    uint32 IsEnable;

    //
    // Determine the interrupt to enable.
    //
    if(ulInterrupt == FAULT_ID4_MPU)
    {
        //
        // Is the MemManage interrupt Enabled.
        //
        IsEnable = (nvic->SystemHandlerCtrlAndState & NVIC_SYSHANDCTRL_MEMFAULTENA);
    }
    else if(ulInterrupt == FAULT_ID5_BUS)
    {
        //
        // Is the bus fault interrupt Enabled.
        //
        IsEnable = (nvic->SystemHandlerCtrlAndState & NVIC_SYSHANDCTRL_BUSFAULTENA);
    }
    else if(ulInterrupt == FAULT_ID6_USAGE)
    {
        //
        // Is the usage fault interrupt Enabled.
        //
        IsEnable = (nvic->SystemHandlerCtrlAndState & NVIC_SYSHANDCTRL_USGFAULTENA);
    }
    else if(ulInterrupt == FAULT_ID15_SYSTICK)
    {
        //
        // Is the System Tick interrupt Enabled.
        //
        IsEnable = (nvic->SysTick.Ctrl & NVIC_SYSTICKCTRL_TICKINT);
    }
    else if (ulInterrupt >= 16)
    {
        //
        // Is the general interrupt Enabled.
        //
        IsEnable = (nvic->Irq.Enable[(ulInterrupt - 16) / 32] & (1 << (ulInterrupt - 16) % 32));
    }
    
    IntDisable(ulInterrupt);

    return(IsEnable);
    
}

/*
--------------------------------------------------------------------------------
  Function name : void IsrEnable(UINT32 ulInterrupt, uint32 IsEnable)
  Author        : ZHengYongzhi
  Description   : open interrupt and it act in concert with IsrDisable.
  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void IsrEnable(UINT32 ulInterrupt, uint32 IsEnable)
{
    if (IsEnable)
    {
        IntEnable(ulInterrupt);
    }
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : uint32 UserIsrDisable(void)
  Author        : ZhengYongzhi
  Description   : PendSV is soft interrupt, it cannot disable and enable,to use semaphore MSG_USER_ISR_ENABLE
                  to enable PendSV,it act in concert with UserIsrEnable,
                  
  Input         : 
  Return        : NULL

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_INTRRUPT_CODE_
uint32 UserIsrDisable(void)
{
	UserIsrDisableCount++;
    
    return 0;
}

/*
--------------------------------------------------------------------------------
  Function name : void UserIsrEnable(uint32 data)
  Author        : ZhengYongzhi
  Description   : PendSV is soft interrupt, it cannot disable and enable,use semaphore MSG_USER_ISR_ENABLE
                  to enable PendSV,it act in concert with UserIsrEnable,it shall judge the status of FAULT_ID14_PENDSV,
                  it must to initial both MSG_USER_ISR_ENABLE and MSG_USER_ISR_STATUES two semaphores before to use
                  these two functions.

  Input         : 
  Return        : NULL

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_INTRRUPT_CODE_
void UserIsrEnable(uint32 data)
{
	if (UserIsrDisableCount > 0)
	{
		UserIsrDisableCount--;
	}

	if ((UserIsrDisableCount == 0) && (UserIsrRequestCount > 0))
    {
		//UserIsrRequestCount --;
		UserIsrRequestCount = 0;
		IntPendingSet(FAULT_ID14_PENDSV);
    }
}

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
_ATTR_INTRRUPT_CODE_
void UserIsrRequest(void)
{
	if (UserIsrDisableCount == 0)
    {
        IntPendingSet(FAULT_ID14_PENDSV);
    }
    else
    {
        //UserIsrRequestCount++;
        UserIsrRequestCount = 1;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void UserIsrEnable(uint32 data)
  Author        : ZhengYongzhi
  Description   : PendSV作为软中断，不能进行Disable和Enable，使用信号量MSG_USER_ISR_ENABLE
                  进行PendSV的使能标志，和UserIsrDisable配合使用，打开时需要判断FAULT_ID14_PENDSV
                  的状态。使用这两个函数之前必须对信号量MSG_USER_ISR_ENABLE和MSG_USER_ISR_STATUES
                  进行初始化
  Input         : 
  Return        : 无

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
uint32 IrqPriorityTab[NUM_INTERRUPTS] =
{
    //  group        SubPriority                                 //Cortex M3 System Intterupt  
    -3,                                 //SP Point                FAULT_ID0_REV,                     
    -3,                                 //Reset                   FAULT_ID1_REV,                     
    -2,                                 //NMI fault               FAULT_ID2_NMI,                     
    -1,                                 //Hard fault              FAULT_ID3_HARD,                    
    ((0x00 << 6) | (0x00 << 3)),        //MPU fault               FAULT_ID4_MPU,                     
    ((0x00 << 6) | (0x00 << 3)),        //Bus fault               FAULT_ID5_BUS,                     
    ((0x00 << 6) | (0x00 << 3)),        //Usage fault             FAULT_ID6_USAGE,                   
    ((0xFF << 6) | (0xFF << 3)),        //                        FAULT_ID7_REV,                     
    ((0xFF << 6) | (0xFF << 3)),        //                        FAULT_ID8_REV,                     
    ((0xFF << 6) | (0xFF << 3)),        //                        FAULT_ID9_REV,                     
    ((0xFF << 6) | (0xFF << 3)),        //                        FAULT_ID10_REV,                    
    ((0xFF << 6) | (0xFF << 3)),        //SVCall                  FAULT_ID11_SVCALL,                 
    ((0xFF << 6) | (0xFF << 3)),        //Debug monitor           FAULT_ID12_DEBUG,                  
    ((0xFF << 6) | (0xFF << 3)),        //                        FAULT_ID13_REV,                    
    ((0x03 << 6) | (0x00 << 3)),        //PendSV                  FAULT_ID14_PENDSV,                 
    ((0x02 << 6) | (0x01 << 3)),        //System Tick             FAULT_ID15_SYSTICK,                
                                                                                                     
    //RKNano Process Int               
    ((0x00 << 6) | (0x01 << 3)),        //00 int_dma              INT_ID16_DMA,                      
    ((0x02 << 6) | (0x00 << 3)),        //01 uart                 INT_ID17_UART,                     
    //((0x01 << 6) | (0x00 << 3)),        //02 sd mmc               INT_ID18_SDMMC,                    
    ((0x02 << 6) | (0x07 << 3)),        //02 sd mmc               INT_ID18_SDMMC,                    
    ((0x02 << 6) | (0x00 << 3)),        //03 pwm1                 INT_ID19_PWM1,                     
    ((0x02 << 6) | (0x00 << 3)),        //04 pwm2                 INT_ID20_PWM2,                     
    ((0x02 << 6) | (0x00 << 3)),        //05 imdct36              INT_ID21_IMDCT36,                  
    ((0x02 << 6) | (0x00 << 3)),        //06 synthesize           INT_ID22_SYNTHESIZE,               
    ((0x01 << 6) | (0x01 << 3)),        //07 usb                  INT_ID23_USB,                      
    ((0x02 << 6) | (0x02 << 3)),        //08 i2c                  INT_ID24_I2C,                      
    ((0x02 << 6) | (0x03 << 3)),        //09 i2s                  INT_ID25_I2S,                      
    ((0x02 << 6) | (0x04 << 3)),        //10 gpio                 INT_ID26_GPIO,                     
    ((0x02 << 6) | (0x05 << 3)),        //11 spi                  INT_ID27_SPI,                      
    ((0x02 << 6) | (0x06 << 3)),        //12 pwm0                 INT_ID28_PWM0,                     
    ((0x02 << 6) | (0x07 << 3)),        //13 timer                INT_ID29_TIMER,                    
    ((0x01 << 6) | (0x02 << 3)),        //14 sar-adc              INT_ID30_ADC,                      
    ((0xFF << 6) | (0xFF << 3)),        //15 reserved             INT_ID31_RESERVED,                 
    ((0xFF << 6) | (0xFF << 3)),        //16 reserved             INT_ID32_RESERVED,                 
    ((0xFF << 6) | (0xFF << 3)),        //17 reserved             INT_ID33_RESERVED,                 
    ((0x02 << 6) | (0x07 << 3)),        //18 rtc                  INT_ID34_RTC,                      
    ((0xFF << 6) | (0xFF << 3)),        //19 reserved             INT_ID35_RESERVED,                 
    ((0x02 << 6) | (0x07 << 3)),        //20 ool_bat_snsen        INT_ID36_OOL_BAT_SNSEN,            
    ((0xFF << 6) | (0xFF << 3)),        //21 reserved             INT_ID37_RESERVED,                 
    ((0x02 << 6) | (0x07 << 3)),        //22 ool_pause            INT_ID38_OOL_PAUSE,                
    ((0x02 << 6) | (0x07 << 3)),        //23 ool_PlayOn           INT_ID39_OOL_PLAYON,               
    ((0x02 << 6) | (0x07 << 3)),        //24 pwr_5v_ready         INT_ID40_PWR_5V_READY,             
    ((0x02 << 6) | (0x07 << 3)),        //25 pwr_uvlo_vdd         INT_ID41_PWR_UVLO_VDD,             
    ((0x02 << 6) | (0x07 << 3)),        //26 pwr_uvp              INT_ID42_PWR_UVP,                  
    ((0x02 << 6) | (0x07 << 3)),        //27 pwr_ovp              INT_ID43_PWR_OVP,                  
    ((0x02 << 6) | (0x07 << 3)),        //28 pwr_ot               INT_ID44_PWR_OT,                   
    ((0x02 << 6) | (0x07 << 3)),        //29 pwr_oc               INT_ID45_PWR_OC,                   
    ((0x02 << 6) | (0x07 << 3)),        //30 pwr_charge           INT_ID46_PWR_CHARGE,               
    ((0xFF << 6) | (0xFF << 3)),        //31 reserved             INT_ID47_RESERVED,                 
};        

_ATTR_SYS_INIT_CODE_
void IrqPriorityInit(void)              //中断优先级初始化, 在IntPriorityGroupingSet执行后调用
{
    uint32 i;
    
    IntPriorityGroupingSet(2);
    for (i = 4; i < NUM_INTERRUPTS; i++)
    {
        IntPrioritySet(i, IrqPriorityTab[i]);
    }
}
                                                             
/*
********************************************************************************
*
*                         End of interrupt.c
*
********************************************************************************
*/

