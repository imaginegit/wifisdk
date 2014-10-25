/*
 * Header file describing the internal (inter-module) DHD interfaces.
 *
 * Provides type definitions and function prototypes used to link the
 * DHD OS, bus, and protocol modules.
 *
 * Copyright (C) 1999-2011, Broadcom Corporation
 * 
 *         Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 * 
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 * 
 *      Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a license
 * other than the GPL, without Broadcom's express prior written consent.
 *
 * $Id: dhd.h 311717 2012-01-31 03:11:13Z $
 */

/****************
 * Common types *
 */

#ifndef _dhd_h_
#define _dhd_h_

#ifndef ASSERT
#define ASSERT(x)	do {if ((x) == 0)  \
                        {RKDEBUG("ASSERT"); \
                        while(1);            \
                        }                  \
                       } while(0)
#endif

/* The level of bus communication with the dongle */
enum dhd_bus_state 
{
	DHD_BUS_DOWN,		/* Not ready for frame transfers */
	DHD_BUS_LOAD,		/* Download access only (CPU reset) */
	DHD_BUS_DATA		/* Ready for frame transfers */
};

//extern define
#define WL_EVENTING_MASK_LEN    16
#define WLC_CNTRY_BUF_SZ    4       

typedef struct wl_country 
{
	char country_abbrev[WLC_CNTRY_BUF_SZ];  
	int32 rev;              
	char ccode[WLC_CNTRY_BUF_SZ];       
} wl_country_t;

/* Firmware requested operation mode */
#define STA_MASK			0x0001
#define HOSTAPD_MASK			0x0002
#define WFD_MASK			0x0004
#define SOFTAP_FW_MASK			0x0008

/* max sequential rxcntl timeouts to set HANG event */
#define MAX_CNTL_TIMEOUT  2

#define DHD_SCAN_ACTIVE_TIME	 40 /* ms : Embedded default Active setting from DHD Driver */
#define DHD_SCAN_PASSIVE_TIME	130 /* ms: Embedded default Passive setting from DHD Driver */

enum dhd_bus_wake_state {
	WAKE_LOCK_OFF,
	WAKE_LOCK_PRIV,
	WAKE_LOCK_DPC,
	WAKE_LOCK_IOCTL,
	WAKE_LOCK_DOWNLOAD,
	WAKE_LOCK_TMOUT,
	WAKE_LOCK_WATCHDOG,
	WAKE_LOCK_LINK_DOWN_TMOUT,
	WAKE_LOCK_PNO_FIND_TMOUT,
	WAKE_LOCK_SOFTAP_SET,
	WAKE_LOCK_SOFTAP_STOP,
	WAKE_LOCK_SOFTAP_START,
	WAKE_LOCK_SOFTAP_THREAD,
	WAKE_LOCK_MAX
};
enum dhd_prealloc_index {
	DHD_PREALLOC_PROT = 0,
	DHD_PREALLOC_RXBUF,
	DHD_PREALLOC_DATABUF,
	DHD_PREALLOC_OSL_BUF
};

typedef enum  {
	DHD_IF_NONE = 0,
	DHD_IF_ADD,
	DHD_IF_DEL,
	DHD_IF_CHANGE,
	DHD_IF_DELETING
} dhd_if_state_t;

/* Packet alignment for most efficient SDIO (can change based on platform) */
#ifndef DHD_SDALIGN
#define DHD_SDALIGN	32
#endif


