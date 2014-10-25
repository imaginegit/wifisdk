/*
********************************************************************************
*
*                Copyright (c): 2013 - 2013 + 5, csun
*                             All rights reserved.
*
* FileName: ..\Common\Xml\XmlAnalyze.c
* Owner: csun
* Date: 2013.3.14
* Time: 19:46:28
* Desc: Xml Analyse
* Name format:
*
*   1、Control macro name is  _[_..._]XXXX[_XXXX..._XXXX][_..._]_
*
*   2、Data macro/struct/union/enum name is XXXX[_XXXX..._XXXX]
*
*   3、Function /label name is Xxxx[_Xxxx..._Xxxx]
*
*   4、Variable / const name is 
*   (ppst/ppen/ppun/ppb/ppw/ppdw/ppf/ppv/pst/pen/pun/pb/
*        pw/pdw/pf/pv/st/en/un/b/w/dw)Xxxx[Xxxx...Xxxx]
*
* History:
*   <author>    <date>       <time>     <version>     <Desc>
*      csun     2013.3.14     19:46:56   1.0          create
*   
********************************************************************************
*/
#include "sysconfig.h"
#include "sysinclude.h"
#include "XmlAnalyze.h"
#include "FsInclude.h"
#include "tools.h"
#include "Http.h"


/*
*-------------------------------------------------------------------------------
*
*							   #define / #typedef define     					 
*
*-------------------------------------------------------------------------------
*/

#define NETDATASIZE			2048
#define XMLNODESIZE			1500


_ATTR_XML_ANALYZE_BSS_ static uint8  netData[NETDATASIZE+1];
_ATTR_XML_ANALYZE_BSS_ static uint8  utf8NodeData[XMLNODESIZE+1];
_ATTR_XML_ANALYZE_BSS_ uint8 XML_DATA[FLASHPAGESIZE];


/** 
  * 该函数用于将utf8数组转为Unicode格式! 
  * 目前该函数返回值为：转换后unicode数据占用的wchar_t的个数（切记不是总char的个数） ！ 
  *     @param out   转换结果存放区域指针 
  *     @param outsize   转换结果存放区域大小 
  *     @param in   源串存放区域指针 
  *     @param insize   源串存放区域大小 
  *     @return QINT 转换结果在目的串中的长度，转换失败则返回-1 
* */   
_ATTR_XML_ANALYZE_CODE_  
static int Utf82Unicode(uint8* in, int insize, uint16* out, int outsize)
{
    char *p = in;  
	char* tmp = (char *)out; 
	int resultsize = 0;
	uint16 t1, t2, t3;
	uint8 t4, t5;

	while(*p && (insize > 0))  
    {  
        if (*p >= 0x00 && *p <= 0x7f)//说明最高位为'0'，这意味着utf8编码只有1个字节！  
        {  
            *tmp = *p;  
            tmp++;  
            *tmp = 0;  
            tmp++;  
            resultsize += 1;  
			insize -= 1;
        }  
        else if ((*p & 0xe0)== 0xc0)//只保留最高三位，看最高三位是不是110，如果是则意味着utf8编码有2个字节！  
        {  
            t4 = 0;  
            t5 = 0;  

            t4 = *p & (0x1f);//高位的后5位！（去除了头部的110这个标志位）  
            p++;  
            t5 = *p & (0x3f);//低位的后6位！（去除了头部的10这个标志位）  

            *tmp = t5 | ((t4 & (0x03)) << 6);  
            tmp++;  
            *tmp = t4 >> 2;//留下其保留的三位  
            tmp++;  
            resultsize += 1;
			insize -= 2;
        }  
        else if ((*p & (0xf0))== 0xe0)//只保留最高四位，看最高三位是不是1110，如果是则意味着utf8编码有3个字节！  
        {  
            t1 = 0;  
            t2 = 0;  
            t3 = 0;  
			
            t1 = *p & (0x1f);  
            p++;  
            t2 = *p & (0x3f);  
            p++;  
            t3 = *p & (0x3f);  

            *tmp = ((t2 & (0x03)) << 6) | t3;  
            tmp++;  
            *tmp = (t1 << 4) | (t2 >> 2);  
            tmp++;  
            resultsize += 1;  
			insize -= 3;
        }  

		if (resultsize >= outsize)
		{
			printf("resultsize:%d>=outsize:%d\n", resultsize, outsize);
			return -1;
		}
		
        p++;  
    }

	/* add end 0*/
	out[resultsize] = 0;

	return resultsize;
}


