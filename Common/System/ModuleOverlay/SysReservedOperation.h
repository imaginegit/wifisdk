/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   SysReservedOperation.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _SYSRESERVED_OP_H_
#define _SYSRESERVED_OP_H_

#undef  EXT
#ifdef _IN_SYSRESERVED_OP_
#define EXT
#else
#define EXT extern
#endif

/*
*-------------------------------------------------------------------------------
*  
*                           Macro define
*  
*-------------------------------------------------------------------------------
*/

/*reserved area data storage,bit definition logic address LBA*/
#define SYSDATA_BLK			0

#define BOOKMARK_BLK		1

#define USERFILE_BLK    1

#define WIFI_AP_PW_BLK		2

#define XML_BUF_BLK		 	4

#define MEDIA_BLK			4


//section define
#define _ATTR_SYSRESERVED_OP_CODE_         __attribute__((section("SysReservedCode")))
#define _ATTR_SYSRESERVED_OP_DATA_         __attribute__((section("SysReservedData")))
#define _ATTR_SYSRESERVED_OP_BSS_          __attribute__((section("SysReservedBss"),zero_init))

/*
*-------------------------------------------------------------------------------
*  
*                           Struct Address
*  
*-------------------------------------------------------------------------------
*/

/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/
//
EXT UINT32  sysFreeMemeryFlash0;    //  by hj
EXT UINT32  sysFreeMemeryFlash1;    //  by hj
EXT UINT32  cardTotalMem;            //  by hj
EXT UINT32  sysFreeMemeryCard;      //  by hj
EXT UINT32  flash0TotalMem;         //  by hj
EXT UINT32  flash1TotalMem;         //  by hj

/*
*-------------------------------------------------------------------------------
*
*                            Function Declaration
*
*-------------------------------------------------------------------------------
*/
extern void LoadSysInformation(void);
extern void SaveSysInformation(void);
extern void ReadReservedData(uint32 LBA , uint8 *Buf, int16 Length);
extern void WriteReservedData(uint32 LBA , uint8 *Buf, uint16 Length);

extern void FlashFreeMemory(void);
extern void SDCardInfoUpdate(void);

/*
********************************************************************************
*
*                         End of SysReservedOperation.h
*
********************************************************************************
*/
#endif