/* Common structure for module and instance linkage */
typedef struct dhd_pub 
{
	/* Linkage ponters */
	struct dhd_bus *bus;	/* Bus module handle */
	struct dhd_prot *prot;	/* Protocol module handle */
	struct dhd_info *info;  /* Info module handle */

	/* Internal dhd items */
	bool up;		/* Driver up/down (to OS) */
	bool txoff;		/* Transmit flow-controlled */
	bool dongle_reset;  /* TRUE = DEVRESET put dongle into reset */
	enum dhd_bus_state busstate;
	uint hdrlen;		/* Total DHD header length (proto + bus) */
	uint maxctl;		/* Max size rxctl request from proto to bus */
	uint rxsz;		/* Rx buffer size bus module should use */
	uint8 wme_dp;	/* wme discard priority */

	/* Dongle media info */
	bool iswl;		/* Dongle-resident driver is wl */
	uint drv_version;	/* Version of dongle-resident driver */
	struct ether_addr mac;	/* MAC address obtained from dongle */
	//dngl_stats_t dstats;	/* Stats for dongle-based data */

	/* Additional stats for the bus level */
	uint tx_packets;	/* Data packets sent to dongle */
	uint tx_multicast;	/* Multicast data packets sent to dongle */
	uint tx_errors;	/* Errors in sending data to dongle */
    uint tx_ctlpkts;	/* Control packets sent to dongle */
	uint tx_ctlerrs;	/* Errors sending control frames to dongle */
	uint rx_packets;	/* Packets sent up the network interface */
	uint rx_multicast;	/* Multicast packets sent up the network interface */
	uint rx_errors;	/* Errors processing rx data packets */
	uint rx_ctlpkts;	/* Control frames processed from dongle */
	uint rx_ctlerrs;	/* Errors in processing rx control frames */
	uint rx_dropped;	/* Packets dropped locally (no memory) */
	uint rx_flushed;  /* Packets flushed due to unscheduled sendup thread */
	uint wd_dpc_sched;   /* Number of times dhd dpc scheduled by watchdog timer */

	uint rx_readahead_cnt;	/* Number of packets where header read-ahead was used. */
	uint tx_realloc;	/* Number of tx packets we had to realloc for headroom */
	uint fc_packets;       /* Number of flow control pkts recvd */

	/* Last error return */
	int bcmerror;
	uint tickcnt;

	/* Last error from dongle */
	int dongle_error;

	/* Suspend disable flag and "in suspend" flag */
	int suspend_disable_flag; /* "1" to disable all extra powersaving during suspend */
	int in_suspend;			/* flag set to 1 when early suspend called */
	int pno_enable;                 /* pno status : "1" is pno enable */
	int dtim_skip;         /* dtim skip , default 0 means wake each dtim */

	/* Pkt filter defination */
	char * pktfilter[100];
	int pktfilter_count;

	wl_country_t dhd_cspec;		/* Current Locale info */
	char eventmask[WL_EVENTING_MASK_LEN];
	int	op_mode;				/* STA, HostAPD, WFD, SoftAP */

	uint16	maxdatablks;
	bool	dongle_isolation;
	//int   hang_was_sent;
	int   rxcnt_timeout;		/* counter rxcnt timeout to send HANG */
	int   txcnt_timeout;		/* counter txcnt timeout to send HANG */
    int   ifIdx;
} dhd_pub_t;

#define DHD_IF_VIF	0x01	/* Virtual IF (Hidden from user) */


/*  Wakelock Functions */
//extern int dhd_os_wake_lock(dhd_pub_t *pub);
//extern int dhd_os_wake_unlock(dhd_pub_t *pub);
//extern int dhd_os_wake_lock_timeout(dhd_pub_t *pub);
//extern int dhd_os_wake_lock_timeout_enable(dhd_pub_t *pub, int val);

#define DHD_PACKET_TIMEOUT_MS	1000
#define DHD_EVENT_TIMEOUT_MS	1500


typedef struct dhd_if_event 
{
	uint8 ifidx;
	uint8 action;
	uint8 flags;
	uint8 bssidx;
	uint8 is_AP;
} dhd_if_event_t;


