/**
 * @file
 * lwIP network interface abstraction
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "sdio_card.h"

#include "lwipopt.h"
#include "def.h"
#include "err.h"
#include "pbuf.h"
#include "ip_addr.h"
#include "tcp.h"
#include "etharp.h"
#include "Autoip.h"
#include "netif.h"
#include "inet.h"

/** the time an ARP entry stays valid after its last update,
 *  for ARP_TMR_INTERVAL = 5000, this is
 *  (240 * 5) seconds = 20 minutes.
 *  (240 * 5) seconds = 20 minutes.
 */
#define ARP_MAXAGE 60


/** the time an ARP entry stays pending after first request,
 *  for ARP_TMR_INTERVAL = 5000, this is
 *  (2 * 5) seconds = 10 seconds.
 * 
 *  @internal Keep this number at least 2, otherwise it might
 *  run out instantly if the timeout occurs directly after a request.
 */
#define ARP_MAXPENDING 2
/** Try hard to create a new entry - we want the IP address to appear in
    the cache (even if this means removing an active entry or so). */
#define ETHARP_FLAG_TRY_HARD     1
#define ETHARP_FLAG_FIND_ONLY    2
#define ETHARP_FLAG_STATIC_ENTRY 4

#define IFNAME0 'e'
#define IFNAME1 'n'
#define HWTYPE_ETHERNET 1
#define IP_ADDR_ANY_VALUE 0x00000000UL
#define IP_ADDR_BROADCAST_VALUE 0xffffffffUL

NETIF __stEMACNetif;
const ETH_ADDR ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};
const ETH_ADDR ethzero = {{0,0,0,0,0,0}};
static ETHARP_ENTRY arp_table[ARP_TABLE_SIZE] = {0};

/* used by IP_ADDR_ANY and IP_ADDR_BROADCAST in ip_addr.h */
const IP_ADDR ip_addr_any = { IP_ADDR_ANY_VALUE };
const IP_ADDR ip_addr_broadcast = { IP_ADDR_BROADCAST_VALUE };

extern PBUF * lwip_unread;

static uint8 etharp_send_ip(NETIF *netif, PBUF *p, ETH_ADDR *src, ETH_ADDR *dst);
char * wifi_get_mac_addr(unsigned char *buf);

/*
void Arp_list_printf(void)
{
	int i;
	for (i=0; i<ARP_TABLE_SIZE; i++)
	{
		if (arp_table->state == ETHARP_STATE_STABLE)
		{
		printf("%d state:%d ip=0x%08X mac=%02x:%02x:%02x:%02x:%02x:%02x\n", i, arp_table[i].state, 
				arp_table[i].ipaddr.addr,  
			   	arp_table[i].ethaddr.addr[0], arp_table[i].ethaddr.addr[1], arp_table[i].ethaddr.addr[2], 
			   	arp_table[i].ethaddr.addr[3], arp_table[i].ethaddr.addr[4], arp_table[i].ethaddr.addr[5]);
		}
	}
}
*/

/**
 * Send a raw ARP packet (opcode and all addresses can be modified)
 *
 * @param netif the lwip network interface on which to send the ARP packet
 * @param ethsrc_addr the source MAC address for the ethernet header
 * @param ethdst_addr the destination MAC address for the ethernet header
 * @param hwsrc_addr the source MAC address for the ARP protocol header
 * @param ipsrc_addr the source IP address for the ARP protocol header
 * @param hwdst_addr the destination MAC address for the ARP protocol header
 * @param ipdst_addr the destination IP address for the ARP protocol header
 * @param opcode the type of the ARP packet
 * @return ERR_OK if the ARP packet has been sent
 *         ERR_MEM if the ARP packet couldn't be allocated
 *         any other err_t on failure
 */
