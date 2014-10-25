/*
********************************************************************************
*                   Copyright (c) 2008,Yangwenjie
*                         All rights reserved.
*
* File Name��   ST7735.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             yangwenjie      2009-1-15          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_Lcd_

#include "DriverInclude.h"

/*
--------------------------------------------------------------------------------
  Function name :  void ST7735_Command(unsigned int cmd)
  Author        : yangwenjie
  Description   :send command to lcd
                  
  Input         : cmd LCD word
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/ 
_ATTR_LCDDRIVER_ST7735_CODE_
void ST7735_Command(UINT16 cmd)
 {
     Lcd->cmd = cmd;
 }
/*
--------------------------------------------------------------------------------
  Function name :  void ST7735_Data(UINT16 data)
  Author        : yangwenjie
  Description   : send display data to lcd.
                  
  Input         : data LCD data
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_ 
 void ST7735_Data(UINT16 data)
 {
    Lcd->data = data;
 }
 
/*
--------------------------------------------------------------------------------
  Function name :  void ST7735_WriteReg(UINT16 addr, UINT16 data)
  Author        : yangwenjie
  Description   : send command and data to LCD.
                  
  Input         : data -- LCD data
                  addr LCD command register address.
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_ 
 void ST7735_WriteReg(UINT16 addr, UINT16 data)
 {
     ST7735_Command(addr);
     ST7735_Data(data);
 }
 
/*
--------------------------------------------------------------------------------
  Function name : void EMCTL_Config(void)
  Author        : yangwenjie
  Description   : config the LCD refresh time. 
                  
  Input         : null
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_ 
void ST7735_WaitSet(UINT32 data )
 {
    //emctl->LCDWait = data;
 }
 /*
--------------------------------------------------------------------------------
  Function name : void ST7735_WriteRAM_Prepare(void)
  Author        : yangwenjie
  Description   : to enable LCD data transfer.
                  
  Input         : null
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_ 
  void ST7735_WriteRAM_Prepare(void)
 {  
    Lcd_SplitMode(LCD_SPLIT1);
    ST7735_Command(0x2c);
    Lcd_SplitMode(LCD_SPLIT2);
 }
  /*
--------------------------------------------------------------------------------
  Function name :  void ST7735_Init(void)
  Author        : yangwenjie
  Description   : LCD initialization.
                  
  Input         : null
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_ 
 void ST7735_Init(void)
 { 
//    IoMuxSetLCD(LCD_MODE);
    Lcd_SplitMode(LCD_SPLIT1);

	ST7735_Command(0x11);       //sleep out
	DelayMs(120);               //-When IC is in Sleep Out or Display On mode, it is necessary to wait 120msec before 
	                           //sending next command due to the download of default value of registers
                               // and the execution of self-diagnostic function. 
	//Power Control//

    ST7735_Command(0xb6); 
    ST7735_Data(0xb4);
	ST7735_Data(0xf0);                           
	
	ST7735_Command(0xb1);     //Set the frame frequency of the full colors normal mode.625/((RTNA x 2 + 40) x (LINE + FPA + BPA))
//	ST7735_Data(0x06);        //RTNA
    ST7735_Data(0x02);        //RTNA
	ST7735_Data(0x35);        //LINE
	ST7735_Data(0x36);        //BPA

    ST7735_Command(0xb2);
    ST7735_Data(0x02);        //RTNA
	ST7735_Data(0x35);        //LINE
	ST7735_Data(0x36);        //BPA
	
	ST7735_Command(0xb3);
    ST7735_Data(0x02);        //RTNA
	ST7735_Data(0x35);        //LINE
	ST7735_Data(0x36);        //BPA
	ST7735_Data(0x02);        //RTNA
	ST7735_Data(0x35);        //LINE
	ST7735_Data(0x36);        //BPA

    ST7735_Command(0xb4);
    ST7735_Data(0x07);
    
	ST7735_Command(0xc0);     // Power Control 1 
//	ST7735_Data(0x02);        //AVDD 4.7
//	ST7735_Data(0x70);        //GVDD 3.9
    ST7735_Data(0xA2);        //AVDD 4.9
    ST7735_Data(0x02);        //GVDD 4.6
    ST7735_Data(0x84);        //AUTO
    
	ST7735_Command(0xc1);     // Power Control 2 
//	ST7735_Data(0x07);
    ST7735_Data(0xC5);
    
	ST7735_Command(0xc2);     // Power Control 3
//	ST7735_Data(0x01);
//	ST7735_Data(0x01);
    ST7735_Data(0x0A);
	ST7735_Data(0x00);
    
	ST7735_Command(0xc3);     // Power Control 4
//	ST7735_Data(0x02);
//	ST7735_Data(0x07);
    ST7735_Data(0x8A);
	ST7735_Data(0x2A);
    
	ST7735_Command(0xc4);     // Power Control 5
//	ST7735_Data(0x02);
//	ST7735_Data(0x04);
	ST7735_Data(0x8a);
	ST7735_Data(0xee);
    
	ST7735_Command(0xc5);
//	ST7735_Data(0x39);
//	ST7735_Data(0x42);
	ST7735_Data(0x06);

    ST7735_Command(0x36);
    #if(LCD_WIDTH == 128)
    ST7735_Data(0xc8); 
    #else
    ST7735_Data(0x68);  // for 160 * 128
    #endif
  
    
	//Setup Gamma  Correction Characteristics Setting//
	ST7735_Command(0xe0);
	ST7735_Data(0x12);
	ST7735_Data(0x1c);
	ST7735_Data(0x10);
	ST7735_Data(0x18);
	ST7735_Data(0x33);
	ST7735_Data(0x2c);
	ST7735_Data(0x25);
	ST7735_Data(0x28);
	ST7735_Data(0x28);
	ST7735_Data(0x27);
	ST7735_Data(0x2f);
	ST7735_Data(0x3c);
	ST7735_Data(0x00);
	ST7735_Data(0x03);
	ST7735_Data(0x10);
	ST7735_Data(0x0e);
    
	ST7735_Command(0xe1);
	ST7735_Data(0x12);
	ST7735_Data(0x1c);
	ST7735_Data(0x10);
	ST7735_Data(0x18);
	ST7735_Data(0x2d);
	ST7735_Data(0x28);
	ST7735_Data(0x23);
	ST7735_Data(0x28);
	ST7735_Data(0x28);
	ST7735_Data(0x26);
	ST7735_Data(0x2f);
	ST7735_Data(0x3b);
	ST7735_Data(0x00);
	ST7735_Data(0x03);
	ST7735_Data(0x03);
	ST7735_Data(0x10);

    ST7735_Command(0x3a);
    ST7735_Data(0x05);

    ST7735_Command(0x2a);
    ST7735_Data(0x00);
    ST7735_Data(0x02);
    ST7735_Data(0x00);
    ST7735_Data(0x81);

    ST7735_Command(0x2b);
    ST7735_Data(0x00);
    ST7735_Data(0x03);
    ST7735_Data(0x00);
    ST7735_Data(0x82);
    
    ST7735_Command(0x29);
	DelayMs(120);

    ST7735_Command(0x2c);

    Lcd_SplitMode(LCD_SPLIT2);
 }

/*
--------------------------------------------------------------------------------
  Function name :  void ST7735_SendData(UINT16 data)
  Author        : yangwenjie
  Description   : send LCD data.
                  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_ 
void ST7735_SendData(UINT16 data)
{
    ST7735_Data(data);    
}
 /*
--------------------------------------------------------------------------------
  Function name : void ST7735_SetCursor(UINT16 x0,UINT16 y0,UINT16 x1,INT16 y1)
  Author        : yangwenjie
  Description   : set display area coordinate
                  
  Input         : x0,y0: the start coordinate of display pictrue.
                  x1,y1: the end coordinate of display pictrue.
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_  
 void ST7735_SetWindow(UINT16 x0,UINT16 y0,UINT16 x1,INT16 y1)
 { 
    Lcd_SplitMode(LCD_SPLIT1);
	ST7735_Command(0x2a);

    #if(LCD_WIDTH == 160)    
	ST7735_Data(0x00);
	ST7735_Data(x0+1);
	
	ST7735_Data(0x00);
	ST7735_Data(x1+1);
    #else
    ST7735_Data(0x00);
	ST7735_Data(x0+2);
	
	ST7735_Data(0x00);
	ST7735_Data(x1+2);
    #endif
	
	ST7735_Command(0x2b);

    #if(LCD_WIDTH == 160)
	ST7735_Data(0x00);
	ST7735_Data(y0+2);
	
	ST7735_Data(0x00);
	ST7735_Data(y1+2);
    #else
    ST7735_Data(0x00);
	ST7735_Data(y0+1);
	
	ST7735_Data(0x00);
	ST7735_Data(y1+1);
    #endif
	
	ST7735_Command(0x2c);
    
    Lcd_SplitMode(LCD_SPLIT2);
	 
 }
 /*
--------------------------------------------------------------------------------
  Function name : void ST7735_Clear(UINT16 color)
  Author        : yangwenjie
  Description   : clear the screen
                  
  Input         : color��clear lcd to the color.
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_  
 void ST7735_Clear(UINT16 color)
 {
     int i,j,n; 

 	ST7735_SetWindow(0,0,128-1,160-1);
   
	 for(i=0;i<160;i++)
     {
	     n=0; 
         for(j = 0; j <128;j++)
         {
             ST7735_SendData (color);
		
         } 
     } 
 }
 
/*
--------------------------------------------------------------------------------
  Function name : void ST7735_Image(UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT32 *src)
  Author        : yangwenjie
  Description   : display the pictrue in specified
                  
  Input         : x0,y0: the start coordinate of display pictrue.
                  x1,y1: the end coordinate of display pictrue.
                  pSrc�� the source address.
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_    
 void ST7735_Image(UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *pSrc)
 {
    int i,j; 
    int Xpos,Ypos;   
    //if(x0<0) x0 = 0;
    //if(y0<0) y0 = 0;
    if(x1>LCD_WIDTH-1) x1 = LCD_WIDTH-1;
    if(y1>LCD_HEIGHT-1) y1 = LCD_HEIGHT-1;
        
    Xpos = x0;
    Ypos = y0;
    
    ST7735_SetWindow(x0,y0, x1,y1); 
    
    Lcd_SplitMode(LCD_SPLIT1);
    for(j = y0; j < y1+1; j++) 
    { 
        for(i = x0; i < x1+1; i++)
        {
            ST7735_SendData (*pSrc++);      
        } 
         Lcd_SplitMode(LCD_SPLIT2);
        Ypos++;
        ST7735_SetWindow(Xpos, Ypos,x1,y1);   
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void ST7735_DMATranfer (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *src)
  Author        : yangwenjie
  Description   : DMA data transfer
                  
  Input         : x0,y0: the start coordinate of display pictrue.
                  x1,y1: the end coordinate of display pictrue.
                  pSrc�� the source address.
  
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1！15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_    
 int32 ST7735_DMATranfer (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *pSrc, pFunc CallBack)
 {
    UINT32 size;
    eDMA_CHN channel; 
    DMA_CFGX DmaCfg = {CTLL_M2LCD_HALF,CFGL_M2LCD_SINGLE,CFGH_M2LCD_SINGLE,0};
    
    ST7735_SetWindow(x0,y0, x1,y1); 
    
    channel = DmaGetChannel();
    if (channel != DMA_FALSE)
    {
        size = (((x1 + 1) - x0) * ((y1+1) - y0));
        DmaStart((uint32)(channel), (UINT32)(pSrc),(uint32)(&(Lcd->data)),size,&DmaCfg, CallBack);
        return channel;
    }
    return (-1);
 }
_ATTR_LCDDRIVER_ST7637_CODE_    
 int32 ST7735_DMATranfer32 (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT32 *pSrc, pFunc CallBack)
 {
    UINT32 size;
    eDMA_CHN channel; 
    DMA_CFGX DmaCfg = {CTLL_M2LCD_WORD,CFGL_M2LCD_SINGLE,CFGH_M2LCD_SINGLE,0};
    
    ST7735_SetWindow(x0,y0, x1,y1); 
    
    channel = DmaGetChannel();
    if (channel != DMA_FALSE)
    {
        size = (((x1 + 1) - x0) * ((y1+1) - y0)) >> 1;
        Lcd_SplitMode(LCD_SPLIT5);
        DmaStart((uint32)(channel), (UINT32)(pSrc),(uint32)(&(Lcd->data)),size,&DmaCfg, CallBack);
        return channel;
    }
    return (-1);
 }

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7735_CODE_  
void ST7735_Standby(void)
{
    Lcd_SplitMode(LCD_SPLIT1);

    ST7735_Command(0x28);
    ST7735_Command(0x10);
    DelayMs(120);
    
    Lcd_SplitMode(LCD_SPLIT2);
}

/*
*/
_ATTR_LCDDRIVER_ST7735_CODE_  
void ST7735_WakeUp(void)
{
    ST7735_Init();
}

