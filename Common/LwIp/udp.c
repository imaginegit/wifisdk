/**
 * @file
 * User Datagram Protocol module
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


/* udp.c
 *
 * The code for the User Datagram Protocol UDP & UDPLite (RFC 3828).
 *
 */

/* @todo Check the use of '(struct udp_pcb).chksum_len_rx'!
 */

#include "sdio_card.h"
#include "lwipopt.h"
#include "def.h"
#include "err.h"
#include "udp.h"
#include "inet.h"
#include "inet_chksum.h"
#include "ip_addr.h"
#include "netif.h"
#include "icmp.h"
#include "dhcp.h"
#include "dns.h"

#include <string.h>

/* The list of UDP PCBs */
/* exported in udp.h (was static) */
#define UDPPCBNUM    3
UDP_PCB *udp_pcbs;
static UDP_PCB udppcb[UDPPCBNUM];

void udp_pcb_init(void)
{
	memset((void*)&udppcb[0], 0, sizeof(UDP_PCB)*UDPPCBNUM);
}
/**
 * Process an incoming UDP datagram.
 *
 * Given an incoming UDP datagram (as a chain of pbufs) this function
 * finds a corresponding UDP PCB and hands over the pbuf to the pcbs
 * recv function. If no pcb is found or the datagram is incorrect, the
 * pbuf is freed.
 *
 * @param p pbuf to be demultiplexed to a UDP PCB.
 * @param inp network interface on which the datagram was received.
 *
 */
