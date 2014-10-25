/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   File.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-10-21          1.0
*    desc:    ORG.
********************************************************************************
*/
#define  IN_FILE
#include "FsInclude.h"

/*********************************************************************************************************
** Name	:StrUprCase
** Description	:小写字母转大写字母 
** Input	:大小写字母混合的文件名
** Output      	:大写字母文件名
** global	: null
** call module	: null
********************************************************************************************************/
IRAM_FAT
void StrUprCase(uint8 *name)
{
#if 0
	while (*name != '\0')
	{
		if (*name >= 'a' && *name <= 'z')
		{
			*name = *name-'a'+'A';
		}
		name++;
	}
#endif	
}

/*********************************************************************************************************
** Name	:FileInit
** Description	:file system initial
** Input	: null
** Output      	: null
** global	: null
** call module	: null
********************************************************************************************************/
IRAM_FAT
void FileInit(void)
{
	HANDLE i;
    
	gFatCache.Sec=0;		//FAT扇区号
	gFatCache.Flag=0;
	FdtCacheSec = -1;		//FDT扇区号
//	FTLInit();			    //变量初始化
	
	for (i = 0; i < MAX_OPEN_FILES; i++)
	{
		FileInfo[i].Flags = 0;
	}
    
	FdtData.DirClus = -1;
}

#ifdef ENCODE
/*********************************************************************************************************
** Name	:FileCreate
** Description	:create file
** Input	:Path, DirFileName:user use filename.
** Output      	:RETURN_OK：成功
** other reference: the return value explaination in file fat.h
** global	:FileInfo
** call module	:AddFDT, GetDirClusIndex
********************************************************************************************************/
IRAM_ENCODE
HANDLE FileCreateSub(uint8 *Path, uint8 *DirFileName, uint8 Attr) // Attr 文件属性，一般为ATTR_ARCHIVE
{
	MY_FILE *fp;
	HANDLE   Rt, OsRt;
	FDT     temp;
	uint8   i;

	OsRt = NOT_OPEN_FILE;

	StrUprCase(Path);
	StrUprCase(DirFileName);

	/* 查找空闲文件登记项 */
	for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
	{
		if (FileInfo[Rt].Flags == 0)
		{
			break;
		}
	}
    
	if (Rt < MAX_OPEN_FILES)
	{
		fp = FileInfo + Rt;		//指向文件句柄
		for (i = 0; i < 11; i++)
		{
			temp.Name[i] = DirFileName[i];
		}

		temp.Attr         = Attr;//ATTR_ARCHIVE;		//存档		
		temp.FileSize     = 0;
		temp.NTRes        = 0;
		temp.CrtTimeTenth = 0;
		temp.CrtTime      = 0;
		temp.CrtDate      = 0;
		temp.LstAccDate   = 0;
		temp.WrtTime      = 0;
		temp.WrtDate      = 0;
		temp.FstClusLO    = 0;
		temp.FstClusHI    = 0;

		fp->DirClus = GetDirClusIndex(Path);
        
		if (fp->DirClus < BAD_CLUS)
		{
//			FTLInit();
			if (AddFDT(fp->DirClus, &temp) == RETURN_OK)        //增加文件
			{
                #if (NAND_DRIVER == 1)
                FtlWrBlkFree();
                #endif
                /* 设置文件信息 */
				for (i = 0; i < 11; i++)
				{
					fp->Name[i] = temp.Name[i];
				}
                
				fp->Flags    = FILE_FLAGS_READ | FILE_FLAGS_WRITE;
				fp->FileSize = 0;
				fp->FstClus  = 0;
				fp->Clus     = 0;
				fp->Offset   = 0;
                
				OsRt = Rt;
			}
		}
	}

	return (OsRt);
}

IRAM_ENCODE
HANDLE FileCreate(uint8 *Path, uint8 *DirFileName)
{
    return (FileCreateSub(Path, DirFileName, ATTR_ARCHIVE)); // Attr 文件属性，一般为ATTR_ARCHIVE
}

/*********************************************************************************************************
** Name	:FileDelete
** Description	:delete file
** Input	:Path, DirFileName:user use filename.
** Output      	:RETURN_OK：成功
** other reference: the return value explaination in file fat.h
** global	:FileInfo
** call module	:FindFDTInfo,FATDelClusChain,DelFDT
********************************************************************************************************/
IRAM_ENCODE
uint8 FileDelete(uint8 *Path, uint8 *DirFileName)
{
	uint32 ClusIndex1;
	uint32 DirClus;
	uint8  Rt;
	FDT    temp;

	StrUprCase(Path);
	StrUprCase(DirFileName);
    
	DirClus = GetDirClusIndex(Path);	//获取路径所在的簇号

    Rt = PATH_NOT_FIND;
    if (DirClus != BAD_CLUS)			//确定路径存在
	{
		Rt = NOT_FIND_FILE;
		if (RETURN_OK == FindFDTInfo(&temp, DirClus, DirFileName))		    //找到目录项
		{
			if ((temp.Attr & ATTR_DIRECTORY) == 0)  		                // 是文件才删除
			{
				Rt = FILE_LOCK;
				if (FindOpenFile(DirClus, DirFileName) >= MAX_OPEN_FILES)	//文件没有打开才删除
				{
//					FTLInit();
                    
					ClusIndex1 = temp.FstClusLO + ((uint32)(temp.FstClusHI) << 16);

                    FATDelClusChain(ClusIndex1);

                    Rt = DelFDT(DirClus, DirFileName);
                    
					UpdataFreeMem(temp.FileSize);
					UpdataFreeMemFast(-(int32)temp.FileSize);
				}
			}
        }
    }

	return (Rt);
}
#endif