uint8 etharp_raw(NETIF *netif, const ETH_ADDR *ethsrc_addr, const ETH_ADDR *ethdst_addr,
                   const ETH_ADDR *hwsrc_addr, const IP_ADDR *ipsrc_addr,
                   const ETH_ADDR *hwdst_addr, const IP_ADDR *ipdst_addr,
                   const uint16 opcode)
{
	PBUF *p;
	uint8 result = ERR_OK;
	uint8 k; /* ARP entry index */
	ETHARP_HDR *hdr;
#if LWIP_AUTOIP
	const uint8 * ethdst_hwaddr;
#endif

	/* allocate a pbuf for the outgoing ARP request packet */
#ifdef DBUGPBUF
	printf("arp0:");
#endif
	p = pbuf_alloc(PBUF_PHY, sizeof(ETHARP_HDR), PBUF_POOL);
	/* could allocate a pbuf for an ARP request? */
	if (p == NULL)
	{
		return ERR_MEM;
	}
	hdr = (ETHARP_HDR *)p->payload;
	hdr->opcode = htons(opcode);  //ARP_REQUEST or ARP_REPLY
	k = ETHARP_HWADDR_LEN;

	/* If we are using Link-Local, ARP packets must be broadcast on the
	* link layer. (See RFC3927 Section 2.5) */
#if LWIP_AUTOIP   
	ethdst_hwaddr = ((netif->autoip != NULL) && (((AUTO_IP *)(netif->autoip))->state != AUTOIP_STATE_OFF)) ? (uint8*)(ethbroadcast.addr) : ethdst_addr->addr;
#endif

	/* Write MAC-Addresses (combined loop for both headers) */
	while(k > 0)
	{
		k--;
		/* Write the ARP MAC-Addresses */
		hdr->shwaddr.addr[k] = hwsrc_addr->addr[k];
		hdr->dhwaddr.addr[k] = hwdst_addr->addr[k];
		/* Write the Ethernet MAC-Addresses */
#if LWIP_AUTOIP 
		hdr->ethhdr.dest.addr[k] = ethdst_hwaddr[k];
#else
		hdr->ethhdr.dest.addr[k] = ethdst_addr->addr[k];
#endif								
		hdr->ethhdr.src.addr[k]  = ethsrc_addr->addr[k];
	}
	hdr->sipaddr = *(IP_ADDR2 *)ipsrc_addr;
	hdr->dipaddr = *(IP_ADDR2 *)ipdst_addr;

	hdr->hwtype = htons(HWTYPE_ETHERNET);
	hdr->proto = htons(ETHTYPE_IP);

	/* set hwlen and protolen together */
	hdr->_hwlen_protolen = htons((ETHARP_HWADDR_LEN << 8) | sizeof(IP_ADDR));

	hdr->ethhdr.type = htons(ETHTYPE_ARP);
	/* send ARP query */
	result = netif->linkoutput(netif, p);//low_level_output
	/* free ARP query packet */
	pbuf_free(p);

	return result;
}



/**
 * Send an ARP request packet asking for ipaddr.
 *
 * @param netif the lwip network interface on which to send the request
 * @param ipaddr the IP address for which to ask
 * @return ERR_OK if the request has been sent
 *         ERR_MEM if the ARP packet couldn't be allocated
 *         any other uint8 on failure
 */
uint8
etharp_request(NETIF *netif, IP_ADDR *ipaddr)
{
	return etharp_raw(netif, (ETH_ADDR *)netif->hwaddr, &ethbroadcast,
	                (ETH_ADDR *)netif->hwaddr, &netif->ip_addr, 
	                &ethzero, ipaddr, ARP_REQUEST);
}


/**
 * Determine if an address is a broadcast address on a network interface
 *
 * @param addr address to be checked
 * @param netif the network interface against which the address is checked
 * @return returns non-zero if the address is a broadcast address
 */
uint8 ip_addr_isbroadcast(IP_ADDR *addr, NETIF *netif)
{
    uint32 addr2test;

    addr2test = addr->addr;
    /* all ones (broadcast) or all zeroes (old skool broadcast) */
    if ((~addr2test == IP_ADDR_ANY_VALUE) || (addr2test == IP_ADDR_ANY_VALUE))
        return 1;
    /* no broadcast support on this network interface? */
    else if ((netif->flags & NETIF_FLAG_BROADCAST) == 0)
        /* the given address cannot be a broadcast address
         * nor can we check against any broadcast addresses */
        return 0;
    /* address matches network interface address exactly? => no broadcast */
    else if (addr2test == netif->ip_addr.addr)
        return 0;
    /*  on the same (sub) network... */
	/* ...and host identifier bits are all ones? =>... */
	/* => network broadcast address */
    else if (ip_addr_netcmp(addr, &(netif->ip_addr), &(netif->netmask))
             && ((addr2test & ~ netif->netmask.addr) == (IP_ADDR_BROADCAST_VALUE & ~ netif->netmask.addr)))
        return 1;
    else
        return 0;
}



/**
 * Search the ARP table for a matching or new entry.
 * 
 * If an IP address is given, return a pending or stable ARP entry that matches
 * the address. If no match is found, create a new entry with this address set,
 * but in state ETHARP_EMPTY. The caller must check and possibly change the
 * state of the returned entry.
 * 
 * If ipaddr is NULL, return a initialized new entry in state ETHARP_EMPTY.
 * 
 * In all cases, attempt to create new entries from an empty entry. If no
 * empty entries are available and ETHARP_FLAG_TRY_HARD flag is set, recycle
 * old entries. Heuristic choose the least important entry for recycling.
 *
 * @param ipaddr IP address to find in ARP cache, or to add if not found.
 * @param flags @see definition of ETHARP_FLAG_*
 * @param netif netif related to this address (used for NETIF_HWADDRHINT)
 *  
 * @return The ARP entry index that matched or is created, ERR_MEM if no
 * entry is found or could be recycled.
 */
