/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   StartLoadTab.c
* 
* Description:  定义模块信息，在模块调度时需要使用
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2009-02-06         1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_STARTLOAD_

#include "SysInclude.h"

extern uint32 Load$$MODULE_INFO$$Base;

extern uint32  Load$$SYS_CODE$$Base;
extern uint32 Image$$SYS_CODE$$Base;
extern uint32 Image$$SYS_CODE$$Length;
extern uint32  Load$$SYS_DATA$$Base;
extern uint32 Image$$SYS_DATA$$RW$$Base;
extern uint32 Image$$SYS_DATA$$RW$$Length;
extern uint32 Image$$SYS_DATA$$ZI$$Base;
extern uint32 Image$$SYS_DATA$$ZI$$Length;

#ifndef _JTAG_DEBUG_
extern uint32  Load$$SYS_INIT_CODE$$Base;
extern uint32 Image$$SYS_INIT_CODE$$Base;
extern uint32 Image$$SYS_INIT_CODE$$Length;
extern uint32  Load$$SYS_INIT_DATA$$Base;
extern uint32 Image$$SYS_INIT_DATA$$RW$$Base;
extern uint32 Image$$SYS_INIT_DATA$$RW$$Length;
extern uint32 Image$$SYS_INIT_DATA$$ZI$$Base;
extern uint32 Image$$SYS_INIT_DATA$$ZI$$Length;


extern uint32 Load$$LCD_DRIVER1_CODE$$Base;
extern uint32 Image$$LCD_DRIVER1_CODE$$Base;
extern uint32 Image$$LCD_DRIVER1_CODE$$Length;
extern uint32 Load$$LCD_DRIVER1_DATA$$Base;
extern uint32 Image$$LCD_DRIVER1_DATA$$RW$$Base;
extern uint32 Image$$LCD_DRIVER1_DATA$$RW$$Length;
extern uint32 Image$$LCD_DRIVER1_DATA$$ZI$$Base;
extern uint32 Image$$LCD_DRIVER1_DATA$$ZI$$Length;

extern uint32 Load$$LCD_DRIVER2_CODE$$Base;
extern uint32 Image$$LCD_DRIVER2_CODE$$Base;
extern uint32 Image$$LCD_DRIVER2_CODE$$Length;
extern uint32 Load$$LCD_DRIVER2_DATA$$Base;
extern uint32 Image$$LCD_DRIVER2_DATA$$RW$$Base;
extern uint32 Image$$LCD_DRIVER2_DATA$$RW$$Length;
extern uint32 Image$$LCD_DRIVER2_DATA$$ZI$$Base;
extern uint32 Image$$LCD_DRIVER2_DATA$$ZI$$Length;

//USB
extern uint32  Load$$USB_CODE$$Base;
extern uint32 Image$$USB_CODE$$Base;
extern uint32 Image$$USB_CODE$$Length;
extern uint32  Load$$USB_DATA$$Base;
extern uint32 Image$$USB_DATA$$RW$$Base;
extern uint32 Image$$USB_DATA$$RW$$Length;
extern uint32 Image$$USB_DATA$$ZI$$Base;
extern uint32 Image$$USB_DATA$$ZI$$Length;

//MP3
extern uint32  Load$$MP3_DECODE_CODE$$Base;
extern uint32 Image$$MP3_DECODE_CODE$$Base;
extern uint32 Image$$MP3_DECODE_CODE$$Length;
extern uint32  Load$$MP3_DECODE_DATA$$Base;
extern uint32 Image$$MP3_DECODE_DATA$$RW$$Base;
extern uint32 Image$$MP3_DECODE_DATA$$RW$$Length;
extern uint32 Image$$MP3_DECODE_DATA$$ZI$$Base;
extern uint32 Image$$MP3_DECODE_DATA$$ZI$$Length;

//MP3 INIT
extern uint32  Load$$MP3_INIT_CODE$$Base;
extern uint32 Image$$MP3_INIT_CODE$$Base;
extern uint32 Image$$MP3_INIT_CODE$$Length;
extern uint32  Load$$MP3_INIT_DATA$$Base;
extern uint32 Image$$MP3_INIT_DATA$$RW$$Base;
extern uint32 Image$$MP3_INIT_DATA$$RW$$Length;
extern uint32 Image$$MP3_INIT_DATA$$ZI$$Base;
extern uint32 Image$$MP3_INIT_DATA$$ZI$$Length;

