/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   Dir.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-10-21          1.0
*    desc:    ORG.
********************************************************************************
*/
#define  IN_DIR

#include "FsInclude.h"


#ifdef FIND_FILE
/*********************************************************************************************************
** Name        :BuildDirInfo
** Description :create direction information
** Input       :expand name related file.
** Output      :file number
** global      :SubDirInfo[],CurDirDeep,CurDirClus
** call module : null
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint16 BuildDirInfo(uint8* ExtName)
{
    uint16 TotalFiles = 0;
    
    GotoRootDir(ExtName);
    while (1)
    {
        TotalFiles += SubDirInfo[CurDirDeep].TotalFile;
        
        GotoNextDir(ExtName);           //traverse all child direction.

        if (CurDirDeep == 0)//check find finish....
        {
            break;
        }
    }
    
    return (TotalFiles);
}


/*********************************************************************************************************
** Name        : GotoNextDir
** Description : turn to next direction
** Input       : extension related files.
** Output      : null
** global      : SubDirInfo[],CurDirDeep,CurDirClus
** call module : null
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
void GotoNextDir(uint8* ExtName)
{
    if (SubDirInfo[CurDirDeep].TotalSubDir == 0)    //该目录下没有目录即为叶结点,要找父目录或同级目录
    {
        while (1)
        {
            if (CurDirDeep == 0)                    //找到根目录了不能再往上找
            {
                return;
            }

            CurDirClus = ChangeDir(2);          //获取上一级目录开始簇号(..目录)
            CurDirDeep--;                       //即指向上一级目录
            if(SubDirInfo[CurDirDeep].CurDirNum < SubDirInfo[CurDirDeep].TotalSubDir)
            {
                SubDirInfo[CurDirDeep].CurDirNum++;
                CurDirDeep++;
                CurDirClus = ChangeDir(SubDirInfo[CurDirDeep-1].CurDirNum);
                break;
            }
        }
    }
    else            //该目录下还有子目录,要找它的第一个子目录
    {
        if (CurDirDeep == 0)
        {
            SubDirInfo[CurDirDeep].CurDirNum = 1;                     //根目录的第一个目录号是1
        }
        else
        {
            SubDirInfo[CurDirDeep].CurDirNum = 3;                     //子目录的第一个目录号是3
        }

        if (++CurDirDeep < MAX_DIR_DEPTH - 1)
        {
            CurDirClus = ChangeDir(SubDirInfo[CurDirDeep - 1].CurDirNum);
        }
        else
        {
            CurDirDeep = MAX_DIR_DEPTH - 1; //边界限制
            CurDirClus = ChangeDir(SubDirInfo[CurDirDeep-1].CurDirNum);
        }
    }

    SubDirInfo[CurDirDeep].TotalFile = GetTotalFiles(".", ExtName);   //add by lxs @2005.02.24
    
    if (CurDirDeep < MAX_DIR_DEPTH-1)
    {
        SubDirInfo[CurDirDeep].TotalSubDir = GetTotalSubDir(".");     //获取该目录下的子目录数
    }
    else
    {
        SubDirInfo[CurDirDeep].TotalSubDir = 0;                       //标记为叶结点
    }
    
    if (SubDirInfo[CurDirDeep].TotalSubDir == 2 && CurDirDeep > 0)    //子目录下有2个子目录"."和".."
    {
        SubDirInfo[CurDirDeep].TotalSubDir = 0;
    }
    
    if (SubDirInfo[CurDirDeep].TotalFile == 0)                      //若没有文件再查找下一个目录
    {
        GotoNextDir(ExtName);
    }
}

/*********************************************************************************************************
** Name        : GotoRootDir
** Description : goto boot direction.
** Input       : extension related file
** Output      : null
** global      : SubDirInfo[],CurDirDeep,CurDirClus
** call module : null
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
void GotoRootDir(uint8* ExtName)
{
    CurDirDeep = 0;                       //direction deepth.
    CurDirClus = BootSector.BPB_RootClus; //direction cluster boot direction.
    
    SubDirInfo[CurDirDeep].TotalSubDir = GetTotalSubDir(".");
    SubDirInfo[CurDirDeep].TotalFile   = GetTotalFiles(".", ExtName);
    SubDirInfo[CurDirDeep].DirClus = CurDirClus;
    SubDirInfo[CurDirDeep].Index = 0;
}

/*********************************************************************************************************
** Name :GetRootDirClus
** Description :
** Input   :
** Output         : null
** call module : null
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
long GetRootDirClus(void)
{
    return(BootSector.BPB_RootClus);
}

/*********************************************************************************************************
** Name 		:ChangeDir
** Description 	:change direction,turn to child direction point by index of current direction.
** Input   		:SubDirIndex
** Output    	:cluster number of index child direction.
** global 		:CurDirClus
** call module 	: null
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint32 ChangeDir(uint16 SubDirIndex)
{
    FDT Rt;
    uint32 cluster;
    uint32 index;
    uint8 *buf;
    uint8 offset;
    uint8 i;

    index=0;
    cluster=0;
    while (1)
    {
        if (RETURN_OK != GetFDTInfo(&Rt, CurDirClus, index++))
        {
            break;
        }

        if (Rt.Name[0] == 0x00)         //null mean end
        {
            break;
        }
        
        if (Rt.Name[0] == FILE_DELETED) //had deletec
        {
            continue;
        }
        
        for (i=0; i<MAX_FILENAME_LEN; i++)
        {
            SubDirInfo[CurDirDeep].LongDirName[i] = '\0';
        }

        while (Rt.Attr == ATTR_LFN_ENTRY)         //长文件名项要找到短文件名
        {
            buf    = (uint8 *)&Rt;
            offset = 13 * ((buf[0] & LFN_SEQ_MASK) - 1);
            
            if ((buf[0] & LFN_SEQ_MASK) <= MAX_LFN_ENTRIES)   
            {/* 长文件名最多目录项数*/
                for (i = 0; i < 10; i++)
                {
                    SubDirInfo[CurDirDeep].LongDirName[i / 2 + offset] |= buf[i + 1] << (i % 2)*8;
                }
                
                for (i = 0; i < 6; i++)
                {
                    SubDirInfo[CurDirDeep].LongDirName[i + 5 + offset] = buf[i + 14];
                }
                
                for (i = 0; i < 2; i++)
                {
                    SubDirInfo[CurDirDeep].LongDirName[i + 11 + offset] = buf[i + 21];
                }
            }
            
            GetFDTInfo(&Rt, CurDirClus, index++);
            
        }
        
        if ((Rt.Attr & ATTR_DIRECTORY) == ATTR_DIRECTORY)
        {
            if (--SubDirIndex == 0)
            {
                cluster   = Rt.FstClusHI;
                cluster <<= 16;
                cluster  |= Rt.FstClusLO;
                
                for (i = 0; i < 11; i++)
                {
                    SubDirInfo[CurDirDeep].DirName[i]=Rt.Name[i];
                }
                
                if (SubDirInfo[CurDirDeep].LongDirName[0] == '\0')
                {
                    for (i = 0; i < 8; i++)
                    {
                        SubDirInfo[CurDirDeep].LongDirName[i] = Rt.Name[i];
                        if (SubDirInfo[CurDirDeep].LongDirName[i] == ' ')
                        {
                            break;
                        }
                    }
                    if (Rt.Name[8] != ' ')
                    {
                        SubDirInfo[CurDirDeep].LongDirName[i++] = '.';              //追加扩展名
                        SubDirInfo[CurDirDeep].LongDirName[i++] = Rt.Name[8];
                        SubDirInfo[CurDirDeep].LongDirName[i++] = Rt.Name[9];
                        SubDirInfo[CurDirDeep].LongDirName[i++] = Rt.Name[10];
                    }
                    SubDirInfo[CurDirDeep].LongDirName[i] = '\0';
                }
                break;
            }
        }
    }

    SubDirInfo[CurDirDeep-1].DirClus = CurDirClus;
    SubDirInfo[CurDirDeep].DirClus = cluster;
    SubDirInfo[CurDirDeep-1].Index = index;   
    return (cluster);
}


