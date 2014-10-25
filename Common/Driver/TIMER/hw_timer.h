/*
********************************************************************************
*                   Copyright (c) 2008,anzhiguo
*                         All rights reserved.
*
* File Name£º   hw_timer.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             anzhiguo      2009-1-14          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _HW_TIMER_H_
#define _HW_TIMER_H_

#undef  EXT
#ifdef  _IN_TIMER_
#define EXT
#else
#define EXT extern
#endif

/*
--------------------------------------------------------------------------------
  
            external memory control memap register define
  
--------------------------------------------------------------------------------
*/
typedef volatile struct {
    
    UINT32 LoadCountRegister;               //load data R Offset:0x00
    UINT32 CurrentValue;                    //current data R Offset:0x04
    UINT32 ControlReg;                      //control R Offset:0x08    
    UINT32 TimerEOI;                        //clear zero R£¨read operation clear zero£© Offset:0x0c
    UINT32 TimerIntStatus;                  //interupt status R Offset:0x10 
    
} TIMER_t,*gTIMER_t;
#define Timer                        ((TIMER_t *) TIMER_BASE)


/*
*-------------------------------------------------------------------------------
*  
*  The following define the bit fields in the TimerControlReg .
*  
*-------------------------------------------------------------------------------
*/
#define TIMR_EN                ((UINT32)1<<0) //timer enable
#define TIMR_USER_MODE         ((UINT32)1<<1)  // 0:free-running, 1:usermode
#define TIMR_IMASK             ((UINT32)1<<2) //timer interrupt mask

/*
********************************************************************************
*
*                         End of hw_timer.h
*
********************************************************************************
*/
#endif
