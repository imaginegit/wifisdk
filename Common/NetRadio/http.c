/*
********************************************************************************
*
*                Copyright (c): 2013 - 2013 + 5, csun
*                             All rights reserved.
*
* FileName: ..\Common\NetRadio\http.c
* Owner: csun
* Date: 2013.3.13
* Time: 11:53:36
* Desc: http driver
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
*       csun     2013.3.13     11:53:52   1.0           create
*   
********************************************************************************
*/
#define __COMMON_NETRADIO_HTTP_C__
#include "sysconfig.h"
#ifdef __COMMON_NETRADIO_HTTP_C__

/*
*-------------------------------------------------------------------------------
*
*                             #include define                                   
*
*-------------------------------------------------------------------------------

*/

#include <stdlib.h>
#include <stdio.h>
#include "sysinclude.h"
#include "Lwipopt.h"
#include "def.h"
#include "Tcp.h"
#include "netif.h"
#include "Ip.h"
#include "pbuf.h"
#include "tools.h"
#include "dns.h"
#include "http.h"

#define SERVER_PORT     	80
#define CLIENT_PORT     	1000

#define DNSRETRYNUM  		5
#define MEMPOOLSIZE			0x6000 //24k

#define MEMPOOLUPLIMIT		(MEMPOOLSIZE * 85 / 100)
#define MEMPOOLDOWNLIMIT	(MEMPOOLSIZE * 10 / 100)
#define MEMPOOLCRASHLIMIT	(MEMPOOLSIZE * 5 / 100)

static volatile unsigned int MemPoolHead = 0, MemPoolTail = 0;
static char MemPoolBuf[MEMPOOLSIZE];

/*
*-------------------------------------------------------------------------------
*
*							   local function declare     	     				 
*
*-------------------------------------------------------------------------------
*/
int    Http_Get_Header_Len(char *data, int dlen);
void   Http_Check_Tcp_State(uint8 bTcpState);
uint32 Http_HdParam(uint8 * dst, uint8 blen, uint8 * src, uint8 * param);
uint32 Http_Response(uint8 * src);

IP_ADDR AtoIp(uint8 *s);
void    AtoAddr(char *s, char *proto, char *user, char *pwrd, char *host, unsigned int *port, char *file);



/*
*-------------------------------------------------------------------------------
*
*							   #define / #typedef define     					 
*
*-------------------------------------------------------------------------------
*/
#define _COMMON_NETRADIO_HTTP_OPEN_  __attribute__((section("common_netradio_http_open")))
#define _COMMON_NETRADIO_HTTP_READ_  __attribute__((section("common_netradio_http_read")))
#define _COMMON_NETRADIO_HTTP_WRITE_  __attribute__((section("common_netradio_http_write")))
#define _COMMON_NETRADIO_HTTP_CLOSE_  __attribute__((section("common_netradio_http_close")))
#define _COMMON_NETRADIO_HTTP_DATA_  __attribute__((section("common_netradio_http_data")))
#define _COMMON_NETRADIO_HTTP_BSS_  __attribute__((section("common_netradio_http_bss")))


typedef __packed struct _LOCAL_VARIABLE_HTTP
{
	uint8 i;

}LOCAL_VARIABLE_HTTP, * P_LOCAL_VARIABLE_HTTP;

typedef struct _HTTP_PCB
{
    uint32  bTcpClosed;
    TCP_PCB *tcb; 
    uint32  dwLocalPort;
    uint32  dwRemotePort;
    uint32  dwTotoLen;
    uint32  dwCurOffset;
    uint32  dwCurLen;
    uint32  bitrate;
	uint32  format;
}HTTP_PCB, *P_HTTP_PCB;

HTTP_PCB stHttpPcb;

static uint32 xml_check_timeout;


/*
*-------------------------------------------------------------------------------
*
*							   local variable define     	     				 
*
*-------------------------------------------------------------------------------
*/



/*
*-------------------------------------------------------------------------------
*
*							   global variable define     	     				 
*
*-------------------------------------------------------------------------------
*/

/*
*-------------------------------------------------------------------------------
*
*							   API(open) define                    				 
*
*-------------------------------------------------------------------------------
*/

