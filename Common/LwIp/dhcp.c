/**
 * @file
 * Dynamic Host Configuration Protocol client
 *
 */

/*
 *
 * Copyright (c) 2001-2004 Leon Woestenberg <leon.woestenberg@gmx.net>
 * Copyright (c) 2001-2004 Axon Digital Design B.V., The Netherlands.
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
 * This file is a contribution to the lwIP TCP/IP stack.
 * The Swedish Institute of Computer Science and Adam Dunkels
 * are specifically granted permission to redistribute this
 * source code.
 *
 * Author: Leon Woestenberg <leon.woestenberg@gmx.net>
 *
 * This is a DHCP client for the lwIP TCP/IP stack. It aims to conform
 * with RFC 2131 and RFC 2132.
 *
 * TODO:
 * - Proper parsing of DHCP messages exploiting file/sname field overloading.
 * - Add JavaDoc style documentation (API, internals).
 * - Support for interfaces other than Ethernet (SLIP, PPP, ...)
 *
 * Please coordinate changes and requests with Leon Woestenberg
 * <leon.woestenberg@gmx.net>
 *
 * Integration with your code:
 *
 * In lwip/dhcp.h
 * #define DHCP_COARSE_TIMER_SECS (recommended 60 which is a minute)
 * #define DHCP_FINE_TIMER_MSECS (recommended 500 which equals TCP coarse timer)
 *
 * Then have your application call dhcp_coarse_tmr() and
 * dhcp_fine_tmr() on the defined intervals.
 *
 * dhcp_start(struct netif *netif);
 * starts a DHCP client instance which configures the interface by
 * obtaining an IP address lease and maintaining it.
 *
 * Use dhcp_release(netif) to end the lease and use dhcp_stop(netif)
 * to remove the DHCP client.
 *
 */

#include "sdio_card.h"
#include "lwipopt.h"
#include "def.h"
#include "err.h"   
#include "udp.h"
#include "ip_addr.h"
#include "netif.h"
#include "inet.h" 
#include "dhcp.h"
#include "dns.h"
#include "etharp.h"

#include <string.h>

/** Default for DHCP_GLOBAL_XID is 0xABCD0000
 * This can be changed by defining DHCP_GLOBAL_XID and DHCP_GLOBAL_XID_HEADER, e.g.
 *  #define DHCP_GLOBAL_XID_HEADER "stdlib.h"
 *  #define DHCP_GLOBAL_XID rand()
 */

static DHCP dhcp;
static UDP_PCB *userPcb = NULL;

/** DHCP_OPTION_MAX_MSG_SIZE is set to the MTU
 * MTU is checked to be big enough in dhcp_start */
#define DHCP_MAX_MSG_LEN(netif)        (netif->mtu)
#define DHCP_MAX_MSG_LEN_MIN_REQUIRED  576

/* DHCP client state machine functions */
static void dhcp_handle_ack(NETIF *netif);
static void dhcp_handle_nak(NETIF *netif);
static void dhcp_handle_offer(NETIF *netif);

static uint8 dhcp_discover(NETIF *netif);
static uint8 dhcp_select(NETIF *netif);
static void dhcp_check(NETIF *netif);
static void dhcp_bind(NETIF *netif);

#if DHCP_DOES_ARP_CHECK
static uint8 dhcp_decline(NETIF *netif);
#endif /* DHCP_DOES_ARP_CHECK */
static uint8 dhcp_rebind(NETIF *netif);
static void dhcp_set_state(DHCP *dhcp, uint8 new_state);

/* receive, unfold, parse and free incoming messages */
static void dhcp_recv(void *arg, UDP_PCB *pcb, PBUF *p, IP_ADDR *addr, uint16 port);
static void user_recv(void *arg, UDP_PCB *pcb, PBUF *p, IP_ADDR *addr, uint16 port);
static uint8 dhcp_unfold_reply(DHCP *dhcp);
static uint8 *dhcp_get_option_ptr(DHCP *dhcp, uint8 option_type);
static uint8 dhcp_get_option_byte(uint8 *ptr);

static uint32 dhcp_get_option_long(uint8 *ptr);
static void dhcp_free_reply(DHCP *dhcp);

/* set the DHCP timers */
static void dhcp_timeout(NETIF *netif);
static void dhcp_t1_timeout(NETIF *netif);
static void dhcp_t2_timeout(NETIF *netif);

/* build outgoing messages */
/* create a DHCP request, fill in common headers */
static uint8 dhcp_create_request(NETIF *netif);
/* free a DHCP request */
static void dhcp_delete_request(NETIF *netif);
/* add a DHCP option (type, then length in bytes) */
static void dhcp_option(DHCP *dhcp, uint8 option_type, uint8 option_len);
/* add option values */
static void dhcp_option_byte(DHCP *dhcp, uint8 value);
static void dhcp_option_short(DHCP *dhcp, uint16 value);
static void dhcp_option_long(DHCP *dhcp, uint32 value);
/* always add the DHCP options trailer to end and pad */
static void dhcp_option_trailer(DHCP *dhcp);

/**
 * Back-off the DHCP client (because of a received NAK response).
 *
 * Back-off the DHCP client because of a received NAK. Receiving a
 * NAK means the client asked for something non-sensible, for
 * example when it tries to renew a lease obtained on another network.
 *
 * We clear any existing set IP address and restart DHCP negotiation
 * afresh (as per RFC2131 3.2.3).
 *
 * @param netif the netif under DHCP control
 */
static void dhcp_handle_nak(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;

    /* remove IP address from interface */
    netif_set_down();
    
    netif_set_ipaddr(netif, IP_ADDR_ANY);
    netif_set_gw(netif, IP_ADDR_ANY);
    netif_set_netmask(netif, IP_ADDR_ANY);
    /* Change to a defined state */
    dhcp_set_state(dhcp, DHCP_BACKING_OFF);
    /* We can immediately restart discovery */
    dhcp_discover(netif);
}

/**
 * Checks if the offered IP address is already in use.
 *
 * It does so by sending an ARP request for the offered address and
 * entering CHECKING state. If no ARP reply is received within a small
 * interval, the address is assumed to be free for use by us.
 *
 * @param netif the netif under DHCP control
 */
