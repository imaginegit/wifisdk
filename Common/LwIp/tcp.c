/**
 * @file
 * Transmission Control Protocol for IP
 *
 * This file contains common functions for the TCP implementation, such as functinos
 * for manipulating the data structures and the TCP timer functions. TCP functions
 * related to input and output is found in tcp_in.c and tcp_out.c respectively.
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

#include "tcp.h"
#include "ip_addr.h"

#include <string.h>

extern BOOLEAN bWiFiAudioPause;

/* Incremented every coarse grained timer shot (typically every 500 ms). */
uint32 tcp_ticks = 0;//every 500 ms

const uint8 tcp_backoff[13] = { 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7};
/* Times per slowtmr hits */
const uint8 tcp_persist_backoff[7] = { 3, 6, 12, 24, 48, 96, 120 };

/* The TCP PCB lists. */

/** List of all TCP PCBs that are in a state in which
 * they accept or send data. */
//TCP_PCB *tcp_active_pcbs;

/** List of all TCP PCBs in TIME-WAIT state */
//TCP_PCB *tcp_tw_pcbs;

//TCP_PCB *tcp_tmp_pcb;
static uint8 tcp_timer = 1;

TCP_PCB stTcpPcb;

/**
 * Called periodically to dispatch TCP timers.
 *
 */
void tcp_tmr(void)
{
    /* Call tcp_fasttmr() every 250 ms */
    tcp_fasttmr();

	tcp_timer = 1 - tcp_timer;
    if (tcp_timer)
    {
        /* Call tcp_tmr() every 500 ms, i.e., every other timer tcp_tmr() is called. */
    	tcp_slowtmr();
    }
}

/**
 * Closes the connection held by the PCB.
 *
 * Listening pcbs are freed and may not be referenced any more.
 * Connection pcbs are freed if not yet connected and may not be referenced
 * any more. If a connection is established (at least SYN received or in
 * a closing state), the connection is closed, and put in a closing state.
 * The pcb is then automatically freed in tcp_slowtmr(). It is therefore
 * unsafe to reference it.
 *
 * @param pcb the tcp_pcb to close
 * @return ERR_OK if connection has been closed
 *         another uint8 if closing failed and pcb is not freed
 */
uint8 tcp_close(TCP_PCB *pcb)
{
    uint8 err = ERR_OK;

	tcp_tick_ack_unable(pcb);//printf("pcb->timerackflag = 0\n");
    switch (pcb->state)
    {
        case CLOSED:
            /* Closing a pcb in the CLOSED state might seem erroneous,
             * however, it is in this state once allocated and as yet unused
             * and the user needs some way to free it should the need arise.
             * Calling tcp_close() with a pcb that has already been closed, (i.e. twice)
             * or for a pcb that has been used and then entered the CLOSED state
             * is erroneous, but this should never happen as the pcb has in those cases
             * been freed, and so any remaining handles are bogus. */
            pcb->pcb_close(TCP_CLOSED);//Http_Check_Tcp_State
            memset(pcb, 0, sizeof(pcb));
            pcb = NULL;
            break;

        case SYN_SENT:
            tcp_pcb_remove_nolist(pcb);
            if (pcb->unacked != NULL)
            {
                tcp_segs_free(pcb->unacked);
    			pcb->unacked = NULL;
            }
            if (pcb->unsent != NULL)
            {
                tcp_segs_free(pcb->unsent);
    			pcb->unsent = NULL;
            }
            pcb->pcb_close(TCP_CLOSED);//Http_Check_Tcp_State
            memset(pcb, 0, sizeof(pcb));
            pcb = NULL;
            break;

        case ESTABLISHED:
            err = tcp_send_ctrl(pcb, TCP_FIN);
            if (err == ERR_OK)
            {
             	pcb->state = FIN_WAIT_1;
            }
            break;

        case CLOSE_WAIT:
            err = tcp_send_ctrl(pcb, TCP_FIN);
            if (err == ERR_OK) 
			{
	            pcb->state = LAST_ACK;
            }
            break;

        default:
            /* Has already been closed, do nothing. */
            pcb = NULL;
            break;
    }

    if (pcb != NULL && err == ERR_OK)
    {
        /* To ensure all data has been sent when tcp_close returns, we have
           to make sure tcp_output doesn't fail.
           Since we don't really have to ensure all data has been sent when tcp_close
           returns (unsent data is sent from tcp timer functions, also), we don't care
           for the return value of tcp_output for now. */
        /* @todo: When implementing SO_LINGER, this must be changed somehow:
           If SOF_LINGER is set, the data should be sent when tcp_close returns. */
        tcp_output(pcb);
    }
    return err;
}

