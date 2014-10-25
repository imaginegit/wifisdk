/**
 * @file
 * AutoIP Automatic LinkLocal IP Configuration
 *
 */

/*
 *
 * Copyright (c) 2007 Dominik Spies <kontakt@dspies.de>
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
 * Author: Dominik Spies <kontakt@dspies.de>
 *
 * This is a AutoIP implementation for the lwIP TCP/IP stack. It aims to conform
 * with RFC 3927.
 *
 *
 * Please coordinate changes and requests with Dominik Spies
 * <kontakt@dspies.de>
 */

/*******************************************************************************
 * USAGE:
 *
 * define LWIP_AUTOIP 1  in your lwipopts.h
 *
 * If you don't use tcpip.c (so, don't call, you don't call tcpip_init):
 * - First, call autoip_init().
 * - call autoip_tmr() all AUTOIP_TMR_INTERVAL msces,
 *   that should be defined in autoip.h.
 *   I recommend a value of 100. The value must divide 1000 with a remainder almost 0.
 *   Possible values are 1000, 500, 333, 250, 200, 166, 142, 125, 111, 100 ....
 *
 * Without DHCP:
 * - Call autoip_start() after netif_add().
 *
 * With DHCP:
 * - define LWIP_DHCP_AUTOIP_COOP 1 in your lwipopts.h.
 * - Configure your DHCP Client.
 *
 */
#if LWIP_AUTOIP
 
#include "sysinclude.h"
#include "lwipopt.h"
#include "def.h"
#include "err.h"
#include "udp.h"
#include "ip_addr.h"
#include "netif.h"
#include "autoip.h"
#include "etharp.h"
#include "inet.h"

#include <stdlib.h>
#include <string.h>

/* 169.254.0.0 */
#define AUTOIP_NET         0xA9FE0000
/* 169.254.1.0 */
#define AUTOIP_RANGE_START (AUTOIP_NET | 0x0100)
/* 169.254.254.255 */
#define AUTOIP_RANGE_END   (AUTOIP_NET | 0xFEFF)


/** Pseudo random macro based on netif informations.
 * You could use "rand()" from the C Library if you define LWIP_AUTOIP_RAND in lwipopts.h */
#ifndef LWIP_AUTOIP_RAND
#define LWIP_AUTOIP_RAND(netif) ( (((uint32)((netif->hwaddr[5]) & 0xff) << 24) | \
                                   ((uint32)((netif->hwaddr[3]) & 0xff) << 16) | \
                                   ((uint32)((netif->hwaddr[2]) & 0xff) << 8) | \
                                   ((uint32)((netif->hwaddr[4]) & 0xff))) + \
                                   (netif->autoip? ((AUTO_IP *)(netif->autoip))->tried_llipaddr : 0))
#endif /* LWIP_AUTOIP_RAND */

/**
 * Macro that generates the initial IP address to be tried by AUTOIP.
 * If you want to override this, define it to something else in lwipopts.h.
 */
#ifndef LWIP_AUTOIP_CREATE_SEED_ADDR
#define LWIP_AUTOIP_CREATE_SEED_ADDR(netif) \
  (AUTOIP_RANGE_START + ((uint32)(((uint8)(netif->hwaddr[4])) | \
                 ((uint32)((uint8)(netif->hwaddr[5]))) << 8)))
#endif /* LWIP_AUTOIP_CREATE_SEED_ADDR */

/* static functions */
static void autoip_handle_arp_conflict(NETIF *netif);

/* creates a pseudo random LL IP-Address for a network interface */
static void autoip_create_addr(NETIF *netif, IP_ADDR *IPAddr);

/* sends an ARP announce */
static uint8 autoip_arp_announce(NETIF *netif);

/* configure interface for use with current LL IP-Address */
static uint8 autoip_bind(NETIF *netif);


extern uint8
etharp_request(NETIF *netif, IP_ADDR *ipaddr);


extern uint8
etharp_raw(NETIF *netif, const ETH_ADDR *ethsrc_addr, const ETH_ADDR *ethdst_addr,
           const ETH_ADDR *hwsrc_addr, const IP_ADDR *ipsrc_addr,
           const ETH_ADDR *hwdst_addr, const IP_ADDR *ipdst_addr,
           const uint16 opcode);



