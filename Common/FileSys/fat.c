/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   fat.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-10-21          1.0
*    desc:    ORG.
********************************************************************************
*/

#define  IN_FAT
#include "FsInclude.h"

#define CHECK_MBR

/***************************************************************************
Description:check whether is the power of 2
Input      :
Output     :-1--no,
Callback   :
***************************************************************************/
IRAM_FAT
int lg2(uint32 arg)
{
    uint16 log;

    for(log = 0; log < 32; log++)
    {
        if(arg & 1)
        {
            arg >>= 1;
            return (arg != 0) ? -1 : log;
        }
        arg >>= 1;
    }
    return (-1);
}

#ifdef FAT32FORMAT

/***************************************************************************
Description:advanced format.
Input      :type:FAT type
Output     :NULL
Callback   :
***************************************************************************/
IRAM_IDLE
void Format(uint8 media, uint8 type)
{
    if( type != FAT32)
        return;

    WriteDBRSector(media, type);           //Write DBR
    
    FileSysSetup(media);              //Write FAT

    WriteFAT2(type);
    
    WriteRootDir(type);             //Write Root Dir

    FileInit();

    #if (NAND_DRIVER == 1)
    FtlWrBlkFree();
    #endif

}

/***************************************************************************
Description:write 512 bytes DBR sector.
Input      :type:FAT type
Output     :NULL
Callback   :
***************************************************************************/
IRAM_IDLE
void WriteDBRSector(uint8 media, uint8 type)
{
    uint32 TotLogicSec;
    uint32 DBRAddr;
    uint16 i;
    uint8  DbrBuf[512];
    uint8  FSInfoBuf[512];
    uint32 TmpVal1;
    uint32 TmpVal2;

//write DBR
{
    memset(DbrBuf,0,512);
    
    DbrBuf[0]  = 0xeb;
    DbrBuf[1]  = 0x58;//0x3e
    DbrBuf[2]  = 0x90;
                 
    DbrBuf[3]  = 'M';
    DbrBuf[4]  = 'S';
    DbrBuf[5]  = 'D';
    DbrBuf[6]  = 'O';
    DbrBuf[7]  = 'S';
    DbrBuf[8]  = '5';
    DbrBuf[9]  = '.';
    DbrBuf[10] = '0';

    //2 0x0b-0x23:BPB
    DbrBuf[12] = 0x02;                                          //每扇区512B
    DbrBuf[14] = 0x20;                                          //保留扇区数 0x20
    DbrBuf[16] = 0x02;                                          //FAT份数
                                                                
    DbrBuf[21] = 0xf8;                                          //硬盘
                                                                
//  DbrBuf[22] = 63;                                            //每FAT占用的扇区数
    DbrBuf[24] = 63;                                            //每磁道扇区数
    DbrBuf[26] = 255;                                           //磁头数
                                                                
    DbrBuf[44] = 2;                                             //BPB_RootClus
    DbrBuf[48] = 1;                                             //BPB_FSInfo ,Usually 1.
    DbrBuf[50] = 6;                                             //BPB_BkBootSec , 6.
    DbrBuf[66] = 0x29;                                          //扩展引导标签,必须是0x28或0x29
                                                                
    DbrBuf[82] = 'F';                                           //文件系统类型(8B)
    DbrBuf[83] = 'A';
    DbrBuf[84] = 'T';
    DbrBuf[85] = '3';
    DbrBuf[86] = '2';
    DbrBuf[87] = ' ';
    DbrBuf[88] = ' ';
    DbrBuf[89] = ' ';

    //2 0x01fe-0x01ff:signtrue
    DbrBuf[510] = 0x55;
    DbrBuf[511] = 0xaa;
    
    TotLogicSec = GetTotalMem(media);

    if(TotLogicSec <= 66600)
    {
        DbrBuf[13] = 0;        
    } 
    else if(TotLogicSec <= 532480)
    {
        DbrBuf[13] = 1;  
    }
    else if(TotLogicSec <= 16777216)
    {
        DbrBuf[13] = 8;  
    }
    else if(TotLogicSec <= 33554432)
    {
        DbrBuf[13] = 16;  
    }
    else if(TotLogicSec <= 67108864)
    {
        DbrBuf[13] = 32;  
    }
    else if(TotLogicSec <= 0xFFFFFFFF)
    {
        DbrBuf[13] = 64;  
    }

    if (TotLogicSec > 65535)                                    //NOT 128M(256M~1G)
    {
        DbrBuf[32] = (uint8)((TotLogicSec      ) & 0x00ff);     //大扇区数
        DbrBuf[33] = (uint8)((TotLogicSec >>  8) & 0x00ff); 
        DbrBuf[34] = (uint8)((TotLogicSec >> 16) & 0x00ff);
        DbrBuf[35] = (uint8)((TotLogicSec >> 24));
    }
    else
    {   
        DbrBuf[19] = (uint8)(TotLogicSec & 0x00ff);             //小扇区数
        DbrBuf[20] = (uint8)((TotLogicSec >> 8) & 0x00ff);  
    }

    TmpVal1 = TotLogicSec - DbrBuf[14];                         //总扇区-保留扇区-根目录扇区
    TmpVal2 = TmpVal1 / ((128 * DbrBuf[13]) + 2) + 7;           //1026 
    TmpVal2 &= 0xFFFFFFF8;                                      //保证4KB对齐

    DbrBuf[36] = (uint8)((TmpVal2      ) & 0x00ff);             //FATSz
    DbrBuf[37] = (uint8)((TmpVal2 >>  8) & 0x00ff);  
    DbrBuf[38] = (uint8)((TmpVal2 >> 16) & 0x00ff);
    DbrBuf[39] = (uint8)((TmpVal2 >> 24));

    FATWriteSector(0, DbrBuf);    
    FATWriteSector(6, DbrBuf);
}
    
//WriteFSInfo_32
{
    memset(FSInfoBuf,0,512);

    FSInfoBuf[0]   = 'R';
    FSInfoBuf[1]   = 'R';
    FSInfoBuf[2]   = 'a';
    FSInfoBuf[3]   = 'A';

    FSInfoBuf[484] = 'r';
    FSInfoBuf[485] = 'r';
    FSInfoBuf[486] = 'A';
    FSInfoBuf[487] = 'a';
    FSInfoBuf[488] = 0xFF;
    FSInfoBuf[489] = 0xFF;
    FSInfoBuf[490] = 0xFF;
    FSInfoBuf[491] = 0xFF;
    FSInfoBuf[492] = 02;
    
    FSInfoBuf[510] = 0x55;
    FSInfoBuf[511] = 0xaa;

    FATWriteSector(1, FSInfoBuf);
    FATWriteSector(7, FSInfoBuf);
    
    memset(FSInfoBuf, 0, 510);

    FATWriteSector(2, FSInfoBuf);
    FATWriteSector(8, FSInfoBuf);
}

}