/**
 * Abandons a connection and optionally sends a RST to the remote
 * host.  Deletes the local protocol control block. This is done when
 * a connection is killed because of shortage of memory.
 *
 * @param pcb the tcp_pcb to abort
 * @param reset boolean to indicate whether a reset should be sent
 */
void tcp_abandon(TCP_PCB *pcb, uint16 reset)
{
    uint32 seqno, ackno;
    uint16 remote_port, local_port;
    IP_ADDR remote_ip, local_ip;

    /* if there is an outstanding delayed ACKs, send it */
    if (pcb->state != TIME_WAIT && pcb->flags & TF_ACK_DELAY)
    {
        pcb->flags |= TF_ACK_NOW;
        tcp_output(pcb);
    }

    /* Figure out on which TCP PCB list we are, and remove us. If we
       are in an active state, call the receive function associated with
       the PCB with a NULL argument, and send an RST to the remote end. */
    if (pcb->state == TIME_WAIT)
    {
        pcb->state = CLOSED;
        tcp_pcb_purge(pcb);
	    tcp_close(pcb);
    }
    else
    {
        seqno = pcb->snd_nxt;
        ackno = pcb->rcv_nxt;
        ip_addr_set(&local_ip, &(pcb->local_ip));
        ip_addr_set(&remote_ip, &(pcb->remote_ip));
        local_port = pcb->local_port;
        remote_port = pcb->remote_port;

        pcb->state = CLOSED;

        if (pcb->unacked != NULL)
        {
            tcp_segs_free(pcb->unacked);
			pcb->unacked = NULL;
        }

        if (pcb->unsent != NULL)
        {
            tcp_segs_free(pcb->unsent);
			pcb->unsent = NULL;
        }

        if (pcb->ooseq != NULL)
        {
            tcp_segs_free(pcb->ooseq);
			pcb->ooseq = NULL;
        }

        if (reset)
        {
			printf("tcp rst tcp_abandon!\n");
            tcp_rst(seqno, ackno, &local_ip, &remote_ip, local_port, remote_port);
        }
    }
}

/**
 * Binds the connection to a local portnumber and IP address. If the
 * IP address is not given (i.e., ipaddr == NULL), the IP address of
 * the outgoing network interface is used instead.
 *
 * @param pcb the tcp_pcb to bind (no check is done whether this pcb is
 *        already bound!)
 * @param ipaddr the local ip address to bind to (use IP_ADDR_ANY to bind
 *        to any local address
 * @param port the local port to bind to
 * @return ERR_USE if the port is already in use
 *         ERR_OK if bound
 */
uint8 tcp_bind(TCP_PCB *pcb, IP_ADDR *ipaddr, uint16 port)
{
    TCP_PCB *cpcb;

    if (port == 0)
    {
        port = 4096;
    }

    if (!ip_addr_isany(ipaddr))
    {
        pcb->local_ip = *ipaddr;
    }
    pcb->local_port = port;

    return ERR_OK;
}



/**
 * Update the state that tracks the available window space to advertise.
 *
 * Returns how much extra window would be advertised if we sent an
 * update now.
 */