//MP3 COMMON
extern uint32  Load$$MP3_COMMON_CODE$$Base;
extern uint32 Image$$MP3_COMMON_CODE$$Base;
extern uint32 Image$$MP3_COMMON_CODE$$Length;
extern uint32  Load$$MP3_COMMON_DATA$$Base;
extern uint32 Image$$MP3_COMMON_DATA$$RW$$Base;
extern uint32 Image$$MP3_COMMON_DATA$$RW$$Length;
extern uint32 Image$$MP3_COMMON_DATA$$ZI$$Base;
extern uint32 Image$$MP3_COMMON_DATA$$ZI$$Length;

//Audio Encode
extern uint32  Load$$ENCODE_CODE$$Base;
extern uint32 Image$$ENCODE_CODE$$Base;
extern uint32 Image$$ENCODE_CODE$$Length;
extern uint32  Load$$ENCODE_DATA$$Base;
extern uint32 Image$$ENCODE_DATA$$RW$$Base;
extern uint32 Image$$ENCODE_DATA$$RW$$Length;
extern uint32 Image$$ENCODE_DATA$$ZI$$Base;
extern uint32 Image$$ENCODE_DATA$$ZI$$Length;

//UI MainMenu
extern uint32  Load$$MAINMENU_CODE$$Base;
extern uint32 Image$$MAINMENU_CODE$$Base;
extern uint32 Image$$MAINMENU_CODE$$Length;
extern uint32  Load$$MAINMENU_DATA$$Base;
extern uint32 Image$$MAINMENU_DATA$$RW$$Base;
extern uint32 Image$$MAINMENU_DATA$$RW$$Length;
extern uint32 Image$$MAINMENU_DATA$$ZI$$Base;
extern uint32 Image$$MAINMENU_DATA$$ZI$$Length;

//File Find
extern uint32  Load$$FINDFILE_CODE$$Base;
extern uint32 Image$$FINDFILE_CODE$$Base;
extern uint32 Image$$FINDFILE_CODE$$Length;
extern uint32  Load$$FINDFILE_DATA$$Base;
extern uint32 Image$$FINDFILE_DATA$$RW$$Base;
extern uint32 Image$$FINDFILE_DATA$$RW$$Length;
extern uint32 Image$$FINDFILE_DATA$$ZI$$Base;
extern uint32 Image$$FINDFILE_DATA$$ZI$$Length;

//Flash Write
extern uint32  Load$$FLASH_WRITE_CODE$$Base;
extern uint32 Image$$FLASH_WRITE_CODE$$Base;
extern uint32 Image$$FLASH_WRITE_CODE$$Length;
extern uint32  Load$$FLASH_WRITE_DATA$$Base;
extern uint32 Image$$FLASH_WRITE_DATA$$RW$$Base;
extern uint32 Image$$FLASH_WRITE_DATA$$RW$$Length;
extern uint32 Image$$FLASH_WRITE_DATA$$ZI$$Base;
extern uint32 Image$$FLASH_WRITE_DATA$$ZI$$Length;

//CHARGE WIN
extern uint32  Load$$CHARGE_WIN_CODE$$Base;
extern uint32 Image$$CHARGE_WIN_CODE$$Base;
extern uint32 Image$$CHARGE_WIN_CODE$$Length;
extern uint32  Load$$CHARGE_WIN_DATA$$Base;
extern uint32 Image$$CHARGE_WIN_DATA$$RW$$Base;
extern uint32 Image$$CHARGE_WIN_DATA$$RW$$Length;
extern uint32 Image$$CHARGE_WIN_DATA$$ZI$$Base;
extern uint32 Image$$CHARGE_WIN_DATA$$ZI$$Length;

//WIFI WIN UI(AP list) 
extern uint32  Load$$WIFI_AP_WIN_CODE$$Base;
extern uint32 Image$$WIFI_AP_WIN_CODE$$Base;
extern uint32 Image$$WIFI_AP_WIN_CODE$$Length;
extern uint32  Load$$WIFI_AP_WIN_DATA$$Base;
extern uint32 Image$$WIFI_AP_WIN_DATA$$RW$$Base;
extern uint32 Image$$WIFI_AP_WIN_DATA$$RW$$Length;
extern uint32 Image$$WIFI_AP_WIN_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_AP_WIN_DATA$$ZI$$Length;

