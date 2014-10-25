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
#ifndef __LWIP_ICMP_H__
#define __LWIP_ICMP_H__

#if LWIP_ICMP /* don't build if not configured for use in lwipopts.h */


#define ICMP_ER 0      /* echo reply */
#define ICMP_DUR 3     /* destination unreachable */
#define ICMP_SQ 4      /* source quench */
#define ICMP_RD 5      /* redirect */
#define ICMP_ECHO 8    /* echo */
#define ICMP_TE 11     /* time exceeded */
#define ICMP_PP 12     /* parameter problem */
#define ICMP_TS 13     /* timestamp */
#define ICMP_TSR 14    /* timestamp reply */
#define ICMP_IRQ 15    /* information request */
#define ICMP_IR 16     /* information reply */

enum icmp_dur_type
{
    ICMP_DUR_NET = 0,    /* net unreachable */
    ICMP_DUR_HOST = 1,   /* host unreachable */
    ICMP_DUR_PROTO = 2,  /* protocol unreachable */
    ICMP_DUR_PORT = 3,   /* port unreachable */
    ICMP_DUR_FRAG = 4,   /* fragmentation needed and DF set */
    ICMP_DUR_SR = 5      /* source route failed */
};

enum icmp_te_type
{
    ICMP_TE_TTL = 0,     /* time to live exceeded in transit */
    ICMP_TE_FRAG = 1     /* fragment reassembly time exceeded */
};

void icmp_input(PBUF *p, NETIF *inp);

void icmp_dest_unreach(PBUF *p, enum icmp_dur_type t);
void icmp_time_exceeded(PBUF *p, enum icmp_te_type t);


/** This is the standard ICMP header only that the uint32 data
 *  is splitted to two uint16 like ICMP echo needs it.
 *  This header is also used for other ICMP types that do not
 *  use the data part.
 */
typedef __packed struct _iCMP_ECHO_HDR
{
    uint8 type;
    uint8 code;
    uint16 chksum;
    uint16 id;
    uint16 seqno;
} ICMP_ECHO_HDR;


#define ICMPH_TYPE(hdr) ((hdr)->type)
#define ICMPH_CODE(hdr) ((hdr)->code)

/** Combines type and code to an uint16 */
#define ICMPH_TYPE_SET(hdr, t) ((hdr)->type = (t))
#define ICMPH_CODE_SET(hdr, c) ((hdr)->code = (c))


#endif /* LWIP_ICMP */

#endif /* __LWIP_ICMP_H__ */