/**
 * @file
 * This is the IPv4 layer implementation for incoming and outgoing IP traffic.
 *
 * @see ip_frag.c
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
#include "ip.h"
#include "inet.h"
#include "netif.h"
#include "udp.h"
#include "tcp.h"
#include "icmp.h"
#include "dhcp.h"

#include <string.h>

/**
 * Finds the appropriate network interface for a given IP address. It
 * searches the list of network interfaces linearly. A match is found
 * if the masked IP address of the network interface equals the masked
 * IP address given to the function.
 *
 * @param dest the destination IP address for which to find the route
 * @return the netif on which to send to reach dest
 */
NETIF * ip_route(IP_ADDR *dest)
{
    NETIF *netif;

    netif = &__stEMACNetif;

   /* return netif on which to forward IP packet */
   return netif;
}


/**
 * This function is called by the network interface device driver when
 * an IP packet is received. The function does the basic checks of the
 * IP header such as packet size being at least larger than the header
 * size etc. If the packet was not destined for us, the packet is
 * forwarded (using ip_forward). The IP checksum is always checked.
 *
 * Finally, the packet is sent to the upper layer protocol input function.
 *
 * @param p the received IP packet (p->payload points to IP header)
 * @param inp the netif on which this packet was received
 * @return ERR_OK if the packet was processed (could return ERR_* if it wasn't
 *         processed, but currently always returns ERR_OK)
 */
uint8 ip_input(PBUF *p, NETIF *inp)
{
    IP_HDR *iphdr;
    NETIF *netif;
    uint16 iphdr_hlen;
    uint16 iphdr_len;

    /* identify the IP header */
    iphdr = p->payload;
    if (IPH_V(iphdr) != 4)
    {
        pbuf_free(p);//this packet is not ipv4
        return ERR_OK;
    }

    /* obtain IP header length in number of 32-bit words */
    iphdr_hlen = IPH_HL(iphdr);
    /* calculate IP header length in bytes */
    iphdr_hlen *= 4;
	
    /* obtain ip pocket total length in bytes */
    iphdr_len = ntohs(IPH_LEN(iphdr));

    /* header length exceeds first pbuf length, or ip length exceeds total pbuf length? */
    if ((iphdr_hlen > p->len) || (iphdr_len > p->len))
    {
        /* free (drop) packet pbufs */
        pbuf_free(p);
        return ERR_OK;
    }

    if (inet_chksum(iphdr, iphdr_hlen) != 0)
    {    
        pbuf_free(p);//ip header checksum error!
        return ERR_OK;
    }
    
    netif = inp;

    /* interface is up and configured? */
    if (ip_addr_isany(&(netif->ip_addr)))
    {
        netif = NULL;
    }
    else
    {
        /* unicast to this interface address? */
        if (!ip_addr_cmp(&(iphdr->dest), &(netif->ip_addr)))
        {
            netif = NULL;
        }
    }

    /* packet consists of multiple fragments? */
    if ((IPH_OFFSET(iphdr) & htons(IP_OFFMASK | IP_MF)) != 0)
    {
		printf("received splited IP packet!! \n");
        pbuf_free(p);
        return ERR_OK;
    }
    if (iphdr_hlen > IP_HLEN)
    {
        pbuf_free(p);
        return ERR_OK;
    }
	
    switch (IPH_PROTO(iphdr))
    {
        case IP_PROTO_TCP://(6)
            //printf("get a tcp packet\n");
            tcp_input(p, inp);
            break;

        case IP_PROTO_UDP://(17)
        	printf("UDP\n");
            udp_input(p, inp);
            break;

        case IP_PROTO_ICMP://(1)
        	printf("gICMP\n");
            icmp_input(p, inp);
            break;

        default:
            /* send ICMP destination protocol unreachable unless is was a broadcast */
            printf("gerr\n");
            if (!ip_addr_isbroadcast(&(iphdr->dest), inp) && !ip_addr_ismulticast(&(iphdr->dest)))
            {
                p->payload = iphdr;
                icmp_dest_unreach(p, ICMP_DUR_PROTO);
            }
            pbuf_free(p);
    }

    return ERR_OK;
}

