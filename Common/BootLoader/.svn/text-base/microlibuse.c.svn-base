/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   nvic.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-8-13          1.0
*    desc:    ORG.
********************************************************************************
*/
#include "typedef.h"

#define _ATTR_MICROLIB_CODE_    __attribute__((section("SysInitCode")))
#define _ATTR_MICROLIB_DATA_    __attribute__((section("SysInitData")))
#define _ATTR_MICROLIB_BSS_     __attribute__((section("SysInitBss"),zero_init))

#if 1
_ATTR_MICROLIB_BSS_ int8   i8tmp[16];
_ATTR_MICROLIB_BSS_ int16  i16tmp[16];
_ATTR_MICROLIB_BSS_ int32  i32tmp[16];
_ATTR_MICROLIB_BSS_ int64  i64tmp[16];

_ATTR_MICROLIB_BSS_ uint8  u8tmp[16];
_ATTR_MICROLIB_BSS_ uint16 u16tmp[16];
_ATTR_MICROLIB_BSS_ uint32 u32tmp[16];
_ATTR_MICROLIB_BSS_ uint64 u64tmp[16];

_ATTR_MICROLIB_BSS_ float  ftmp[16];
_ATTR_MICROLIB_BSS_ double dtmp[16];

_ATTR_MICROLIB_BSS_ sint   itmp[16];
_ATTR_MICROLIB_BSS_ uint   utmp[16];

