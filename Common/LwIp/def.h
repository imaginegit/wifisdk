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
#ifndef __LWIP_DEF_H__
#define __LWIP_DEF_H__

#define ETHARP_HWADDR_LEN     6

/** must be the maximum of all used hardware address lengths
    across all types of interfaces in use */
#define NETIF_MAX_HWADDR_LEN 6U

#define PHY_FRAME_MAX_SIZE 1124 //100(header)+1024(data)
#define BUF_MAX_CNT  10


typedef __packed struct IP_ADDR
{
    uint32 addr;
}IP_ADDR;

/*
 * struct ipaddr2 is used in the definition of the ARP packet format in
 * order to support compilers that don't have structure packing.
 */
typedef __packed struct _IP_ADDR2
{
    uint16 addrw[2];

}IP_ADDR2;

typedef __packed struct _IP_ADDR3
{
    uint8 addrb[4];
    
}IP_ADDR3;


typedef __packed struct _ETH_ADDR
{
    uint8 addr[ETHARP_HWADDR_LEN];

}ETH_ADDR;


typedef __packed struct _TCP_HDR
{
    uint16 src;
    uint16 dest;
    uint32 seqno;
    uint32 ackno;
    uint16 _hdrlen_rsvd_flags;
    uint16 wnd;
    uint16 chksum;
    uint16 urgp;

}TCP_HDR;

typedef __packed struct _UDP_HDR
{
    uint16 src;
    uint16 dest;  /* src/dest UDP ports */
    uint16 len;
    uint16 chksum;

}UDP_HDR;


typedef __packed struct _IP_HDR
{
    /* version / header length / type of service */
    uint16 _v_hl_tos;
    /* total length */
    uint16 _len;
    /* identification */
    uint16 _id;
    /* fragment offset field */
    uint16 _offset;

    /* time to live / protocol UDP:17 TCP:6 ICMP:1*/
    uint16 _ttl_proto;
    /* checksum */
    uint16 _chksum;
    /* source and destination IP addresses */
    IP_ADDR src;
    IP_ADDR dest;

}IP_HDR;


typedef __packed struct  _ETH_HDR
{
    ETH_ADDR dest;//when broadcast it is ff ff ff ff ff ff
    ETH_ADDR src;
    uint16 type;  //0x0806 is ARP frame header..

}ETH_HDR;


/** the ARP message */
typedef __packed struct  _ETHARP_HDR
{
    ETH_HDR ethhdr;         //physical frame header
    uint16 hwtype;          //ethernet type. 0x01
    uint16 proto;           //top layer protocal..0x0800 ip proto
    uint16 _hwlen_protolen; //mac address length (6)
    uint16 opcode;          //control code,1:arp request pocket,2:reply pocket.
    ETH_ADDR shwaddr;       //sender's mac address
    IP_ADDR2 sipaddr;       //sender's ip address
    ETH_ADDR dhwaddr;       //receiver's mac address
    IP_ADDR2 dipaddr;       //receiver's ip address
}ETHARP_HDR;


typedef __packed struct _ETHIP_HDR
{
    ETH_HDR eth;
    IP_HDR ip;

}ETHIP_HDR;


typedef __packed struct _PBUF
{
    /** next pbuf in singly linked pbuf chain */
    struct _PBUF *next;

    /** pointer to the actual data in the buffer */
    void *payload;

    /** length of this buffer */
    uint16 tcplen;//only tcp data len.
    uint16 len;   //may include (TCP_FIN | TCP_SYN) add 1

    /** misc flags */
    uint8 unused[3];   
    uint8 flags;

    TCP_HDR *tcphdr;

    uint8 buf[PHY_FRAME_MAX_SIZE];

}PBUF;


/* This is the common part of all PCB types. It needs to be at the
   beginning of a PCB type definition. It is located here so that
   changes to this common part are made in one location instead of
   having to change all PCB structs. */
#define __IP_PCB \
  /* ip addresses in network byte order */ \
  IP_ADDR local_ip; \
  IP_ADDR remote_ip; \
   /* Socket options */  \
  uint16 so_options;      \
   /* Type Of Service */ \
  uint8 tos;              \
  /* Time To Live */     \
  uint8 ttl   


