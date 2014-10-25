/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name��   nFat.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-10-21          1.0
*    desc:    ORG.
********************************************************************************
*/
#define	 IN_NFAT
#include "FsInclude.h"

/***************************************************************************
Description:������
Input      :LBA
Output     :
Callback   :
***************************************************************************/
IRAM_FAT
bool FATReadSector(uint32 LBA, void *pData)
{
    return (bool)MDRead(FileDiskID, LBA, 1, pData);
}

/***************************************************************************
Description:д����
Input      :LBA�����ݻ�����
Output     : null
Callback   :
***************************************************************************/
IRAM_FAT
bool FATWriteSector(uint32 LBA, void *pData)
{
    return (bool)MDWrite(FileDiskID, LBA, 1, pData);
}


/*********************************************************************************************************
** Name	:
** Description	:����FAT��ָ���ص���һ���غ�
** Input	:Index���غ�
** Output      	:��һ���غ�
** global	:FATType, BootSector
** call module	:GetFATPosition, NandFlashReadSector
********************************************************************************************************/
IRAM_FAT
uint32 FATGetNextClus(uint32 Index, uint32 Count)
{
	uint16 ByteIndex;
	uint32 SecIndex;
	uint32 Rt;
 
	if (Index >= BootSector.CountofClusters + 2)
	{
		return (BAD_CLUS);
	}
    
    Rt = Index;
	while (Count-- != 0)
	{
    	/* ���������ź��ֽ����� */
        GetFATPosition(Index, &SecIndex, &ByteIndex);
    	/* ��ȡFAT������ */
    	if (SecIndex != gFatCache.Sec)
    	{
    	    if (gFatCache.Sec!=0 && gFatCache.Flag!=0)
    	    {
    	        gFatCache.Flag=0;
    	        FATWriteSector(gFatCache.Sec, gFatCache.Buf);
	        }
    		gFatCache.Sec = SecIndex;
    		if (OK != FATReadSector(SecIndex, gFatCache.Buf))
    		{
                return (BAD_CLUS);
    		}
    	}
    	
    	switch (FATType)
    	{
            case FAT16:
    			Rt = gFatCache.Buf[ByteIndex>>1];
    			break;
                
    		case FAT32:
    			Rt = gFatCache.Buf[ByteIndex>>1] | ((uint32)gFatCache.Buf[(ByteIndex+2)>>1] << 16);
    			break;
    		default:
    			Rt = BAD_CLUS;
    			break;
       } 
        Index = Rt;
	}
    if (FATType==FAT16 && Rt>=(BAD_CLUS & 0xffff))
	{
		Rt |= 0x0ffful << 16;
	}
	Rt &= 0x0fffffff;
	return (Rt);
}

#ifdef ENCODE
/*****************************************************************************************
** Name	:FATDelClusChain
** Description	:ɾ��ָ������
** Input	:Index���������״غ�
** Output      	: null
** global	:FATType
** call module	:
*****************************************************************************************/
IRAM_ENCODE
void FATDelClusChain(uint32 Index)
{
	uint16 FATEntOffset;
	uint32 ThisClus;
	uint32 NextClus;
	uint32 FATSecNum, pre_FATSecNum;
	uint8  Buf[512];

	if (Index <= EMPTY_CLUS_1 || Index >= BAD_CLUS)
	{
		return;
	}

	ThisClus      = Index;
	pre_FATSecNum = 0;
    
    do
    {
        NextClus = FATGetNextClus(ThisClus, 1);					    //��FAT1�л�ȡ��һ��
			
        GetFATPosition(ThisClus, &FATSecNum, &FATEntOffset);		//��ȡ�ô���FAT���ƫ��
			
        ThisClus = NextClus;
			
        if (pre_FATSecNum != FATSecNum)
        {
            if (pre_FATSecNum != 0)
            {
                FATWriteSector(pre_FATSecNum, Buf);
            }
            pre_FATSecNum = FATSecNum;
            FATReadSector(FATSecNum, Buf);
        }

        switch (FATType)		//���ô���
        {
            case FAT16:
                Buf[FATEntOffset + 0] = EMPTY_CLUS;
                Buf[FATEntOffset + 1] = EMPTY_CLUS;
                break;
                    
            case FAT32:
                Buf[FATEntOffset + 0] = EMPTY_CLUS;
                Buf[FATEntOffset + 1] = EMPTY_CLUS;
                Buf[FATEntOffset + 2] = EMPTY_CLUS;
                Buf[FATEntOffset + 3] = EMPTY_CLUS;
                break;
                    
            default:
                break;
        }
    }while (NextClus > EMPTY_CLUS_1 && NextClus < BAD_CLUS);
        
    FATWriteSector(pre_FATSecNum, Buf);
}


