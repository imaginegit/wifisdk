/*
********************************************************************************
*
*				  Copyright (c): 2013 - 2013 + 5, csun
*							   All rights reserved.
*
* FileName: ..\Common\NetRadio\http.h
* Owner: csun
* Date: 2013.3.13
* Time: 11:54:09
* Desc: http driver
* History:
*	 <author>	 <date> 	  <time>	 <version>	   <Desc>
*       csun     2013.3.13     11:54:23   1.0           create
*	 
********************************************************************************
*/

#ifndef __COMMON_NETRADIO_HTTP_H__
#define __COMMON_NETRADIO_HTTP_H__

#include "def.h"
/*
*-------------------------------------------------------------------------------
*
*							   #include define									 
*
*-------------------------------------------------------------------------------
*/

#define XMLCHECKNOTHING  -1

#define FORMAT_NULL			0
#define FORMAT_WAV			1
#define FORMAT_MP3			2
#define FORMAT_AAC			3
#define FORMAT_OGG			4
#define FORMAT_WMA			5
#define FORMAT_FLAC			6

/*
*-------------------------------------------------------------------------------
*
*							   #define / #typedef define						 
*
*-------------------------------------------------------------------------------
*/

/*
*-------------------------------------------------------------------------------
*
*							   global variable declare							 
*
*-------------------------------------------------------------------------------
*/



/*
*-------------------------------------------------------------------------------
*
*							   API Declare          							 
*
*-------------------------------------------------------------------------------
*/
extern void   Http_Recive_Data(TCP_PCB *pcb);
extern void   Http_Set_Timeout(uint32 timeout);
extern int    Http_Valid_Datalen(void);
extern uint32 Http_Resume(void);
extern uint32 Http_Pause(void);
extern uint32 Http_Close_Cur_Url(void);
extern uint32 Http_Write_Data(uint8 * pbDataBuf, uint32 dwLength);
extern uint32 Http_Read_Data(uint8 * pbDataBuf, uint32 dwLength);
extern uint32 Http_Get_Url(uint8 * pbUrl, uint32 * pdwDataLen, uint32 dwOffset, uint8 bWait);

extern int    Http_warn_pause_check(void);
extern uint32 Http_Get_CurOffset(void);

#endif

