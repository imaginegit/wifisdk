/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   FDT.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-10-21          1.0
*    desc:    ORG.
********************************************************************************
*/
#define   IN_FDT
#include "FsInclude.h"

/*********************************************************************************************************
** Name	:ReadFDTInfo
** Description	:读取FDT信息
** Input	:Rt：存储返回信息的指针
**        			 SecIndex：扇区号
**         			 ByteIndex：偏移量
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	: null
** call module	:NandFlashReadSector
********************************************************************************************************/
IRAM_FAT
uint8 ReadFDTInfo(FDT *Rt, uint32 SecIndex, uint16 ByteIndex)
{
	uint8 i;
	uint8 *pRt=(uint8 *)Rt;
	uint8 *Buf;
	uint8  status;
	uint32 ImaskTemp;
    
    ImaskTemp = UserIsrDisable();
    
	status = RETURN_OK;
	if (SecIndex != FdtCacheSec)
	{
		FdtCacheSec = SecIndex;
		if (OK != FATReadSector(SecIndex, FdtBuf))
		{
			status = NOT_FIND_FDT;
			goto exit;
		}
	}

	Buf = FdtBuf + ByteIndex;
    memcpy(pRt, Buf, sizeof(FDT));

    if(Rt->Attr != ATTR_LFN_ENTRY)
    {
        if(Rt->Name[0] == 0xFF && Rt->Name[1] == 0xFF && Rt->Name[2] == 0xFF && Rt->Name[3] == 0xFF)
        {
            Rt->Name[0] = 0;
        }
    }

exit:
    UserIsrEnable(ImaskTemp);   
	return (status);
    
}

#ifdef ENCODE
/*********************************************************************************************************
** Name	:WriteFDTInfo
** Description	:写FDT信息
** Input	:SecIndex：扇区号
**        			 ByteIndex：偏移量
**        			 FDT *FDTData:数据
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	: null
** call module	:NandFlashReadSector,NandFlashWriteSector
********************************************************************************************************/
IRAM_ENCODE
uint8 WriteFDTInfo(FDT *FDTData, uint32 SecIndex, uint16 ByteIndex)
{
	uint8 i;
	uint8 *pRt=(uint8 *)FDTData;
	uint8 *Buf;

	FATReadSector(SecIndex, FdtBuf);
    
	FdtCacheSec = SecIndex;
	Buf         = FdtBuf + ByteIndex;
    memcpy(Buf, pRt, sizeof(FDT));

	if (OK != FATWriteSector(SecIndex, FdtBuf))
	{
		return (NOT_FIND_FDT);
	}
	else		
	{
		return (RETURN_OK);
	}
}
#endif

/*********************************************************************************************************
** Name	:GetRootFDTInfo
** Description	:获取根目录指定文件(目录)信息
** Input	:Rt：存储返回信息的指针
**        			 Index：文件(目录)在FDT中的位置
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	:FATType, BootSector
** call module	:ReadFDTInfo
********************************************************************************************************/
IRAM_FAT
uint8 GetRootFDTInfo(FDT *Rt, uint32 Index)
{
	uint16 ByteIndex;
	uint32 SecIndex;
	uint8  temp;
    
	temp = NOT_FAT_DISK;
	Index = Index << 5;        /* 32:sizeof(FDT) */
	if (FATType == FAT12 || FATType == FAT16)
	{
		temp = FDT_OVER;
		if (Index < ((uint32)BootSector.RootDirSectors << LogBytePerSec))
		{
			ByteIndex = Index & (BootSector.BPB_BytsPerSec - 1);
            SecIndex  = (Index >> LogBytePerSec) + (BootSector.FirstDataSector - BootSector.RootDirSectors);

            temp      = ReadFDTInfo(Rt, SecIndex, ByteIndex);
        }
	}
    
	return (temp);
}