uint32 tcp_update_rcv_ann_wnd(TCP_PCB *pcb)
{
    uint32 new_right_edge = pcb->rcv_nxt + pcb->rcv_wnd;

    if (new_right_edge >= pcb->rcv_ann_right_edge + pcb->mss)
    {
        /*here is move receved window to head.. we can advertise more window */
        pcb->rcv_ann_wnd = pcb->rcv_wnd;
        return new_right_edge - pcb->rcv_ann_right_edge;//in ask:rcv_ann_right_edge==pcb->rcv_nxt+pcb->rcv_ann_wnd;
    }
    else
    {
        if (pcb->rcv_nxt > pcb->rcv_ann_right_edge)
        {
            /* Can happen due to other end sending out of advertised window,
             * but within actual available (but not yet advertised) window */
            pcb->rcv_ann_wnd = 0;
        }
        else
        {
            /* keep the right edge of window constant */
            pcb->rcv_ann_wnd = pcb->rcv_ann_right_edge - pcb->rcv_nxt;//rcv_ann_right_edge==pcb->rcv_nxt+pcb->rcv_ann_wnd;
        }
        return 0;
    }
}

/**
 * This function should be called by the application when it has
 * processed the data. The purpose is to advertise a larger window
 * when the data has been processed.
 *
 * @param pcb the tcp_pcb for which data is read
 * @param len the amount of bytes that have been read by the application
 */
void tcp_recved(TCP_PCB *pcb, uint16 len)
{
    int wnd_inflation;

    pcb->rcv_wnd += len;
    if (pcb->rcv_wnd > TCP_WND)
    {
        pcb->rcv_wnd = TCP_WND;
	}

    wnd_inflation = tcp_update_rcv_ann_wnd(pcb);

    /* If the change in the right edge of window is significant (default
     * watermark is TCP_WND/2), then send an explicit update now.
     * Otherwise wait for a packet to be sent in the normal course of
     * events (or more window to be available later) */
    if (wnd_inflation >= TCP_WND_UPDATE_THRESHOLD)//tell receive window changed...
    {
        //printf("tcp_recved:");
        tcp_ack_now(pcb);
	}
	return;
}

/**
 * Set a receive callback for a TCP PCB
 *
 * This callback will be called when receiving a datagram for the pcb.
 *
 * @param pcb the pcb for wich to set the recv callback
 * @param recv function pointer of the callback function
 * @param recv_arg additional argument to pass to the callback function
 */
void tcp_recv(TCP_PCB *pcb, void (* recv)(PBUF *p))
{    
    pcb->recv = recv;
}


void tcp_pcb_close(TCP_PCB *pcb, void (* pcb_close)(uint8 bTcpState))
{
    pcb->pcb_close = pcb_close;
}

/**
 * Connects to another host. The function given as the "connected"
 * argument will be called when the connection has been established.
 *
 * @param pcb the tcp_pcb used to establish the connection
 * @param ipaddr the remote ip address to connect to
 * @param port the remote tcp port to connect to
 * @param connected callback function to call when connected (or on error)
 * @return ERR_VAL if invalid arguments are given
 *         ERR_OK if connect request has been sent
 *         other uint8 values if connect request couldn't be sent
 */

uint8 tcp_connect(TCP_PCB *pcb, IP_ADDR *ipaddr, uint16 port)
{
    uint8 ret;
    uint32 iss;

    if (ipaddr != NULL)
    {
        pcb->remote_ip = *ipaddr;
    }
    else
    {
        return ERR_VAL;
    }

    pcb->remote_port = port;
    if (pcb->local_port == 0)
    {
        pcb->local_port = 4096;
    }
    iss = tcp_next_iss();

    pcb->rcv_nxt = 0;
    pcb->snd_nxt = iss;
    pcb->lastack = iss - 1;
    pcb->snd_lbb = iss - 1;
    pcb->rcv_wnd = TCP_WND;
    pcb->rcv_ann_wnd = TCP_WND;
    pcb->rcv_ann_right_edge = pcb->rcv_nxt;
    pcb->snd_wnd = TCP_WND;
    /* As initial send MSS, we use TCP_MSS but limit it to 536.
       The send MSS is updated when an MSS option is received. */
    pcb->mss = (TCP_MSS > 1460) ? 1460 : TCP_MSS;//(TCP_MSS > 536) ? 536 : TCP_MSS;
    pcb->cwnd = 1;
    pcb->ssthresh = TCP_WND;
    pcb->state = SYN_SENT;

	printf("SYN\n");   
    ret = tcp_enqueue(pcb, NULL, 0, TCP_SYN, TCP_WRITE_FLAG_COPY, TF_SEG_OPTS_MSS);
    if (ret == ERR_OK)
    {
        tcp_output(pcb);
    }
	
    return ret;
}

