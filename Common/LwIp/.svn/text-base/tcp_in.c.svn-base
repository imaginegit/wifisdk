/**
 * @file
 * Transmission Control Protocol, incoming traffic
 *
 * The input processing functions of the TCP layer.
 *
 * These functions are generally called in the order (ip_input() ->)
 * tcp_input() -> * tcp_process() -> tcp_receive() (-> application).
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


/* These variables are global to all functions involved in the input
   processing of TCP segments. They are set by the tcp_input()
   function. */
//static PBUF *inseg;

static PBUF *inseg = NULL;
static TCP_HDR *tcphdr = NULL;
static IP_HDR *iphdr = NULL;
static uint32 seqno = 0, ackno = 0;
static uint8 flags = 0;
static uint16 tcplen = 0;//this len include SYN or FIN length...

static uint8 recv_flags = 0;

TCP_PCB *tcp_input_pcb;

/* Forward declarations. */
static uint8 tcp_process(TCP_PCB *pcb);
static uint8 tcp_receive(TCP_PCB *pcb);
static uint8 tcp_timewait_input(TCP_PCB *pcb);

/**
 * The initial input processing of TCP. It verifies the TCP header, demultiplexes
 * the segment between the PCBs and passes it on to tcp_process(), which implements
 * the TCP finite state machine. This function is called by the IP layer (in
 * ip_input()).
 *
 * @param p received TCP segment to process (p->payload pointing to the IP header)
 * @param inp network interface on which this segment was received
 */
int ooquenum = 0;