_ATTR_MICROLIB_BSS_ int8*  pint8;
//type conversion.
_ATTR_MICROLIB_CODE_
volatile void toi8(void)
{
    i8tmp[0] = (volatile)((int8)i8tmp[1]);
    i8tmp[1] = (volatile)((int8)i16tmp[1]);
    i8tmp[2] = (volatile)((int8)i32tmp[1]);
    i8tmp[3] = (volatile)((int8)i64tmp[1]);
    i8tmp[4] = (volatile)((int8)u8tmp[1]);
    i8tmp[5] = (volatile)((int8)u16tmp[1]);
    i8tmp[6] = (volatile)((int8)u32tmp[1]);
    i8tmp[7] = (volatile)((int8)u64tmp[1]);
    i8tmp[8] = (volatile)((int8)ftmp[1]);
    i8tmp[9] = (volatile)((int8)dtmp[1]);
    i8tmp[10]= (volatile)((int8)itmp[1]);
    i8tmp[11]= (volatile)((int8)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void tou8(void)
{
    u8tmp[0] = (volatile)((uint8)i8tmp[1]);
    u8tmp[1] = (volatile)((uint8)i16tmp[1]);
    u8tmp[2] = (volatile)((uint8)i32tmp[1]);
    u8tmp[3] = (volatile)((uint8)i64tmp[1]);
    u8tmp[4] = (volatile)((uint8)u8tmp[1]);
    u8tmp[5] = (volatile)((uint8)u16tmp[1]);
    u8tmp[6] = (volatile)((uint8)u32tmp[1]);
    u8tmp[7] = (volatile)((uint8)u64tmp[1]);
    u8tmp[8] = (volatile)((uint8)ftmp[1]);
    u8tmp[9] = (volatile)((uint8)dtmp[1]);
    u8tmp[10]= (volatile)((uint8)itmp[1]);
    u8tmp[11]= (volatile)((uint8)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void toi16(void)
{
    i16tmp[0] = (volatile)((int16)i8tmp[1]);
    i16tmp[1] = (volatile)((int16)i16tmp[1]);
    i16tmp[2] = (volatile)((int16)i32tmp[1]);
    i16tmp[3] = (volatile)((int16)i64tmp[1]);
    i16tmp[4] = (volatile)((int16)u8tmp[1]);
    i16tmp[5] = (volatile)((int16)u16tmp[1]);
    i16tmp[6] = (volatile)((int16)u32tmp[1]);
    i16tmp[7] = (volatile)((int16)u64tmp[1]);
    i16tmp[8] = (volatile)((int16)ftmp[1]);
    i16tmp[9] = (volatile)((int16)dtmp[1]);
    i16tmp[10]= (volatile)((int16)itmp[1]);
    i16tmp[11]= (volatile)((int16)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void tou16(void)
{
    u16tmp[0] = (volatile)((uint16)i8tmp[1]);
    u16tmp[1] = (volatile)((uint16)i16tmp[1]);
    u16tmp[2] = (volatile)((uint16)i32tmp[1]);
    u16tmp[3] = (volatile)((uint16)i64tmp[1]);
    u16tmp[4] = (volatile)((uint16)u8tmp[1]);
    u16tmp[5] = (volatile)((uint16)u16tmp[1]);
    u16tmp[6] = (volatile)((uint16)u32tmp[1]);
    u16tmp[7] = (volatile)((uint16)u64tmp[1]);
    u16tmp[8] = (volatile)((uint16)ftmp[1]);
    u16tmp[9] = (volatile)((uint16)dtmp[1]);
    u16tmp[10]= (volatile)((uint16)itmp[1]);
    u16tmp[11]= (volatile)((uint16)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void toi32(void)
{
    i32tmp[0] = (volatile)((int32)i8tmp[1]);
    i32tmp[1] = (volatile)((int32)i16tmp[1]);
    i32tmp[2] = (volatile)((int32)i32tmp[1]);
    i32tmp[3] = (volatile)((int32)i64tmp[1]);
    i32tmp[4] = (volatile)((int32)u8tmp[1]);
    i32tmp[5] = (volatile)((int32)u16tmp[1]);
    i32tmp[6] = (volatile)((int32)u32tmp[1]);
    i32tmp[7] = (volatile)((int32)u64tmp[1]);
    i32tmp[8] = (volatile)((int32)ftmp[1]);
    i32tmp[9] = (volatile)((int32)dtmp[1]);
    i32tmp[10]= (volatile)((int32)itmp[1]);
    i32tmp[11]= (volatile)((int32)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void tou32(void)
{
    u32tmp[0] = (volatile)((uint32)i8tmp[1]);
    u32tmp[1] = (volatile)((uint32)i16tmp[1]);
    u32tmp[2] = (volatile)((uint32)i32tmp[1]);
    u32tmp[3] = (volatile)((uint32)i64tmp[1]);
    u32tmp[4] = (volatile)((uint32)u8tmp[1]);
    u32tmp[5] = (volatile)((uint32)u16tmp[1]);
    u32tmp[6] = (volatile)((uint32)u32tmp[1]);
    u32tmp[7] = (volatile)((uint32)u64tmp[1]);
    u32tmp[8] = (volatile)((uint32)ftmp[1]);
    u32tmp[9] = (volatile)((uint32)dtmp[1]);
    u32tmp[10]= (volatile)((uint32)itmp[1]);
    u32tmp[11]= (volatile)((uint32)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void toi64(void)
{
    i64tmp[0] = (volatile)((int64)i8tmp[1]);
    i64tmp[1] = (volatile)((int64)i16tmp[1]);
    i64tmp[2] = (volatile)((int64)i32tmp[1]);
    i64tmp[3] = (volatile)((int64)i64tmp[1]);
    i64tmp[4] = (volatile)((int64)u8tmp[1]);
    i64tmp[5] = (volatile)((int64)u16tmp[1]);
    i64tmp[6] = (volatile)((int64)u32tmp[1]);
    i64tmp[7] = (volatile)((int64)u64tmp[1]);
    i64tmp[8] = (volatile)((int64)ftmp[1]);
    i64tmp[9] = (volatile)((int64)dtmp[1]);
    i64tmp[10]= (volatile)((int64)itmp[1]);
    i64tmp[11]= (volatile)((int64)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void tou64(void)
{
    u64tmp[0] = (volatile)((uint64)i8tmp[1]);
    u64tmp[1] = (volatile)((uint64)i16tmp[1]);
    u64tmp[2] = (volatile)((uint64)i32tmp[1]);
    u64tmp[3] = (volatile)((uint64)i64tmp[1]);
    u64tmp[4] = (volatile)((uint64)u8tmp[1]);
    u64tmp[5] = (volatile)((uint64)u16tmp[1]);
    u64tmp[6] = (volatile)((uint64)u32tmp[1]);
    u64tmp[7] = (volatile)((uint64)u64tmp[1]);
    u64tmp[8] = (volatile)((uint64)ftmp[1]);
    u64tmp[9] = (volatile)((uint64)dtmp[1]);
    u64tmp[10]= (volatile)((uint64)itmp[1]);
    u64tmp[11]= (volatile)((uint64)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void tof(void)
{
    ftmp[0] = (volatile)((float)i8tmp[1]);
    ftmp[1] = (volatile)((float)i16tmp[1]);
    ftmp[2] = (volatile)((float)i32tmp[1]);
    ftmp[3] = (volatile)((float)i64tmp[1]);
    ftmp[4] = (volatile)((float)u8tmp[1]);
    ftmp[5] = (volatile)((float)u16tmp[1]);
    ftmp[6] = (volatile)((float)u32tmp[1]);
    ftmp[7] = (volatile)((float)u64tmp[1]);
    ftmp[8] = (volatile)((float)ftmp[1]);
    ftmp[9] = (volatile)((float)dtmp[1]);
    ftmp[10]= (volatile)((float)itmp[1]);
    ftmp[11]= (volatile)((float)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void tod(void)
{
    dtmp[0] = (volatile)((double)i8tmp[1]);
    dtmp[1] = (volatile)((double)i16tmp[1]);
    dtmp[2] = (volatile)((double)i32tmp[1]);
    dtmp[3] = (volatile)((double)i64tmp[1]);
    dtmp[4] = (volatile)((double)u8tmp[1]);
    dtmp[5] = (volatile)((double)u16tmp[1]);
    dtmp[6] = (volatile)((double)u32tmp[1]);
    dtmp[7] = (volatile)((double)u64tmp[1]);
    dtmp[8] = (volatile)((double)ftmp[1]);
    dtmp[9] = (volatile)((double)dtmp[1]);
    dtmp[10]= (volatile)((double)itmp[1]);
    dtmp[11]= (volatile)((double)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void tosint(void)
{
    itmp[0] = (volatile)((sint)i8tmp[1]);
    itmp[1] = (volatile)((sint)i16tmp[1]);
    itmp[2] = (volatile)((sint)i32tmp[1]);
    itmp[3] = (volatile)((sint)i64tmp[1]);
    itmp[4] = (volatile)((sint)u8tmp[1]);
    itmp[5] = (volatile)((sint)u16tmp[1]);
    itmp[6] = (volatile)((sint)u32tmp[1]);
    itmp[7] = (volatile)((sint)u64tmp[1]);
    itmp[8] = (volatile)((sint)ftmp[1]);
    itmp[9] = (volatile)((sint)dtmp[1]);
    itmp[10]= (volatile)((sint)itmp[1]);
    itmp[11]= (volatile)((sint)utmp[1]);
}

_ATTR_MICROLIB_CODE_
volatile void touint(void)
{
    utmp[0] = (volatile)((uint)i8tmp[1]);
    utmp[1] = (volatile)((uint)i16tmp[1]);
    utmp[2] = (volatile)((uint)i32tmp[1]);
    utmp[3] = (volatile)((uint)i64tmp[1]);
    utmp[4] = (volatile)((uint)u8tmp[1]);
    utmp[5] = (volatile)((uint)u16tmp[1]);
    utmp[6] = (volatile)((uint)u32tmp[1]);
    utmp[7] = (volatile)((uint)u64tmp[1]);
    utmp[8] = (volatile)((uint)ftmp[1]);
    utmp[9] = (volatile)((uint)dtmp[1]);
    utmp[10]= (volatile)((uint)itmp[1]);
    utmp[11]= (volatile)((uint)utmp[1]);
}

//addition.
_ATTR_MICROLIB_CODE_
volatile void toadd(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] +  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] +  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]+ i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]+ u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]+ i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]+ u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]+ i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]+ u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  +   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  +   dtmp[2]);
}
//subtraction.
_ATTR_MICROLIB_CODE_
volatile void tosub(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] -  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] -  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]- i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]- u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]- i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]- u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]- i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]- u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  -   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  -   dtmp[2]);
}
//multiplication.
_ATTR_MICROLIB_CODE_
volatile void tomul(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] *  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] *  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]* i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]* u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]* i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]* u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]* i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]* u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  *   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  *   dtmp[2]);
}
//division.
_ATTR_MICROLIB_CODE_
volatile void todiv(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] /  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] /  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]/ i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]/ u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]/ i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]/ u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]/ i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]/ u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  /   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  /   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void tomod(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] %  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] %  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]% i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]% u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]% i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]% u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]% i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]% u64tmp[2]);
    //ftmp[0]   = (volatile)(ftmp[1]  %   ftmp[2]);
    //dtmp[0]   = (volatile)(dtmp[1]  %   dtmp[2]);
}