static int8 find_entry(IP_ADDR *ipaddr, uint8 flags)
{
    int8 old_pending = ARP_TABLE_SIZE, old_stable = ARP_TABLE_SIZE;
    int8 empty = ARP_TABLE_SIZE;
    uint8 i = 0, age_pending = 0, age_stable = 0;
    uint8 state;
	/* oldest entry with packets on queue */
	uint8 old_queue = ARP_TABLE_SIZE;
	/* its age */
	uint8 age_queue = 0;
    /**
     * a) do a search through the cache, remember candidates
     * b) select candidate entry
     * c) create new entry
     */

    /* a) in a single search sweep, do all of this
     * 1) remember the first empty entry (if any)
     * 2) remember the oldest stable entry (if any)
     * 3) remember the oldest pending entry without queued packets (if any)
     * 4) remember the oldest pending entry with queued packets (if any)
     * 5) search for a matching IP entry, either pending or stable
     *    until 5 matches, or all entries are searched for.
     */

    for (i = 0; i < ARP_TABLE_SIZE; i++)
    {
		state = arp_table[i].state;
        /* no empty entry found yet and now we do find one? */
        if ((empty == ARP_TABLE_SIZE) && (state == ETHARP_STATE_EMPTY))
        {
            /* remember first empty entry */
            empty = i;
        }
        /* stable entry? */
        else if (state != ETHARP_STATE_EMPTY)
        {
            /* if given, does IP address match IP address in ARP entry? */
            if (ipaddr && ip_addr_cmp(ipaddr, &arp_table[i].ipaddr))
            {
                /* found exact IP address match, simply bail out */
                return i;
            }
			/* pending entry? */
			if (state == ETHARP_STATE_PENDING)
			{
				/* pending with queued packets? */
				if (arp_table[i].q != NULL) 
				{
					if (arp_table[i].ctime >= age_queue) 
					{
						old_queue = i;
						age_queue = arp_table[i].ctime;
					}
				} 
				else
				{
					if (arp_table[i].ctime >= age_pending) 
					{
						old_pending = i;
						age_pending = arp_table[i].ctime;
					}
				}
			}
			else if(state == ETHARP_STATE_STABLE)
			{
				/* stable entry? */
				if (arp_table[i].ctime >= age_stable) 
				{
					old_stable = i;
					age_stable = arp_table[i].ctime;
				}
			}
		}
    }

	/* { we have no match } => try to create a new entry */

	/* don't create new entry, only search? */
	if (((flags & ETHARP_FLAG_FIND_ONLY) != 0) ||
	/* or no empty entry found and not allowed to recycle? */
		((empty == ARP_TABLE_SIZE) && ((flags & ETHARP_FLAG_TRY_HARD) == 0))) 
	{
		return (int8)ERR_MEM;
	}
    /* b) choose the least destructive entry to recycle:
	* 1) empty entry
	* 2) oldest stable entry
	* 3) oldest pending entry without queued packets
	* 4) oldest pending entry with queued packets
	* 
	* { ETHARP_FLAG_TRY_HARD is set at this point }
	*/ 

	/* 1) empty entry available? */
	if (empty < ARP_TABLE_SIZE)
	{
		i = empty;
	}
	else 
	{
		/* 2) found recyclable stable entry? */
		if (old_stable < ARP_TABLE_SIZE)
		{
			/* recycle oldest stable*/
			i = old_stable;
		}
		/* 3) found recyclable pending entry without queued packets? */
		else if (old_pending < ARP_TABLE_SIZE)
		{
			/* recycle oldest pending */
			i = old_pending;
		}
		/* 4) found recyclable pending entry with queued packets? */
		else if (old_queue < ARP_TABLE_SIZE)
		{
			/* recycle oldest pending (queued packets are free in free_entry) */
			i = old_queue;
		}
		else
		{
			/* no empty or recyclable entries found */
			return (int8)ERR_MEM;
		}
	}
	/* IP address given? */
	if (ipaddr != NULL) 
	{
		/* set IP address */
		ip_addr_copy(arp_table[i].ipaddr, *ipaddr);
	}
	arp_table[i].ctime = 0;

    return (uint8)i;
}

void arp_list_init(void)
{
	memset((void*)&arp_table[0], 0, sizeof(ETHARP_ENTRY) * ARP_TABLE_SIZE);
}

/** Clean up ARP table entries */
static void free_entry(int i)
{
	PBUF *q;
	PBUF *next;

	if (arp_table[i].q)
	{
		q = arp_table[i].q;
		while (next = q->next)
		{
			pbuf_free(q);
			q = next;
		}
		pbuf_free(q);
	}
	//arp_table[i].state = ETHARP_STATE_EMPTY;
	memset((void*)&arp_table[i], 0, sizeof(ETHARP_ENTRY));
}

void arp_entry_p_clr(void)
{
	int i;

	for (i = 0; i < ARP_TABLE_SIZE; i++)
	{
		arp_table[i].q = NULL;
	}
}


void arp_list_clr_init(void)
{
	int i = 0;
	
	for (i=0; i<ARP_TABLE_SIZE; i++)
	{
		free_entry(i);
	}
	return;
}

/**
 * Clears expired entries in the ARP table.
 *
 * This function should be called every ETHARP_TMR_INTERVAL milliseconds (5 seconds),
 * in order to expire entries in the ARP table.
 */
void etharp_tmr(void)
{
	uint8 i;

	/* remove expired entries from the ARP table */
	for (i = 0; i < ARP_TABLE_SIZE; ++i)
	{
		uint8 state = arp_table[i].state;
		if (state != ETHARP_STATE_EMPTY) 
		{
			arp_table[i].ctime++;
			if ((arp_table[i].ctime >= ARP_MAXAGE) ||
				((arp_table[i].state == ETHARP_STATE_PENDING) && (arp_table[i].ctime >= ARP_MAXPENDING))) 
			{
				/* pending or stable entry has become old! */
				/* clean up entries that have just been expired */
				printf("arp_table[%d] ctime:%d etharp_tmr\n", i, arp_table[i].ctime);
				free_entry(i);
			}
		}
	}
	return;
}