/**
 * Sends an IP packet on a network interface. This function constructs
 * the IP header and calculates the IP header checksum. If the source
 * IP address is NULL, the IP address of the outgoing network
 * interface is filled in as source address.
 * If the destination IP address is IP_HDRINCL, p is assumed to already
 * include an IP header and p->payload points to it instead of the data.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
            protocol header; if dest == IP_HDRINCL, p already includes an IP
            header and p->payload points to that IP header)
 * @param src the source IP address to send from (if src == IP_ADDR_ANY, the
 *         IP  address of the netif used to send is used as source address)
 * @param dest the destination IP address to send the packet to
 * @param ttl the TTL value to be set in the IP header
 * @param tos the TOS value to be set in the IP header
 * @param proto the PROTOCOL to be set in the IP header
 * @param netif the netif on which to send this packet
 * @return ERR_OK if the packet was sent OK
 *         ERR_BUF if p doesn't have enough space for IP/LINK headers
 *         returns errors returned by netif->output
 *
 * @note ip_id: RFC791 "some host may be able to simply use
 *  unique identifiers independent of destination"
 */
uint8 ip_output_if(PBUF *p, IP_ADDR *src, IP_ADDR *dest,
             uint8 ttl, uint8 tos,
             uint8 proto, NETIF *netif)
{
    IP_HDR *iphdr;
    static uint16 ip_id = 0;
    uint8 ret;
	
    /* Should the IP header be generated or is it already included in p? */
    if (dest != IP_HDRINCL)
    {
        uint16 ip_hlen = IP_HLEN;

        if (p != NULL)
        {
          	pbuf_header(p, IP_HLEN);
        }

        iphdr = p->payload;

        IPH_TTL_SET(iphdr, ttl);
        IPH_PROTO_SET(iphdr, proto);

        ip_addr_set(&(iphdr->dest), dest);

        IPH_VHLTOS_SET(iphdr, 4, ip_hlen / 4, tos);
        IPH_LEN_SET(iphdr, htons(p->len));
        IPH_OFFSET_SET(iphdr, 0);
        IPH_ID_SET(iphdr, htons(ip_id));
        ++ip_id;

        if (ip_addr_isany(src))
        {
            ip_addr_set(&(iphdr->src), &(netif->ip_addr));
        }
        else
        {
            ip_addr_set(&(iphdr->src), src);
        }

        IPH_CHKSUM_SET(iphdr, 0);
		#if CHECKSUM_GEN_IP
        	IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, ip_hlen));  //wp
		#endif
    }
    else
    {
        /* IP header already included in p */
        iphdr = p->payload;
        dest = &(iphdr->dest);
    }
//printf("fle--> nefif output\n");
    ret =  netif->output(netif, p, dest);  // etharp_output  
    return ret;
}

/**
 * Simple interface to ip_output_if. It finds the outgoing network
 * interface and calls upon ip_output_if to do the actual work.
 *
 * @param p the packet to send (p->payload points to the data, e.g. next
            protocol header; if dest == IP_HDRINCL, p already includes an IP
            header and p->payload points to that IP header)
 * @param src the source IP address to send from (if src == IP_ADDR_ANY, the
 *         IP  address of the netif used to send is used as source address)
 * @param dest the destination IP address to send the packet to
 * @param ttl the TTL value to be set in the IP header
 * @param tos the TOS value to be set in the IP header
 * @param proto the PROTOCOL to be set in the IP header
 *
 * @return ERR_RTE if no route is found
 *         see ip_output_if() for more return values
 */
uint8 ip_output(PBUF *p, IP_ADDR *src, IP_ADDR *dest, uint8 ttl, uint8 tos, uint8 proto)
{
    NETIF *netif;

	netif = ip_route(dest);
	{
		TCP_HDR *tcphdr;
		tcphdr = (TCP_HDR *)p->payload;
		//printf("fle-->send: seq=%010u nxt:%010u len:%3u wnd:%u\n", ntohl(tcphdr->seqno), ntohl(tcphdr->ackno), p->tcplen, ntohs(tcphdr->wnd));
	}

    return ip_output_if(p, src, dest, ttl, tos, proto, netif);
}