//left shift, right shift,and, orr.
_ATTR_MICROLIB_CODE_
volatile void toshl(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] <<  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] <<  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]<< i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]<< u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]<< i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]<< u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]<< i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]<< u64tmp[2]);
//    ftmp[0]   = (volatile)(ftmp[1]  <<   ftmp[2]);
//    dtmp[0]   = (volatile)(dtmp[1]  <<   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toshr(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] >>  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] >>  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]>> i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]>> u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]>> i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]>> u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]>> i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]>> u64tmp[2]);
//    ftmp[0]   = (volatile)(ftmp[1]  >>   ftmp[2]);
//    dtmp[0]   = (volatile)(dtmp[1]  >>   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toand(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] &  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] &  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]& i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]& u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]& i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]& u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]& i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]& u64tmp[2]);
//    ftmp[0]   = (volatile)(ftmp[1]  &   ftmp[2]);
//    dtmp[0]   = (volatile)(dtmp[1]  &   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toor(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] |  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] |  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]| i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]| u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]| i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]| u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]| i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]| u64tmp[2]);
//    ftmp[0]   = (volatile)(ftmp[1]  |   ftmp[2]);
//    dtmp[0]   = (volatile)(dtmp[1]  |   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toland(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] &&  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] &&  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]&& i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]&& u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]&& i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]&& u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]&& i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]&& u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  &&   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  &&   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void tolor(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] ||  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] ||  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]|| i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]|| u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]|| i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]|| u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]|| i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]|| u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  ||   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  ||   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toeor(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] ^  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] ^  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]^ i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]^ u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]^ i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]^ u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]^ i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]^ u64tmp[2]);
//    ftmp[0]   = (volatile)(ftmp[1]  ^   ftmp[2]);
//    dtmp[0]   = (volatile)(dtmp[1]  ^   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toreverse(void)
{
    i8tmp[0]  = (volatile)(~(i8tmp[1] ));
    u8tmp[0]  = (volatile)(~(u8tmp[1] ));
    i16tmp[0] = (volatile)(~(i16tmp[1]));
    u16tmp[0] = (volatile)(~(u16tmp[1]));
    i32tmp[0] = (volatile)(~(i32tmp[1]));
    u32tmp[0] = (volatile)(~(u32tmp[1]));
    i64tmp[0] = (volatile)(~(i64tmp[1]));
    u64tmp[0] = (volatile)(~(u64tmp[1]));
//    ftmp[0]   = (volatile)(~(ftmp[1]  ));
//    dtmp[0]   = (volatile)(~(dtmp[1]  ));
}

