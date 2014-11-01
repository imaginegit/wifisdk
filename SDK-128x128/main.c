/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   main.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_MAIN_

#include "SysInclude.h"
#include "FsInclude.h"
#include "MainMenu.h"
#include "PowerOn_Off.h"
#include "Hold.h"
#include "protocol.h"


/*
--------------------------------------------------------------------------------
  Function name : __error__(char *pcFilename, unsigned long ulLine)
  Author        : ZHengYongzhi
  Description   : test program.
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
#ifdef DEBUG
_ATTR_SYS_CODE_
void __error__(char *pcFilename, unsigned long ulLine)
{
    while(1);
}
#endif

_ATTR_SYS_INIT_DATA_ uint16 MediaInitStr[]   = {'M','e','d','i','a',' ','I','n','i','t','.','.','.',0};
_ATTR_SYS_INIT_DATA_ uint16 LowFormatStr[]   = {'L','o','w','F','o','r','m','a','.','.','.',0};
_ATTR_SYS_INIT_DATA_ uint16 SysAreaCopyStr[] = {'S','y','s','A','r','e','a','C','o','p','y','.','.','.',0};

//lable must be 8.3 format.
_ATTR_SYS_INIT_DATA_ uint8  VolumeName[] =
{
	'R', 
	'o', 
	'c', 
	'k', 
	'C', 
	'h', 
	'i', 
	'p', 
    '\0'
};    
    
/*
--------------------------------------------------------------------------------
  Function name : void SysCpuInit(void)
  Author        : ZHengYongzhi
  Description   : PLL seting. disable interruption,PWM Etc.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysCpuInit(void)
{   
    Scu_OOL_Power_Set(1);    //power on

    ScuLDOEnable();
    ScuDCoutEnable();
    PMU_Enable();
    
    Scu_Force_PWM_Mode_Set(0);
    Scu_Threshold_Set(PFM_26MA);
    Scu_Battery_Sns_Set(1);
    
    SysFreqSet(24);
    SetAdcFreq(1);          // 1M
    SetI2sFreq(12);         // I2s clk = 12M
    SetUartFreq(24);        // uart clk = 24M

    Scu_otgphy_suspend(1);
    Scu_mic_pga_volset(0);
    Scu_mic_boosten_set(0);
    Scu_mic_pga_powerup(0);  
    
    ScuClockGateInit();
 }

/*
--------------------------------------------------------------------------------
  Function name : void SysVariableInit(void)
  Author        : ZHengYongzhi
  Description   : system global variable initialization.
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysVariableInit(void)
{
    //auto power off time initalization.
    HoldState                      = HOLD_STATE_OFF;
    SysTickCounter                 = 0;
    AutoPowerOffDisableCounter     = 0;
    
    chip_freq.armclk = 0;

    QpwNumber = 0;

#ifdef MOUDLE_3LED
    TimingMark = 0;
    OpenTimer = 0;
    CloseTimer = 0;
    ColorMark = 0;
    memset(StartColor, 0, sizeof(StartColor));
    CurrentTimer = 0;
#endif
    SoftApStart = 0;
    
/*    LCD_TEXTFORT   = 0xffff;
    LCD_IMAGEINDEX = 0xffff;*/

    DCDCLastLevel  = (Grf->MPMU_PWR_CON & MPMU_PWR_VID_BUCK_MASK) >> 6;
    LDOLastLevel   = (Grf->MPMU_PWR_CON & MPMU_PWR_VID_LDO_MASK ) >> 0;
}