void udp_input(PBUF *p, NETIF *inp)
{
    UDP_HDR *udphdr;
    UDP_PCB *pcb;
    IP_HDR *iphdr;
    uint16 src, dest, datalen;
    uint8 broadcast;
	int i;

    iphdr = p->payload;

//printf("fle--> len = %d, %d\n", p->len, ntohs((iphdr)->_v_hl_tos));

    /* Check minimum length (IP header + UDP header)
     * and move payload pointer to UDP header */
    if (p->len < (IPH_HL(iphdr) * 4 + UDP_HLEN) || !pbuf_header(p, -(int16)(IPH_HL(iphdr) * 4)))
    {
        /* drop short packets */
		//printf("fle-->udp_input\n");
        pbuf_free(p);
        return;
    }

    udphdr = (UDP_HDR *)p->payload;

    /* is broadcast packet ? */
	//printf("broadcast = %x\n", iphdr->dest);
    broadcast = ip_addr_isbroadcast(&(iphdr->dest), inp);

    /* convert src and dest ports to host byte order */
    src = ntohs(udphdr->src);
    dest = ntohs(udphdr->dest);
    datalen = ntohs(udphdr->len);
	//printf("udp src:%d dest:%d len:%d ip=%08x.\n", src, dest, datalen, iphdr->dest.addr);
	//for(i = 0; i < 34 + datalen;i++) {
	//printf("%02x ", p->buf[i]);
	//}
//printf("\n");
//udp_send(udp_pcbs, p);
    pcb = NULL;
    /* when LWIP_DHCP is active, packets to DHCP_CLIENT_PORT may only be processed by
       the dhcp module, no other UDP pcb may use the local UDP port DHCP_CLIENT_PORT */
    if (dest == DHCP_CLIENT_PORT)
    {
        /* all packets for DHCP_CLIENT_PORT not coming from DHCP_SERVER_PORT are dropped! */
        if (src == DHCP_SERVER_PORT)
        {
            if ((inp->dhcp != NULL) && (inp->dhcp->pcb != NULL))
            {
                /* accept the packe if
                   (- broadcast or directed to us) -> DHCP is link-layer-addressed, local ip is always ANY!
                   - inp->dhcp->pcb->remote == ANY or iphdr->src */
                if ((ip_addr_isany(&inp->dhcp->pcb->remote_ip) ||
                        ip_addr_cmp(&(inp->dhcp->pcb->remote_ip), &(iphdr->src))))
                {
                    pcb = inp->dhcp->pcb;
                }
            }
        }
    }
    else if(src == DNS_SERVER_PORT)
    {
        pcb = &udppcb[1];//dgl dns use here..
        /* Iterate through the UDP pcb list for a matching pcb.
         * 'Perfect match' pcbs (connected to the remote port & ip address) are
         * preferred. If no perfect match is found, the first unconnected pcb that
         * matches the local port and ip address gets the datagram. */

        /* compare PCB local addr+port to UDP destination addr+port */
        if ((pcb->local_port == dest) &&
                ((!broadcast && ip_addr_isany(&pcb->local_ip)) ||
                 ip_addr_cmp(&(pcb->local_ip), &(iphdr->dest)) ||
                 (broadcast)))
        {

            ;
        }
		else
		{
			pcb = NULL;
		}
    }
	else if(dest == USER_CLIENT_PORT ||src == USER_SERVER_PORT) {
		//printf("fle-->pcb user\n");
		pcb = &udppcb[2];
		//printf("fle-->addr = %d\n", pcb);
		//printf("fle-->prot = %d, dest = %d\n", pcb->local_port, dest);
		if ((pcb->local_port == dest) &&
                ((!broadcast && ip_addr_isany(&pcb->local_ip)) ||
                 ip_addr_cmp(&(pcb->local_ip), &(iphdr->dest)) ||
                 (broadcast)))
                {
                    //printf("fle-->ok\n");
                } else {
                	pcb = NULL;
			//printf("fle-->nok\n");
                }
	}

    /* Check checksum if this is a match or if it was directed at us. */
    if (pcb != NULL || ip_addr_cmp(&inp->ip_addr, &iphdr->dest))
    {
        if (udphdr->chksum != 0)
        {
            if (inet_chksum_pseudo(p, (IP_ADDR *)&(iphdr->src),
                                   (IP_ADDR *)&(iphdr->dest),
                                   IP_PROTO_UDP, datalen) != 0) //wp  p->len ---> datalen
            {
            //printf("fle-->free\n");
                pbuf_free(p);
                return;
            }
        }
    }

    if (!pbuf_header(p, -UDP_HLEN))
    {
        /* Can we cope with this failing? Just assert for now */
		//printf("fle-->free header\n");
        pbuf_free(p);
        return;
    }
	
    if (pcb != NULL)
    {
        /* callback */
        if (pcb->recv != NULL)
        {
            /* now the recv function is responsible for freeing p */
            pcb->recv(pcb->recv_arg, pcb, p, &(iphdr->src), src);
        }
        else
        {
            /* no recv function registered? then we have to free the pbuf! */
			//printf("fle-->free header1\n");
            pbuf_free(p);
            return;
        }
    }
    else
    {
        /* No match was found, send ICMP destination port unreachable unless
           destination address was broadcast/multicast. */
        if (!broadcast && !ip_addr_ismulticast(&iphdr->dest))
        {
            /* move payload pointer back to ip header */
			printf("fle-->broadcast\n");
            pbuf_header(p, (IPH_HL(iphdr) * 4) + UDP_HLEN);
            icmp_dest_unreach(p, ICMP_DUR_PORT);
        }
//		for(i = 0; i < 34 + datalen;i++) {
//	printf("%02x ", p->buf[i]);
//	}
//printf("\n");
		//printf("fle-->free header2\n");
        pbuf_free(p);
    }
	
	return;
}


/**
 * Send data using UDP.
 *
 * @param pcb UDP PCB used to send the data.
 * @param p chain of pbuf's to be sent.
 *
 * The datagram will be sent to the current remote_ip & remote_port
 * stored in pcb. If the pcb is not bound to a port, it will
 * automatically be bound to a random port.
 *
 * @return lwIP error code.
 * - ERR_OK. Successful. No error occured.
 * - ERR_MEM. Out of memory.
 * - ERR_RTE. Could not find route to destination address.
 * - More errors could be returned by lower protocol layers.
 *
 * @see udp_disconnect() udp_sendto()
 */
