/*
********************************************************************************
*                   Copyright (c) 2008,anzhiguo
*                         All rights reserved.
*
* File Name：  AD_key.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*               anzhiguo      2009-2-10          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_ADKEY_

#include "DriverInclude.h"
#include "OsInclude.h"

/*
--------------------------------------------------------------------------------
  Function name : KeyReset(void)
  Author        : anzhiguo
  Description   : key reset
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void KeyReset(void)
{
    KeyVal       = KEY_VAL_NONE;
    KeyScanVal   = KEY_VAL_NONE;
    KeyStatus    = KEY_STATUS_NONE;
    
    KeyScanCounter  = 0;
    
    KeyFlag.bc.bKeyHave = 0;
    KeyFlag.bc.bKeyDown = 0;
    
}

/*
--------------------------------------------------------------------------------
  Function name : KeyReset(void)
  Author        : anzhiguo
  Description   : get key value
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
uint32 keybackup = 0;
_ATTR_DRIVER_CODE_
UINT32 GetKey(UINT32 *pKeyVal)
{
    UINT32 KeyTemp,HoldTemp;
    UINT32 RetVal = 0;

	KeyTemp = 1;
    if (Scu_PlayOn_Level_Get())
    {
        KeyTemp = 0;
    }
	HoldTemp = KeyTemp & 0x01;
	
	if(KeyScanCounter == 0)
	{		
	    *pKeyVal = (UINT32)((KeyTemp & 0x0001));    //paly key or no key
		RetVal = -1;
	}
	else if(KeyScanCounter == 1)
	{
        if (CheckAdcState(ADC_CHANEL_KEY))
        {
			KeyTemp = GetAdcData(ADC_CHANEL_KEY);
            //DEBUG("ADKey = %d", KeyTemp);
		    
			if ((ADKEY2_MIN <= KeyTemp ) && (KeyTemp < ADKEY2_MAX)) 
			{
				*pKeyVal |= KEY_VAL_ADKEY2;
			} 
			else if ((ADKEY3_MIN <= KeyTemp ) && (KeyTemp < ADKEY3_MAX)) 
			{
				*pKeyVal |= KEY_VAL_ADKEY3;		
			}
			else if ((ADKEY4_MIN <= KeyTemp ) && (KeyTemp < ADKEY4_MAX)) 
			{
				*pKeyVal |= KEY_VAL_ADKEY4;
			}
			else if ((ADKEY5_MIN <= KeyTemp ) && (KeyTemp < ADKEY5_MAX)) 
			{
				*pKeyVal |= KEY_VAL_ADKEY5;
			}
			else if ((ADKEY6_MIN <= KeyTemp ) && (KeyTemp < ADKEY6_MAX)) 
			{
				*pKeyVal |= KEY_VAL_ADKEY6;
			}
			else if ((ADKEY7_MIN <= KeyTemp ) && (KeyTemp < ADKEY7_MAX)) 
			{
				*pKeyVal |= KEY_VAL_ADKEY7;
			}
			
		}
		RetVal = 0;
	}
    
	if ((HoldTemp == 1) && (*pKeyVal & KEY_VAL_MENU))
	{
	    *pKeyVal = KEY_VAL_HOLD;
	}

    KeyScanCounter = (KeyScanCounter + 1) % 2;//为什么要进行2次 ，如果两个按键合在一起会有什么哪?
	
    return RetVal;
}
/*
--------------------------------------------------------------------------------
  Function name : void KeyScan(void)
  Author        : anzhiguo
  Description   : key scan program
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void KeyScan(void)
{
	
}

/*
--------------------------------------------------------------------------------
  Function name : AdcKey_SysTimer_Handler()
  Author        : anzhiguo
  Description   : ADC Key Timer Isr
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void AdcKey_SysTimer_Handler(void)
{
    KeyScan();
    AdcStart(ADC_CHANEL_KEY);
}

_ATTR_DRIVER_DATA_
SYSTICK_LIST AdcKey_SysTimer =
{ 
    NULL, 
    0, 
    1, 
    0, 
    AdcKey_SysTimer_Handler,
};

/*
--------------------------------------------------------------------------------
  Function name : KeyInit()
  Author        : anzhiguo
  Description   : key scan variables initialization.
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_SYS_INIT_CODE_
void KeyInit(void)
{
    KeyVal          = 0;//KeyStatus + KeyScanVal
    PrevKeyVal      = 0;
    KeyStatus       = 0;
    KeyScanCnt      = 0;
    
    KeyScanVal      = KEY_VAL_NONE;//which key press down...
    KeyScanCounter  = 0;
    KeyFlag.word    = 0;

    SystickTimerStart(&AdcKey_SysTimer);
}
/*
--------------------------------------------------------------------------------
  Function name : void ClearVolMsg(void)
  Author        : anzhiguo
  Description   : 
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_DATA_ UINT8 showtimes = 0;
_ATTR_DRIVER_CODE_
void ClearVolMsg(void)
{
    showtimes = 0;
    ClearMsg(MSG_KEY_VOL_STATE);
    ClearMsg(MSG_KEY_VOL_SHOWSTATE);
}

/*
--------------------------------------------------------------------------------
  Function name : void KeyVolAdjStart(void)
  Author        : anzhiguo
  Description   : 
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void KeyVolAdjStart(void)
{
    KeyVolAdjustCounter = SysTickCounter;
    SendMsg(MSG_KEY_VOL_STATE);
    SendMsg(MSG_KEY_VOL_STATE_UPDATE);
}

/*
--------------------------------------------------------------------------------
  Function name : void KeyVolAdjCheck(void)
  Author        : anzhiguo
  Description   : 
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
void KeyVolAdjCheck(void)
{
    UINT32 Count;

    if (CheckMsg(MSG_KEY_VOL_STATE))
    {
        if((SysTickCounter - KeyVolAdjustCounter) >= 50)//0.5s
        {
            KeyVolAdjustCounter = SysTickCounter;
            showtimes++;
            if (showtimes & 0x01)
            {
                SendMsg(MSG_KEY_VOL_SHOWSTATE);
            }
            else
            {
                ClearMsg(MSG_KEY_VOL_SHOWSTATE);
            }
            
            if(showtimes > 10)// 8s显示时间
            {
                showtimes = 0;
                ClearMsg(MSG_KEY_VOL_STATE);
			    ClearMsg(MSG_KEY_VOL_SHOWSTATE);
            }
            
            SendMsg(MSG_KEY_VOL_STATE_UPDATE);
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : UINT32 GetKeyVal(void)
  Author        : anzhiguo
  Description   : 
                  
  Input         : null
                  
  Return        : null

  History:     <author>         <time>         <version>       
               anzhiguo        2009-1-14         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_DRIVER_CODE_
UINT32 GetKeyVal(void)
{
    UINT32 imask_tmp;
    UINT32 KeyValTemp;
    
    if (gSysConfig.KeyNum == KEY_NUM_5)
    {
        KeyVolAdjCheck();
    }
    
    if(KeyFlag.bc.bKeyHave == 0) //没有任何按键按下
    {
        return  KEY_VAL_NONE;
    }
        
    //TIMER0_INTERRUPT_DISABLE;
    KeyValTemp = KeyVal;
    KeyFlag.bc.bKeyHave = 0;
    //TIMER0_INTERRUPT_ENABLE;
 
    if (KeyFlag.bc.bKeyHold == 0) 
    {
        if (KeyValTemp == KEY_VAL_HOLD_PRESS_START) 
        {
            KeyFlag.bc.bKeyHold = 1;
         
            KeyValTemp = KEY_VAL_HOLD_ON;
        }
    } 
    else
    {
        if ((KeyValTemp & KEY_VAL_MASK) == KEY_VAL_HOLD) 
        {
            if (KeyValTemp == KEY_VAL_HOLD_PRESS_START) 
            {
                KeyFlag.bc.bKeyHold = 0;
               
                KeyValTemp = KEY_VAL_HOLD_OFF;
            } 
            else 
            {
                KeyValTemp = KEY_VAL_NONE;
            }
        } 
        else 
        {
            if ((KeyValTemp & KEY_VAL_UNMASK) == KEY_STATUS_DOWN)
            {
                KeyValTemp = KEY_VAL_HOLD_ON;
            } 
            else 
            {
                KeyValTemp = KEY_VAL_NONE;
            }
        }
    }
    
    return (KeyValTemp);

}

/*
********************************************************************************
*
*                         End of AD_Key.c
*
********************************************************************************
*/