_ATTR_XML_ANALYZE_CODE_  
static void XmlFilterUTF8Space(uint16 *utfs, int len)
{
	int i, j;

	if ((utfs == NULL) || (len < 5))
		return;
	
	i = 0;
	j = 0;
	while (i < len)
	{
		/* find the utf-8 space "&#32;" */
		if ((i + 4 < len) && (utfs[i+0]=='&') && (utfs[i+1]=='#') 
			&& (utfs[i+2]=='3') && (utfs[i+3]=='2') && (utfs[i+4]==';'))
		{
			utfs[j++] = ' ';
			i += 5;
		}
		else
		{
			utfs[j++] = utfs[i++];
		}
	}
    utfs[j] = 0;
}


_ATTR_XML_ANALYZE_CODE_  
static int XmlGetUTF8NodeConnect(const uint8 *label, int labelLen, uint8 *inData, 
								int inLen, uint8 *outData, int outLen, int *lableEndPos)
{
	uint8 labS[32];
	uint8 labE[32];
	int cLen;
	uint8 * pbufS;
	uint8 * pbufE;

	if (labelLen > 30)// 2 == '/0'
		return -1;

	if ((inData == NULL) || (inLen < (labelLen+labelLen+1)) || (outData == NULL) || (outLen < 1))
		return -1;

	memcpy(labS, label, labelLen);
	labS[labelLen] = 0;
	
	memcpy(labE+1, label, labelLen);
	labE[0] = '<';
	labE[1] = '/';
	labE[1+labelLen] = 0;
	
	pbufS = strstri(inData, labS);
	if (pbufS == NULL)
		return -1;
	
	pbufE = strstri(inData, labE);
	if (pbufE == NULL)
		return -1;
	
	pbufS += labelLen;
	cLen = (int)pbufE - (int)pbufS;
	if (cLen < 1)
		return -1;
	if (cLen > outLen)
		return -1;

	memcpy(outData, pbufS, cLen);
	outData[cLen] = 0;

	if (lableEndPos)
	{
		*lableEndPos = (int)(pbufE + labelLen + 1) - (int)inData;
	}
		
	return cLen;// 1 is character '/'
}


_ATTR_XML_ANALYZE_CODE_  
int XmlAnalyzeLableInt(const uint8 *label, int labelLen, uint8 *inData, int inLen, int *out)
{
	uint8 buf[17];
	int result;

	result = XmlGetUTF8NodeConnect(label, labelLen, inData, inLen, buf, 16, NULL);
	if (result < 0)
		return -1;

	result = atoi(buf);

	//printf("%s:%d\n", label, result);

	if (out != NULL)
		*out = result;

	return 0;
}


_ATTR_XML_ANALYZE_CODE_  
int XmlAnalyzeLableCharstr(const uint8 *label, int labelLen, uint8 *inData, int inLen, uint8 *oStr, int oLen)
{
	int result;

	result = XmlGetUTF8NodeConnect(label, labelLen, inData, inLen, oStr, oLen, NULL);
	if (result < 0)
		return -1;
	//printf("%s:%s\n", label, oStr);
	
	return 0;
}


