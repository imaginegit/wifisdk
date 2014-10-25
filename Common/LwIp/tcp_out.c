/**
 * @file
 * Transmission Control Protocol, outgoing traffic
 *
 * The output functions of TCP.
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

#include "pbuf.h"
#include "tcp.h"
#include "ip_addr.h"
#include "ip.h"
#include "netif.h"
#include "inet.h"
#include "inet_chksum.h"


#include <string.h>

/* Forward declarations.*/
static void tcp_output_segment(PBUF *seg, TCP_PCB *pcb);

static TCP_HDR * tcp_output_set_header(TCP_PCB *pcb, PBUF *p, int optlen,
                      uint32 seqno_be /* already in network byte order */)
{
    TCP_HDR *tcphdr = p->payload;
    tcphdr->src = htons(pcb->local_port);
    tcphdr->dest = htons(pcb->remote_port);
    tcphdr->seqno = seqno_be;
    tcphdr->ackno = htonl(pcb->rcv_nxt);
    TCPH_FLAGS_SET(tcphdr, TCP_ACK);
    tcphdr->wnd = htons(pcb->rcv_ann_wnd);//htons(pcb->rcv_ann_wnd); TCP_TEST_WIN_SIZE
    tcphdr->chksum = 0;
    tcphdr->urgp = NULL;
    TCPH_HDRLEN_SET(tcphdr, (5 + optlen / 4));

    /* If we're sending a packet, update the announced right window edge */
    pcb->rcv_ann_right_edge = pcb->rcv_nxt + pcb->rcv_ann_wnd;

    return tcphdr;
}

/**
 * Called by tcp_close() to send a segment including flags but not data.
 *
 * @param pcb the tcp_pcb over which to send a segment
 * @param flags the flags to set in the segment header
 * @return ERR_OK if sent, another err_t otherwise
 */
uint8 tcp_send_ctrl(TCP_PCB *pcb, uint8 flags)
{
    /* no data, no length, flags, copy=1, no optdata */
    return tcp_enqueue(pcb, NULL, 0, flags, TCP_WRITE_FLAG_COPY, 0);
}

uint16 tcp_read_firstframe(TCP_PCB *pcb, char *dst)
{
	PBUF *p;
	uint16 len;	
	
	if (pcb == NULL)
	{
		pcb = (TCP_PCB *)&stTcpPcb;
	}
	
    if ((pcb->unRead == NULL) || (pcb->unRead->len == 0))
        return 0;
	
	len = pcb->unRead->len;
	memcpy(dst, pcb->unRead->payload, len);
	
	p = pcb->unRead;
	pcb->unRead = pcb->unRead->next;
	pbuf_free(p);
	
    return len;
}

uint16 tcp_set_firstframe(TCP_PCB *pcb, PBUF * p)
{
	PBUF *unRead;

	if (p == NULL)
		return -1 ;

	if (pcb == NULL)
	{
		pcb = (TCP_PCB *)&stTcpPcb;
	}
    if (pcb->unRead == NULL)
    {
		pcb->unRead = p;
		p->next = NULL;
	}
	else
	{
		p->next = pcb->unRead;
		pcb->unRead = p;
	}
    return 0;    
}


/**
 * Write data for sending (but does not send it immediately).
 *
 * It waits in the expectation of more data being sent soon (as
 * it can send them more efficiently by combining them together).
 * To prompt the system to send data now, call tcp_output() after
 * calling tcp_write().
 *
 * @param pcb Protocol control block of the TCP connection to enqueue data for.
 * @param data pointer to the data to send
 * @param len length (in bytes) of the data to send
 * @param apiflags combination of following flags :
 * - TCP_WRITE_FLAG_COPY (0x01) data will be copied into memory belonging to the stack
 * - TCP_WRITE_FLAG_MORE (0x02) for TCP connection, PSH flag will be set on last segment sent,
 * @return ERR_OK if enqueued, another err_t on error
 *
 * @see tcp_write()
 */
uint8 tcp_write(TCP_PCB *pcb, const void *data, uint16 len)
{
    /* connection is in valid state for data transmission? */
    if (pcb->state == ESTABLISHED)
    {
        if (len > 0)
        {
            tcp_enqueue(pcb, (void *)data, len, 0, TCP_WRITE_FLAG_COPY, 0);
			return tcp_output(pcb);
        }
        return ERR_OK;
    }
	
    return ERR_CONN;
}

