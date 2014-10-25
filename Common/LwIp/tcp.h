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
#ifndef __LWIP_TCP_H__
#define __LWIP_TCP_H__

#if LWIP_TCP /* don't build if not configured for use in lwipopts.h */

/* Flags for "apiflags" parameter in tcp_write and tcp_enqueue */
#define TCP_WRITE_FLAG_COPY 0x01
#define TCP_WRITE_FLAG_MORE 0x02
#define tcp_mss(pcb)      ((pcb)->mss)
#define tcp_sndbuf(pcb)   ((pcb)->snd_buf)
#define tcp_abort(pcb) tcp_abandon((pcb), 1)
#define TCP_PRIO_MIN    1
#define TCP_PRIO_NORMAL 64
#define TCP_PRIO_MAX    127
#define TF_SEG_OPTS_MSS   (uint8)0x01U   /* Include MSS option. */
#define TF_SEG_OPTS_TS    (uint8)0x02U   /* Include timestamp option. */
#define TCP_CONNECT_TIMEOUT      10

void tcp_tmr(void);  /* Must be called every TCP_TMR_INTERVAL ms. (Typically 250 ms). */

/* It is also possible to call these two functions at the right
   intervals (instead of calling tcp_tmr()). */
void  tcp_slowtmr (void);
void  tcp_fasttmr (void);

/* Application program's interface: */
extern void  tcp_pcb_init(void);
TCP_PCB * tcp_new     (void);
TCP_PCB * tcp_alloc   (uint8 prio);
TCP_PCB * tcp_curget(void);

uint8 tcp_segs_free(PBUF *seg);
uint8 tcp_seg_free(PBUF *seg);

void tcp_recv(TCP_PCB *pcb, void (* recv)(PBUF *p));
void tcp_recved (TCP_PCB *pcb, uint16 len);
uint8 tcp_bind (TCP_PCB *pcb, IP_ADDR *ipaddr, uint16 port);
uint8 tcp_connect (TCP_PCB *pcb, IP_ADDR *ipaddr, uint16 port);
void  tcp_abandon (TCP_PCB *pcb, uint16 reset);
uint8 tcp_close (TCP_PCB *pcb);
uint16 tcp_read_firstframe(TCP_PCB *pcb, char *dst);
uint16 tcp_set_firstframe(TCP_PCB *pcb, PBUF * p);
uint8  tcp_write (TCP_PCB *pcb, const void *data, uint16 len);
//uint16 tcp_read(TCP_PCB *pcb, char *dstdata, uint16 len);
void  tcp_setprio (TCP_PCB *pcb, uint8 prio);

/* Only used by IP to pass a TCP segment to TCP: */
void  tcp_input   (PBUF *p, NETIF *inp);
/* Used within the TCP code only: */
uint8 tcp_output  (TCP_PCB *pcb);
void  tcp_rexmit  (TCP_PCB *pcb);
void  tcp_rexmit_rto  (TCP_PCB *pcb);
uint32 tcp_update_rcv_ann_wnd(TCP_PCB *pcb);

void tcp_pcb_purge_unread(TCP_PCB *pcb);

/**
 * This is the Nagle algorithm: try to combine user data to send as few TCP
 * segments as possible. Only send if
 * - no previously transmitted data on the connection remains unacknowledged or
 * - the TF_NODELAY flag is set (nagle algorithm turned off for this pcb) or
 * - the only unsent segment is at least pcb->mss bytes long (or there is more
 *   than one unsent segment - with lwIP, this can happen although unsent->len < mss)
 */
#define tcp_do_output_nagle(tpcb) ((((tpcb)->unacked == NULL) || \
                            ((tpcb)->flags & TF_NODELAY) || \
                            (((tpcb)->unsent != NULL) && (((tpcb)->unsent->next != NULL) || \
                              ((tpcb)->unsent->len >= (tpcb)->mss))) \
                            ) ? 1 : 0)
#define tcp_output_nagle(tpcb) (tcp_do_output_nagle(tpcb) ? tcp_output(tpcb) : ERR_OK)