_ATTR_LCDDRIVER_ST7735_CODE_
void ST7735_MP4_Init(void)
{
    short i,j ;
   
   // LCD_Init(MP4_LCD_INIT) ;
#if 0
    IoMuxSetLCD(LCD_MODE);

    ST7735_WaitSet(0x000011d1);
    ST7735_Command(0x11); //Exit Sleep
    DelayMs(20);
    
    ST7735_Command(0x26); //Set Default Gamma
    ST7735_Data(0x04);
    
    //WriteData(WRCOMM,0xF2); //E0h & E1h Enable/Disable
    //WriteData(WRDATA,0x00);
    
    ST7735_Command(0xB1);
    ST7735_Data(0x0C);
    ST7735_Data(0x14);
    
    ST7735_Command(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    ST7735_Data(0x0C);
    ST7735_Data(0x05);
    
    ST7735_Command(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    ST7735_Data(0x02);
    
    ST7735_Command(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    ST7735_Data(0x3F);//0x29
    ST7735_Data(0x38);//0x43
    
    ST7735_Command(0xC7);
    ST7735_Data(0x40);
    
    ST7735_Command(0x3a); //Set Color Format
    ST7735_Data(0x05);
    
    ST7735_Command(0x2A); //Set Column Address
    ST7735_Data(0x00);
    ST7735_Data(0x00);
    ST7735_Data(0x00);
    ST7735_Data(0x7F);
    
    ST7735_Command(0x2B); //Set Page Address
    ST7735_Data(0x00);
    ST7735_Data(0x00);
    ST7735_Data(0x00);
    ST7735_Data(0x9F);
#endif
    Lcd_SplitMode(LCD_SPLIT1);
    ST7735_Command(0x36); //Set Scanning Direction
    ST7735_Data(0x68);   
    //ST7735_Command(0x29);

    //ST7735_Clear(0x0000);
    Lcd_SplitMode(LCD_SPLIT2);

}


_ATTR_LCDDRIVER_ST7735_CODE_
void ST7735_MP4_DeInit(void)
{
    Lcd_SplitMode(LCD_SPLIT1);
    ST7735_Command(0x36); //Set Scanning Direction
    ST7735_Data(0xc8);
    Lcd_SplitMode(LCD_SPLIT2);
}


/*
********************************************************************************
*
*                         End of Lcd.c
*
********************************************************************************
*/ 