#if 0
/**
 * Write data for sending (but does not send it immediately).
 *
 * It waits in the expectation of more data being sent soon (as
 * it can send them more efficiently by combining them together).
 * To prompt the system to send data now, call tcp_output() after
 * calling tcp_write().
 *
 * @param pcb Protocol control block of the TCP connection to enqueue data for.
 * @param data pointer to the data to send
 * @param len length (in bytes) of the data to send
 * @param apiflags combination of following flags :
 * - TCP_WRITE_FLAG_COPY (0x01) data will be copied into memory belonging to the stack
 * - TCP_WRITE_FLAG_MORE (0x02) for TCP connection, PSH flag will be set on last segment sent,
 * @return ERR_OK if enqueued, another err_t on error
 *
 * @see tcp_write()
 */
uint16 tcp_read(TCP_PCB *pcb, char *dstdata, uint16 len)
{
    /* connection is in valid state for data transmission? */
    PBUF *unread;
    int hasreadlen = 0;
    uint16 count;
    uint8 read_pbuf_count = 0;

	if (pcb == NULL)
	{
		pcb = (TCP_PCB *)&stTcpPcb;
	}
	
    if (pcb->state != ESTABLISHED)
    {
        return 0;
    }

    read_pbuf_count = 0;
    while(len > 0)
    {
        unread = pcb->unRead;
        if (unread == NULL)
        {
            return hasreadlen;
        }

		count = ((len < unread->len)? len : unread->len);

		if (dstdata != NULL)
		{
	        memcpy(dstdata + hasreadlen, unread->payload, count);
		}
        hasreadlen += count;
        len -= count; 
        if (count == unread->len)
        {
            pcb->unRead = unread->next;
            read_pbuf_count++;
            pbuf_free(unread);
        }
        else
        {
            unread->payload = (char *)unread->payload + count;
            unread->len -= count; 
        }
    }

    if (read_pbuf_count)
    {
        tcp_recved(pcb, TCP_MSS*read_pbuf_count); 
    }
    return hasreadlen;
}    
#endif

/**
 * Enqueue data and/or TCP options for transmission
 *
 * Called by tcp_connect(), tcp_listen_input(), tcp_send_ctrl() and tcp_write().
 *
 * @param pcb Protocol control block for the TCP connection to enqueue data for.
 * @param arg Pointer to the data to be enqueued for sending.
 * @param len Data length in bytes
 * @param flags tcp header flags to set in the outgoing segment
 * @param apiflags combination of following flags :
 * - TCP_WRITE_FLAG_COPY (0x01) data will be copied into memory belonging to the stack
 * - TCP_WRITE_FLAG_MORE (0x02) for TCP connection, PSH flag will be set on last segment sent,
 * @param optflags options to include in segment later on (see definition of struct tcp_seg)
 */