// >, >=, <, <=, ==
_ATTR_MICROLIB_CODE_
volatile void togt(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] >  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] >  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]> i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]> u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]> i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]> u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]> i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]> u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  >   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  >   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toge(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] >=  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] >=  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]>= i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]>= u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]>= i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]>= u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]>= i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]>= u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  >=   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  >=   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void tolt(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] <  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] <  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]< i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]< u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]< i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]< u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]< i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]< u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  <   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  <   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void tole(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] <=  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] <=  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]<= i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]<= u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]<= i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]<= u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]<= i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]<= u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  <=   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  <=   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toeq(void)
{
    i8tmp[0]  = (volatile)(i8tmp[1] ==  i8tmp[2]);
    u8tmp[0]  = (volatile)(u8tmp[1] ==  u8tmp[2]);
    i16tmp[0] = (volatile)(i16tmp[1]== i16tmp[2]);
    u16tmp[0] = (volatile)(u16tmp[1]== u16tmp[2]);
    i32tmp[0] = (volatile)(i32tmp[1]== i32tmp[2]);
    u32tmp[0] = (volatile)(u32tmp[1]== u32tmp[2]);
    i64tmp[0] = (volatile)(i64tmp[1]== i64tmp[2]);
    u64tmp[0] = (volatile)(u64tmp[1]== u64tmp[2]);
    ftmp[0]   = (volatile)(ftmp[1]  ==   ftmp[2]);
    dtmp[0]   = (volatile)(dtmp[1]  ==   dtmp[2]);
}