static void dhcp_check(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    uint8 result;
    uint16 msecs;
	
    dhcp_set_state(dhcp, DHCP_CHECKING);
    /* create an ARP query for the offered IP address, expecting that no host
       responds, as the IP address should not be in use. */
    result = etharp_query(netif, &dhcp->offered_ip_addr, NULL);
    if (result != ERR_OK)
    {
        ;
    }
    dhcp->tries++;
    msecs = 500;
    dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;
}

/**
 * Remember the configuration offered by a DHCP server.
 *
 * @param netif the netif under DHCP control
 */
static void dhcp_handle_offer(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    /* obtain the server address */
    uint8 *option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_SERVER_ID);

    if (option_ptr != NULL)
    {
        dhcp->server_ip_addr.addr = htonl(dhcp_get_option_long(&option_ptr[2]));//offer ip lease address.
        /* remember offered address */
        ip_addr_set(&dhcp->offered_ip_addr, (IP_ADDR *)&dhcp->msg_in->yiaddr);//dgl our ip address get by DHCP..
        dhcp_select(netif);
    }
}

/**
 * Select a DHCP server offer out of all offers.
 *
 * Simply select the first offer received.
 *
 * @param netif the netif under DHCP control
 * @return lwIP specific error (see error.h)
 */
static uint8 dhcp_select(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    uint8 result;
    uint16 msecs;
    char *p;

    dhcp_set_state(dhcp, DHCP_REQUESTING);

    /* create and initialize the DHCP message header */
    result = dhcp_create_request(netif);
    if (result == ERR_OK)
    {
        dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
        dhcp_option_byte(dhcp, DHCP_REQUEST);

        dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
        dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));

        /* MUST request the offered IP address */
        dhcp_option(dhcp, DHCP_OPTION_REQUESTED_IP, 4);
        dhcp_option_long(dhcp, ntohl(dhcp->offered_ip_addr.addr));

        dhcp_option(dhcp, DHCP_OPTION_SERVER_ID, 4);
        dhcp_option_long(dhcp, ntohl(dhcp->server_ip_addr.addr));

        dhcp_option(dhcp, DHCP_OPTION_PARAMETER_REQUEST_LIST, 4/*num options*/);
        dhcp_option_byte(dhcp, DHCP_OPTION_SUBNET_MASK);
        dhcp_option_byte(dhcp, DHCP_OPTION_ROUTER);
        dhcp_option_byte(dhcp, DHCP_OPTION_BROADCAST);
        dhcp_option_byte(dhcp, DHCP_OPTION_DNS_SERVER);

        p = (char*)netif->hostname;
        if (p != NULL)
        {
            dhcp_option(dhcp, DHCP_OPTION_HOSTNAME, strlen(p));
            while (*p)
            {
                dhcp_option_byte(dhcp, *p++);
            }
        }

        dhcp_option_trailer(dhcp);

        /* TODO: we really should bind to a specific local interface here
           but we cannot specify an unconfigured netif as it is addressless */
        /* send broadcast to any DHCP server */
        udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT, netif);
        /* reconnect to any (or to server here?!) */
        udp_connect(dhcp->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
        dhcp_delete_request(netif);
    }

    dhcp->tries++;
    msecs = (dhcp->tries < 6 ? 1 << dhcp->tries : 60) * 1000;
    dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;
    return result;
}

/**
 * The DHCP timer that checks for lease renewal/rebind timeouts.
 *
 */
void dhcp_coarse_tmr()
{
    NETIF *netif = &__stEMACNetif;

    /* only act on DHCP configured interfaces */
    if (netif->dhcp != NULL)
    {
        /* timer is active (non zero), and triggers (zeroes) now? */
        if (netif->dhcp->t2_timeout-- == 1)
        {
            /* this clients' rebind timeout triggered */
            dhcp_t2_timeout(netif);
            /* timer is active (non zero), and triggers (zeroes) now */
        }
        else if (netif->dhcp->t1_timeout-- == 1)
        {
            /* this clients' renewal timeout triggered */
            dhcp_t1_timeout(netif);
        }
    }
    /* proceed to next netif */
}


/**
 * DHCP transaction timeout handling
 *
 * A DHCP server is expected to respond within a short period of time.
 * This timer checks whether an outstanding DHCP request is timed out.
 *
 */
void dhcp_fine_tmr()
{
    NETIF *netif = &__stEMACNetif;
    /* loop through netif's */

    /* only act on DHCP configured interfaces */
    if (netif->dhcp != NULL)
    {
        /* timer is active (non zero), and is about to trigger now */
        if (netif->dhcp->request_timeout > 1)
        {
            netif->dhcp->request_timeout--;
        }
        else if (netif->dhcp->request_timeout == 1)
        {
            netif->dhcp->request_timeout--;
            /* this clients' request timeout triggered */
            dhcp_timeout(netif);
        }
    }
}


/**
 * A DHCP negotiation transaction, or ARP request, has timed out.
 *
 * The timer that was started with the DHCP or ARP request has
 * timed out, indicating no response was received in time.
 *
 * @param netif the netif under DHCP control
 */
static void dhcp_timeout(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;

	printf("dhcptt=%d\n", dhcp->state);
    /* back-off period has passed, or server selection timed out */
    if ((dhcp->state == DHCP_BACKING_OFF) || (dhcp->state == DHCP_SELECTING))
    {
        dhcp_discover(netif);
        /* receiving the requested lease timed out */
    }
    else if (dhcp->state == DHCP_REQUESTING)
    {
        if (dhcp->tries <= 5)
        {
            dhcp_select(netif);
        }
        else
        {
            dhcp_release(netif);
            dhcp_discover(netif);
        }
        /* received no ARP reply for the offered address (which is good) */
    }
    else if (dhcp->state == DHCP_CHECKING)
    {
        if (dhcp->tries <= 1)
        {
            dhcp_check(netif);
            /* no ARP replies on the offered address,
               looks like the IP address is indeed free */
        }
        else
        {
            /* bind the interface to the offered address */
            dhcp_bind(netif);
        }
    }
    /* did not get response to renew request? */
    else if (dhcp->state == DHCP_RENEWING)
    {
        /* just retry renewal */
        /* note that the rebind timer will eventually time-out if renew does not work */
        dhcp_renew(netif);
        /* did not get response to rebind request? */
    }
    else if (dhcp->state == DHCP_REBINDING)
    {
        if (dhcp->tries <= 8)
        {
            dhcp_rebind(netif);
        }
        else
        {
            dhcp_release(netif);
            dhcp_discover(netif);
        }
    }
}

