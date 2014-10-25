/*
********************************************************************************
*                   Copyright (c) 2008, Rockchip
*                         All rights reserved.
*
* File Name£º   audio_main.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             Vincent Hsiung     2009-1-8          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _AUDIO_MAIN_H_
#define _AUDIO_MAIN_H_

#include "SysConfig.h"

/************************************************************
1. Select any Audio CODEC plug-in
*************************************************************/
#ifdef _MP3_DECODE_
	#define MP3_INCLUDE
#endif

//WIFI Music Section define
#define	_ATTR_WiFi_AUDIO_TEXT_	__attribute__((section("WiFiAudioCode")))
#define	_ATTR_WiFi_AUDIO_DATA_	__attribute__((section("WiFiAudioData")))
#define	_ATTR_WiFi_AUDIO_BSS_	__attribute__((section("WiFiAudioBss"),zero_init))

//-------------------------------------------MP3----------------------------------------------------------
#define _ATTR_MP3INIT_TEXT_		__attribute__((section("Mp3InitCode")))
#define _ATTR_MP3INIT_DATA_		__attribute__((section("Mp3InitData")))
#define _ATTR_MP3INIT_BSS_		__attribute__((section("Mp3InitBss"),zero_init))

#define _ATTR_MP3DEC_TEXT_     	__attribute__((section("Mp3Code")))
#define _ATTR_MP3DEC_DATA_     	__attribute__((section("Mp3Data")))
#define _ATTR_MP3DEC_BSS_      	__attribute__((section("Mp3Bss"),zero_init))

#define _ATTR_MP3COMMON_TEXT_	__attribute__((section("Mp3CommonCode")))
#define _ATTR_MP3COMMON_DATA_	__attribute__((section("Mp3CommonData")))
#define _ATTR_MP3COMMON_BSS_	__attribute__((section("Mp3CommonBss"),zero_init))

#endif		// _AUDIO_MAIN_H_

