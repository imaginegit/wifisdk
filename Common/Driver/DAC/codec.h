/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   Hw_codec.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                               2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _HW_CODEC_H
#define _HW_CODEC_H

#define CODEC_ROCKC     0
#define CODEC_WM8987    1

#define CODEC_CONFIG    CODEC_ROCKC

#define MAX_VOLUME      32

typedef enum CodecMode
{
    Codec_DACout,  // for MP3, MP4 playback
    Codec_FMin,   // for FM playback
    Codec_FMADC,  // for FM recording, output remain FM singal
    Codec_LineADC,  // for Line in recording
    Codec_MICAdc,  // for MIC recording
    Codec_Standby  // for player no application work, like main menu, or exit from FM
}CodecMode_en_t;

typedef enum CodecFS
{
    FS_8000Hz   = 8000,
    FS_11025Hz  = 11025,
    FS_12KHz    = 12000,
    FS_16KHz    = 16000,
    FS_22050Hz  = 22050,
    FS_24KHz    = 24000,
    FS_32KHz    = 32000,
    FS_44100Hz  = 44100,
    FS_48KHz    = 48000,
    FSSTOP      = 96000
} CodecFS_en_t;


void Codec_PowerOnInitial(void);
void Codec_SetMode(CodecMode_en_t Codecmode);
void Codec_SetSampleRate(CodecFS_en_t CodecFS);
void Codec_SetVolumet(unsigned int Volume);
void Codec_DACMute(void);
void Codec_DACUnMute(void);
void Codec_DeInitial(void);
void Codec_ExitMode(void);

/*
********************************************************************************
*
*                         End of Codec.h
*
********************************************************************************
*/

#endif