_ATTR_MICROLIB_CODE_
volatile void toabs(void)
{
    u8tmp[0]  = (volatile)(abs(i8tmp[1] ));
    u8tmp[1]  = (volatile)(abs(u8tmp[1] ));
    u16tmp[0] = (volatile)(abs(i16tmp[1]));
    u16tmp[1] = (volatile)(abs(u16tmp[1]));
    u32tmp[0] = (volatile)(abs(i32tmp[1]));
    u32tmp[1] = (volatile)(abs(u32tmp[1]));
    u64tmp[0] = (volatile)(abs(i64tmp[1]));
    u64tmp[1] = (volatile)(abs(u64tmp[1]));
    ftmp[0]   = (volatile)(abs(ftmp[1]  ));
    dtmp[0]   = (volatile)(abs(dtmp[1]  ));
}

//memory
//memset£¬memcpy£¬memmove£¬memcmp,memchr
//
_ATTR_MICROLIB_CODE_
volatile void tomem(void)
{
    memset((uint8*)i8tmp, 16, 16);
    memcpy(i8tmp, u8tmp, 16);
    memcmp(i8tmp, u8tmp, 16);
    memmove(i8tmp, u8tmp, 16);
    memchr(i8tmp, 0);
    
    strlen((char *)i8tmp);
    strcpy((char *)i8tmp, (const char*)u8tmp);
    strncpy((char *)i8tmp, (const char*)u8tmp, 16);
    strxfrm((char *)i8tmp, (const char*)u8tmp, 16);
    strcat((char *)i8tmp, (const char*)u8tmp);
    strncat((char *)i8tmp, (const char*)u8tmp, 16);
    strcmp((char *)i8tmp, (const char*)u8tmp);
    strncmp((char *)i8tmp, (const char*)u8tmp, 16);
    strcoll((const char *)i8tmp, (const char*)u8tmp);
    strchr((char *)i8tmp, 41);
    strrchr((char *)i8tmp, 41);
    strspn((const char *)i8tmp, (const char*)u8tmp);
    strcspn((const char *)i8tmp, (const char*)u8tmp);
    strpbrk((const char *)i8tmp, (const char*)u8tmp);
}
#endif

_ATTR_MICROLIB_CODE_
void microlibuse(void)
{
#if 1
    //type conversion.
    toi8();
    tou8();
    toi16();
    tou16();
    toi32();
    tou32();
    toi64();
    tou64();
    tof();
    tod();
    tosint();
    touint();
    
    //+ - * / %
    toadd();
    tosub();
    tomul();
    todiv();
    tomod();
    
    //>>, <<, or, and,~,^,
    toshr();
    toshl();
    toand();
    toor();
    toland();
    tolor();
    toeor();
    toreverse();
    
    //>,>=,<,<=,==
    togt();
    toge();
    tolt();
    tole();
    toeq();
    
    //mem, str
    tomem();
    
    //abs
    toabs();
#endif
}


/*
********************************************************************************
*
*                         End of nvic.c
*
********************************************************************************
*/