typedef enum dhd_attach_states
{
	DHD_ATTACH_STATE_INIT = 0x0,
	DHD_ATTACH_STATE_NET_ALLOC = 0x1,
	DHD_ATTACH_STATE_DHD_ALLOC = 0x2,
	DHD_ATTACH_STATE_ADD_IF = 0x4,
	DHD_ATTACH_STATE_PROT_ATTACH = 0x8,
	DHD_ATTACH_STATE_WL_ATTACH = 0x10,
	DHD_ATTACH_STATE_THREADS_CREATED = 0x20,
	DHD_ATTACH_STATE_WAKELOCKS_INIT = 0x40,
	DHD_ATTACH_STATE_CFG80211 = 0x80,
	DHD_ATTACH_STATE_EARLYSUSPEND_DONE = 0x100,
	DHD_ATTACH_STATE_DONE = 0x200
} dhd_attach_states_t;

/* Value -1 means we are unsuccessful in creating the kthread. */
#define DHD_PID_KT_INVALID 	-1
/* Value -2 means we are unsuccessful in both creating the kthread and tasklet */
#define DHD_PID_KT_TL_INVALID	-2

/*
 * Exported from dhd OS modules (dhd_linux/dhd_ndis)
 */


/* Indication from bus module regarding presence/insertion of dongle.
 * Return dhd_pub_t pointer, used as handle to OS module in later calls.
 * Returned structure should have bus and prot pointers filled in.
 * bus_hdrlen specifies required headroom for bus module header.
 */
extern dhd_pub_t *dhd_attach(struct dhd_bus *bus, uint bus_hdrlen);
//extern int dhd_net_attach(dhd_pub_t *dhdp, int idx);

/* Indication from bus module regarding removal/absence of dongle */
//extern void dhd_detach(dhd_pub_t *dhdp);
//extern void dhd_free(dhd_pub_t *dhdp);

/* Indication from bus module to change flow-control state */
//extern void dhd_txflowcontrol(dhd_pub_t *dhdp, int ifidx, bool on);

//extern bool dhd_prec_enq(dhd_pub_t *dhdp, struct pktq *q, void *pkt, int prec);

/* Receive frame for delivery to OS.  Callee disposes of rxp. */
extern void dhd_rx_frame(dhd_pub_t *dhdp, int ifidx, void *rxp, int numpkt, uint8 chan);

/* Return pointer to interface name */
//extern char *dhd_ifname(dhd_pub_t *dhdp, int idx);

/* Request scheduling of the bus dpc */
//extern void dhd_sched_dpc(dhd_pub_t *dhdp);

/* Notify tx completion */
//extern void dhd_txcomplete(dhd_pub_t *dhdp, void *txp, bool success);

/* OS independent layer functions */
//extern int dhd_os_proto_block(dhd_pub_t * pub);
//extern int dhd_os_proto_unblock(dhd_pub_t * pub);
//extern int dhd_os_ioctl_resp_wait(dhd_pub_t * pub, uint * condition, bool * pending);
//extern int dhd_os_ioctl_resp_wake(dhd_pub_t * pub);
//extern unsigned int dhd_os_get_ioctl_resp_timeout(void);
//extern void dhd_os_set_ioctl_resp_timeout(unsigned int timeout_msec);
//extern void * dhd_os_open_image(char * filename);
//extern int dhd_os_get_image_block(char * buf, int len, void * image);
//extern void dhd_os_close_image(void * image);
//extern void dhd_os_wd_timer(void *bus, uint wdtick);
//extern void dhd_init_lock_local(dhd_pub_t * pub); // terence 20120530: fix for preinit function missed called after resume
//extern void dhd_init_unlock_local(dhd_pub_t * pub);
//extern void dhd_os_sdlock(dhd_pub_t * pub);
//extern void dhd_os_sdunlock(dhd_pub_t * pub);
//extern void dhd_os_sdlock_txq(dhd_pub_t * pub);
//extern void dhd_os_sdunlock_txq(dhd_pub_t * pub);
//extern void dhd_os_sdlock_rxq(dhd_pub_t * pub);
//extern void dhd_os_sdunlock_rxq(dhd_pub_t * pub);
//extern void dhd_os_sdlock_sndup_rxq(dhd_pub_t * pub);
//extern void dhd_customer_gpio_wlan_ctrl(int onoff);
//extern int dhd_custom_get_mac_address(unsigned char *buf);
extern int wifi_set_mac_addr(unsigned char *buf);
extern char * wifi_get_mac_addr(unsigned char *buf);
//extern void dhd_os_sdunlock_sndup_rxq(dhd_pub_t * pub);
//extern void dhd_os_sdlock_eventq(dhd_pub_t * pub);
//extern void dhd_os_sdunlock_eventq(dhd_pub_t * pub);
extern bool dhd_os_check_hang(dhd_pub_t *dhdp, int ifidx, int ret);