/****************************************************************************************
** Name	:FATSetNextClus
** Description	:������һ����
** Input	:Index���غ�
**        			 Next����һ���غ�
** Output      	: null
** global	:FATType, BootSector
** call module	:GetFATPosition, NandFlashReadSector, NandFlashWriteSector
*****************************************************************************************/
IRAM_ENCODE
void FATSetNextClus(uint32 Index, uint32 Next)
{
	uint16 ByteIndex;
	uint32 SecIndex;
    
	if (Index <= EMPTY_CLUS_1 || Index >= BootSector.CountofClusters + 2)
	{
		return;
	}
    
	/* ���������ź��ֽ����� */
	switch (FATType)
	{
		case FAT16:
		case FAT32:
			GetFATPosition(Index, &SecIndex, &ByteIndex);
			break;
            
		default:
			return;
	}

	if (SecIndex != gFatCache.Sec)
	{
	    if (gFatCache.Sec!=0 && gFatCache.Flag!=0)
	    {
	        gFatCache.Flag=0;
	        FATWriteSector(gFatCache.Sec, gFatCache.Buf);
        }
		gFatCache.Sec = SecIndex;
		FATReadSector(SecIndex, gFatCache.Buf);
	}

	switch (FATType)
	{
	
        case FAT16:
			gFatCache.Buf[ByteIndex>>1] = Next;
			break;
		case FAT32:
			gFatCache.Buf[ByteIndex>>1] = Next & 0xffff;
			gFatCache.Buf[(ByteIndex+2)>>1] = (Next >> 16) & 0xffff;
			break;
		default:
			break;
	}
    gFatCache.Flag=1;
    
}


/****************************************************************************************
** Name	:FATAddClus
** Description	:Ϊָ����������һ����
** Input	:Index������������һ���غ�,���Ϊ0,��Ϊһ����������һ����
** Output      	:���ӵĴغ�
** global	:BootSector
** call module	:
*****************************************************************************************/
IRAM_ENCODE
uint32 FATAddClus(uint32 Index)
{
	uint32 NextClus,ThisClus,MaxClus;

	if (Index >= BAD_CLUS)
	{
		return (BAD_CLUS);
	}
    
	MaxClus = BootSector.CountofClusters+2;

	//���Ҵ��������һ����
	ThisClus = Index;
	if (ThisClus > EMPTY_CLUS_1)
	{
		while (1)
		{
			NextClus = FATGetNextClus(ThisClus, 1);
			if (NextClus >= EOF_CLUS_1 || NextClus <= EMPTY_CLUS_1)
			{
				break;		                                    //���ҵ��մػ������
			}
            
			if (NextClus == BAD_CLUS)
			{
				return (BAD_CLUS);
			}
            
			ThisClus = NextClus;
		}
	}
	else
	{
		ThisClus = EMPTY_CLUS_1;
	}

	//�Ӵ�����β����ʼ����һ���մ�
	for (NextClus = ThisClus + 1; NextClus < MaxClus; NextClus++)
	{
		if (FATGetNextClus(NextClus, 1) == EMPTY_CLUS)
		{
			break;
		}
	}

	//�յ�β��û�յ����ٴ�ͷ����
	if (NextClus >= MaxClus)
	{
		for (NextClus = EMPTY_CLUS_1 + 1; NextClus < ThisClus; NextClus++)
		{
			if (FATGetNextClus(NextClus, 1) == EMPTY_CLUS)
			{
				break;
			}
		}
	}

	//���յ�Ϊ��β������һ����
	if (FATGetNextClus(NextClus, 1) == EMPTY_CLUS)
	{
		if (ThisClus > EMPTY_CLUS_1)
		{
			FATSetNextClus(ThisClus, NextClus);
		}
        
		FATSetNextClus(NextClus, EOF_CLUS_END);
		UpdataFreeMemFast(1 << (LogBytePerSec + LogSecPerClus));
        
		return (NextClus);
	}
	else
	{
		return (BAD_CLUS);
	}
}


/*********************************************************************************************************
** Name	:GetFreeMemFast
** Description	:����ʣ��ռ䣨���٣�
** Input	: null
** Output      	:�ܿ���K�ֽ���
** global	:BootSector
** call module	:
********************************************************************************************************/
IRAM_ENCODE
uint32 GetFreeMemFast(uint8 memory)
{
#if 0
	uint32 TotalFree = 0;

	if (memory < 2 && FreeMem[memory] <= BootSector.CountofClusters)
	{
		TotalFree = FreeMem[memory];
		TotalFree <<= LogSecPerClus;
		TotalFree >>= 1;
	}
	return (TotalFree);
#endif
}
#endif


/*********************************************************************************************************
** Name	:GetTotalMem
** Description	:����ʣ��ռ�
** ��ڲ���	:memory=0ѡ��FLASH, memory=1ѡ��SD��
** Output      	:��K�ֽ���
** global	:
** call module	:
********************************************************************************************************/
IRAM_FAT
uint32 GetTotalMem(uint8 memory)
{
    uint32 MemDevID = GetMemDevID(memory);

	return MDGetCapacity(MemDevID);
}