/*********************************************************************************************************
** Name        :GetTotalSubDir
** Description :get the total number of child direction.
** Input       :Path
** Output      :child direction number
** global      :CurDirClus
** call module : null
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint16 GetTotalSubDir(uint8 *Path)
{
    FDT Rt;
    uint32 index;
    uint32 DirClus;
    uint16 TotSubDir=0;

    DirClus = GetDirClusIndex(Path);
    
    if (DirClus == BAD_CLUS)
    {
        return (0);
    }
    
    for (index = 0; ; index++)
    {
        if (RETURN_OK != GetFDTInfo(&Rt, DirClus, index))
        {
            break;
        }
        
        if (Rt.Name[0] == FILE_NOT_EXIST)   //找到空的意为结束
        {
            break;
        }
        
        if (Rt.Name[0] == FILE_DELETED)     //已删除
        {
            continue;
        }
        
        if ((Rt.Attr & ATTR_DIRECTORY) == ATTR_DIRECTORY)
        {
            TotSubDir++;
        }
    }
    return (TotSubDir);
}
#endif

/*********************************************************************************************************
** Name        : GetDirClusIndex
** Description : get start cluster number of sepecified direction.
** Input       : Path:(no include file name)
** Output      : start cluster number.
** global      : CurDirClus
** call module : FindFDTInfo
** explain     : '\' can not be the path end,suppot 1~11 path name.
********************************************************************************************************/
IRAM_FAT
uint32 GetDirClusIndex(uint8 *Path)
{
    uint8  i;
    uint32 DirClusIndex;
    FDT    temp;
    uint8  PathName[12];
    
    DirClusIndex = BAD_CLUS;
    
    if (Path != NULL)       //null pointer
    {
//***********************************************************************
//支持盘符如A:
//***********************************************************************
        StrUprCase(Path);
        if (Path[1] == ':')
        {
            Path += 2;
        }
        
        DirClusIndex = BootSector.BPB_RootClus; //根目录
//***********************************************************************
//A:TEMP、TEMP和.\TEMP都是指当前目录下的TEMP子目录
        if (Path[0] != '\\')            //* 不是目录分隔符号,表明起点是当前路径
        {
            DirClusIndex = CurDirClus;
        }
        else
        {
            Path++;
        }
        
        if (Path[0] == '.')             // '\.'表明起点是当前路径
        {
            DirClusIndex = CurDirClus;
            if (Path[1] == '\0' || Path[1] == '\\')     //case "." or ".\"
            {
                Path++;
            }
        }

        while (Path[0] != '\0')
        {
            if (Path[0] == ' ')         //首个字符不允许为空格
            {
                DirClusIndex = BAD_CLUS;
                break;
            }

            for (i = 0; i < 11; i++)        //目录项填空格
            {
                PathName[i] = ' ';
            }
            
            for (i = 0; i < 12; i++)
            {
                if (*Path == '\0')      //到路径结束
                {
                    break;
                }
                
                PathName[i] = *Path++;
            }
            
            if (FindFDTInfo(&temp, DirClusIndex, PathName) != RETURN_OK)    //获取FDT信息
            {
                DirClusIndex = BAD_CLUS;
                break;
            }

            if ((temp.Attr & ATTR_DIRECTORY) == 0)  //FDT是否是目录
            {
                DirClusIndex = BAD_CLUS;
                break;
            }
            
            DirClusIndex = ((uint32)(temp.FstClusHI) << 16) + temp.FstClusLO;
        }
    }

    return (DirClusIndex);
    
}