#define TCP_SEQ_LT(a,b)     ((int32)((a)-(b)) < 0)
#define TCP_SEQ_LEQ(a,b)    ((int32)((a)-(b)) <= 0)
#define TCP_SEQ_GT(a,b)     ((int32)((a)-(b)) > 0)
#define TCP_SEQ_GEQ(a,b)    ((int32)((a)-(b)) >= 0)
/* is b<=a<=c? */


#define TCP_SEQ_BETWEEN(a,b,c) (TCP_SEQ_GEQ(a,b) && TCP_SEQ_LEQ(a,c))
#define TCP_FIN 0x01U
#define TCP_SYN 0x02U
#define TCP_RST 0x04U
#define TCP_PSH 0x08U
#define TCP_ACK 0x10U
#define TCP_URG 0x20U
#define TCP_ECE 0x40U
#define TCP_CWR 0x80U

#define TCP_FLAGS 0x3fU

/* Length of the TCP header, excluding options. */
#define TCP_HLEN 20

#ifndef TCP_TMR_INTERVAL
#define TCP_TMR_INTERVAL       250  /* The TCP timer interval in milliseconds. */
#endif /* TCP_TMR_INTERVAL */

#ifndef TCP_FAST_INTERVAL
#define TCP_FAST_INTERVAL      TCP_TMR_INTERVAL /* the fine grained timeout in milliseconds */
#endif /* TCP_FAST_INTERVAL */

#ifndef TCP_SLOW_INTERVAL
#define TCP_SLOW_INTERVAL      (TCP_TMR_INTERVAL << 1)  /* the coarse grained timeout in milliseconds */
#endif /* TCP_SLOW_INTERVAL */

#define TCP_FIN_WAIT_TIMEOUT 20000 /* milliseconds */
#define TCP_SYN_RCVD_TIMEOUT 20000 /* milliseconds */

#define TCP_OOSEQ_TIMEOUT        6U /* x RTO */

#ifndef TCP_MSL
#define TCP_MSL 60000UL /* The maximum segment lifetime in milliseconds */
#endif

/* Keepalive values, compliant with RFC 1122. Don't change this unless you know what you're doing */
#ifndef  TCP_KEEPIDLE_DEFAULT
#define  TCP_KEEPIDLE_DEFAULT     7200000UL /* Default KEEPALIVE timer in milliseconds */
#endif

#ifndef  TCP_KEEPINTVL_DEFAULT
#define  TCP_KEEPINTVL_DEFAULT    75000UL   /* Default Time between KEEPALIVE probes in milliseconds */
#endif

#ifndef  TCP_KEEPCNT_DEFAULT
#define  TCP_KEEPCNT_DEFAULT      9U        /* Default Counter for KEEPALIVE probes */
#endif

#define  TCP_MAXIDLE              TCP_KEEPCNT_DEFAULT * TCP_KEEPINTVL_DEFAULT  /* Maximum KEEPALIVE probe time */


/* Fields are (of course) in network byte order.
 * Some fields are converted to host byte order in tcp_input().
 */
#define TCPH_OFFSET(phdr) (ntohs((phdr)->_hdrlen_rsvd_flags) >> 8)
#define TCPH_HDRLEN(phdr) (ntohs((phdr)->_hdrlen_rsvd_flags) >> 12)
#define TCPH_FLAGS(phdr)  (ntohs((phdr)->_hdrlen_rsvd_flags) & TCP_FLAGS)

#define TCPH_OFFSET_SET(phdr, offset) (phdr)->_hdrlen_rsvd_flags = htons(((offset) << 8) | TCPH_FLAGS(phdr))
#define TCPH_HDRLEN_SET(phdr, len) (phdr)->_hdrlen_rsvd_flags = htons(((len) << 12) | TCPH_FLAGS(phdr))
#define TCPH_FLAGS_SET(phdr, flags) (phdr)->_hdrlen_rsvd_flags = htons((ntohs((phdr)->_hdrlen_rsvd_flags) & ~TCP_FLAGS) | (flags))
#define TCPH_SET_FLAG(phdr, flags ) (phdr)->_hdrlen_rsvd_flags = htons(ntohs((phdr)->_hdrlen_rsvd_flags) | (flags))
#define TCPH_UNSET_FLAG(phdr, flags) (phdr)->_hdrlen_rsvd_flags = htons(ntohs((phdr)->_hdrlen_rsvd_flags) | (TCPH_FLAGS(phdr) & ~(flags)) )