uint8 tcp_enqueue(TCP_PCB *pcb, void *arg, uint16 len, uint8 flags, uint8 apiflags, uint8 optflags)
{
    PBUF *p;
    PBUF *seg, *useg, *queue;
    uint32 seqno;
    uint16 left, seglen;
    void *ptr;
    uint16 queuelen;
    uint8 optlen;


    /* fail on too much data */
    if (len > pcb->snd_buf)
    {
        pcb->flags |= TF_NAGLEMEMERR;
        return ERR_MEM;
    }
    left = len;
    ptr = arg;

    optlen = LWIP_TCP_OPT_LENGTH(optflags);

    /* seqno will be the sequence number of the first segment enqueued
     * by the call to this function. */
    seqno = pcb->snd_lbb;

    /* If total number of pbufs on the unsent/unacked queues exceeds the
     * configured maximum, return an error */
    queuelen = pcb->snd_queuelen;
    /* check for configured max queuelen and possible overflow */
    if ((queuelen >= TCP_SND_QUEUELEN) || (queuelen > TCP_SNDQUEUELEN_OVERFLOW))
    {
        pcb->flags |= TF_NAGLEMEMERR;
        return ERR_MEM;
    }

    /* First, break up the data into segments and tuck them together in
     * the local "queue" variable. */
    useg = queue = seg = NULL;
    seglen = 0;
    while (queue == NULL || left > 0)
    {
        /* The segment length (including options) should be at most the MSS */
        seglen = left > (pcb->mss - optlen) ? (pcb->mss - optlen) : left;

        /* Allocate memory for tcp_seg, and fill in fields. */
	#ifdef DBUGPBUF
		printf("tcp:");
	#endif
        seg = pbuf_alloc(PBUF_TRANSPORT, seglen+optlen, PBUF_POOL); //reserved options space its length is optlen.. 
        if (seg == NULL)
            goto memerr;
        seg->next = NULL;

        /* first segment of to-be-queued data? */
        if (queue == NULL)
        {
            queue = seg;
        }
        /* subsequent segments of to-be-queued data */
        else
        {
            /* Attach the segment to the end of the queued segments */
            useg->next = seg;
        }
        /* remember last segment of to-be-queued data for next iteration */
        useg = seg;

        /* If copy is set, memory should be allocated
         * and data copied into pbuf, otherwise data comes from
         * ROM or other static memory, and need not be copied.  */
        if (apiflags & TCP_WRITE_FLAG_COPY)
        {
            queuelen += 1;
            if (arg != NULL)
            {
                MEMCPY((char *)seg->payload + optlen, ptr, seglen);
            }
        }

        /* Now that there are more segments queued, we check again if the
        length of the queue exceeds the configured maximum or overflows. */
        if ((queuelen > TCP_SND_QUEUELEN) || (queuelen > TCP_SNDQUEUELEN_OVERFLOW))
            goto memerr;

        seg->tcplen = seglen; //tcp packet data valid length.

        /* build TCP header */
        if (pbuf_header(seg, TCP_HLEN) == 0) 
            goto memerr;

        seg->tcphdr = seg->payload;
        seg->tcphdr->src = htons(pcb->local_port);
        seg->tcphdr->dest = htons(pcb->remote_port);
        seg->tcphdr->seqno = htonl(seqno);
        seg->tcphdr->urgp = NULL;//urgent pointer
        TCPH_FLAGS_SET(seg->tcphdr, flags);
        /* don't fill in tcphdr->ackno and tcphdr->wnd until later */
        seg->flags = optflags; //flag some things,as the TCP packet has option data(mss size etc.)
        /* Set the length of the header */
        TCPH_HDRLEN_SET(seg->tcphdr, (5 + optlen / 4));

        left -= seglen;
        seqno += seglen;
        ptr = (void *)((uint8 *)ptr + seglen);
    }

    /* Now that the data to be enqueued has been broken up into TCP
    segments in the queue variable, we add them to the end of the
    pcb->unsent queue. */
    if (pcb->unsent == NULL)
    {
        /* initialize list with this segment */
        pcb->unsent = queue;
    }
    else
    {
		useg = pcb->unsent;
		while (useg->next != NULL)
		{
			useg = useg->next;
		}
        useg->next = queue;
    }


    if ((flags & TCP_SYN) || (flags & TCP_FIN))
        ++len;

    if (flags & TCP_FIN)
    {
         printf("give the fin !!\n");
         pcb->flags |= TF_FIN;
    }
    pcb->snd_lbb += len;

    pcb->snd_buf -= len;

    /* update number of segments on the queues */
    pcb->snd_queuelen = queuelen;

    /* Set the PSH flag in the last segment that we enqueued, but only
    if the segment has data (indicated by seglen > 0). */
    if (seg != NULL && seglen > 0 && seg->tcphdr != NULL && ((apiflags & TCP_WRITE_FLAG_MORE) == 0))
    {
		/* TCP_WRITE_FLAG_MORE:建议计算机立即将数据交给应用程序。*/
        TCPH_SET_FLAG(seg->tcphdr, TCP_PSH);
    }

    return ERR_OK;
memerr:
    pcb->flags |= TF_NAGLEMEMERR;

    tcp_segs_free(queue);

    return ERR_MEM;
}


/**
 * Find out what we can send and send it
 *
 * @param pcb Protocol control block for the TCP connection to send data
 * @return ERR_OK if data has been sent or nothing to send
 *         another err_t on error
 */
