/**
 * @file
 * DNS - host name to IP address resolver.
 *
 */

/**

 * This file implements a DNS host name to IP address resolver.

 * Port to lwIP from uIP
 * by Jim Pettinato April 2007

 * uIP version Copyright (c) 2002-2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * DNS.C
 *
 * The lwIP DNS resolver functions are used to lookup a host name and
 * map it to a numerical IP address. It maintains a list of resolved
 * hostnames that can be queried with the dns_lookup() function.
 * New hostnames can be resolved using the dns_query() function.
 *
 * The lwIP version of the resolver also adds a non-blocking version of
 * gethostbyname() that will work with a raw API application. This function
 * checks for an IP address string first and converts it if it is valid.
 * gethostbyname() then does a dns_lookup() to see if the name is 
 * already in the table. If so, the IP is returned. If not, a query is 
 * issued and the function returns with a ERR_INPROGRESS status. The app
 * using the dns client must then go into a waiting state.
 *
 * Once a hostname has been resolved (or found to be non-existent),
 * the resolver code calls a specified callback function (which 
 * must be implemented by the module that uses the resolver).
 */

/*-----------------------------------------------------------------------------
 * RFC 1035 - Domain names - implementation and specification
 * RFC 2181 - Clarifications to the DNS Specification
 *----------------------------------------------------------------------------*/

/** @todo: define good default values (rfc compliance) */
/** @todo: improve answer parsing, more checkings... */
/** @todo: check RFC1035 - 7.3. Processing responses */

/*-----------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include "sdio_card.h"
#include "lwipopt.h"
#include "def.h"
#include "err.h"	 
#include "udp.h"
#include "dns.h"
#include "inet.h"
#include "eth.h"
#include <string.h>

/** DNS server IP address */
#ifndef DNS_SERVER_ADDRESS
#define DNS_SERVER_ADDRESS        inet_addr("208.67.222.222") /* resolver1.opendns.com */
#endif

/** DNS maximum number of retries when asking for a name, before "timeout". */
#ifndef DNS_MAX_RETRIES
#define DNS_MAX_RETRIES           2
#endif

/** DNS resource record max. TTL (one week as default) */
#ifndef DNS_MAX_TTL
#define DNS_MAX_TTL               604800
#endif

/* DNS protocol flags */
#define DNS_FLAG1_RESPONSE        0x80
#define DNS_FLAG1_OPCODE_STATUS   0x10
#define DNS_FLAG1_OPCODE_INVERSE  0x08
#define DNS_FLAG1_OPCODE_STANDARD 0x00
#define DNS_FLAG1_AUTHORATIVE     0x04
#define DNS_FLAG1_TRUNC           0x02
#define DNS_FLAG1_RD              0x01
#define DNS_FLAG2_RA              0x80
#define DNS_FLAG2_ERR_MASK        0x0f
#define DNS_FLAG2_ERR_NONE        0x00
#define DNS_FLAG2_ERR_NAME        0x03

/* DNS protocol states */
#define DNS_STATE_UNUSED          0
#define DNS_STATE_NEW             1
#define DNS_STATE_ASKING          2
#define DNS_STATE_DONE            3


/** DNS message header */
typedef __packed struct _DNS_HDR 
{
	uint16 id;
	uint8 flags1;
	uint8 flags2;
	uint16 numquestions;
	uint16 numanswers;
	uint16 numauthrr;
	uint16 numextrarr;
} DNS_HDR;


#define SIZEOF_DNS_HDR 12


/** DNS query message structure */
typedef __packed struct _DNS_QUERY
{
  /* DNS query record starts with either a domain name or a pointer
     to a name already present somewhere in the packet. */
	uint16 type;
	uint16 class;
} DNS_QUERY;


#define SIZEOF_DNS_QUERY 4


/** DNS answer message structure */
typedef __packed struct _DNS_ANSWER
{
  /* DNS answer record starts with either a domain name or a pointer
     to a name already present somewhere in the packet. */
	uint16 type;
	uint16 class;
	uint32 ttl;
	uint16 len;
} DNS_ANSWER;

#define SIZEOF_DNS_ANSWER 10

