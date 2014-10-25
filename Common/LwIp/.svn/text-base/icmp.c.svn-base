/**
 * @file
 * ICMP - Internet Control Message Protocol
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

/* Some ICMP messages should be passed to the transport protocols. This
   is not implemented. */

#include "sdio_card.h"
#include "lwipopt.h"
#include "def.h"
#include "err.h"

#include "pbuf.h"
#include "icmp.h"
#include "inet.h"
#include "inet_chksum.h"
#include "ip.h"
#include "ip_addr.h"

#include <string.h>


/* The amount of data from the original packet to return in a dest-unreachable */
#define ICMP_DEST_UNREACH_DATASIZE 8

static void icmp_send_response(PBUF *p, uint8 type, uint8 code);

/**
 * Processes ICMP input packets, called from ip_input().
 *
 * Currently only processes icmp echo requests and sends
 * out the echo response.
 *
 * @param p the icmp echo request packet, p->payload pointing to the ip header
 * @param inp the netif on which this packet was received
 */
void icmp_input(PBUF *p, NETIF *inp)
{
	uint8 type;
	ICMP_ECHO_HDR *iecho;
	IP_HDR *iphdr;
	IP_ADDR tmpaddr;
	int16 hlen;

	iphdr = p->payload;
	hlen = IPH_HL(iphdr) * 4;

	if (pbuf_header(p, -hlen) == 0)
		return;

	type = *((uint8 *)p->payload);
	iecho = p->payload;
	printf("icmp_input %d %d\n", iecho->type, iecho->code);
	switch (type)
	{
		case ICMP_ECHO:
		{
			int accepted = 1;

			/* multicast destination address? */
			if (ip_addr_ismulticast(&iphdr->dest)) 
			{
				accepted = 0;
			}

			/* broadcast destination address? */
			if (ip_addr_isbroadcast(&iphdr->dest, inp)) 
			{
				accepted = 0;
			}

			/* broadcast or multicast destination address not acceptd? */
			if (!accepted) 
			{       
				pbuf_free(p);
				return;
			}

			if (p->len < sizeof(ICMP_ECHO_HDR))
				goto lenerr;

			if (inet_chksum_pbuf(p) != 0)
			{
				pbuf_free(p);
				return;
			}

			/* At this point, all checks are OK. */
			/* We generate an answer by switching the dest and src ip addresses,
			* setting the icmp type to ECHO_RESPONSE and updating the checksum. */
			tmpaddr.addr = iphdr->src.addr;
			iphdr->src.addr = iphdr->dest.addr;
			iphdr->dest.addr = tmpaddr.addr;

			iecho = p->payload;
			ICMPH_TYPE_SET(iecho, ICMP_ER);

			/* adjust the checksum */
			if (iecho->chksum >= htons(0xffff - (ICMP_ECHO << 8)))
			{
				iecho->chksum += htons(ICMP_ECHO << 8) + 1;
			}
			else
			{
				iecho->chksum += htons(ICMP_ECHO << 8);
			}
			if (pbuf_header(p, hlen) == 0)
				return;

			ip_output_if(p, &(iphdr->src), IP_HDRINCL, ICMP_TTL, 0, IP_PROTO_ICMP, inp);
			break;
		}

		default:
			break;
	}
  
lenerr:
	pbuf_free(p);

	return;
}

/**
 * Send an icmp 'destination unreachable' packet, called from ip_input() if
 * the transport layer protocol is unknown and from udp_input() if the local
 * port is not bound.
 *
 * @param p the input packet for which the 'unreachable' should be sent,
 *          p->payload pointing to the IP header
 * @param t type of the 'unreachable' packet
 */
void icmp_dest_unreach(PBUF *p, enum icmp_dur_type t)
{
	icmp_send_response(p, ICMP_DUR, t);
}

#if IP_FORWARD || IP_REASSEMBLY
/**
 * Send a 'time exceeded' packet, called from ip_forward() if TTL is 0.
 *
 * @param p the input packet for which the 'time exceeded' should be sent,
 *          p->payload pointing to the IP header
 * @param t type of the 'time exceeded' packet
 */
void icmp_time_exceeded(PBUF *p, enum icmp_te_type t)
{
	icmp_send_response(p, ICMP_TE, t);
}

#endif /* IP_FORWARD || IP_REASSEMBLY */

/**
 * Send an icmp packet in response to an incoming packet.
 *
 * @param p the input packet for which the 'unreachable' should be sent,
 *          p->payload pointing to the IP header
 * @param type Type of the ICMP header
 * @param code Code of the ICMP header
 */ 
static void icmp_send_response(PBUF *p, uint8 type, uint8 code)
{
	PBUF *q;
	IP_HDR *iphdr;
	/* we can use the echo header here */
	ICMP_ECHO_HDR *icmphdr;

	/* ICMP header + IP header + 8 bytes of data */
#ifdef DBUGPBUF
	printf("ic:");
#endif
	q = pbuf_alloc(PBUF_IP, sizeof(ICMP_ECHO_HDR) + IP_HLEN + ICMP_DEST_UNREACH_DATASIZE, PBUF_POOL);
	if (q == NULL)
	{
		return;
	}

	iphdr = p->payload;

	icmphdr = q->payload;
	icmphdr->type = type;
	icmphdr->code = code;
	icmphdr->id = 0;
	icmphdr->seqno = 0;

	/* copy fields from original packet */
	SMEMCPY((uint8 *)q->payload + sizeof(ICMP_ECHO_HDR), (uint8 *)p->payload, IP_HLEN + ICMP_DEST_UNREACH_DATASIZE);

	/* calculate checksum */
	icmphdr->chksum = 0;
	icmphdr->chksum = inet_chksum(icmphdr, q->len);
	ip_output(q, NULL, &(iphdr->src), ICMP_TTL, 0, IP_PROTO_ICMP);printf("icmp ip_output\n");
	pbuf_free(q);
}

