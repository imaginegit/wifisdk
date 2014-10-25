/*
********************************************************************************
*                   Copyright (c) 2009,ZhengYongzhi
*                         All rights reserved.
*
* File Name£º   dir.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2009-02-06         1.0
*    desc:    ORG.
********************************************************************************
*/
#ifndef _DIR_H
#define _DIR_H

#undef  EXT
#ifdef  IN_DIR
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
//1configurable parameter
#define MAX_DIR_DEPTH       8               //the deepth of direction is 4
#define MAX_PATH_NAME       (2 + (MAX_DIR_DEPTH  - 1) * (11 + 1) + 4)

/*
*-------------------------------------------------------------------------------
*  
*                          Struct Define
*  
*-------------------------------------------------------------------------------
*/
typedef __packed struct
{
    uint16 TotalFile;
    uint16 TotalSubDir;
    uint16 CurDirNum;
    uint8  DirName[11];                         //short direction name take the occupation of 11 charactors.
    uint8  LongDirName[MAX_FILENAME_LEN];       //long direction name.
    uint32 DirClus;    //SubDirInfo[0].DirClus boot diretion
    uint32 Index;      //SubDirInfo[0].Index   the child direction index of boot direction.
} SUB_DIR;

/*
*-------------------------------------------------------------------------------
*  
*                          Variable Define
*  
*-------------------------------------------------------------------------------
*/
DRAM_FAT    EXT     uint8   CurDirDeep;                 //current direction deepth
DRAM_FAT    EXT     SUB_DIR SubDirInfo[MAX_DIR_DEPTH];  //child direction infomation list.

/*
*-------------------------------------------------------------------------------
*  
*                          Function Declaration
*  
*-------------------------------------------------------------------------------
*/
extern uint16 BuildDirInfo(uint8* ExtName);
extern void GotoNextDir(uint8* ExtName);
extern void GotoRootDir(uint8* ExtName);
extern long GetRootDirClus(void);
extern uint32 ChangeDir(uint16 SubDirIndex);
extern uint16 GetTotalSubDir(uint8 *Path);
extern uint32 GetDirClusIndex(uint8 *Path);
extern uint32 GetDirClusIndexLong(uint8 *Path, uint16 len);
extern uint8 MakeDir(uint8 *Path, uint8 *DirFileName);
extern uint8 ClearClus(uint32 Index);

/*
********************************************************************************
*
*                         End of dir.h
*
********************************************************************************
*/
#endif