void tcp_input(PBUF *p, NETIF *inp)
{
    TCP_PCB *pcb;
    uint8 hdrlen;
    uint8 err;

	if (p == NULL)
		return;
	
    iphdr = p->payload;
	hdrlen = (IPH_HL(iphdr) << 2);
    tcphdr = (TCP_HDR *)((uint8 *)p->payload + hdrlen);

    p->payload = (uint8 *)(p->payload) + hdrlen;
	/*To move playload pointer to clear the packet header of wifi chip fill in...*/
    p->len = ntohs(iphdr->_len)- hdrlen;

    /* remove header from payload */
    if (p->len < sizeof(TCP_HDR))
	{
    	pbuf_free(p);
        return;
    }

    /* Don't even process incoming broadcasts/multicasts. */
    if (ip_addr_isbroadcast(&(iphdr->dest), inp) || ip_addr_ismulticast(&(iphdr->dest)))
    {
        pbuf_free(p);
        return;
    }

    /* Verify TCP checksum. */
    if (inet_chksum_pseudo(p, (IP_ADDR *)&(iphdr->src),(IP_ADDR *)&(iphdr->dest),IP_PROTO_TCP, p->len) != 0)
    {
        pbuf_free(p);
        return;
    }

    /* Move the payload pointer in the pbuf so that it points to the
       TCP data instead of the TCP header. */
    hdrlen = (TCPH_HDRLEN(tcphdr) << 2);
    if(p->len < hdrlen)
    {
    	/* drop short packets */
	    pbuf_free(p);
        return;
    }

    p->payload = (uint8 *)(p->payload) + hdrlen;
    p->len -= hdrlen;
    
    /* Convert fields in TCP header to host byte order. */
    tcphdr->src = ntohs(tcphdr->src);
    tcphdr->dest = ntohs(tcphdr->dest);
    seqno = tcphdr->seqno = ntohl(tcphdr->seqno);
    ackno = tcphdr->ackno = ntohl(tcphdr->ackno);
    tcphdr->wnd = ntohs(tcphdr->wnd);

    flags = TCPH_FLAGS(tcphdr);
    tcplen = p->len + ((flags & (TCP_FIN | TCP_SYN)) ? 1 : 0);
    p->tcplen = p->len;
	//printf("recv: rev:%010u seq=%010u len:%3u wnd:%u %u\n", http_had_revd(), seqno, tcplen, tcphdr->wnd, ooquenum);

    /* Demultiplex an incoming segment. First, we check if it is destined
       for an active connection. */
    pcb = &stTcpPcb;
    if (pcb->remote_port == tcphdr->src &&
        pcb->local_port == tcphdr->dest &&
        ip_addr_cmp(&(pcb->remote_ip), &(iphdr->src)) &&
        ip_addr_cmp(&(pcb->local_ip), &(iphdr->dest)))
	{
	    /* Set up a tcp_seg structure. */
	    inseg = p;
	    inseg->next = NULL;
        inseg->tcphdr = tcphdr;
	    recv_flags = 0;

        if (pcb->state == TIME_WAIT)
		{
		    tcp_timewait_input(pcb);	
		}

        tcp_input_pcb = pcb;
        err = tcp_process(pcb);
        tcp_input_pcb = NULL;
            
	    /* A return value of ERR_ABRT means that tcp_abort() was called
	       and that the pcb has been freed. If so, we don't do anything. */
	    if (err != ERR_ABRT)
	    {
	        if ((recv_flags & TF_RESET) || (recv_flags & TF_CLOSED))
	        {
                tcp_pcb_remove_nolist(pcb);
                tcp_close(pcb);
	        }
	        else
	        {
	            err = ERR_OK;
	            /* If the application has registered a "sent" function to be
	               called when new send buffer space is available, we call it
	               now. */
	            if (pcb->acked > 0)
	            {
	                //send a msg to app for coutinue send datagroup
	            }

	            if (pcb->unRead != NULL)
	            {
	                if (flags & TCP_PSH)
	                {
	                   //send msg to app for receive datagroup
	                }                    
	            }

	            /* If a FIN segment was received, we call the callback
	               function with a NULL buffer to indicate EOF. */
	            if (recv_flags & TF_GOT_FIN)
	            {
	                //send msg
	            }

	            /* If there were no errors, we try to send something out. */
	            if (err == ERR_OK  && pcb->state != FIN_WAIT_1)
	            {
	                tcp_output(pcb);
	            }
	        }
	    } 

		if (inseg != NULL)
		{
			if (inseg->tcplen) 
			{
				printf("error error: %d\n", inseg->tcplen);
			}
			pbuf_free(inseg);
			inseg = NULL;
		}
	}
    else
    {
		//printf("fp: %d %d %d %d: 0x%08x 0x%08x 0x%08x 0x%08x\n", 
		//		pcb->remote_port, tcphdr->src, pcb->local_port, tcphdr->dest, 
		//		pcb->remote_ip.addr, iphdr->src.addr, pcb->local_ip.addr, iphdr->dest.addr);
		//printf("free packet:local_port:%d dest:%d\n", pcb->local_port, tcphdr->dest);
        /* If no matching PCB was found, send a TCP RST (reset) to the sender. */
        if (!(TCPH_FLAGS(tcphdr) & TCP_RST))
        {
			printf("tcp rst tcp_input!\n");
            tcp_rst(ackno, seqno + tcplen, &(iphdr->dest), &(iphdr->src), tcphdr->dest, tcphdr->src);
        }
        pbuf_free(p);
    }
}

 /*
 * Purges the PCB and removes it whithout the list .
 *
 * @param pcblist PCB list to purge.
 * @param pcb tcp_pcb to purge. The pcb itself is also deallocated!
 */
void tcp_pcb_remove_nolist(TCP_PCB *pcb )
{
	//printf("tcp_pcb_remove_nolist..\n");
	tcp_pcb_purge(pcb);
	if (pcb->state != TIME_WAIT && pcb->flags & TF_ACK_DELAY && pcb->state != LAST_ACK) 
	{
		pcb->flags |= TF_ACK_NOW;
		tcp_output(pcb);
	}
	pcb->state = CLOSED;
}

/**
 * Called by tcp_input() when a segment arrives for a connection in
 * TIME_WAIT.
 *
 * @param pcb the tcp_pcb for which a segment arrived
 *
 * @note the segment which arrived is saved in global variables, therefore only the pcb
 *       involved is passed as a parameter to this function
 */
static uint8 tcp_timewait_input(TCP_PCB *pcb)
{
    if (TCP_SEQ_GT(seqno + tcplen, pcb->rcv_nxt))
    {
        pcb->rcv_nxt = seqno + tcplen;
    }
    if (tcplen > 0)
    {
        printf("tcp time wait\n");
        tcp_ack_now(pcb);
    }
    return tcp_output(pcb);
}

