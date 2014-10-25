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
#ifndef __LWIP_NETIF_H__
#define __LWIP_NETIF_H__


/* Throughout this file, IP addresses are expected to be in
 * the same byte order as in IP_PCB. */

/** TODO: define the use (where, when, whom) of netif flags */

/** whether the network interface is 'up'. this is
 * a software flag used to control whether this network
 * interface is enabled and processes traffic.
 */
#define NETIF_FLAG_UP           0x01U
/** if set, the netif has broadcast capability */
#define NETIF_FLAG_BROADCAST    0x02U
/** if set, the netif is one end of a point-to-point connection */
#define NETIF_FLAG_POINTTOPOINT 0x04U
/** if set, the interface is configured using DHCP */
#define NETIF_FLAG_DHCP         0x08U
/** if set, the interface has an active link
 *  (set by the network interface driver) */
#define NETIF_FLAG_LINK_UP      0x10U
/** if set, the netif is an device using ARP */
#define NETIF_FLAG_ETHARP       0x20U
/** if set, the netif has IGMP capability */
#define NETIF_FLAG_IGMP         0x40U

/** Generic data structure used for all lwIP network interfaces.
 *  The following fields should be filled in by the initialization
 *  function for the device driver: hwaddr_len, hwaddr[], mtu, flags */



extern  NETIF __stEMACNetif;

void arp_list_init(void);
void arp_list_clr_init(void);
void arp_entry_p_clr(void);
void ethernetif_input(void);

NETIF *netif_add(IP_ADDR *ipaddr, IP_ADDR *netmask, IP_ADDR *gw);

void netif_set_addr(NETIF *netif, IP_ADDR *ipaddr, IP_ADDR *netmask, IP_ADDR *gw);
void netif_remove(void);

/* Returns a network interface given its name. The name is of the form
   "et0", where the first two letters are the "name" field in the
   netif structure, and the digit is in the num field in the same
   structure. */
NETIF *netif_find(char *name);
void netif_set_default(NETIF *netif);
void netif_set_ipaddr(NETIF *netif, IP_ADDR *ipaddr);
void netif_set_netmask(NETIF *netif, IP_ADDR *netmask);
void netif_set_gw(NETIF *netif, IP_ADDR *gw);
void netif_set_up(void);
void netif_set_down(void);
uint8 netif_is_up(void);
#endif /* __LWIP_NETIF_H__ */
