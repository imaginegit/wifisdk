/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   hw_lcd.h
* 
* Description:  
*                      
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _HW_LCD_H_
#define _HW_LCD_H_

/*
*-------------------------------------------------------------------------------
*
*                  external memory control memap register define
*
*-------------------------------------------------------------------------------
*/
typedef enum 
{
    LCD_SPLIT1 = 0,
    LCD_SPLIT2,
    LCD_SPLIT3,
    LCD_SPLIT4,
	reserve5,
	reserve6,
	reserve7,
	LCD_SPLIT5 
} LCD_MODE_SEL;

typedef volatile struct {
    UINT32 wait;
    UINT32 mode;
    UINT32 cmd;
    UINT32 data;
    
} LCD_t;

#define Lcd                     ((LCD_t *) LCD_BASE)
#define Lcd_SplitMode(var)     ((Lcd->mode) = ((var) << 1))

/*
********************************************************************************
*
*                         End of hw_lcd.h
*
********************************************************************************
*/
#endif
