/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   FsConfig.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                               2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#ifndef _USBCONFIG_H
#define _USBCONFIG_H

/*
*-------------------------------------------------------------------------------
*  
*                           FileSystem Configer
*  
*-------------------------------------------------------------------------------
*/

/*
*-------------------------------------------------------------------------------
*  
*                           Section define
*  
*-------------------------------------------------------------------------------
*/

    
#include <stdio.h>
#include <string.h>
#include "SysConfig.h"
#include "typedef.h"
#include "Macro.h"

#include "hw_nvic.h"
#include "hw_scu.h"

#include "hw_memap.h"
#include "hook.h"


#define     USB_DRM_EN
#define     DMA_BULKIN_EN
#define     DMA_BULKOUT_EN

//USB代码、数据段
#define     IRAM_USB                    __attribute__((section("USBCode")))
#define     DRAM_USB_TABLE              __attribute__((section("USBData")))
#define     DRAM_USB                    __attribute__((section("USBBss"), zero_init))

//使能USB&ENCODE
#ifdef  _USB_
#define     USB   
//#define     USB_IF_TEST               //要过USB论证测试需要打开此定义, 打开此定义只要USB线有连接,就不退出USB
#endif  
//配置USB

/*
*-------------------------------------------------------------------------------
*  
*                           Macro define
*  
*-------------------------------------------------------------------------------
*/

#include "FSconfig.h"
#include "FSInclude.h"

#include "USBreg.h"
#include "USB20.h"
#include "bulkonly.h"

#include "MDConfig.h"

/*
********************************************************************************
*
*                         End of FsConfig.h
*
********************************************************************************
*/
#endif