/*********************************************************************************************************
** Name	:FileOpen
** Description	:open one file by specified mode
** Input	:Path:路径, DirFileName:user use file name, Type:open type.
** Output      	:Not_Open_FILE is can not open,other is the file handle
** global: FileInfo
** call module: NULL
********************************************************************************************************/
IRAM_FAT
HANDLE FileOpen(uint8 *Path, uint8 *DirFileName, uint8 *Type)
{
	uint8   i;
	MY_FILE *fp;
	HANDLE  Rt, OsRt;
	FDT     FileFDT;
    
	OsRt = NOT_OPEN_FILE;

	StrUprCase(Type);
	StrUprCase(DirFileName);

	/* 查找空闲文件登记项 */
	for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
	{
		if (FileInfo[Rt].Flags == 0)
		{
			break;
		}
	}
    
	if (Rt < MAX_OPEN_FILES)
	{
		fp = FileInfo + Rt;

		fp->DirClus = GetDirClusIndex(Path);
        
		if (fp->DirClus < BAD_CLUS)	//找到路径
		{
			if (RETURN_OK == FindFDTInfo(&FileFDT, fp->DirClus, DirFileName))
			{
				if ((FileFDT.Attr & ATTR_DIRECTORY) == 0)	//是文件
				{
					for (i = 0; i < 11; i++)
					{
						fp->Name[i] = *DirFileName++;
					}

					fp->FileSize  = FileFDT.FileSize;
					fp->FstClus   = FileFDT.FstClusLO | (uint32)FileFDT.FstClusHI << 16;
					fp->Clus      = fp->FstClus;
					fp->Offset    = 0;
					fp->RefClus   = fp->Clus;
					fp->RefOffset = 0;
					{
						{
							fp->Flags = FILE_FLAGS_READ;
							if (Type[0] == 'W' || Type[1] == 'W')
							{
								fp->Flags |= FILE_FLAGS_WRITE;
							}
							OsRt = Rt;
						}
					}
				}
			}
		}
	}	

	return (OsRt);
}

#ifdef LONG_NAME_OPEN

#if 0
IRAM_FAT
void TestFileOpen(void)
{
    HANDLE hFile;
    uint8 name[256];
    memset(name, 0, 256);
    strcpy(name, "U:\\Abc\\abcdefg123456789\\test123456789.mp3");

    hFile = FileOpenLong(name, "R");

    if (hFile != NOT_OPEN_FILE)
    {
        FileClose(hFile);
    }   
}
#endif
/*********************************************************************************************************
** Name	:FileOpen
** Description	:open file by long file name.
** Input	:Path: DirFileName:user use filename, Type:open mode
** Output      	:Not_Open_FILE:cannot open. if not is the file handle.
** global: FileInfo
** call module: NULL
********************************************************************************************************/
IRAM_FAT
HANDLE FileOpenLong(uint8 *FilePath, uint8 *Type)
{
	uint8   i;
	MY_FILE *fp;
	HANDLE  Rt, OsRt;
	FDT     FileFDT;
    uint8 *pFileName;
    uint16 len;
    uint8 ret;
    uint16  lname[260];
    uint8   sname[13];
    
	OsRt = NOT_OPEN_FILE;

	/* 查找空闲文件登记项 */
	for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
	{
		if (FileInfo[Rt].Flags == 0)
		{
			break;
		}
	}
    
	if (Rt < MAX_OPEN_FILES)
	{
		fp = FileInfo + Rt;

        pFileName = strrchr(FilePath, '\\');
        if (pFileName == NULL)
        {
            return (OsRt);
        }
        pFileName++;
        len = pFileName - FilePath;
        
		fp->DirClus = GetDirClusIndexLong(FilePath, len);
        
		if (fp->DirClus < BAD_CLUS)	//找到路径
		{
            len = strlen(pFileName);
            if (FSIsLongName(pFileName, len))
            {
                FSStr2Wstr(lname, pFileName, len);
                ret = FindFDTInfoLong(&FileFDT, fp->DirClus, lname);
            }

            else
            {
                FSRealname(sname, pFileName, 0); 
                ret = FindFDTInfo(&FileFDT, fp->DirClus, sname);               

            }
            
			if (RETURN_OK == ret)
			{
				if ((FileFDT.Attr & ATTR_DIRECTORY) == 0)	//是文件
				{
					for (i = 0; i < 11; i++)
					{
						fp->Name[i] = FileFDT.Name[i];
					}

					fp->FileSize  = FileFDT.FileSize;
					fp->FstClus   = FileFDT.FstClusLO | (uint32)FileFDT.FstClusHI << 16;
					fp->Clus      = fp->FstClus;
					fp->Offset    = 0;
					fp->RefClus   = fp->Clus;
					fp->RefOffset = 0;
					{
						{
							fp->Flags = FILE_FLAGS_READ;
							if (Type[0] == 'W' || Type[1] == 'W')
							{
								fp->Flags |= FILE_FLAGS_WRITE;
							}
							OsRt = Rt;
						}
					}
				}
			}
		}

	}	

	return (OsRt);
}

