#ifndef DHD_TXRX_PROCESS_H
#define DHD_TXRX_PROCESS_H

extern PBCMSDH_SDMMC_INSTANCE gInstance;

/******************************bcmsdh.c****************************/

extern bcmsdh_info_t * l_bcmsdh;
extern bcmsdh_info_t g_bcmsdh_info_t;
extern sdioh_info_t  g_sdioh_info_t;


/******************************bcmsdh.c end ****************************/

/******************************dhd_linux.c begin ****************************/

/* Interface control information */
typedef struct dhd_if {
	struct dhd_info *info;			/* back pointer to dhd_info */
	/* OS/stack specifics */
	struct net_device *net;
	struct net_device_stats stats;
	int 			idx;			/* iface idx in dongle */
	dhd_if_state_t	state;			/* interface state */
	uint 			subunit;		/* subunit */
	uint8			mac_addr[ETHER_ADDR_LEN];	/* assigned MAC address */
	bool			attached;		/* Delayed attachment when unset */
	bool			txflowcontrol;	/* Per interface flow control indicator */
	char			name[IFNAMSIZ+1]; /* linux interface name */
	uint8			bssidx;			/* bsscfg index for the interface */
	bool			set_multicast;
} dhd_if_t;

/* Local private structure (extension of pub) */
typedef struct dhd_info 
{
	dhd_pub_t pub;

	/* For supporting multiple interfaces */
	dhd_if_t *iflist[DHD_MAX_IFS];

	bool wd_timer_valid;
	bool dhd_tasklet_create;

	/* Wakelocks */
	int wakelock_counter;
	int wakelock_timeout_enable;

	/* Thread to issue ioctl for multicast */
	bool set_macaddress;
	struct ether_addr macvalue;
	dhd_attach_states_t dhd_state;
} dhd_info_t;


/******************************dhd_linux.c end ****************************/

/******************************dhd_sdio.c begin*************************/
#define DHD_FIRSTREAD   32

/* Total length of frame header for dongle protocol */
/* Total length of frame header for dongle protocol */
#define SDPCM_HDRLEN	(SDPCM_FRAMETAG_LEN + SDPCM_SWHEADER_LEN)
#define SDPCM_RESERVE	(SDPCM_HDRLEN + DHD_SDALIGN)
/* Space for header read, limit for data packets */


#define MAX_RX_DATASZ	2048


#define ALIGNMENT  4
#define F2SYNC	(SDIO_REQ_4BYTE | SDIO_REQ_FIXED)

extern dhd_bus_t * bcmdhd_bus;
extern dhd_pub_t	*G_dhd;

extern dhd_bus_t  g_dhd_bus_t;

extern si_info_t g_si_info_t;

extern bool dhd_alignctl;

extern bool retrydata;

extern const uint firstread;

extern const uint retry_limit;
extern bool sd1idle;
extern bool forcealign;
/* clkstate */
#define CLK_NONE	0
#define CLK_SDONLY	1
#define CLK_PENDING	2	/* Not used yet */
#define CLK_AVAIL	3

#define TXRETRIES	2	/* # of retries for tx frames */

/* To check if there's window offered */
#define DATAOK(bus) \
	(((uint8)(bus->tx_max - bus->tx_seq) > 1) && \
	(((uint8)(bus->tx_max - bus->tx_seq) & 0x80) == 0))

/* To check if there's window offered for ctrl frame */
#define TXCTLOK(bus) \
	(((uint8)(bus->tx_max - bus->tx_seq) != 0) && \
	(((uint8)(bus->tx_max - bus->tx_seq) & 0x80) == 0))
	
void MSG_SET_WPA_SH(void);
void MSG_CLR_WPA_SH(void);
int  MSG_LINK_DOWN(void);
bool DHDBUS_DOWN(void);
void  dhdsdio_isr(void *arg);
int   dhdsdio_clkctl(dhd_bus_t *bus, uint target, bool pendok);
uint  bcm_mkiovar(char *name, char *data, uint datalen, char *buf, uint len);
int32 wldev_iovar_setbuf(dhd_pub_t *pub, int8 *iovar_name,
	void *param, int32 paramlen, void *buf, int32 buflen, void * buf_sync);

void  SetScanEventOpen(int mask);
uint32 GetScanEventOpen(void);

#endif