//extern int dhd_pno_enable(dhd_pub_t *dhd, int pfn_enabled);
//extern int dhd_pno_clean(dhd_pub_t *dhd);
//extern int dhd_pno_set(dhd_pub_t *dhd, wlc_ssid_t* ssids_local, int nssid,
//                       ushort  scan_fr, int pno_repeat, int pno_freq_expo_max);
//extern int dhd_pno_get_status(dhd_pub_t *dhd);
//extern int dhd_dev_pno_reset(struct net_device *dev);
//extern int dhd_dev_pno_set(struct net_device *dev, wlc_ssid_t* ssids_local,
//                           int nssid, ushort  scan_fr, int pno_repeat, int pno_freq_expo_max);
//extern int dhd_dev_pno_enable(struct net_device *dev,  int pfn_enabled);
//extern int dhd_dev_get_pno_status(struct net_device *dev);

#define DHD_UNICAST_FILTER_NUM		0
#define DHD_BROADCAST_FILTER_NUM	1
#define DHD_MULTICAST4_FILTER_NUM	2
#define DHD_MULTICAST6_FILTER_NUM	3
//extern int net_os_set_packet_filter(struct net_device *dev, int val);
//extern int net_os_rxfilter_add_remove(struct net_device *dev, int val, int num);

//extern int dhd_get_dtim_skip(dhd_pub_t *dhd);
extern bool dhd_check_ap_wfd_mode_set(dhd_pub_t *dhd);
//extern void dhd_os_sdtxlock(dhd_pub_t * pub);
//extern void dhd_os_sdtxunlock(dhd_pub_t * pub);

//extern int dhd_ifname2idx(struct dhd_info *dhd, char *name);
//extern int dhd_net2idx(struct dhd_info *dhd, struct net_device *net);
//extern struct net_device * dhd_idx2net(void *pub, int ifidx);
extern int wl_host_event(dhd_pub_t *dhd_pub, int *idx, void *pktdata,
                         wl_event_msg_t *, void **data_ptr);
//extern void wl_event_to_host_order(wl_event_msg_t * evt);

extern int dhd_wl_ioctl_cmd(dhd_pub_t *dhd_pub, int cmd, void *arg, int len, uint8 set);

//extern struct dhd_cmn *dhd_common_init(osl_t *osh);
//extern void dhd_common_deinit(dhd_pub_t *dhd_pub, dhd_cmn_t *sa_cmn);

//extern int dhd_do_driver_init(struct net_device *net);
//extern int dhd_add_if(struct dhd_info *dhd, int ifidx, void *handle,
//	char *name, uint8 *mac_addr, uint32 flags, uint8 bssidx);
//extern void dhd_del_if(struct dhd_info *dhd, int ifidx);

//extern void dhd_vif_add(struct dhd_info *dhd, int ifidx, char * name);
//extern void dhd_vif_del(struct dhd_info *dhd, int ifidx);

//extern void dhd_event(struct dhd_info *dhd, char *evpkt, int evlen, int ifidx);
//extern void dhd_vif_sendup(struct dhd_info *dhd, int ifidx, uint8 *cp, int len);


/* Send packet to dongle via data channel */
//extern int dhd_sendpkt(dhd_pub_t *dhdp, int ifidx, void *pkt);