AUTO_IP stAutoIp;

SYSTICK_LIST AutoIP_SysTimer =
{ 
    NULL, 
    0, 
    AUTOIP_TMR_INTERVAL / 10, 
    0, 
    autoip_tmr,
};

extern SYSTICK_LIST SysTickHead;
/**
 * Initialize this module
 */
void
autoip_init(void)
{

}

/**
 * Handle a IP address conflict after an ARP conflict detection
 */
static void
autoip_handle_arp_conflict(NETIF *netif)
{
    /* Somehow detect if we are defending or retreating */
    unsigned char defend = 1; /* tbd */
    AUTO_IP * pAutoIp;
	
    pAutoIp = (AUTO_IP *)(netif->autoip);
    if (defend)
    {
        if (pAutoIp->lastconflict > 0)
        {
            /* retreat, there was a conflicting ARP in the last
             * DEFEND_INTERVAL seconds
             */

            /* TODO: close all TCP sessions */
            autoip_start(netif);
        }
        else
        {
            autoip_arp_announce(netif);
            pAutoIp->lastconflict = DEFEND_INTERVAL * AUTOIP_TICKS_PER_SECOND;
        }
    }
    else
    {

        /* TODO: close all TCP sessions */
        autoip_start(netif);
    }
}

/**
 * Create an IP-Address out of range 169.254.1.0 to 169.254.254.255
 *
 * @param netif network interface on which create the IP-Address
 * @param IPAddr ip address to initialize
 */
static void
autoip_create_addr(NETIF *netif, IP_ADDR *IPAddr)
{
    /* Here we create an IP-Address out of range 169.254.1.0 to 169.254.254.255
     * compliant to RFC 3927 Section 2.1
     * We have 254 * 256 possibilities */

    uint32 addr = ntohl(LWIP_AUTOIP_CREATE_SEED_ADDR(netif));
    addr += ((AUTO_IP *)(netif->autoip))->tried_llipaddr;
    addr = AUTOIP_NET | (addr & 0xffff);
    /* Now, 169.254.0.0 <= addr <= 169.254.255.255 */

    if (addr < AUTOIP_RANGE_START)
    {
        addr += AUTOIP_RANGE_END - AUTOIP_RANGE_START + 1;
    }
    if (addr > AUTOIP_RANGE_END)
    {
        addr -= AUTOIP_RANGE_END - AUTOIP_RANGE_START + 1;
    }

    IPAddr->addr = htonl(addr);
}

/**
 * Sends an ARP announce from a network interface
 *
 * @param netif network interface used to send the announce
 */
static uint8
autoip_arp_announce(NETIF *netif)
{
    return etharp_raw(netif, (ETH_ADDR *)netif->hwaddr, &ethbroadcast,
                      (ETH_ADDR *)netif->hwaddr, &((AUTO_IP *)(netif->autoip))->llipaddr, 
                      &ethzero, &((AUTO_IP *)(netif->autoip))->llipaddr, 
                      ARP_REQUEST);
}

/**
 * Configure interface for use with current LL IP-Address
 *
 * @param netif network interface to configure with current LL IP-Address
 */
static uint8
autoip_bind(NETIF *netif)
{
    AUTO_IP *autoip = (AUTO_IP *)(netif->autoip);
    IP_ADDR sn_mask, gw_addr;

    IP4_ADDR(&sn_mask, 255, 255, 0, 0);
    IP4_ADDR(&gw_addr, 0, 0, 0, 0);

    netif_set_ipaddr(netif, &autoip->llipaddr);
    netif_set_netmask(netif, &sn_mask);
    netif_set_gw(netif, &gw_addr);
    
    return ERR_OK;
}

/**
 * Start AutoIP client
 *
 * @param netif network interface on which start the AutoIP client
 */