/** DNS table entry */
typedef __packed struct _DNS_TABLE_ENTRY
{
  uint8  state;
  uint8  numdns;//which DNS server to get the result...
  uint8  tmr;
  uint8  retries;
  uint8  seqno;
  uint8  err;
  uint32 ttl;
  char name[DNS_MAX_NAME_LENGTH];
  IP_ADDR ipaddr;
  /* pointer to callback on DNS query done */
  dns_found_callback found;
  void *arg;
}DNS_TABLE_ENTRY;


/* forward declarations */
static void dns_recv(void *s, UDP_PCB *pcb, PBUF *p, IP_ADDR *addr, uint16 port);
static void dns_check_entries(void);

/*-----------------------------------------------------------------------------
 * Globales
 *----------------------------------------------------------------------------*/

/* DNS variables */
static UDP_PCB *dns_pcb = NULL;
static uint8  dns_seqno = 0;
static uint8  dns_jump_once = 0;
static DNS_TABLE_ENTRY  dns_table[DNS_TABLE_SIZE];
static IP_ADDR  dns_servers[DNS_MAX_SERVERS];

int dns_jump_this_tmr()
{
	if (dns_jump_once)
	{
		dns_jump_once--;
		return 1;
	}
	return 0;
}
/**
 * Initialize the resolver: set up the UDP pcb and configure the default server
 * (DNS_SERVER_ADDRESS).
 */
void dns_init()
{
	IP_ADDR dnsserver;

	/* initialize default DNS server address */
	dnsserver.addr = DNS_SERVER_ADDRESS;

	/* if dns client not yet initialized... */
	memset((void *)&dns_table[0], 0, sizeof(DNS_TABLE_ENTRY)*DNS_TABLE_SIZE);
	dns_pcb = udp_new(1);
	if (dns_pcb != NULL)
	{
		/* initialize DNS table not needed (initialized to zero since it is a
		* global variable) */

		/* initialize DNS client */
		udp_bind(dns_pcb, IP_ADDR_ANY, 0);
		udp_recv(dns_pcb, dns_recv, NULL);
		/* initialize default DNS primary server */
		dns_setserver(0, &dnsserver);
	}

	eth_tmr_mask(DNS_TMR_1000MS, TRUE);
}

/**
 * Initialize one of the DNS servers.
 *
 * @param numdns the index of the DNS server to set must be < DNS_MAX_SERVERS
 * @param dnsserver IP address of the DNS server to set
 */
void dns_setserver(uint8 numdns, IP_ADDR *dnsserver)
{
	if ((numdns < DNS_MAX_SERVERS) && (dns_pcb != NULL) &&
	  (dnsserver != NULL) && (dnsserver->addr !=0 ))
	{
		dns_servers[numdns] = (*dnsserver);
	}
}

/**
 * Obtain one of the currently configured DNS server.
 *
 * @param numdns the index of the DNS server
 * @return IP address of the indexed DNS server or "ip_addr_any" if the DNS
 *         server has not been configured.
 */
IP_ADDR dns_getserver(uint8 numdns)
{
	if (numdns < DNS_MAX_SERVERS)
	{
		return dns_servers[numdns];
	}
	else
	{
		return *IP_ADDR_ANY;
	}
}

/**
 * The DNS resolver client timer - handle retries and timeouts and should
 * be called every DNS_TMR_INTERVAL milliseconds (every second by default).
 */
void dns_tmr(void)
{
	if (dns_pcb != NULL)
	{    
		dns_check_entries();
	}
}


/**
 * Look up a hostname in the array of known hostnames.
 *
 * @note This function only looks in the internal array of known
 * hostnames, it does not send out a query for the hostname if none
 * was found. The function dns_enqueue() can be used to send a query
 * for a hostname.
 *
 * @param name the hostname to look up
 * @return the hostname's IP address, as u32_t (instead of struct ip_addr to
 *         better check for failure: != INADDR_NONE) or INADDR_NONE if the hostname
 *         was not found in the cached dns_table.
 */
static uint32 dns_lookup(const char *name)
{
	uint8 i;

	/* Walk through name list, return entry if found. If not, return NULL. */
    #if 0
	for (i = 0; i < DNS_TABLE_SIZE; ++i)
	{
		if ((dns_table[i].state == DNS_STATE_DONE) &&
		    (strcmp((const char *)name, (const char *)dns_table[i].name) == 0))
		{
			return dns_table[i].ipaddr.addr;
		}
	}
    #endif
    
	return INADDR_NONE;
}