/*********************************************************************************************************
** Name	:GetFDTInfo
** Description	:获取指定目录指定文件(目录)信息
** Input	:Rt：存储返回信息的指针
**        			 ClusIndex：目录首簇号
**        			 Index：文件(目录)在FDT中的位置
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	:FATType, BootSector
** call module	:
********************************************************************************************************/
IRAM_FAT
uint8 GetFDTInfo(FDT *Rt, uint32 ClusIndex, uint32 Index)
{
	uint16 ByteIndex;
	uint16 ClusCnt;
	uint32 SecIndex, i;
	uint32 NextClus;
    uint32 ImaskTemp;

	if (ClusIndex == EMPTY_CLUS)
	{
		if (FATType == FAT32)
		{
			ClusIndex = BootSector.BPB_RootClus;
		}
		else
		{
			return (GetRootFDTInfo(Rt, Index));
		}
	}

	if (FATType == FAT12 || FATType == FAT16 || FATType == FAT32)
	{
		if (ClusIndex != FdtData.DirClus)
		{
			FdtData.DirClus = ClusIndex;	//cache dir clus
			FdtData.CurClus = ClusIndex;
			FdtData.Cnt     = 0;
		}
        
		Index     = Index << 5;//because one SFN/LFN is 32 bytes.dgl
		ByteIndex = Index & (BootSector.BPB_BytsPerSec - 1);
		SecIndex  = Index >> LogBytePerSec;
		ClusCnt   = SecIndex >> LogSecPerClus;
        
		if (ClusCnt < FdtData.Cnt)
		{
			FdtData.Cnt     = 0;
			FdtData.CurClus = ClusIndex;
		}
		else
		{
			SecIndex -= FdtData.Cnt << LogSecPerClus;
		}
        
		/* 计算目录项所在扇区 */
		i = BootSector.BPB_SecPerClus;
		while(SecIndex >= i)
		{
			ImaskTemp = UserIsrDisable();

            FdtData.CurClus = FATGetNextClus(FdtData.CurClus, 1);
			FdtData.Cnt++;
            
            UserIsrEnable(ImaskTemp);
            
			if (FdtData.CurClus <= EMPTY_CLUS_1 || FdtData.CurClus >= BAD_CLUS) 
			{
				return (FDT_OVER);
			}
            
			SecIndex -= i;
		}
		SecIndex = ((FdtData.CurClus - 2) << LogSecPerClus) + SecIndex + BootSector.FirstDataSector;

        return (ReadFDTInfo(Rt, SecIndex, ByteIndex));
	}
    
	return (NOT_FAT_DISK);
}

#ifdef ENCODE
/*********************************************************************************************************
** Name	:SetRootFDTInfo
** Description	:设置根目录指定文件(目录)信息
** Input	:FDTData：要写入的信息
**        			 Index：文件(目录)在FDT中的位置
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	:FATType, BootSector
** call module	:WriteFDTInfo
********************************************************************************************************/
IRAM_ENCODE
uint8 SetRootFDTInfo(uint32 Index, FDT *FDTData)
{
	uint16 ByteIndex;
	uint32 SecIndex;
	uint8 Rt;
    
	Rt    = NOT_FIND_DISK;
	Index = Index << 5;
    
	if (FATType == FAT12 || FATType == FAT16)
	{
		Rt = FDT_OVER;
		if (Index < (BootSector.RootDirSectors << LogBytePerSec))
		{
			ByteIndex = Index & (BootSector.BPB_BytsPerSec - 1);
			SecIndex = (Index >> LogBytePerSec) + (BootSector.FirstDataSector-BootSector.RootDirSectors);

            Rt = WriteFDTInfo(FDTData, SecIndex, ByteIndex);
		}
	}
    
	return (Rt);
}