#endif
/*********************************************************************************************************
** Name	:FileClose
** Description	:close the specified file.
** Input	    :Path, DirFileName:user use filename.
** Output      	:RETURN_OK：success
** other reference: the return value explaination in file fat.h
** global	:FileInfo
** call module	: null
********************************************************************************************************/
IRAM_FAT
uint8 FileClose(HANDLE Handle)
{
	uint8  Rt;
    uint8  ChipSel;
	uint32 TempRow;
    uint16 mod;
	uint32 DestBlock;
	uint32 SrcBlock;
	uint16 len; 
	FDT    FileFDT;
	MY_FILE *fp;
   
	Rt = PARAMETER_ERR;

	if (Handle >= 0 && Handle < MAX_OPEN_FILES)
	{
		Rt = RETURN_OK;
		fp = FileInfo + Handle;

#ifdef ENCODE
		if ((fp->Flags & FILE_FLAGS_WRITE) == FILE_FLAGS_WRITE)
		{
			if (fp->FileSize > 0)
			{
                FATSetNextClus(fp->Clus, EOF_CLUS_END);
                if (gFatCache.Sec!=0 && gFatCache.Flag!=0)
        	    {
        	        gFatCache.Flag=0;
        	        FATWriteSector(gFatCache.Sec, gFatCache.Buf);
    	        }
            }

			Rt = FindFDTInfo(&FileFDT, fp->DirClus, fp->Name);

			if (Rt == RETURN_OK)
			{
				FileFDT.FileSize = fp->FileSize;
				if (FileFDT.FstClusLO == 0 && FileFDT.FstClusHI == 0)	//是新建文件的情况
				{
					FileFDT.FstClusLO = fp->FstClus & 0xffff;
					FileFDT.FstClusHI = (fp->FstClus >> 16) & 0xffff;
				}
                
				ChangeFDT(fp->DirClus, &FileFDT);

				UpdataFreeMem(-fp->FileSize);
				UpdataFreeMemFast(fp->FileSize);
				//ClearEncInfo();
			}
		}
#endif		
		fp->Flags = 0;
	}
  
	return (Rt);
}


/*********************************************************************************************************
** Name	:FileRead
** Description	:read file
** Input	:Buf:the buffer pointer to read data.
**        			 Size: would read number of byte.not bigger than 64k
				 Handle specified file handle.
** Output      	:the real number that had readed.
** global	:FileInfo,BootSector
** call module: NULL
********************************************************************************************************/
IRAM_FAT
uint16 FileRead(uint8 *Buf, uint16 Size, HANDLE Handle)
{
	MY_FILE *fp;
	uint32 SecIndex;
    uint32 SecIndexbak;
	uint16 offsetInSec,offsetInClu;
	uint16 len;
	uint16 count;
    uint16 tmp;
	uint32 ImaskTemp;
	
	fp = FileInfo + Handle;		//指向指定文件

	if (Handle >= 0 && Handle < MAX_OPEN_FILES)
	{
		if (fp->Flags == 0)			// 对应的文件没打开
		{
			goto ReadErr;
		}
        
		if ((Size + fp->Offset) > fp->FileSize) //判断所读的数据是不是超出文件大小
		{
			Size = fp->FileSize  - fp->Offset;
		}
        
		count = Size;

		while (count > 0)	//判文件结束
		{
			if (fp->Clus >= BAD_CLUS)		//增加出错判断
			{
				break;
			}
		
			offsetInSec = fp->Offset &  (BootSector.BPB_BytsPerSec - 1);
			offsetInClu = fp->Offset & ((1 << (LogBytePerSec + LogSecPerClus)) - 1);
			SecIndex    = ((fp->Clus - 2) << LogSecPerClus) + BootSector.FirstDataSector + (offsetInClu >> LogBytePerSec);
			    len         = BootSector.BPB_BytsPerSec - offsetInSec;			//读到该扇区结束
			    
			if (len > count)
			{
				len = count;
			}
            
            ImaskTemp = UserIsrDisable();

			//SecIndex = GetRemap(SecIndex);
			if (offsetInSec != 0 || (len % 512) != 0 || ((uint32)Buf & 0x3))
			{
                uint8 tmp[512]; 
                MDRead(FileDiskID, SecIndex, 1, tmp);
                memcpy(Buf, &tmp[offsetInSec], len);				                    
			}
			else
			{
                MDRead(FileDiskID, SecIndex, 1, Buf);
		    }
            Buf   += len;
			count -= len;
            
			if ((offsetInClu+len) >= (1 << (LogBytePerSec + LogSecPerClus)))
			{
				SecIndex = FATGetNextClus(fp->Clus, 1);
                
				if (SecIndex == BAD_CLUS)
				{
                    UserIsrEnable(ImaskTemp);
                    goto ReadErr;
				}
				else
				{
					fp->Clus = SecIndex;
				}
			}
            
			fp->Offset += len;
            UserIsrEnable(ImaskTemp);
			if (fp->Offset >= fp->FileSize)		//判文件结束
			{
				count = fp->Offset-fp->FileSize;
				fp->Offset = fp->FileSize;
				break;
			}
		}

		return (Size-count);
	}
ReadErr:    
	return (0);
}


