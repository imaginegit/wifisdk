/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   Thread.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _THREAD_H_
#define _THREAD_H_

#undef  EXT
#ifdef _IN_THREAD_
#define EXT
#else
#define EXT extern
#endif

/*
--------------------------------------------------------------------------------
  
  Description:  task initial parameter
  
--------------------------------------------------------------------------------
*/
//music playing window task switch parameters
typedef __packed struct _AUDIO_THREAD_ARG_
{
    uint16 FileNum;
    
} AUDIO_THREAD_ARG;

//video  playing window task switch parameters
typedef __packed struct _VIDEO_THREAD_ARG_
{
    uint16 FileNum;
    
} VIDEO_THREAD_ARG;

//picture playing window task switch parameters
typedef __packed struct _PICTURE_THREAD_ARG_
{
    uint16 FileNum;
    
} PICTURE_THREAD_ARG;

/*
--------------------------------------------------------------------------------
  
  Description:  window structrue definition
  
--------------------------------------------------------------------------------
*/
typedef struct THREAD_STRUCT {
    
    struct THREAD_STRUCT *Prev;
    struct THREAD_STRUCT *Next;
    
    void (*pgThreadInit)(void*);            //initial function of thread initial
    void (*pgThreadProc)(void);             //handle function of thread serivice.
    void (*pgThreadDeInit)(void);           //initial function for thread auti-initial
    
    void *pArg;                             //thread initial parameter
    
}THREAD;

_ATTR_OS_BSS_   EXT THREAD  *pMainThread;

/*
--------------------------------------------------------------------------------
  
                        Funtion Declaration
  
--------------------------------------------------------------------------------
*/

#ifdef DRIVERLIB_IRAM

UINT32 ThreadCreat(THREAD **pThreadFirst, THREAD *pThread, void *pArg);
UINT32 ThreadDelete(THREAD **pThreadFirst, THREAD *pThread);
UINT32 ThreadDeleteAll(THREAD **pThreadFirst);
UINT32 ThreadCheck(THREAD *pThreadFirst, THREAD *pThread);
UINT32 ThreadProcess(THREAD *pThreadFirst);

#else

typedef UINT32 (*pThreadCreat)(THREAD **pThreadFirst, THREAD *pThread, void *pArg);
typedef UINT32 (*pThreadDelete)(THREAD **pThreadFirst, THREAD *pThread);
typedef UINT32 (*pThreadDeleteAll)(THREAD **pThreadFirst);
typedef UINT32 (*pThreadCheck)(THREAD *pThreadFirst, THREAD *pThread);
typedef UINT32 (*pThreadProcess)(THREAD *pThreadFirst);

#define ThreadCreat(pThreadFirst, pThread, pArg) (((pThreadCreat    )(Addr_ThreadCreat    ))(pThreadFirst, pThread, pArg))
#define ThreadDelete(pThreadFirst, pThread)      (((pThreadDelete   )(Addr_ThreadDelete   ))(pThreadFirst, pThread))
#define ThreadDeleteAll(pThreadFirst)            (((pThreadDeleteAll)(Addr_ThreadDeleteAll))(pThreadFirst))
#define ThreadCheck(pThreadFirst, pThread)       (((pThreadCheck    )(Addr_ThreadCheck    ))(pThreadFirst, pThread))
#define ThreadProcess(pThreadFirst)              (((pThreadProcess  )(Addr_ThreadProcess  ))(pThreadFirst))

#endif

/*
********************************************************************************
*
*                         End of Thread.h
*
********************************************************************************
*/
#endif
