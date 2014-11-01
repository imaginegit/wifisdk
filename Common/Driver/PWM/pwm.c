/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name：   Pwm.c
* 
* Description:  C program template
*
* History:      <author>          <time>        <version>       
*             anzhiguo      2008-11-20         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_PWM_

#include "DriverInclude.h"
#include "SysInclude.h"

#if 0
/*------------------------------------------------------------------------------
*
* Virtual PWM3 ,Create by system tick and GPIO
*
------------------------------------------------------------------------------*/
uint8 pwm3protstate = 0;
int pwm3duty = 0;
void VirPWM3Init(void){
    	GpioMuxSet(EXT_PWM_GPIO, Type_Gpio);
	Gpio_SetPinDirection(EXT_PWM_GPIO, GPIO_OUT);
	Gpio_SetPinLevel(EXT_PWM_GPIO, GPIO_HIGH);
}

void VirPWM3SetDuty (int mduty){
	if((mduty>=0) && (mduty <= 100)){
		pwm3duty = mduty;
	}
}

void VirPWM3Ctrl(){
	/*pwm3duty++;
	if(pwm3duty == 3) {
		Pwm->CHANNEL[1].Tacnt = 1;
		pwm3duty = 0;
	}*/
	printf("fle--->pwm3duty = %d, cntr = %d\n", pwm3duty, Pwm->CHANNEL[1].Tacnt);
	/*if(pwm3duty < 1000) return;
	pwm3duty = 0;
	printf("pwm3 cntr = %d\n", Pwm->CHANNEL[0].Tacnt);*/
	pwm3protstate ^= 0x01;
	/*if(pwm3duty) {
		printf("fle-->pwm3protstate = %d\n", pwm3protstate);
		pwm3duty = 0;
	}*/
	Gpio_SetPinLevel(GPIOPortA_Pin0, pwm3protstate);
}

_ATTR_SYS_DATA_
SYSTICK_LIST VirPWM3FuncTimer = 
{
    NULL,
    0,
    10,		// set virtual pwm3 100Hz
    0,
    VirPWM3Ctrl,
};

void PWM3_Start(void){
	VirPWM3Init();
	//VirPWM3SetDuty(75);
	//SystickTimerStart(&VirPWM3FuncTimer);
}
#endif

void PWM0_Start(void) {
	GpioMuxSet(EXT_PWM_GPIO, Type_Gpio);
	Gpio_SetPinDirection(EXT_PWM_GPIO, GPIO_OUT);
	Gpio_SetPinLevel(EXT_PWM_GPIO, GPIO_LOW);
}

int irqcycle=0;
__irq void pwm0_int(void) {

	Pwm->CHANNEL[EXT_PWM_COM].Tacmd |= PWM_INTCLR;
	Pwm->CHANNEL[EXT_PWM_COM].Tacmd &= ~(PWM_CMODEEN | PWM_ENABLE | PWM_OUTPUT_ENABLE | PWM_TIMEINTEN);
	irqcycle++;
	if(irqcycle==1){
		Gpio_SetPinLevel(EXT_PWM_GPIO, GPIO_LOW);
	} else {
		Pwm->CHANNEL[EXT_PWM_COM].Tacnt = 0;
		Gpio_SetPinLevel(EXT_PWM_GPIO, GPIO_HIGH);
		irqcycle = 0;
	}
	Pwm->CHANNEL[EXT_PWM_COM].Tacmd |= (PWM_CMODEEN | PWM_OUTPUT_ENABLE | PWM_TIMEINTEN | PWM_ENABLE);
}

/*
--------------------------------------------------------------------------------
  Function name : void PWM_Start(int num)
  Author        : LinChongSen
  Description   : 启动PWM 输出
                  
  Input         : num -- PWM号(0~4)
                  
  Return        : *rate -- 当前PWM的占空比
                  *PWM_freq  -- 当前PWM的频率
--------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void PWM_Start(ePWM_CHN ch)
{    
    ScuClockGateCtr(CLOCK_GATE_PWM, 1);
}

/*
--------------------------------------------------------------------------------
  Function name : void PWM_Stop(int num)
  Author        : LinChongSen
  Description   : 关闭PWM 输出
                  
  Input         : num -- PWM号(0~4)
                  
  Return        : 
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void PWM_Stop(ePWM_CHN ch)
{
    Pwm->CHANNEL[ch].Tacmd &= ~(PWM_ENABLE);
    ScuClockGateCtr(CLOCK_GATE_PWM, 0);
}

#ifdef DRIVERLIB_IRAM
/*
--------------------------------------------------------------------------------
  Function name : void PwmRegReset(int num)
  Author        : LinChongSen
  Description   : 复位PWM 
                  
  Input         : num -- PWM号(0~4)
                  
  Return        : 
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void PwmRegReset(ePWM_CHN ch)
{
    Pwm->CHANNEL[ch].Tacmd |= PWM_REGRESET;
}

/*
--------------------------------------------------------------------------------
  Function name : void PwmPrescalefFctorSet(int num,int mpwmPrescalefFctor)
  Author        : LinChongSen
  Description   : PWM 分频率设置
                  
  Input         : num -- PWM号(0~4)
                  mpwmPrescalefFctor -- 0000: 1/2             0001: 1/4  (0~15)
                                        0000: 1/8             0001: 1/16
                                        0100: 1/32            0101: 1/64
                                        0110: 1/128           0111: 1/256
                                        1000: 1/512           1001: 1/1024
                                        1010: 1/2048          1011: 1/4096
                                        1100: 1/8192          1101: 1/16384
                                        1110: 1/32768         1111: 1/65536

  Return        : 
  desc          : 不调用，默认为1/2分频
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
void PwmPrescalefFctorSet(ePWM_CHN ch, ePWM_PRESCALE mpwmPrescalefFctor)
{
    Pwm->CHANNEL[ch].Tacmd |= (mpwmPrescalefFctor << 9);
}

/*
--------------------------------------------------------------------------------
  Function name : int PwmPrescalefFctorGet(int num)
  Author        : LinChongSen
  Description   : PWM 分频率读取
                  
  Input         : num -- PWM号(0~4)
  Output        : mpwmPrescalefFctor  -- 返回2的次方数 如:2，4，8，16....65536
                                         可以参考PwmPrescalefFctorSet中的mpwmPrescalefFctor注释
  Return        : 
--------------------------------------------------------------------------------
*/
_ATTR_DRIVERLIB_CODE_
uint32 PwmPrescalefFctorGet(ePWM_CHN ch)
{       
    return ((Pwm->CHANNEL[ch].Tacmd) & 0x00001E00) >> 9;
}
#endif