/*********************************************************************************************************
** Name	:SetFDTInfo
** Description	:设置指定目录指定文件(目录)信息
** Input	:FDTData：要写入的信息
**        			 ClusIndex：目录首簇号
**        			 Index：文件(目录)在FDT中的位置
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	:FATType, BootSector
** call module	:
********************************************************************************************************/
IRAM_ENCODE
uint8 SetFDTInfo(uint32 ClusIndex, uint32 Index, FDT *FDTData)
{
	uint16 ByteIndex;
	uint32 SecIndex;
	uint8 i;
    
	if (ClusIndex == EMPTY_CLUS)
	{
		if (FATType == FAT32)
		{
			ClusIndex = BootSector.BPB_RootClus;
		}
		else
		{
			return (SetRootFDTInfo(Index, FDTData));
		}
	}

	if (FATType == FAT12 ||FATType == FAT16 || FATType == FAT32)
	{
		Index     = Index << 5;
		ByteIndex = Index & (BootSector.BPB_BytsPerSec - 1);
		SecIndex  = Index >> LogBytePerSec;	/* 计算目录项所在偏移扇区 */
		i = BootSector.BPB_SecPerClus;
        
		while(SecIndex >= i)
		{
			ClusIndex = FATGetNextClus(ClusIndex, 1);
            
			if (ClusIndex <= EMPTY_CLUS_1 || ClusIndex >= BAD_CLUS) 
			{
				return (FDT_OVER);
			}
            
			SecIndex -= i;
		}
        
		SecIndex = ((ClusIndex - 2) << LogSecPerClus) + SecIndex + BootSector.FirstDataSector;
        
		return (WriteFDTInfo(FDTData, SecIndex, ByteIndex));
	}
    
	return (NOT_FAT_DISK);
}

#endif //ENCODE

/*********************************************************************************************************
** Name	:FindFDTInfo
** Description	:在指定目录查找指定文件(目录)
** Input	:Rt：存储返回信息的指针
**        			 ClusIndex：目录首簇号
**        			 FileName：文件(目录)名
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	: null
** call module	:GetFDTInfo
********************************************************************************************************/
IRAM_FAT
uint8 FindFDTInfo(FDT *Rt, uint32 ClusIndex, uint8 *FileName)
{
	uint32 i;
	uint8 temp, j;
    
	i = 0;
	if (FileName[0] == FILE_DELETED)
	{
		FileName[0] = ESC_FDT;
	}
    
	while (1)
	{
		temp = GetFDTInfo(Rt, ClusIndex, i);		//返回RETURN_OK\NOT_FAT_DISK\FDT_OVER
		if (temp != RETURN_OK)
		{
			break;
		}
        
		if (Rt->Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}
        
		if ((Rt->Attr & ATTR_VOLUME_ID) == 0)
		{
			for (j = 0; j < 11; j++)
			{
				if (FileName[j] != Rt->Name[j])
				{
					break;
				}
			}

            if (j == 11)
			{
				temp = RETURN_OK;
				break;
			}
		}
        
		i++;
	}
    
	if (FileName[0] == ESC_FDT)
	{
		FileName[0] = FILE_DELETED;
	}
    
	return (temp);
}

#ifdef LONG_NAME_OPEN
/*********************************************************************************************************
** Name	:FindFDTInfo
** Description	:在指定目录查找指定文件(目录)
** Input	:Rt：存储返回信息的指针
**        			 ClusIndex：目录首簇号
**        			 FileName：文件(目录)名
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	: null
** call module	:GetFDTInfo
********************************************************************************************************/
IRAM_FAT
uint8 FindFDTInfoLong(FDT *Rt, uint32 ClusIndex, uint16 *FileName)
{
	uint32 i, index;
	uint8 temp, j;
    uint16 state = 0;
    uint16 *strCur, *plfName;
    uint8 *buf;
    uint16 lfName[13];
  
	i = 0;
	if (FileName[0] == FILE_DELETED)
	{
		FileName[0] = ESC_FDT;
	}
    
	while (1)
	{
		temp = GetFDTInfo(Rt, ClusIndex, i);		//返回RETURN_OK\NOT_FAT_DISK\FDT_OVER
		if (temp != RETURN_OK)
		{
			break;
		}
        
		if (Rt->Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}

        if  (Rt->Name[0] == FILE_DELETED)
        {
            i++;
            continue;
        }

        if  (Rt->Attr != ATTR_LFN_ENTRY)
        {
            i++;
            continue;
        }

        if (state == 0)
        {
            if(Rt->Name[0] & 0x01) //first          //找到长文件名的第1项
            {
                index = i;
                state = 1;
                strCur = FileName;
            }
            else
            {
                i++;
                continue;
            }
        }

        if (state == 1)
        {
            /*获取文件名*/
            buf = (uint8 *)Rt;
            plfName = lfName;
            buf++;
            for(j = 0; j<5; j++)
            {//前面10个是byte
                *plfName = (uint16)*buf++;
                *plfName |= ((uint16)(*buf++))<<8;
                 plfName++;
            }
             buf += 3;
            for(j = 0; j<6;j++)
            {
                *plfName = (uint16)*buf++;
                *plfName |= ((uint16)(*buf++))<<8;
                plfName++;
            }
            buf += 2;
            for(j = 0; j<2; j++)
            {
                *plfName = (uint16)*buf++;
                *plfName |= ((uint16)(*buf++))<<8;
                plfName++;
            }

            j = 13;
			plfName = lfName;
            while (--j && *strCur && *strCur == *plfName)
            {
                strCur++;
                plfName++;
            }

            if (*strCur == L'\0' || (j==0 && strCur[1] == L'\0'))
            {
                GetFDTInfo(Rt, ClusIndex, index+1);         //匹配，在读一次对应的短文件名
                temp = RETURN_OK;
				break;
            }

            if (j == 0)
            {
                strCur++;
                i--;                                        //没比较完, 往前比较
                continue;
            }

            /*不匹配，往后继续查找*/
            state = 0;
            i = index+2;
        }

	}
    
	if (FileName[0] == ESC_FDT)
	{
		FileName[0] = FILE_DELETED;
	}
    
	return (temp);
}
#endif