_ATTR_XML_ANALYZE_CODE_  
int XmlAnalyzeLableUnicodestr(const uint8 *label, int labelLen, uint8 *inData, int inLen, uint16 *oStr, int oLen)
{
	uint16 uStr[256];
	uint8 *pcStr;
	int i, result, len;

	pcStr = (uint8*)&uStr[128];
	result = XmlGetUTF8NodeConnect(label, labelLen, inData, inLen, pcStr, 255, NULL);
	if (result < 0)
		return -1;
	
	result = Utf82Unicode(pcStr, 255, uStr, 255);
	if (result < 0)
		return -1;

	XmlFilterUTF8Space(uStr, result);

	for (i = 0; i < oLen; i++)
	{
		if (uStr[i] == 0)
			break;
		oStr[i] = uStr[i];
	}
	oStr[i] = 0;
	

	#if 0
	{
		int i = 0;
		printf("%s:", label);
		
		while (oStr[i])
		{
			printf("%04x ", oStr[i]);
			i++;
		}
		printf("\n");
	}
	#endif

	return 0;
}

//http://api.xbd61.com/Api2013/GetMusicList.php?userId=13638&codeId=36761&page=1&count=57&Apiadmin=xiaobuding
_ATTR_XML_ANALYZE_CODE_  
int XmlConnectAnalyze(uint8 *pXmlUrl, uint32 StartPage, int nodeNum, int timout)
{
	MEMDEV_INFO DevInfo;
	int i, nNum, VailDataLen, len, noSavNodeN, totalNodeN, ret, nxtLablePs;
	BOOLEAN bNetDataEnd;
	uint32 nextXmlBufAddr;
	XmlUMLNode *pNode;
	uint8 buffer[256];
	HANDLE hFileHandle;
	uint32 XmlBufAddr;

    if (OK != MDGetInfo(DataDiskID, &DevInfo))
        return ERROR;

	if ((DevInfo.PageSize << 9) > FLASHPAGESIZE)
        return ERROR;

	hFileHandle = FileOpen("\\", "STORYURLXML", "R");
    if (hFileHandle == NOT_OPEN_FILE)
    {
		printf("STORYURLXML open err\n");
		return ERROR;
	}

	
	/* get the frist save node information flash address unit is sector.512B*/
	XmlBufAddr = (uint32)XML_BUF_BLK * DevInfo.BlockSize;
	nextXmlBufAddr = XmlBufAddr+ StartPage * DevInfo.PageSize;

	pNode = (XmlUMLNode *)XML_DATA;
	bNetDataEnd = FALSE;
	VailDataLen = 0;
	noSavNodeN = 0;
	totalNodeN = 0;
	
	for (nNum = 0; nNum < nodeNum; nNum++)
	{
		/* step 1: get enough UTF-8 data for one xml node. */
		printf("node%03d", nNum);
		while ((bNetDataEnd == FALSE) && (VailDataLen < NETDATASIZE))
		{
			len = FileRead((uint8 *)netData+VailDataLen, NETDATASIZE-VailDataLen, hFileHandle);
			VailDataLen += len;
			netData[VailDataLen] = 0;
			if (len < NETDATASIZE-VailDataLen)
			{
				bNetDataEnd = TRUE;
				FileClose(hFileHandle);
				hFileHandle = -1;
			}
		}
		
		/* step 2: cut one xml node utf-8 data. */
		printf("L:%d", VailDataLen);
		ret = XmlGetUTF8NodeConnect("<TBL_MusicListD>", 16, netData, NETDATASIZE, utf8NodeData, XMLNODESIZE, &nxtLablePs);
		if (ret < 0)
			goto XMLNODEANALYZEEND;
		
		/* step 3: update net data pool. */
		VailDataLen = NETDATASIZE - nxtLablePs;
		for (i = 0; i < VailDataLen; i++)
		{
			netData[i] = netData[nxtLablePs + i];
		}
		netData[i] = 0;

		/* step 4: get node <UML_GUID> connection. */
		printf("node%d", nNum);
		pNode->Guid = 0;
		if (XmlAnalyzeLableInt("<UML_GUID>", 10, utf8NodeData, XMLNODESIZE, &pNode->Guid))
			goto XMLNODEANALYZEEND;
			
		/* step 5: get node <UML_NAME> connection. */
		printf("id:%d\n", pNode->Guid);
		pNode->Name[0] = 0;
		if (XmlAnalyzeLableUnicodestr("<UML_NAME>", 10, utf8NodeData, XMLNODESIZE, pNode->Name, UMLNAMEMAXLEN-1))
		{
			goto XMLNODEANALYZEEND;
		}

		/* step 6: get node <UML_URL> connection. */
		if (XmlAnalyzeLableCharstr("<UML_URL>", 9, utf8NodeData, XMLNODESIZE, pNode->URL, UMLURLMAXLEN-1))
		{
			goto XMLNODEANALYZEEND;
		}

		/* step 7: get node <UML_IMG> connection. */
		pNode->IMG[0] = 0;
		noSavNodeN++;
		pNode++;
		if (noSavNodeN == MAX_XML_NODE_NUM_ONE_PAGE)//512 * 16 == 8K
		{
			noSavNodeN = 0;
			totalNodeN += MAX_XML_NODE_NUM_ONE_PAGE;
			MDWrite(DataDiskID, nextXmlBufAddr, DevInfo.PageSize, XML_DATA);
			nextXmlBufAddr += DevInfo.PageSize;
			pNode = (XmlUMLNode *)XML_DATA;
		}
	}
	
XMLNODEANALYZEEND:	
	printf("\nnNum = %d, left = %d\n", nNum, noSavNodeN);
	if (noSavNodeN > 0)
	{
		MDWrite(DataDiskID, nextXmlBufAddr, DevInfo.PageSize, XML_DATA);
		totalNodeN += noSavNodeN;
	}
	printf("Analyze=%d want=%d\n", totalNodeN, nodeNum);
	#if 0
	{	
		int i, j;
		XmlUMLNode testNode;

		printf("ok now test start.%d\n", totalNodeN);
		nextXmlBufAddr = XmlBufAddr + StartPage * DevInfo.PageSize;
		for (i = 0; i < totalNodeN; i++)
		{
			printf("i=%03d ", i);
			MDRead(DataDiskID, nextXmlBufAddr+i, 1, (void *)&testNode);
			printf("id=%d\n", testNode.Guid);
			printf("url=%s\n", testNode.URL);
			printf("name=");
			for (j = 0; j < UMLNAMEMAXLEN; j++)
			{
				if (0 == testNode.Name[j])
					break;
				printf("%04x ", testNode.Name[j]);
			}
			printf("\n\n\n");
		}
		printf("ok now test end...\n");
	}
	#endif

	return totalNodeN;
}


/*******************************************************************************
** Name: Xml_Get_TotleFile
** Input:uint8 * pUrl
** Return: uint32
** Owner:csun
** Date: 2013.4.28
** Time: 9:02:12
*******************************************************************************/
_ATTR_XML_ANALYZE_CODE_  
int Xml_Get_TotleFile(uint8 * pUrl)
{
    uint32 dataLen;
    uint8 *pBuf;
    int len, num;

    if (Http_Get_Url(pUrl, NULL, 0, 0) != RETURN_OK)
		return 0;
	
    pBuf = XML_DATA;
    dataLen = 0;
	Http_Set_Timeout(2);
    while (1)  //读取网络XML列表
    {
		eth_service(1);
		len = Http_Valid_Datalen();
		if (len >= 0)
		{
			if (len > 0)
			{                    
                len = Http_Read_Data(pBuf + dataLen, 512);
				dataLen += len;
				pBuf[dataLen] = 0;
				break;
			}
		}
		else
		{              
            pBuf[dataLen] = 0;
			break;
		}
    }
	
	printf("xmllen=%d\n", dataLen);
	if (XmlAnalyzeLableInt("<num>", 5, XML_DATA, dataLen, &num))
		return 0;
	
	return num;
}

