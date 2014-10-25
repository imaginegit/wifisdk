/* Copyright (C) 2012 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: sdio_card.h
Desc: 

Author: 
Date: 12-09-03
Notes:

$Log: $
 *
 *
*/

#ifndef _MMC_SDIO_CARD_H
#define _MMC_SDIO_CARD_H


#include     "macro.h"
#include     <string.h>		//标准头文件
#include     "typedef.h"		//数据类型头文件
#include     "hw_nvic.h"
#include     "SysConfig.h"
#include     "global.h"
#include     "main.h"
/*
* driver header file start.
*/
#include     "SDConfig.h"

#include     "wificommon.h"
#include     "sysmalloc.h"//dgl for malloc and free.
#include     "errno-base.h"
#include     "ieee80211.h"
#include     "if_ether.h"
#include     "wireless.h"
#include     "ioctl.h"
#include     "if.h"
#include     "sockios.h"

#include     "sdio_func.h"
#include     "sdio_cis.h"
#include     "sdio.h"      //Head files of SDIO partion.
#include     "sdio_ops.h"
#include     "sdio_ids.h"

//#define      DBUGPBUF
#define ARPHRD_ETHER 	    1		/* Ethernet 10Mbps		*/

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define min(x, y) (x < y ? x : y)
#define max(x, y) (x < y ? y : x)


#define isprint(c)  ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || \
                    (c >= '0' && c <= '9')) || (c == '_')


 struct iw_request_info
 {
     unsigned short       cmd;        /* Wireless Extension command */
     unsigned short       flags;      /* More to come ;-) */
 };



#endif