#if DNS_DOES_NAME_CHECK
/**
 * Compare the "dotted" name "query" with the encoded name "response"
 * to make sure an answer from the DNS server matches the current dns_table
 * entry (otherwise, answers might arrive late for hostname not on the list
 * any more).
 *
 * @param query hostname (not encoded) from the dns_table
 * @param response encoded hostname in the DNS response
 * @return 0: names equal; 1: names differ
 dgl:
 	gemini.tuc.noao.edu
 	6gemini3tuc4noao3edu0
 */
static uint8 dns_compare_name(unsigned char *query, unsigned char *response)
{
	unsigned char n;

	do 
	{
		n = *response++;
		/** @see RFC 1035 - 4.1.4. Message compression */
		if ((n & 0xc0) == 0xc0)
		{
			/* Compressed name */
			break;
		} 
		else 
		{
			/* Not compressed name */
			while (n > 0) 
			{
				if ((*query) != (*response))
				{
					return 1;
				}
				++response;
				++query;
				--n;
			};
			++query;
		}
	} while (*response != 0);

  return 0;
}
#endif /* DNS_DOES_NAME_CHECK */

/**
 * Walk through a compact encoded DNS name and return the end of the name.
 *
 * @param query encoded DNS name in the DNS server response
 * @return end of the name
 */
static unsigned char * dns_parse_name(unsigned char *query)
{
	unsigned char n;

	do 
	{
		n = *query++;
		/** @see RFC 1035 - 4.1.4. Message compression */
		if ((n & 0xc0) == 0xc0) 
		{
			/* Compressed name */
			break;
		}
		else
		{
			/* Not compressed name */
			while (n > 0)
			{
				++query;
				--n;
			};
		}
	} while (*query != 0);

	return query + 1;
}

/**
 * Send a DNS query packet.
 *
 * @param numdns index of the DNS server in the dns_servers table
 * @param name hostname to query
 * @param id index of the hostname in dns_table, used as transaction ID in the
 *        DNS query packet
 * @return ERR_OK if packet is sent; an err_t indicating the problem otherwise
 */
static uint8 dns_send(uint8 numdns, const char* name, uint8 id)
{
	uint8 err;
	DNS_HDR *hdr;
	DNS_QUERY qry;
	PBUF *p;
	char *query, *nptr;
	const char *pHostname;
	uint8 n;

	/* if here, we have either a new query or a retry on a previous query to process */
#ifdef DBUGPBUF
	printf("ds:");
#endif
	p = pbuf_alloc(PBUF_TRANSPORT, SIZEOF_DNS_HDR + DNS_MAX_NAME_LENGTH + SIZEOF_DNS_QUERY, PBUF_POOL);
	if (p != NULL)
	{    
		/* fill dns header */
		hdr = (DNS_HDR *)p->payload;
		memset(hdr, 0, SIZEOF_DNS_HDR);
		hdr->id = htons(id);
		hdr->flags1 = DNS_FLAG1_RD;
		hdr->numquestions = htons(1);
		query = (char*)hdr + SIZEOF_DNS_HDR;
		pHostname = name;
		--pHostname;  //- the '.' for loop.

		/* convert hostname into suitable query format. */
		do
		{
			++pHostname;  //+ the '.'
			nptr = query;
			++query;

			for(n = 0; *pHostname != '.' && *pHostname != 0; ++pHostname)
			{
				*query = *pHostname;
				++query;
				++n;
			}

			*nptr = n;
		} while(*pHostname != 0);
		*query++='\0';

		/* fill dns query */
		qry.type  = htons(DNS_RRTYPE_A);
		qry.class = htons(DNS_RRCLASS_IN);
		MEMCPY(query, &qry, SIZEOF_DNS_QUERY);

		/* connect to the server for faster receiving */
		udp_connect(dns_pcb, &dns_servers[numdns], DNS_SERVER_PORT);
		/* send dns packet */
		err = udp_sendto(dns_pcb, p, &dns_servers[numdns], DNS_SERVER_PORT);

		/* free pbuf */
		pbuf_free(p);
	}
	else 
	{
		err = ERR_MEM;
	}

	return err;
}