/***************************************************************************
Description:format by itself,used for frist.
Input      :type:FAT type
Output     :NULL
Callback   :
***************************************************************************/
IRAM_IDLE
void WriteRootDir(uint8 type)
{
    uint16 j;
    uint32 RootDirAddr;
    uint32 PBA;
    uint8  DirBuf[512];

    RootDirAddr = (uint32)BootSector.BPB_ResvdSecCnt + (uint32)(BootSector.BPB_NumFATs * BootSector.FATSz) + (uint32)BootSector.PBRSector;//First

    memset(DirBuf, 0, 512);

    for (j = 0; j < BootSector.BPB_SecPerClus; j++)
    {
        FATWriteSector(RootDirAddr + j, DirBuf);
    }
}


/***************************************************************************
Description:write FAT table,use for frist using,
Input      :type:FAT type
Output     :NULL
Callback   :
***************************************************************************/
IRAM_IDLE
void WriteFAT2(uint8 type)
{
    uint16 i;
    uint32 FAT1LBA;
    uint32 FAT1PBA;
    uint32 EraseSectors;
    uint8  Buf[4096];
    uint8  FATFlag = 1;

    uint32 OldPBA;
    uint16 Offset;
    uint16 len;
    uint8 flag;
    
    EraseSectors = BootSector.FATSz;    //总擦除连续逻辑扇区数
    FAT1LBA      = BootSector.BPB_ResvdSecCnt + BootSector.PBRSector;

    for (i = 0; i < EraseSectors; i++)
    {
        memset(Buf,0,512);
        if(FATFlag)
        {
            FATFlag = 0;
            Buf[0] = 0xF8;
            Buf[1] = 0xFF;
            Buf[2] = 0xFF;
            Buf[3] = 0x0F;
            Buf[4] = 0xFF;
            Buf[5] = 0xFF;
            Buf[6] = 0xFF;
            Buf[7] = 0x0F;
            Buf[8] = 0xFF;
            Buf[9] = 0xFF;
            Buf[10] = 0xFF;
            Buf[11] = 0x0F;
        }
        FATWriteSector(i+FAT1LBA, Buf);
    }

}

