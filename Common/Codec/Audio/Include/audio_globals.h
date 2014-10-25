/*
********************************************************************************
*                   Copyright (c) 2008, Rockchip
*                         All rights reserved.
*
* File Name£º   audio_globals.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             Vincent Hsiung     2009-1-8          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _AUDIO_GLOBALS_H_
#define _AUDIO_GLOBALS_H_

#include "audio_main.h"
//#include "Effect.h"
//****************************************************************************
// The following values are the IOCTLs which are sent to the individual codec
// drivers.
//****************************************************************************

enum
{
    SUBFN_CODEC_GETNAME,
    SUBFN_CODEC_GETARTIST,
    SUBFN_CODEC_GETTITLE,
    SUBFN_CODEC_GETBITRATE,
    SUBFN_CODEC_GETSAMPLERATE,
    SUBFN_CODEC_GETCHANNELS,
    SUBFN_CODEC_GETLENGTH,
    SUBFN_CODEC_GETTIME,
    SUBFN_CODEC_OPEN_DEC,
    SUBFN_CODEC_OPEN_ENC,
    SUBFN_CODEC_GETBUFFER,
    SUBFN_CODEC_SETBUFFER,
    SUBFN_CODEC_DECODE,
    SUBFN_CODEC_ENCODE,
    SUBFN_CODEC_SEEK,
    SUBFN_CODEC_CLOSE,
    SUBFN_CODEC_ZOOM
};

//****************************************************************************
// The following are the flags passed to CodecOpen.
//****************************************************************************
#define CODEC_OPEN_ENCODE       0x00000001
#define CODEC_OPEN_DECODE       0x00000002
#define CODEC_OPEN_PLAYLIST     0x00000004

//****************************************************************************
// The following are the flags passed to CodecSeek.
//****************************************************************************
#define CODEC_SEEK_FF           0x00000000
#define CODEC_SEEK_REW          0x00000001
#define CODEC_SEEK_OTHER        0x00000002

//****************************************************************************
// Function prototypes and global variables.
//****************************************************************************

// pMP3.C
extern unsigned long MP3Function(unsigned long ulSubFn, unsigned long ulParam1,
                                 unsigned long ulParam2,unsigned long ulParam3);

// pWMA.C
extern unsigned long WMAFunction(unsigned long ulSubFn, unsigned long ulParam1,
                                 unsigned long ulParam2,unsigned long ulParam3);

//pFLAC.C
extern unsigned long FLACDecFunction(unsigned long ulSubFn, unsigned long ulParam1,
                                 unsigned long ulParam2,unsigned long ulParam3);

// pAPE.C
extern unsigned long APEDecFunction(unsigned long ulSubFn, unsigned long ulParam1,
                                 unsigned long ulParam2,unsigned long ulParam3);

// pMSADPCM.C
extern unsigned long MSADPCMFunction(unsigned long ulSubFn, unsigned long ulParam1, 
                                    unsigned long ulParam2,unsigned long ulParam3);
                                    
// pWAV.C
extern unsigned long PCMFunction(unsigned long ulIoctl, unsigned long ulParam1,
                                unsigned long ulParam2, unsigned long ulParam3);

//pAAC.c
extern unsigned long 
AACDecFunction(unsigned long ulSubFn, unsigned long ulParam1,
               unsigned long ulParam2, unsigned long ulParam3);
// From codec.c
///////////////////////////////////////////////////////////////////////////////
extern unsigned long CodecOpen(unsigned long ulCodec, unsigned long ulFlags);
extern unsigned long CodecGetSampleRate(unsigned long *pulSampleRate);
extern unsigned long CodecGetChannels(unsigned long *pulChannels);
extern unsigned long CodecGetLength(unsigned long *pulLength);
extern unsigned long CodecGetTime(unsigned long *pulTime);
extern unsigned long CodecGetCaptureBuffer(short *ppsBuffer, long *plLength);
extern unsigned long CodecSetBuffer(short *psBuffer);
extern unsigned long CodecDecode(void);
extern unsigned long CodecEncode(void);
extern unsigned long CodecSeek(unsigned long ulTime, unsigned long ulSeekType);
extern unsigned long CodecClose(void);
extern unsigned long CodecGetBitrate(unsigned long *pulBitrate);

//------------------------------------------------------------
enum 
{
    CODEC_MP3 = 0,
#ifdef  WMA_INCLUDE
    CODEC_WMA,
#endif

#ifdef  AAC_INCLUDE
    CODEC_AAC,
#endif

#ifdef WAV_INCLUDE
    CODEC_WAV,
#endif

#ifdef APE_DEC_INCLUDE
    CODEC_APE_DEC,
#endif

#ifdef FLAC_DEC_INCLUDE
    CODEC_FLAC_DEC,
#endif
    NUMCODECS
};
#if 0
//-------------------------------------------------------------
#define TRACK_INFO_STRING_LEN   32

typedef struct
{
    unsigned long  TotalTime;
    unsigned long  CurrentTime;
    int            samplerate;
    int            bitrate;
    int            channels;
    int            total_length;
    int            cur_pos;
    int            RepeatMode;
    int            PlayOrder; //lyrics or order
    int            AudioFileDirOrAll;//directory files or all files.
    RKEffect       UserEQ;
    char           artist[TRACK_INFO_STRING_LEN];
    char           title[TRACK_INFO_STRING_LEN];
    char           albumb[TRACK_INFO_STRING_LEN];
    int            IsEQUpdate;
}track_info;



typedef struct
{
    short  *pPCMBuf;
    
    long   lOutLength;

    short  *EncOutBuf;

    long   flag;

    RKEffect EffectCtl;
}AudioInOut_Type;
#endif

#endif