#ifdef ENCODE
/*********************************************************************************************************
** Name	:FileWrite
** Description	:write file
** Input	:Buf:the buffer pointer would to write
**  		Size:the size of would write is not bigger than 64k
			Handle point to file handle.
** Output   :real writed byte number.
** global	:FileInfo,BootSector
** call module	: null
********************************************************************************************************/
IRAM_ENCODE
uint16 FileWrite(uint8 *Buf, uint16 Size, HANDLE Handle)
{
	MY_FILE *fp;
	uint32 SecIndex;
	uint16 offsetInSec,offsetInClu;
	uint16 len;
	uint16 count = Size;
	uint16 i;
    uint8 tmp[512]; 

	if ((Size % 512) != 0)
	{
		return (0);
	}

	fp = FileInfo + Handle;					            //指向指定文件

    if (Handle >= 0 && Handle < MAX_OPEN_FILES)
	{
		if ((fp->Flags & FILE_FLAGS_WRITE) == 0)		// 对应的文件是否以写方式打开
		{
			goto WriteErr;
		}
        
		while (count > 0)
		{
			offsetInSec = fp->Offset &  (BootSector.BPB_BytsPerSec - 1);
			offsetInClu = fp->Offset & ((1 << (LogBytePerSec + LogSecPerClus)) - 1);

			if (fp->Offset > fp->FileSize)	            //文件指针超出文件长度
			{
				break;
			}
			
			if (offsetInClu == 0)			            //要新增一个簇
			{
				if (fp->Offset < fp->FileSize)
				{
					if (fp->Offset == 0)		        //回写首簇
					{
						fp->Clus = fp->FstClus;
					}
				}
				else
				{
                    SecIndex = FATAddClus(fp->Clus);
					if (SecIndex >= BAD_CLUS)			//磁盘空间满
					{
						break;
					}
                    
					fp->Clus = SecIndex;
					if (fp->FstClus == EMPTY_CLUS)		//还未分配簇
					{
						fp->FstClus = fp->Clus;
					}
				}
			}
            
			SecIndex = ((fp->Clus - 2) << LogSecPerClus) + BootSector.FirstDataSector + (offsetInClu >> LogBytePerSec);
			len      = BootSector.BPB_BytsPerSec - offsetInSec;		//读到该扇区结束

            if (len > count)
            {
				len = count;
            }

            if ((offsetInClu == 0)	&& (offsetInSec == 0) && (Size == 0x1000))
            {
                len = 0x1000;
                if ((uint32)Buf & 0x3)
                {
                    for (i = 0; i < 8; i++)
                    {
                        memcpy(tmp, Buf + i*512, 512);
                        MDWrite(FileDiskID, SecIndex+i, 1, tmp);
                    }
                }
                else
                {                    
                    MDWrite(FileDiskID, SecIndex, 8, Buf);
                }
            }
			else
			{     
                if ((uint32)Buf & 0x3)
                {
                    memcpy(tmp, Buf, 512);
                    MDWrite(FileDiskID, SecIndex, 1, tmp);
                }
                else
                {           
            	    FATWriteSector(SecIndex, Buf);
                }
			}
            
			count -= len;
			Buf   += len;
			fp->Offset += len;
            
			if (fp->Offset > fp->FileSize)
			{
				fp->FileSize = fp->Offset;
			}
		}
        
		return (Size - count);
	}
WriteErr:	

	return (0);
}
#endif


/*********************************************************************************************************
** Name	:FileEof
** Description	:check whether it is had read/wrote to the end of file.
** Input	: null
** Output      	:0:no,1:yes
** global: FileInfo
** call module: NULL
********************************************************************************************************/
IRAM_FAT
bool FileEof(HANDLE Handle)
{
	bool Rt;

	Rt = TRUE;
    
	if (Handle < MAX_OPEN_FILES)
	{
		if (FileInfo[Handle].Offset < FileInfo[Handle].FileSize)
		{
			Rt = FALSE;
		}
	}

	return (Rt);
}


/*********************************************************************************************************
** Name	:FileSeek
** Description	:move the postion of read/write
** Input	:offset:movement amount
**        		 Whence:move mode
				 SEEK_SET:
				 SEEK_CUR:
				 SEEK_END:
** Output      	: null
** global	:FileInfo
** call module	: null
********************************************************************************************************/
IRAM_FAT
uint8 FileSeek(int32 offset, uint8 Whence, HANDLE Handle)
{
	uint8   Rt;
	uint32  OldClusCnt;
	uint32  NewClusCnt;
	uint32  Clus;
	MY_FILE *fp;
	uint32 ImaskTemp;

    ImaskTemp = UserIsrDisable();
	Rt = PARAMETER_ERR;

	if (Handle >= 0 && Handle < MAX_OPEN_FILES)
	{
		fp = FileInfo + Handle;

        if (fp->Flags  != 0)                            		// 对应的文件是否已打开
		{
			Rt = RETURN_OK;
            
			OldClusCnt = fp->Offset >> (LogSecPerClus + LogBytePerSec);

            switch (Whence)
			{
				case SEEK_END:					 /* 从文件尾计算 */
					fp->Offset = fp->FileSize - offset;
					offset = -offset;
					break;
                    
				case SEEK_SET:
					fp->Offset = offset;			/* 从文件头计算 */
					break;
                    
				case SEEK_CUR:                        	/* 从当前位置计算 */
					fp->Offset += offset;
					break;
                    
				case SEEK_REF:
					fp->Offset += offset;
					if (fp->Offset >= fp->RefOffset)
					{
						OldClusCnt = fp->RefOffset >> (LogSecPerClus + LogBytePerSec);
						fp->Clus = fp->RefClus;
					}
					break;
                    
				default:
					Rt = PARAMETER_ERR;
					break;
                    
			}
            
			if (Rt == RETURN_OK)
			{
				if (fp->Offset > fp->FileSize)
				{
					fp->Offset = (offset > 0) ? fp->FileSize : 0;
				}
                
				/* 改变当前簇号 */
				NewClusCnt = fp->Offset >> (LogSecPerClus + LogBytePerSec);
				if (NewClusCnt >= OldClusCnt)
				{
					NewClusCnt -= OldClusCnt;
					Clus = fp->Clus;
				}
				else
				{
					Clus = fp->FstClus;
				}

                OldClusCnt=FATGetNextClus(Clus, NewClusCnt);
				if (OldClusCnt == BAD_CLUS)
				{
					Rt = FAT_ERR;
				}
				else
				{
					fp->Clus = OldClusCnt;
				}
			}
		}
	}

    UserIsrEnable(ImaskTemp);
	return (Rt);
}