/* send up locally generated event */
//extern void dhd_sendup_event_common(dhd_pub_t *dhdp, wl_event_msg_t *event, void *data);
/* Send event to host */
//extern void dhd_sendup_event(dhd_pub_t *dhdp, wl_event_msg_t *event, void *data);
//extern int dhd_bus_devreset(dhd_pub_t *dhdp, uint8 flag);
//extern uint dhd_bus_status(dhd_pub_t *dhdp);
extern int  dhd_bus_start(dhd_pub_t *dhdp);
//extern int dhd_bus_membytes(dhd_pub_t *dhdp, bool set, uint32 address, uint8 *data, uint size);
//extern void dhd_print_buf(void *pbuf, int len, int bytes_per_line);
//extern bool dhd_is_associated(dhd_pub_t *dhd, void *bss_buf);

extern int dhd_keep_alive_onoff(dhd_pub_t *dhd);

typedef enum cust_gpio_modes {
	WLAN_RESET_ON,
	WLAN_RESET_OFF,
	WLAN_POWER_ON,
	WLAN_POWER_OFF
} cust_gpio_modes_t;

/* Use interrupts */
extern uint dhd_intr;

/* Initial idletime ticks (may be -1 for immediate idle, 0 for no idle) */
extern int dhd_idletime;
#define DHD_IDLETIME_TICKS 1

/* SDIO Drive Strength */
extern uint dhd_sdiod_drive_strength;

/* Override to force tx queueing all the time */
//extern uint dhd_force_tx_queueing;
/* Default KEEP_ALIVE Period is 55 sec to prevent AP from sending Keep Alive probe frame */
#define KEEP_ALIVE_PERIOD 55000
#define NULL_PKT_STR	"null_pkt"

/* optionally set by a module_param_string() */

/* For supporting multiple interfaces */
#define DHD_MAX_IFS	1//dgl 16
#define DHD_DEL_IF	-0xe
#define DHD_BAD_IF	-0xf



/* Private data for SDIO bus interaction */