uint8 tcp_output(TCP_PCB *pcb)
{
    PBUF *p;
    TCP_HDR *tcphdr;
    PBUF *seg, *useg;

    void *oldpayload;
    uint16 oldlen;
    uint32 wnd, snd_nxt;

    /* First, check if we are invoked by the TCP input processing
       code. If so, we do not output anything. Instead, we rely on the
       input processing code to call us when input processing is done
       with. */

    if (tcp_input_pcb == pcb)
    {
        return ERR_OK;
    }

    wnd = LWIP_MIN(pcb->snd_wnd, pcb->cwnd);

    /* If the TF_ACK_NOW flag is set and no data will be sent (either
     * because the ->unsent queue is empty or because the window does
     * not allow it), construct an empty ACK segment and send it.
     *
     * If data is to be sent, we will just piggyback the ACK (see below).
     */
    seg = pcb->unsent;
    if ((pcb->flags & TF_ACK_NOW) 
		&& ((seg == NULL) || (ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->tcplen > wnd)))
    {
	#ifdef DBUGPBUF
		printf("tcpo:");
	#endif
        p = pbuf_alloc(PBUF_IP, TCP_HLEN, PBUF_POOL);
        if (p == NULL)
        {
            return ERR_BUF;         
        }
        /* remove ACK flags from the PCB, as we send an empty ACK now */
        pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);

        tcphdr = tcp_output_set_header(pcb, p, 0, htonl(pcb->snd_nxt));

        tcphdr->chksum = inet_chksum_pseudo(p, &(pcb->local_ip), &(pcb->remote_ip), IP_PROTO_TCP, p->len);

        //printf("ACK:ackno=%010u, seqno=%010u\n", pcb->rcv_nxt, pcb->snd_nxt);

        ip_output(p, &(pcb->local_ip), &(pcb->remote_ip), pcb->ttl, pcb->tos, IP_PROTO_TCP);
        	
        pbuf_free(p);

        return ERR_OK;
    }

    /* useg should point to last segment on unacked queue */
    useg = pcb->unacked;
    if (useg != NULL)
    {
		for (; useg->next != NULL; useg = useg->next);
    }

    /* data available and window allows it to be sent? */
    while ((seg != NULL) && (ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->tcplen <= wnd))
    {
        /* Stop sending if the nagle algorithm would prevent it
         * Don't stop:
         * - if tcp_enqueue had a memory error before (prevent delayed ACK timeout) or
         * - if FIN was already enqueued for this PCB (SYN is always alone in a segment -
         *   either seg->next != NULL or pcb->unacked == NULL;
         *   RST is no sent using tcp_enqueue/tcp_output.
         */
        if ((tcp_do_output_nagle(pcb) == 0) && (pcb->flags & (TF_NAGLEMEMERR | TF_FIN)) == 0)
        {
            break;
        }

        pcb->unsent = seg->next;

        if (pcb->state != SYN_SENT)
        {
            TCPH_SET_FLAG(seg->tcphdr, TCP_ACK);
            pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
        }

        oldpayload = seg->payload;
        oldlen = seg->len;
        tcp_output_segment(seg, pcb);
        seg->payload = oldpayload;
        seg->len = oldlen;
		
        snd_nxt = ntohl(seg->tcphdr->seqno) + TCP_TCPLEN(seg);
        if (pcb->snd_nxt < snd_nxt)
        {
            pcb->snd_nxt = snd_nxt;
        }
        /* put segment on unacknowledged list if length > 0 */
        if (TCP_TCPLEN(seg) > 0)
        {
            seg->next = NULL;
            /* unacked list is empty? */
            if (pcb->unacked == NULL)
            {
                pcb->unacked = seg;
                useg = seg;
                /* unacked list is not empty? */
            }
            else
            {
                /* In the case of fast retransmit, the packet should not go to the tail
                 * of the unacked queue, but rather somewhere before it. We need to check for
                 * this case. -STJ Jul 27, 2004 */
                if (ntohl(seg->tcphdr->seqno) < ntohl(useg->tcphdr->seqno))
                {
                    /* add segment to before tail of unacked list, keeping the list sorted */
                    PBUF **cur_seg = (PBUF **)(&(pcb->unacked));
                    while (*cur_seg && (ntohl((*cur_seg)->tcphdr->seqno) < ntohl(seg->tcphdr->seqno)))
                    {
                        cur_seg = (PBUF **)(&((*cur_seg)->next));
                    }
                    seg->next = (*cur_seg);
                    (*cur_seg) = seg;
                }
                else
                {
                    /* add segment to tail of unacked list */
                    useg->next = seg;
                    useg = useg->next;
                }
            }
            /* do not queue empty segments on the unacked list */
        }
        else
        {
            tcp_seg_free(seg); //free the packet which neither data nor both FIN or SYN flag....
        }
        seg = pcb->unsent;
    }

    if ((seg != NULL) && (pcb->persist_backoff == 0) && 
		(ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->tcplen > pcb->snd_wnd))
    {
        /* prepare for persist timer */
        pcb->persist_cnt = 0;
        pcb->persist_backoff = 1;
    }

    pcb->flags &= ~TF_NAGLEMEMERR;
    return ERR_OK;
}