#ifdef ENCODE
/*********************************************************************************************************
** Name	        :FindOpenFile
** Description	:find the file handle that had been opened by specified file
** Input	    :FileName:internal file name.
** Output      	:file handle
** global	    :FileInfo
** call module  :NULL
********************************************************************************************************/
IRAM_ENCODE
HANDLE FindOpenFile(uint32 DirClus, uint8 *FileName)
{
	HANDLE Rt;
	MY_FILE *fp;
	uint8 i;
	
	fp = FileInfo;
	for (Rt = 0; Rt < MAX_OPEN_FILES; Rt++)
	{
		if (fp->Flags != 0)
		{
    		if (fp->DirClus == DirClus)
    		{
        		for (i = 0; i < 11; i++)
        		{
        			if (fp->Name[i] != FileName[i])
        			{
        				break;
        			}
        		}
        		if (i == 11)
        		{
		        	break;
        		}
    		}
		}
		fp++;
	}
    
	return (Rt);
}
#endif

#ifdef FIND_FILE
/*********************************************************************************************************
** Name	:FindFile
** Description	:find the specified index file that is been in current direction or all direction.
**				 FileNum:,Path:,ExtName:, Attr:
** Output      	:Rt:the diretion information of found file items.
** global	    :LongFileName,FileInfo
** call module	:null
** explain		:if the extension is "*",it also will find direction.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint8 FindFileSub(FDT *Rt, uint16 FileNum, uint32 DirClus, uint8 *ExtName, uint32* RefIndex, uint8 Attr)
{
	uint16 i, num;
	uint8  offset;
	uint8  *buf;
	uint32 index;
	uint16 Items;
	FDT    tmp;

	index = *RefIndex;
	num   = 0;
    
	while (1)
	{
		if (RETURN_OK != GetFDTInfo(Rt, DirClus, index++))
		{
			break;
		}
		
		if (Rt->Name[0]==FILE_NOT_EXIST)				//空目录项,后面不再有文件
		{
			break;
		}
		
		if (Rt->Name[0] != FILE_DELETED)
		{
			Items = 0;
			while (Rt->Attr == ATTR_LFN_ENTRY)			//长文件名项要找到短文件名
			{
				GetFDTInfo(Rt, DirClus, index++);
				Items++;	
			}

			if ((Rt->Attr & Attr) == Attr)
			{
                //查找的是相同的文件类型
                if (FileExtNameMatch(&Rt->Name[8], ExtName, Rt->Attr))
				{	
					if (++num == FileNum)
					{
						*RefIndex = index;

                        if (Items != 0)	//长目录项
						{
							index -= Items;
							for (i = 0; i < MAX_FILENAME_LEN; i++)
							{
								LongFileName[i] = '\0';
							}
                            
							while (1)
							{
								GetFDTInfo(&tmp, DirClus, index + Items - 2);
                                
								buf    = (uint8 *)&tmp;
								offset = 13 * ((buf[0] & LFN_SEQ_MASK) - 1);
                                
								if ((buf[0] & LFN_SEQ_MASK) <= MAX_LFN_ENTRIES)   
								{
								    /* 长文件名最多目录项数*/
									for (i = 0; i < 10; i++)
									{
										LongFileName[i / 2 + offset] |= buf[i + 1] << (i % 2) * 8;
									}
                                    
									for (i = 0; i < 6; i++)
									{
										LongFileName[i + 5 + offset] = *(uint16 *)&buf[2*i + 14];
									}
                                    
									for (i = 0; i < 2; i++)
									{
										LongFileName[i + 11 + offset] = *(uint16 *)&buf[2*i + 28];
									}
								}
                                
								if (--Items == 0)
								{
									break;
								}
							}
						}
						else		//短目录项
						{
							for (i = 0; i < 8; i++)
							{
								LongFileName[i] = Rt->Name[i];
								if (LongFileName[i] == ' ')
								{
									break;
								}
							}
							if (Rt->Name[8] != ' ')
							{
								LongFileName[i++] = '.';				//追加扩展名
								LongFileName[i++] = Rt->Name[8];
								LongFileName[i++] = Rt->Name[9];
								LongFileName[i++] = Rt->Name[10];
							}
							LongFileName[i] = '\0';        			//结束标志符unicode码的NUL
						}
						return (RETURN_OK);
					}
				}
			}
		}
	}
	return (NOT_FIND_FILE);
}