/**
 * Implements the TCP state machine. Called by tcp_input. In some
 * states tcp_receive() is called to receive data. The tcp_seg
 * argument will be freed by the caller (tcp_input()) unless the
 * recv_data pointer in the pcb is set.
 *
 * @param pcb the tcp_pcb for which a segment arrived
 *
 * @note the segment which arrived is saved in global variables, therefore only the pcb
 *       involved is passed as a parameter to this function
 */
static uint8 tcp_process(TCP_PCB *pcb)
{
    PBUF *rseg;
    uint8 acceptable = 0;
    uint8 err = ERR_OK;

    /* Process incoming RST segments. */
    if (flags & TCP_RST)
    {
		printf("TCP HAVE BEEN THE SEVER RST \n");
        /* First, determine if the reset is acceptable. */
        if (pcb->state == SYN_SENT)
        {
            if (ackno == pcb->snd_nxt)
            {
                acceptable = 1;
            }
        }
        else
        {
            if (TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt, pcb->rcv_nxt+pcb->rcv_wnd))
            {
                acceptable = 1;
            }
        }

        if (acceptable)
        {
            recv_flags |= TF_RESET;
            pcb->flags &= ~TF_ACK_DELAY;
            return ERR_RST;
        }
        else
        {
            return ERR_OK;
        }
    }

    if ((flags & TCP_SYN) && (pcb->state != SYN_SENT))
    {
        /* Cope with new connection attempt after remote end crashed */
        printf("rev tcp syn\n");
        tcp_ack_now(pcb);
        return ERR_OK;
    }

    /* Update the PCB (in)activity timer. */
    pcb->tmr = tcp_ticks;
    pcb->keep_cnt_sent = 0;
	
    /* Do different things depending on the TCP state. */
    switch (pcb->state)
    {
        case SYN_SENT:
			printf("SYN_S %xH\n", flags);
            if ((flags & TCP_ACK) && (flags & TCP_SYN) && (ackno == ntohl(pcb->unacked->tcphdr->seqno) + 1))
            {
                pcb->snd_buf++;
                pcb->rcv_nxt = seqno + 1;
                pcb->rcv_ann_right_edge = pcb->rcv_nxt;
                pcb->lastack = ackno;
                pcb->snd_wnd = tcphdr->wnd;
                pcb->snd_wl1 = seqno - 1; /* initialise to seqno - 1 to force window update */
                pcb->state = ESTABLISHED;
                /* Set ssthresh again after changing pcb->mss (already set in tcp_connect
                 * but for the default value of pcb->mss) */
                pcb->ssthresh = TCP_WND;
                pcb->cwnd = ((pcb->cwnd == 1) ? (pcb->mss * 2) : pcb->mss);

                rseg = pcb->unacked;
                pcb->unacked = rseg->next;

                /* If there's nothing left to acknowledge, stop the retransmit
                   timer, otherwise reset it to start again */
                if (pcb->unacked == NULL)
                {
                    pcb->rtime = -1;
                }
                else
                {
                    pcb->rtime = 0;
                    pcb->nrtx = 0;
                }

                pbuf_free(rseg);
                tcp_ack_now(pcb);
            } 
            else if (flags & TCP_ACK) 
			{
                /* send a RST to bring the other side in a non-synchronized state. */
				printf("tcp rst tcp_process!\n");
                tcp_rst(ackno, seqno + tcplen, &(iphdr->dest), &(iphdr->src), tcphdr->dest, tcphdr->src);
                pcb->rtime = 3000;
            }
            break;

        case CLOSE_WAIT :
        case ESTABLISHED:
            tcp_receive(pcb);
			if (recv_flags & TF_GOT_FIN) 
            { 
				/* passive close */
                printf("get the fin xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
                tcp_ack_now(pcb);                
                pcb->state = CLOSE_WAIT;
                tcp_close(pcb);
            }
            break;

        case FIN_WAIT_1:
            tcp_receive(pcb);
            if (recv_flags & TF_GOT_FIN)
            {
                if ((flags & TCP_ACK) && (ackno == pcb->snd_nxt)) 
                {
                     tcp_ack_now(pcb);
                     tcp_pcb_purge(pcb);
                     pcb->state = TIME_WAIT;
                } 
                else 
                {
                    tcp_ack_now(pcb);
                    pcb->state = CLOSING;
                }
            } 
            else if ((flags & TCP_ACK) && (ackno == pcb->snd_nxt))
            {
                pcb->state = FIN_WAIT_2;
            }
            break;

        case FIN_WAIT_2:
            tcp_receive(pcb);
            if (recv_flags & TF_GOT_FIN)
            {
                tcp_ack_now(pcb);
                tcp_pcb_purge(pcb);
                pcb->state = TIME_WAIT;
            }
            else
            {
                 printf("tcp rst.......\n");
                 tcp_rst(ackno, seqno + tcplen, &(iphdr->dest), &(iphdr->src), tcphdr->dest, tcphdr->src);
                 pcb->state = CLOSED;
                 tcp_close(pcb);
            }
            break;
            
        case CLOSING:
            tcp_receive(pcb);
            if (flags & TCP_ACK && ackno == pcb->snd_nxt)
            {
                tcp_pcb_purge(pcb);
                pcb->state = TIME_WAIT;
            }
            break;
            
        case LAST_ACK:
            tcp_receive(pcb);
            if (flags & TCP_ACK && ackno == pcb->snd_nxt) 
			{
	            recv_flags |= TF_CLOSED;
            }
            break;
       default:
            break;
    }

    return ERR_OK;
}