/**
 * Update (or insert) a IP/MAC address pair in the ARP cache.
 *
 * If a pending entry is resolved, any queued packets will be sent
 * at this point.
 *
 * @param ipaddr IP address of the inserted ARP entry.
 * @param ethaddr Ethernet address of the inserted ARP entry.
 * @param flags Defines behaviour:
 * - ETHARP_TRY_HARD Allows ARP to insert this as a new item. If not specified,
 * only existing ARP entries will be updated.
 *
 * @return
 * - ERR_OK Succesfully updated ARP cache.
 * - ERR_MEM If we could not add a new ARP entry when ETHARP_TRY_HARD was set.
 * - ERR_ARG Non-unicast address given, those will not appear in ARP cache.
 *
 * @see pbuf_free()
 */
static uint8 update_arp_entry(NETIF *netif, IP_ADDR *ipaddr, ETH_ADDR *ethaddr, uint8 flags)
{
    int8 i;
    uint8 k;
    PBUF * q;
	PBUF *tmp;

    if (ip_addr_isany(ipaddr) || 
		ip_addr_isbroadcast(ipaddr, netif) || 
		ip_addr_ismulticast(ipaddr))
    {
        return ERR_ARG;
    }
	
    /* find or create ARP entry */
    i = find_entry(ipaddr, flags); 
	
	MINDEBUG("find a arp cache num is %d", i);
    /* bail out if no entry could be found */
    if (i < 0)
    {
        return (uint8)i;
	}

    /* mark it stable */
    arp_table[i].state = ETHARP_STATE_STABLE;
    /* record network interface */
    arp_table[i].netif = netif;
	/* reset time stamp */
	arp_table[i].ctime = 0;
    /* update address */
    k = ETHARP_HWADDR_LEN;
    while (k-- > 0)
    {
        arp_table[i].ethaddr.addr[k] = ethaddr->addr[k];
    }

	q = arp_table[i].q;
	while (q)
	{
        printf("send ip packet for mac addr delay\n");
        etharp_send_ip(netif, q, (ETH_ADDR *)netif->hwaddr, &(arp_table[i].ethaddr));
        tmp = q;
        q = q->next;
        pbuf_free(tmp);
	}
	arp_table[i].q = NULL;
	
    return ERR_OK;
}

/**
 * Send an IP packet on the network using netif->linkoutput
 * The ethernet header is filled in before sending.
 *
 * @params netif the lwIP network interface on which to send the packet
 * @params p the packet to send, p->payload pointing to the (uninitialized) ethernet header
 * @params src the source MAC address to be copied into the ethernet header
 * @params dst the destination MAC address to be copied into the ethernet header
 * @return ERR_OK if the packet was sent, any other uint8 on failure
 */
static uint8 etharp_send_ip(NETIF *netif, PBUF *p, ETH_ADDR *src, ETH_ADDR *dst)
{
    ETH_HDR *ethhdr = p->payload;
    uint8 k;

    k = ETHARP_HWADDR_LEN;
    while (k-- > 0)
    {
        ethhdr->dest.addr[k] = dst->addr[k];
        ethhdr->src.addr[k]  = src->addr[k];
    }
    ethhdr->type = htons(ETHTYPE_IP);

    /* send the packet */
    return netif->linkoutput(netif, p);//low_level_output
}



/**
 * Send an ARP request for the given IP address and/or queue a packet.
 *
 * If the IP address was not yet in the cache, a pending ARP cache entry
 * is added and an ARP request is sent for the given address. The packet
 * is queued on this entry.
 *
 * If the IP address was already pending in the cache, a new ARP request
 * is sent for the given address. The packet is queued on this entry.
 *
 * If the IP address was already stable in the cache, and a packet is
 * given, it is directly sent and no ARP request is sent out.
 *
 * If the IP address was already stable in the cache, and no packet is
 * given, an ARP request is sent out.
 *
 * @param netif The lwIP network interface on which ipaddr
 * must be queried for.
 * @param ipaddr The IP address to be resolved.
 * @param q If non-NULL, a pbuf that must be delivered to the IP address.
 * q is not freed by this function.
 *
 * @note q must only be ONE packet, not a packet queue!
 *
 * @return
 * - ERR_BUF Could not make room for Ethernet header.
 * - ERR_MEM Hardware address unknown, and no more ARP entries available
 *   to query for address or queue the packet.
 * - ERR_MEM Could not queue packet due to memory shortage.
 * - ERR_RTE No route to destination (no gateway to external networks).
 * - ERR_ARG Non-unicast address given, those will not appear in ARP cache.
 *
 */