void pwm_set_gpio(ePWM_CHN ch, uint8 enable) {
	if(ch == PWM_CHN1) {
		if(GpioMuxGet(SD_DET)) {
			GpioMuxSet(SD_DET, IOMUX_GPIOD2_IO);
			Gpio_SetPinDirection(SD_DET, GPIO_OUT);
		}
		Gpio_SetPinLevel(SD_DET, enable);
	}
	if(ch == PWM_CHN2) {
		if(GpioMuxGet(BACKLIGHT_PIN)) {
			GpioMuxSet(BACKLIGHT_PIN, IOMUX_GPIOD3_IO);
			Gpio_SetPinDirection(BACKLIGHT_PIN, GPIO_OUT);
		}
		Gpio_SetPinLevel(BACKLIGHT_PIN, enable);
	}
}

void pwm_set_pwm(ePWM_CHN ch) {
	if(ch == PWM_CHN1) {
		if(!GpioMuxGet(SD_DET)) {
			GpioMuxSet(SD_DET, IOMUX_GPIOD2_PWM1);
		}
	}
	if(ch == PWM_CHN2) {
		if(!GpioMuxGet(BACKLIGHT_PIN)) {
			GpioMuxSet(BACKLIGHT_PIN, IOMUX_GPIOD3_PWM2);
		}
	}
}

/*
--------------------------------------------------------------------------------
  Function name : BOOL PwmRateSet(int num,UINT32 rate, UINT32 PWM_freq)
  Author        : LinChongSen
  Description   : PWM 占空比设置
                  
  Input         : num -- PWM号(0~4)
                  rate -- 高与低的比值(0~255)
                  PWM_freq  -- 当前PWM的频率(<10k)
                  
  Return        : 
  desc          : data1 过小,则无法正常设置
--------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
int32 PwmRateSet(ePWM_CHN ch, UINT32 rate, UINT32 PWM_freq)
{
    UINT32 data0,data1;
    UINT32 pwmclk;
    UINT32 pwmPrescalefFctor;
    
    if (rate == 255)
    {
        if(ch == EXT_PWM_COM) {
		Gpio_SetPinLevel(EXT_PWM_GPIO, GPIO_HIGH);
	 }
	 pwm_set_gpio(ch, GPIO_HIGH);
        Pwm->CHANNEL[ch].Tacmd &= ~(PWM_ENABLE);
        Pwm->CHANNEL[ch].Tacmd |= PWM_REGRESET;
	 
        return OK; 
    }
    else if (rate == 0)
    {
        if(ch == EXT_PWM_COM) {
		Gpio_SetPinLevel(EXT_PWM_GPIO, GPIO_LOW);
	 }
	 pwm_set_gpio(ch, GPIO_LOW);
        Pwm->CHANNEL[ch].Tacmd &= ~(PWM_ENABLE);
        Pwm->CHANNEL[ch].Tacmd |= PWM_REGRESET;
        
        return OK; 
    }
        
    if((rate > 0) && (rate < 255))
    {
    	 pwm_set_pwm(ch);
        pwmclk = GetPclkFreq();
        irqcycle=0;
        data1 = ((pwmclk * 1000/* * 1000*/) >> ((PwmPrescalefFctorGet(ch)) + 1)) / PWM_freq;
        data0 = rate * data1 / 255;

	 if(ch == EXT_PWM_COM) {
        	Pwm->CHANNEL[ch].Tacmd &= ~(PWM_CMODEEN | PWM_ENABLE | PWM_OUTPUT_ENABLE | PWM_TIMEINTEN);
	 } else {
	 	Pwm->CHANNEL[ch].Tacmd &= ~(PWM_ENABLE | PWM_OUTPUT_ENABLE | PWM_TIMEINTEN);
	 }
        Pwm->CHANNEL[ch].Tacnt = 0;
        Pwm->CHANNEL[ch].Tadata0 = data0;
        Pwm->CHANNEL[ch].Tadata1 = data1;
	 if(ch == EXT_PWM_COM) {
        	Pwm->CHANNEL[ch].Tacmd |= (PWM_CMODEEN | PWM_OUTPUT_ENABLE | PWM_TIMEINTEN | PWM_ENABLE);
	 } else {
	 	Pwm->CHANNEL[ch].Tacmd |= (PWM_OUTPUT_ENABLE | PWM_TIMEINTEN | PWM_ENABLE);
	 }
		
        return OK; 
    }

    return ERROR; 
}

/*
********************************************************************************
*
*                         End of pwm.c
*
********************************************************************************
*/

