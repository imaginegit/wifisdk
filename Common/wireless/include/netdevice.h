/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions for the Interfaces handler.
 *
 * Version:	@(#)dev.h	1.0.10	08/12/93
 *
 * Authors:	Ross Biro
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Corey Minyard <wf-rch!minyard@relay.EU.net>
 *		Donald J. Becker, <becker@cesdis.gsfc.nasa.gov>
 *		Alan Cox, <alan@lxorguk.ukuu.org.uk>
 *		Bjorn Ekwall. <bj0rn@blox.se>
 *              Pekka Riikonen <priikone@poseidon.pspt.fi>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 *		Moved to /usr/include/linux for NET3
 */
#ifndef _LINUX_NETDEVICE_H
#define _LINUX_NETDEVICE_H

#define MAX_ADDR_LEN	6		/* Largest hardware address length */
#define GSO_MAX_SIZE		65536

#define	NETDEV_ALIGN		32

/*
 *	Old network device statistics. Fields are native words
 *	(unsigned long) so they can be read and written atomically.
 */

struct net_device_stats 
{
	unsigned long	rx_packets;
	unsigned long	tx_packets;
	unsigned long	rx_bytes;
	unsigned long	tx_bytes;
	unsigned long	rx_errors;
	unsigned long	tx_errors;
	unsigned long	rx_dropped;
	unsigned long	tx_dropped;
	unsigned long	multicast;
	unsigned long	collisions;
	unsigned long	rx_length_errors;
	unsigned long	rx_over_errors;
	unsigned long	rx_crc_errors;
	unsigned long	rx_frame_errors;
	unsigned long	rx_fifo_errors;
	unsigned long	rx_missed_errors;
	unsigned long	tx_aborted_errors;
	unsigned long	tx_carrier_errors;
	unsigned long	tx_fifo_errors;
	unsigned long	tx_heartbeat_errors;
	unsigned long	tx_window_errors;
	unsigned long	rx_compressed;
	unsigned long	tx_compressed;
};

/*
 *	The DEVICE structure.
 *	Actually, this whole structure is a big mistake.  It mixes I/O
 *	data with strictly "high-level" data, and it has to know about
 *	almost every data structure used in the INET module.
 *
 *	FIXME: cleanup struct net_device such that network protocol info
 *	moves out.
 */

struct net_device 
{
	/*
	 * This is the first field of the "visible" part of this structure
	 * (i.e. as seen by users in the "Space.c" file).  It is the name
	 * of the interface.
	 */
	char			name[IFNAMSIZ];

	//struct pm_qos_request_list pm_qos_req;

	/* device name hash chain */
	//struct hlist_node	name_hlist;
	
	/* snmp alias */
	//char 			*ifalias;

	/*
	 *	I/O specific fields
	 *	FIXME: Merge these and struct ifmap into one
	 */
	unsigned long		mem_end;	/* shared mem end	*/
	unsigned long		mem_start;	/* shared mem start	*/
	unsigned long		base_addr;	/* device I/O address	*/
	unsigned int		irq;		/* device IRQ number	*/

	/*
	 *	Some hardware also needs these fields, but they are not
	 *	part of the usual set specified in Space.c.
	 */

	unsigned long		state;

	//struct list_head	dev_list;
	//struct list_head	napi_list;
	//struct list_head	unreg_list;

	/* currently active device features */
	uint32			features;
	/* user-changeable features */
	uint32			hw_features;
	/* user-requested features */
	uint32			wanted_features;
	/* mask of features inheritable by VLAN devices */
	uint32			vlan_features;

	/* Net device feature bits; if you change something,
	 * also update netdev_features_strings[] in ethtool.c */

#define NETIF_F_SG		1	/* Scatter/gather IO. */
#define NETIF_F_IP_CSUM		2	/* Can checksum TCP/UDP over IPv4. */
#define NETIF_F_NO_CSUM		4	/* Does not require checksum. F.e. loopack. */
#define NETIF_F_HW_CSUM		8	/* Can checksum all the packets. */
#define NETIF_F_IPV6_CSUM	16	/* Can checksum TCP/UDP over IPV6 */
#define NETIF_F_HIGHDMA		32	/* Can DMA to high memory. */
#define NETIF_F_FRAGLIST	64	/* Scatter/gather IO. */
#define NETIF_F_HW_VLAN_TX	128	/* Transmit VLAN hw acceleration */
#define NETIF_F_HW_VLAN_RX	256	/* Receive VLAN hw acceleration */
#define NETIF_F_HW_VLAN_FILTER	512	/* Receive filtering on VLAN */
#define NETIF_F_VLAN_CHALLENGED	1024	/* Device cannot handle VLAN packets */
#define NETIF_F_GSO		2048	/* Enable software GSO. */
#define NETIF_F_LLTX		4096	/* LockLess TX - deprecated. Please */
					/* do not use LLTX in new drivers */
#define NETIF_F_NETNS_LOCAL	8192	/* Does not change network namespaces */
#define NETIF_F_GRO		16384	/* Generic receive offload */
#define NETIF_F_LRO		32768	/* large receive offload */

/* the GSO_MASK reserves bits 16 through 23 */
#define NETIF_F_FCOE_CRC	(1 << 24) /* FCoE CRC32 */
#define NETIF_F_SCTP_CSUM	(1 << 25) /* SCTP checksum offload */
#define NETIF_F_FCOE_MTU	(1 << 26) /* Supports max FCoE MTU, 2158 bytes*/
#define NETIF_F_NTUPLE		(1 << 27) /* N-tuple filters supported */
#define NETIF_F_RXHASH		(1 << 28) /* Receive hashing offload */
#define NETIF_F_RXCSUM		(1 << 29) /* Receive checksumming offload */
#define NETIF_F_NOCACHE_COPY	(1 << 30) /* Use no-cache copyfromuser */
#define NETIF_F_LOOPBACK	(1 << 31) /* Enable loopback */

	/* Segmentation offload features */
//#define NETIF_F_GSO_SHIFT	16
//#define NETIF_F_GSO_MASK	0x00ff0000
//#define NETIF_F_TSO		(SKB_GSO_TCPV4 << NETIF_F_GSO_SHIFT)
//#define NETIF_F_UFO		(SKB_GSO_UDP << NETIF_F_GSO_SHIFT)
//#define NETIF_F_GSO_ROBUST	(SKB_GSO_DODGY << NETIF_F_GSO_SHIFT)
//#define NETIF_F_TSO_ECN		(SKB_GSO_TCP_ECN << NETIF_F_GSO_SHIFT)
//#define NETIF_F_TSO6		(SKB_GSO_TCPV6 << NETIF_F_GSO_SHIFT)
//#define NETIF_F_FSO		(SKB_GSO_FCOE << NETIF_F_GSO_SHIFT)

	/* Features valid for ethtool to change */
	/* = all defined minus driver/device-class-related */
//#define NETIF_F_NEVER_CHANGE	(NETIF_F_VLAN_CHALLENGED | NETIF_F_LLTX | NETIF_F_NETNS_LOCAL)
//#define NETIF_F_ETHTOOL_BITS	(0xff3fffff & ~NETIF_F_NEVER_CHANGE)

	/* List of features with software fallbacks. */
//#define NETIF_F_GSO_SOFTWARE	(NETIF_F_TSO | NETIF_F_TSO_ECN | NETIF_F_TSO6 | NETIF_F_UFO)


#define NETIF_F_GEN_CSUM	(NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)
#define NETIF_F_V4_CSUM		(NETIF_F_GEN_CSUM | NETIF_F_IP_CSUM)
#define NETIF_F_V6_CSUM		(NETIF_F_GEN_CSUM | NETIF_F_IPV6_CSUM)
#define NETIF_F_ALL_CSUM	(NETIF_F_V4_CSUM | NETIF_F_V6_CSUM)

//#define NETIF_F_ALL_TSO 	(NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_TSO_ECN)

//#define NETIF_F_ALL_FCOE	(NETIF_F_FCOE_CRC | NETIF_F_FCOE_MTU | NETIF_F_FSO)

	/*
	 * If one device supports one of these features, then enable them
	 * for all in netdev_increment_features.
	 */
//#define NETIF_F_ONE_FOR_ALL	(NETIF_F_GSO_SOFTWARE | NETIF_F_GSO_ROBUST | \
//				 NETIF_F_SG | NETIF_F_HIGHDMA |		\
//				 NETIF_F_FRAGLIST | NETIF_F_VLAN_CHALLENGED)
	/*
	 * If one device doesn't support one of these features, then disable it
	 * for all in netdev_increment_features.
	 */
//#define NETIF_F_ALL_FOR_ALL	(NETIF_F_NOCACHE_COPY | NETIF_F_FSO)

	/* changeable features with no special hardware requirements */
#define NETIF_F_SOFT_FEATURES	(NETIF_F_GSO | NETIF_F_GRO)

	/* Interface index. Unique device identifier	*/
	int			ifindex;
	int			iflink;

	//struct net_device_stats	stats;
	//atomic_long_t		rx_dropped;
                                	/* dropped packets by core network
                                     * Do not use this in drivers.
                                     */

	/* Management operations */
	//const struct net_device_ops *netdev_ops;
	//const struct ethtool_ops *ethtool_ops;

	/* Hardware header description */
	//const struct header_ops *header_ops;

	//unsigned int		flags;	/* interface flags (a la BSD)	*/
	//unsigned int		priv_flags; /* Like 'flags' but invisible to userspace. */
	//unsigned short		gflags;
	unsigned short		padded;	/* How much padding added by alloc_netdev() */

	//unsigned char		operstate; /* RFC2863 operstate */
	//unsigned char		link_mode; /* mapping policy to operstate */

	//unsigned char		if_port;	/* Selectable AUI, TP,..*/
	unsigned char		dma;		/* DMA channel		*/

	unsigned int		mtu;	/* interface MTU value		*/
	unsigned short		type;	/* interface hardware type	*/
	unsigned short		hard_header_len;	/* hardware hdr length	*/

	/* extra head- and tailroom the hardware may need, but not in all cases
	 * can this be guaranteed, especially tailroom. Some cases also use
	 * LL_MAX_HEADER instead to allocate the skb.
	 */
	unsigned short		needed_headroom;
	unsigned short		needed_tailroom;

	/* Interface address info. */
	unsigned char		perm_addr[MAX_ADDR_LEN]; /* permanent hw address */
	unsigned char		addr_assign_type; /* hw address assignment type */
	unsigned char		addr_len;	/* hardware address length	*/
	//unsigned short          dev_id;		/* for shared network cards */

	//struct netdev_hw_addr_list	uc;	/* Unicast mac addresses */
	//struct netdev_hw_addr_list	mc;	/* Multicast mac addresses */
	int			uc_promisc;
	unsigned int		promiscuity;
	unsigned int		allmulti;


	/* Protocol specific pointers */

	void 			*atalk_ptr;	/* AppleTalk link 	*/
	//struct in_device __rcu	*ip_ptr;	/* IPv4 specific data	*/
	//struct dn_dev __rcu     *dn_ptr;        /* DECnet specific data */
	//struct inet6_dev __rcu	*ip6_ptr;       /* IPv6 specific data */
	//void			*ec_ptr;	/* Econet specific data	*/
	//void			*ax25_ptr;	/* AX.25 specific data */
	//struct wireless_dev	*ieee80211_ptr; /* IEEE 802.11 specific data,assign before registering */

/*
 * Cache lines mostly used on receive path (including eth_type_trans())
 */
	unsigned long		last_rx;	/* Time of last Rx
						 * This should not be set in
						 * drivers, unless really needed,
						 * because network stack (bonding)
						 * use it if/when necessary, to
						 * avoid dirtying this cache line.
						 */

	/* Interface address info used in eth_type_trans() */
	unsigned char		*dev_addr;	/* hw address, (before bcast
						   because most packets are
						   unicast) */

	//struct netdev_hw_addr_list	dev_addrs; /* list of device hw addresses */

	unsigned char		broadcast[MAX_ADDR_LEN];	/* hw bcast add	*/


	//struct kset		*queues_kset;
	//struct netdev_rx_queue	*_rx;
	
	/* Number of RX queues allocated at register_netdev() time */
	unsigned int		num_rx_queues;

	/* Number of RX queues currently active in device */
	unsigned int		real_num_rx_queues;

	/* CPU reverse-mapping for RX completion interrupts, indexed
	 * by RX queue number.  Assigned by driver.  This must only be
	 * set if the ndo_rx_flow_steer operation is defined. */
	//struct cpu_rmap		*rx_cpu_rmap;

	//rx_handler_func_t __rcu	*rx_handler;
	//void __rcu		*rx_handler_data;

	//struct netdev_queue __rcu *ingress_queue;

/*
 * Cache lines mostly used on transmit path
 */
	//struct netdev_queue	*_tx ____cacheline_aligned_in_smp;

	/* Number of TX queues allocated at alloc_netdev_mq() time  */
	unsigned int		num_tx_queues;

	/* Number of TX queues currently active in device  */
	unsigned int		real_num_tx_queues;

	/* root qdisc from userspace point of view */
	//struct Qdisc		*qdisc;

	unsigned long		tx_queue_len;	/* Max frames per queue allowed */


	/* These may be needed for future network-power-down code. */

	/*
	 * trans_start here is expensive for high speed devices on SMP,
	 * please use netdev_queue->trans_start instead.
	 */
	unsigned long		trans_start;	/* Time (in jiffies) of last Tx	*/

	/* delayed register/unregister */
	//struct list_head	todo_list;
	/* device index hash chain */
	//struct hlist_node	index_hlist;

	//struct list_head	link_watch_list;

	/* register/unregister state machine */
	enum 
	{ 
		NETREG_UNINITIALIZED=0,
		NETREG_REGISTERED,	/* completed register_netdevice */
		NETREG_UNREGISTERING,	/* called unregister_netdevice */
		NETREG_UNREGISTERED,	/* completed unregister todo */
		NETREG_RELEASED,		/* called free_netdev */
		NETREG_DUMMY,		/* dummy device for NAPI poll */
	} reg_state:8;

	bool dismantle; /* device is going do be freed */

	enum 
	{
		RTNL_LINK_INITIALIZED,
		RTNL_LINK_INITIALIZING,
	} rtnl_link_state:16;

	/* Called from unregister, can be used to call free_netdev */
	//void (*destructor)(struct net_device *dev);
	/* mid-layer private */
	void				*ml_priv;
	
	/* GARP */
	//struct garp_port __rcu	*garp_port;

	/* space for optional device, statistics, and wireless sysfs groups */
	//const struct attribute_group *sysfs_groups[4];

	/* rtnetlink link ops */
	//const struct rtnl_link_ops *rtnl_link_ops;

	/* for setting kernel sock attribute on TCP connection setup */
	unsigned int		gso_max_size;

	/* group the device belongs to */
	int group;
};



/**
 *	netdev_priv - access network device private data
 *	@dev: network device
 *
 * Get network device private data
 */
static __inline void *netdev_priv(const struct net_device *dev)
{
	return (char *)dev + ALIGN(sizeof(struct net_device), NETDEV_ALIGN);
}


#endif	/* _LINUX_NETDEVICE_H */
