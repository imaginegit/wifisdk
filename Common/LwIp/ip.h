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
#ifndef __LWIP_IP_H__
#define __LWIP_IP_H__

/** Currently, the function ip_output_if_opt() is only used with IGMP */
#define IP_OPTIONS_SEND   LWIP_IGMP

#define IP_HLEN 20

#define IP_PROTO_ICMP    1
#define IP_PROTO_UDP     17
#define IP_PROTO_UDPLITE 136
#define IP_PROTO_TCP     6

/* This is passed as the destination address to ip_output_if (not
   to ip_output), meaning that an IP header already is constructed
   in the pbuf. This is used when TCP retransmits. */
#ifdef IP_HDRINCL
#undef IP_HDRINCL
#endif /* IP_HDRINCL */
#define IP_HDRINCL  NULL 

/*
 * Option flags per-socket. These are the same like SO_XXX.
 */
#define SOF_DEBUG       (uint16)0x0001U    /* turn on debugging info recording */
#define SOF_ACCEPTCONN  (uint16)0x0002U    /* socket has had listen() */
#define SOF_REUSEADDR   (uint16)0x0004U    /* allow local address reuse */
#define SOF_KEEPALIVE   (uint16)0x0008U    /* keep connections alive */
#define SOF_DONTROUTE   (uint16)0x0010U    /* just use interface addresses */
#define SOF_BROADCAST   (uint16)0x0020U    /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */
#define SOF_USELOOPBACK (uint16)0x0040U    /* bypass hardware when possible */
#define SOF_LINGER      (uint16)0x0080U    /* linger on close if data present */
#define SOF_OOBINLINE   (uint16)0x0100U    /* leave received OOB data in line */
#define SOF_REUSEPORT   (uint16)0x0200U    /* allow local address & port reuse */


#define IP_RF 0x8000        /* reserved fragment flag */
#define IP_DF 0x4000        /* dont fragment flag */
#define IP_MF 0x2000        /* more fragments flag */
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */




#define IPH_V(hdr)  (ntohs((hdr)->_v_hl_tos) >> 12)
#define IPH_HL(hdr) ((ntohs((hdr)->_v_hl_tos) >> 8) & 0x0f)
#define IPH_TOS(hdr) (ntohs((hdr)->_v_hl_tos) & 0xff)
#define IPH_LEN(hdr) ((hdr)->_len)
#define IPH_ID(hdr) ((hdr)->_id)
#define IPH_OFFSET(hdr) ((hdr)->_offset)
#define IPH_TTL(hdr) (ntohs((hdr)->_ttl_proto) >> 8)
#define IPH_PROTO(hdr) (ntohs((hdr)->_ttl_proto) & 0xff)
#define IPH_CHKSUM(hdr) ((hdr)->_chksum)

#define IPH_VHLTOS_SET(hdr, v, hl, tos) (hdr)->_v_hl_tos = (htons(((v) << 12) | ((hl) << 8) | (tos)))
#define IPH_LEN_SET(hdr, len) (hdr)->_len = (len)
#define IPH_ID_SET(hdr, id) (hdr)->_id = (id)
#define IPH_OFFSET_SET(hdr, off) (hdr)->_offset = (off)
#define IPH_TTL_SET(hdr, ttl) (hdr)->_ttl_proto = (htons(IPH_PROTO(hdr) | ((uint16)(ttl) << 8)))
#define IPH_PROTO_SET(hdr, proto) (hdr)->_ttl_proto = (htons((proto) | (IPH_TTL(hdr) << 8)))
#define IPH_CHKSUM_SET(hdr, chksum) (hdr)->_chksum = (chksum)

NETIF *ip_route(IP_ADDR *dest);
uint8  ip_input(PBUF *p, NETIF *inp);
uint8  ip_output(PBUF *p, IP_ADDR *src, IP_ADDR *dest, uint8 ttl, uint8 tos, uint8 proto);
uint8  ip_output_if(PBUF *p, IP_ADDR *src, IP_ADDR *dest, uint8 ttl, uint8 tos, uint8 proto, NETIF *netif);
NETIF *ip_current_netif(void);

#endif /* __LWIP_IP_H__ */