/*
--------------------------------------------------------------------------------
  Function name : BoardInit
  Author        : ZHengYongzhi
  Description   : hardware initializtion
                  GPIO and AD initializtion£¬low power level check.
                  LCD and Codec initializtion.
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void GpioInit(void)
{
    //=========================================================
    //WIFI Power Initial: PowerDown, SDMMC Low Level
    //SDMMC
    
    GpioMuxSet(SDMMC_POWER_EN,IOMUX_GPIOA0_SDMMC_PWREN);
    
    GpioMuxSet(SDMMC_PORT0_CMD,IOMUX_GPIOC1_IO);
    GpioMuxSet(SDMMC_PORT0_CLK,IOMUX_GPIOC2_IO);
    GpioMuxSet(SDMMC_PORT0_DATA0,IOMUX_GPIOC3_IO);
    GpioMuxSet(SDMMC_PORT0_DATA1,IOMUX_GPIOC4_IO);
    GpioMuxSet(SDMMC_PORT0_DATA2,IOMUX_GPIOC5_IO);
    GpioMuxSet(SDMMC_PORT0_DATA3,IOMUX_GPIOC6_IO);

    Gpio_SetPinDirection(SDMMC_PORT0_CMD,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_CLK,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_DATA0,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_DATA1,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_DATA2,GPIO_OUT);
    Gpio_SetPinDirection(SDMMC_PORT0_DATA3,GPIO_OUT);

    Gpio_SetPinLevel(SDMMC_PORT0_CMD,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_CLK,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_DATA0,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_DATA1,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_DATA2,GPIO_LOW);
    Gpio_SetPinLevel(SDMMC_PORT0_DATA3,GPIO_LOW); 

    //WIFI Host wakeup: hold low level after powerup
    GpioMuxSet(WIFI_HOST_WAKE,IOMUX_GPIOB1_IO);
    Gpio_SetPinDirection(WIFI_HOST_WAKE,GPIO_OUT);
    Gpio_SetPinLevel(WIFI_HOST_WAKE,GPIO_LOW);

    //WIFI RegOn: hold low level after powerup
    GpioMuxSet(WIFI_REG_ON,IOMUX_GPIOB2_IO);
    Gpio_SetPinDirection(WIFI_REG_ON,GPIO_OUT);
    Gpio_SetPinLevel(WIFI_REG_ON,GPIO_LOW);

    //WIFI Power Down
    Gpio_SetPinDirection(WIFI_POWER, GPIO_OUT);
    Gpio_SetPinLevel(WIFI_POWER, GPIO_LOW);

    //=========================================================
    //BT PowerDown, BT status, BT control hold to low Level    
    //status
    GpioMuxSet(BT_STATUS0,IOMUX_GPIOB6_IO);
	GpioMuxSet(BT_STATUS1,IOMUX_GPIOB7_IO);
	GpioMuxSet(BT_STATUS2,IOMUX_GPIOC0_IO);

	Gpio_SetPinDirection(BT_STATUS0,GPIO_OUT);
	Gpio_SetPinDirection(BT_STATUS1,GPIO_OUT);
	Gpio_SetPinDirection(BT_STATUS2,GPIO_OUT);

	Gpio_SetPinLevel(BT_STATUS0,GPIO_LOW);
	Gpio_SetPinLevel(BT_STATUS1,GPIO_LOW);
	Gpio_SetPinLevel(BT_STATUS2,GPIO_LOW);

    //control
	GpioMuxSet(BT_CONTROL1,IOMUX_GPIOD0_IO);
	GpioMuxSet(BT_CONTROL2,IOMUX_GPIOD0_IO);
	GpioMuxSet(BT_CONTROL3,IOMUX_GPIOD0_IO);
	    
	Gpio_SetPinDirection(BT_CONTROL1,GPIO_OUT);
	Gpio_SetPinDirection(BT_CONTROL2,GPIO_OUT);
	Gpio_SetPinDirection(BT_CONTROL3,GPIO_OUT);

	Gpio_SetPinLevel(BT_CONTROL1, GPIO_LOW);
	Gpio_SetPinLevel(BT_CONTROL2, GPIO_LOW);
	Gpio_SetPinLevel(BT_CONTROL3, GPIO_LOW);

    //BT Power Down
	GpioMuxSet(BT_POWER,IOMUX_GPIOD1_IO);
	Gpio_SetPinDirection(BT_POWER,GPIO_OUT);
	Gpio_SetPinLevel(BT_POWER, GPIO_LOW);

    //=========================================================
    //nand flash iomux
    #ifdef _NANDFLASH_
    GpioMuxSet(GPIOPortA_Pin0,IOMUX_GPIOA0_FLAS_CS0);
    GpioMuxSet(GPIOPortA_Pin1,IOMUX_GPIOA1_FLAS_CS1);
    GpioMuxSet(GPIOPortA_Pin2,IOMUX_GPIOA2_FLAS_ALE);
    GpioMuxSet(GPIOPortA_Pin3,IOMUX_GPIOA3_FLAS_RDY);
    GpioMuxSet(GPIOPortA_Pin4,IOMUX_GPIOA4_FLAS_RDN);
    GpioMuxSet(GPIOPortA_Pin5,IOMUX_GPIOA5_FLAS_WRN);
    GpioMuxSet(GPIOPortA_Pin6,IOMUX_GPIOA6_FLAS_CLE);
    #else
    //eMMC
    
    //SPI
    #endif

    //lcd iomux
    GpioMuxSet(GPIOPortA_Pin7,IOMUX_GPIOA7_LCD_CSN);
    GpioMuxSet(GPIOPortB_Pin0,IOMUX_GPIOB0_LCD_WRN);

    //=========================================================
    //uart iomux
    #if 1
    GpioMuxSet(GPIOPortB_Pin5,IOMUX_GPIOB5_UART_TXD);
    //GpioMuxSet(GPIOPortB_Pin6,IOMUX_GPIOB6_UART_RXD);     //used for BT_STATUS0
    #else
    GpioMuxSet(GPIOPortB_Pin5,IOMUX_GPIOB5_IO);
    Gpio_SetPinDirection(GPIOPortB_Pin5,GPIO_OUT);
    Gpio_SetPinLevel(GPIOPortB_Pin5, GPIO_LOW);
    #endif

    //=========================================================
    //pwm iomux
    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD3_IO);
    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD2_IO);
	Gpio_SetPinDirection(BACKLIGHT_PIN , GPIO_OUT);
	Gpio_SetPinLevel(BACKLIGHT_PIN, GPIO_LOW);
    
    //=========================================================
    //spi iomux
    #if 0
    GpioMuxSet(GPIOPortB_Pin1,IOMUX_GPIOB1_SPI_CSN1);
    GpioMuxSet(GPIOPortB_Pin2,IOMUX_GPIOB2_SPI_CLKP1);
    GpioMuxSet(GPIOPortB_Pin3,IOMUX_GPIOB3_SPI_TXDP1);
    GpioMuxSet(GPIOPortB_Pin4,IOMUX_GPIOB4_SPI_RXDP1);
    #else
    GpioMuxSet(GPIOPortB_Pin1,IOMUX_GPIOB1_IO);
    GpioMuxSet(GPIOPortB_Pin2,IOMUX_GPIOB2_IO);
    GpioMuxSet(GPIOPortB_Pin3,IOMUX_GPIOB3_IO);
    GpioMuxSet(GPIOPortB_Pin4,IOMUX_GPIOB4_IO);
    
    Gpio_SetPinDirection(GPIOPortB_Pin1,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortB_Pin2,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortB_Pin3,GPIO_IN);
    Gpio_SetPinDirection(GPIOPortB_Pin4,GPIO_IN);
    
    GPIO_SetPinPull(GPIOPortB_Pin1,0);
    GPIO_SetPinPull(GPIOPortB_Pin2,0);
    GPIO_SetPinPull(GPIOPortB_Pin3,0);
    GPIO_SetPinPull(GPIOPortB_Pin4,0);
    #endif
	
    //pwm iomux
    GpioMuxSet(BACKLIGHT_PIN,IOMUX_GPIOD3_IO);
	Gpio_SetPinDirection(BACKLIGHT_PIN , GPIO_OUT);
	Gpio_SetPinLevel(BACKLIGHT_PIN, GPIO_LOW);
    
    //SD Dect
    GpioMuxSet(GPIOPortD_Pin2, IOMUX_GPIOD2_IO);
    Gpio_SetPinDirection(GPIOPortD_Pin2, GPIO_IN);
    GPIO_SetPinPull(GPIOPortD_Pin2, 1);
    
    //8bit data port iomux
    //DataPortMuxSet(IO_LCD);    
}

/*
--------------------------------------------------------------------------------
  Function name : BoardInit
  Author        : ZHengYongzhi
  Description   : hardware initialization.
                  GPIO and AD initialization. low power level check.
                  lcd and Codec initial.
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void BoardInit(void)
{
    //Gpio initialization.
    GpioInit();

    UARTInit(115200,UART_DATA_8B,UART_ONE_STOPBIT,UART_PARITY_DISABLE);
    DEBUG("board initial...");
    
    //ADC initialization.
    AdcInit();
    
    //Codec initialization.
    I2sCs(I2S_IN);
    I2SInit(I2S_SLAVE_MODE);
    ScuClockGateCtr(CLOCK_GATE_I2S, 0);
    
    Codec_PowerOnInitial();
	
    //key initialization.
    KeyInit();
}

/*
--------------------------------------------------------------------------------
  Function name : BoardInit
  Author        : ZHengYongzhi
  Description   : Flash and file system Etc initialization.
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void MediaInit(void)
{
    HANDLE  hMainFile;

    DEBUG("Enter");    
    
    PMU_EnterModule(PMU_MAX);
    
    MemDevInit();
    
    SetDataDiskSize(SYS_DATA_DISK_SIZE);

    LoadSysInformation();
    
    gSysConfig.Memory = FLASH0;

    ModuleOverlay(MODULE_ID_FILE_FIND, MODULE_OVERLAY_ALL);
    FileSysSetup(gSysConfig.Memory);
	
    //system poweron, need get memory info
    FlashTotalMemory();
    SendMsg(MSG_FLASH_MEM_UPDATE);
    SendMsg(MSG_SDCARD_MEM_UPDATE);

    FLASH_PROTECT_ON();
    
    PMU_ExitModule(PMU_MAX);
    
}

/*
--------------------------------------------------------------------------------
  Function name : BoardInit
  Author        : ZHengYongzhi
  Description   : Flash and file system Etc initialization.
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SystemInit(void)
{
    uint16 TempColor;
    uint8  temp;
    uint32 i;

    //power key detect
    if(0 == CheckVbus())
    {
        if(Scu_PlayOn_Level_Get())
        {
            Scu_OOL_Power_Set(0);
            while(1);
        } 
    }

    ScuClockGateCtr(CLOCK_GATE_EFUSE, 1);
    EfuseReadData = Scu_Efuse_Read(4);
    DEBUG("eFuse Read Data = 0x%x", EfuseReadData);
    for (i = 0; i < 3; i++)
    {
        temp = Scu_Efuse_Read(4);
        DEBUG("eFuse Read Data = 0x%x", temp);
        if (temp != EfuseReadData)
        {
            i = 0;
            EfuseReadData = temp;
            DEBUG("eFuse Read Error");
        }
    }
    DEBUG("eFuse Save Data = 0x%x", gSysConfig.eFuseSaveData);
    if (gSysConfig.eFuseSaveData == 0xffffffff)
    {
        gSysConfig.eFuseSaveData = EfuseReadData & 0xff;
    }
    ScuClockGateCtr(CLOCK_GATE_EFUSE, 0);
    
    ScuDCoutSet(SCU_DCOUT_120);
    ScuLDOSet(SCU_LDO_30);
    
/*    TempColor = LCD_GetColor();
    
    LCD_SetCharSizeInit();
    LCD_SetCharSize(FONT_12x12);
    LCD_SetImageIndex((uint16)(gSysConfig.ImageIndex));
    LCD_SetColor(COLOR_WHITE);

    PowerOnLcdinit();
    if (0 == CheckVbus())
    {
        DisplayPowerOnFlash();
    }
    
    LCD_SetColor(TempColor);*/
    GpioMuxSet(LED_BL_EN, Type_Gpio);
    Gpio_SetPinDirection(LED_BL_EN, GPIO_OUT);
    GpioMuxSet(SD_DET, IOMUX_GPIOD2_PWM1);
    GpioMuxSet(BACKLIGHT_PIN, IOMUX_GPIOD3_PWM2);
    
    PWM_Start(1);
}