/**
 * Called every 500 ms and implements the retransmission timer and the timer that
 * removes PCBs that have been in TIME-WAIT for enough time. It also increments
 * various timers such as the inactivity timer in each PCB.
 *
 * Automatically called from tcp_tmr().
 */
void tcp_slowtmr(void)
{
    TCP_PCB *pcb, *pcb2, *prev;
    uint16 eff_wnd;
    uint8 err= ERR_OK;

    ++tcp_ticks;

    /* Steps through all of the active PCBs. */
    prev = NULL;
    pcb = &stTcpPcb;

    if (pcb->state == SYN_SENT && pcb->nrtx == TCP_SYNMAXRTX)
    {
        printf("SYN SENT CLOSED\n");
        pcb->state = CLOSED;
        tcp_pcb_purge(pcb);
	    tcp_close(pcb);
        return;
    }
    else if (pcb->nrtx == TCP_MAXRTX)
    {
        printf("TCP MAXRTX CLOSED\n");
        pcb->state = CLOSED;
        tcp_pcb_purge(pcb);
	    tcp_close(pcb);
        return;
    }
    else
    {
        if (pcb->persist_backoff > 0)
        {
            /* If snd_wnd is zero, use persist timer to send 1 byte probes
             * instead of using the standard retransmission mechanism. */
            pcb->persist_cnt++;
            if (pcb->persist_cnt >= tcp_persist_backoff[pcb->persist_backoff-1])
            {
                pcb->persist_cnt = 0;
                if (pcb->persist_backoff < sizeof(tcp_persist_backoff))
                {
                    pcb->persist_backoff++;
                }
                tcp_zero_window_probe(pcb);
            }
        }
        else
        {
            /* Increase the retransmission timer if it is running */
            if (pcb->rtime >= 0)
                ++pcb->rtime;

            if (pcb->unacked != NULL && pcb->rtime >= pcb->rto)
            {
                /* Double retransmission time-out unless we are trying to
                 * connect to somebody (i.e., we are in SYN_SENT). */
                if (pcb->state != SYN_SENT)
                {
                    pcb->rto = ((pcb->sa >> 3) + pcb->sv) << tcp_backoff[pcb->nrtx];
                }

                /* Reset the retransmission timer. */
                pcb->rtime = 0;

                /* Reduce congestion window and ssthresh. */
                eff_wnd = LWIP_MIN(pcb->cwnd, pcb->snd_wnd);
                pcb->ssthresh = eff_wnd >> 1;
                if (pcb->ssthresh < pcb->mss)
                {
                    pcb->ssthresh = pcb->mss * 2;
                }
                pcb->cwnd = pcb->mss;

                /* The following needs to be called AFTER cwnd is set to one
                   mss - STJ */
                tcp_rexmit_rto(pcb);
            }
        }
    }
    /* Check if this PCB has stayed too long in FIN-WAIT-2 */
    if (pcb->state == FIN_WAIT_2)
    {
        if ((uint32)(tcp_ticks - pcb->tmr) > TCP_FIN_WAIT_TIMEOUT / TCP_SLOW_INTERVAL)
        {
			printf("513:%d\n", pcb->state);
			pcb->state = CLOSED;
			tcp_pcb_purge(pcb);
            tcp_close(pcb);
            return;
        }
    }

    /* Check if KEEPALIVE should be sent */
    if (pcb->state == ESTABLISHED || pcb->state == CLOSE_WAIT)
    {        
        if ((uint32)(tcp_ticks - pcb->tmr) > (pcb->keep_idle + TCP_MAXIDLE) / TCP_SLOW_INTERVAL)
        {
            tcp_abort(pcb);
        }
        else if ((uint32)(tcp_ticks - pcb->tmr) > (pcb->keep_idle + pcb->keep_cnt_sent * TCP_KEEPINTVL_DEFAULT) / TCP_SLOW_INTERVAL)
        {
            tcp_keepalive(pcb);
            pcb->keep_cnt_sent++;
        }        
    }

    if (pcb->state == LAST_ACK  || pcb->state == TIME_WAIT) 
    {
		if ((uint32)(tcp_ticks - pcb->tmr) > 2 * TCP_MSL / TCP_SLOW_INTERVAL)
		{
			printf("538:%d\n", pcb->state);
			pcb->state = CLOSED;
			tcp_pcb_purge(pcb);
			tcp_close(pcb);
			return;
		}
    }

    /* If this PCB has queued out of sequence data, but has been
       inactive for too long, will drop the data (it will eventually
       be retransmitted). */
    if ((pcb->ooseq != NULL) && (tcp_ticks - pcb->tmr >= pcb->rto * TCP_OOSEQ_TIMEOUT))
    {
        tcp_segs_free(pcb->ooseq);
        pcb->ooseq = NULL;
    }
}