static void MemPoolReset(void)
{
	MemPoolHead = 0;
	MemPoolTail = 0;
	return;
}

static unsigned int MemPoolFreeSiz(void)
{
	unsigned int head, tail;

	head = MemPoolHead;//the vaild data begin form positon 'MemPoolTail' and end in position 'MemPoolHead'.
	tail = MemPoolTail;

	if (head > tail)
	{
		return (MEMPOOLSIZE - (head - tail));
	}
	else if(head < tail)
	{
		return (tail - head);
	}
	
	return MEMPOOLSIZE;
}

static unsigned int MemPoolValidSiz(void)
{
	unsigned int head, tail;

	head = MemPoolHead;//the vaild data begin form positon 'MemPoolTail' and end in position 'MemPoolHead'.
	tail = MemPoolTail;

	if (head > tail)
	{
		return (head-tail);
	}
	else if (head < tail)
	{
		return (MEMPOOLSIZE - (tail - head));
	}
	
	return 0;
}

/*
MemPoolPutData:这个函数只管放数据，空间外部保证.
*/
static unsigned int MemPoolPutData(char *data, unsigned int len)
{
    unsigned int head = MemPoolHead;
    unsigned int size = len;

    while (len--)
    {
        MemPoolBuf[head++] = *data++;
        if (head == MEMPOOLSIZE)
        {
			head = 0;
		}
    }
    MemPoolHead = head;
	
	return size;
}

static unsigned int MemPoolGetData(char *dst, unsigned int len)
{
    unsigned int tail = MemPoolTail;
    unsigned int size, i; 

	size = MemPoolValidSiz();
	len = (size > len) ? len : size;
	size = len;
	i = 0;
    while (i < len)
    {
		if (dst)
		{
		    dst[i++] = MemPoolBuf[tail++];
		}
		else
		{
			i++; 
			tail++;
		}
	    if (tail == MEMPOOLSIZE)
	    {
			tail = 0;
	    }
    }
    MemPoolTail = tail;
	
	return size;
}