/*
--------------------------------------------------------------------------------
  Function name : void ModuleInit(void)
  Author        : ZHengYongzhi
  Description   : power on module initialization, breakpoint resume.
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void ModuleInit(void)
{
    TASK_ARG TaskArg;
    
  if (CheckVbus())
    {
        UsbAdpterProbe();
    }
    else
    {
        TaskArg.MainMenu.MenuID = MAINMENU_ID_NET_RADIO;
    	TaskSwitch(TASK_ID_MAINMENU, &TaskArg);
    }
    
    pMainThread = (THREAD*)NULL;
}

/*
--------------------------------------------------------------------------------
  Function name : void SysTickInit(void)
  Author        : ZHengYongzhi
  Description   : system tick initialization.
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysTickInit(void)
{
    SysTickDisable();
    
    SysTickClkSourceSet(NVIC_SYSTICKCTRL_CLKIN);
    SysTickPeriodSet(10); //10
    
    SysTickEnable();
    
    DEBUG("Systick initial: periodset 10ms");
}

/*
--------------------------------------------------------------------------------
  Function name : void IntrruptInit(void)
  Author        : ZHengYongzhi
  Description   : system breakpoint initialization
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void SysInterruptInit(void)
{
    IntMasterEnable();
    
    //set priority.
    IrqPriorityInit();
    //......
    //register interrupt functions.
    IntRegister(FAULT_ID15_SYSTICK, (void*)SysTickHandler);
    IntRegister(INT_ID16_DMA,       (void*)DmaInt);
    //IntRegister(INT_ID26_GPIO, (void*)GpioInt);   //test GPIO INT
    IntRegister(INT_ID30_ADC,       (void*)AdcIsr);
    
    IntRegister(INT_ID39_OOL_PLAYON,  (void*)playon_wakeup_int);
    IntRegister(INT_ID40_PWR_5V_READY,(void*)vbus_wakeup_int);
    IntRegister(INT_ID28_PWM0, (void*)pwm0_int);
    
    //interrupt enable.
    IntEnable(FAULT_ID4_MPU);
    IntEnable(FAULT_ID5_BUS);
    IntEnable(FAULT_ID6_USAGE);
    IntEnable(FAULT_ID15_SYSTICK);
    IntEnable(INT_ID16_DMA);
    //IntEnable(INT_ID26_GPIO);                     //test GPIO INT
    IntEnable(INT_ID30_ADC);
    IntEnable(INT_ID28_PWM0);

    DEBUG("system INT initial: DMA & Systick INT enable");
}

/*
--------------------------------------------------------------------------------
  Function name : void poweron(void)
  Author        : ZHengYongzhi
  Description   : system tick initialization.
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void poweron(void)
{
    uint32 ms, SysFreq;

    if(CheckVbus())
    {
        return;
    }

    ms = 50;
    
    SysFreq = GetPllSysFreq();

    ms *= SysFreq * 1000 / 3;

    do
    {
        //printf("xxx\n");
        if(Scu_PlayOn_Level_Get())
        {
            Scu_OOL_Power_Set(0);
        }        
    }while (ms--);
}


/*
--------------------------------------------------------------------------------
  Function name : INT Main(void)
  Author        : ZHengYongzhi
  Description   : this the main funtion for tasks manage,and call task main window callback function.
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_CODE_
UINT32 Main(void)
{
    //initialization for global variable.
    SysVariableInit();
    
    //PLL setting,disable interrupt and PWM.
    SysCpuInit();
    
    //ADC initializtion, low power check,LCD initialization, Codec initialization.
    BoardInit();
    
    //poweron();

    //interruption initialization.
    SysInterruptInit();
    
    //Flash initial,get system parameter, file system initial
    MediaInit();

    SystemInit();

    //module initialization.
    ModuleInit();

    //system tick initialization.
    SysTickInit();

    SysServiceInit();

    PWM0_Start();

#ifdef MOUDLE_3LED
    PMU_EnterModule(PMU_MAX);
    led_init();
    //PMU_ExitModule(PMU_MAX);
#endif

    printf("Os Start!\n");
    OSStart(&pMainWin, &pMainThread);
	
	/*for some funcition code decode into the build. so do no to delete them!!!*/
	BuildDirInfo(".");
	MakeDir(".", "dgl");
	FileCreate(".", "wifi");
	FileDelete(".", "wifi");
    microlibuse();
    
    return (TRUE);
}

/*
********************************************************************************
*
*                         End of main.c
*
********************************************************************************
*/

