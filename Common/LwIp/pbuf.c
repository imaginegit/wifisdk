/**
 * @file
 * Packet buffer management
 *
 * Packets are built from the pbuf data structure. It supports dynamic
 * memory allocation for packet contents or can reference externally
 * managed packet contents both in RAM and ROM. Quick allocation for
 * incoming packets is provided through pools with fixed sized pbufs.
 *
 * A packet may span over multiple pbufs, chained as a singly linked
 * list. This is called a "pbuf chain".
 *
 * Multiple packets may be queued, also using this singly linked list.
 * This is called a "packet queue".
 *
 * So, a packet queue consists of one or more pbuf chains, each of
 * which consist of one or more pbufs. CURRENTLY, PACKET QUEUES ARE
 * NOT SUPPORTED!!! Use helper structs to queue multiple packets.
 *
 * The differences between a pbuf chain and a packet queue are very
 * precise but subtle.
 *
 * The last pbuf of a packet has a ->tot_len field that equals the
 * ->len field. It can be found by traversing the list. If the last
 * pbuf of a packet has a ->next field other than NULL, more packets
 * are on the queue.
 *
 * Therefore, looping through a pbuf of a single packet, has an
 * loop end condition (tot_len == p->len), NOT (next == NULL).
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
#include "pbuf.h"
#include <string.h>

static volatile int PBUFNUM;
static PBUF * pEmpty;
static PBUF stPuf[BUF_MAX_CNT];//is more than 10k byte..

void pbuf_init(void)
{
    uint8 i;

    stPuf[0].next = NULL;

    for (i = 1; i < BUF_MAX_CNT; i++)
    {
        stPuf[i].next = &stPuf[i - 1];
    }
    pEmpty = &stPuf[i - 1];
	PBUFNUM = BUF_MAX_CNT;
}

PBUF *pbuf_alloc(pbuf_layer layer, uint16 length, pbuf_type type)
{
    PBUF *p = NULL;
    uint16 offset;

    if (length > PHY_FRAME_MAX_SIZE)
    {
        return NULL;
    }

    /* determine header offset */
    offset = 0;
    switch (layer)
    {
        case PBUF_TRANSPORT:
            /* add room for transport (often TCP) layer header */
            offset += PBUF_TRANSPORT_HLEN;
			
        case PBUF_IP:
            /* add room for IP layer header */
            offset += PBUF_IP_HLEN;

        case PBUF_ETHNET:
            /* add room for link layer header */
            offset += PBUF_ETHNET_HLEN;

        case PBUF_PHY:
        	//offset += PBUF_PHY_HLEN ;
        	//offset += 8;//reservd some space.
			
        case PBUF_RAW:
            break;

        default:
            break;
    }

    switch (type)
    {
        case PBUF_POOL:
            p = pEmpty;
            if (p == NULL)
            {
				break;
            }
	#ifdef DBUGPBUF
			printf("%x:%d\n", (int)p, PBUFNUM-1);
	#endif
            pEmpty = p->next;
			p->next = NULL;
			PBUFNUM--;
            memset((void *)p, 0, sizeof(PBUF));
            p->len = length;
            /* make the payload pointer point 'offset' bytes into pbuf data memory */
            p->payload = (void *)(p->buf + offset); 
            break;

        default:
            return NULL;
    }
	if (!p)
	{
		printf("pbuf alloc error.\n");
	}

    return p;
}

uint8 pbuf_header(PBUF *p, int dec)
{
    if ((dec == 0) || (p == NULL))
    {
        return 0;
    }

    p->payload = (uint8 *)p->payload - dec;
    p->len += dec;

    return 1;

}

void pbuf_free(PBUF *p)
{   
	if (p == NULL)    
	{
        return;  
	}

    if (PBUFNUM >= BUF_MAX_CNT)
	{
		printf("pbuf free error 0x%x : %d >= BUF_MAX_CNT\n", (int)p, PBUFNUM);
        return;
	}
	#ifdef DBUGPBUF
	printf("f:%x:%d\n", (int)p,PBUFNUM+1);
	#endif
    p->next = pEmpty;   
    pEmpty = p;  
	PBUFNUM++;
	//printf("pbuf_free PBUFNUM = %d\n", PBUFNUM);

    return;
}