uint8 etharp_query(NETIF *netif, IP_ADDR *ipaddr, PBUF *q)
{
    ETH_ADDR * srcaddr = (ETH_ADDR *)netif->hwaddr;
    uint8 result = ERR_MEM;
    int8 i; /* ARP entry index */

    /* non-unicast address? */
    if (ip_addr_isbroadcast(ipaddr, netif) ||
            ip_addr_ismulticast(ipaddr) ||
            ip_addr_isany(ipaddr))
    {
        return ERR_ARG;
    }
    /* find entry in ARP cache, ask to create entry if queueing packet */
    i = find_entry(ipaddr, ETHARP_FLAG_TRY_HARD);
	//printf("fle-->find_entry\n");

    /* could not find or create entry? */
    if (i < 0)
    {
        return (uint8)i;
    }

    /* mark a fresh entry as pending (we just sent a request) */
	if (arp_table[i].state == ETHARP_STATE_EMPTY) 
	{
		arp_table[i].state = ETHARP_STATE_PENDING;
	}
	
	/* do we have a pending entry? or an implicit query request? */
	if ((arp_table[i].state == ETHARP_STATE_PENDING) || (q == NULL)) 
	{
		/* try to resolve it; send out ARP request */
		result = etharp_request(netif, ipaddr);
		if (result != ERR_OK) 
		{
			/* ARP request couldn't be sent */
			/* We don't re-send arp request in etharp_tmr, but we still queue packets,
			since this failure could be temporary, and the next packet calling
			etharp_query again could lead to sending the queued packets. */
		}
		if (q == NULL) 
		{
			return result;
		}
	}

	/* stable entry? */
	if (arp_table[i].state == ETHARP_STATE_STABLE)
	{
		/* send the packet */
		result = etharp_send_ip(netif, q, srcaddr, &(arp_table[i].ethaddr));
	}
	else if (arp_table[i].state == ETHARP_STATE_PENDING)
	{
		PBUF *temp;
		PBUF *buf;
	#ifdef DBUGPBUF
		printf("arp:");
	#endif
	//printf("fle-->arp\n");
		buf = pbuf_alloc(PBUF_PHY, q->len, PBUF_POOL);
		if (buf == NULL)
			return ERR_MEM;
		
		memcpy((char *)buf->payload, (char *)q->payload, q->len);
		//printf("fle-->memcpy\n");
        if (arp_table[i].q == NULL)
        {
          	arp_table[i].q = buf; 
        }
        else
        {
			temp = arp_table[i].q;
			while (temp->next != NULL)
			{
				temp = temp->next;
			}
			temp->next = buf;
        }
	}
//printf("fle-->return\n");
    return result;
}




/**
 * Responds to ARP requests to us. Upon ARP replies to us, add entry to cache
 * send out queued IP packets. Updates cache with snooped address pairs.
 *
 * Should be called for incoming ARP packets. The pbuf in the argument
 * is freed by this function.
 *
 * @param netif The lwIP network interface on which the ARP packet pbuf arrived.
 * @param ethaddr Ethernet address of netif.
 * @param p The ARP packet that arrived on netif. Is freed by this function.
 *
 * @return NULL
 *
 * @see pbuf_free()
 */