/*********************************************************************************************************
** Name	:FindFile
** Description	:find the specified index file that is been in current direction or all direction.
**				 FileNum:,Path:,ExtName:, Attr:
** Output      	:Rt:the diretion information of found file items.
** global	    :LongFileName,FileInfo
** call module	:null
** explain		:if the extension is "*",it also will find direction.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint8 FindFile(FDT *Rt, uint16 FileNum, uint8 *Path, uint8 *ExtName)
{
	uint8  OsRt;
	uint32 DirClus;
	uint32 index;

	DirClus = GetDirClusIndex(Path);
    
	if (DirClus == BAD_CLUS)
	{
		OsRt=PATH_NOT_FIND;
		goto FindErr;
	}

	if ((Path[0] == '\\'  && Path[1] == '\0') || (Path[1] == ':' && Path[2] == '\\' && Path[3] == '\0'))//DirClus == BootSector.BPB_RootClus)		//根目录----全局目录
	{
		GotoRootDir(ExtName);
        
		while (FileNum > SubDirInfo[CurDirDeep].TotalFile)
		{
			FileNum -= SubDirInfo[CurDirDeep].TotalFile;
            
			GotoNextDir(ExtName);

            if (CurDirDeep == 0)
			{
				OsRt=NOT_FIND_FILE;
				goto FindErr;
			}
		}
        
		DirClus = CurDirClus;
	}
    
	index = 0;
	OsRt  = FindFileSub(Rt, FileNum, DirClus, ExtName, &index, 0);
    
FindErr:

	return (OsRt);
}


/*********************************************************************************************************
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint8 FindFileBrowser(FDT *Rt, uint16 FileNum, uint8 *Path, uint8 *ExtName)
{
	uint8  OsRt;
	uint32 DirClus;
	uint32 index;

	DirClus = GetDirClusIndex(Path);
    
	if (DirClus == BAD_CLUS)
	{
		OsRt=PATH_NOT_FIND;
		goto FileBErr;
	}
	index = 0;
	OsRt  = FindFileSub(Rt, FileNum, DirClus, ExtName, &index, 0);

FileBErr:

	return (OsRt);
}

/*********************************************************************************************************
** Name	:FindFirst
** Description	:find the frist file in specified direction.
				FindData:file find structure,Path:ExtName:extension
**				FileNum:,Path:,ExtName:, Attr:
** Output      	:Rt:the diretion information of found file items.
** global	    :LongFileName,FileInfo
** call module	:null
** explain		:if the extension is "*",it also will find direction.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint8 FindFirst(FDT *Rt, FIND_DATA* FindData, uint8 *Path, uint8 *ExtName)
{
	FindData->Clus  = GetDirClusIndex(Path);
	FindData->Index = 0;
	return (FindNext(Rt, FindData, ExtName));
}

/*********************************************************************************************************
** Name	:FindNext
** Description	:find the next file in specified direction.
				FindData:file find structure,Path:ExtName:extension
**				FileNum:,Path:,ExtName:, Attr:
** Output      	:Rt:the diretion information of found file items.
** global	    :LongFileName,FileInfo
** call module	:null
** explain		:if the extension is "*",it also will find direction.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint8 FindNext(FDT *Rt, FIND_DATA* FindData, uint8 *ExtName)
{
	uint8 OsRt;

	if (FindData->Clus >= BAD_CLUS)
	{
		OsRt = PATH_NOT_FIND;
		goto FileBErr;
	}

	OsRt = FindFileSub(Rt, 1, FindData->Clus, ExtName, (uint32*)&FindData->Index, 0);

FileBErr:

	return (OsRt);
}

/*********************************************************************************************************
** Name	:FindFirst
** Description	:find the frist file in specified direction.
				FindData:file find structure,Path:ExtName:extension
**				FileNum:,Path:,ExtName:, Attr:
** Output      	:Rt:the diretion information of found file items.
** global	    :LongFileName,FileInfo
** call module	:null
** explain		:if the extension is "*",it also will find direction.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint8 FindDirFirst(FDT *Rt, FIND_DATA* FindData, uint8 *Path)
{
	FindData->Clus  = GetDirClusIndex(Path);
	FindData->Index =0;
	return (FindDirNext(Rt, FindData));
}

/*********************************************************************************************************
** Name	:FindDirNext
** Description	:find the next file in specified direction.
				FindData:file find structure,Path:ExtName:extension
**				FileNum:,Path:,ExtName:, Attr:
** Output      	:Rt:the diretion information of found file items.
** global	    :LongFileName,FileInfo
** call module	:null
** explain		:if the extension is "*",it also will find direction.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint8 FindDirNext(FDT *Rt, FIND_DATA* FindData)
{
	uint8 OsRt;

	if (FindData->Clus >= BAD_CLUS)
	{
		OsRt = PATH_NOT_FIND;
		goto FileBErr;
	}

	OsRt = FindFileSub(Rt, 1, FindData->Clus, "*", (uint32*)&FindData->Index, ATTR_DIRECTORY);
	if ((Rt->Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == ATTR_VOLUME_ID)	//找到卷标NULL效
	{
		FindDirNext(Rt, FindData);
	}
    
FileBErr:

	return (OsRt);
}


/*********************************************************************************************************
** Name	:FileExtNameMatch
** Description	:filter the file by file extension.
** Input        :SrcExtName source extension,Filter 
** Output       :TRUE match,FALSE no
** global	    :null
** call module	:null
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
bool FileExtNameMatch(uint8 *SrcExtName, uint8 *Filter,uint8 FileAttr)
{
	if (*Filter == '*')
	{
		return (TRUE);
	}

	StrUprCase(Filter);
    
	while (*Filter != '\0')
	{
		if (SrcExtName[0] == Filter[0])
		{
			if (SrcExtName[1] == Filter[1])
			{
				if (SrcExtName[2] == Filter[2])
				{
                    if(FileAttr&ATTR_HIDDEN)
                    {
                        return (FALSE);
                    }
                    else
                    {
                        return (TRUE);
                    }
				}
			}
		}
		Filter += 3;
	}
	return (FALSE);
}


/*********************************************************************************************************
** Name	        :GetTotalFiles
** Description	:get total file number of current direction.
** Input	    :ExtName:file extension
** Output      	:file total
** global	    :FileInfo
** call module	:null
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint16 GetTotalFiles(uint8 *Path, uint8 *ExtName)
{
	uint32 offset;
	uint16 TotalFiles;
	FDT    temp;
	uint32 DirClus;

	offset     = 0; 
	TotalFiles = 0;
    
	DirClus = GetDirClusIndex(Path);
    
	if (DirClus != BAD_CLUS)
	{
		while (1)
		{
			if (RETURN_OK != GetFDTInfo(&temp, DirClus, offset++))
			{
				break;
			}

			if (temp.Name[0] == FILE_NOT_EXIST)
			{
				break;
			}

			if (temp.Name[0] != FILE_DELETED)
			{
				if ((temp.Attr & (ATTR_DIRECTORY | ATTR_VOLUME_ID)) == 0)	//是文件
				{
					while (temp.Attr == ATTR_LFN_ENTRY)		//长文件名
					{
						GetFDTInfo(&temp, DirClus, offset++);
					}
                    
			        if (FileExtNameMatch(&temp.Name[8], ExtName,temp.Attr))
			        {
						TotalFiles++;
			        }
				}
			}
		}
	}

	return (TotalFiles);
}

/*
********************************************************************************
*  Copyright (C),2004-2007, Fuzhou Rockchip Electronics Co.,Ltd.
*  Function name :  GetDirPath()
*  Author:          ZHengYongzhi
*  Description:     get file full path
*                  
*  Input:   pPath           —— store the start address of file path
*  Output:  pPath           —— file path
*  Return:  
*  Calls:   
*
*  History:     <author>         <time>         <version>       
*             ZhengYongzhi     2006/01/01         Ver1.0
*     desc: ORG
********************************************************************************
*/
_ATTR_FAT_FIND_CODE_
void GetLongFileName( int32 DirClus, int32 Index,uint16* lfName )
{
    uint16 i;
    uint16 Item = 1;
    FDT     TempFDT;
    uint16 *buf = lfName;
    uint16 offset;
    
    if(Index == 0)
        return;
        
    Index--;//传进来的Index都是指向下一个文件的
    memset((void*)lfName, 0, (MAX_FILENAME_LEN * 2));
    GetFDTInfo(&TempFDT, DirClus, Index);//读数据，判断是不是长文件名
//------------------------------------------------------------------------------
//先得到短文件名
    for (i=0; i<8; i++)
    {
        if(TempFDT.Name[i] == ' ')
            break;
        if ((TempFDT.NTRes & 0x08) && ((TempFDT.Name[i] >= 'A')&&(TempFDT.Name[i] <= 'Z')))
            *buf= TempFDT.Name[i]+32;
        else
            *buf= TempFDT.Name[i];
        buf++;
    }
    
    if (TempFDT.Name[8] != ' ')
    {
        *buf++ = '.';                //追加扩展名

        for (i=8; i<11; i++)
        {
            if ((TempFDT.NTRes & 0x10) && ((TempFDT.Name[i] >= 'A')&&(TempFDT.Name[i] <= 'Z')))
                *buf= TempFDT.Name[i]+32;
            else
                *buf= TempFDT.Name[i];
            buf++;
        }       
    }
    
//    lfName[i] = '\0';                 //结束标志符unicode码的NUL
//------------------------------------------------------------------------------
//如果有长文件名，获取长文件名
    if(Index == 0)
        return;
        
    while(Item <= MAX_LFN_ENTRIES)
    {
        uint16 * plfName;
        uint8 *buf;
        GetFDTInfo(&TempFDT, DirClus, Index - Item);//读数据，判断是不是长文件名
        if(ATTR_LFN_ENTRY != TempFDT.Attr)
            break;//不是长文件名，退出
            
        buf = (uint8 *)&TempFDT;
        
        if ((buf[0] & LFN_SEQ_MASK) <= MAX_LFN_ENTRIES)
        {
            plfName = lfName + 13 * ((buf[0] & LFN_SEQ_MASK) - 1);
            buf++;
            for(i = 0;i<5;i++)
            {//前面10个是byte
               *plfName = (uint16)*buf++;
               *plfName |= ((uint16)(*buf++))<<8;
               plfName++;
            }
            buf += 3;
            for(i = 0;i<6;i++)
            {
               *plfName = (uint16)*buf++;
               *plfName |= ((uint16)(*buf++))<<8;
                plfName++;
            }
            buf += 2;
            for(i = 0;i<2;i++)
            {
               *plfName = (uint16)*buf++;
               *plfName |= ((uint16)(*buf++))<<8;
                plfName++;
            }
        }

        if((Index - Item) == 0)
            break;
        Item++;
    };
}

