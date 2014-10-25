/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   Main.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__

/*
*-------------------------------------------------------------------------------
*
*                            Module Config
*
*-------------------------------------------------------------------------------
*/
//debug compile switch.
//#define _JTAG_DEBUG_
#define _UART_DEBUG_

#ifdef _UART_DEBUG_
#define DEBUG(format,...) printf("FILE: %s, FUNCTION: %s, LINE: %d: "format"\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(format,...)    
#endif

#define WIFI_DEBUG         1
#if WIFI_DEBUG
#define MINDEBUG(format,...)printf("%s: "format" \r\n", __FUNCTION__, ##__VA_ARGS__)
#else
#define MINDEBUG(format,...)
#endif


#if 0
#define RKDEBUG(format,...) printf("%s: LINE: %d:"format" \r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define RKDEBUG(format,...)
#endif

#if 0
#define BCMDEBUG(format,...) printf("%s: LINE: %d:"format" \r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define BCMDEBUG(format,...)
#endif
/*
*-------------------------------------------------------------------------------
* Memory Device Option
* Select which memory device are used
*
*-------------------------------------------------------------------------------
*/
//#define _NANDFLASH_       //Nand Flash Support
#define _SPINOR_          //SPI NOR Flash Support
//#define _EMMC_            //EMMC Flash Support

#ifdef _NANDFLASH_
#define FW_IN_DEV       1   //Firmware Stored in: 1:nandflash 2:sipnor 3:emmc 4:sd card
//#define _SDCARD_
#endif

#ifdef _SPINOR_
#define FW_IN_DEV       2   //Firmware Stored in: 1:nandflash 2:sipnor 3:emmc 4:sd card
#endif

#ifdef _EMMC_
#define FW_IN_DEV       3   //Firmware Stored in: 1:nandflash 2:sipnor 3:emmc 4:sd card
                            //eMMC Flash not support SDCARD
#endif

/*
*-------------------------------------------------------------------------------
*
*                            Application Modules Option
*
*-------------------------------------------------------------------------------
*/
#define _MUSIC_
#define _WIFI_
#define _USB_

//the audio module compile switch.
#ifdef _MUSIC_
#define _MP3_DECODE_
#endif

#ifdef _WIFI_
#define  WPA_AUTH
#endif

//USB module compile switch.
#ifdef _USB_

#endif

/*
*-------------------------------------------------------------------------------
*
*                            Misc Option
*
*-------------------------------------------------------------------------------
*/
#define FILE_ERROR_DIALOG           //Support Dialog Display 
#define _DISP_FROM_RAM_             //Support Display Pictures from buffer when recording

/*
*-------------------------------------------------------------------------------
*
*   chip version definition.
*   
*-------------------------------------------------------------------------------
*/
#define MIC_RECORD_GAINS_SHIFTS  6              //Gain adjust range of MIC record.(0~6)
#define FM_RECORD_GAINS_SHIFTS   6              //Gain adjust range of FM record.(0~6)


/*
*-------------------------------------------------------------------------------
*
*                            LCD Driver Config
*
*-------------------------------------------------------------------------------
*/
//LCD Pixel Define
//#define _LCD_SIZE_20_

#define LCD_PIXEL_1         1
#define LCD_PIXEL_16        16

//Horizontal or Verital LCD Configuration.
#define LCD_HORIZONTAL      0
#define LCD_VERTICAL        1

//LCD Type Define
#define LCD_PIXEL           LCD_PIXEL_16
#define LCD_DIRECTION       LCD_HORIZONTAL

#define LCD_WIDTH 			128
#define LCD_HEIGHT			128

#define LCD_MAX_YSIZE		128
#define LCD_MAX_XSIZE		128

/*
*-------------------------------------------------------------------------------
*
*                            Key Driver Configuration
*
*-------------------------------------------------------------------------------
*/
//Key Num Define
#define KEY_NUM_4		    4
#define KEY_NUM_5		    5
#define KEY_NUM_6		    6
#define KEY_NUM_7		    7
#define KEY_NUM_8		    8

//Config KeyDriver
#define KEY_NUM  	        KEY_NUM_7

//Key Val Define
#define KEY_VAL_NONE        ((UINT32)0x0000)
#define KEY_VAL_PLAY        ((UINT32)0x0001 << 0) 
#define KEY_VAL_MENU        ((UINT32)0x0001 << 1) 
#define KEY_VAL_FFD         ((UINT32)0x0001 << 2) 
#define KEY_VAL_FFW         ((UINT32)0x0001 << 3) 
#define KEY_VAL_UP          ((UINT32)0x0001 << 4) 
#define KEY_VAL_DOWN        ((UINT32)0x0001 << 5) 
#define KEY_VAL_ESC         ((UINT32)0x0001 << 6) 
//#define KEY_VAL_VOL         ((UINT32)0x0001 << 7) 

#define KEY_VAL_UPGRADE     KEY_VAL_MENU
#define KEY_VAL_POWER       KEY_VAL_PLAY
#define KEY_VAL_HOLD        (KEY_VAL_MENU | KEY_VAL_PLAY)
#define KEY_VAL_VOL         KEY_VAL_ESC