void etharp_arp_input(NETIF *netif, ETH_ADDR *ethaddr, PBUF *p)
{
    ETHARP_HDR *hdr;
    /* these are aligned properly, whereas the ARP header fields might not be */
    IP_ADDR sipaddr, dipaddr;
    uint8 i;
    uint8 for_us;
#if LWIP_AUTOIP
    const uint8 * ethdst_hwaddr;
#endif

    /* drop short ARP packets: we have to check for p->len instead of p->tot_len here
       since a struct etharp_hdr is pointed to p->payload, so it musn't be chained! */
    if (p->len < sizeof(ETHARP_HDR))
    {
        MINDEBUG("the arp packets size is too small = %04x\n", p->len);
        pbuf_free(p);
        return;
    }

    hdr = p->payload;

    /* RFC 826 "Packet Reception": */
    if ((hdr->hwtype != htons(HWTYPE_ETHERNET)) ||
        (hdr->_hwlen_protolen != htons((ETHARP_HWADDR_LEN << 8) | sizeof(IP_ADDR))) ||
        (hdr->proto != htons(ETHTYPE_IP)) ||
        (hdr->ethhdr.type != htons(ETHTYPE_ARP)))
    {

        MINDEBUG("the arp packets format err\n");
        pbuf_free(p);
        return;
    }

    /* We have to check if a host already has configured our random
     * created link local address and continously check if there is
     * a host with this IP-address so we can detect collisions */
#if LWIP_AUTOIP
	autoip_arp_reply(netif, hdr);
#endif
    
    /* Copy struct ip_addr2 to aligned ip_addr, to support compilers without
     * structure packing (not using structure copy which breaks strict-aliasing rules). */
    SMEMCPY(&sipaddr, &hdr->sipaddr, sizeof(sipaddr));
    SMEMCPY(&dipaddr, &hdr->dipaddr, sizeof(dipaddr));

    /* this interface is not configured? */
    if (netif->ip_addr.addr == 0)
    {
        for_us = 0;
    }
    else
    {
        /* ARP packet directed to us? */
        for_us = ip_addr_cmp(&dipaddr, &(netif->ip_addr));
    }

	/* ARP message directed to us?
	  -> add IP address in ARP cache; assume requester wants to talk to us,
	     can result in directly sending the queued packets for this host.
	 ARP message not directed to us?
	  ->  update the source IP address in the cache, if present */
	update_arp_entry(netif, &sipaddr, &(hdr->shwaddr), for_us ? ETHARP_FLAG_TRY_HARD : ETHARP_FLAG_FIND_ONLY);
	
    /* now act on the message itself */
    switch (htons(hdr->opcode))
    {
            /* ARP request? */
        case ARP_REQUEST:
            /* ARP request. If it asked for our address, we send out a
             * reply. In any case, we time-stamp any existing ARP entry,
             * and possiby send out an IP packet that was queued on it. */

            /* ARP request for our address? */
            if (for_us)
            {
                /* Re-use pbuf to send ARP reply.
                   Since we are re-using an existing pbuf, we can't call etharp_raw since
                   that would allocate a new pbuf. */
                hdr->opcode = htons(ARP_REPLY);
                hdr->dipaddr = hdr->sipaddr;
                SMEMCPY(&hdr->sipaddr, &netif->ip_addr, sizeof(hdr->sipaddr));

                i = ETHARP_HWADDR_LEN;
			#if LWIP_AUTOIP
                /* If we are using Link-Local, ARP packets must be broadcast on the
                 * link layer. (See RFC3927 Section 2.5) */
                ethdst_hwaddr = ((netif->autoip != NULL) && (((AUTO_IP *)(netif->autoip))->state != AUTOIP_STATE_OFF)) ? (uint8 *)(ethbroadcast.addr) : hdr->shwaddr.addr;
			#endif /* LWIP_AUTOIP */

                while (i-- > 0)
                {
                    hdr->dhwaddr.addr[i] = hdr->shwaddr.addr[i];
				#if LWIP_AUTOIP
                    hdr->ethhdr.dest.addr[i] = ethdst_hwaddr[i];
				#else 
                    hdr->ethhdr.dest.addr[i] = hdr->shwaddr.addr[i];
				#endif
                    hdr->shwaddr.addr[i] = ethaddr->addr[i];
                    hdr->ethhdr.src.addr[i] = ethaddr->addr[i];
                }
                p->len = 42;

                /* hwtype, hwaddr_len, proto, protolen and the type in the ethernet header
                   are already correct, we tested that before */
                   
                /* return ARP reply */
                netif->linkoutput(netif, p);//low_level_output
                /* we are not configured? */
            }
            else if (netif->ip_addr.addr == 0)
            {
                /* etharp_arp_input: we are unconfigured, ARP request ignored.
                   request was not directed to us */
            }
            else
            {
                /* etharp_arp_input: ARP request was not for us.  */
            }
            break;

        case ARP_REPLY:
			if (!netif_is_up())
			{
			#if (LWIP_DHCP && DHCP_DOES_ARP_CHECK)
			    /* DHCP wants to know about ARP replies from any host with an
			     * IP address also offered to us by the DHCP server. We do not
			     * want to take a duplicate IP address on a single network.
			     * @todo How should we handle redundant (fail-over) interfaces? */
			    dhcp_arp_reply(netif, &sipaddr);
			#endif
			}
            break;

        default:
			/* etharp_arp_input: ARP unknown opcode type.   */
            break;
    }
    /* free ARP packet */
    pbuf_free(p);
}


/**
 * Updates the ARP table using the given IP packet.
 *
 * Uses the incoming IP packet's source address to update the
 * ARP cache for the local network. The function does not alter
 * or free the packet. This function must be called before the
 * packet p is passed to the IP layer.
 *
 * @param netif The lwIP network interface on which the IP packet pbuf arrived.
 * @param p The IP packet that arrived on netif.
 *
 * @return NULL
 *
 * @see pbuf_free()
 */
#if ETHARP_TRUST_IP_MAC
void etharp_ip_input(NETIF *netif, PBUF *p)
{
    ETHIP_HDR *hdr;
	
    /* Only insert an entry if the source IP address of the
       incoming IP packet comes from a host on the local network. */
    hdr = p->payload;
    /* source is not on the local network? */
    if (!ip_addr_netcmp(&(hdr->ip.src), &(netif->ip_addr), &(netif->netmask)))
    {
        /* do nothing */
        MINDEBUG("ip address is not lan");
        return;
    }
    /* update ARP table */
    update_arp_entry(netif, &(hdr->ip.src), &(hdr->eth.src), ETHARP_FLAG_FIND_ONLY);
}
#endif