/*********************************************************************************************************
** Name	       :GetGlobeFileNum
** Description :get global file index pointer from current file index pointer.
** Input	   :current direction file pointer FileNum, current Path, file type ExtName
** Output      :global file pointer, return 0,if err
** global	   :CurDirDeep, CurDirClus
** call module :GetDirClusIndex, GotoRootDir, GotoNextDir
** explain     :it will enter the direction automatically after call it.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint16 GetGlobeFileNumExt(uint16 FileNum, uint32 DirClus, uint8 *ExtName)
{
	if (DirClus == BAD_CLUS)
	{
		FileNum=0;
	}
	else
	{
		GotoRootDir(ExtName);
		while (DirClus != CurDirClus)		//直到找到当前目录
        {
            FileNum += SubDirInfo[CurDirDeep].TotalFile;
			GotoNextDir(ExtName);
            if (CurDirDeep == 0)
			{
				FileNum=0;
				break;
			}
		}
	}

    return (FileNum);
}

/*
********************************************************************************
*  Copyright (C),2004-2007, Fuzhou Rockchip Electronics Co.,Ltd.
*  Function name :  GetDirPath()
*  Author:          ZHengYongzhi
*  Description:     get file full path
*                  
*  Input:   pPath           ——  store the start address of file address.
*  Output:  pPath           ——  file path.
*  Return:  
*  Calls:   
*
*  History:     <author>         <time>         <version>       
*             ZhengYongzhi     2006/01/01         Ver1.0
*     desc: ORG
********************************************************************************
*/
_ATTR_FAT_FIND_CODE_
void GetLongDirPath(UINT16 *pPath)
{
    UINT16 i, j, k;
    UINT16 TempPath[MAX_FILENAME_LEN];

    *pPath++ = 'U';
    *pPath++ = ':';
    *pPath++ = '\\';
    for (i = 0; i < CurDirDeep; i++)
    {
        GetLongFileName(SubDirInfo[i].DirClus,SubDirInfo[i].Index,TempPath);
        for (j = 0; j < MAX_FILENAME_LEN ; j++) 
        {
            *pPath = TempPath[j];
            if (TempPath[j] == '\0') 
            {
                *pPath++ = '\\';
                break;
            }
            pPath ++;
        }
    }
    if (CurDirDeep != 0) pPath--;
    *pPath= '\0';
}

