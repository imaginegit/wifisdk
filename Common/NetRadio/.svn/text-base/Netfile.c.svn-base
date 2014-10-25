/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name£º   netbuffer.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                move           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/

#include "sysinclude.h"
#include "Msg.h"
#include "sdio_card.h"
#include "lwipopt.h"
#include "NetFile.h"
#include "Audio_File_Access.h"
#include "http.h"


typedef __packed struct _NET_FILE_PCB
{
    uint32 dwFileSize;
    uint32 dwOffset;
}NET_FILE_PCB, * P_NET_FILE_PCB;

NET_FILE_PCB xstNetFilePcb;



/*
 * audio file system interface.. 
 */
uint32 NetFileOpen(uint8 * pbUrl)
{    
    uint32 dwFileSize;
    uint32 dwRet;   

    dwRet = Http_Get_Url(pbUrl, &dwFileSize, 0, 1);    
	if((dwRet == RETURN_OK) && (dwFileSize != 0))
    {
        xstNetFilePcb.dwFileSize = dwFileSize;
        xstNetFilePcb.dwOffset = 0;
        SendMsg(MSG_WIFI_TCP_CONNECTING);
        return RETURN_OK;        
    }
    else
    {
        printf("NetFileOpen error!\n");
        Http_Close_Cur_Url();
        return RETURN_FAIL;
    }    
} 

unsigned long Net_FLength(FILE *in)
{
    printf("FLength\n");
    return xstNetFilePcb.dwFileSize;
}

unsigned short NetFileRead(unsigned char *b, unsigned short s, void *f)
{   
    uint32 readed = 0;
   
    if (xstNetFilePcb.dwOffset < xstNetFilePcb.dwFileSize)
    {
        readed = Http_Read_Data((uint8 *)b, (uint32)s);
        xstNetFilePcb.dwOffset += readed;
    }
    else
    {
        readed = 0;
    }

	if (readed < s)
	{
    	printf("NetFileRead: s = %d, readed = %d\n", s, readed);
	}
    
    return readed;
}

unsigned char NetFileSeek(unsigned long offset, unsigned char Whence, char Handle)
{
	uint32 Rt = RETURN_FAIL;//RETURN_OK;
    uint32 wantread;

    UserIsrDisable();
    printf("Fseek:%d %d %d\n", Whence, offset, xstNetFilePcb.dwOffset);
	switch (Whence)
	{
		case SEEK_SET:
            if((offset >= xstNetFilePcb.dwOffset) && (offset < xstNetFilePcb.dwFileSize))
            {
				wantread = offset - xstNetFilePcb.dwOffset;
				Http_Set_Timeout(XMLCHECKNOTHING);
				if (Http_Valid_Datalen() >= wantread)
				{
					Http_Read_Data(NULL, wantread);
					Rt = RETURN_OK;
				}
            }			
			break;

        case SEEK_CUR:
            if ((offset >= 0) && ((xstNetFilePcb.dwOffset + offset) < xstNetFilePcb.dwFileSize))
            {
				Http_Set_Timeout(XMLCHECKNOTHING);
				if (Http_Valid_Datalen() >= offset)
				{
					Http_Read_Data(NULL, offset);
					Rt = RETURN_OK;
				}
            }
			break;
            
		default:
			Rt = RETURN_FAIL;
			break;
            
	}
    UserIsrEnable(0);
	if (Rt == RETURN_FAIL)
	{
		SendMsg(MSG_WIFI_SEEK_ERROR);
	}
	
	return (Rt);
}

unsigned long Net_FTell(FILE *in)
{
     printf("Ftel\n");
     return xstNetFilePcb.dwOffset;
}

unsigned char NetFileClose(char Handle)
{
    Http_Close_Cur_Url();

    ClearMsg(MSG_WIFI_TCP_CONNECTING);   
    
    printf("NetFileClose\n");    
    return 0;
}