#ifdef ENCODE
/*********************************************************************************************************
** Name	:AddFDT
** Description	:在指定目录查增加指定文件(目录)
** Input	:ClusIndex：目录首簇号
**        			 FDTData：文件(目录)名
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	:BootSector
** call module	:FindFDTInfo,GetFDTInfo,SetFDTInfo,FATAddClus
********************************************************************************************************/
IRAM_ENCODE
uint8 AddFDT(uint32 ClusIndex, FDT *FDTData)
{
	uint32 i;
	FDT TempFDT;
	uint8 temp;

	if (ClusIndex == EMPTY_CLUS)
	{
		if (FATType == FAT32)
		{
			ClusIndex = BootSector.BPB_RootClus;
		}
	}

	temp = FindFDTInfo(&TempFDT, ClusIndex, FDTData->Name);		//NOT_FIND_FDT\RETURN_OK
	if (temp == RETURN_OK)
	{
		return (FDT_EXISTS);
	}

	if (temp != NOT_FIND_FDT && temp != FDT_OVER)		//NOT_FAT_DISK
	{
		return (temp);
	}

	if (FDTData->Name[0] == FILE_DELETED)
	{
		FDTData->Name[0] = ESC_FDT;
	}

	i = 0;
	temp = RETURN_OK;
	while (temp == RETURN_OK)
	{
		temp = GetFDTInfo(&TempFDT, ClusIndex, i);
		if (temp == RETURN_OK)
		{
			if (TempFDT.Name[0] == FILE_DELETED || TempFDT.Name[0] == FILE_NOT_EXIST)
			{
				temp = SetFDTInfo(ClusIndex, i, FDTData);
				break;
			}
		}
		i++;
	}
	
	if (temp == FDT_OVER && ClusIndex != EMPTY_CLUS)	//当前目录项簇已满,需增加一个簇
	{
		i = FATAddClus(ClusIndex);
		temp = DISK_FULL;
		if (i != BAD_CLUS)
		{
			ClearClus(i);		//2006.11.24 debug by lxs
			temp = SetFDTInfo(i, 0, FDTData);
		}
	}
	
	if (FDTData->Name[0] == ESC_FDT)
	{
		FDTData->Name[0] = FILE_DELETED;
	}
	return (temp);
}