/**
 * dns_check_entry() - see if pEntry has not yet been queried and, if so, sends out a query.
 * Check an entry in the dns_table:
 * - send out query for new entries
 * - retry old pending entries on timeout (also with different servers)
 * - remove completed entries from the table if their TTL has expired
 *
 * @param i index of the dns_table entry to check
 */
static void dns_check_entry(uint8 i)
{
	DNS_TABLE_ENTRY *pEntry = &dns_table[i];

	switch(pEntry->state) 
	{
		case DNS_STATE_NEW: 
		{
			/* initialize new entry */
			pEntry->state   = DNS_STATE_ASKING;
			pEntry->numdns  = 0;
			pEntry->tmr     = 1;
			pEntry->retries = 0;
			/* send DNS packet for this entry */
			dns_jump_once = 1;
			eth_tmr_mask(DNS_TMR_1000MS, FALSE);
			printf("NEW dns_send:");
			dns_send(pEntry->numdns, (const char*)pEntry->name, i);//=>dns_recv
			break;
		}

		case DNS_STATE_ASKING:
		{
			if (--pEntry->tmr == 0)
			{
				if (++pEntry->retries == DNS_MAX_RETRIES)
				{
					if ((pEntry->numdns+1 < DNS_MAX_SERVERS) && (dns_servers[pEntry->numdns+1].addr != 0))
					{
						/* change of server */
						pEntry->numdns++;
						pEntry->tmr     = 1;
						pEntry->retries = 0;
						break;
					}
					else
					{            
						/* call specified callback function if provided */
						if (pEntry->found)//my_dns_found_callback
						{
							(*pEntry->found)((const char*)pEntry->name, NULL, pEntry->arg);
						}
						/* flush this entry */
						pEntry->state   = DNS_STATE_UNUSED;
						pEntry->found   = NULL;
						break;
					}
				}

				/* wait longer for the next retry */
				pEntry->tmr = pEntry->retries;
				/* send DNS packet for this entry */
				printf("ASKING dns_send:");
				dns_send(pEntry->numdns, (const char*)pEntry->name, i);
			}
			break;
		}

		case DNS_STATE_DONE: 
		{
			/* if the time to live is nul */
			if (--pEntry->ttl == 0)
			{
				pEntry->state = DNS_STATE_UNUSED; //flush this entry
				pEntry->found = NULL;
			}
			break;
		}

		case DNS_STATE_UNUSED:
			/* nothing to do */
			break;

		default:
			break;
	}
}

/**
 * Call dns_check_entry for each entry in dns_table - check all entries.
 */
static void dns_check_entries(void)
{
	uint8 i;

	for (i = 0; i < DNS_TABLE_SIZE; ++i)
	{
		dns_check_entry(i);
	}
}

/**
 * Receive input function for DNS response packets arriving for the dns UDP pcb.
 *
 * @params see udp.h
 */
