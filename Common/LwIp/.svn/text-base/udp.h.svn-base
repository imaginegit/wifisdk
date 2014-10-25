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
#ifndef __LWIP_UDP_H__
#define __LWIP_UDP_H__

#include "lwipopt.h"

#if LWIP_UDP /* don't build if not configured for use in lwipopts.h */

#include "pbuf.h"
#include "netif.h"
#include "ip_addr.h"
#include "ip.h"

#define UDP_HLEN 8


#define UDP_FLAGS_NOCHKSUM 0x01U
#define UDP_FLAGS_UDPLITE  0x02U
#define UDP_FLAGS_CONNECTED  0x04U

/* udp_pcbs export for exernal reference (e.g. SNMP agent) */
extern UDP_PCB *udp_pcbs;

/* The following functions is the application layer interface to the
   UDP code. */
void udp_pcb_init(void);
UDP_PCB * udp_new(uint8 id);
//void udp_remove(UDP_PCB *pcb);
uint8 udp_bind (UDP_PCB *pcb, IP_ADDR *ipaddr, uint16 port);
uint8 udp_connect (UDP_PCB *pcb, IP_ADDR *ipaddr, uint16 port);
void  udp_disconnect(UDP_PCB *pcb);
void  udp_recv(UDP_PCB *pcb,
               void (* recv)(void *arg, UDP_PCB *upcb, PBUF *p, IP_ADDR *addr, uint16 port),
               void *recv_arg);

uint8 udp_sendto_if(UDP_PCB *pcb, PBUF *p, IP_ADDR *dst_ip, uint16 dst_port, NETIF *netif);
uint8 udp_sendto(UDP_PCB *pcb, PBUF *p, IP_ADDR *dst_ip, uint16 dst_port);
uint8 udp_send (UDP_PCB *pcb, PBUF *p);

#define          udp_flags(pcb)  ((pcb)->flags)
#define          udp_setflags(pcb, f)  ((pcb)->flags = (f))

/* The following functions are the lower layer interface to UDP. */
void udp_input (PBUF *p, NETIF *inp);

#endif /* LWIP_UDP */

#endif /* __LWIP_UDP_H__ */
