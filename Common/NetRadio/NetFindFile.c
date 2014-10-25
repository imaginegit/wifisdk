/*
********************************************************************************
*
*                Copyright (c): 2013 - 2013 + 5, csun
*                             All rights reserved.
*
* FileName: ..\Common\NetRadio\NetFindFile.c
* Owner: csun
* Date: 2013.3.21
* Time: 21:50:24
* Desc: find net file
* Name format:
*
*   1 Control macro name is  _[_..._]XXXX[_XXXX..._XXXX][_..._]_
*
*   2 Data macro/struct/union/enum name is XXXX[_XXXX..._XXXX]
*
*   3 Function /label name is Xxxx[_Xxxx..._Xxxx]
*
*   4 Variable / const name is 
*   (ppst/ppen/ppun/ppb/ppw/ppdw/ppf/ppv/pst/pen/pun/pb/
*        pw/pdw/pf/pv/st/en/un/b/w/dw)Xxxx[Xxxx...Xxxx]
*
* History:
*   <author>    <date>       <time>     <version>     <Desc>
*       csun     2013.3.21     21:51:40   1.0          create
*     
********************************************************************************
*/

#include "sysconfig.h"

/*
*-------------------------------------------------------------------------------
*
*                             #include define                                   
*
*-------------------------------------------------------------------------------
*/

#include "sysinclude.h"
#include "stdio.h"
#include "Audio_File_Access.h"
#include "NetFile.h"
#include "NetFindFile.h"
#include "XmlAnalyze.h"
#include "Http.h"
#include "WifiControl.h"
#include "MemDev.h"

/*
*-------------------------------------------------------------------------------
*
*							   #define / #typedef define     					 
*
*-------------------------------------------------------------------------------
*/


/* bmp anlyse information start..*/
typedef __packed struct tagBITMAPFILEHEADER
{
	uint16 bfType;		// λͼ�ļ������ͣ�����ΪBM
	uint32 bfSize;		// λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ
	uint16 bfReserved1;	// λͼ�ļ������֣�����Ϊ0
	uint16 bfReserved2;	// λͼ�ļ������֣�����Ϊ0
	uint32 bfOffBits;	// λͼ���ݵ���ʼλ�ã��������λͼ�ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ
}BITMAPFILEHEADER;

typedef __packed struct tagBITMAPINFOHEADER
{
	uint32 biSize;			// ���ṹ��ռ���ֽ���
	uint32 biWidth;			// λͼ�Ŀ�ȣ�������Ϊ��λ	
	uint32 biHeight;		// λͼ�ĸ߶ȣ�������Ϊ��λ	
	uint16 biPlanes;		// Ŀ���豸�ļ��𣬱���Ϊ1
	uint16 biBitCount;		// ÿ�����������λ����������1(˫ɫ),4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ
	uint16 biCompression;	// λͼѹ�����ͣ������� 0(��ѹ��),1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ
	uint32 biSizeImage;		// λͼ�Ĵ�С�����ֽ�Ϊ��λ
	uint32 biXPelsPerMeter;	// λͼˮƽ�ֱ��ʣ�ÿ��������
	uint32 biYPelsPerMeter;	// λͼ��ֱ�ֱ��ʣ�ÿ��������
	uint32 biClrUsed;		// λͼʵ��ʹ�õ���ɫ���е���ɫ��
	uint32 biClrImportant;	// λͼ��ʾ��������Ҫ����ɫ��
}BITMAPINFOHEADER;

typedef __packed struct tagRGBQUAD
{
	uint8 rgbBlue;		// ��ɫ������(ֵ��ΧΪ0-255)
	uint8 rgbGreen;		// ��ɫ������(ֵ��ΧΪ0-255)
	uint8 rgbRed;		// ��ɫ������(ֵ��ΧΪ0-255)
	uint8 rgbReserved;	// ����������Ϊ0
}RGBQUAD;

typedef struct tagBITMAPINFO
{
	BITMAPFILEHEADER bmpFHeader;// λͼ�ļ�ͷ
	BITMAPINFOHEADER bmpIHeader;// λͼ��Ϣͷ
	RGBQUAD bmpHeader;			// ��ɫ��
}BITMAPINFO;