#ifdef LONG_NAME_OPEN

IRAM_FAT
int  FSStr2Wstr(uint16 *wstr,     uint8 *str, uint32 n)
{
    uint16 *s;
    int len = n;
    for (s = wstr; (0 < n) && (*str != '\0');--n)
    {
        *s++ = *str++;  /* copy at most n chars from s2[] */
    }
        
    *s++ = L'\0';
        
    return len;
}

/*
IRAM_FAT
int  FSCheckWchar(uint16 *s1, uint32 n)
{
	while (--n && *s1 && *s1 < 0x007F) 
     s1++;

    if(n != 0 && *s1)
		return 1;

	return 0;
}
*/

IRAM_FAT
void FSRealname(uint8 *dname, uint8 *pDirName, bool bDir)
{
    uint8 i;
    uint8 *pdname = dname;
    uint16 len;
        
    for (i = 0; i < 11; i++)        //目录项填空格
    {
        dname[i] = ' ';
    }
    
    for (i = 0; i < 13; i++)   // 11 + '.' + '\0' = 13
    {
        if (*pDirName == '\0' || *pDirName == '\\' )      //到路径结束, 
        {
            break;
        }

        if(*pDirName == '.') //文件名要去掉后缀名的.
		{
            pDirName++;
            pdname = &dname[8];
			continue;
        }
        *pdname++ = *pDirName++;
    }

    for (i = 0; i < 11; i++)
	{
		if (dname[i] >= 'a' && dname[i] <= 'z')
		{
			dname[i] = dname[i]-'a'+'A';
		}
	}
}