#define TCP_TCPLEN(seg) ((seg)->tcplen + (( TCPH_FLAGS((seg)->tcphdr) & TCP_FIN || TCPH_FLAGS((seg)->tcphdr) & TCP_SYN )? 1: 0))


/** Flags used on input processing, not on pcb->flags
*/
#define TF_RESET     (uint8)0x08U   /* Connection was reset. */
#define TF_CLOSED    (uint8)0x10U   /* Connection was sucessfully closed. */
#define TF_GOT_FIN   (uint8)0x20U   /* Connection was closed by the remote end. */
#define TF_ACK_DELAY   ((uint8)0x01U)   /* Delayed ACK. */
#define TF_ACK_NOW     ((uint8)0x02U)   /* Immediate ACK. */
#define TF_INFR        ((uint8)0x04U)   /* In fast recovery. */
#define TF_TIMESTAMP   ((uint8)0x08U)   /* Timestamp option enabled */
#define TF_FIN         ((uint8)0x20U)   /* Connection was closed locally (FIN segment enqueued). */
#define TF_NODELAY     ((uint8)0x40U)   /* Disable Nagle algorithm */
#define TF_NAGLEMEMERR ((uint8)0x80U)   /* nagle enabled, memerr, try to output to prevent delayed ACK to happen */
#define TCP_SNDQUEUELEN_OVERFLOW (0xffff-3)


#define LWIP_TCP_OPT_LENGTH(flags)              \
  (flags & TF_SEG_OPTS_MSS ? 4  : 0) +          \
  (flags & TF_SEG_OPTS_TS  ? 10 : 0)

/** This returns a TCP header option for MSS in an uint32 */
#define TCP_BUILD_MSS_OPTION(x) (x) = htonl(((uint32)2 << 24) |          \
                                            ((uint32)4 << 16) |          \
                                            (((uint32)TCP_MSS / 256) << 8) | \
                                            (TCP_MSS & 255))

/* Internal functions and global variables: */
TCP_PCB *tcp_pcb_copy(TCP_PCB *pcb);
void tcp_pcb_purge(TCP_PCB *pcb);
void tcp_pcb_remove(TCP_PCB **pcblist, TCP_PCB *pcb);


#define tcp_tick_ack_enable(pcb)    (pcb)->timerackflag = 1                      
#define tcp_tick_ack_unable(pcb)    (pcb)->timerackflag = 0                      

#define tcp_ack(pcb)                               \
  do                                               \
  {                                                \
	if ((pcb)->flags & TF_ACK_DELAY)               \
	{                                              \
		(pcb)->flags &= ~TF_ACK_DELAY;             \
		(pcb)->flags |= TF_ACK_NOW;                \
		if ((pcb)->timerackflag != 0)               \
		{                                          \
			(pcb)->timerackflag = 1;               \
		}                                          \
		tcp_output(pcb);                           \
	}                                              \
	else                                           \
	{                                              \
		(pcb)->flags |= TF_ACK_DELAY;              \
	}                                              \
  } while(0)

#define tcp_ack_now(pcb)                           \
  do                                               \
  {                                                \
	(pcb)->flags |= TF_ACK_NOW;                    \
	if ((pcb)->timerackflag != 0)                  \
	{                                              \
		(pcb)->timerackflag = 1;                   \
	}                                              \
	tcp_output(pcb);                               \
  } while(0)


#define tcp_ack_tail(pcb)                          \
  do                                               \
  {                                                \
    if ((pcb)->flags & TF_ACK_DELAY)               \
	{                                              \
       	(pcb)->flags &= ~TF_ACK_DELAY;             \
       	(pcb)->flags |= TF_ACK_NOW;                \
		if ((pcb)->timerackflag != 0)              \
		{                                          \
			(pcb)->timerackflag = 1;               \
		}                                          \
       	tcp_output(pcb);                           \
    }                                              \
  } while(0)

uint8 tcp_send_ctrl(TCP_PCB *pcb, uint8 flags);
uint8 tcp_enqueue(TCP_PCB *pcb, void *arg, uint16 len, uint8 flags, uint8 apiflags, uint8 optflags);