/*******************************************************************************
** Name: Http_Get_Url
** Input:uint8 * pbUrl
** Return: uint32
** Owner:csun
** Date: 2013.3.13
** Time: 11:54:57
*******************************************************************************/
_COMMON_NETRADIO_HTTP_OPEN_
uint32 Http_Get_Url(uint8 * pbUrl, uint32 * pdwDataLen, uint32 dwOffset, uint8 bWait)
{
	uint8 proto[32];
	uint8 *pBuf, *ptr;
    NETIF * netif;
	TCP_PCB * pcb = NULL;
    uint8 bhttpbuf[1024];
    uint8 * bHost;
    uint8 * bFile;
    IP_ADDR stIp;    
    PBUF * pbuf;
    int    dwHttpHeadLen;
	unsigned int rx_len, tx_len;
    uint32 dwHttpStatus;
    uint8 tcptrycnt;
    uint32 timeout;
    uint32 port;

	if (MSG_LINK_DOWN() || DHDBUS_DOWN())
	{
		return RETURN_FAIL;
	}
	
    bHost = &bhttpbuf[0];
    bFile = &bhttpbuf[100];
    tcptrycnt = 0;

	/* reset memory pool...*/
	MemPoolReset();
	
	/* clear arp list pbuf. */
	arp_entry_p_clr();

	/* reset pbuf...*/
	pbuf_init();
	//printf("pbuf num=%d, unread=0x%x\n", pbuf_free_num(), (int)pcb->unRead);

	/* reset lwip data input list...*/
    lwip_data_input_init();

	/* analyze the URL to host port file....*/
	AtoAddr(pbUrl, proto, 0, 0, bHost, &port, bFile);
    
    printf("Http_Get_Url bHost:%s\n", bHost);
    if (strcmp(proto, "http") != 0)
	{
        return RETURN_FAIL;
	}	

    //wait for DNS, retry 3 times
    for (tcptrycnt = 0; tcptrycnt < DNSRETRYNUM; tcptrycnt++)
    {
    	stIp = AtoIp(bHost);
        if (stIp.addr != 0)
        {
            break;
        }
		else
		{
			arp_list_clr_init();
			printf("dns fail,clear arp list.\n");
		}
    }        

    if ((tcptrycnt >= DNSRETRYNUM) && (stIp.addr == 0))
    {
        printf("DNS Error!!!\n");
        return RETURN_FAIL;
    }       

    //wait for tcp_connect, retry 5 times
    for (tcptrycnt = 0; tcptrycnt < 5; tcptrycnt++)
    {
        stHttpPcb.bTcpClosed   = TCP_WORKING;

        //new pcb
        netif = ip_route(0);
    	pcb   = tcp_new();
        
        stHttpPcb.tcb          = pcb;        
        stHttpPcb.dwLocalPort  = (stHttpPcb.dwLocalPort + 1) % 256 + 4096;

    	tcp_bind(pcb, &netif->ip_addr, stHttpPcb.dwLocalPort);        
        tcp_recv(pcb, Http_Recive_Data);
        tcp_pcb_close(pcb, Http_Check_Tcp_State);

        if (port == 0) 
        {
            port = SERVER_PORT;
        }   
        
        tcp_connect(pcb, &stIp, port);
        timeout = SysTickCounter + 200; //10ms/tick, 2s==2000ms==10ms * 200
        while(1)
    	{
            eth_service(FALSE);
            if (pcb->state == ESTABLISHED)
            {
                break;
    	    }	
			
            if (SysTickCounter > timeout)
            {
				//tcp_abandon(pcb, 1);
				printf("tcp time out try again..\n");
				break;
			}
            if (stHttpPcb.bTcpClosed == TCP_CLOSED)
            {
                printf("tcp_connect: tcptrycnt = %d\n",tcptrycnt);
                break;;
            }
    	}
		printf("tcp finsh\n");
        //ESTABLISHED
        if (pcb->state == ESTABLISHED)
        {
            break;
	    }

		tcp_pcb_purge(pcb);
    }        
    if (tcptrycnt >= 5)
    {
        printf("tcp_connect Error!!!\n");
        return RETURN_FAIL;
    }

#ifdef DBUGPBUF
	printf("Url:");
#endif
    pbuf = pbuf_alloc(PBUF_TRANSPORT,TCP_MSS,PBUF_POOL);
    if(pbuf == NULL)
    {
        return RETURN_FAIL;   
    }
	
	printf("HTTP1.1 GET\n");
	/* send Get HTTP/1.1 1.0 command...*/
	#if 0
    tx_len = sprintf(pbuf->payload, "GET %s HTTP/1.0\r\n"
									"Host: %s\r\n"
									"User-Agent: RkNanoCPlayer\r\n"
									"Ultravox-transport-type: TCP\r\n"
									"Accept: */*\r\n"
									"Icy-MetaData: 1\r\n"
									"Connection: close\r\n"
									"\r\n",
									bFile, bHost);
	printf("%s\n", pbuf->payload);
	#else
    tx_len = sprintf(pbuf->payload, 
    				"GET %s HTTP/1.1\r\n"
					"Accept: */*\r\n"
					"Cache-Control: no-cache\r\n"
					"Connection: Keep-Alive\r\n"
					"Host: %s\r\n"
					"Pragma: no-cache\r\n"
					"Range: bytes=%d-\r\n"
					"User-Agent: RkNanoc Audio Player\r\n"
					"\r\n",
					bFile, bHost, dwOffset);
	//printf("\n\n\n%s\n\n\n", pbuf->payload);
	#endif
	
    tcp_write(pcb, pbuf->payload, tx_len);

    stHttpPcb.dwCurLen    = 0;   
    stHttpPcb.dwCurOffset = 0;    
    stHttpPcb.dwTotoLen = 0;

	/*get data header information*/
	for(;;)
	{
		eth_service(FALSE);
			
        if (stHttpPcb.bTcpClosed == TCP_CLOSED)
        {
             printf("Get tcp connect err %d\n", stHttpPcb.bTcpClosed);
             return RETURN_FAIL;
        }

		rx_len = tcp_read_firstframe(pcb, pbuf->payload);
		if (rx_len > 0)
		{
			pbuf->len    = rx_len;
			dwHttpStatus = Http_Response((char*)pbuf->payload);
			if ((dwHttpStatus != 200) && (dwHttpStatus != 206))
			{
				printf("http error \n");
                return RETURN_FAIL;
			}
			break;
		}
	}
	
    stHttpPcb.dwCurLen    = 0;   
    stHttpPcb.dwCurOffset = 0;    
    stHttpPcb.dwTotoLen = 0xFFFFFFFF;
	stHttpPcb.bitrate = 0;
	stHttpPcb.format = FORMAT_NULL;
	
	pBuf = proto;
	while (1)
	{
		if (Http_HdParam(pBuf, 31, (char*)pbuf->payload, "Content-Length:") == 0)
		{
			stHttpPcb.dwTotoLen = atoi(pBuf);
			printf("Content-Length:%d\n", stHttpPcb.dwTotoLen);
		}

		#if 0//For shoutcast radio Get response...
		if (Http_HdParam(pBuf, 31, (char*)pbuf->payload, "ICY-BR:") == 0)
		{
			stHttpPcb.bitrate = atoi(pBuf);
		}

		if (Http_HdParam(pBuf, 31, (char*)pbuf->payload, "CONTENT-TYPE:") == 0)
		{
			ptr = pBuf;
			if (strncmpi(pBuf, "AUDIO/", 6) == 0)
			{
				ptr += 6;
			}
			else if(strncmpi(pBuf, "APPLICATION/", 12) == 0)
            {
              	ptr += 12;
            }
			if ((strncmpi(ptr, "MPEG", 4) == 0) || (strncmpi(ptr, "MP3",  3) == 0))
			{
              	stHttpPcb.format = FORMAT_MP3;
			}
			else
			{
              	stHttpPcb.format = FORMAT_NULL;
			}
		}
		#endif

		//find the response end..
		dwHttpHeadLen = Http_Get_Header_Len(pbuf->payload, rx_len);
		if (dwHttpHeadLen)
		{
			pbuf_header(pbuf, -dwHttpHeadLen);
			tcp_set_firstframe(pcb, pbuf);

			/* move vaild data to memory pool.*/
			Http_Recive_Data(tcp_curget());
			break;
		}
		
		eth_service(FALSE);
        if (stHttpPcb.bTcpClosed == TCP_CLOSED)
        {
             printf("Get tcp connect err %d\n", stHttpPcb.bTcpClosed);
             return RETURN_FAIL;
        }
		rx_len = tcp_read_firstframe(pcb, pbuf->payload);
		if (rx_len > 0)
		{
			pbuf->len  = rx_len;
		}
	}
	
    if (bWait)
    {
		/*tcp_tick_ack_enable*/
		tcp_tick_ack_enable(pcb);
		
        while ((stHttpPcb.dwCurOffset < MEMPOOLUPLIMIT) && (stHttpPcb.dwCurOffset < stHttpPcb.dwTotoLen))
        {
            eth_service(-1);	
            if (stHttpPcb.bTcpClosed == TCP_CLOSED)
            {
                 printf("bWait tcp connect err \n");
                 return RETURN_FAIL;
            }
        }
		printf("dwCurOffset=%d\n", stHttpPcb.dwCurOffset);
    }

	if (pdwDataLen)
	{
	    *pdwDataLen = stHttpPcb.dwTotoLen;
	}

    return RETURN_OK;
}

