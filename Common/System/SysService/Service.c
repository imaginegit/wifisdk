/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name:  SysService.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    
********************************************************************************
*/
#define _IN_SYSSERVICE_

#include "SysInclude.h"
#include "UsbAdapterProbe.h"

#include "FunUSBInterface.h"
#include "FsInclude.h"
#include "Mainmenu.h"
#include "PowerOn_Off.h"

extern BOOLEAN VideoPause(void);

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : 系统服务程序，该程序调用系统消息、系统线程、外设检测等后台需要
                  完成的工作
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
uint32 USBStatusDetect(void)
{
    uint32 RetVal = 0;
    int32  UsbAdapterRet = 0;
    TASK_ARG TaskArg;
    
    if (CheckVbus())
    {
        if (FALSE == CheckMsg(MSG_USB_INSERT))
        {
            SendMsg(MSG_USB_INSERT);
            SendMsg(MSG_SYS_RESUME);
            
            AutoPowerOffDisable();
            //SetPowerOffTimerDisable();

            UsbAdpterProbeStart();            
            
        }

        UsbAdapterRet = UsbAdpterProbe();

        if (1 == UsbAdapterRet)
        {   
            #ifdef _USB_
            PMU_EnterModule(PMU_BLON);
            ThreadDeleteAll(&pMainThread);

            if (GetMsg(MSG_MES_FIRMWAREUPGRADE))
            {
                TaskArg.Usb.FunSel = FUN_USB_UPGRADE;
            }
            else
            {
                TaskArg.Usb.FunSel = FUN_USB_DISK;
            }            
            TaskSwitch(TASK_ID_USB, &TaskArg);
            
            RetVal = RETURN_FAIL;
            #endif

            //charge...
            SendMsg(MSG_CHARGE_START);        
        }
        else if (2 == UsbAdapterRet)
        {
            //charge...
            SendMsg(MSG_CHARGE_START);
        }
    }
    else 
    {
        if (CheckMsg(MSG_USB_INSERT)) 
        {
            ClearMsg(MSG_USB_INSERT);
            SendMsg(MSG_SYS_RESUME);
            ClearMsg(MSG_CHARGE_ENABLE);
            ClearMsg(MSG_CHARGE_START);
            PMU_EnterModule(PMU_BLON); 
            
            UsbAdpterProbeStop();
            
            AutoPowerOffEnable();
            //SetPowerOffTimerEnable();
        }
    }

    return (RetVal);
}


/*
--------------------------------------------------------------------------------
  Function name : AdcKey_SysTimer_Handler()
  Author        : anzhiguo
  Description   : ADC Key Timer Isr
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AutoPowerDownTimerISR(void)
{
    SendMsg(MSG_POWER_DOWN);
}

_ATTR_SYS_CODE_
void SetPowerDownTimerISR(void)
{
    SetPowerOffFlag = 0;
    SendMsg(MSG_POWER_DOWN);
}

_ATTR_SYS_DATA_
SYSTICK_LIST AutoPowerDownTimer =
{ 
    NULL, 
    0, 
    10 * 60 * 100, 
    1, 
    AutoPowerDownTimerISR,
};

_ATTR_SYS_DATA_
SYSTICK_LIST SetPowerDownTimer =
{ 
    NULL, 
    0, 
    0, 
    1, 
    SetPowerDownTimerISR,
};

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AutioPowerOffTimerRest(void)
{
    AutoPowerDownTimer.Counter = 0;
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AutoPowerOffEnable(void)
{
    if (AutoPowerOffDisableCounter > 0)
    {
        AutoPowerOffDisableCounter--;
        
        if (AutoPowerOffDisableCounter == 0)
        {
            SystickTimerStart(&AutoPowerDownTimer);
        }
    }    
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void AutoPowerOffDisable(void)
{
    if (AutoPowerOffDisableCounter == 0)
    {
        SystickTimerStop(&AutoPowerDownTimer);
    }
    AutoPowerOffDisableCounter++;
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SetPowerOffTimerEnable(void)
{
    uint32 PowerOffBuff[6] = {0, 10*60*100, 15*60*100, 30*60*100, 60*60*100, 120*60*100};
        
    if (gSysConfig.ShutTime) 
    {
        SetPowerDownTimer.Counter = 0;
        SetPowerDownTimer.Reload = PowerOffBuff[gSysConfig.ShutTime];

        if (SetPowerOffFlag == 0)
        {
            SetPowerOffFlag = 1;
            SystickTimerStart(&SetPowerDownTimer);
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SetPowerOffTimerDisable(void)
{
    if (SetPowerOffFlag == 1)
    {
        SetPowerOffFlag = 0;
        SystickTimerStop(&SetPowerDownTimer);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
uint32 PowerOffDetec(void)
{
    uint32 RetVal = RETURN_OK;
    TASK_ARG TaskArg;
   
    //--------------------------------------------------------------------------
    if (GetMsg(MSG_POWER_DOWN))
    {
        ThreadDeleteAll(&pMainThread);
            
        if ((CheckVbus()) && (0 == Scu_DCout_Issue_State_Get()))
        {
            //TaskArg.Usb.FunSel = FUN_USB_CHARGE;
            RetVal = RETURN_FAIL;
        }
        else
        {
            ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
            SaveSysInformation();

            #if (NAND_DRIVER == 1)
            FtlClose();
            #endif

            MDDeInitAll();

            PowerOff();
        }
    }
    
    return(RetVal);
}

/*
--------------------------------------------------------------------------------
  Function name : void PowerOffDetec(void)
  Author        : ZHengYongzhi
  Description   : 关机检测
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SysResume(void)
{
    if(TRUE == GetMsg(MSG_SYS_RESUME))
    {
        //KeyReset();
        AutioPowerOffTimerRest();
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void SysBatteryStateChk(void)
  
  Author        : yangwenjie
  Description   : shutdown check
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
void SysBatteryStateCheck(void)
{
	
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : 系统服务程序，该程序调用系统消息、系统线程、外设检测等后台需要
                  完成的工作
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysServiceInit(void)
{
    DEBUG("system service initial...");

    AutoPowerOffDisableCounter = 1;
    AutoPowerOffEnable();
    
    SetPowerOffTimerEnable();
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 SysService(void)
  Author        : ZHengYongzhi
  Description   : system service progarm,this program will call system backgroud message,system thread 
                  and devices check etc to finish work.,
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
UINT32 SysService(void)
{
    uint32 retval;
    
   if (RETURN_FAIL == USBStatusDetect())
    {
        return(RETURN_FAIL);
    }
    
    if (RETURN_FAIL == PowerOffDetec())
    {
        return(RETURN_FAIL);
    }
    
    SysBatteryStateCheck();

    SysResume();

    return(RETURN_OK);
}

/*
********************************************************************************
*
*                         End of SysService.c
*
********************************************************************************
*/