/**
 * Called by tcp_output() to actually send a TCP segment over IP.
 *
 * @param seg the tcp_seg to send
 * @param pcb the tcp_pcb for the TCP connection used to send the segment
 */
static void tcp_output_segment(PBUF *seg, TCP_PCB *pcb)
{
    uint16 len;
    NETIF *netif;
    uint32 *opts;

    /* The TCP header has already been constructed, but the ackno and
     wnd fields remain. */
    seg->tcphdr->ackno = htonl(pcb->rcv_nxt);

    /* advertise our receive window size in this TCP segment */
    seg->tcphdr->wnd = htons(pcb->rcv_ann_wnd);//htons(pcb->rcv_ann_wnd); TCP_TEST_WIN_SIZE

    pcb->rcv_ann_right_edge = pcb->rcv_nxt + pcb->rcv_ann_wnd;

    /* Add any requested options.  NB MSS option is only set on SYN
       packets, so ignore it here */
    opts = (uint32 *)(seg->tcphdr + 1);
    if (seg->flags & TF_SEG_OPTS_MSS)
    {
        TCP_BUILD_MSS_OPTION(*opts);
        opts += 1;
    }

    /* If we don't have a local IP address, we get one by
       calling ip_route(). */
    if (ip_addr_isany(&(pcb->local_ip)))
    {
        netif = ip_route(&(pcb->remote_ip));
        if (netif == NULL)
            return;
        ip_addr_set(&(pcb->local_ip), &(netif->ip_addr));
    }

    /* Set retransmission timer running if it is not currently enabled */
    if (pcb->rtime == -1)
        pcb->rtime = 0;

    if (pcb->rttest == 0)
    {
        pcb->rttest = tcp_ticks;
        pcb->rtseq = ntohl(seg->tcphdr->seqno);
    }
    seg->tcphdr->chksum = 0;
    seg->tcphdr->chksum = inet_chksum_pseudo(seg, &(pcb->local_ip), &(pcb->remote_ip), IP_PROTO_TCP, seg->len);

    ip_output(seg, &(pcb->local_ip), &(pcb->remote_ip), pcb->ttl, pcb->tos, IP_PROTO_TCP);
}

/**
 * Send a TCP RESET packet (empty segment with RST flag set) either to
 * abort a connection or to show that there is no matching local connection
 * for a received segment.
 *
 * Called by tcp_abort() (to abort a local connection), tcp_input() (if no
 * matching local pcb was found), tcp_listen_input() (if incoming segment
 * has ACK flag set) and tcp_process() (received segment in the wrong state)
 *
 * Since a RST segment is in most cases not sent for an active connection,
 * tcp_rst() has a number of arguments that are taken from a tcp_pcb for
 * most other segment output functions.
 *
 * @param seqno the sequence number to use for the outgoing segment
 * @param ackno the acknowledge number to use for the outgoing segment
 * @param local_ip the local IP address to send the segment from
 * @param remote_ip the remote IP address to send the segment to
 * @param local_port the local TCP port to send the segment from
 * @param remote_port the remote TCP port to send the segment to
 */
void tcp_rst(uint32 seqno, uint32 ackno,
        IP_ADDR *local_ip, IP_ADDR *remote_ip,
        uint16 local_port, uint16 remote_port)
{
    PBUF *p;
    TCP_HDR *tcphdr;

#ifdef DBUGPBUF
	printf("rst:");
#endif
    p = pbuf_alloc(PBUF_IP, TCP_HLEN, PBUF_POOL);
    if (p == NULL)
    {
        return;
    }
    tcphdr = p->payload;
    tcphdr->src = htons(local_port);
    tcphdr->dest = htons(remote_port);
    tcphdr->seqno = htonl(seqno);
    tcphdr->ackno = htonl(ackno);
    TCPH_FLAGS_SET(tcphdr, TCP_RST|TCP_ACK);
    tcphdr->wnd = htons(TCP_WND);
    tcphdr->urgp = NULL;
    TCPH_HDRLEN_SET(tcphdr, (TCP_HLEN>>2));

    tcphdr->chksum = 0;
    tcphdr->chksum = inet_chksum_pseudo(p, local_ip, remote_ip, IP_PROTO_TCP, p->len);

    /* Send output with hardcoded TTL since we have no access to the pcb */
    ip_output(p, local_ip, remote_ip, TCP_TTL, 0, IP_PROTO_TCP);
    pbuf_free(p);
}