void tcp_rexmit_seg(TCP_PCB *pcb, PBUF *seg);

void tcp_rst(uint32 seqno, uint32 ackno,
             IP_ADDR *local_ip, IP_ADDR *remote_ip,
             uint16 local_port, uint16 remote_port);

uint32 tcp_next_iss(void);

extern void tcp_pcb_remove_nolist(TCP_PCB*pcb);
void tcp_keepalive(TCP_PCB *pcb);
void tcp_zero_window_probe(TCP_PCB *pcb);

#if TCP_CALCULATE_EFF_SEND_MSS
uint16 tcp_eff_send_mss(uint16 sendmss, struct ip_addr *addr);
#endif /* TCP_CALCULATE_EFF_SEND_MSS */

extern TCP_PCB *tcp_input_pcb;
extern uint32 tcp_ticks;

#if NO_SYS
#define tcp_timer_needed()
#else
void tcp_timer_needed(void);
#endif

extern TCP_PCB stTcpPcb;


/* Axioms about the above lists:
   1) Every TCP PCB that is not CLOSED is in one of the lists.
   2) A PCB is only in one of the lists.
   3) All PCBs in the tcp_listen_pcbs list is in LISTEN state.
   4) All PCBs in the tcp_tw_pcbs list is in TIME-WAIT state.
*/

/* Define two macros, TCP_REG and TCP_RMV that registers a TCP PCB
   with a PCB list or removes a PCB from a list, respectively. */
#if 0
#define TCP_REG(pcbs, npcb) do {\
                            LWIP_DEBUGF(TCP_DEBUG, ("TCP_REG %p local port %d\n", npcb, npcb->local_port)); \
                            for(tcp_tmp_pcb = *pcbs; \
          tcp_tmp_pcb != NULL; \
        tcp_tmp_pcb = tcp_tmp_pcb->next) { \
                                LWIP_ASSERT("TCP_REG: already registered\n", tcp_tmp_pcb != npcb); \
                            } \
                            LWIP_ASSERT("TCP_REG: pcb->state != CLOSED", npcb->state != CLOSED); \
                            npcb->next = *pcbs; \
                            LWIP_ASSERT("TCP_REG: npcb->next != npcb", npcb->next != npcb); \
                            *(pcbs) = npcb; \
              tcp_timer_needed(); \
                            } while(0)
#define TCP_RMV(pcbs, npcb) do { \
                            LWIP_ASSERT("TCP_RMV: pcbs != NULL", *pcbs != NULL); \
                            LWIP_DEBUGF(TCP_DEBUG, ("TCP_RMV: removing %p from %p\n", npcb, *pcbs)); \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(tcp_tmp_pcb = *pcbs; tcp_tmp_pcb != NULL; tcp_tmp_pcb = tcp_tmp_pcb->next) { \
                               if(tcp_tmp_pcb->next != NULL && tcp_tmp_pcb->next == npcb) { \
                                  tcp_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            LWIP_DEBUGF(TCP_DEBUG, ("TCP_RMV: removed %p from %p\n", npcb, *pcbs)); \
                            } while(0)

#else /* LWIP_DEBUG */

#define TCP_REG(pcbs, npcb)                        \
  do {                                             \
    npcb->next = *pcbs;                            \
    *(pcbs) = npcb;                                \
    tcp_timer_needed();                            \
  } while (0)

#define TCP_RMV(pcbs, npcb)                        \
  do {                                             \
    if(*(pcbs) == npcb) {                          \
      (*(pcbs)) = (*pcbs)->next;                   \
    }                                              \
    else {                                         \
      for(tcp_tmp_pcb = *pcbs;                                         \
          tcp_tmp_pcb != NULL;                                         \
          tcp_tmp_pcb = tcp_tmp_pcb->next) {                           \
        if(tcp_tmp_pcb->next != NULL && tcp_tmp_pcb->next == npcb) {   \
          tcp_tmp_pcb->next = npcb->next;          \
          break;                                   \
        }                                          \
      }                                            \
    }                                              \
    npcb->next = NULL;                             \
  } while(0)

#endif /* LWIP_DEBUG */


#endif /* LWIP_TCP */

#endif /* __LWIP_TCP_H__ */
