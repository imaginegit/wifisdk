/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   macro.h
* 
* Description:  typedef.h -Macros define the data type
*                      
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _MACRO_H_
#define _MACRO_H_

/*
*-------------------------------------------------------------------------------
*
*                           macro define
*
*-------------------------------------------------------------------------------
*/
//section define
#define ALIGN_ATTR( align )     __attribute__((aligned (align)))

#define NULL                0

#define BUSY                1
#define OK                  0
#define ERROR               (-1)
#define TIMEOUT             (-2)

#define RETURN_OK           0
#define RETURN_FAIL         (-1)

/*
********************************************************************************
*
*                         End of macro.h
*
********************************************************************************
*/
#endif
