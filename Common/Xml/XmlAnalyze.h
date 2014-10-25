/*
********************************************************************************
*
*				  Copyright (c): 2013 - 2013 + 5, csun
*							   All rights reserved.
*
* FileName: ..\Common\Xml\XmlAnalyze.h
* Owner: csun
* Date: 2013.3.14
* Time: 19:46:06
* Desc: Xml analyze
* History:
*	 <author>	 <date> 	  <time>	 <version>	   <Desc>
*       csun     2013.3.14     19:47:27   1.0           create
*	 
********************************************************************************
*/

#ifndef __COMMON_XML_XMLANALYZE_H__
#define __COMMON_XML_XMLANALYZE_H__


/*
*-------------------------------------------------------------------------------
*
*							   #include define									 
*
*-------------------------------------------------------------------------------
*/

#define _ATTR_XML_ANALYZE_CODE_         __attribute__((section("XmlAnalyzeCode")))
#define _ATTR_XML_ANALYZE_DATA_         __attribute__((section("XmlAnalyzeData")))
#define _ATTR_XML_ANALYZE_BSS_          __attribute__((section("XmlAnalyzeBss"),zero_init))


/*
*-------------------------------------------------------------------------------
*
*							   #define / #typedef define						 
*
*-------------------------------------------------------------------------------
*/

#define FLASHPAGESIZE		0x4000  

#define UMLGUIDMAXLEN		4	//this is a 'INT' size..
#define UMLNAMEMAXLEN		54	//actual is 108 here the unit is UINT16..
#define UMLURLMAXLEN		200 //The UML_URL max len uint8 200..
#define UMLIMGMAXLEN		200 //The UML_IMG max len uint8 200..

#define MAX_XML_NODE_NUM_ONE_PAGE     	16

typedef enum eXMLUMLType
{                   
    XML_UML_ID,
    XML_UML_NAME,
    XML_UML_URL,
    XML_UML_IMG,
    
}XMLUMLType;

/* This struct size 512 = 4 + 54*2 + 200 + 200*/
typedef struct tagXmlUMLNode_INFO
{
	int    Guid;
    uint16 Name[UMLNAMEMAXLEN];
    uint8  URL[UMLURLMAXLEN];
    uint8  IMG[UMLIMGMAXLEN];
}XmlUMLNode;

/*
*-------------------------------------------------------------------------------
*
*							   API Declare          							 
*
*-------------------------------------------------------------------------------
*/
extern int XmlAnalyzeLableInt(const uint8 *label, int labelLen, uint8 *inData, int inLen, int *out);
extern int XmlAnalyzeLableCharstr(const uint8 *label, int labelLen, uint8 *inData, int inLen, uint8 *oStr, int oLen);
extern int XmlAnalyzeLableUnicodestr(const uint8 *label, int labelLen, uint8 *inData, int inLen, uint16 *oStr, int oLen);
extern int XmlConnectAnalyze(uint8 *pXmlUrl, uint32 StartPage, int nodeNum, int timout);
extern int Xml_Get_TotleFile(uint8 * pUrl);
extern int Xml_Get_Item(void *pDst, int DataBufLen, XMLUMLType infoType, int bItemID);

#endif

