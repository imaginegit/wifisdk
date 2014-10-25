/*
********************************************************************************
*                   Copyright (c) 2008,CHENFEN
*                         All rights reserved.
*
* File Name: FunUSB.c
* 
* Description: 
*
* History:      <author>          <time>        <version>       
*             chenfen          2008-3-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_FUNUSB_
#include "SysInclude.h"
#include "FsInclude.h"

#include "USBConfig.h"

#ifdef USB

//#define RE_CONNECT

#include "FunUSB.h"
#include "FunUSBInterface.h"
#include "MainMenu.h"

/*
--------------------------------------------------------------------------------
  Function name : 
  Author        : chenfen
  Description   : USB device descriptor modify
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
               chenfen        2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
uint8 SCSI_INQUIRY[] DRAM_USB_TABLE=
{
    0x00,               //Peripheral Device Type    0, 5:CD-ROM
    0x80,               //RMB   1
    0x00,               //ISO Version+ECMA Version+ANSI Version 2
    0x00,               //Response Data Format  3
    0x1f,               //Additional Length(31) 4

    0x00,0x00,0x00,         //reserved  5~7

    'R','o','c','k','C','h','i','p',    //Vendor Information        8~15

    'U','S','B',' ','M','P','3',        //Product Idernification    16~31
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',

    '1','.','0','0'         //version : 1.00    //32~35
};

uint8 SCSI_INQUIRY_SD[] DRAM_USB_TABLE=
{
    0x00,               //Peripheral Device Type    0
    0x80,               //RMB   1
    0x00,               //ISO Version+ECMA Version+ANSI Version 2
    0x01,               //Response Data Format  3
    0x1f,               //Additional Length(31) 4

    0x00,0x00,0x00,         //reserved  5~7

    'R','o','c','k','C','h','i','p',    //Vendor Information        8~15

    'U','S','B',' ',' ','S','D',    //Product Idernification    16~31
    ' ',' ',' ',' ',' ',' ',' ',' ',' ',

    '1','.','0','0'         //version : 1.00    //32~35
};

#ifdef _CDROM_
IRAM_USB 
uint8 SCSI_INQUIRY_CDROM[]=
{
    0x05,
    0x00,               //RMB   1
    0x04,               //ISO Version+ECMA Version+ANSI Version 2
    0x00,               //Response Data Format  3
    0x1f,               //Additional Length(31) 4

    0x00,0x00,0x00,         //reserved  5~7

    'R','o','c','k','C','h','i','p',    //Vendor Information        8~15

    'U','S','B',' ','C','D','-',    //Product Idernification    16~31
    'R','O','M',' ',' ',' ',' ',' ',' ',

    '1','.','0','0'         //version : 1.00    //32~35
};
#else
uint8 SCSI_INQUIRY_CDROM[1];
#endif

uint8 StringProduct[] DRAM_USB_TABLE=
{
	18,
	0x03,

	'R', 0x00,
	'O', 0x00,
	'C', 0x00,
	'K', 0x00,
	' ', 0x00,
	'M', 0x00,
	'P', 0x00,
	'3', 0x00
};

// string descriptor
uint8 StringManufacture[] DRAM_USB_TABLE=
{
	18,
	0x03,

	'R', 0x00,
	'o', 0x00,
	'c', 0x00,
	'k', 0x00,
	'C', 0x00,
	'h', 0x00,
	'i', 0x00,
	'p', 0x00
};

uint8 StringSerialNumbel[] DRAM_USB_TABLE=
{
	18,
	0x03,

	'U', 0x00,
	'S', 0x00,
	'B', 0x00,
	'V', 0x00,
	'1', 0x00,
	'.', 0x00,
	'0', 0x00,
	'0', 0x00
};

/*
--------------------------------------------------------------------------------
  Function name : 
  Author        : chenfen
  Description   : USB device descriptor modify.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
               chenfen        2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
//device descriptor
USB_DEVICE_DESCRIPTOR DeviceDescr DRAM_USB_TABLE=
{
	sizeof(USB_DEVICE_DESCRIPTOR),				//descriptor's size 18(1B)
	USB_DEVICE_DESCRIPTOR_TYPE,				    //descriptor's type 01(1B)
    0x0110,                                     //USB plan distorbution number (2B)
	0,                                          //USB_CLASS_CODE_TEST_CLASS_DEVICE,					
	                                            //1type code (point by USB)(1B),0x00
	0, 0,										//child type and protocal (usb alloc)(2B)
	EP0_PACKET_SIZE,							//endpoint 0 max package length(1B)
    0x071b,
    0x3203,                                     //PID, rk28xx use 0x3228
    0x0100,
	iManufactuer, iProduct, iSerialNumber,		//producter,produce,device serial number index(3B)
	1											//feasible configuration parameter(1B)
};

//high speed device descriptor
USB_DEVICE_DESCRIPTOR HSDeviceDescr DRAM_USB_TABLE=
{
	sizeof(USB_DEVICE_DESCRIPTOR),				//descriptor's size 18(1B)
	USB_DEVICE_DESCRIPTOR_TYPE,				    //descriptor's type 01(1B)
	0x0200,								         //USB plan distorbution number (2B)
	0,                                          //1type code (point by USB)(1B),0x00
	0, 0,										//child type and protocal (usb alloc)(2B)
	EP0_PACKET_SIZE,							//endpoint 0 max package length(1B)
	0x071b,
	0x3203,
	0x0100,								        // device serial number 
	iManufactuer, iProduct, iSerialNumber,	    //producter,produce,device serial number index(3B)
	1											//feasible configuration parameter(1B)
};



/*
--------------------------------------------------------------------------------
  Function name : void UsbBusyHook(void)
  Author        : ZhengYongzhi
  Description   : USB busy status flag function
                  
  Input         : 
  Return        : null

  History:     <author>         <time>         <version>       
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void UsbBusyHook(void)
{

}

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
uint32 ttttt = 0;
DRAM_USB uint32 UsbChargeSystickCnt;
IRAM_USB
void UsbReadyHook(void)
{
    UINT32 batt_adcval;
    uint32 alarm_state;
    uint32 temp;

    if ((SysTickCounter - UsbChargeSystickCnt) > 10)
    {
        UsbChargeSystickCnt = SysTickCounter;
        
        //batt_adcval = (UINT32)(Adc->ADC_DATA);
        alarm_state = Scu_DCout_Issue_State_Get();

        //Adc->ADC_CTRL &= ~ADC_START;
        //temp = Adc->ADC_CTRL & (~ADC_CH_MASK);
        //Adc->ADC_CTRL = temp | ADC_CHANEL_BATTERY | ADC_START;

        if (SysTickCounter - ttttt > 500)
        {
            ttttt = SysTickCounter;
            //printf("batt_adcval=%d, gBatt_Value,=%d, charge_state=%d, alarmstate=0x%x\n", 
            //       batt_adcval,gBattery.Batt_Value,Scu_Charge_State_Get(),alarm_state);
        }
    }
}

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
IRAM_USB
void UsbConnectedHook(void)
{
    
}

/*
--------------------------------------------------------------------------------
  Function name : UsbBGetVetsion
  Author        : ZhengYongzhi
  Description   : USB get system version number. 

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
extern uint16 LoaderVer;
IRAM_USB
void UsbGetVetsion(PRKNANO_VERSION pVersion)
{
    pVersion->dwBootVer = (uint16)LoaderVer;
    //pVersion->dwFirmwareVer = ((((uint32)gSysConfig.SlaveVersion) << 8) | ((uint32)gSysConfig.SmallVersion) & 0xff);
    pVersion->dwFirmwareVer = (((uint32)gSysConfig.MasterVersion & 0xFF) << 24) | 
                              (((uint32)gSysConfig.SlaveVersion  & 0xFF) << 16) | 
                               ((uint32)gSysConfig.SmallVersion & 0xFFFF);
}

IRAM_USB
void UsbGetMacAddr(uint8 *pAddr)
{
    ;
}

/*
--------------------------------------------------------------------------------
  Function name : void UsbBusyHook(void)
  Author        : ZhengYongzhi
  Description   : USB busy status display function

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void UsbRKMSCHook(uint32 cmd, uint32 param)
{
    if (cmd == 0xFFFFFFFE)
    {
        if (0 == param)
            SendMsg(MSG_MES_FIRMWAREUPGRADE);//system backgroud check this message ,and execute the firmware update
        else
            SendMsg(MSG_ENTER_LOADER_USB);
    }
    else if (cmd == 0xFFFFFFEE)// Get mac address
    {
        
    }
    #ifdef _CDROM_    
    else if (cmd == 0xFFFFFFF6)       // Show User Disk, not enum the cdrom
    {
        gSysConfig.bShowCdrom = 0;
        SendMsg(MSG_SYS_REBOOT); 
    }
    #endif    
}


/*
--------------------------------------------------------------------------------
  Function name : void FunUSBCPUInit(void)
  Author        : chenfen
  Description   : USB CPU frequency initial
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                chenfen      2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBCPUInit(void)
{
    FunUsbLdoBak = ScuLDOSet(SCU_LDO_33);
    PMU_EnterModule(PMU_USB);  

    ScuClockGateCtr(CLOCK_GATE_USBPHY, 1);
    ScuClockGateCtr(CLOCK_GATE_USBBUS, 1);

    Scu_otgphy_suspend(0);
    
    FLASH_PROTECT_OFF();
}

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBCPUDeinit(void)
{
    FLASH_PROTECT_ON();
    
    Scu_otgphy_suspend(1);

    ScuClockGateCtr(CLOCK_GATE_USBPHY, 0);
    ScuClockGateCtr(CLOCK_GATE_USBBUS, 0);
    
    PMU_ExitModule(PMU_USB);
    ScuLDOSet(FunUsbLdoBak);
}

/*
--------------------------------------------------------------------------------
  Function name : void FunUSBIntInit(void)
  Author        : chenfen
  Description   : USB
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                chenfen      2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
extern SYSTICK_LIST AdcKey_SysTimer;
IRAM_USB
void FunUSBIntInit(void)
{
    //MainMenu中断初始化
    SetPowerOffTimerDisable();
    SystickTimerStop(&AdcKey_SysTimer);
    
    IntRegister(INT_ID23_USB, (void*)usb_handle_intr);
}

/*
--------------------------------------------------------------------------------
  Function name : void FunUSBIntInit(void)
  Author        : chenfen
  Description   : USB
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                chenfen      2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBIntDeInit(void)
{
    IntDisable(INT_ID23_USB);
    IntUnregister(INT_ID23_USB);  
    IntPendingClear(INT_ID23_USB);

    UsbConnected = 0;
    
    SystickTimerStart(&AdcKey_SysTimer);
    SetPowerOffTimerEnable();
}

IRAM_USB 
BOOL UsbShowCdrom(void)
{
#ifdef _CDROM_
    return bShowCdrom;
#else
    return 0;
#endif 

}

/*
--------------------------------------------------------------------------------
  Function name : void FunUSBInstallDevice(void)
  Author        : chenfen
  Description   : install letter
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                chenfen      2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBMDNotify(uint16 major, uint32 event, uint32 param)
{
    if (MD_EVENT_CHANGE == event /*&& 0==param*/)
    {
        uint32 i;

        for (i=0; i<USBDisk.LunNum; i++)
        {
            if (major == MD_MAJOR(USBDisk.DiskID[i]))
            {
                USBDisk.changed[i] = 1; 
            }
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void FunUSBInstallDevice(void)
  Author        : chenfen
  Description   : 安装盘符
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
                chenfen      2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBInstallDevice(void)
{
    memset(&USBDisk, 0, sizeof(USBDisk));
    
    MDScanDev(1);
    USBDisk.LunNum = MDGetUDiskTab(&USBDisk.DiskID[0]);
    
    MDRegNotify(FunUSBMDNotify);

}

/*
--------------------------------------------------------------------------------
  Function name : void FunUSBInit(void)
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
               chenfen       2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBInit(void *pArg)
{
    void(*pgUpgrade)(void);
    
    if (((USB_WIN_ARG*)pArg)->FunSel == FUN_USB_UPGRADE)
    {
        SendMsg(MSG_MES_FIRMWAREUPGRADE);
    }
    else if (((USB_WIN_ARG*)pArg)->FunSel == FUN_USB_DISK)
    {
//        DispPictureWithIDNum(IMG_ID_USB_BACKGROUND);       //display usb picture
        
        ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
    
        FunUSBCPUInit();        //CPU frequency initial
        FunUSBIntInit();        //interrupt initial
        FunUSBInstallDevice();
        USBInit(1);              //initial program finish,call main code
        
        FunUSBDisconnect();
        
        #ifndef RE_CONNECT
        FunUSBIntDeInit();
        FunUSBCPUDeinit();
        #endif
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuDeInit(void)
  Author        : ZHengYongzhi
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBDeInit(void)
{   
    #ifdef RE_CONNECT
    FunUSBIntDeInit();
    FunUSBCPUDeinit();
    #endif

    MDUnRegNotify(FunUSBMDNotify);
    
    //zyz: After USB Transfer, need get Flash & Card free memory
    SendMsg(MSG_FLASH_MEM_UPDATE);
    SendMsg(MSG_SDCARD_MEM_UPDATE);    
}

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/


/*
--------------------------------------------------------------------------------
  Function name : void FunUSBDisconnect(void)
  Author        : chenfen
  Description   : USB disconnect
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
               chenfen        2009/03/16         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBDisconnect(void)
{
    FileSysSetup(FLASH0);
    #ifdef _CDROM_
    bShowCdrom = 1;
    #endif
    //gSysConfig.FlashFreeMemory = GetFreeMem(0);
}

/*
--------------------------------------------------------------------------------
  Function name : FunUSBSysReboot
  Author        : ZhengYongzhi
  Description   : USB get system version number

  Input         :
  Return        : null

  History:     <author>         <time>         <version>
             ZhengYongzhi      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBSysReboot(uint32 addr, uint32 param)
{
    void(*pgUpgrade)(void);
    uint8 temp, i;

    //if (0 == gSysConfig.bShowCdrom)
    {
        //save all system setting before update
        ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
        SaveSysInformation();
        MDDeInitAll();
    }
    
//    LcdStandby();
    Codec_DeInitial();
    
    //disable all interrupt
    nvic->SysTick.Ctrl     &= ~NVIC_SYSTICKCTRL_TICKINT;
    nvic->Irq.Disable[0]    = 0xffffffff;       
    nvic->Irq.Disable[1]    = 0xffffffff;
    nvic->Irq.Disable[2]    = 0xffffffff;
    nvic->Irq.Disable[3]    = 0xffffffff;
    nvic->Irq.Disable[4]    = 0xffffffff;
    nvic->Irq.Disable[5]    = 0xffffffff;
    nvic->Irq.Disable[6]    = 0xffffffff;
    nvic->Irq.Disable[7]    = 0xffffffff;
    
    //open all module
	Scu->CLKGATE[0] = 0xffff0000;
	Scu->CLKGATE[1] = 0xffff0000;
    Scu->SOFTRST[0] = 0xffff0200;
    Scu->SOFTRST[0] = 0xffff0000;
    Scu->SOFTRST[1] = 0xffff001E;
    Scu->SOFTRST[1] = 0xffff0000;
    PMU_EnterModule(PMU_USB);
      
    nvic->VectorTableOffset = (UINT32)0x00000000;

    if (addr == 0x0000008c)//firmware update.
    {
        *((UINT32 *)0X03000000) = chip_freq.armclk;
        pgUpgrade = (void *)(*((uint32 *)addr)); 

        (*pgUpgrade)();
    }
    else
    {
        if (1 == param) //enter loader usb
        {
            Grf->SOFT_DBG0 = (0x18AF|0xFFFF0000);
        }
    
        nvic->APIntRst = 0x05fa0007;       //system reseet.
    }
    
    while (1);
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 MainMenuService(void)
  Author        : ZHengYongzhi
  Description   : it is USB service handle that is used to handle window message,window service task etc.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
UINT32 FunUSBService(void)
{
    uint32  RetVal;
    TASK_ARG TaskArg;
    
    RetVal = RETURN_OK;

	//check battery status,send message that charge status.
	//check Vbus status,pull out USB,return to main interface.
    if (CheckVbus() == 0)
    {
        TaskArg.MainMenu.MenuID = MAINMENU_ID_NET_RADIO;
        TaskSwitch(TASK_ID_MAINMENU, &TaskArg);//enter main menu
        RetVal = 1;
    }

    if ((GetMsg(MSG_MES_FIRMWAREUPGRADE)) && (CheckVbus() != 0))
    {
        FunUSBSysReboot(0x0000008c, 0);
    }

    if (GetMsg(MSG_ENTER_LOADER_USB) && (CheckVbus() != 0))
    {
        FunUSBSysReboot(0x00000000, 1);
    }
    
    if (GetMsg(MSG_SYS_REBOOT))
    {
        FunUSBSysReboot(0x00000000, 0);
    }

    #ifdef RE_CONNECT
    if (UsbConnected != 0)
    {
        DispPictureWithIDNum(IMG_ID_USB_BACKGROUND);       //display usb piture
        IntDisable(FAULT_ID15_SYSTICK);
        USBInit(0); 
    }
    #endif

    RetVal = 1;
    
    return (RetVal);
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 MainMenuKey(void)
  Author        : ZHengYongzhi
  Description   : monitor the key handle of charge status
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
UINT32 FunUSBKey(void)
{
    uint32 RetVal;
    uint32 UsbKeyVal;
    TASK_ARG TaskArg;
    
    RetVal = RETURN_OK;
    
    UsbKeyVal =  GetKeyVal();
    
    switch (UsbKeyVal) 
    {
        case KEY_VAL_MENU_DOWN:
        case KEY_VAL_PLAY_DOWN:
        case KEY_VAL_FFD_DOWN:
        case KEY_VAL_FFW_DOWN:
        case KEY_VAL_UP_DOWN:
        case KEY_VAL_DOWN_DOWN:
        case KEY_VAL_VOL_DOWN:
            TaskArg.MainMenu.MenuID = MAINMENU_ID_NET_RADIO;
            TaskSwitch(TASK_ID_MAINMENU, &TaskArg);//enter main menu
            RetVal = RETURN_FAIL;
            break;
            
        default: 
            break;
    }
    
    return (RetVal);
}

/*
--------------------------------------------------------------------------------
  Function name : void MainMenuDisplay(void)
  Author        : ZHengYongzhi
  Description   : main menu display handle program
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void FunUSBDisplay(void)
{
	
}

IRAM_SD
void SDDelayUs(void)
{
    //DelayUs(180);
}

/*
********************************************************************************
*
*                         End of MainMenu.c
*
********************************************************************************
*/
#endif