IRAM_FAT
uint8 FSIsLongName(uint8 *dname, uint16 len)
{
    uint16  i = len;
    uint8 *p = dname;
    uint16 dot = 0;

    if (len > 12)
    {
        return 1;
    }

    while(i--)
    {
        if ( (*p > 127) 
            ||(*p == '+')
            ||(*p == '=')
            ||(*p == ',')
            ||(*p == ';')
            ||(*p == '[')
            ||(*p == ']')
            ||(*p == ' ')) //有"+ = , ; [ ]"，也是长文件名
        {
            return 1;
        }

        if (*p == '.')
        {
            dot++;  
            if (dot > 1) /* 小数点多于1个，是长文件名 */
            {
                return 1;
            }
            
            if ((len-i-1) > 8)  /* 文件主名超过8，是长文件名 */      
            {
                return 1;
            }

            if (i > 3)   /* 扩展名超过3，是长文件名 */
            {
                return 1;
            }
            /* 有大小写字符混合，也是长文件名，8.3的可以转成大写通过短目录项找到*/                                
        }
        p++;
    }

    if ((len == 12) && (dot == 0))
    {
        return 1;
    }

    return 0;
}

/*********************************************************************************************************
********************************************************************************************************/
IRAM_FAT
uint32 GetDirClusIndexLong(uint8 *Path, uint16 len)
{
    uint16  i;
    uint32 DirClusIndex;
    FDT    temp;
    uint16  LName[260];
    uint8   SName[13];
    uint8 *start;
    uint8 ret;

    DirClusIndex = BAD_CLUS;
    
    if (Path != NULL)       //null pointer
    {
//***********************************************************************
//支持盘符如A:
//***********************************************************************
        if (Path[1] == ':')
        {
            Path += 2;
            len -= 2;
        }
        
        DirClusIndex = BootSector.BPB_RootClus; //根目录
//***********************************************************************
//A:TEMP、TEMP和.\TEMP都是指当前目录下的TEMP子目录
        if (Path[0] != '\\')            //* 不是目录分隔符号,表明起点是当前路径
        {
            DirClusIndex = CurDirClus;
        }
        else
        {
            Path++;
            len--;
        }
        
        if (Path[0] == '.')             // '\.'表明起点是当前路径
        {
            DirClusIndex = CurDirClus;
            if (Path[1] == '\0' || Path[1] == '\\')     //case "." or ".\"
            {
                Path++;
                len--;
            }
        }

        while (len > 0)
        {
            if (Path[0] == ' ')         //首个字符不允许为空格
            {
                DirClusIndex = BAD_CLUS;
                break;
            }

            start = Path;
            for (i = 1; i < 256; i++)
            {
                Path++;
                len--;
                if (*Path == '\\')                      
                {                    
                    Path++;
                    len--;
                    break;
                }
            }

            if( FSIsLongName(start, i))
            {
                FSStr2Wstr(LName, start, i);
                ret = FindFDTInfoLong(&temp, DirClusIndex, LName);
            }
            else
            {
                FSRealname(SName, start, 1);     
                ret = FindFDTInfo(&temp, DirClusIndex, SName);
            }
            if (ret != RETURN_OK)    //获取FDT信息
            {
                DirClusIndex = BAD_CLUS;
                break;
            }

            
            if ((temp.Attr & ATTR_DIRECTORY) == 0)  //FDT是否是目录
            {
                DirClusIndex = BAD_CLUS;
                break;
            }
            
            DirClusIndex = ((uint32)(temp.FstClusHI) << 16) + temp.FstClusLO;
        }
    }

    return (DirClusIndex);
    
}

#endif