typedef __packed struct _IP_PCB
{
    /* Common members of all PCB types */
    __IP_PCB;
    
}IP_PCB;



enum tcp_state
{
    CLOSED      = 0,
    SYN_SENT    = 1,
    ESTABLISHED = 2,
    FIN_WAIT_1  = 3,
    FIN_WAIT_2  = 4,
    CLOSE_WAIT  = 5,
    CLOSING     = 6,
    LAST_ACK    = 7,
    TIME_WAIT   = 8
};


enum http_tcp_state
{
    TCP_CLOSED = 0,
    TCP_IDLE,
    TCP_WORKING,     
};

/**
 * members common to struct tcp_pcb and struct tcp_listen_pcb
 */
#define TCP_PCB_COMMON(type) \
  type * next; /* for the linked list */ \
  enum tcp_state state; /* TCP state */ \
  uint8 prio; \
  void *callback_arg; \
  /* ports are in host byte order */ \
  uint16 local_port


/* the TCP protocol control block */
typedef __packed struct _TCP_PCB
{
    /** common PCB members */
	/* ip addresses in network byte order */ 
	IP_ADDR local_ip;
	IP_ADDR remote_ip; 
	uint16 so_options;/* Socket options */      
	uint8 tos;/* Type Of Service */               
	uint8 ttl;/* Time To Live */ 

	/**
	 * members common to struct tcp_pcb and struct tcp_listen_pcb
	 */
    /** protocol specific PCB members */
	struct _TCP_PCB * next; /* for the linked list */ 
	enum tcp_state state; /* TCP state */ 
	uint8 prio; 
    uint16 local_port;/* ports are in host byte order */ 
    /* ports are in host byte order */
    uint16 remote_port;

    uint8 flags;

    /* the rest of the fields are in host byte order
       as we have to do some math with them */
    /* receiver variables */
    uint32 rcv_nxt;   /* next seqno expected (Ack Number)*/
    uint16 rcv_wnd;   /* receiver window available */
    uint16 rcv_ann_wnd; /* receiver window to announce */
    uint32 rcv_ann_right_edge; /* announced right edge of window */
    uint32 timerackflag;/* tcp timerack flag */
	
    /* Timers */
    uint32 tmr;
    uint8 polltmr, pollinterval;

    /* Retransmission timer. */
    int16 rtime;

    uint16 mss;   /* maximum segment size */

    /* RTT (round trip time) estimation variables */
    uint32 rttest; /* RTT estimate in 500ms ticks */
    uint32 rtseq;  /* sequence number being timed */
    int16  sa,sv;  /* @todo document this */

    int16 rto;    /* retransmission time-out */
    uint8 nrtx;    /* number of retransmissions */

    /* fast retransmit/recovery */
    uint32 lastack; /* Highest acknowledged seqno. */
    uint8  dupacks; /* retransmit requset number(>3 start once retransmit.)*/

    /* congestion avoidance/control variables */
    uint16 cwnd;
    uint16 ssthresh;

    /* sender variables */
    uint32 snd_nxt;   /* next new seqno to be sent */
    uint16 snd_wnd;   /* sender window (how many byte can been send out at most..)*/
    uint32 snd_wl1, snd_wl2; /* Sequence and acknowledgement numbers of last window update. */
    uint32 snd_lbb;       /* Sequence number of next byte to be send(Sequence Number). */

    uint16 acked; /* the count of acked data grounp */

    uint16 snd_buf;   /* Available buffer space for sending data to put in(in bytes). */

    uint16 snd_queuelen; /* Available buffer space for sending (in tcp_segs). */

    /* These are ordered by sequence number: */
    PBUF *unsent;   /* Unsent (queued) segments. */
    PBUF *unacked;  /* Sent but unacknowledged segments. */
    PBUF *ooseq;    /* Received out of sequence segments. */
    PBUF *unRead;   /*unRead by app layper*/

    /* idle time before KEEPALIVE is sent */
    uint32 keep_idle;

    /* Persist timer counter */
    uint32 persist_cnt;

    /* Persist timer back-off */
    uint8 persist_backoff;

    /* KEEPALIVE counter */
    uint8 keep_cnt_sent;

    /* receive callback function
     * addr and port are in same byte order as in the pcb
     * The callback is responsible for freeing the pbuf
     * if it's not used any more.
     *
     * @param arg user supplied argument (udp_pcb.recv_arg)
     * @param pcb the udp_pcb which received data
     * @param p the packet buffer that was received
     * @param addr the remote IP address from which the packet was received
     * @param port the remote port from which the packet was received
     */
    void (* recv)(PBUF *p);
    void (* pcb_close)(uint8 bTcbState);

}TCP_PCB;