/**
 * The renewal period has timed out.
 *
 * @param netif the netif under DHCP control
 */
static void dhcp_t1_timeout(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    if ((dhcp->state == DHCP_REQUESTING) || (dhcp->state == DHCP_BOUND) || (dhcp->state == DHCP_RENEWING))
    {
        /* just retry to renew - note that the rebind timer (t2) will
         * eventually time-out if renew tries fail. */
        dhcp_renew(netif);
    }
}

/**
 * The rebind period has timed out.
 *
 * @param netif the netif under DHCP control
 */
static void dhcp_t2_timeout(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    if ((dhcp->state == DHCP_REQUESTING) || (dhcp->state == DHCP_BOUND) || (dhcp->state == DHCP_RENEWING))
    {
        /* just retry to rebind */
        dhcp_rebind(netif);
    }
}

/**
 * Handle a DHCP ACK packet
 *
 * @param netif the netif under DHCP control
 */ 
static void dhcp_handle_ack(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    uint8 *option_ptr;
	
    /* clear options we might not get from the ACK */
    dhcp->offered_sn_mask.addr = 0;
    dhcp->offered_gw_addr.addr = 0;
    dhcp->offered_bc_addr.addr = 0;

    /* lease time given? */
    option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_LEASE_TIME);
    if (option_ptr != NULL)
    {
        /* remember offered lease time */
        dhcp->offered_t0_lease = dhcp_get_option_long(option_ptr + 2);
    }
    /* renewal period given? */
    option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_T1);/* T1 renewal time */
    if (option_ptr != NULL)
    {
        /* remember given renewal period */
        dhcp->offered_t1_renew = dhcp_get_option_long(option_ptr + 2);
    }
    else
    {
        /* calculate safe periods for renewal */
        dhcp->offered_t1_renew = dhcp->offered_t0_lease / 2;
    }

    /* renewal period given? */
    option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_T2);/* T2 rebinding time */
    if (option_ptr != NULL)
    {
        /* remember given rebind period */
        dhcp->offered_t2_rebind = dhcp_get_option_long(option_ptr + 2);
    }
    else
    {
        /* calculate safe periods for rebinding */
        dhcp->offered_t2_rebind = dhcp->offered_t0_lease;
    }

    /* (y)our internet address */
    ip_addr_set(&dhcp->offered_ip_addr, &dhcp->msg_in->yiaddr);


    /* subnet mask */
    option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_SUBNET_MASK);
    /* subnet mask given? */
    if (option_ptr != NULL)
    {
        dhcp->offered_sn_mask.addr = htonl(dhcp_get_option_long(&option_ptr[2]));
    }

    /* gateway router */
    option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_ROUTER);
    if (option_ptr != NULL)
    {
        dhcp->offered_gw_addr.addr = htonl(dhcp_get_option_long(&option_ptr[2]));
    }

    /* broadcast address */
    option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_BROADCAST);
    if (option_ptr != NULL)
    {
        dhcp->offered_bc_addr.addr = htonl(dhcp_get_option_long(&option_ptr[2]));
    }

    /* DNS servers */
    option_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_DNS_SERVER);
    if (option_ptr != NULL)
    {
        uint8 n;
        dhcp->dns_count = dhcp_get_option_byte(&option_ptr[1]) / (uint32)sizeof(IP_ADDR);
        /* limit to at most DHCP_MAX_DNS DNS servers */
        if (dhcp->dns_count > DHCP_MAX_DNS)
            dhcp->dns_count = DHCP_MAX_DNS;
        for (n = 0; n < dhcp->dns_count; n++)
        {
            dhcp->offered_dns_addr[n].addr = htonl(dhcp_get_option_long(&option_ptr[2 + n * 4]));
            dns_setserver( n, (IP_ADDR *)(&(dhcp->offered_dns_addr[n].addr)));


            printf("dns server = %d.%d.%d.%d \n",  ((IP_ADDR3 *)&dhcp->offered_dns_addr[n].addr)->addrb[0],
                                                     ((IP_ADDR3 *)&dhcp->offered_dns_addr[n].addr)->addrb[1],
                                                     ((IP_ADDR3 *)&dhcp->offered_dns_addr[n].addr)->addrb[2],
                                                     ((IP_ADDR3 *)&dhcp->offered_dns_addr[n].addr)->addrb[3]);
        }
        dns_setserver(n, (IP_ADDR *)(&ip_addr_any));
    }
}

/**
 * find a server.
 * Start DHCP negotiation for a network interface.
 *
 * If no DHCP client instance was attached to this interface,
 * a new client is created first. If a DHCP client instance
 * was already present, it restarts negotiation.
 *
 * @param netif The lwIP network interface
 * @return lwIP error code
 * - ERR_OK - No error
 * - ERR_MEM - Out of memory
 */