uint8 udp_send(UDP_PCB *pcb, PBUF *p)
{
    /* send to the packet using remote ip and port stored in the pcb */   
    return udp_sendto(pcb, p, &pcb->remote_ip, pcb->remote_port);
}

/**
 * Send data to a specified address using UDP.
 *
 * @param pcb UDP PCB used to send the data.
 * @param p chain of pbuf's to be sent.
 * @param dst_ip Destination IP address.
 * @param dst_port Destination UDP port.
 *
 * dst_ip & dst_port are expected to be in the same byte order as in the pcb.
 *
 * If the PCB already has a remote address association, it will
 * be restored after the data is sent.
 *
 * @return lwIP error code (@see udp_send for possible error codes)
 *
 * @see udp_disconnect() udp_send()
 */
uint8 udp_sendto(UDP_PCB *pcb, PBUF *p, IP_ADDR *dst_ip, uint16 dst_port)
{
    return udp_sendto_if(pcb, p, dst_ip, dst_port, ip_route(dst_ip));
}

/**
 * Send data to a specified address using UDP.
 * The netif used for sending can be specified.
 *
 * This function exists mainly for DHCP, to be able to send UDP packets
 * on a netif that is still down.
 *
 * @param pcb UDP PCB used to send the data.
 * @param p chain of pbuf's to be sent.
 * @param dst_ip Destination IP address.
 * @param dst_port Destination UDP port.
 * @param netif the netif used for sending.
 *
 * dst_ip & dst_port are expected to be in the same byte order as in the pcb.
 *
 * @return lwIP error code (@see udp_send for possible error codes)
 *
 * @see udp_disconnect() udp_send()
 */
uint8 udp_sendto_if(UDP_PCB *pcb, PBUF *p, IP_ADDR *dst_ip, uint16 dst_port, NETIF *netif)
{
    UDP_HDR *udphdr;
    IP_ADDR *src_ip;
    uint8 err;
    //printf("uif --> %d\n", pcb->local_port);
    /* if the PCB is not yet bound to a port, bind it here */
    if (pcb->local_port == 0)
    {
    //printf("fle-->pcb = %d, %d\n", pcb, pcb->local_port);
        err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
        if (err != ERR_OK)
        {
            return err;
        }
    }
	//printf("fle-----> local ip: %d, local port: %d, desport: %d, ttl: %d, tos: %d\n", pcb->local_ip.addr, pcb->local_port, dst_port, pcb->ttl, pcb->tos);

    pbuf_header(p, UDP_HLEN);

    /* q now represents the packet to be sent */
    udphdr = p->payload;
    udphdr->src = htons(pcb->local_port);
    udphdr->dest = htons(dst_port);
    /* in UDP, 0 checksum means 'no checksum' */
    udphdr->chksum = 0x0000;

    /* PCB local address is IP_ANY_ADDR? */
    if (ip_addr_isany(&pcb->local_ip))
    {
        /* use outgoing network interface IP address as source address */
        src_ip = &(netif->ip_addr);
    }
    else
    {
        /* check if UDP PCB local IP address is correct
         * this could be an old address if netif->ip_addr has changed */
        if (!ip_addr_cmp(&(pcb->local_ip), &(netif->ip_addr)))
        {
            pbuf_free(p);
			return ERR_VAL;
        }
        /* use UDP PCB local IP address as source address */
        src_ip = &(pcb->local_ip);
    }
	
	/* UDP */
    udphdr->len = htons(p->len);
	
    /* calculate checksum */
    if ((pcb->flags & UDP_FLAGS_NOCHKSUM) == 0)
    {
        udphdr->chksum = inet_chksum_pseudo(p, src_ip, dst_ip, IP_PROTO_UDP, p->len);
        /* chksum zero must become 0xffff, as zero means 'no checksum' */
        if (udphdr->chksum == 0x0000) 
			udphdr->chksum = 0xffff;
    }
	//printf("fle-->ip_out\n");
    err = ip_output_if(p, src_ip, dst_ip, pcb->ttl, pcb->tos, IP_PROTO_UDP, netif);
	//printf("fle-->r %d\n", err);

    return err;
}