/**
 * Is called every TCP_FAST_INTERVAL (250 ms) and process data previously
 * "refused" by upper layer (application) and sends delayed ACKs.
 *
 * Automatically called from tcp_tmr().
 */
void tcp_fasttmr(void)//every 250ms once..
{
	uint32 timeracktmp;
    TCP_PCB *pcb;

    pcb = &stTcpPcb;

    /* send delayed ACKs old:(pcb->flags & TF_ACK_DELAY)*/
	if ((pcb->timerackflag) && (bWiFiAudioPause == FALSE))
	{
		//printf("timerackflag:%d tcp_data_force_ack:%d\n", pcb->timerackflag, tcp_data_force_ack);
		pcb->timerackflag++;
	    if (pcb->timerackflag > 4)//(!(pcb->timerackflag % 8))//define 2s delay most.
	    {
			if (!MSG_LINK_DOWN())
			{
		        //printf("tcp_fasttmr:");
				timeracktmp = pcb->timerackflag;
	            tcp_ack_now(pcb);
		        pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
				pcb->timerackflag = timeracktmp;
			}
	    }
		if (pcb->timerackflag > 160)// 160 * 250ms = 40s
		{
			//printf("no data stream time over 10s..\n");
            tcp_pcb_remove_nolist(&stTcpPcb);
            tcp_close(&stTcpPcb);
		}
	}
	
	return;
}

void tcp_pcb_init(void)
{
	memset((void*)&stTcpPcb, 0, sizeof(TCP_PCB));
}

/**
 * Allocate a new tcp_pcb structure.
 *
 * @param prio priority for the new pcb
 * @return a new tcp_pcb that initially is in state CLOSED
 */
 extern int ooquenum;