static void dns_recv(void *arg, UDP_PCB *pcb, PBUF *p, IP_ADDR *addr, uint16 port)
{
	uint8 i;
	char *pHostname;
	DNS_HDR *hdr;
	DNS_ANSWER ans;
	DNS_TABLE_ENTRY *pEntry;
	uint8 nquestions, nanswers;


printf("fle-->dns_recv\n");
	/* is the dns message too big ? */
	if (p->len > DNS_MSG_SIZE)
	{
		/* free pbuf and return */
		goto memerr1;
	}

	/* is the dns message big enough ? */
	if (p->len < (SIZEOF_DNS_HDR + SIZEOF_DNS_QUERY + SIZEOF_DNS_ANSWER))
	{
		/* free pbuf and return */
		goto memerr1;
	}

	/* copy dns payload inside static buffer for processing */ 
    /* The ID in the DNS header should be our entry into the name table. */
    hdr = (DNS_HDR *)(p->payload);
    i = htons(hdr->id);
    if (i < DNS_TABLE_SIZE)
    {
		pEntry = &dns_table[i];
		if (pEntry->state == DNS_STATE_ASKING)
		{
			/* This entry is now completed. */
			pEntry->state = DNS_STATE_DONE;
			pEntry->err   = hdr->flags2 & DNS_FLAG2_ERR_MASK;

			/* We only care about the question(s) and the answers. The authrr
			and the extrarr are simply discarded. */
			nquestions = htons(hdr->numquestions);
			nanswers   = htons(hdr->numanswers);

			/* Check for error. If so, call callback to inform. */
			if (((hdr->flags1 & DNS_FLAG1_RESPONSE) == 0) || (pEntry->err != 0) || (nquestions != 1))
			{
				goto responseerr;
			}

			/* Check if the name in the "question" part match with the name in the entry. */
			if (dns_compare_name((unsigned char *)(pEntry->name), (unsigned char *)(p->payload) + SIZEOF_DNS_HDR) != 0)
			{
				/* call callback to indicate error, clean up memory and return */
				goto responseerr;
			}
			
			/* Skip the name in the "question" part */
			pHostname = (char *)dns_parse_name((unsigned char *)(p->payload) + SIZEOF_DNS_HDR) + SIZEOF_DNS_QUERY;

			while(nanswers > 0)
			{
				/* skip answer resource record's host name */
				pHostname = (char *) dns_parse_name((unsigned char *)pHostname);

				/* Check for IP address type and Internet class. Others are discarded. */
				MEMCPY(&ans, pHostname, SIZEOF_DNS_ANSWER);
				if((ntohs(ans.type) == DNS_RRTYPE_A) 
					&& (ntohs(ans.class) == DNS_RRCLASS_IN) 
					&& (ntohs(ans.len) == sizeof(IP_ADDR)) )
				{
					/* read the answer resource record's TTL, and maximize it if needed */
					pEntry->ttl = ntohl(ans.ttl);
					if (pEntry->ttl > DNS_MAX_TTL)
					{
						pEntry->ttl = DNS_MAX_TTL;
					}
					/* read the IP address after answer resource record's header */
					MEMCPY( &(pEntry->ipaddr), (pHostname+SIZEOF_DNS_ANSWER), sizeof(IP_ADDR));

					/* call specified callback function if provided */
					if (pEntry->found)//my_dns_found_callback
					{
						(*pEntry->found)((const char*)pEntry->name, &pEntry->ipaddr, pEntry->arg);
					}
					/* deallocate memory and return */
					goto memerr1;
				}
				else 
				{
					pHostname = pHostname + SIZEOF_DNS_ANSWER + htons(ans.len);
				}
				--nanswers;
			}
			/* call callback to indicate error, clean up memory and return */
			goto responseerr;
		}
		else if(pEntry->state == DNS_STATE_DONE)
		{
			printf("dns_recv ************************************\n");
			goto memerr1;
		}
    }

responseerr:
	/* ERROR: call specified callback function with NULL as name to indicate an error */
	if(pEntry->found)//my_dns_found_callback
	{										  
		(*pEntry->found)((const char*)pEntry->name, NULL, pEntry->arg);
	}
	/* flush this entry */
	pEntry->state = DNS_STATE_UNUSED;
	pEntry->found = NULL;
  
memerr1:
	/* free pbuf */
	pbuf_free(p);
	return;
}

/**
 * Queues a new hostname to resolve and sends out a DNS query for that hostname
 *
 * @param name the hostname that is to be queried
 * @param found a callback founction to be called on success, failure or timeout
 * @param callback_arg argument to pass to the callback function
 * @return @return a err_t return code.
 */