typedef __packed struct _UDP_PCB
{
    /* Common members of all PCB types */
	/* ip addresses in network byte order */ 
	IP_ADDR local_ip; 
	IP_ADDR remote_ip; 
	/* Socket options */  
	uint16 so_options;      
	/* Type Of Service */ 
	uint8 tos;              
	/* Time To Live */     
	uint8 ttl;  

    uint8 flags;
    /* ports are in host byte order */
    uint16 local_port;
    uint16 remote_port;

    /* receive callback function
     * addr and port are in same byte order as in the pcb
     * The callback is responsible for freeing the pbuf
     * if it's not used any more.
     *
     * @param arg user supplied argument (udp_pcb.recv_arg)
     * @param pcb the udp_pcb which received data
     * @param p the packet buffer that was received
     * @param addr the remote IP address from which the packet was received
     * @param port the remote port from which the packet was received
     */
    void (* recv)(void *arg, struct _UDP_PCB *pcb, PBUF *p, IP_ADDR *addr, uint16 port);
    /* user-supplied argument for the recv callback */
    void *recv_arg;

}UDP_PCB;


typedef __packed struct _AUTO_IP
{
    IP_ADDR llipaddr;  /* the currently selected, probed, announced or used LL IP-Address */
    uint8 state;               /* current AutoIP state machine state */
    uint8 sent_num;            /* sent number of probes or announces, dependent on state */
    uint16 ttw;                /* ticks to wait, tick is AUTOIP_TMR_INTERVAL long */
    uint8 lastconflict;        /* ticks until a conflict can be solved by defending */
    uint8 tried_llipaddr;      /* total number of probed/used Link Local IP-Addresses */

}AUTO_IP;


#define DHCP_CHADDR_LEN 16U
#define DHCP_SNAME_LEN 64U
#define DHCP_FILE_LEN 128U
#define DHCP_MIN_OPTIONS_LEN 68U

/** minimum set of fields of any DHCP message */
typedef __packed struct _DHCP_MSG
{
    uint8 op;
    uint8 htype;
    uint8 hlen;
    uint8 hops;
    uint32 xid;
    uint16 secs;
    uint16 flags;
    IP_ADDR ciaddr;
    IP_ADDR yiaddr;
    IP_ADDR siaddr;
    IP_ADDR giaddr;
    uint8 chaddr[DHCP_CHADDR_LEN];
    uint8 sname[DHCP_SNAME_LEN];
    uint8 file[DHCP_FILE_LEN];
    uint32 cookie;
    uint8 options[DHCP_MIN_OPTIONS_LEN];

} DHCP_MSG;



typedef __packed struct  _DHCP
{
    /** current DHCP state machine state */
    uint8 state;
    /** retries of current request */
    uint8 tries;
    /** transaction identifier of last sent request */
    uint32 xid;
    /** our connection to the DHCP server */
    UDP_PCB *pcb;
    /** (first) pbuf of incoming msg */
    PBUF *p;
    /** incoming msg */
    DHCP_MSG *msg_in;
    /** incoming msg options */
    DHCP_MSG *options_in;
    /** ingoing msg options length */
    uint16 options_in_len;

    PBUF *p_out; /* pbuf of outcoming msg */
    DHCP_MSG *msg_out; /* outgoing msg */
    uint16 options_out_len; /* outgoing msg options length */
    uint16 request_timeout; /* #ticks with period DHCP_FINE_TIMER_SECS for request timeout */
    uint16 t1_timeout;  /* #ticks with period DHCP_COARSE_TIMER_SECS for renewal time */
    uint16 t2_timeout;  /* #ticks with period DHCP_COARSE_TIMER_SECS for rebind time */
    IP_ADDR server_ip_addr; /* dhcp server address that offered this lease */
    IP_ADDR offered_ip_addr;
    IP_ADDR offered_sn_mask;
    IP_ADDR offered_gw_addr;
    IP_ADDR offered_bc_addr;
#define DHCP_MAX_DNS 2
    uint32 dns_count; /* actual number of DNS servers obtained */
    IP_ADDR offered_dns_addr[DHCP_MAX_DNS]; /* DNS server addresses */

    uint32 offered_t0_lease; /* lease period (in seconds) */
    uint32 offered_t1_renew; /* recommended renew time (usually 50% of lease period) */
    uint32 offered_t2_rebind; /* recommended rebind time (usually 66% of lease period)  */

    uint8 autoip_coop_state;
    
    /** Patch #1308
     *  TODO: See dhcp.c "TODO"s
     */
}DHCP;