uint8
autoip_start(NETIF *netif)
{
    AUTO_IP *autoip = (AUTO_IP *)(netif->autoip);
    uint8 result = ERR_OK;

    /* Set IP-Address, Netmask and Gateway to 0 to make sure that
     * ARP Packets are formed correctly
     */
    netif->ip_addr.addr = 0;
    netif->netmask.addr = 0;
    netif->gw.addr      = 0;

    DEBUG("start AutoIp");

    if (autoip == NULL)
    {
        autoip = &stAutoIp;
        memset(autoip, 0, sizeof(AUTO_IP));
        /* store this AutoIP client in the netif */
        netif->autoip = autoip;

    }
    else
    {
        autoip->state = AUTOIP_STATE_OFF;
        autoip->ttw = 0;
        autoip->sent_num = 0;
        memset(&autoip->llipaddr, 0, sizeof(IP_ADDR));
        autoip->lastconflict = 0;
    }
    
    autoip_create_addr(netif, &(autoip->llipaddr));

    DEBUG("probe ip addr = %d.%d.%d.%d",  ((IP_ADDR3 *)(&(autoip->llipaddr)))->addrb[0],
                                           ((IP_ADDR3 *)(&(autoip->llipaddr)))->addrb[1],
                                           ((IP_ADDR3 *)(&(autoip->llipaddr)))->addrb[2],
                                           ((IP_ADDR3 *)(&(autoip->llipaddr)))->addrb[3]);
    
    autoip->tried_llipaddr++;
    autoip->state = AUTOIP_STATE_PROBING;
    autoip->sent_num = 0;

    /* time to wait to first probe, this is randomly
     * choosen out of 0 to PROBE_WAIT seconds.
     * compliant to RFC 3927 Section 2.2.1
     */
    autoip->ttw = (uint16)(LWIP_AUTOIP_RAND(netif) % (PROBE_WAIT * AUTOIP_TICKS_PER_SECOND));

    /*
     * if we tried more then MAX_CONFLICTS we must limit our rate for
     * accquiring and probing address
     * compliant to RFC 3927 Section 2.2.1
     */

    if (autoip->tried_llipaddr > MAX_CONFLICTS)
    {
        autoip->ttw = RATE_LIMIT_INTERVAL * AUTOIP_TICKS_PER_SECOND;
    }

    DEBUG("start autoip timer");
    SystickTimerStart(&AutoIP_SysTimer);
    MINDEBUG("SysTickHead->pNext = 0x%08x\n", SysTickHead.pNext);

    return result;
}

/**
 * Stop AutoIP client
 *
 * @param netif network interface on which stop the AutoIP client
 */
uint8
autoip_stop(NETIF *netif)
{
    ((AUTO_IP *)(netif->autoip))->state = AUTOIP_STATE_OFF;
    return ERR_OK;
}

/**
 * Has to be called in loop every AUTOIP_TMR_INTERVAL milliseconds
 */