static int8 dns_enqueue(const char *name, dns_found_callback found, void *callback_arg)
{
	uint8 i;
	uint8 lseq, lseqi;
	DNS_TABLE_ENTRY  *pEntry = NULL;

#if 0
	/* search an unused entry, or the oldest one */
	lseq = lseqi = 0;
	for (i = 0; i < DNS_TABLE_SIZE; ++i)
	{
		pEntry = &dns_table[i];
		/* is it an unused entry ? */
		if (pEntry->state == DNS_STATE_UNUSED)
			break;

		/* check if this is the oldest completed entry */
		if (pEntry->state == DNS_STATE_DONE)
		{
			if ((dns_seqno - pEntry->seqno) > lseq)
			{
				lseq = dns_seqno - pEntry->seqno;
				lseqi = i;
			}
		}
	}

	/* if we don't have found an unused entry, use the oldest completed one */
	if (i == DNS_TABLE_SIZE)
	{
		if ((lseqi >= DNS_TABLE_SIZE) || (dns_table[lseqi].state != DNS_STATE_DONE))
		{
			/* no entry can't be used now, table is full */
			return ERR_MEM;
		} 
		else
		{
			/* use the oldest completed one */
			i = lseqi;
			pEntry = &dns_table[i];
		}
	}
#else
	i = 0;
	pEntry = &dns_table[i];
#endif

	/* fill the entry */
	pEntry->state = DNS_STATE_NEW;
	pEntry->seqno = dns_seqno++;
	pEntry->found = found;
	pEntry->arg   = callback_arg;
	strcpy((char*)pEntry->name, name);

	/* force to send query without waiting timer */
	dns_check_entry(i);

	/* dns query is enqueued */
	return ERR_INPROGRESS;
}

/**
 * Resolve a hostname (string) into an IP address.
 * NON-BLOCKING callback version for use with raw API!!!
 *
 * Returns immediately with one of err_t return codes:
 * - ERR_OK if hostname is a valid IP address string or the host
 *   name is already in the local names table.
 * - ERR_INPROGRESS enqueue a request to be sent to the DNS server
 *   for resolution if no errors are present.
 *
 * @param hostname the hostname that is to be queried
 * @param addr pointer to a struct ip_addr where to store the address if it is already
 *             cached in the dns_table (only valid if ERR_OK is returned!)
 * @param found a callback function to be called on success, failure or timeout (only if
 *              ERR_INPROGRESS is returned!)
 * @param callback_arg argument to pass to the callback function
 * @return a err_t return code.
 */
int8 dns_gethostbyname(const char *hostname, IP_ADDR *addr, dns_found_callback found, void *callback_arg)
{
  /* not initialized or no valid server yet, or invalid addr pointer
   * or invalid hostname or invalid hostname length */
	if ((dns_pcb == NULL) || (addr == NULL)
		 || (!hostname) || (!hostname[0]) ||(strlen(hostname) >= DNS_MAX_NAME_LENGTH))
	{
		return ERR_VAL;
	}

	/* host name already in octet notation? set ip addr and return ERR_OK
	* already have this address cached? */
	if (((addr->addr = inet_addr(hostname)) != INADDR_NONE) 
		||((addr->addr = dns_lookup(hostname)) != INADDR_NONE))
	{
		return ERR_OK;
	}

	/* queue query with specified callback */
	return dns_enqueue(hostname, found, callback_arg);
}	

static int MSG_WIFI_DNS_RETURN = 0;

void my_dns_found_callback(const char *name, IP_ADDR *ipaddr, void *callback_arg)
{
	IP_ADDR *dipaddr = callback_arg;

	if (ipaddr == NULL)
	{
	    if (dipaddr)
	    {
	        dipaddr->addr = 0;
	    }
		MSG_WIFI_DNS_RETURN = 1;
	}
	else
	{
	    if (dipaddr)
	    {
	        dipaddr->addr = ipaddr->addr;
	    }
		MSG_WIFI_DNS_RETURN = 1;
	}
	eth_tmr_mask(DNS_TMR_1000MS, TRUE);
	return;
}

IP_ADDR dns_getip(uint8 *domain)
{
	IP_ADDR dnsAddr = {0};
	
	if (netif_is_up())
	{
		if (dns_gethostbyname(domain, &dnsAddr, my_dns_found_callback, (void*)&dnsAddr))
		{
			MSG_WIFI_DNS_RETURN = 0;
			while(1)
			{
				eth_service(FALSE);
				if (MSG_WIFI_DNS_RETURN)
				{
					MSG_WIFI_DNS_RETURN = 0;
					break;
				}
			}
		}
	}
	printf("dns_getip:%s==%u.%u.%u.%u\n", domain,((dnsAddr.addr & 0x000000FF)), ((dnsAddr.addr & 0x0000FF00)>>8),
	                                   ((dnsAddr.addr & 0x00FF0000)>>16),((dnsAddr.addr & 0xFF000000)>>24));
	
	return dnsAddr;
}