/*
*-------------------------------------------------------------------------------
*
*							   local function(open) define         				 
*
*-------------------------------------------------------------------------------
*/
/*******************************************************************************
** Name: Http_Get_Header_Len
** Input:PBUF * pBuf
** Return: uint32
** Owner:csun
** Date: 2013.4.17
** Time: 15:51:34
*******************************************************************************/
_COMMON_NETRADIO_HTTP_OPEN_
int Http_Get_Header_Len(char *data, int dlen)
{
	int i;

	if ((data == NULL) || (dlen < 4))
	{
		return 0;
	}

	for (i = 0; i < dlen; i++)
	{
		if ((data[i+0] == '\r') && (data[i+1] == '\n') && 
			(data[i+2] == '\r') && (data[i+3] == '\n'))
		{
			return (i + 4);
		}
	}
    
	return 0;
}

/*******************************************************************************
** Name: Http_HdParam
** Input:uint8 * dst, uint8 blen, uint8 * src, uint8 * param
** Return: uint32
** Owner:csun
** Date: 2013.4.16
** Time: 20:48:56
*******************************************************************************/
_COMMON_NETRADIO_HTTP_OPEN_
uint32 Http_HdParam(uint8 * dst, uint8 blen, uint8 * src, uint8 * param)
{
	char *ptr;

	ptr = dst; //save dst

	src = strstri(src, param);
	if (src)
	{
		src += strlen(param);
		for (; blen != 0; blen--)
		{
			if((*src==0) || (*src=='\n') || (*src=='\r'))
			{
				break;
			}
			*dst++ = *src++;
		}
		*dst = 0;
		//remove space at start and end of string
		strrmvspace(ptr, ptr);
		if (strlen(ptr))
		{
			return RETURN_OK;
		}
	}

	return RETURN_FAIL;
}