uint8 dhcp_start(NETIF *netif)
{
    DHCP *pdhcp;
    uint8 result = ERR_OK;

    pdhcp = netif->dhcp;

    /* Remove the flag that says this netif is handled by DHCP,
       it is set when we succeeded starting. */
    netif->flags &= ~NETIF_FLAG_DHCP;

    /* check MTU of the netif */
    if (netif->mtu < DHCP_MAX_MSG_LEN_MIN_REQUIRED)
        return ERR_MEM;

    /* no DHCP client attached yet? */
    if (pdhcp == NULL)
    {
        pdhcp = &dhcp;
        /* store this dhcp client in the netif */
        netif->dhcp = pdhcp;
        /* already has DHCP client attached */
    }
    else
    {
        if (pdhcp->p != NULL)
        {
            pbuf_free(pdhcp->p);
		}
    }

    /* clear data structure */
    memset(pdhcp, 0, sizeof(DHCP));
	
    /* allocate UDP PCB */
    pdhcp->pcb = udp_new(0);

    /* set up local and remote port for the pcb */
    udp_bind(pdhcp->pcb, IP_ADDR_ANY, DHCP_CLIENT_PORT);
    udp_connect(pdhcp->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
	
    /* set up the recv callback and argument */
    udp_recv(pdhcp->pcb, dhcp_recv, netif);
	
    /* (re)start the DHCP negotiation */
    result = dhcp_discover(netif);
	
    if (result != ERR_OK)
    {
        dhcp_stop(netif);// free resources allocated above */
        return ERR_MEM;
    }
    /* Set the flag that says this netif is handled by DHCP. */
    netif->flags |= NETIF_FLAG_DHCP;
    return result;
}


/**
 * Match an ARP reply with the offered IP address.
 *
 * @param netif the network interface on which the reply was received
 * @param addr The IP address we received a reply from
 */
void dhcp_arp_reply(NETIF *netif, IP_ADDR *addr)
{
    /* is a DHCP client doing an ARP check? */
    if ((netif->dhcp != NULL) && (netif->dhcp->state == DHCP_CHECKING))
    {
        /* did a host respond with the address we
           were offered by the DHCP server? */
        if (ip_addr_cmp(addr, &netif->dhcp->offered_ip_addr))
        {
            /* we will not accept the offered address */
            dhcp_decline(netif);
        }
    }
}

/**
 * Decline an offered lease.
 *
 * Tell the DHCP server we do not accept the offered address.
 * One reason to decline the lease is when we find out the address
 * is already in use by another host (through ARP).
 *
 * @param netif the netif under DHCP control
 */
static uint8 dhcp_decline(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    uint8 result = ERR_OK;
    uint16 msecs;
    dhcp_set_state(dhcp, DHCP_BACKING_OFF);
	
    /* create and initialize the DHCP message header */
    result = dhcp_create_request(netif);
    if (result == ERR_OK)
    {
        dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
        dhcp_option_byte(dhcp, DHCP_DECLINE);

        dhcp_option(dhcp, DHCP_OPTION_REQUESTED_IP, 4);
        dhcp_option_long(dhcp, ntohl(dhcp->offered_ip_addr.addr));

        dhcp_option_trailer(dhcp);

        /* @todo: should we really connect here? we are performing sendto() */
        udp_connect(dhcp->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
        /* per section 4.4.4, broadcast DECLINE messages */
        udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT, netif);
        dhcp_delete_request(netif);

    }
    dhcp->tries++;
    msecs = 10*1000;
    dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;
    return result;
}

/**
 * Start the DHCP process, discover a DHCP server.
 *
 * @param netif the netif under DHCP control
 */
static uint8 dhcp_discover(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    uint8 result = ERR_OK;
    uint16 msecs;
	
    ip_addr_set(&dhcp->offered_ip_addr, IP_ADDR_ANY);
    dhcp_set_state(dhcp, DHCP_SELECTING);
    /* create and initialize the DHCP message header */
    result = dhcp_create_request(netif);
    if (result == ERR_OK)
    {
        dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
        dhcp_option_byte(dhcp, DHCP_DISCOVER);

        dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
        dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));

        dhcp_option(dhcp, DHCP_OPTION_PARAMETER_REQUEST_LIST, 4/*num options*/);
        dhcp_option_byte(dhcp, DHCP_OPTION_SUBNET_MASK);
        dhcp_option_byte(dhcp, DHCP_OPTION_ROUTER);
        dhcp_option_byte(dhcp, DHCP_OPTION_BROADCAST);
        dhcp_option_byte(dhcp, DHCP_OPTION_DNS_SERVER);

        dhcp_option_trailer(dhcp);

        udp_connect(dhcp->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);

        udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT, netif);

        dhcp_delete_request(netif);

    }
	
    dhcp->tries++;
    msecs = (dhcp->tries < 6 ? 1 << dhcp->tries : 60) * 1000;
    dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;
    return result;
}


/**
 * Bind the interface to the offered IP address.
 *
 * @param netif network interface to bind to the offered address
 */