//WIFI PLAY WIN UI
extern uint32  Load$$WIFI_PLAY_WIN_CODE$$Base;
extern uint32 Image$$WIFI_PLAY_WIN_CODE$$Base;
extern uint32 Image$$WIFI_PLAY_WIN_CODE$$Length;
extern uint32  Load$$WIFI_PLAY_WIN_DATA$$Base;
extern uint32 Image$$WIFI_PLAY_WIN_DATA$$RW$$Base;
extern uint32 Image$$WIFI_PLAY_WIN_DATA$$RW$$Length;
extern uint32 Image$$WIFI_PLAY_WIN_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_PLAY_WIN_DATA$$ZI$$Length;


//WIFI SYS COMMON
extern uint32  Load$$WIFI_SYS_CODE$$Base;
extern uint32 Image$$WIFI_SYS_CODE$$Base;
extern uint32 Image$$WIFI_SYS_CODE$$Length;
extern uint32  Load$$WIFI_SYS_DATA$$Base;
extern uint32 Image$$WIFI_SYS_DATA$$RW$$Base;
extern uint32 Image$$WIFI_SYS_DATA$$RW$$Length;
extern uint32 Image$$WIFI_SYS_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_SYS_DATA$$ZI$$Length;

//WIFI scan
extern uint32  Load$$WIFI_SCAN_CODE$$Base;
extern uint32 Image$$WIFI_SCAN_CODE$$Base;
extern uint32 Image$$WIFI_SCAN_CODE$$Length;
extern uint32  Load$$WIFI_SCAN_DATA$$Base;
extern uint32 Image$$WIFI_SCAN_DATA$$RW$$Base;
extern uint32 Image$$WIFI_SCAN_DATA$$RW$$Length;
extern uint32 Image$$WIFI_SCAN_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_SCAN_DATA$$ZI$$Length;

//WIFI LWIP
extern uint32  Load$$WIFI_LWIP_CODE$$Base;
extern uint32 Image$$WIFI_LWIP_CODE$$Base;
extern uint32 Image$$WIFI_LWIP_CODE$$Length;
extern uint32  Load$$WIFI_LWIP_DATA$$Base;
extern uint32 Image$$WIFI_LWIP_DATA$$RW$$Base;
extern uint32 Image$$WIFI_LWIP_DATA$$RW$$Length;
extern uint32 Image$$WIFI_LWIP_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_LWIP_DATA$$ZI$$Length;


//WIFI DHCP
extern uint32  Load$$WIFI_DHCP_CODE$$Base;
extern uint32 Image$$WIFI_DHCP_CODE$$Base;
extern uint32 Image$$WIFI_DHCP_CODE$$Length;
extern uint32  Load$$WIFI_DHCP_DATA$$Base;
extern uint32 Image$$WIFI_DHCP_DATA$$RW$$Base;
extern uint32 Image$$WIFI_DHCP_DATA$$RW$$Length;
extern uint32 Image$$WIFI_DHCP_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_DHCP_DATA$$ZI$$Length;


//Find Net File
extern uint32  Load$$FIND_NETFILE_CODE$$Base;
extern uint32 Image$$FIND_NETFILE_CODE$$Base;
extern uint32 Image$$FIND_NETFILE_CODE$$Length;
extern uint32  Load$$FIND_NETFILE_DATA$$Base;
extern uint32 Image$$FIND_NETFILE_DATA$$RW$$Base;
extern uint32 Image$$FIND_NETFILE_DATA$$RW$$Length;
extern uint32 Image$$FIND_NETFILE_DATA$$ZI$$Base;
extern uint32 Image$$FIND_NETFILE_DATA$$ZI$$Length;


//WIFI WPA
extern uint32  Load$$WIFI_WPA_CODE$$Base;
extern uint32 Image$$WIFI_WPA_CODE$$Base;
extern uint32 Image$$WIFI_WPA_CODE$$Length;
extern uint32  Load$$WIFI_WPA_DATA$$Base;
extern uint32 Image$$WIFI_WPA_DATA$$RW$$Base;
extern uint32 Image$$WIFI_WPA_DATA$$RW$$Length;
extern uint32 Image$$WIFI_WPA_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_WPA_DATA$$ZI$$Length;


//WIFI AUDIO CONTROL
extern uint32  Load$$WIFI_AUDIO_CTRL_CODE$$Base;
extern uint32 Image$$WIFI_AUDIO_CTRL_CODE$$Base;
extern uint32 Image$$WIFI_AUDIO_CTRL_CODE$$Length;
extern uint32  Load$$WIFI_AUDIO_CTRL_DATA$$Base;
extern uint32 Image$$WIFI_AUDIO_CTRL_DATA$$RW$$Base;
extern uint32 Image$$WIFI_AUDIO_CTRL_DATA$$RW$$Length;
extern uint32 Image$$WIFI_AUDIO_CTRL_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_AUDIO_CTRL_DATA$$ZI$$Length;