#endif


/***************************************************************************
Description:check type of file system
Input      :NULL
Output     :OK=file system right,ERROR=file system wrong.
Callback   :
***************************************************************************/
IRAM_FAT
uint32 CheckFileSystem(void)
{
    uint32 PBRSector=0;
    uint8  DbrBuf[512];

    FATReadSector(0, DbrBuf);         //读DBR扇区
    if(ERROR == CheckFatBootSector(DbrBuf)) //not a FAT volume
    {
#ifdef CHECK_MBR
        PBRSector = CheckMbr(DbrBuf);
        if(PBRSector==0 || PBRSector==-1 || PBRSector > MDGetCapacity(FileDiskID))       //not a MBR
        {
            return (ERROR);
        }
        else
        {   
            #if 0
            FATReadSector(PBRSector, DbrBuf);         //读DBR扇区
            if(ERROR == CheckFatBootSector(DbrBuf))
                return (ERROR);
            #endif
            int8 i = 4;
            while(i--)
            {
                FATReadSector(PBRSector, DbrBuf);         //读DBR扇区
                if (OK == CheckFatBootSector(DbrBuf))
                    break;
                PBRSector++;
            }

            if (i<0)
                return (ERROR);
        }
#else
		return (ERROR);
#endif 
    }

    FATType = GetBootInfo(PBRSector, DbrBuf);
  
    return (OK);
}