#ifdef ENCODE
/*********************************************************************************************************
** Name        :ClearClus
** Description :clear the specified cluster to zero.
** Input       :Path
** Output      :RETURN_OK：success
** other reference:the return value explain in file fat.h中关于返回值的说明
** global :FileInfo
** call module :AddFDT, GetDirClusIndex
********************************************************************************************************/
IRAM_ENCODE
uint8 ClearClus(uint32 Index)
{
    uint16 i;
    uint32 SecIndex;
    uint8  buf[512];

    memset(buf, 0, 512);
    
    if (Index < (BootSector.CountofClusters + 2))
    {
        SecIndex = ((Index - 2) << LogSecPerClus) + BootSector.FirstDataSector;
        
        for (i = 0; i < BootSector.BPB_SecPerClus; i++)
        {
            FATWriteSector(SecIndex++, buf);
        }
        return (RETURN_OK);
    }
    else
    {
        return (CLUSTER_NOT_IN_DISK);
    }

}

/*********************************************************************************************************
** Name        :MakeDir
** Description :create direction
** Input       :Path: DirFileName the path name use the 8.3 format.
** Output      :RETURN_OK：成功
** other reference the return value explain in file fat.h
** global :FileInfo
** call module :AddFDT, GetDirClusIndex
********************************************************************************************************/
IRAM_ENCODE
uint8 MakeDir(uint8 *Path, uint8 *DirFileName)
{
    uint8 Rt;
    uint8 i;
    uint32 ClusIndex, Temp1;
    FDT temp;
    
    StrUprCase(Path);
    ClusIndex = GetDirClusIndex(Path);
    
    Rt = PATH_NOT_FIND;
    if (ClusIndex != BAD_CLUS)
    {
        for (i = 0; i < 11; i++)        //目录项填空格
        {
            temp.Name[i] = ' ';
        }
        
        for (i = 0; i < 11; i++)
        {
            if (*DirFileName == '\0')   //到路径结束
            {
                break;
            }
            
            temp.Name[i] = *DirFileName++;
        }

        /* FDT是否存在 */
        Rt = FDTIsLie(ClusIndex, temp.Name);
        if (Rt == NOT_FIND_FDT)
        {
            /* 不存在 */
            Temp1 = FATAddClus(0);                  /* 获取目录所需磁盘空间 */
            Rt    = DISK_FULL;                      /* 没有空闲空间 */

            if ((Temp1 > EMPTY_CLUS_1) && (Temp1 < BAD_CLUS))
            {
                ClearClus(Temp1);                       /* 清空簇 */

                /* 设置FDT属性 */
                temp.Attr         = ATTR_DIRECTORY;
                temp.FileSize     = 0;
                temp.NTRes        = 0;
                temp.CrtTimeTenth = 0;
                temp.CrtTime      = 0;
                temp.CrtDate      = 0;
                temp.LstAccDate   = 0;
                temp.WrtTime      = 0;
                temp.WrtDate      = 0;
                temp.FstClusLO    = Temp1 & 0xffff;
                temp.FstClusHI    = Temp1 / 0x10000;

                Rt = AddFDT(ClusIndex, &temp);       /* 增加目录项 */
                
                if (Rt == RETURN_OK)
                {
                    /* 建立'.'目录 */
                    temp.Name[0] = '.';
                    for (i = 1; i < 11; i++)
                    {
                        temp.Name[i] = ' ';
                    }
                    AddFDT(Temp1, &temp);

                    /* 建立'..'目录 */
                    temp.Name[1] = '.';
                    if (ClusIndex == BootSector.BPB_RootClus)
                    {
                        ClusIndex = 0;
                    }
                    
                    temp.FstClusLO = ClusIndex & 0xffff;
                    temp.FstClusHI = ClusIndex / 0x10000;
                    Rt = AddFDT(Temp1, &temp);
                }
                else
                {
                    FATDelClusChain(Temp1);
                }
                
                Rt = RETURN_OK;
            }
        }
    }
    
    ///////////////////////////////////////////////////////////
    //回写FAT Cache
    if (gFatCache.Sec!=0 && gFatCache.Flag!=0)
    {
        gFatCache.Flag=0;
        FATWriteSector(gFatCache.Sec, gFatCache.Buf);
    }
    ///////////////////////////////////////////////////////////
    
    return (Rt);
}
#endif
/*
********************************************************************************
*
*                         End of Dir.c
*
********************************************************************************
*/