/*********************************************************************************************************
** Name	        :GetCurFileNum
** Description	:get current direction file index pointer from global file index pointer.
** Input	    :global file pointer FileNum, file type ExtName
** Output      	:the file index pointer of current direction.return 0 if happen err.
** global	    :CurDirDeep
** call module	:GotoRootDir, GotoNextDir
** explain     :it will enter the direction automatically after call it.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint16 GetCurFileNum(uint16 FileNum, uint8 *ExtName)
{
	GotoRootDir(ExtName);
    
	while (FileNum > SubDirInfo[CurDirDeep].TotalFile)
	{
		FileNum -= SubDirInfo[CurDirDeep].TotalFile;
        
		GotoNextDir(ExtName);
		if (CurDirDeep == 0)
		{
			FileNum = 0;
			break;
		}
	}

	return (FileNum);
}


/*********************************************************************************************************
** Name	:GetGlobeFileNum
** Description	:get global file index pointer from current direction file index pointer.
** Input	:current direction file index pointerFileNum, current file direction Path, file type ExtName
** Output      	:global file pointer, return 0 if happen err.
** global	:CurDirDeep, CurDirClus
** call module	:GetDirClusIndex, GotoRootDir, GotoNextDir
** explain     :it will enter the direction automatically after call it.
********************************************************************************************************/
_ATTR_FAT_FIND_CODE_
uint16 GetGlobeFileNum(uint16 FileNum, uint8 *Path, uint8 *ExtName)
{
	uint32 DirClus;

	DirClus=GetDirClusIndex(Path);
    
	if (DirClus == BAD_CLUS)
	{
		FileNum = 0;
	}
	else
	{
		GotoRootDir(ExtName);
        
		while (DirClus != CurDirClus)		//直到找到当前目录
		{
			FileNum += SubDirInfo[CurDirDeep].TotalFile;
            
			GotoNextDir(ExtName);
            
			if (CurDirDeep == 0)
			{
				FileNum=0;
				break;
			}
		}
	}

	return (FileNum);
}
#endif

#ifdef ENCODE

/*********************************************************************************************************
** Name        :VolumeCreate
** Description :build disk lable
** Input       :DirFileName:lable name must be 8.3 format.
** Output      :
** global      :
** call module :
********************************************************************************************************/
IRAM_ENCODE
void VolumeCreate(uint8 *DirFileName)
{
    FDT temp, temp1;
    uint8 i;
    uint8 Result;
    uint32 index;

    for (i = 0; i < 11; i++)
    {
        temp.Name[i] = ' ';
    }
    for (i = 0; i < 11; i++)
    {
        if (DirFileName[i] == '\0')
        break;
        temp.Name[i] = DirFileName[i];
    }
    temp.Attr = ATTR_VOLUME_ID;
    temp.FileSize = 0;
    temp.NTRes = 0;
    temp.CrtTimeTenth = 0;
    temp.CrtTime = 0;
    temp.CrtDate = 0;
    temp.LstAccDate = 0;
    temp.WrtTime = 0;
    temp.WrtDate = 0;
    temp.FstClusLO = 0;
    temp.FstClusHI = 0;
    index=0;
    while (1)
    {
        Result = GetFDTInfo(&temp1, BootSector.BPB_RootClus, index);
        if (Result == FDT_OVER || Result != RETURN_OK)
        {
            break;
        }

        if (temp1.Name[0] == FILE_NOT_EXIST)
        {
            SetFDTInfo(BootSector.BPB_RootClus, index, &temp);
            #if (NAND_DRIVER == 1)
            FtlWrBlkFree();
            #endif
            break;
        }

        if ((temp1.Attr == ATTR_VOLUME_ID) || (temp1.Attr == (ATTR_ARCHIVE | ATTR_VOLUME_ID)))
        {
            for (i=0; i<11; i++)
            {
                if (temp1.Name[i] != temp.Name[i])
                {
                    SetFDTInfo(BootSector.BPB_RootClus, index, &temp);
                    #if (NAND_DRIVER == 1)
                    FtlWrBlkFree();
                    #endif
                    break;
                }
            }
            break;
        }
        index++;
    }
}
#endif

/*
********************************************************************************
*
*                         End of File.c
*
********************************************************************************
*/
