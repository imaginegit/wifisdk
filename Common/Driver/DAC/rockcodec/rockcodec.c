/*
********************************************************************************
*                   Copyright (c) 2008, Rock-Chips
*                         All rights reserved.
*
* File Name：   Rockcodec.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo          2009-3-24         1.0
*    desc:    ORG.
********************************************************************************
*/

#include "DriverInclude.h"

#if (CODEC_CONFIG == CODEC_ROCKC)

#include "rockcodec.h"

#define  MAX_OUTLEVEL            10
typedef struct _SRUCT_CODEC_CONFIG
{
    UINT8 DacAnaVol;
    UINT8 DacDigVol;
}SRUCT_CODEC_CONFIG;

_ATTR_DRIVER_CODE_ SRUCT_CODEC_CONFIG CodecConfig[MAX_VOLUME + 1 + 23] = 
{
    0, 255,
    0, 142, 
    0, 137,
    0, 132,
    0, 126,
    0, 120,
    0, 114,
    0, 109,
    0, 103,
    0, 97,
    0, 91,
    0, 87,
    0, 81,
    0, 75,
    0, 69,
    0, 64,
    0, 59,
    0, 51,
    0, 47,
    0, 42,
    0, 36,
    0, 28,
    0, 23,
    0, 17,
    0, 12,
    0, 7,
    0, 0,
    2, 0,    
    4, 0,    
    5, 0,
    6, 0,
    7, 0,
    8, 2,

//以下用于邋EQ补偿
    8, 1,
    8, 0,
    9, 2,
    9, 1,
    9, 0,
    10, 2,
    10, 1,
    10, 0,
    11, 2,
    11, 1,
    11, 0,
    12, 2,
    12, 1,
    12, 0,
    13, 2,
    13, 1,
    13, 0,
    14, 2,
    14, 1,
    14, 0,
    15, 2,
    15, 1,
    15, 0,
};