/*******************************************************************************
** Name: Http_Response
** Input:uint8 * src
** Return: uint32
** Owner:csun
** Date: 2013.4.16
** Time: 20:35:57
*******************************************************************************/
_COMMON_NETRADIO_HTTP_OPEN_
uint32 Http_Response(uint8 * src)
{
	unsigned int search=16;

	/* skip spaces*/
	while((*src==' ') && search)
	{ 
		src++; 
		search--; 
	}
	if((strncmpi(src, "ICY", 3)  == 0) ||
       (strncmpi(src, "HTTP", 4) == 0) ||
       (strncmpi(src, "RTSP", 4) == 0))
	{
		/* skip proto name*/
		while(*src && (*src!=' ') && search)
		{ 
			src++; 
			search--; 
		} 
		/* skip spaces*/
		while((*src==' ') && search)
		{
			src++; 
			search--; 
		}
		if(search)
		{
			return atoi(src);
		}
	}

	return 0;
}


/*******************************************************************************
** Name: AtoAddr
** Input:uint8 *s, uint8 *proto, uint8 *host, uint8 *file
** Return: void
** Owner:void
** Date: 2013.3.26
** Time: 22:15:15
*******************************************************************************/
_COMMON_NETRADIO_HTTP_OPEN_
void AtoAddr(char *s, char *proto, char *user, char *pwrd, char *host, unsigned int *port, char *file)
{
	/*
	for example:s = "http://api.xbd61.com/ApiForM/TjMusicList.php?Apiadmin=xiaobuding"
	*/
	if (proto){*proto = 0;}
	if (user) {*user  = 0;}
	if (pwrd) {*pwrd  = 0;}
	if (host) {*host  = 0;}
	if (port) {*port  = 0;}
	if (file) {*file++ = '/'; *file = 0;}

	while (*s == ' '){s++;} //skip spaces

	//get proto "http://"
	if (strncmpi(s, "http://", 7) == 0)
	{
		s += 7;
		if(proto)
		{
			strcpy(proto, "http");
		}
	}
	else
	{
		return;
	}

	//get user & password
	if (strstr(s, "@") != 0)//find string "@" form string "s",
	{
		while (*s && (*s!=':'))
		{
			if (user)
			{
				*user++ = *s;
				*user   = 0;
			}
			s++;
		}
		s++; //skip ":"
		while (*s && (*s!='@'))
		{
			if (pwrd)
			{
				*pwrd++ = *s;
				*pwrd   = 0;
			}
			s++;
		}
		s++; //skip "@"
	}

	//get host:"api.xbd61.com"
	while (*s && (*s!=':') && (*s!='/'))
	{
		if(host)
		{
			*host++ = *s;
			*host   = 0;
		}
		s++;
	}

	//get port.the part:"80"
	if (*s == ':')
	{
		s++; //skip ":"
		if (port)
		{
			*port = atoi(s);
		}
		while(isdigit(*s))//skip port
		{ 
			s++; 
		}; 
	}

	//get file:"ApiForM/TjMusicList.php?Apiadmin=xiaobuding"
	if(*s == '/')// "/ApiForM/TjMusicList.php?Apiadmin=xiaobuding"
	{
		s++; //skip "/"
		while(*s && !isspace(*s))
		{
			if(file)
			{
				*file++ = *s;
				*file   = 0;
			}
			s++;
		}
	}
	return;
}