/**
 * Called by tcp_process. Checks if the given segment is an ACK for outstanding
 * data, and if so frees the memory of the buffered data. Next, is places the
 * segment on any of the receive queues (pcb->recved or pcb->ooseq). If the segment
 * is buffered, the pbuf is referenced by pbuf_ref so that it will not be freed until
 * i it has been removed from the buffer.
 *
 * If the incoming segment constitutes an ACK for a segment that was used for RTT
 * estimation, the RTT is estimated here as well.
 *
 * Called from tcp_process().
 *
 * @return 1 if the incoming segment is the next in sequence, 0 if not
 */
static uint8 tcp_receive(TCP_PCB *pcb)
{
    PBUF *next, *unRead;
    PBUF *prev, *cseg;
    PBUF *p;
    int32 off;
    int16 m;
    uint32 right_wnd_edge;
    uint16 new_tot_len;
    uint8 accepted_inseq = 0;

    if (flags & TCP_ACK)
    {
        right_wnd_edge = pcb->snd_wl2 + pcb->snd_wnd;
        if(pcb->state == FIN_WAIT_2)
        {
             //printf("have enter ack fin wait2**************************\n");
        }
        /* Update window. */
        if ((pcb->snd_wl1 < seqno)
			||(pcb->snd_wl1 == seqno && (pcb->snd_wl2 < ackno)) 
			||(pcb->snd_wl2 == ackno && (tcphdr->wnd > pcb->snd_wnd)))
        {
            pcb->snd_wnd = tcphdr->wnd;
            pcb->snd_wl1 = seqno;
            pcb->snd_wl2 = ackno;
            if (pcb->snd_wnd > 0 && pcb->persist_backoff > 0)
            {
                pcb->persist_backoff = 0;
            }
        }

		/* here mean: receiver tell us that it had finish received here(pcb->lastack)
		 * than we maybe send some data packet,but it do not received,so here to ack  
		 * last ack,it may be need us to remit that data packet...
		*/
        if (pcb->lastack == ackno)//may lost some packet which we had sent.so ask number is equal with lastack.
        {
            pcb->acked = 0;
            if (pcb->snd_wl2 + pcb->snd_wnd == right_wnd_edge)
            {
                ++pcb->dupacks;
                if (pcb->dupacks >= 3 && pcb->unacked != NULL)
                {
                    if (!(pcb->flags & TF_INFR))
                    {
                        tcp_rexmit(pcb);//insert one most early unack packet to unsent list packet.
                        /* Set ssthresh to max (FlightSize / 2, 2*SMSS) */
                        /*pcb->ssthresh = LWIP_MAX((pcb->snd_max -
                                                  pcb->lastack) / 2,
                                                  2 * pcb->mss);*/
                        /* Set ssthresh to half of the minimum of the current cwnd and the advertised window */
                        if (pcb->cwnd > pcb->snd_wnd)
                        {
                            pcb->ssthresh = pcb->snd_wnd / 2;
                        }
                        else
                        {
                            pcb->ssthresh = pcb->cwnd / 2;
                        }
                        
                        /* The minimum value for ssthresh should be 2 MSS */
                        if (pcb->ssthresh < 2 * pcb->mss)
                        {
                            pcb->ssthresh = 2 * pcb->mss;
                        }

                        pcb->cwnd = pcb->ssthresh + 3 * pcb->mss;
                        pcb->flags |= TF_INFR;
                    }
                    else
                    {
                        /* Inflate the congestion window, but not if it means that
                           the value overflows. */
                        if ((uint16)(pcb->cwnd + pcb->mss) > pcb->cwnd)
                        {
                            pcb->cwnd += pcb->mss;
                        }
                    }
                }
            }
            else
            {
                //if receive a no sequence group, receive windows is not change
            }
        }
        else if (TCP_SEQ_BETWEEN(ackno, pcb->lastack + 1, pcb->snd_nxt))
        {
            /* We come here when the ACK acknowledges new data. */

            /* Reset the "IN Fast Retransmit" flag, since we are no longer
               in fast retransmit. Also reset the congestion window to the
               slow start threshold. */
            if (pcb->flags & TF_INFR)
            {
                pcb->flags &= ~TF_INFR;
                pcb->cwnd = pcb->ssthresh;
            }

            /* Reset the number of retransmissions. */
            pcb->nrtx = 0;

            /* Reset the retransmission time-out. */
            pcb->rto = (pcb->sa >> 3) + pcb->sv;

            /* Update the send buffer space. Diff between the two can never exceed 64K? */
            pcb->acked = (uint16)(ackno - pcb->lastack);

            pcb->snd_buf += pcb->acked;

            /* Reset the fast retransmit variables. */
            pcb->dupacks = 0;
            pcb->lastack = ackno;

            /* Update the congestion control variables (cwnd and ssthresh). */
            if (pcb->state >= ESTABLISHED)
            {
                if (pcb->cwnd < pcb->ssthresh)
                {
                    if ((uint16)(pcb->cwnd + pcb->mss) > pcb->cwnd)
                    {
                        pcb->cwnd += pcb->mss;
                    }
                }
                else
                {
                    uint16 new_cwnd = (pcb->cwnd + pcb->mss * pcb->mss / pcb->cwnd);
                    if (new_cwnd > pcb->cwnd)
                    {
                        pcb->cwnd = new_cwnd;
                    }
                }
            }

            /* Remove segment from the unacknowledged list if the incoming
               ACK acknowlegdes them. */
            while (pcb->unacked != NULL &&
                    TCP_SEQ_LEQ(ntohl(pcb->unacked->tcphdr->seqno) + TCP_TCPLEN(pcb->unacked), ackno))
            {
                next = pcb->unacked;
                pcb->unacked = pcb->unacked->next;
                pcb->snd_queuelen -= 1;                
                pbuf_free(next);
                if (pcb->snd_queuelen != 0)
                {

                }
            }

            /* If there's nothing left to acknowledge, stop the retransmit
               timer, otherwise reset it to start again */
            if (pcb->unacked == NULL)
            {
                pcb->rtime = -1;
            }
            else
            {
                pcb->rtime = 0;
            }
            pcb->polltmr = 0;
        }
        else
        {
            /* Fix bug bug #21582: out of sequence ACK, didn't really ack anything */
            pcb->acked = 0;
        }

        /* We go through the ->unsent list to see if any of the segments
           on the list are acknowledged by the ACK. This may seem
           strange since an "unsent" segment shouldn't be acked. The
           rationale is that lwIP puts all outstanding segments on the
           ->unsent list after a retransmission, so these segments may
           in fact have been sent once. */
        while (pcb->unsent != NULL &&
                TCP_SEQ_BETWEEN(ackno, ntohl(pcb->unsent->tcphdr->seqno) + TCP_TCPLEN(pcb->unsent), pcb->snd_nxt))
        {
            next = pcb->unsent;
            pcb->unsent = pcb->unsent->next;

            pcb->snd_queuelen -= 1;
            pbuf_free(next);
        }
        /* End of ACK for new data processing. */

        /* RTT estimation calculations. This is done by checking if the
           incoming segment acknowledges the segment we use to take a
           round-trip time measurement. */
        if (pcb->rttest && TCP_SEQ_LT(pcb->rtseq, ackno))
        {
            /* diff between this shouldn't exceed 32K since this are tcp timer ticks
               and a round-trip shouldn't be that long... */
            m = (uint16)(tcp_ticks - pcb->rttest);          

            /* This is taken directly from VJs original code in his paper */
            m = m - (pcb->sa >> 3);
            pcb->sa += m;
            if (m < 0)
            {
                m = -m;
            }
            m = m - (pcb->sv >> 2);
            pcb->sv += m;
            pcb->rto = (pcb->sa >> 3) + pcb->sv;        

            pcb->rttest = 0;
        }
    }
    
    /* If the incoming segment contains data, we must process it further. */
    #if 0
    if (TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt, pcb->rcv_nxt+pcb->rcv_wnd-1))
    {      
          /*get one sequence packet*/
        if (pcb->rcv_nxt == seqno)
        {
           if (TCPH_FLAGS(inseg->tcphdr) & TCP_FIN) 
		   {
              recv_flags |= TF_GOT_FIN;
              pcb->rcv_nxt += tcplen;             
              return;
            }
         }
    }
	#endif

    if ((tcplen > 0) && (pcb->state != FIN_WAIT_1) )
    {
        /* The sequence number must be within the window (above rcv_nxt
           and below rcv_nxt + rcv_wnd) in order to be further
           processed. */
        if (TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt, pcb->rcv_nxt+pcb->rcv_wnd-1))
        {
            //printf("seq = %u, rcv_nxt = %u\n", seqno, pcb->rcv_nxt);

            /*get one sequence packet*/
            if (pcb->rcv_nxt == seqno)
            {
                if (tcplen > pcb->rcv_wnd)
                {
                    pbuf_free(inseg);
					inseg = NULL;
					printf("tcplen > rcv_wnd.\n");
                    return ERR_OK;
                }
				
				if (TCPH_FLAGS(inseg->tcphdr) & TCP_FIN) 
				{
					recv_flags |= TF_GOT_FIN;
				}
				accepted_inseq = 1;
				
                pcb->rcv_nxt += tcplen;
                /* Update the receiver's (our) window. */
                pcb->rcv_wnd -= inseg->tcplen;
                if (inseg->len > 0)
			    {
                    if (pcb->unRead == NULL)
                    {
                        pcb->unRead = inseg;
                    }
                    else
                    {
						unRead = pcb->unRead;
	                    while(unRead->next != NULL)
	                    {
	                        unRead = unRead->next;
	                    } 
                        unRead->next = inseg;
                    }   
					inseg->next = NULL;
					inseg = NULL;
                }
			#if TCP_QUEUE_OOSEQ  
				if (pcb->ooseq != NULL)
				{
					if (pcb->ooseq->tcphdr->seqno < pcb->rcv_nxt)
					{
						while ((pcb->ooseq != NULL) && (pcb->ooseq->tcphdr->seqno < pcb->rcv_nxt))
						{
							ooquenum--;
							cseg = pcb->ooseq;
							pcb->ooseq = pcb->ooseq->next;
							pbuf_free(cseg);
							cseg = NULL;
						}
						//printf("ooseq. %d\n", ooquenum);
					}
					if ((pcb->ooseq != NULL) && (pcb->ooseq->tcphdr->seqno == pcb->rcv_nxt))
					{
		                while ((pcb->ooseq != NULL) && (pcb->ooseq->tcphdr->seqno == pcb->rcv_nxt)) 
		                {
							ooquenum--;
		                    cseg = pcb->ooseq;//seqno = pcb->ooseq->tcphdr->seqno;
		                    pcb->rcv_nxt += TCP_TCPLEN(cseg);
		                    pcb->rcv_wnd -= cseg->tcplen;;
		                    pcb->ooseq = cseg->next;
							
							unRead = pcb->unRead;
		                    if (unRead == NULL)
		                    {
		                        pcb->unRead = cseg;
								cseg->next = NULL;
		                    }
		                    else
		                    {
		                        while (unRead->next != NULL)
		                        {
		                            unRead = unRead->next;
		                        } 
		                        unRead->next = cseg; 
		                        cseg->next = NULL;
		                    }
		                }
						//printf("ooseq. %d\n", ooquenum);
                        tcp_update_rcv_ann_wnd(pcb);
                        tcp_ack_now(pcb);
					}
				}
				else
			#endif /*TCP_QUEUE_OOSEQ */
				{
					tcp_update_rcv_ann_wnd(pcb);
                    tcp_ack(pcb);
				}

                //if (pcb->recv)
                //{
					//pcb->recv();
				//}
            }
            else  /*get one oosqu packet*/
            {
                /* We get here if the incoming segment is out-of-sequence. 
                 * send last ACK Number and to ready to receive this packet
				*/
				ooquenum++;
				//printf("ooseq. %d \n", ooquenum);
				
			#if TCP_QUEUE_OOSEQ 
                /* We queue the segment on the ->ooseq queue. */
                if (pcb->ooseq == NULL)
                {
					//printf("ooseq == NULL.\n");
					/* set the frist one...*/
                    pcb->ooseq = inseg;
					inseg->next = NULL;
                    inseg = NULL;
                }
				else if (seqno < pcb->ooseq->tcphdr->seqno)
				{
					//printf("insert start.\n");
					/* insert the start...*/
					inseg->next = pcb->ooseq;
					pcb->ooseq = inseg;
					inseg = NULL;
				}
                else
                {
                   /* If the queue is not empty, we walk through the queue and
                      try to find a place where the sequence number of the
                      incoming segment is between the sequence numbers of the
                      previous and the next segment on the ->ooseq queue. That is
                      the place where we put the incoming segment. If needed, we
                      trim the second edges of the previous and the incoming
                      segment so that it will fit into the sequence.
                      If the incoming segment has the same sequence number as a
                      segment on the ->ooseq queue, we discard the segment that
                      contains less data. */
                    prev = pcb->ooseq;
				    next = pcb->ooseq->next;
                    while (next != NULL)
                    {
						if ((seqno == prev->tcphdr->seqno) || (seqno == next->tcphdr->seqno))
						{
							/* judge wether is remit oosqu..*/
							pbuf_free(inseg);
							inseg = NULL;
							ooquenum--;
							break;
						}
                        if (TCP_SEQ_BETWEEN(seqno, prev->tcphdr->seqno+1,  next->tcphdr->seqno-1))
                        {
							/* insert the midden...*/
							inseg->next = next;
							prev->next = inseg;                                          
							inseg = NULL;
							break;
                        }
                       	prev = next; 
						next = next->next;
                    } 
					/* the end...*/	
					if (inseg)
					{
						//printf("insert end.\n");
						prev->next = inseg;
						inseg->next = NULL;
						inseg = NULL;
					}
				}
				if (ooquenum < 4)
				{
	                tcp_ack_now(pcb);
				}
			#endif /* TCP_QUEUE_OOSEQ */  
			}
        }
        else
        {
			/* the packet is not inside of range. 
			send last ACK Number and 
			do not received this packet...*/
            //printf("seq = %u, rcv_nxt = %u xxxx\n", seqno, pcb->rcv_nxt);
            pbuf_free(inseg);
            inseg = NULL;      
           	tcp_ack_now(pcb);
        }		
	}    
    else
    {
        /* Segments with length 0 is taken care of here. Segments that
           fall out of the window are ACKed. */
        /*if (TCP_SEQ_GT(pcb->rcv_nxt, seqno) || TCP_SEQ_GEQ(seqno, pcb->rcv_nxt + pcb->rcv_wnd)) {*/

        printf("tcp try tcplen:%d pcb->state=%d\n", tcplen, pcb->state);
        pbuf_free(inseg);
	    inseg = NULL;
        
        if (!TCP_SEQ_BETWEEN(seqno, pcb->rcv_nxt, pcb->rcv_nxt + pcb->rcv_wnd - 1 - tcplen))
        {
            tcp_ack_now(pcb);
        }
    }
    return accepted_inseq;
}