//WIFI XML
extern uint32  Load$$WIFI_XML_CODE$$Base;
extern uint32 Image$$WIFI_XML_CODE$$Base;
extern uint32 Image$$WIFI_XML_CODE$$Length;
extern uint32  Load$$WIFI_XML_DATA$$Base;
extern uint32 Image$$WIFI_XML_DATA$$RW$$Base;
extern uint32 Image$$WIFI_XML_DATA$$RW$$Length;
extern uint32 Image$$WIFI_XML_DATA$$ZI$$Base;
extern uint32 Image$$WIFI_XML_DATA$$ZI$$Length;

#endif

extern void ST7637_WriteRAM_Prepare(void);
extern void ST7637_Init(void);
extern void ST7637_SendData(UINT16 data);
extern void ST7637_SetWindow(UINT16 x0,UINT16 y0,UINT16 x1,INT16 y1);
extern void ST7637_Clear(UINT16 color);
extern void ST7637_DMATranfer (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *pSrc);
extern void ST7637_Standby(void);
extern void ST7637_WakeUp(void);
extern void ST7637_MP4_Init(void);
extern void ST7637_MP4_DeInit(void);

extern void ST7735_WriteRAM_Prepare(void);
extern void ST7735_Init(void);
extern void ST7735_SendData(UINT16 data);
extern void ST7735_SetWindow(UINT16 x0,UINT16 y0,UINT16 x1,INT16 y1);
extern void ST7735_Clear(UINT16 color);
extern void ST7735_DMATranfer (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *pSrc);
extern void ST7735_Standby(void);
extern void ST7735_WakeUp(void);
extern void ST7735_MP4_Init(void);
extern void ST7735_MP4_DeInit(void);

extern void Qn8035_Tuner_SetInitArea(UINT8 area);
extern void Qn8035_Tuner_SetFrequency(UINT32 n10KHz, UINT8 HILO, BOOL ForceMono,UINT16 Area);              
extern void Qn8035_Tuner_SetStereo(BOOL bStereo);
extern void Qn8035_Tuner_Vol_Set(UINT8 gain);
extern void Qn8035_Tuner_PowerOffDeinit(void);
extern UINT16 Qn8035_Tuner_SearchByHand(UINT16 direct, UINT32 *FmFreq);
extern void Qn8035_Tuner_PowerDown(void);
extern void Qn8035_Tuner_MuteControl(bool active);               
extern BOOLEAN Qn8035_GetStereoStatus(void);

extern void FM5807_Tuner_SetInitArea(UINT8 area);
extern void FM5807_Tuner_SetFrequency(UINT32 n10KHz, UINT8 HILO, BOOL ForceMono,UINT16 Area);              
extern void FM5807_Tuner_SetStereo(BOOL bStereo);
extern void FM5807_Tuner_Vol_Set(UINT8 gain);
extern void FM5807_Tuner_PowerOffDeinit(void);
extern UINT16 FM5807_Tuner_SearchByHand(UINT16 direct, UINT32 *FmFreq);
extern void FM5807_Tuner_PowerDown(void);
extern void FM5807_Tuner_MuteControl(bool active);               
extern BOOLEAN FM5807_GetStereoStatus(void);