/**
 * Bind an UDP PCB.
 *
 * @param pcb UDP PCB to be bound with a local address ipaddr and port.
 * @param ipaddr local IP address to bind with. Use IP_ADDR_ANY to
 * bind to all local interfaces.
 * @param port local UDP port to bind with. Use 0 to automatically bind
 * to a random port between UDP_LOCAL_PORT_RANGE_START and
 * UDP_LOCAL_PORT_RANGE_END.
 *
 * ipaddr & port are expected to be in the same byte order as in the pcb.
 *
 * @return lwIP error code.
 * - ERR_OK. Successful. No error occured.
 * - ERR_USE. The specified ipaddr and port are already bound to by
 * another UDP PCB.
 *
 * @see udp_disconnect()
 */
uint8 udp_bind(UDP_PCB *pcb, IP_ADDR *ipaddr, uint16 port)
{
    UDP_PCB *ipcb;
    uint8 rebind;

    ip_addr_set(&pcb->local_ip, ipaddr);

    /* no port specified? */
    pcb->local_port = (port == 0)? 4096 : port;

    return ERR_OK;
}
/**
 * Connect an UDP PCB.
 *
 * This will associate the UDP PCB with the remote address.
 *
 * @param pcb UDP PCB to be connected with remote address ipaddr and port.
 * @param ipaddr remote IP address to connect with.
 * @param port remote UDP port to connect with.
 *
 * @return lwIP error code
 *
 * ipaddr & port are expected to be in the same byte order as in the pcb.
 *
 * The udp pcb is bound to a random local port if not already bound.
 *
 * @see udp_disconnect()
 */
uint8 udp_connect(UDP_PCB *pcb, IP_ADDR *ipaddr, uint16 port)
{
    UDP_PCB *ipcb;
	uint8 err;

    if (pcb->local_port == 0)
    {
        err = udp_bind(pcb, &pcb->local_ip, pcb->local_port);
        if (err != ERR_OK)
            return err;
    }
    ip_addr_set(&pcb->remote_ip, ipaddr);
    pcb->remote_port = port;
    pcb->flags |= UDP_FLAGS_CONNECTED;
	
    return ERR_OK;
}

/**
 * Disconnect a UDP PCB
 *
 * @param pcb the udp pcb to disconnect.
 */
void udp_disconnect(UDP_PCB *pcb)
{
    /* reset remote address association */
    ip_addr_set(&pcb->remote_ip, IP_ADDR_ANY);
    pcb->remote_port = 0;
    /* mark PCB as unconnected */
    pcb->flags &= ~UDP_FLAGS_CONNECTED;
}

/**
 * Set a receive callback for a UDP PCB
 *
 * This callback will be called when receiving a datagram for the pcb.
 *
 * @param pcb the pcb for wich to set the recv callback
 * @param recv function pointer of the callback function
 * @param recv_arg additional argument to pass to the callback function
 */
void
udp_recv(UDP_PCB *pcb,
         void (* recv)(void *arg, UDP_PCB *upcb, PBUF *p,
                       IP_ADDR *addr, uint16 port),
         void *recv_arg)
{
    /* remember recv() callback and user data */
    pcb->recv = recv;
    pcb->recv_arg = recv_arg;
}


/**
 * Create a UDP PCB.
 *
 * @return The UDP PCB which was created. NULL if the PCB data structure
 * could not be allocated.
 *
 * @see udp_remove()
 */
UDP_PCB * udp_new(uint8 id)
{
	UDP_PCB * ppcb;
	
	//dgl warn:if here may return null,you must judge the return value of the function...
	ppcb = &udppcb[id];
	memset(ppcb, 0, sizeof(UDP_PCB));
	ppcb->ttl = UDP_TTL;

    return ppcb;
}
