/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   nvic.h
* 
* Description:  
*                      
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _NVIC_H_
#define _NVIC_H_

/*
*-------------------------------------------------------------------------------
*  
*                           Function Declaration
*
*-------------------------------------------------------------------------------
*/
#ifdef DRIVERLIB_IRAM

extern void  NvicSystemReset(void);
extern void  NvicCpuReset(void);
extern void  NvicClearActive(void);
extern void  NvicVectorTableSet(uint32 vectortaboffset);

#else

typedef void (*pNvicSystemReset)(void);
typedef void (*pNvicCpuReset)(void);
typedef void (*pNvicClearActive)(void);
typedef void (*pNvicVectorTableSet)(uint32 vectortaboffset);

#define NvicSystemReset()                    (((pNvicSystemReset   )(Addr_NvicSystemReset   ))())
#define NvicCpuReset()                       (((pNvicCpuReset      )(Addr_NvicCpuReset      ))())
#define NvicClearActive()                    (((pNvicClearActive   )(Addr_NvicClearActive   ))())
#define NvicVectorTableSet(vectortaboffset)  (((pNvicVectorTableSet)(Addr_NvicVectorTableSet))(vectortaboffset))

#endif

/*
********************************************************************************
*
*                         End of nvic.h
*
********************************************************************************
*/
#endif