TCP_PCB * tcp_alloc(uint8 prio)
{
    TCP_PCB *pcb;
    uint32 iss;

    pcb = &stTcpPcb;

    if (pcb != NULL)
    {
        memset(pcb, 0, sizeof(TCP_PCB));
		
        pcb->prio = prio;
        pcb->snd_buf = TCP_SND_BUF;
        pcb->snd_queuelen = 0;//otal number of pbufs on the unsent/unacked queues
        pcb->rcv_wnd = TCP_WND;
        pcb->rcv_ann_wnd = TCP_WND;
        pcb->timerackflag = 0;
        pcb->tos = 0;
        pcb->ttl = TCP_TTL;
        /* As initial send MSS, we use TCP_MSS but limit it to 536.
           The send MSS is updated when an MSS option is received. */
        pcb->mss = (TCP_MSS > 1460) ? 1460 : TCP_MSS;//(TCP_MSS > 536) ? 536 : TCP_MSS
        pcb->rto = 3000 / TCP_SLOW_INTERVAL;
        pcb->sa = 0;
        pcb->sv = 3000 / TCP_SLOW_INTERVAL;
        pcb->rtime = -1;
        pcb->cwnd = 1;
        iss = tcp_next_iss();
        pcb->snd_wl2 = iss;
        pcb->snd_nxt = iss;
        pcb->lastack = iss;
        pcb->snd_lbb = iss;
        pcb->tmr = tcp_ticks;

        pcb->polltmr = 0;

        /* Init KEEPALIVE timer */
        pcb->keep_idle  = TCP_KEEPIDLE_DEFAULT;

        pcb->keep_cnt_sent = 0;
    }
    ooquenum = 0;
    return pcb;
}

/**
 * add for get the active tcp control struct.
*/
TCP_PCB * tcp_curget(void)
{
	TCP_PCB *pcb;

	pcb = &stTcpPcb;
	return pcb;
}


/**
 * Creates a new TCP protocol control block but doesn't place it on
 * any of the TCP PCB lists.
 * The pcb is not put on any list until binding using tcp_bind().
 *
 * @internal: Maybe there should be a idle TCP PCB list where these
 * PCBs are put on. Port reservation using tcp_bind() is implemented but
 * allocated pcbs that are not bound can't be killed automatically if wanting
 * to allocate a pcb with higher prio (@see tcp_kill_prio())
 *
 * @return a new tcp_pcb that initially is in state CLOSED
 */
TCP_PCB * tcp_new(void)
{
    return tcp_alloc(TCP_PRIO_NORMAL);
}


/**
 * Calculates a new initial sequence number for new connections.
 *
 * @return uint32 pseudo random sequence number
 */
uint32 tcp_next_iss(void)
{
    static uint32 iss = 1;

    iss += (tcp_ticks * 64000);       /* XXX */
    return iss;
}


/**
 * Deallocates a list of TCP segments (tcp_seg structures).
 *
 * @param seg tcp_seg list of TCP segments to free
 * @return the number of pbufs that were deallocated
 */
uint8 tcp_segs_free(PBUF *seg)
{
    uint8 count = 0;
    PBUF *next;
    while (seg != NULL)
    {
        next = seg->next;
        count += tcp_seg_free(seg);
        seg = next;
    }
    return count;
}

/**
 * Frees a TCP segment (tcp_seg structure).
 *
 * @param seg single tcp_seg to free
 * @return the number of pbufs that were deallocated
 */
uint8 tcp_seg_free(PBUF *seg)
{
    uint8 count = 0;

    if (seg != NULL)
    {
        count = pbuf_free(seg);
    }
    return count;
}


/**
 * Purges a TCP PCB. Removes any buffered data and frees the buffer memory
 * (pcb->ooseq, pcb->unsent and pcb->unacked are freed).
 *
 * @param pcb tcp_pcb to purge. The pcb itself is not deallocated!
 */
void tcp_pcb_purge(TCP_PCB *pcb)
{
	if (pcb == NULL)
		return;
	
    if (pcb->state != CLOSED && pcb->state != TIME_WAIT)
    {
        /* Stop the retransmission timer as it will expect data on unacked
        queue if it fires */
        pcb->rtime = -1;

        tcp_segs_free(pcb->ooseq);
        pcb->ooseq = NULL;
		
        tcp_segs_free(pcb->unsent);
		pcb->unsent = NULL;
		
        tcp_segs_free(pcb->unacked);
        pcb->unacked = NULL;
    }
}


void tcp_pcb_purge_unread(TCP_PCB *pcb)
{
	if (!pcb)
	{
		pcb = &stTcpPcb;
	}
	if (pcb->unRead && (pcb->state != CLOSED))
	{
		tcp_segs_free(pcb->unRead);
		pcb->unRead = NULL;
	}
}