static void dhcp_bind(NETIF *netif)
{
    uint32 timeout;
    DHCP *dhcp;
    IP_ADDR sn_mask, gw_addr;
    dhcp = netif->dhcp;

    /* temporary DHCP lease? */
    if (dhcp->offered_t1_renew != 0xffffffffUL)
    {
        /* set renewal period timer */
        timeout = (dhcp->offered_t1_renew + DHCP_COARSE_TIMER_SECS / 2) / DHCP_COARSE_TIMER_SECS;
        if (timeout > 0xffff)
        {
            timeout = 0xffff;
        }
        dhcp->t1_timeout = (uint16)timeout;

        if (dhcp->t1_timeout == 0)
        {
            dhcp->t1_timeout = 1;
        }
    }
    /* set renewal period timer */
    if (dhcp->offered_t2_rebind != 0xffffffffUL)
    {
        timeout = (dhcp->offered_t2_rebind + DHCP_COARSE_TIMER_SECS / 2) / DHCP_COARSE_TIMER_SECS;
        if (timeout > 0xffff)
        {
            timeout = 0xffff;//the unit is min
        }
        dhcp->t2_timeout = (uint16)timeout;
        if (dhcp->t2_timeout == 0)
        {
            dhcp->t2_timeout = 1;
        }
    }
    /* copy offered network mask */
    ip_addr_set(&sn_mask, &dhcp->offered_sn_mask);

    /* subnet mask not given? */
    /* TODO: this is not a valid check. what if the network mask is 0? */
    if (sn_mask.addr == 0)
    {
        /* choose a safe subnet mask given the network class */
        uint8 first_octet = ip4_addr1(&sn_mask);
        if (first_octet <= 127)
        {
            sn_mask.addr = htonl(0xff000000);
        }
        else if (first_octet >= 192)
        {
            sn_mask.addr = htonl(0xffffff00);
        }
        else
        {
            sn_mask.addr = htonl(0xffff0000);
        }
    }

    ip_addr_set(&gw_addr, &dhcp->offered_gw_addr);
    /* gateway address not given? */
    if (gw_addr.addr == 0)
    {
        /* copy network address */
        gw_addr.addr = (dhcp->offered_ip_addr.addr & sn_mask.addr);
        /* use first host address on network as gateway */
        gw_addr.addr |= htonl(0x00000001);
    }

//    if (dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_ON)
//    {
//        autoip_stop(netif);
//        dhcp->autoip_coop_state = DHCP_AUTOIP_COOP_STATE_OFF;
//    }


    netif_set_ipaddr(netif, &dhcp->offered_ip_addr);

    netif_set_netmask(netif, &sn_mask);

    netif_set_gw(netif, &gw_addr);

    printf("ip=%d.%d.%d.%d\n",((IP_ADDR3 *)&dhcp->offered_ip_addr)->addrb[0],
                                           ((IP_ADDR3 *)&dhcp->offered_ip_addr)->addrb[1],
                                           ((IP_ADDR3 *)&dhcp->offered_ip_addr)->addrb[2],
                                           ((IP_ADDR3 *)&dhcp->offered_ip_addr)->addrb[3]);

    printf("mk=%d.%d.%d.%d\n",((IP_ADDR3 *)&sn_mask)->addrb[0],
                                           		((IP_ADDR3 *)&sn_mask)->addrb[1],
                                           		((IP_ADDR3 *)&sn_mask)->addrb[2],
                                           		((IP_ADDR3 *)&sn_mask)->addrb[3]);

    printf("gw=%d.%d.%d.%d\n",((IP_ADDR3 *)&gw_addr)->addrb[0],
                                           ((IP_ADDR3 *)&gw_addr)->addrb[1],
                                           ((IP_ADDR3 *)&gw_addr)->addrb[2],
                                           ((IP_ADDR3 *)&gw_addr)->addrb[3]);
    
    /* netif is now bound to DHCP leased address */
    dhcp_set_state(dhcp, DHCP_BOUND);

	/* notice the dhcp ok...*/
    netif_set_up();
    
    return;
}

/**
 * Renew an existing DHCP lease at the involved DHCP server.
 *
 * @param netif network interface which must renew its lease
 */
uint8 dhcp_renew(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    uint8 result;
    uint16 msecs;

    const char *p;
    dhcp_set_state(dhcp, DHCP_RENEWING);

    /* create and initialize the DHCP message header */
    result = dhcp_create_request(netif);
    if (result == ERR_OK)
    {
        dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
        dhcp_option_byte(dhcp, DHCP_REQUEST);

        dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
        dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));

        p = (const char*)netif->hostname;
        if (p != NULL)
        {
            dhcp_option(dhcp, DHCP_OPTION_HOSTNAME, strlen(p));
            while (*p)
            {
                dhcp_option_byte(dhcp, *p++);
            }
        }

        /* append DHCP message trailer */
        dhcp_option_trailer(dhcp);

        udp_connect(dhcp->pcb, &dhcp->server_ip_addr, DHCP_SERVER_PORT);
        udp_sendto_if(dhcp->pcb, dhcp->p_out, &dhcp->server_ip_addr, DHCP_SERVER_PORT, netif);
        dhcp_delete_request(netif);


    }

    dhcp->tries++;
    /* back-off on retries, but to a maximum of 20 seconds */
    msecs = dhcp->tries < 10 ? dhcp->tries * 2000 : 20 * 1000;
    dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;

    return result;
}

/**
 * Rebind with a DHCP server for an existing DHCP lease.
 *
 * @param netif network interface which must rebind with a DHCP server
 */