/*
--------------------------------------------------------------------------------
  Function name : 
  Author        : ZHengYongzhi
  Description   : 模块信息表，生成固件模块头信息，用于代码调度

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
__attribute__((section("ModuleInfo")))
FIRMWARE_INFO_T const ModuleInfo = {

    (uint32)(&Load$$MODULE_INFO$$Base),

    //CODE_INFO_TABLE
    {
        (uint32)(MAX_MODULE_NUM),        
#ifndef _JTAG_DEBUG_
        {
            
            {//00
                (uint32)(&Load$$SYS_CODE$$Base),
                (uint32)(&Image$$SYS_CODE$$Base),      
                (uint32)(&Image$$SYS_CODE$$Length),    
                
                (uint32)(&Load$$SYS_DATA$$Base),       
                (uint32)(&Image$$SYS_DATA$$RW$$Base),  
                (uint32)(&Image$$SYS_DATA$$RW$$Length),
                
                (uint32)(&Image$$SYS_DATA$$ZI$$Base),  
                (uint32)(&Image$$SYS_DATA$$ZI$$Length),
            },
            
            {//01
                (uint32)(&Load$$SYS_INIT_CODE$$Base),
                (uint32)(&Image$$SYS_INIT_CODE$$Base),      
                (uint32)(&Image$$SYS_INIT_CODE$$Length),    
                
                (uint32)(&Load$$SYS_INIT_DATA$$Base),       
                (uint32)(&Image$$SYS_INIT_DATA$$RW$$Base),  
                (uint32)(&Image$$SYS_INIT_DATA$$RW$$Length),
                
                (uint32)(&Image$$SYS_INIT_DATA$$ZI$$Base),  
                (uint32)(&Image$$SYS_INIT_DATA$$ZI$$Length),
            },
            
            {//02
                (uint32)(&Load$$FLASH_WRITE_CODE$$Base),
                (uint32)(&Image$$FLASH_WRITE_CODE$$Base),      
                (uint32)(&Image$$FLASH_WRITE_CODE$$Length),    
                
                (uint32)(&Load$$FLASH_WRITE_DATA$$Base),       
                (uint32)(&Image$$FLASH_WRITE_DATA$$RW$$Base),  
                (uint32)(&Image$$FLASH_WRITE_DATA$$RW$$Length),
                
                (uint32)(&Image$$FLASH_WRITE_DATA$$ZI$$Base),  
                (uint32)(&Image$$FLASH_WRITE_DATA$$ZI$$Length),
            },
            
            {//03
                (uint32)(&Load$$LCD_DRIVER1_CODE$$Base),
                (uint32)(&Image$$LCD_DRIVER1_CODE$$Base),      
                (uint32)(&Image$$LCD_DRIVER1_CODE$$Length),    
                
                (uint32)(&Load$$LCD_DRIVER1_DATA$$Base),       
                (uint32)(&Image$$LCD_DRIVER1_DATA$$RW$$Base),  
                (uint32)(&Image$$LCD_DRIVER1_DATA$$RW$$Length),
                
                (uint32)(&Image$$LCD_DRIVER1_DATA$$ZI$$Base),  
                (uint32)(&Image$$LCD_DRIVER1_DATA$$ZI$$Length),
            },
            
            {//04
                (uint32)(&Load$$LCD_DRIVER2_CODE$$Base),
                (uint32)(&Image$$LCD_DRIVER2_CODE$$Base),      
                (uint32)(&Image$$LCD_DRIVER2_CODE$$Length),    
              
                (uint32)(&Load$$LCD_DRIVER2_DATA$$Base),       
                (uint32)(&Image$$LCD_DRIVER2_DATA$$RW$$Base),  
                (uint32)(&Image$$LCD_DRIVER2_DATA$$RW$$Length),
              
                (uint32)(&Image$$LCD_DRIVER2_DATA$$ZI$$Base),  
                (uint32)(&Image$$LCD_DRIVER2_DATA$$ZI$$Length),
            },
            
            {//05
                (uint32)(&Load$$USB_CODE$$Base),
                (uint32)(&Image$$USB_CODE$$Base),      
                (uint32)(&Image$$USB_CODE$$Length),    
                
                (uint32)(&Load$$USB_DATA$$Base),       
                (uint32)(&Image$$USB_DATA$$RW$$Base),  
                (uint32)(&Image$$USB_DATA$$RW$$Length),
                
                (uint32)(&Image$$USB_DATA$$ZI$$Base),  
                (uint32)(&Image$$USB_DATA$$ZI$$Length),
            },
            
            {//16
                (uint32)(&Load$$MAINMENU_CODE$$Base),
                (uint32)(&Image$$MAINMENU_CODE$$Base),      
                (uint32)(&Image$$MAINMENU_CODE$$Length),    
                
                (uint32)(&Load$$MAINMENU_DATA$$Base),       
                (uint32)(&Image$$MAINMENU_DATA$$RW$$Base),  
                (uint32)(&Image$$MAINMENU_DATA$$RW$$Length),
                
                (uint32)(&Image$$MAINMENU_DATA$$ZI$$Base),  
                (uint32)(&Image$$MAINMENU_DATA$$ZI$$Length),
            },
            
            {//07
                (uint32)(&Load$$MP3_DECODE_CODE$$Base),
                (uint32)(&Image$$MP3_DECODE_CODE$$Base),      
                (uint32)(&Image$$MP3_DECODE_CODE$$Length),    
                
                (uint32)(&Load$$MP3_DECODE_DATA$$Base),       
                (uint32)(&Image$$MP3_DECODE_DATA$$RW$$Base),  
                (uint32)(&Image$$MP3_DECODE_DATA$$RW$$Length),
                
                (uint32)(&Image$$MP3_DECODE_DATA$$ZI$$Base),  
                (uint32)(&Image$$MP3_DECODE_DATA$$ZI$$Length),
            },
            
            {//08
                (uint32)(&Load$$MP3_INIT_CODE$$Base),
                (uint32)(&Image$$MP3_INIT_CODE$$Base),      
                (uint32)(&Image$$MP3_INIT_CODE$$Length),    
                
                (uint32)(&Load$$MP3_INIT_DATA$$Base),       
                (uint32)(&Image$$MP3_INIT_DATA$$RW$$Base),  
                (uint32)(&Image$$MP3_INIT_DATA$$RW$$Length),
                
                (uint32)(&Image$$MP3_INIT_DATA$$ZI$$Base),  
                (uint32)(&Image$$MP3_INIT_DATA$$ZI$$Length),
            },
            
            {//09
                (uint32)(&Load$$MP3_COMMON_CODE$$Base),
                (uint32)(&Image$$MP3_COMMON_CODE$$Base),      
                (uint32)(&Image$$MP3_COMMON_CODE$$Length),    
                
                (uint32)(&Load$$MP3_COMMON_DATA$$Base),       
                (uint32)(&Image$$MP3_COMMON_DATA$$RW$$Base),  
                (uint32)(&Image$$MP3_COMMON_DATA$$RW$$Length),
                
                (uint32)(&Image$$MP3_COMMON_DATA$$ZI$$Base),  
                (uint32)(&Image$$MP3_COMMON_DATA$$ZI$$Length),
            },
            
            {//10
                (uint32)(&Load$$ENCODE_CODE$$Base),
                (uint32)(&Image$$ENCODE_CODE$$Base),      
                (uint32)(&Image$$ENCODE_CODE$$Length),    
                
                (uint32)(&Load$$ENCODE_DATA$$Base),       
                (uint32)(&Image$$ENCODE_DATA$$RW$$Base),  
                (uint32)(&Image$$ENCODE_DATA$$RW$$Length),
                
                (uint32)(&Image$$ENCODE_DATA$$ZI$$Base),  
                (uint32)(&Image$$ENCODE_DATA$$ZI$$Length),
            },
            
            {//11
                (uint32)(&Load$$FINDFILE_CODE$$Base),
                (uint32)(&Image$$FINDFILE_CODE$$Base),      
                (uint32)(&Image$$FINDFILE_CODE$$Length),    
                
                (uint32)(&Load$$FINDFILE_DATA$$Base),       
                (uint32)(&Image$$FINDFILE_DATA$$RW$$Base),  
                (uint32)(&Image$$FINDFILE_DATA$$RW$$Length),
                
                (uint32)(&Image$$FINDFILE_DATA$$ZI$$Base),  
                (uint32)(&Image$$FINDFILE_DATA$$ZI$$Length),
            },
            
            {//12
                (uint32)(&Load$$CHARGE_WIN_CODE$$Base),
                (uint32)(&Image$$CHARGE_WIN_CODE$$Base),      
                (uint32)(&Image$$CHARGE_WIN_CODE$$Length),    
                
                (uint32)(&Load$$CHARGE_WIN_DATA$$Base),       
                (uint32)(&Image$$CHARGE_WIN_DATA$$RW$$Base),  
                (uint32)(&Image$$CHARGE_WIN_DATA$$RW$$Length),
                
                (uint32)(&Image$$CHARGE_WIN_DATA$$ZI$$Base),  
                (uint32)(&Image$$CHARGE_WIN_DATA$$ZI$$Length),
            },
			
            //MODUEL_ID_WIFI_AP_WIN
            {//13
                (uint32)(&Load$$WIFI_AP_WIN_CODE$$Base),
                (uint32)(&Image$$WIFI_AP_WIN_CODE$$Base),      
                (uint32)(&Image$$WIFI_AP_WIN_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_AP_WIN_DATA$$Base),       
                (uint32)(&Image$$WIFI_AP_WIN_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_AP_WIN_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_AP_WIN_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_AP_WIN_DATA$$ZI$$Length),
            }, 
			
			
            //MODUEL_ID_WIFI_PLAY_WIN
            {//14
                (uint32)(&Load$$WIFI_PLAY_WIN_CODE$$Base),
                (uint32)(&Image$$WIFI_PLAY_WIN_CODE$$Base),      
                (uint32)(&Image$$WIFI_PLAY_WIN_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_PLAY_WIN_DATA$$Base),       
                (uint32)(&Image$$WIFI_PLAY_WIN_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_PLAY_WIN_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_PLAY_WIN_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_PLAY_WIN_DATA$$ZI$$Length),
            }, 

            //MODUEL_ID_WIFI_SYS
            {//15
                (uint32)(&Load$$WIFI_SYS_CODE$$Base),
                (uint32)(&Image$$WIFI_SYS_CODE$$Base),      
                (uint32)(&Image$$WIFI_SYS_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_SYS_DATA$$Base),       
                (uint32)(&Image$$WIFI_SYS_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_SYS_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_SYS_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_SYS_DATA$$ZI$$Length),
            }, 

            //MODULE_ID_WIFI_SCAN
            {//16
                (uint32)(&Load$$WIFI_SCAN_CODE$$Base),
                (uint32)(&Image$$WIFI_SCAN_CODE$$Base),      
                (uint32)(&Image$$WIFI_SCAN_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_SCAN_DATA$$Base),       
                (uint32)(&Image$$WIFI_SCAN_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_SCAN_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_SCAN_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_SCAN_DATA$$ZI$$Length),
			},
				
            //MODULE_ID_WIFI_LWIP
            {//17
                (uint32)(&Load$$WIFI_LWIP_CODE$$Base),
                (uint32)(&Image$$WIFI_LWIP_CODE$$Base),      
                (uint32)(&Image$$WIFI_LWIP_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_LWIP_DATA$$Base),       
                (uint32)(&Image$$WIFI_LWIP_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_LWIP_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_LWIP_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_LWIP_DATA$$ZI$$Length),
            },
            
            //MODULE_ID_WIFI_DHCP
            {//18
                (uint32)(&Load$$WIFI_DHCP_CODE$$Base),
                (uint32)(&Image$$WIFI_DHCP_CODE$$Base),      
                (uint32)(&Image$$WIFI_DHCP_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_DHCP_DATA$$Base),       
                (uint32)(&Image$$WIFI_DHCP_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_DHCP_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_DHCP_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_DHCP_DATA$$ZI$$Length),
            },

            //MODULE_ID_NETFILE_FIND
            {//19
                (uint32)(&Load$$FIND_NETFILE_CODE$$Base),
                (uint32)(&Image$$FIND_NETFILE_CODE$$Base),
                (uint32)(&Image$$FIND_NETFILE_CODE$$Length),
                (uint32)(&Load$$FIND_NETFILE_DATA$$Base),
                (uint32)(&Image$$FIND_NETFILE_DATA$$RW$$Base),
                (uint32)(&Image$$FIND_NETFILE_DATA$$RW$$Length),
                (uint32)(&Image$$FIND_NETFILE_DATA$$ZI$$Base),
                (uint32)(&Image$$FIND_NETFILE_DATA$$ZI$$Length)

            },

            //MODULE_ID_WIFI_WPA
            {//20
                (uint32)(&Load$$WIFI_WPA_CODE$$Base),
                (uint32)(&Image$$WIFI_WPA_CODE$$Base),      
                (uint32)(&Image$$WIFI_WPA_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_WPA_DATA$$Base),       
                (uint32)(&Image$$WIFI_WPA_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_WPA_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_WPA_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_WPA_DATA$$ZI$$Length),
            },

            //MODULE_ID_WIFI_AUDIOC
            {//21
                (uint32)(&Load$$WIFI_AUDIO_CTRL_CODE$$Base),
                (uint32)(&Image$$WIFI_AUDIO_CTRL_CODE$$Base),      
                (uint32)(&Image$$WIFI_AUDIO_CTRL_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_AUDIO_CTRL_DATA$$Base),       
                (uint32)(&Image$$WIFI_AUDIO_CTRL_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_AUDIO_CTRL_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_AUDIO_CTRL_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_AUDIO_CTRL_DATA$$ZI$$Length),
            },

			//MODULE_ID_XML
            {//22
                (uint32)(&Load$$WIFI_XML_CODE$$Base),
                (uint32)(&Image$$WIFI_XML_CODE$$Base),      
                (uint32)(&Image$$WIFI_XML_CODE$$Length),    
                
                (uint32)(&Load$$WIFI_XML_DATA$$Base),       
                (uint32)(&Image$$WIFI_XML_DATA$$RW$$Base),  
                (uint32)(&Image$$WIFI_XML_DATA$$RW$$Length),
                
                (uint32)(&Image$$WIFI_XML_DATA$$ZI$$Base),  
                (uint32)(&Image$$WIFI_XML_DATA$$ZI$$Length),
            }, 
        },
#endif    
    },

    //系统默认参数  //.SysDefaultPara
    {
        //系统参数
        #ifdef _SDCARD_
        (uint32)1,      //UINT32 SDEnable;
        #else
        (uint32)0,
        #endif
        
        (uint32)0,
        
        (uint32)KEY_NUM,//UINT32 KeyNum
        
                
        (uint32)2,      //UINT32 Langule;
        (uint32)24,     //UINT32 Volume;
                
        (uint32)0,      //UINT32 BLMode;
        (uint32)2,      //UINT32 BLevel;
        (uint32)5,      //UINT32 BLtime;
                
        (uint32)0,      //UINT32 SetPowerOffTime;
        (uint32)0,      //UINT32 IdlePowerOffTime;
            
        //Music参数
        (uint32)5,      //UINT32 MusicRepMode;
        (uint32)0,      //UINT32 MusicPlayOrder;
        (uint32)0,      //UINT32 MusicEqSel;
        (uint32)0,      //UINT32 MusicPlayFxSel;
            
        //Video参数
            
        //Radio参数
        (uint32)0,      //UINT32 FMArea;
        (uint32)0,      //UINT32 FMStereo;
            
        //Record参数
            
        //Picture参数
        (uint32)2,      //UINT32 PicAutoPlayTime;
            
        //Text参数
        (uint32)2,      //UINT32 TextAutoPlayTime;

        //Image ID
        (uint32)0,

        //多国语言选择
        (uint32)TOTAL_MENU_ITEM,    //最大菜单项
        (uint32)TOTAL_LANAUAGE_NUM, //uint32 LanguageTotle, 实际最大多国语言个数
        //语言配置表                //最多可支持64国语言
        {
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //8
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //16
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //24
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //32
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //40
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //48
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //56
            (uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1,(uint32)1, //64
        },
    },

    //FM 参数及驱动程序列表 .FMDriverTable
    {
        (uint32)1,      //FMDriverID;
        (uint32)FM_DRIVER_MAX,

        //FM Driver List
        {
            //FM5767
            {
                0,
                0,
                0,
                0,
    
                0,
                0,
                0,
                0,
                0,
            },

            //FM5807
            {
                0,
                0,
                0,
                0,
    
                0,
                0,
                0,
                0,
                0,

            },
        },
    },

    //LCD 参数及驱动程序列表 .LCDDriverTable
    {
        (uint32)1,      //LcdDriverID;
        (uint32)LCD_DRIVER_MAX,

        //LCD Driver List
        {
            //DRIVER1
            {
                ST7637_WriteRAM_Prepare,
                ST7637_Init,
                ST7637_SendData,
                ST7637_SetWindow,
                ST7637_Clear,
                ST7637_DMATranfer,
                ST7637_Standby,
                ST7637_WakeUp,
                ST7637_MP4_Init,
                ST7637_MP4_DeInit,
                0,
                0,
                0,
                0,
				0,
            },

            {
                ST7735_WriteRAM_Prepare,
                ST7735_Init,
                ST7735_SendData,
                ST7735_SetWindow,
                ST7735_Clear,
                ST7735_DMATranfer,
                ST7735_Standby,
                ST7735_WakeUp,
                ST7735_MP4_Init,
                ST7735_MP4_DeInit,
                0,
                0,
                0,
                0,
                0,
            },
        },
    }
};

/*
--------------------------------------------------------------------------------
  Function name : 
  Author        : ZHengYongzhi
  Description   : 模块信息表

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
__attribute__((section("START_CODE")))
void ScatterLoader(void)
{
    uint32 i,len;
    uint8  *pDest;

    //清除Bss段
    pDest = (uint8*)((uint32)(&Image$$SYS_DATA$$ZI$$Base));
    len = (uint32)((uint32)(&Image$$SYS_DATA$$ZI$$Length));
    for (i = 0; i < len; i++)
    {
        *pDest++ = 0;
    }
}

/*
********************************************************************************
*
*                         End of StartLoadTab.c
*
********************************************************************************
*/
