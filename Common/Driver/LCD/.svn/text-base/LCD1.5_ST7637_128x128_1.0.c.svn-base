/*
********************************************************************************
*                   Copyright (c) 2008,Yangwenjie
*                         All rights reserved.
*
* File Name£º   Lcd.c
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
  Function name :  void Lcd_Command(unsigned int cmd)
  Author        : yangwenjie
  Description   :
                  
  Input         : 
  
  Return        : NULL

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/ 
_ATTR_LCDDRIVER_ST7637_CODE_
void ST7637_Command(UINT16 cmd)
{    
    Lcd->cmd = cmd;
}

/*
--------------------------------------------------------------------------------
  Function name :  void Lcd_Data(UINT16 data)
  Author        : yangwenjie
  Description   :
                  
  Input         : LCD data 
  
  Return        : NULL

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
            
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_ 
void ST7637_Data(UINT16 data)
{
    Lcd->data = data;
}
 
 /*
--------------------------------------------------------------------------------
  Function name : void Lcd_WriteRAM_Prepare(void)
  Author        : yangwenjie
  Description   : LCD data transfer enable
                  
  Input         : 
  
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
            
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_ 
void ST7637_WriteRAM_Prepare(void)
{ 
    Lcd_SplitMode(LCD_SPLIT1);
    ST7637_Command(0x2c);
    Lcd_SplitMode(LCD_SPLIT2);
}
 
/*
--------------------------------------------------------------------------------
  Function name :  void Lcd_Init(void)
  Author        : yangwenjie
  Description   : LCD initial
                  
  Input         : NULL
  
  Return        : NULL

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
            
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_ 
 void ST7637_Init(void)
 {
    //IoMuxSetLCD(LCD_MODE);

    Lcd_SplitMode(LCD_SPLIT1);
    //Soft ware Reset       
    ST7637_Command(0x01);
    DelayUs(100);
    
    //Disable Autoread
    ST7637_Command(0xD7);       //Auto read control 
    ST7637_Data(0xbF);          //Disable auto read 
    ST7637_Command(0xE0);       //Control OTP / MTP 
    ST7637_Data(0x00);          //For read mode
    DelayUs(50);                
    ST7637_Command(0xE3);       //Control OTP/ MTP 
    DelayUs(50);                
    ST7637_Command(0xE1);       //Close read mode 
       
    //Sleep Out          
    ST7637_Command(0x28);       //display off
    ST7637_Command(0x11);       //Sleep out             
    DelayUs(50);
    
    //VOP Set  
    ST7637_Command(0xC0);       //V0 Voltage Set 
    ST7637_Data(0x02);          //Total VOP= 13.72  
    ST7637_Data(0x01);          //contrast set 
    
    //OTPB Set
    ST7637_Command(0xC3); //Bias set 
    ST7637_Data(0x00); // =1/12
    ST7637_Command(0xC4); //Booster set 
    ST7637_Data(0x05); //06-->7X (condition :Vdd=2.8V)   ///////////////////////////////////07-->8X
    ST7637_Command(0xC5);    //Booster efficiency set 
    ST7637_Data(0x01); //                                     
    ST7637_Command(0xCB); //VG booster set 
    ST7637_Data(0x01); // 
    ST7637_Command(0xCC); //ID1 Set 
    ST7637_Data(0x00);        //ID1 = 0x00 
    ST7637_Command(0xCE); //ID3 Set 
    ST7637_Data(0x00);        // ID3 = 0x00 
    ST7637_Command(0xB7); //Glass¡¯s Com/Seg direction set 
    ST7637_Data(0xC8); // 
    ST7637_Command(0xD0); //Follower Type set 
    ST7637_Data(0x1D); // 
    
    //-------------------MTP Set----------------------------//
    ST7637_Command(0xD7); //Auto read control
    ST7637_Data(0xbF); //                         ////////////////////////////////////////////BF  df
    ST7637_Command(0xB5); //N-line Set 
    ST7637_Data(0x07); //Î¢Õ{n-line Öµ£¬¸ÄÉÆcrosstalk 
    ST7637_Command(0xCD); //ID2 Set 
    ST7637_Data(0x80); //ID2 = 0x80
    ST7637_Command(0xD0); //Set Vg Source 
    ST7637_Data(0x1D); // 
    ST7637_Command(0xB4); //PTL Saving Mode Set 
    ST7637_Data(0x18);     //Power normal mode 
    
    //------------------Command Table 1----------------------// 
    ST7637_Command(0x20);    //Normal Display 
                      //0x21 = Inv.display 
    ST7637_Command(0xF7); //Temp detection threshold  
    ST7637_Data(0x06); // 
    ST7637_Command(0x2A);   //Column address set
    ST7637_Data(0x04);  //Start address = 4
    ST7637_Data(0x83);  //End dddress = 131 
    ST7637_Command(0x2B);   //line adress set
    ST7637_Data(0x04);  //Start Address = 4
    ST7637_Data(0x83); //End address = 131 
    ST7637_Command(0x3A); //Pixel Format Set 
    ST7637_Data(0x05); // 65K color 
	   
    ST7637_Command(0x36); //Memory access control 
    ST7637_Data(0x80); //Scan direction set
  
    ST7637_Command(0xB0); //Duty Set 
    ST7637_Data(0x7F); //1/128
    ST7637_Command(0x29); //Display on 
    
    ST7637_Command(0x25); 
    ST7637_Data(0x54);
    
    ST7637_Command(0xF0); 
    ST7637_Data(0x0C);
    ST7637_Command(0x0C); 
    ST7637_Data(0x0C);
     
    
    //-----------------Gamma Table Set---------------------//
    ST7637_Command(0xF9); //Gamma 
    ST7637_Data(0x00); 
    ST7637_Data(0x03); 
    ST7637_Data(0x05); 
    ST7637_Data(0x07); 
    ST7637_Data(0x09); 
    ST7637_Data(0x0B); 
    ST7637_Data(0x0D); 
    ST7637_Data(0x0F); 
    ST7637_Data(0x11); 
    ST7637_Data(0x13); 
    ST7637_Data(0x15); 
    ST7637_Data(0x17); 
    ST7637_Data(0x19); 
    ST7637_Data(0x1B); 
    ST7637_Data(0x1D); 
    ST7637_Data(0x1F); // 
    //-----------------init code finis
	ST7637_Command(0x29); //Display on 

    Lcd_SplitMode(LCD_SPLIT2);
 }

/*
--------------------------------------------------------------------------------
  Function name :  void Lcd_SendData(UINT16 data)
  Author        : yangwenjie
  Description   : 
                  
  Input         :
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
            
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_ 
void ST7637_SendData(UINT16 data)
{
    Lcd->data = data;
}

void ST7637_SendData32(UINT32 data)
{
    Lcd->data = data;
}
 /*
--------------------------------------------------------------------------------
  Function name : void Lcd_SetCursor(UINT16 x0,UINT16 y0,UINT16 x1,INT16 y1)
  Author        : yangwenjie
  Description   : set display aera.
                  
  Input         : x0,y0: start coordinate
                  x1,y1: end coordinate
  Return        : NULL

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_  
 void ST7637_SetWindow(UINT16 x0,UINT16 y0,UINT16 x1,INT16 y1)
 {
    Lcd_SplitMode(LCD_SPLIT1);
     
    ST7637_Command(0x2A);   //Column address set
    ST7637_Data(0x04+x0);   //Start address = 4
    ST7637_Data(0x04+x1);   //End dddress = 131

    ST7637_Command(0x2B);   //line adress set
    ST7637_Data(0x04+y0);   //Start Address = 4
    ST7637_Data(0x04+y1);   //End address = 131
  
	ST7637_Command(0x2c);   //write data  

    Lcd_SplitMode(LCD_SPLIT2);
 }
 /*
--------------------------------------------------------------------------------
  Function name : void Lcd_Clear(UINT16 color)
  Author        : yangwenjie
  Description   : clear screen
                  
  Input         : clear screen to color
  
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_  
void ST7637_Clear(UINT16 color)
{
    int i; 
    ST7637_SetWindow(0,0,127,127); 

    for(i = 0; i < 128*128; i++)
    {
        ST7637_SendData(color);
    } 
}
 
/*
--------------------------------------------------------------------------------
  Function name : void Lcd_Image(UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT32 *src)
  Author        : yangwenjie
  Description   : draw pictrue in specified position.
                  
  Input         : x0,y0: the start coordinate of display pictrue.
                  x1,y1: the end coordinate of display pictrue.
                  pSrc£º the source address.
  
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_    
 void ST7637_Image(UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *pSrc)
 {
    int i; 
    uint32 size;
    uint32 *pSrcWord;
    
    if(x1>LCD_WIDTH-1) x1 = LCD_WIDTH-1;
    if(y1>LCD_HEIGHT-1) y1 = LCD_HEIGHT-1;
    
    ST7637_SetWindow(x0,y0, x1,y1); 

    size = ((x1 + 1) - x0) * ((y1+1) - y0);
    if ((uint32)pSrc & 0x03 != 0)
    {
        ST7637_SendData (*pSrc++);  
        size --;
    }
    
    pSrcWord = (uint32*)pSrc;
    Lcd_SplitMode(LCD_SPLIT5);
    for(i= y0; i < size >> 1; i++) 
    { 
        ST7637_SendData32(*pSrcWord++);     
    }
    Lcd_SplitMode(LCD_SPLIT2);
    
    if (size & 0x01 != 0)
    {
        ST7637_SendData (*pSrc++);  
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Lcd_DMATranfer (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *src)
   Author        : yangwenjie
   Description   : DMA transfer data.
                  
  Input         : x0,y0: the start coordinate of display pictrue.
                  x1,y1: the end coordinate of display pictrue.
                  pSrc£º the source address.
  
  Return        : NULL

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
  Note:          
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_    
 int32 ST7637_DMATranfer (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT16 *pSrc, pFunc CallBack)
 {
    UINT32 size;
    eDMA_CHN channel; 
    DMA_CFGX DmaCfg = {CTLL_M2LCD_HALF,CFGL_M2LCD_SINGLE,CFGH_M2LCD_SINGLE,0};
    
    ST7637_SetWindow(x0,y0, x1,y1); 
    
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
 int32 ST7637_DMATranfer32 (UINT8 x0,UINT8 y0,UINT8 x1,UINT8 y1,UINT32 *pSrc, pFunc CallBack)
 {
    UINT32 size;
    eDMA_CHN channel; 
    DMA_CFGX DmaCfg = {CTLL_M2LCD_WORD,CFGL_M2LCD_SINGLE,CFGH_M2LCD_SINGLE,0};
    
    ST7637_SetWindow(x0,y0, x1,y1); 
    
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
  Function name :  void Lcd_Data(UINT16 data)
  Author        : yangwenjie
  Description   : lcd enter standby mode
                  
  Input         : 
  
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-1¡ª15         Ver1.0
  desc:         ORG
            
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_  
void ST7637_Standby(void)
{
    Lcd_SplitMode(LCD_SPLIT1);
    ST7637_Command(0x10);
    Lcd_SplitMode(LCD_SPLIT2);
}

/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/
_ATTR_LCDDRIVER_ST7637_CODE_  
void ST7637_WakeUp(void)
{
    Lcd_SplitMode(LCD_SPLIT1);
    ST7637_Command(0x11);
    Lcd_SplitMode(LCD_SPLIT2);
}


_ATTR_LCDDRIVER_ST7637_CODE_
void ST7637_MP4_Init(void)
{
}

_ATTR_LCDDRIVER_ST7637_CODE_
void ST7637_MP4_DeInit(void)
{
}

/*
********************************************************************************
*
*                         End of Lcd.c
*
********************************************************************************
*/ 