/*
--------------------------------------------------------------------------------
  Function name : void Codec_PowerOnInitial(void)
  Author        : yangwenjie
  Description   : Codec power on initial
                  
  Input         :      
                            
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  Note:      
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void Codec_PowerOnInitial(void)
{
    //Disable Force to VDD1.2 or VDDIO
    Grf->MPMU_HP_CON = ((MPMU_HP_FORCE_1200MV_MASK) << 16) | MPMU_HP_FORCE_1200MV_INACTIVE;
    Grf->MPMU_HP_CON = ((MPMU_HP_FORCE_VDDIO_MASK) << 16) | MPMU_HP_FORCE_VDDIO_INACTIVE;

    //Mute
    Grf->MPMU_HP_CON  = (MPMU_HP_MIX_MASK << 16) | MPMU_HP_MIX_NO_INPUT;
    Grf->MPMU_DAC_CON = (MPMU_DAC_MUTE_MASK << 16) | MPMU_DAC_MUTE;

    //Power Down
    Grf->MPMU_DAC_CON = (MPMU_DAC_POWER_MASK << 16) | MPMU_DAC_POWER_DOWM;
    Grf->MPMU_HP_CON  = (MPMU_HP_POWER_MASK << 16) | MPMU_HP_POWER_DOWM;
}

/*
--------------------------------------------------------------------------------
  Function name : void Codec_DeIitial(void)
  Author        : yangwenjie
  Description   : close Codec
                  
  Input         : null     
                            
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  Note:      
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Codec_DeInitial(void)
{
    //Mute
    Grf->MPMU_HP_CON  = (MPMU_HP_MIX_MASK << 16) | MPMU_HP_MIX_NO_INPUT;
    Grf->MPMU_DAC_CON = (MPMU_DAC_MUTE_MASK << 16) | MPMU_DAC_MUTE;

    //Power Down
    Grf->MPMU_DAC_CON = (MPMU_DAC_POWER_MASK << 16) | MPMU_DAC_POWER_DOWM;
    Grf->MPMU_HP_CON  = (MPMU_HP_POWER_MASK << 16) | MPMU_HP_POWER_DOWM;
}

/*
--------------------------------------------------------------------------------
  Function name : void Codec_SetMode(CodecMode_en_t Codecmode)
  Author        : yangwenjie
  Description   : 
                  
  Input         : Codecmode：
                            
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  Note:      if exit from application, like FM or MIC , please set codec to standby mode
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Codec_SetMode(CodecMode_en_t Codecmode)
{
    Grf->MPMU_HP_CON  = (MPMU_HP_POWER_MASK << 16) | MPMU_HP_POWER_UP;
    switch (Codecmode)
    {
    case Codec_DACout:
        Codec_SetVolumet(0);
        Grf->MPMU_HP_CON  = ((MPMU_HP_MIX_MASK) << 16) | (MPMU_HP_MIX_DAC_IN); 
        DelayMs(20);
        Grf->MPMU_DAC_CON = (MPMU_DAC_POWER_MASK << 16) | MPMU_DAC_POWER_UP;
        Codec_DACUnMute();
        break;

    case Codec_FMin:
        Grf->MPMU_HP_CON = ((MPMU_HP_MIX_MASK) << 16) | (MPMU_HP_MIX_LINE_IN);
        DelayMs(20);
        Grf->MPMU_DAC_CON = (MPMU_DAC_POWER_MASK << 16) | MPMU_DAC_POWER_DOWM;
        break;
        
    default:
        Grf->MPMU_DAC_CON = (MPMU_DAC_POWER_MASK << 16) | MPMU_DAC_POWER_DOWM;
        //Grf->MPMU_HP_CON  = ((MPMU_HP_MIX_MASK | MPMU_HP_POWER_MASK) << 16) | (MPMU_HP_MIX_NO_INPUT | MPMU_HP_POWER_DOWM);
        break;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Codec_SetMode(CodecMode_en_t Codecmode)
  Author        : yangwenjie
  Description   : 
                  
  Input         : 
                            
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  note:      if exit from application, like FM or MIC , please set codec to standby mode
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Codec_ExitMode(void)
{
    Codec_DACMute();
    DelayMs(20);
    
    //no input
    Grf->MPMU_HP_CON = ((MPMU_HP_MIX_MASK) << 16) | (MPMU_HP_MIX_NO_INPUT);
    DelayMs(20);

    //Power Down
    Grf->MPMU_DAC_CON = (MPMU_DAC_POWER_MASK << 16) | MPMU_DAC_POWER_DOWM;
    Grf->MPMU_HP_CON  = (MPMU_HP_POWER_MASK << 16) | MPMU_HP_POWER_DOWM;
}
/*
--------------------------------------------------------------------------------
  Function name : void Codec_SetSampleRate(CodecFS_en_t CodecFS)
  Author        : yangwenjie
  Description   : 
                  
  Input         : 
                            
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  Note:      
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Codec_SetSampleRate(CodecFS_en_t CodecFS)
{  
    switch (CodecFS)
    {
        case FS_8000Hz:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ8kHz;
            break;
            
        case FS_11025Hz:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ11025Hz;
            break;
            
        case FS_12KHz:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ12kHz;
            break;
            
        case FS_16KHz:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ16kHz;
            break;
            
        case FS_22050Hz:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ22050Hz;
            break;
            
        case FS_24KHz:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ24kHz;
            break;
            
        case FS_32KHz:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ32kHz;
            break;
            
        case FS_48KHz:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ48kHz;
            break;
            
        case FS_44100Hz:
        default:
            Grf->MPMU_HP_CON = (MPMU_FREQ_MASK << 16) | MPMU_FREQ441kHz;
            break;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void Codec_SetVolumet(unsigned int Volume)
  Author        : yangwenjie
  Description   : codec control volume
                  
  Input         : Volume
                            
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  Note:      volume = 0 mean mute,
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Codec_SetVolumet(unsigned int Volume)
{
    Grf->MPMU_DAC_CON = (MPMU_DAC_VOL_UPD_MASK << 16) | MPMU_DAC_VOL_UPD_INACTIVE;

    //Grf->MPMU_HP_CON = ((MPMU_HP_FORCE_VDDIO_MASK) << 16) | MPMU_HP_FORCE_VDDIO_ACTIVE;
    //Grf->MPMU_HP_CON = ((MPMU_HP_FORCE_1200MV_MASK) << 16) | MPMU_HP_FORCE_1200MV_ACTIVE;

    if (Volume == 0)
    {
        Grf->MPMU_DAC_CON = (((MPMU_DAC_DIG_VOL_MASK | MPMU_DAC_ANA_VOL_MASK) << 16) | (MPMU_DAC_DIG_VOL(CodecConfig[Volume].DacDigVol) | MPMU_DAC_ANA_VOL(CodecConfig[Volume].DacAnaVol)));
        //Grf->MPMU_DAC_CON = ((MPMU_DAC_MUTE_MASK << 16) | MPMU_DAC_MUTE);      
    }
    else
    {
        Volume += OutputVolOffset;
        Grf->MPMU_DAC_CON = (((MPMU_DAC_DIG_VOL_MASK | MPMU_DAC_ANA_VOL_MASK) << 16) | (MPMU_DAC_DIG_VOL(CodecConfig[Volume].DacDigVol) | MPMU_DAC_ANA_VOL(CodecConfig[Volume].DacAnaVol)));    
    }
    
    Grf->MPMU_DAC_CON = (MPMU_DAC_VOL_UPD_MASK << 16) | MPMU_DAC_VOL_UPD_ACTIVE;
    
}
/*
--------------------------------------------------------------------------------
  Function name : void Codec_DACMute(void)
  Author        : yangwenjie
  Description   : set codec mute
                  
  Input         : null     
                            
  Return        : null

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  Note:      this function only used when DAC working 
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Codec_DACMute(void)
{
    Grf->MPMU_DAC_CON = ((MPMU_DAC_MUTE_MASK) << 16) | (MPMU_DAC_MUTE);
    //Grf->MPMU_HP_CON = ((MPMU_HP_FORCE_1200MV_MASK) << 16) | MPMU_HP_FORCE_1200MV_ACTIVE;
}

/*
--------------------------------------------------------------------------------
  Function name : void Codec_DACUnMute(void)
  Author        : yangwenjie
  Description   : set codec exit from mute.
                  
  Input         :      
                            
  Return        : 

  History:     <author>         <time>         <version>       
             yangwenjie     2008-11-20         Ver1.0
  desc:         ORG
  note:      this function only used when DAC working
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void Codec_DACUnMute(void)
{
    Grf->MPMU_DAC_CON = ((MPMU_DAC_MUTE_MASK) << 16) | (MPMU_DAC_UNMUTE);
    //Codec_SetVolumet(gSysConfig.OutputVolume);
}

/*
********************************************************************************
*
*                         End of Rockcodec.c
*
********************************************************************************
*/
#endif