/*this struct represent our wifi ethernet information..*/
typedef __packed struct _NETIF
{
    /** IP address configuration in network byte order */
    IP_ADDR ip_addr;
    IP_ADDR netmask;
    IP_ADDR gw;

    /** This function is called by the network device driver
     *  to pass a packet up the TCP/IP stack. */
    uint8 (* input)(PBUF *p, struct _NETIF *inp);
	
    /** This function is called by the IP module when it wants
     *  to send a packet on the interface. This function typically
     *  first resolves the hardware address, then sends the packet. */
    uint8 (* output)(struct _NETIF *netif, PBUF *p, IP_ADDR *ipaddr); //etharp_output 
    
    /** This function is called by the ARP module when it wants
     *  to send a packet on the interface. This function outputs
     *  the pbuf as-is on the link medium. */
    uint8 (* linkoutput)(struct _NETIF *netif, PBUF *p);//low_level_output


    /** the DHCP client state information for this netif */
    DHCP *dhcp;
    /** the AutoIP client state information for this netif */
    AUTO_IP *autoip;
    /* the hostname for this netif, NULL is a valid value */
    char*  hostname;
    /** number of bytes used in hwaddr */
    uint8 hwaddr_len;
    /** link level hardware address of this interface */
    uint8 hwaddr[NETIF_MAX_HWADDR_LEN];
    /** maximum transfer unit (in bytes) */
    uint16 mtu;
    /** flags (see NETIF_FLAG_ above) */
    uint8 flags;
    /** descriptive abbreviation */
    char name[2];
    /** number of this interface */
    uint8 num;

}NETIF;


/* 802.11-related definitions */

/* TxPD descriptor */
typedef __packed struct _TXPD
{
	/* union to cope up with later FW revisions */
	uint32 xxx;
	/* Tx control */
	uint32 tx_control;
	uint32 tx_packet_location;
	/* Tx packet length */
	uint16 tx_packet_length;
	/* First 2 byte of destination MAC address */
	uint8 tx_dest_addr_high[2];
	/* Last 4 byte of destination MAC address */
	uint8 tx_dest_addr_low[4];
	/* Pkt Priority */
	uint8 priority;
	/* Pkt Trasnit Power control */
	uint8 powermgmt;
	/* Amount of time the packet has been queued (units = 2ms) */
	uint8 pktdelay_2ms;
	/* reserved */
	uint8 reserved1;
}TXPD;

/* RxPD Descriptor */
typedef __packed struct _RXPD
{
	/* union to cope up with later FW revisions */
	uint16 xxx;

	/* SNR */
	uint8 snr;

	/* Tx control */
	uint8 rx_control;

	/* Pkt length */
	uint16 pkt_len;

	/* Noise Floor */
	uint8 nf;

	/* Rx Packet Rate */
	uint8 rx_rate;

	/* Pkt addr */
	uint32 pkt_ptr;

	/* Next Rx RxPD addr */
	uint32 next_rxpd_ptr;

	/* Pkt Priority */
	uint8 priority;
	uint8 reserved[3];
} RXPD;


#define LWIP_MAX(x , y)  (((x) > (y)) ? (x) : (y))
#define LWIP_MIN(x , y)  (((x) < (y)) ? (x) : (y))


#endif /* __LWIP_DEF_H__ */