/***************************************************************************
Description:get parameter of DBR sector.
Input      :the sector that has DBR.
Output     :type of file system.
Callback   :
***************************************************************************/
IRAM_FAT
uint8 GetBootInfo(uint32 PBRSector, uint8 *DbrBuf)
{
    uint8 type;
    uint32 TotSec, DataSec;

    BootSector.BPB_BytsPerSec = ((uint16)DbrBuf[12] << 8) + DbrBuf[11];
    BootSector.BPB_SecPerClus = DbrBuf[13];
    
    LogBytePerSec = lg2(BootSector.BPB_BytsPerSec);
    LogSecPerClus = lg2(BootSector.BPB_SecPerClus);
    
    BootSector.BPB_NumFATs = DbrBuf[16];
    BootSector.BPB_RootEntCnt = ((uint16)DbrBuf[18] << 8) + DbrBuf[17];//the max directory item number, FATA32 it is zero.
    BootSector.FATSz = ((uint32)DbrBuf[23] << 8) + DbrBuf[22];//the sector number each fat table.FAT12 or FAT16 is O in here.
    
    if(BootSector.FATSz == 0)
    {
        BootSector.FATSz = ((uint32)DbrBuf[39] << 8) + DbrBuf[38];
        BootSector.FATSz <<= 16;
        BootSector.FATSz += ((uint32)DbrBuf[37] << 8) + DbrBuf[36];
    }

    TotSec = ((uint32)DbrBuf[20] << 8) + DbrBuf[19];//FAT file system total sector number...
    if(TotSec == 0)
    {
        TotSec=((uint32)DbrBuf[35] << 8) + DbrBuf[34];
        TotSec <<= 16;
        TotSec += ((uint32)DbrBuf[33] << 8) + DbrBuf[32];
    }

    BootSector.BPB_ResvdSecCnt = ((uint16)DbrBuf[15] << 8) + DbrBuf[14];//reserve sector number....
    BootSector.RootDirSectors = ((BootSector.BPB_RootEntCnt * 32) + (BootSector.BPB_BytsPerSec - 1)) >> LogBytePerSec;
    BootSector.PBRSector = PBRSector;//000
    BootSector.FirstDataSector = BootSector.BPB_ResvdSecCnt + (BootSector.BPB_NumFATs * BootSector.FATSz) + 
		                         BootSector.RootDirSectors + PBRSector;

    DataSec = TotSec - BootSector.FirstDataSector + PBRSector;

    while((DataSec & (BootSector.BPB_SecPerClus - 1)) != 0)   //Modify by lxs @2006.01.10 for No Standard Lib compiler
    {
        DataSec--;
    }

    BootSector.CountofClusters = DataSec >> LogSecPerClus;
    if(BootSector.CountofClusters < 4085)
    {
        type = FAT12;       // Volume is FAT12
    }
    else if(BootSector.CountofClusters < 65525)
    {
        type = FAT16;       // Volume is FAT16
    }
    else
    {
        type = FAT32;       // Volume is FAT32
    }


    if(type == FAT32)//the clus number of root drectory....0x00000002==boot dir clus number.
    {
        BootSector.BPB_RootClus=((uint32)DbrBuf[47] << 8) + DbrBuf[46];
        BootSector.BPB_RootClus <<= 16;
        BootSector.BPB_RootClus +=((uint32)DbrBuf[45] <<8 ) + DbrBuf[44];
    }
    else
    {
        BootSector.BPB_RootClus=0;
	}

    BootSector.TotSec=TotSec;       //add by lxs @2005.4.19
    return (type);
}


/***************************************************************************
Description:check whether it is guide sector.(DBR)
Input      :NULL
Output     :ERROR=no,OK=yes
Callback   :
***************************************************************************/
IRAM_FAT
int32 CheckFatBootSector(uint8 *DbrBuf)
{
    uint16 temp;
    int32 bad = OK;

    if(DbrBuf[0] == 0xE9);  // OK
    else if(DbrBuf[0] == 0xEB && DbrBuf[2] == 0x90);    // OK FAT32
    else
    {
        bad = ERROR;        // Missing JMP/NOP
    }

    // check other stuff
    temp = DbrBuf[13];//sector number of one clus. must be the power of 2
    if((lg2(temp)) < 0)
    {
        bad = ERROR;        //Sectors per cluster is not a power of 2
    }

    // very few disks have only 1 FAT, but it's valid
    temp = DbrBuf[16]; //the FAT table number....
    if(temp != 1 && temp != 2)
    {
        bad = ERROR;        // Invalid number of FATs
    }

    // can't check against dev.sects because dev.sects may not yet be set
    temp = (DbrBuf[25]<<8)+DbrBuf[24];//the sector number of each track cylinder..
    if(temp == 0 || temp > 63)
    {
        bad = ERROR;        // Invalid number of sectors
    }

    // can't check against dev.heads because dev.heads may not yet be set
    temp = (DbrBuf[27]<<8)+DbrBuf[26];//magnetic head number...
    if(temp == 0 || temp > 255)
    {
        bad = ERROR;        // Invalid number of heads
    }

    temp = (DbrBuf[511]<<8)+DbrBuf[510];//the standard end charactor 
    if(temp != 0xAA55)
    {
        bad = ERROR;        // Invalid signature of FATs
    }

    return (bad);
}