/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static PBUF *low_level_input(NETIF *netif)
{
	PBUF  *__pstPbuf = NULL;

	/*
	* now, it had get the interrupt,so to take care the command response.
	* read the receive data form wifi to user space buffer.
	*/    
    if (lwip_unread)
    {
         __pstPbuf = lwip_unread;
         lwip_unread = lwip_unread->next;
		 __pstPbuf->next = NULL;
    }
    else
    {
         return NULL;
    }
    return __pstPbuf;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */

void ethernetif_input(void)
{
    ETH_HDR  *ethhdr;
    PBUF  *p = NULL;
    NETIF *netif;
    uint32 timeout = 0;

    netif = &__stEMACNetif;

    //Get the data form EMAC loop read.
    while (1)
    {
		p = low_level_input(netif);
	    if (p == NULL)
	    {
	        break;
	    }
		
	    ethhdr = p->payload;
	    switch (htons(ethhdr->type))
	    {
	        case ETHTYPE_IP://this is a ip packet.
	        	//printf("get a ip packet\n");
				#if 0//dgl ETHARP_TRUST_IP_MAC
	            etharp_ip_input(netif, p);// update arp table. 
				#endif
	          	/* clear the Ethernet header.  */
	            if (pbuf_header(p, -sizeof(ETH_HDR)) == 0)
	            {
	                printf("0 == pbuf_header\n");
	                return;
	            }
				/*  it send message to tcpip task ip_input by tcpip_input.*/ 
	            netif->input(p, netif); //tcpip_input()
	            break;

	        case ETHTYPE_ARP://this is a arp packet.
	            etharp_arp_input(netif, (ETH_ADDR *)(&(netif->hwaddr)), p);
	            break;

	        default:
                printf("no p\n");
	            pbuf_free(p);
	            break;
	    }
    }

	/*make this term frames all been ask.*/
	tcp_ack_tail(tcp_curget());
	
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an uint8 value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static uint8 low_level_output(NETIF *netif, PBUF *p)
{
    uint8 __errReturn = ERR_OK;

    __errReturn = dhd_senddata(NULL, p->payload, p->len);
	
    return __errReturn;
}


/**
 * Resolve and fill-in Ethernet address header for outgoing IP packet.
 *
 * For IP multicast and broadcast, corresponding Ethernet addresses
 * are selected and the packet is transmitted on the link.
 *
 * For unicast addresses, the packet is submitted to etharp_query(). In
 * case the IP address is outside the local network, the IP address of
 * the gateway is used.
 *
 * @param netif The lwIP network interface which the IP packet will be sent on.
 * @param q The pbuf(s) containing the IP packet to be sent.
 * @param ipaddr The IP address of the packet destination.
 *
 * @return
 * - ERR_RTE No route to destination (no gateway to external networks),
 * or the return type of either etharp_query() or etharp_send_ip().
 */
uint8 etharp_output(NETIF *netif, PBUF *q, IP_ADDR *ipaddr)
{
    ETH_ADDR *dest, mcastaddr;
    int i;

    if (pbuf_header(q, PBUF_ETHNET_HLEN) == 0)
        return ERR_MEM;
//printf("fle->etharp_ouput\n");

    /* assume unresolved Ethernet address */

    /* Determine on destination hardware address. Broadcasts and multicasts
     * are special, other IP addresses are looked up in the ARP table. */

    /* broadcast destination IP address? */
    if (ip_addr_isbroadcast(ipaddr, netif))
    {
        /* broadcast on Ethernet also */
        dest = (ETH_ADDR *)&ethbroadcast;
    }
    else if (ip_addr_ismulticast(ipaddr)) /* multicast destination IP address? */
    {
        /* Hash IP multicast address to MAC address.*/
        mcastaddr.addr[0] = 0x01;
        mcastaddr.addr[1] = 0x00;
        mcastaddr.addr[2] = 0x5e;
        mcastaddr.addr[3] = ip4_addr2(ipaddr) & 0x7f;
        mcastaddr.addr[4] = ip4_addr3(ipaddr);
        mcastaddr.addr[5] = ip4_addr4(ipaddr);
        /* destination Ethernet address is multicast */
        dest = &mcastaddr;
    }
    else /* unicast destination IP address? */
    {
        /* outside local network? */
        if (!ip_addr_netcmp(ipaddr, &(netif->ip_addr), &(netif->netmask)))
        {
            /* interface has default gateway? */
            if (netif->gw.addr != 0)
            {
                /* send to hardware address of default gateway IP address */
                ipaddr = &(netif->gw);
                /* no default gateway available */
            }
            else
            {
                /* 
                   no route to destination error (default gateway missing),
                   outside net and no route gateway,so error happen!
				*/
                return ERR_RTE;
            }
        }
        /* queue on destination Ethernet address belonging to ipaddr */
		//printf("fle->query\n");
        return etharp_query(netif, ipaddr, q);
    }

    /* continuation for multicast/broadcast destinations */
    /* obtain source Ethernet address of the given interface */
    /* send packet directly on the link */
	//printf("fle->send ip\n");
    return etharp_send_ip(netif, q, (ETH_ADDR*)(netif->hwaddr), dest);
}


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(NETIF *netif)
{
    uint8 * pmac = NULL;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    pmac = wifi_get_mac_addr(NULL);
    memcpy((char *)netif->hwaddr, pmac, ETHARP_HWADDR_LEN);
	
    MINDEBUG("mac:%02x-%02x-%02x-%02x-%02x-%02x",
	          netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2], 
	          netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
}



/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other uint8 on error
 */
uint8 ethernetif_init(NETIF *netif)
{
    /* Initialize interface hostname */
#if LWIP_NETIF_HOSTNAME
    netif->hostname = "lwip";
#else
    netif->hostname = NULL;
#endif

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

/**
 * Pass a received packet to tcpip_thread for input processing
 *
 * @param p the received packet, p->payload pointing to the Ethernet header or
 *          to an IP header (if netif doesn't got NETIF_FLAG_ETHARP flag)
 * @param inp the network interface on which the packet was received
 */
uint8 tcpip_input(PBUF *p, NETIF *netif)
{
    ip_input(p, netif); 
}


/**
 * Add a network interface to the list of lwIP netifs.
 *
 * @param netif a pre-allocated netif structure
 * @param ipaddr IP address for the new netif
 * @param netmask network mask for the new netif
 * @param gw default gateway IP address for the new netif
 * @param state opaque data passed to the new netif
 * @param init callback function that initializes the interface
 * @param input callback function that is called to pass
 * ingress packets up in the protocol layer stack.
 *
 * @return netif, or NULL if failed.
 */
NETIF *netif_add(IP_ADDR *ipaddr, IP_ADDR *netmask, IP_ADDR *gw)
{

    NETIF *netif;
	static uint8 netifnum = 0;

    netif = &__stEMACNetif;
	memset(netif, 0, sizeof(NETIF));
	
    /* reset new interface configuration state */
    netif->ip_addr.addr = 0;
    netif->netmask.addr = 0;
    netif->gw.addr = 0;
    netif->flags = 0;

#if LWIP_DHCP
    /* netif not under DHCP control by default */
    netif->dhcp = NULL;
#endif /* LWIP_DHCP */

#if LWIP_AUTOIP
    /* netif not under AutoIP control by default */
    netif->autoip = NULL;
#endif /* LWIP_AUTOIP */

    /* remember netif specific state information data */
    netif->num = netifnum++;
    netif->input = tcpip_input;

    netif_set_addr(netif, ipaddr, netmask, gw);
    MINDEBUG("netif ip addr = %d.%d.%d.%d",  ((IP_ADDR3 *)ipaddr)->addrb[0],
                                           ((IP_ADDR3 *)ipaddr)->addrb[1],
                                           ((IP_ADDR3 *)ipaddr)->addrb[2],
                                           ((IP_ADDR3 *)ipaddr)->addrb[3]);

    /* call user specified initialization function for netif */
    if (ethernetif_init(netif) != ERR_OK)
        return NULL;

    pbuf_init();
	
    return netif;
}

/**
 * Change IP address configuration for a network interface (including netmask
 * and default gateway).
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 * @param netmask the new netmask
 * @param gw the new default gateway
 */
void
netif_set_addr(NETIF *netif, IP_ADDR *ipaddr, IP_ADDR *netmask, IP_ADDR *gw)
{
    netif_set_ipaddr(netif, ipaddr);
    netif_set_netmask(netif, netmask);
    netif_set_gw(netif, gw);
}

void netif_remove(void)
{	
	int i;
	
	lwip_unread = NULL;
	
	for (i=0; i<ARP_TABLE_SIZE; i++)
	{
		arp_table[i].state = ETHARP_STATE_EMPTY;
	}

	pbuf_init();
}

/**
 * Change the IP address of a network interface
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 *
 * @note call netif_set_addr() if you also want to change netmask and
 * default gateway
 */
void
netif_set_ipaddr(NETIF *netif, IP_ADDR *ipaddr)
{
    /* TODO: Handling of obsolete pcbs */
    /* See:  http://mail.gnu.org/archive/html/lwip-users/2003-03/msg00118.html */

    /* set new IP address to netif */
    ip_addr_set(&(netif->ip_addr), ipaddr);

}

/**
 * Change the default gateway for a network interface
 *
 * @param netif the network interface to change
 * @param gw the new default gateway
 *
 * @note call netif_set_addr() if you also want to change ip address and netmask
 */
void
netif_set_gw(NETIF *netif, IP_ADDR *gw)
{
    ip_addr_set(&(netif->gw), gw);    
}

/**
 * Change the netmask of a network interface
 *
 * @param netif the network interface to change
 * @param netmask the new netmask
 *
 * @note call netif_set_addr() if you also want to change ip address and
 * default gateway
 */
void
netif_set_netmask(NETIF *netif, IP_ADDR *netmask)
{
    /* set new netmask to netif */
    ip_addr_set(&(netif->netmask), netmask);
}


/**
 * Bring an interface up, available for processing
 * traffic.
 * 
 * @note: Enabling DHCP on a down interface will make it come
 * up once configured.
 * 
 * @see dhcp_start()
 */ 
void netif_set_up(void)
{
	NETIF *netif = &__stEMACNetif;

	if (!(netif->flags & NETIF_FLAG_UP ))
	{
		netif->flags |= NETIF_FLAG_UP;
	}
}

/**
 * Bring an interface down, disabling any traffic processing.
 *
 * @note: Enabling DHCP on a down interface will make it come
 * up once configured.
 * 
 * @see dhcp_start()
 */ 
void netif_set_down(void)
{
	NETIF *netif = &__stEMACNetif;

	if (netif->flags & NETIF_FLAG_UP )
	{
		netif->flags &= ~NETIF_FLAG_UP;
	}
}

/**
 * Ask if an interface is up
 */ 
uint8 netif_is_up(void)
{
  NETIF *netif = &__stEMACNetif;
  
  return (netif->flags & NETIF_FLAG_UP)?1:0;
}