/*********************************************************************************************************
** Name	:GetFreeMem
** Description	:����ʣ��ռ�
** Input	: null
** Output      	:�ܿ���K�ֽ���
** global	:BootSector
** call module	:
********************************************************************************************************/
IRAM_FAT
uint32 GetFreeMem(uint8 memory)
{
	uint32 TotalFree;
	uint32 Rt;
	uint32 clus;
	uint32 EndClus;
	uint32 ImaskTemp;
	uint32 MemBak;
	
#if (MAX_LUN > 1)
    MemBak = CurMemSel;
    if (memory != CurMemSel)
    {
        ImaskTemp = UserIsrDisable();
    	MemDevSetup(memory);
        //UserIsrEnable(ImaskTemp);
    }
#endif

	TotalFree = 0;
	EndClus   = BootSector.CountofClusters + 2;	

	for (clus = 2; clus < EndClus; clus++)
	{
        //ImaskTemp = UserIsrDisable();
		Rt = FATGetNextClus(clus, 1);
        //UserIsrEnable(ImaskTemp);
        
		if (Rt == EMPTY_CLUS)
		{
			TotalFree++;				//�ܿմ�
			continue;
		}
        
		if (Rt == BAD_CLUS)
		{
		    TotalFree = 0;
            goto out;
		}
	}

	TotalFree <<= LogSecPerClus;        //ת��ΪSector��
	TotalFree >>= 1;                    //ת��ΪKByte
	
out:    
#if (MAX_LUN > 1)
    if (memory != MemBak)
    {
        //ImaskTemp = UserIsrDisable();
    	MemDevSetup(MemBak);
        UserIsrEnable(ImaskTemp);
   }
    
#endif

	return (TotalFree);
}


/*********************************************************************************************************
** Name	:GetFreeMemFast
** Description	:����ʣ��ռ䣨���٣�
** Input	: null
** Output      	:�ܿ���K�ֽ���
** global	:BootSector
** call module	:
********************************************************************************************************/
IRAM_FAT
void UpdataFreeMemFast(int32 size)
{
#if 0
	uint16 ClusOffset;
	uint32 ClusCnt;
	uint32 usize = size > 0 ? size : -size;
    
	if (MediaSel < 2)
	{
		ClusOffset = usize & ((1 << (LogBytePerSec + LogSecPerClus)) - 1);
		ClusCnt    = usize >>  (LogBytePerSec + LogSecPerClus);
        
		if (ClusOffset != 0)
		{
			ClusCnt++;
		}
        
		if (ClusCnt >= FreeMem[MediaSel])
		{
			FreeMem[MediaSel] = 0;
		}
		else
		{
			if(size > 0)
			{
				FreeMem[MediaSel] -= ClusCnt;
			}
			else
			{
				FreeMem[MediaSel] += ClusCnt;
			}
		}
	}
    #endif
}


/*********************************************************************************************************
** Name	:UpdataFreeMem
** Description	:��ɾ���ļ��������ļ������ʣ��ռ�
** Input	:���µ��ֽ���
** Output      	: null
** global	:LogSecPerClus, FATType
** call module	:NandFlashReadSector
** ˵		��	:��������(��ɾ���ļ�)Ϊ'+', ��С����Ϊ(�紴���ļ�)'-'
********************************************************************************************************/
IRAM_FAT
void UpdataFreeMem(int32 size)
{
#if 0
	uint32 TotalFree;
	uint32 BPB_FsInfo;
	uint32 clus;
	uint8  buf[512];
	
	if (FATType == FAT32)
	{
		FATReadSector(BootSector.PBRSector, buf);				        //DBR
		BPB_FsInfo = ((uint16)buf[49] << 8) + buf[48];			            //BPB_FSINFO
		
		FATReadSector(BootSector.PBRSector + BPB_FsInfo, buf);		//FSI_FREE_COUNT
		TotalFree = ((uint16)buf[491] << 8) + buf[490];
		TotalFree <<= 16;
		TotalFree |= ((uint16)buf[489] << 8) + buf[488]; 
        
		if (TotalFree == 0xffffffff)
		{
			TotalFree = GetFreeMem(MediaSel);
			TotalFree <<= 1;
			TotalFree >>= LogSecPerClus;
		}

		if (size < 0)
		{
			clus = (~size) + 1;
		}
		else
		{
			clus = size;
		}
		clus  += ((uint32)1 << (LogSecPerClus + 9)) - 1;
		clus >>= LogSecPerClus + 9;
        
		if (size >= 0)
		{
			TotalFree += clus;
		}
		else
		{
			if (TotalFree >= clus)
			{
				TotalFree -= clus;
			}
			else
			{
				TotalFree = 0;
			}
		}
		buf[488] =(uint8)TotalFree;
		buf[489] =(uint8)(TotalFree >> 8);
		buf[490] =(uint8)(TotalFree >> 16);
		buf[491] =(uint8)(TotalFree >> 24);
        
		FATWriteSector(BootSector.PBRSector+BPB_FsInfo, buf);		//FSI_FREE_COUNT
	}
#endif
}

/*
********************************************************************************
*
*                         End of nFat.c
*
********************************************************************************
*/