#define MAX_HDR_READ	32
typedef struct dhd_bus 
{
	dhd_pub_t	*dhd;

	bcmsdh_info_t	*sdh;			/* Handle for BCMSDH calls */
	si_t		*sih;			/* Handle for SI calls */
	char		*vars;			/* Variables (from CIS and/or other) */
	uint		varsz;			/* Size of variables buffer */
	uint32		sbaddr;			/* Current SB window pointer (-1, invalid) */

	sdpcmd_regs_t	*regs;			/* Registers for SDIO core */
	uint		sdpcmrev;		/* SDIO core revision */
	uint		armrev;			/* CPU core revision */
	uint		ramrev;			/* SOCRAM core revision */
	uint32		ramsize;		/* Size of RAM in SOCRAM (bytes) */
	uint32		orig_ramsize;		/* Size of RAM in SOCRAM (bytes) */

	uint32		bus;			/* gSPI or SDIO bus */
	uint32		hostintmask;		/* Copy of Host Interrupt Mask */
	uint32		intstatus;		/* Intstatus bits (events) pending */
	bool		dpc_sched;		/* Indicates DPC schedule (intrpt rcvd) */
	bool		fcstate;		/* State of dongle flow-control */

	uint16		cl_devid;		/* cached devid for dhdsdio_probe_attach() */
	int		    fw_path_fmsize; /* module_param: path to firmware image */
	int		    nv_path_fmsize; /* module_param: path to nvram vars file */
	const char      *nvram_params;		/* user specified nvram params. */

	uint		blocksize;		/* Block size of SDIO transfers */
	uint		roundup;		/* Max roundup limit */

	uint8		flowcontrol;		/* per prio flow control bitmask */
	uint8		tx_seq;			/* Transmit sequence number (next) */
	uint8		tx_max;			/* Maximum transmit sequence allowed */

	uint8		hdrbuf[MAX_HDR_READ + DHD_SDALIGN];
	uint8		*rxhdr;			/* Header of current rx frame (in hdrbuf) */
	uint16		nextlen;		/* Next Read Len from last header */
	uint8		rx_seq;			/* Receive sequence number (expected) */
	bool		rxskip;			/* Skip receive (awaiting NAK ACK) */

	void		*glomd;			/* Packet containing glomming descriptor */
	void		*glom;			/* Packet chain for glommed superframe */
	uint		glomerr;		/* Glom packet read errors */

	uint8		*rxbuf;			/* Buffer for receiving control packets */
	uint		rxblen;			/* Allocated length of rxbuf */
	uint8		*rxctl;			/* Aligned pointer into rxbuf */
	uint8		*databuf;		/* Buffer for receiving big glom packet */
	uint8		*dataptr;		/* Aligned pointer into databuf */
	uint		rxlen;			/* Length of valid data in buffer */

	uint8		sdpcm_ver;		/* Bus protocol reported by dongle */

	bool		intr;			/* Use interrupts */
	bool		poll;			/* Use polling */
	bool		ipend;			/* Device interrupt is pending */
	bool		intdis;			/* Interrupts disabled by isr */
	uint 		intrcount;		/* Count of device interrupt callbacks */
	uint		lastintrs;		/* Count as of last watchdog timer */
	uint		spurious;		/* Count of spurious interrupts */
	uint		pollrate;		/* Ticks between device polls */
	uint		polltick;		/* Tick counter */
	uint		pollcnt;		/* Count of active polls */


	uint		regfails;		/* Count of R_REG/W_REG failures */

	uint		clkstate;		/* State of sd and backplane clock(s) */
	bool		activity;		/* Activity flag for clock down */
	int32		idletime;		/* Control for activity timeout */
	int32		idlecount;		/* Activity timeout counter */
	int32		idleclock;		/* How to set bus driver when idle */
	int32		sd_divisor;		/* Speed control to bus driver */
	int32		sd_mode;		/* Mode control to bus driver */
	int32		sd_rxchain;		/* If bcmsdh api accepts PKT chains */
	bool		use_rxchain;		/* If dhd should use PKT chains */
	bool		sleeping;		/* Is SDIO bus sleeping? */
	bool		rxflow_mode;	/* Rx flow control mode */
	bool		rxflow;			/* Is rx flow control on */
	uint		prev_rxlim_hit;		/* Is prev rx limit exceeded (per dpc schedule) */
	bool		alp_only;		/* Don't use HT clock (ALP only) */
	/* Field to decide if rx of control frames happen in rxbuf or lb-pool */
	bool		usebufpool;

	/* Some additional counters */
	uint		tx_sderrs;		/* Count of tx attempts with sd errors */
	uint		fcqueued;		/* Tx packets that got queued */
	uint		rxrtx;			/* Count of rtx requests (NAK to dongle) */
	uint		rx_toolong;		/* Receive frames too long to receive */
	uint		rxc_errors;		/* SDIO errors when reading control frames */
	uint		rx_hdrfail;		/* SDIO errors on header reads */
	uint		rx_badhdr;		/* Bad received headers (roosync?) */
	uint		rx_badseq;		/* Mismatched rx sequence number */
	uint		fc_rcvd;		/* Number of flow-control events received */
	uint		fc_xoff;		/* Number which turned on flow-control */
	uint		fc_xon;			/* Number which turned off flow-control */
	uint		rxglomfail;		/* Failed deglom attempts */
	uint		rxglomframes;		/* Number of glom frames (superframes) */
	uint		rxglompkts;		/* Number of packets from glom frames */
	uint		f2rxhdrs;		/* Number of header reads */
	uint		f2rxdata;		/* Number of frame data reads */
	uint		f2txdata;		/* Number of f2 frame writes */
	uint		f1regdata;		/* Number of f1 register accesses */

	uint8		*ctrl_frame_buf;
	uint32		ctrl_frame_len;
	bool		ctrl_frame_stat;
	uint32		rxint_mode;	/* rx interrupt mode */
	uint8       is_get_ctrl_frame;
} dhd_bus_t;



#endif /* _dhd_h_ */