static uint8 dhcp_rebind(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    uint8 result;
    uint16 msecs;

    const char *p;

    dhcp_set_state(dhcp, DHCP_REBINDING);

    /* create and initialize the DHCP message header */
    result = dhcp_create_request(netif);
    if (result == ERR_OK)
    {
        dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
        dhcp_option_byte(dhcp, DHCP_REQUEST);

        dhcp_option(dhcp, DHCP_OPTION_MAX_MSG_SIZE, DHCP_OPTION_MAX_MSG_SIZE_LEN);
        dhcp_option_short(dhcp, DHCP_MAX_MSG_LEN(netif));

        p = (const char*)netif->hostname;
        if (p != NULL)
        {
            dhcp_option(dhcp, DHCP_OPTION_HOSTNAME, strlen(p));
            while (*p)
            {
                dhcp_option_byte(dhcp, *p++);
            }
        }
        dhcp_option_trailer(dhcp);

        /* broadcast to server */
        udp_connect(dhcp->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
        udp_sendto_if(dhcp->pcb, dhcp->p_out, IP_ADDR_BROADCAST, DHCP_SERVER_PORT, netif);
        dhcp_delete_request(netif);

    }

    dhcp->tries++;
    msecs = dhcp->tries < 10 ? dhcp->tries * 1000 : 10 * 1000;
    dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;

    return result;
}

/**
 * Release a DHCP lease.
 *
 * @param netif network interface which must release its lease
 */
uint8 dhcp_release(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
    uint8 result;
    uint16 msecs;

    /* idle DHCP client */
    dhcp_set_state(dhcp, DHCP_OFF);
    /* clean old DHCP offer */
    dhcp->server_ip_addr.addr = 0;
    dhcp->offered_ip_addr.addr = dhcp->offered_sn_mask.addr = 0;
    dhcp->offered_gw_addr.addr = dhcp->offered_bc_addr.addr = 0;
    dhcp->offered_t0_lease = dhcp->offered_t1_renew = dhcp->offered_t2_rebind = 0;
    dhcp->dns_count = 0;

    /* create and initialize the DHCP message header */
    result = dhcp_create_request(netif);
    if (result == ERR_OK)
    {
        dhcp_option(dhcp, DHCP_OPTION_MESSAGE_TYPE, DHCP_OPTION_MESSAGE_TYPE_LEN);
        dhcp_option_byte(dhcp, DHCP_RELEASE);

        dhcp_option_trailer(dhcp);

        udp_connect(dhcp->pcb, &dhcp->server_ip_addr, DHCP_SERVER_PORT);
        udp_sendto_if(dhcp->pcb, dhcp->p_out, &dhcp->server_ip_addr, DHCP_SERVER_PORT, netif);
        dhcp_delete_request(netif);

    }
    dhcp->tries++;
    msecs = dhcp->tries < 10 ? dhcp->tries * 1000 : 10 * 1000;
    dhcp->request_timeout = (msecs + DHCP_FINE_TIMER_MSECS - 1) / DHCP_FINE_TIMER_MSECS;

    netif_set_down();    
    /* remove IP address from interface */
    netif_set_ipaddr(netif, IP_ADDR_ANY);
    netif_set_gw(netif, IP_ADDR_ANY);
    netif_set_netmask(netif, IP_ADDR_ANY);

    /* TODO: netif_down(netif); */
    return result;
}

/**
 * Remove the DHCP client from the interface.
 *
 * @param netif The network interface to stop DHCP on
 */
void dhcp_stop(NETIF *netif)
{
    DHCP *dhcp = netif->dhcp;
	
    /* Remove the flag that says this netif is handled by DHCP. */
    netif->flags &= ~NETIF_FLAG_DHCP;

    /* netif is DHCP configured? */
    if (dhcp != NULL)
    {
        if (dhcp->p != NULL)
        {
            pbuf_free(dhcp->p);
        }
        /* free unfolded reply */
        dhcp_free_reply(dhcp);
        netif->dhcp = NULL;
    }
}

/*
 * Set the DHCP state of a DHCP client.
 *
 * If the state changed, reset the number of tries.
 *
 * TODO: we might also want to reset the timeout here?
 */
static void dhcp_set_state(DHCP *dhcp, uint8 new_state)
{
    if (new_state != dhcp->state)
    {
		printf("dhcp=%d\n", new_state);
        dhcp->state = new_state;
        dhcp->tries = 0;
    }
}

/*
 * Concatenate an option type and length field to the outgoing
 * DHCP message.
 *
 */
static void dhcp_option(DHCP *dhcp, uint8 option_type, uint8 option_len)
{
    dhcp->msg_out->options[dhcp->options_out_len++] = option_type;
    dhcp->msg_out->options[dhcp->options_out_len++] = option_len;
}
/*
 * Concatenate a single byte to the outgoing DHCP message.
 *
 */
static void dhcp_option_byte(DHCP *dhcp, uint8 value)
{
    dhcp->msg_out->options[dhcp->options_out_len++] = value;
}

static void dhcp_option_short(DHCP *dhcp, uint16 value)
{
    dhcp->msg_out->options[dhcp->options_out_len++] = (uint8)((value & 0xff00U) >> 8);
    dhcp->msg_out->options[dhcp->options_out_len++] = (uint8) (value & 0x00ffU);
}

static void dhcp_option_long(DHCP *dhcp, uint32 value)
{
    dhcp->msg_out->options[dhcp->options_out_len++] = (uint8)((value & 0xff000000UL) >> 24);
    dhcp->msg_out->options[dhcp->options_out_len++] = (uint8)((value & 0x00ff0000UL) >> 16);
    dhcp->msg_out->options[dhcp->options_out_len++] = (uint8)((value & 0x0000ff00UL) >> 8);
    dhcp->msg_out->options[dhcp->options_out_len++] = (uint8)((value & 0x000000ffUL));
}

/**
 * Extract the DHCP message and the DHCP options.
 *
 * Extract the DHCP message and the DHCP options, each into a contiguous
 * piece of memory. As a DHCP message is variable sized by its options,
 * and also allows overriding some fields for options, the easy approach
 * is to first unfold the options into a conitguous piece of memory, and
 * use that further on.
 *
 */
static uint8 dhcp_unfold_reply(DHCP *dhcp)
{
    /* free any left-overs from previous unfolds */
    dhcp_free_reply(dhcp);
	
    /* options present? */
    if (dhcp->p->len > (sizeof(DHCP_MSG) - DHCP_OPTIONS_LEN))
    {
        dhcp->options_in_len = dhcp->p->len - (sizeof(DHCP_MSG) - DHCP_OPTIONS_LEN);
        dhcp->options_in = (DHCP_MSG *)((uint8 *)dhcp->p->payload + (sizeof(DHCP_MSG) - DHCP_OPTIONS_LEN));
        if (dhcp->options_in == NULL)
        {
            return ERR_MEM;
        }
    }

    dhcp->msg_in = (DHCP_MSG *)dhcp->p->payload;
    if (dhcp->msg_in == NULL)
    {
        dhcp->options_in = NULL;
        return ERR_MEM;
    }

    return ERR_OK;
}

/**
 * Free the incoming DHCP message including contiguous copy of
 * its DHCP options.
 *
 */
static void dhcp_free_reply(DHCP *dhcp)
{
    if (dhcp->msg_in != NULL)
    {
        dhcp->msg_in = NULL;
    }
    if (dhcp->options_in)
    {
        dhcp->options_in = NULL;
        dhcp->options_in_len = 0;
    }
}


/**
 * If an incoming DHCP message is in response to us, then trigger the state machine
 */
static void dhcp_recv(void *arg, UDP_PCB *pcb, PBUF *p, IP_ADDR *addr, uint16 port)
{
    NETIF *netif = (NETIF *)arg;
    DHCP *dhcp = netif->dhcp;
    DHCP_MSG *reply_msg = (DHCP_MSG *)p->payload;
    uint8 *options_ptr;
    uint8 msg_type;
    uint8 i;

    dhcp->p = p;
    /* TODO: check packet length before reading them */
    if (reply_msg->op != DHCP_BOOTREPLY)
    {
        goto free_pbuf_and_return;
    }
    /* iterate through hardware address and match against DHCP message */
    for (i = 0; i < netif->hwaddr_len; i++)
    {
        if (netif->hwaddr[i] != reply_msg->chaddr[i])
        {
            goto free_pbuf_and_return;
        }
    }
    /* match transaction ID against what we expected */
    if (ntohl(reply_msg->xid) != dhcp->xid)
    {
        goto free_pbuf_and_return;
    }
    /* option fields could be unfold? */
    if (dhcp_unfold_reply(dhcp) != ERR_OK)
    {
        goto free_pbuf_and_return;
    }

    /* obtain pointer to DHCP message type */
    options_ptr = dhcp_get_option_ptr(dhcp, DHCP_OPTION_MESSAGE_TYPE);
    if (options_ptr == NULL)
    {
        goto free_pbuf_and_return;
    }

    /* read DHCP message type */
    msg_type = dhcp_get_option_byte(options_ptr + 2);//the 2 == messge list header length...
//printf("fle ---> %d %d\n", msg_type, dhcp->state);///2 6 = 5 1
	
    /* message type is DHCP ACK? */
    if (msg_type == DHCP_ACK)
    {
        /* in requesting state? */
        if (dhcp->state == DHCP_REQUESTING)//dhcp second step..
        {
            dhcp_handle_ack(netif);
            dhcp->request_timeout = 0;

            /* check if the acknowledged lease address is already in use */
            dhcp_check(netif);//confirm the lease promision.
        }
        /* already bound to the given lease address? */
        else if ((dhcp->state == DHCP_REBOOTING)   //release
				|| (dhcp->state == DHCP_REBINDING) //rebind
				|| (dhcp->state == DHCP_RENEWING)) //renew
        {
            dhcp->request_timeout = 0;
            dhcp_bind(netif);
        }
    }
    /* received a DHCP_NAK in appropriate state? */
    else if ((msg_type == DHCP_NAK) &&
             ((dhcp->state == DHCP_REBOOTING) || (dhcp->state == DHCP_REQUESTING) ||
              (dhcp->state == DHCP_REBINDING) || (dhcp->state == DHCP_RENEWING  )))
    {
        dhcp->request_timeout = 0;
        dhcp_handle_nak(netif);
    }
    /* received a DHCP_OFFER in DHCP_SELECTING state? */
    else if ((msg_type == DHCP_OFFER) && (dhcp->state == DHCP_SELECTING))//frist step dhcp
    {
        dhcp->request_timeout = 0;
        /* remember offered lease */
        dhcp_handle_offer(netif);//Offer IP lease address(get acv). Accept the IP lease promise(send udp).
    }
	
free_pbuf_and_return:
    dhcp_free_reply(dhcp);
    pbuf_free(p);
	dhcp->p = NULL;
	return;
}

/**
 * Create a DHCP request, fill in common headers
 *
 * @param netif the netif under DHCP control
 */
static uint8 dhcp_create_request(NETIF *netif)
{
    DHCP *dhcp;
    uint16 i;
    /** default global transaction identifier starting value (easy to match
     *  with a packet analyser). We simply increment for each new request.
     *  Predefine DHCP_GLOBAL_XID to a better value or a function call to generate one
     *  at runtime, any supporting function prototypes can be defined in DHCP_GLOBAL_XID_HEADER */
    static uint32 xid = 0xABCD0000;
	
    dhcp = netif->dhcp;
#ifdef DBUGPBUF
	printf("dhcp:");
#endif
    dhcp->p_out = pbuf_alloc(PBUF_TRANSPORT, sizeof(DHCP_MSG), PBUF_POOL);
    if (dhcp->p_out == NULL)
    {
        return ERR_MEM;
	}

    /* reuse transaction identifier in retransmissions */
    if (dhcp->tries == 0)
    {
        xid++;
	}
    dhcp->xid = xid;

    dhcp->msg_out = (DHCP_MSG *)dhcp->p_out->payload;

    dhcp->msg_out->op = DHCP_BOOTREQUEST;
    /* TODO: make link layer independent */
    dhcp->msg_out->htype = DHCP_HTYPE_ETH;
    /* TODO: make link layer independent */
    dhcp->msg_out->hlen = DHCP_HLEN_ETH;
    dhcp->msg_out->hops = 0;
    dhcp->msg_out->xid = htonl(dhcp->xid);
    dhcp->msg_out->secs = 0;
    dhcp->msg_out->flags = 0;
    dhcp->msg_out->ciaddr.addr = 0;
    if (dhcp->state == DHCP_BOUND || dhcp->state == DHCP_RENEWING || dhcp->state == DHCP_REBINDING)
    {
        dhcp->msg_out->ciaddr.addr = netif->ip_addr.addr;
	}

    dhcp->msg_out->yiaddr.addr = 0;
    dhcp->msg_out->siaddr.addr = 0;
    dhcp->msg_out->giaddr.addr = 0;
    for (i = 0; i < DHCP_CHADDR_LEN; i++)
    {
        /* copy netif hardware address, pad with zeroes */
        dhcp->msg_out->chaddr[i] = (i < netif->hwaddr_len) ? netif->hwaddr[i] : 0/* pad byte*/;
    }
    for (i = 0; i < DHCP_SNAME_LEN; i++)
    {
        dhcp->msg_out->sname[i] = 0;
    }
    for (i = 0; i < DHCP_FILE_LEN; i++)
    {
        dhcp->msg_out->file[i] = 0;
    }
    dhcp->msg_out->cookie = htonl(0x63825363UL);
    dhcp->options_out_len = 0;
    /* fill options field with an incrementing array (for debugging purposes) */
    for (i = 0; i < DHCP_OPTIONS_LEN; i++)
    {
		/* for debugging only, no matter if truncated */
        dhcp->msg_out->options[i] = (uint8)i; 
    }
    return ERR_OK;
}

/**
 * Free previously allocated memory used to send a DHCP request.
 *
 * @param netif the netif under DHCP control
 */
static void dhcp_delete_request(NETIF *netif)
{
    DHCP *dhcp;

    dhcp = netif->dhcp;

    if (dhcp->p_out != NULL)
    {
        pbuf_free(dhcp->p_out);
    }
    dhcp->p_out = NULL;
    dhcp->msg_out = NULL;
}

/**
 * Add a DHCP message trailer
 *
 * Adds the END option to the DHCP message, and if
 * necessary, up to three padding bytes.
 *
 * @param dhcp DHCP state structure
 */
static void dhcp_option_trailer(DHCP *dhcp)
{
    dhcp->msg_out->options[dhcp->options_out_len++] = DHCP_OPTION_END;
    /* packet is too small, or not 4 byte aligned? */
    while ((dhcp->options_out_len < DHCP_MIN_OPTIONS_LEN) || (dhcp->options_out_len & 3))
    {
        /* add a fill/padding byte */
        dhcp->msg_out->options[dhcp->options_out_len++] = 0;
    }
}

/**
 * Find the offset of a DHCP option inside the DHCP message.
 *
 * @param dhcp DHCP client
 * @param option_type
 *
 * @return a byte offset into the UDP message where the option was found, or
 * zero if the given option was not found.
 */
static uint8 *dhcp_get_option_ptr(DHCP *dhcp, uint8 option_type)
{
    uint8 overload = DHCP_OVERLOAD_NONE;

    /* options available? */
    if ((dhcp->options_in != NULL) && (dhcp->options_in_len > 0))
    {
        /* start with options field */
        uint8 *options = (uint8 *)dhcp->options_in;
        uint16 offset = 0;
        /* at least 1 byte to read and no end marker, then at least 3 bytes to read? */
        while ((offset < dhcp->options_in_len) && (options[offset] != DHCP_OPTION_END))
        {
            /* LWIP_DEBUGF(DHCP_DEBUG, ("msg_offset=%"U16_F", q->len=%"U16_F, msg_offset, q->len)); */
            /* are the sname and/or file field overloaded with options? */
            if (options[offset] == DHCP_OPTION_OVERLOAD)
            {
                /* skip option type and length */
                offset += 2;
                overload = options[offset++];
            }
            /* requested option found */
            else if (options[offset] == option_type)
            {
                return &options[offset];
                /* skip option */
            }
            else
            {
                /* skip option type */
                offset++;
                /* skip option length, and then length bytes */
                offset += 1 + options[offset];
            }
        }
        /* is this an overloaded message? */
        if (overload != DHCP_OVERLOAD_NONE)
        {
            uint16 field_len;
            if (overload == DHCP_OVERLOAD_FILE)
            {
                options = (uint8 *)&dhcp->msg_in->file;
                field_len = DHCP_FILE_LEN;
            }
            else if (overload == DHCP_OVERLOAD_SNAME)
            {
                options = (uint8 *)&dhcp->msg_in->sname;
                field_len = DHCP_SNAME_LEN;
                /* TODO: check if else if () is necessary */
            }
            else
            {
                options = (uint8 *)&dhcp->msg_in->sname;
                field_len = DHCP_FILE_LEN + DHCP_SNAME_LEN;
            }
			
            offset = 0;
            /* at least 1 byte to read and no end marker */
            while ((offset < field_len) && (options[offset] != DHCP_OPTION_END))
            {
                if (options[offset] == option_type)
                {
                    return &options[offset];
                    /* skip option */
                }
                else
                {
                    /* skip option type */
                    offset++;
                    offset += 1 + options[offset];
                }
            }
        }
    }
    return NULL;
}

/**
 * Return the byte of DHCP option data.
 *
 * @param client DHCP client.
 * @param ptr pointer obtained by dhcp_get_option_ptr().
 *
 * @return byte value at the given address.
 */
static uint8 dhcp_get_option_byte(uint8 *ptr)
{
    return *ptr;
}

void udp_user_use(void) {
	userPcb = udp_new(2);

	if(userPcb != NULL) {
		/* set up local and remote port for the pcb */
		udp_bind(userPcb, IP_ADDR_ANY, USER_CLIENT_PORT);
		udp_connect(userPcb, IP_ADDR_ANY, USER_SERVER_PORT);
	
		/* set up the recv callback and argument */
		udp_recv(userPcb, user_recv, NULL);
	}
}

UDP_PCB * get_udp_pcb() {
	return userPcb;
}

IP_ADDR get_ip_addr() {
	return dhcp.offered_ip_addr;
}

void send_user_data(UDP_PCB *pcb, IP_ADDR *addr, uint16 port, char *dat, int len) {
	IP_ADDR destaddr = *addr;
	PBUF *buff;
	char *buf;
	printf("fle-->port send = %d, %d\n", addr->addr, port);
	memset(buf, 0, len);
	buff = pbuf_alloc(PBUF_IP, PHY_FRAME_MAX_SIZE, PBUF_POOL);
	buf = (char *)buff->payload;
	memcpy(buf, dat, len);
	buff->len = len;
	udp_sendto(pcb, buff, &destaddr, USER_SERVER_PORT);
	free(buf);
	pbuf_free(buff);
}

int ack_enable(PBUF *p) {
	if(p->buf[33] == 0xff)
		return 0;
	return 0;
}

void printf_pbuf(PBUF *p) {
	int i;
	for(i = 0; i < 34; i++) {
		printf("%02x ", p->buf[i]);
	}
	printf("\n");
	for(i = 34; i < 34 + p->len; i++) {
		printf("%02x ", p->buf[i]);
	}
}

static void user_recv(void *arg, UDP_PCB *pcb, PBUF *p, IP_ADDR *addr, uint16 port) {
	int i;
	uint8 *start;
	UDP_HDR *h;
	uint8 *dat;
	uint16 len;
	uint8 output[600] = {0x11, 0x22};

	if(p != NULL) {
		if(ack_enable(p) != 1) {
			start = p->buf + PBUF_IP_HLEN + PBUF_ETHNET_HLEN;
			h = (UDP_HDR *)start;
			dat = (uint8 *)p->payload;
			len = analytical_command(output, dat, htons(h->len) - UDP_HLEN);
			if(len)
				send_user_data(pcb, addr, port, output, len);
		}
	}
	pbuf_free(p);
}

/**
 * Return the 32-bit value of DHCP option data.
 *
 * @param client DHCP client.
 * @param ptr pointer obtained by dhcp_get_option_ptr().
 *
 * @return byte value at the given address.
 */
static uint32 dhcp_get_option_long(uint8 *ptr)
{
    uint32 value;
    value  = (uint32)(*ptr++) << 24;
    value |= (uint32)(*ptr++) << 16;
    value |= (uint32)(*ptr++) << 8;
    value |= (uint32)(*ptr++);
    return value;
}						   
