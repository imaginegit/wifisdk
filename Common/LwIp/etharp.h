/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * Copyright (c) 2003-2004 Leon Woestenberg <leon.woestenberg@axon.tv>
 * Copyright (c) 2003-2004 Axon Digital Design B.V., The Netherlands.
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

#ifndef __NETIF_ETHARP_H__
#define __NETIF_ETHARP_H__

#if LWIP_ARP /* don't build if not configured for use in lwipopts.h */

enum etharp_state
{
	ETHARP_STATE_EMPTY = 0,
	ETHARP_STATE_PENDING,
	ETHARP_STATE_STABLE
};

typedef __packed struct _ETHARP_ENTRY
{
	PBUF *q;
	IP_ADDR ipaddr;
	ETH_ADDR ethaddr;
	NETIF *netif;
	enum etharp_state state;
	uint8 ctime;
}ETHARP_ENTRY;


/** 5 seconds period */
#define ARP_TMR_INTERVAL 5000

#define ETHTYPE_ARP       0x0806
#define ETHTYPE_IP        0x0800
#define ETHTYPE_PPPOEDISC 0x8863  /* PPP Over Ethernet Discovery Stage */
#define ETHTYPE_PPPOE     0x8864  /* PPP Over Ethernet Session Stage */

/** ARP message types (opcodes) */
#define ARP_REQUEST 1
#define ARP_REPLY   2

extern const ETH_ADDR ethbroadcast, ethzero;
#define eth_addr_cmp(addr1, addr2) (memcmp((addr1)->addr, (addr2)->addr, ETHARP_HWADDR_LEN) == 0)

extern void etharp_tmr(void);

#endif /* LWIP_ARP */

#endif /* __NETIF_ARP_H__ */
