/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   Msg.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    
********************************************************************************
*/
#define _IN_MSG_

#include "OsInclude.h"

/*
--------------------------------------------------------------------------------
  Function name :  void SendMsg(MSG_ID MsgId)
  Author        :  ZHengYongzhi
  Description   :  
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_CODE_
void SendMsg(MSG_ID MsgId)
{
//    UINT16 imask_tmp;
    
//    GLOBAL_INTERRUPT_DISABLE;
    
    Msg[MsgId / 32] |= 1 << (MsgId % 32);
    
//    GLOBAL_INTERRUPT_ENABLE;
}

/*
--------------------------------------------------------------------------------
  Function name : BOOL GetMsg(MSG_ID MsgId)
  Author        : ZHengYongzhi
  Description   : receive and clear message
                  
  Input         : message ID
  Return        : whether get message.

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_CODE_
BOOL GetMsg(MSG_ID MsgId)
{
//    UINT16 imask_tmp;
    
    if (Msg[MsgId / 32] & (1 << (MsgId % 32))) 
    {     
//        GLOBAL_INTERRUPT_DISABLE;
        Msg[MsgId / 32] &= ~(1 << (MsgId % 32));
//        GLOBAL_INTERRUPT_ENABLE;
        
        return(TRUE);
    }
    return(FALSE);
}

/*
--------------------------------------------------------------------------------
  Function name : void ClearMsg(MSG_ID MsgId)
  Author        : ZHengYongzhi
  Description   : clear message
                  
  Input         : message ID
  Return        : 

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_CODE_
void ClearMsg(MSG_ID MsgId)
{
//    UINT16 imask_tmp;
    
//    GLOBAL_INTERRUPT_DISABLE;
    
    Msg[MsgId / 32] &= ~(1 << (MsgId % 32));
    
//    GLOBAL_INTERRUPT_ENABLE;
}

/*
--------------------------------------------------------------------------------
  Function name : BOOL CheckMsg(MSG_ID MsgId)
  Author        : ZHengYongzhi
  Description   : judge message bit is been set
                  
  Input         : message ID
  Return        : whether get message

  History:     <author>         <time>         <version>       
             ZhengYongzhi     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
_ATTR_OS_CODE_
BOOL CheckMsg(MSG_ID MsgId)
{
    if (Msg[MsgId / 32] & (1 << (MsgId % 32))) 
    {     
        return(TRUE);
    }
    return(FALSE);
}
/*
********************************************************************************
*
*                         End of Win.c
*
********************************************************************************
*/