void
autoip_tmr()
{
    NETIF *netif = &__stEMACNetif;
    AUTO_IP * pAutoIp;

    pAutoIp = (AUTO_IP *)(netif->autoip);
    DEBUG("pAutoIp->ttw = 0x%04x", pAutoIp->ttw);
    /* only act on AutoIP configured interfaces */
    if (pAutoIp != NULL)
    {
        if (pAutoIp->lastconflict > 0)
        {
            pAutoIp->lastconflict--;
        }
        
        switch (pAutoIp->state)
        {
            case AUTOIP_STATE_PROBING:
                if (pAutoIp->ttw > 0)
                {
                    pAutoIp->ttw--;
                }
                else
                {
                    if (pAutoIp->sent_num == PROBE_NUM)
                    {
                        pAutoIp->state = AUTOIP_STATE_ANNOUNCING;
                        pAutoIp->sent_num = 0;
                        pAutoIp->ttw = ANNOUNCE_WAIT * AUTOIP_TICKS_PER_SECOND;
                        DEBUG("Ip address is not conflict");
                    }
                    else
                    {
                        DEBUG("send a probe(arp) packet, total = %d", pAutoIp->sent_num);
                        etharp_request(netif, &(pAutoIp->llipaddr));

                        pAutoIp->sent_num++;
                        /* calculate time to wait to next probe */
                        pAutoIp->ttw = (uint16)((LWIP_AUTOIP_RAND(netif) %
                                                      ((PROBE_MAX - PROBE_MIN) * AUTOIP_TICKS_PER_SECOND) ) +
                                                     PROBE_MIN * AUTOIP_TICKS_PER_SECOND);                        
                        
                    }
                }
                break;

            case AUTOIP_STATE_ANNOUNCING:
                if (pAutoIp->ttw > 0)
                {
                    pAutoIp->ttw--;
                }
                else
                {
                    if (pAutoIp->sent_num == 0)
                    {
                        /* We are here the first time, so we waited ANNOUNCE_WAIT seconds
                         * Now we can bind to an IP address and use it
                         */
                        autoip_bind(netif);
                        DEBUG("Ip address is bind");
                    }

                    if (pAutoIp->sent_num == ANNOUNCE_NUM)
                    {
                        pAutoIp->state = AUTOIP_STATE_BOUND;
                        pAutoIp->sent_num = 0;
                        pAutoIp->ttw = 0;
                        
                    }
                    else
                    {
                        autoip_arp_announce(netif);
                        pAutoIp->sent_num++;
                        pAutoIp->ttw = ANNOUNCE_INTERVAL * AUTOIP_TICKS_PER_SECOND;
                        DEBUG("send a announce(arp) packet, total = %d", pAutoIp->sent_num);
                        
                    }
                }
                break;
        }
    }
}

/**
 * Handles every incoming ARP Packet, called by etharp_arp_input.
 *
 * @param netif network interface to use for autoip processing
 * @param hdr Incoming ARP packet
 */
void
autoip_arp_reply(NETIF *netif, ETHARP_HDR *hdr)
{
    IP_ADDR sipaddr, dipaddr;
    ETH_ADDR netifaddr;
	
    if ((netif->autoip != NULL) && (netif->autoip->state != AUTOIP_STATE_OFF))
    {
        /* when ip.src == llipaddr && hw.src != netif->hwaddr
         *
         * when probing  ip.dst == llipaddr && hw.src != netif->hwaddr
         * we have a conflict and must solve it
         */
        netifaddr.addr[0] = netif->hwaddr[0];
        netifaddr.addr[1] = netif->hwaddr[1];
        netifaddr.addr[2] = netif->hwaddr[2];
        netifaddr.addr[3] = netif->hwaddr[3];
        netifaddr.addr[4] = netif->hwaddr[4];
        netifaddr.addr[5] = netif->hwaddr[5];

        /* Copy struct ip_addr2 to aligned ip_addr, to support compilers without
         * structure packing (not using structure copy which breaks strict-aliasing rules).
         */
        SMEMCPY(&sipaddr, &hdr->sipaddr, sizeof(sipaddr));
        SMEMCPY(&dipaddr, &hdr->dipaddr, sizeof(dipaddr));

        if ((netif->autoip->state == AUTOIP_STATE_PROBING) ||
                (netif->autoip->state == AUTOIP_STATE_ANNOUNCING) &&
                 (netif->autoip->sent_num == 0))
        {
            /* RFC 3927 Section 2.2.1:
             * from beginning to after ANNOUNCE_WAIT
             * seconds we have a conflict if
             * ip.src == llipaddr OR
             * ip.dst == llipaddr && hw.src != own hwaddr
             */
            if (ip_addr_cmp(&sipaddr, &(netif->autoip->llipaddr)) ||
                    (ip_addr_cmp(&dipaddr, &(netif->autoip->llipaddr)) &&
                     !eth_addr_cmp(&netifaddr, &hdr->shwaddr)))
            {
                autoip_start(netif);
            }
        }
        else
        {
            /* RFC 3927 Section 2.5:
             * in any state we have a conflict if
             * ip.src == llipaddr && hw.src != own hwaddr
             */
            if (ip_addr_cmp(&sipaddr, &((AUTO_IP *)(netif->autoip))->llipaddr) &&
                    !eth_addr_cmp(&netifaddr, &hdr->shwaddr))
            {
                autoip_handle_arp_conflict(netif);
            }
        }
    }
}  

#endif