/***************************************************************************
Description:check whether it is main guide record(MBR)
Input      :NULL
Output     :0--no, other: yes
Callback   :
***************************************************************************/
IRAM_FAT
uint32 CheckMbr(uint8 *DbrBuf)
{
    bool bad = 0;
    uint32 MbrLba;

    MbrLba   = ((uint32)DbrBuf[457] << 8) | DbrBuf[456];
    MbrLba <<= 16;
    MbrLba  |= ((uint32)DbrBuf[455] << 8) | DbrBuf[454];
    
    return (bad? 0: MbrLba);        //DBR sector, [457][456][455][454]
}


/***************************************************************************
Description:get the postion of one cluster in FAT table.
Input      :cluster number.
Output     :sector number and sector offset
Callback   :
***************************************************************************/
IRAM_FAT
void GetFATPosition(uint32 cluster, uint32 *FATSecNum, uint16 *FATEntOffset)
{
    uint32 FATOffset;

    if(FATType == FAT16)
    {
        FATOffset = cluster << 1;
    }
    else if (FATType == FAT32)
    {
        FATOffset = cluster << 2;
    }
#ifdef K_FAT12
    else if(FATType == FAT12)
    {
        FATOffset = (cluster * 3) >> 1;
    }
#endif

    *FATSecNum    = (FATOffset >> LogBytePerSec) + BootSector.BPB_ResvdSecCnt + BootSector.PBRSector;//+BootSector.FATSz;//改为写FAT1
    *FATEntOffset = (uint16)(FATOffset & (BootSector.BPB_BytsPerSec - 1));
}

uint32 GetMemDevID(uint8 memory)
{
    uint32 DevID = UserDisk0ID;
        
    switch (memory)
    {
        case FLASH0:
            DevID = UserDisk0ID;
            break;
        case FLASH1:
            DevID = UserDisk1ID;
            break;
        case CARD:    //现在IC不支持8bit
            DevID = UserDisk2ID;
            break;
        default:
            return DevID;
    }

    if (0 == DevID)
    {
        DevID = UserDisk0ID;
    }

    return DevID;
}
/*
--------------------------------------------------------------------------------
  Function name : FileSysSetup
  Author        : chenfen
  Description   :

    函数描述:媒体设置
    入口参数:memory=0选择FLASH, memory=1选择SD卡
    出口参数:=OK设置成功, ERROR=错误
                  
  Input         : 
  Return        : 无

  History:     <author>         <time>         <version>       
                chenfen      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_FAT
uint32 MemDevSetup(uint8 memory)
{
    uint32 DiskID;

	DiskID = GetMemDevID(memory);
    if (!MDChkValid(DiskID))
    {
        if (MDChkPresent(DiskID))
        {
            if (OK != MDProbe(DiskID))
            {
                DEBUG("MemDevSetup MDProbe Error\n");
                return ERROR;
            }
        }
    }

    CurMemSel = memory;
	FileDiskID = DiskID;
    if (OK != CheckFileSystem())
    {
        DEBUG("MemDevSetup Check FileSystem Error\n");
        return ERROR;
    }

	return OK;
}

/*
--------------------------------------------------------------------------------
  Function name : FileSysSetup
  Author        : chenfen
  Description   :

    函数描述:媒体设置
    入口参数:memory=0选择FLASH, memory=1选择SD卡
    出口参数:=OK设置成功, ERROR=错误
                  
  Input         : 
  Return        : 无

  History:     <author>         <time>         <version>       
                chenfen      2008-1-15          Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_FAT
uint32 FileSysSetup(uint8 memory)
{
    if (ERROR == MemDevSetup(memory))
    {
        return ERROR;
    }

    FileInit();

	return OK;
}

/*
********************************************************************************
*
*                         End of fat.c
*
********************************************************************************
*/