/* bmp anlyse information end..*/
#define  BMP_XML_URL_SIZE				512
/*
*-------------------------------------------------------------------------------
*
*							   local variable define     	     				 
*
*-------------------------------------------------------------------------------
*/
uint32 NetTrackInit(uint8 bPlayMode, uint16 *pdwCurID, uint16 * pdwTotleMusic)
{   
	uint8 *pMusicList;
	uint8 *pMusicTotal;
	uint8  bUrl[512];
	MEMDEV_INFO DevInfo;
	int TotalUsePage, page, Num, wTotolNum, fileid;
	UINT16 XmlFinshItemNum;

    if (OK != MDGetInfo(DataDiskID, &DevInfo))
        return ERROR;
	
	ModuleOverlay(MODULE_ID_XML, MODULE_OVERLAY_ALL);  
    ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL); 

	wTotolNum = 186;

	/* first must find first null flash page start index,for wirte..*/
	gSysConfig.XmlSavStartPage = GetSysReservedNextPage(XML_BUF_BLK);
	
	TotalUsePage = (wTotolNum + MAX_XML_NODE_NUM_ONE_PAGE - 1) / MAX_XML_NODE_NUM_ONE_PAGE;
	if ((gSysConfig.XmlSavStartPage + TotalUsePage) > (DevInfo.BlockSize / DevInfo.PageSize))
	{
		gSysConfig.XmlSavStartPage = 0;
	}
	printf("XmlSavStartPage:%d\n", gSysConfig.XmlSavStartPage);

	//strcpy(bUrl, "http://api.xbd61.com/Api2013/GetUserMusicList.php?
	//userId=10526&codeId=27803&page=1&count=186&Apiadmin=xiaobuding");
	wTotolNum = XmlConnectAnalyze(bUrl, gSysConfig.XmlSavStartPage, wTotolNum, wTotolNum/5);
	if (wTotolNum <= 0)
		return ERROR;

	if (gSysConfig.WiFiFileNo >=  wTotolNum)
	{
		gSysConfig.WiFiFileNo = 0;
	}
	else
	{
		Xml_Get_Item((void*)&fileid, 4, XML_UML_ID, gSysConfig.WiFiFileNo);
		if (fileid != gSysConfig.WiFiFileId)
			gSysConfig.WiFiFileNo = 0;
	}

	if (pdwCurID)
		*pdwCurID = gSysConfig.WiFiFileNo;

	if (pdwTotleMusic)
		*pdwTotleMusic = wTotolNum;
	
	return wTotolNum;
}

/*******************************************************************************
** Name: NetTrackDeInit
** Input:void
** Return: void
** Owner:csun
** Date: 2013.3.25
** Time: 22:38:23
*******************************************************************************/
void NetTrackDeInit(void)
{
	int fileid;
	
	Xml_Get_Item((void*)&fileid, 4, XML_UML_ID, gSysConfig.WiFiFileNo);
	gSysConfig.WiFiFileId = fileid;
	ModuleOverlay(MODULE_ID_FLASH_PROG, MODULE_OVERLAY_ALL);
	SaveSysInformation();
	
}

/*******************************************************************************
** Name: Xml_Get_Item
** Input:uint8 * pbDataBuf, uint8 *pbItemName, uint8 bItemID
** Return: uint8
** Owner:
** Date: 2013.3.20
** Time: 11:33:36
*******************************************************************************/
int Xml_Get_Item(void *pDst, int DataBufLen, XMLUMLType infoType, int bItemID)
{
    MEMDEV_INFO DevInfo;
 	XmlUMLNode node;
	uint32 sectorAddr;
	int i = 0;
  
    if (OK != MDGetInfo(DataDiskID, &DevInfo))
        return RETURN_FAIL;

	if ((pDst == NULL) || (DataBufLen < 1))
        return RETURN_FAIL;

	printf("XmlGetItem:%d ", bItemID);
	sectorAddr = (uint32)XML_BUF_BLK * DevInfo.BlockSize + gSysConfig.XmlSavStartPage * DevInfo.PageSize + bItemID;
	MDRead(DataDiskID, sectorAddr, 1, (void *)&node);

	DataBufLen--;
	if (infoType == XML_UML_ID)
	{
		printf("XML_UML_ID\n");
		*((int*)pDst) = node.Guid;
	}
	else if(infoType == XML_UML_NAME)
	{
		uint16 *pStr = (uint16*)pDst;

		printf("XML_UML_NAME\n");
		while ((i < DataBufLen) && (node.Name[i] != 0))
		{
			pStr[i] = node.Name[i];
			i++;
		}
		pStr[i] = 0;
	}
	else if(infoType == XML_UML_URL)
	{
		uint8 *pStr = (uint8*)pDst;

		printf("XML_UML_URL\n");
		while ((i < DataBufLen) && (node.URL[i] != 0))
		{
			pStr[i] = node.URL[i];
			i++;
		}
		pStr[i] = 0;
	}
	else
	{
        return RETURN_FAIL;
	}
    
    return RETURN_OK;
}


/*******************************************************************************
** Name: NetTrackOpen
** Input:void
** Return: uint8
** Owner:csun
** Date: 2013.3.21
** Time: 21:55:04
*******************************************************************************/
uint32 NetTrackOpen(UINT16     wCurFileNum)
{ 
    return RETURN_OK;
}

unsigned long NetFLength(FILE *in)
{
	return Net_FLength(in);
}

unsigned long NetFTell(FILE *in)
{
    return Net_FTell(in); 
}

/*******************************************************************************
** Name: NetFuncInit
** Input:void
** Return: void
** Owner:csun
** Date: 2013.3.21
** Time: 21:58:12
*******************************************************************************/
void NetFuncInit(void)
{
	RKFIO_FLength = Net_FLength;
    RKFIO_FRead = NetFileRead;
	RKFIO_FSeek = NetFileSeek;
	RKFIO_FTell = Net_FTell;
	RKFIO_FClose = NetFileClose;
    RKFIO_FLength = NetFLength;
}


/*******************************************************************************
** Name: NetGetFileName
** Input:uint16 * pwFileName, uint32 index
** Return: uint32
** Owner:csun
** Date: 2013.4.28
** Time: 11:56:15
*******************************************************************************/
uint32 NetGetFileName(uint16 * pwFileName, uint32 index)
{
	return 0;
}