/*********************************************************************************************************
** Name	:DelFDT
** Description	:在指定目录删除指定文件(目录)
** Input	:ClusIndex：目录首簇号
**        			 FileName：文件(目录)名
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	: null
** call module	:GetFDTInfo,SetFDTInfo
********************************************************************************************************/
IRAM_ENCODE
uint8 DelFDT(uint32 ClusIndex, uint8 *FileName)
{
	uint32 i;
	FDT TempFDT;
	uint8 temp, j;
    
	i = 0;
	if (FileName[0] == FILE_DELETED)
	{
		FileName[0] = ESC_FDT;
	}
    
	while (1)
	{
		temp = GetFDTInfo(&TempFDT, ClusIndex, i);
		if (temp != RETURN_OK)
		{
			break;
		}
            
		if (TempFDT.Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}
        
//		if ((TempFDT.Attr & ATTR_VOLUME_ID) == 0)		//卷标不能删除
		{
			for (j = 0; j < 11; j++)
			{
				if (FileName[j] != TempFDT.Name[j])
				{
					break;
				}
			}
            
			if (j == 11)
			{
				do
				{
					TempFDT.Name[0] = FILE_DELETED;
					temp = SetFDTInfo(ClusIndex, i, &TempFDT);
                    
					if (RETURN_OK != GetFDTInfo(&TempFDT, ClusIndex, --i))
					{
						break;
					}
                    
				}while (TempFDT.Attr == ATTR_LFN_ENTRY);			//长文件名项要找到短文件名

				break;
			}
		}

		i++;
	}
    
	if (FileName[0] == ESC_FDT)
	{
		FileName[0] = FILE_DELETED;
	}
    
	return (temp);
}

/*********************************************************************************************************
** Name	:ChangeFDT
** Description	:改变指定目录指定文件（目录）的属性
** Input	:ClusIndex：目录首簇号
**        			 FileName：文件（目录）名
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	: null
** call module	:GetFDTInfo,SetFDTInfo
********************************************************************************************************/
IRAM_ENCODE
uint8 ChangeFDT(uint32 ClusIndex, FDT *FDTData)
{
	uint32 i;
	uint8  temp, j;
	FDT    TempFDT;

	i = 0;
	if (FDTData->Name[0] == FILE_DELETED)
	{
		FDTData->Name[0] = ESC_FDT;
	}
    
	while (1)
	{
		temp = GetFDTInfo(&TempFDT, ClusIndex, i);
		if (temp != RETURN_OK)
		{
			break;
		}
            
		if (TempFDT.Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}
        
		if ((TempFDT.Attr & ATTR_VOLUME_ID) == 0)
		{
			for (j = 0; j < 11; j++)
			{
				if (FDTData->Name[j] != TempFDT.Name[j])
				{
					break;
				}
			}
            
			if (j==11)
			{
				temp = SetFDTInfo(ClusIndex, i, FDTData);
				break;
			}
		}
        
		i++;
	}
    
	if (FDTData->Name[0] == ESC_FDT)
	{
		FDTData->Name[0] = FILE_DELETED;
	}
    
	return (temp);
}

/*********************************************************************************************************
** Name	:FDTIsLie
** Description	:在指定目录查看指定文件(目录)是否存在
** Input	:ClusIndex：目录首簇号
**        			 FileName：文件(目录)名
** Output      	:RETURN_OK：成功
** 其它参考  fat.h中关于返回值的说明
** global	: null
** call module	:GetFDTInfo
********************************************************************************************************/
IRAM_ENCODE
uint8 FDTIsLie(uint32 ClusIndex, uint8 *FileName)
{
	uint32 i;
	uint8  temp, j;
	FDT    temp1;
    
	i = 0;
	if (FileName[0] == FILE_DELETED)
	{
		FileName[0] = ESC_FDT;
	}
    
	while (1)
	{
		temp = GetFDTInfo(&temp1, ClusIndex, i);
		if (temp == FDT_OVER)
		{
			temp = NOT_FIND_FDT;
			break;
		}

		if (temp != RETURN_OK)
		{
			break;
		}

		if (temp1.Name[0] == FILE_NOT_EXIST)
		{
			temp = NOT_FIND_FDT;
			break;
		}
		
		if ((temp1.Attr & ATTR_VOLUME_ID) == 0)
		{
			for (j = 0; j < 11; j++)
			{
				if (FileName[j] != temp1.Name[j])
				{
					break;
				}
			}
            
			if (j == 11)
			{
				temp = FDT_EXISTS;
				break;
			}
		}
        
		i++;
	}
    
	if (FileName[0] == ESC_FDT)
	{
		FileName[0] = FILE_DELETED;
	}
    
	return (temp);
}
#endif

/*
********************************************************************************
*
*                         End of FDT.c
*
********************************************************************************
*/