/**
 * Requeue all unacked segments for retransmission
 *
 * Called by tcp_slowtmr() for slow retransmission.
 *
 * @param pcb the tcp_pcb for which to re-enqueue all unacked segments
 */
void tcp_rexmit_rto(TCP_PCB *pcb)
{
    PBUF *seg;

    if (pcb->unacked == NULL)
        return;

	printf("tcp_rexmit_rto \n");
    /* Move all unacked segments to the head of the unsent queue */
    for (seg = pcb->unacked; seg->next != NULL; seg = seg->next);
	
    /* concatenate unsent queue after unacked queue */
    seg->next = pcb->unsent;
	
    /* unsent queue is the concatenated queue (of unacked, unsent) */
    pcb->unsent = pcb->unacked;
	
    /* unacked queue is now empty */
    pcb->unacked = NULL;

    /* increment number of retransmissions */
    ++pcb->nrtx;

    /* Don't take any RTT measurements after retransmitting. */
    pcb->rttest = 0;

    /* Do the actual retransmission */
    tcp_output(pcb);
}

/**
 * Requeue the first unacked segment for retransmission
 *
 * Called by tcp_receive() for fast retramsmit.
 *
 * @param pcb the tcp_pcb for which to retransmit the first unacked segment
 */
void tcp_rexmit(TCP_PCB *pcb)
{
    PBUF *seg;
    PBUF **cur_seg;

    if (pcb->unacked == NULL)
        return;

    /* Move the first unacked segment to the unsent queue */
    /* Keep the unsent queue sorted. */
    seg = pcb->unacked;
    pcb->unacked = seg->next;

    cur_seg = (PBUF **)(&(pcb->unsent));
    while (*cur_seg && (ntohl((*cur_seg)->tcphdr->seqno) < ntohl(seg->tcphdr->seqno)))//TCP_SEQ_LT
    {
        cur_seg = (PBUF **)(&((*cur_seg)->next));
    }
    seg->next = *cur_seg;
    *cur_seg = seg;

    ++pcb->nrtx;

    /* Don't take any rtt measurements after retransmitting. */
    pcb->rttest = 0;

    /* Do the actual retransmission. */
    tcp_output(pcb);
}

/**
 * Send keepalive packets to keep a connection active although
 * no data is sent over it.
 *
 * Called by tcp_slowtmr()
 *
 * @param pcb the tcp_pcb for which to send a keepalive packet
 */
void tcp_keepalive(TCP_PCB *pcb)
{
    PBUF *p;
    TCP_HDR *tcphdr;

#ifdef DBUGPBUF
	printf("ka:");
#endif
    p = pbuf_alloc(PBUF_IP, TCP_HLEN, PBUF_POOL);
    if (p == NULL)
        return;

    tcphdr = tcp_output_set_header(pcb, p, 0, htonl(pcb->snd_nxt - 1));

    tcphdr->chksum = inet_chksum_pseudo(p, &pcb->local_ip, &pcb->remote_ip, IP_PROTO_TCP, p->len);
	
    /* Send output to IP */
    ip_output(p, &pcb->local_ip, &pcb->remote_ip, pcb->ttl, 0, IP_PROTO_TCP);

    pbuf_free(p);
}


/**
 * Send persist timer zero-window probes to keep a connection active
 * when a window update is lost.
 *
 * Called by tcp_slowtmr()
 *
 * @param pcb the tcp_pcb for which to send a zero-window probe packet
 */
void tcp_zero_window_probe(TCP_PCB *pcb)
{
    PBUF *p;
    TCP_HDR *tcphdr;
    PBUF *seg;

    seg = pcb->unacked;

    if (seg == NULL)
        seg = pcb->unsent;

    if (seg == NULL)
        return;
#ifdef DBUGPBUF
	printf("zer:");
#endif
    p = pbuf_alloc(PBUF_IP, TCP_HLEN + 1, PBUF_POOL);
    if (p == NULL)
    {
        return;
    }
    tcphdr = tcp_output_set_header(pcb, p, 0, seg->tcphdr->seqno);

    /* Copy in one byte from the head of the unacked queue */
    *((char *)p->payload + sizeof(TCP_HDR)) = *(char *)seg->payload;

    tcphdr->chksum = inet_chksum_pseudo(p, &pcb->local_ip, &pcb->remote_ip,	 IP_PROTO_TCP, p->len);

    ip_output(p, &pcb->local_ip, &pcb->remote_ip, pcb->ttl, 0, IP_PROTO_TCP);printf("probe ip_output\n");

    pbuf_free(p);
}