#define KEY_VAL_MASK        ((UINT32)0x0fffffff)
#define KEY_VAL_UNMASK      ((UINT32)0xf0000000)

//Bit position define for AD keys.
#define KEY_VAL_ADKEY2		KEY_VAL_MENU
#define KEY_VAL_ADKEY3		KEY_VAL_UP
#define KEY_VAL_ADKEY4      KEY_VAL_FFD
#define KEY_VAL_ADKEY5      KEY_VAL_FFW
#define KEY_VAL_ADKEY6      KEY_VAL_DOWN
#define KEY_VAL_ADKEY7      KEY_VAL_ESC

//AdKey Reference Voltage define     
//ADC ref = LDO Output                        
#define ADKEY2_MIN          ((0   +   0) / 2) 
#define ADKEY2_MAX          ((0   + 113) / 2) 
                                              
#define ADKEY3_MIN          ((113 +   0) / 2) 
#define ADKEY3_MAX          ((113 + 250) / 2) 
                                              
#define ADKEY4_MIN          ((250 + 113) / 2) 
#define ADKEY4_MAX          ((250 + 395) / 2) 
                                              
#define ADKEY5_MIN          ((395 + 250) / 2) 
#define ADKEY5_MAX          ((395 + 590) / 2) 
                                              
#define ADKEY6_MIN          ((590 + 395) / 2) 
#define ADKEY6_MAX          ((590 + 725) / 2) 
                                              
#define ADKEY7_MIN          ((725 + 590) / 2) 
#define ADKEY7_MAX          ((725 + 1023) / 2)

/*
*-------------------------------------------------------------------------------
*
*                            Voltage About
*
*-------------------------------------------------------------------------------
*/
//degree for battery power.
#define BATTERY_LEVEL       5

//bind of PWM output.
#define BL_LEVEL_MAX        5       //backlight 5 degree.

#define BL_PWM_RATE_MIN     30
#define BL_PWM_RATE_MAX     80
#define BL_PWM_RATE_STEP    ((BL_PWM_RATE_MAX - BL_PWM_RATE_MIN) / (BL_LEVEL_MAX))

/*
*-------------------------------------------------------------------------------
*
*                            GPIO Config
*
*-------------------------------------------------------------------------------
*/
#define SDMMC_POWER_EN      GPIOPortA_Pin0

#define SDMMC_PORT0_CMD     GPIOPortC_Pin1
#define SDMMC_PORT0_CLK     GPIOPortC_Pin2
#define SDMMC_PORT0_DATA0   GPIOPortC_Pin3
#define SDMMC_PORT0_DATA1   GPIOPortC_Pin4
#define SDMMC_PORT0_DATA2   GPIOPortC_Pin5
#define SDMMC_PORT0_DATA3   GPIOPortC_Pin6

#define BACKLIGHT_PIN       GPIOPortD_Pin3
#define SD_DET              GPIOPortD_Pin2

//BT Control IO Define
#define BT_STATUS0          GPIOPortB_Pin6
#define BT_STATUS1          GPIOPortB_Pin7
#define BT_STATUS2          GPIOPortC_Pin0 

#define BT_CONTROL1         GPIOPortD_Pin4
#define BT_CONTROL2         GPIOPortD_Pin5
#define BT_CONTROL3         GPIOPortD_Pin0

#define BT_POWER            GPIOPortD_Pin1

//WIFI Control IO Define
#define WIFI_POWER          GPIOPortD_Pin7
#define WIFI_REG_ON         GPIOPortB_Pin2
#define WIFI_HOST_WAKE      GPIOPortB_Pin1

/*
*-------------------------------------------------------------------------------
*
*                            Language Define
*
*-------------------------------------------------------------------------------
*/
#define LANGUAGE_CHINESE_S               0      //Simplified Chinese. 
#define LANGUAGE_CHINESE_T               1      //Traditional Chinese
#define LANGUAGE_ENGLISH                 2      //Englis
#define LANGUAGE_KOREAN                  3      //Korean
#define LANGUAGE_JAPANESE                4      //Japanese
#define LANGUAGE_SPAISH                  5      //Spanish
#define LANGUAGE_FRENCH                  6      //French
#define LANGUAGE_GERMAN                  7      //German
#define LANGUAGE_ITALIAN                 8      //Italian
#define LANGUAGE_PORTUGUESE              9      //Portuguess
#define LANGUAGE_RUSSIAN                10      //Russian
        
#define LANGUAGE_SWEDISH                11      //Swedish
#define LANGUAGE_THAI                   12      //Thai
#define LANGUAGE_POLAND                 13      //Polish
#define LANGUAGE_DENISH                 14      //Danish
#define LANGUAGE_DUTCH                  15      //Dutch
#define LANGUAGE_HELLENIC               16      //Greek
#define LANGUAGE_CZECHIC                17      //Czech
#define LANGUAGE_TURKIC                 18      //Turkish
#define LANGUAGE_RABBINIC               19      //Hebrew
#define LANGUAGE_ARABIC                 20      //Arabic
#define LANGUAGE_MAX_COUNT              21
        
#define DEFAULT_LANGUE                  LANGUAGE_CHINESE_S

/*
********************************************************************************
*
*                         End of Include.h
*
********************************************************************************
*/

#endif 