/*******************************************************************************
** Name: AtoIp
** Input:uint8 *s
** Return: IP_ADDR
** Owner:void
** Date: 2013.3.26
** Time: 22:24:58
*******************************************************************************/
_COMMON_NETRADIO_HTTP_OPEN_
IP_ADDR AtoIp(uint8 *s)
{
	IP_ADDR ip = {0};

	if (isdigit(*s)) //ip
	{
		ip.addr |= atou(s)<<0;
		while(isdigit(*s)){ s++; }; while(!isdigit(*s)){ s++; };
		ip.addr |= atou(s)<<8;
		while(isdigit(*s)){ s++; }; while(!isdigit(*s)){ s++; };
		ip.addr |= atou(s)<<16;
		while(isdigit(*s)){ s++; }; while(!isdigit(*s)){ s++; };
		ip.addr |= atou(s)<<24;
	}
	else //get ip -> dns resolve
	{
		ip = dns_getip(s);
	}

	return ip;
}


/*
*-------------------------------------------------------------------------------
*
*							   API(read) define             	     			 
*
*-------------------------------------------------------------------------------
*/
/*******************************************************************************
** Name: Http_Resume
** Input:void
** Return: uint32
** Owner:csun
** Date: 2013.4.17
** Time: 20:09:39
*******************************************************************************/
_COMMON_NETRADIO_HTTP_READ_
uint32 Http_Resume(void)
{
    if ((stHttpPcb.dwCurLen > 10 * TCP_MSS) || (stHttpPcb.bTcpClosed != TCP_WORKING))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
** Name: Http_Pause
** Input:void
** Return: uint32
** Owner:csun
** Date: 2013.3.13
** Time: 15:21:29
*******************************************************************************/
_COMMON_NETRADIO_HTTP_READ_
uint32 Http_Pause(void)
{
    if ((stHttpPcb.dwCurLen < MEMPOOLDOWNLIMIT) && (stHttpPcb.bTcpClosed == TCP_WORKING))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
fun		:Http_Set_Timeout
parm	:timeout unit is second
*/
_COMMON_NETRADIO_HTTP_READ_
void Http_Set_Timeout(uint32 timeout)
{
	xml_check_timeout = XMLCHECKNOTHING;
	
	/* 1 s have 100 tick,*/
	if (timeout != -1)
	{
		xml_check_timeout = timeout * 100 + SysTickCounter;
	}
}


_COMMON_NETRADIO_HTTP_READ_
int Http_Valid_Datalen(void)
{
	int len = -1;
    int vaillen;

    vaillen  = MemPoolValidSiz();
	if ((stHttpPcb.bTcpClosed == TCP_WORKING) || (stHttpPcb.tcb->unRead) || vaillen)
	{
		len = vaillen;
	}
	
	/*bug: tcp abort close,result in close time so long...	*/
	if ((len == 0) && (stHttpPcb.tcb->state == LAST_ACK))
	{
		printf("Xml get fin.\n");
		len = -1;
	}

	if (xml_check_timeout != XMLCHECKNOTHING)
	{
		if (SysTickCounter >= xml_check_timeout)
		{
			printf("xml timeout\n");
			len = -1;
		}
	}
	
	return len;
}

_COMMON_NETRADIO_HTTP_READ_
int Http_warn_pause_check(void)
{
	if ((stHttpPcb.dwCurLen < MEMPOOLCRASHLIMIT) && (stHttpPcb.bTcpClosed == TCP_WORKING))
	{
		printf("CurLen:%d CurOffset:%d\n", stHttpPcb.dwCurLen, stHttpPcb.dwCurOffset);
		return 1;
	}
		
	return 0;
}

_COMMON_NETRADIO_HTTP_READ_
uint32 Http_Get_CurOffset(void)
{
	return stHttpPcb.dwCurLen;
}
/*******************************************************************************
** Name: Http_Read_Data
** Input:uint8 * pbDataBuf, uint32 dwLength
** Return: uint32
** Owner:csun
** Date: 2013.3.13
** Time: 11:55:45
*******************************************************************************/
_COMMON_NETRADIO_HTTP_READ_
uint32 Http_Read_Data(uint8 * pbDataBuf, uint32 dwLength)
{
	uint HasReadSize;
	
	HasReadSize = MemPoolGetData(pbDataBuf, dwLength);
	stHttpPcb.dwCurLen -= HasReadSize;
	
    //printf("mp3 read = %d\n", pbuf_free_num());
    return HasReadSize;
}


/*
*-------------------------------------------------------------------------------
*
*							  local function(read) define              	  		 
*
*-------------------------------------------------------------------------------
*/
/*******************************************************************************
** Name: Http_Check_Tcp_State
** Input:void
** Return: void
** Owner:csun
** Date: 2013.4.17
** Time: 9:42:23
*******************************************************************************/
_COMMON_NETRADIO_HTTP_READ_
void Http_Check_Tcp_State(uint8 bTcpState)
{   
    stHttpPcb.bTcpClosed = bTcpState;    
}

/*******************************************************************************
** Name: Http_Recive_Data
** Input:PBUF * pbuf
** Return: void
** Owner:csun
** Date: 2013.4.16
** Time: 20:18:47
*******************************************************************************/
_COMMON_NETRADIO_HTTP_READ_
void Http_Recive_Data(TCP_PCB *pcb)
{
	PBUF *nextpbuf, *freepbuf; 
	uint FreeSize, recvdlen;

	UserIsrDisable();

	FreeSize = MemPoolFreeSiz();
	nextpbuf = pcb->unRead;
	recvdlen = 0;
	while (nextpbuf && (FreeSize > nextpbuf->len))
	{
		MemPoolPutData((char*)nextpbuf->payload, nextpbuf->len);
		recvdlen += nextpbuf->len;
		FreeSize = MemPoolFreeSiz();
		freepbuf = nextpbuf;
		nextpbuf = nextpbuf->next;
		pbuf_free(freepbuf);
	}
	pcb->unRead = nextpbuf;
	
	stHttpPcb.dwCurOffset += recvdlen;
	stHttpPcb.dwCurLen += recvdlen;
	tcp_recved(pcb, recvdlen);

    if((stHttpPcb.dwTotoLen != 0) && (stHttpPcb.dwTotoLen == stHttpPcb.dwCurOffset))
    {
        stHttpPcb.bTcpClosed = TCP_IDLE;
    }      
  	UserIsrEnable(1);
	#if 0
	if (recvdlen)
	{
		printf("clr_tcp_win : %d\n", recvdlen);
	}
	#endif
	
	return;
}

uint32 http_had_revd(void)
{
	return stHttpPcb.dwCurOffset;
}

/*
*-------------------------------------------------------------------------------
*
*							   API(write) define            	     			 
*
*-------------------------------------------------------------------------------
*/



/*
*-------------------------------------------------------------------------------
*
*							  local function(write) define             	  		 
*
*-------------------------------------------------------------------------------
*/



/*
*-------------------------------------------------------------------------------
*
*							   API(close) define              	     			 
*
*-------------------------------------------------------------------------------
*/
/*******************************************************************************
** Name: Http_Close_Cur_Url
** Input:void
** Return: uint32
** Owner:csun
** Date: 2013.4.17
** Time: 8:40:43
*******************************************************************************/
_COMMON_NETRADIO_HTTP_CLOSE_
uint32 Http_Close_Cur_Url(void)
{
    if(stHttpPcb.bTcpClosed != TCP_CLOSED)
    {
        tcp_pcb_remove_nolist(stHttpPcb.tcb);
        tcp_close(stHttpPcb.tcb);
        while(1)
        {
            eth_service(FALSE);
            if (stHttpPcb.bTcpClosed == TCP_CLOSED)
            {
                break;
            }         
			tcp_pcb_purge_unread(NULL);
        }
    }
    printf("Http_Close_Cur_Url:%d\n", stHttpPcb.bTcpClosed);
    
    return RETURN_OK;
    
}

/*
*-------------------------------------------------------------------------------
*
*							  local function(close) define             	  		 
*
*-------------------------------------------------------------------------------
*/



#endif

