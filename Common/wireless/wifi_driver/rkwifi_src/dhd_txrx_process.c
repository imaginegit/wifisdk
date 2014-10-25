/**
  * This file contains the handling of TX in wlan driver.
  */
#include "sysinclude.h"
#include "def.h"
#include "netif.h"
#include "Eth.h"
#include "lwipopt.h"
#include "tcp.h"
#include "pBuf.h"
#include "rk903.h"

#define DHD_RXBOUND	5	/* Default for max rx frames in one scheduling */
#define DHD_TXBOUND	2	/* Default for max tx frames in one scheduling */

#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i

_ATTR_SYS_DATA_ int wpadebugen = 0;
static int ReTryAssocAP = 0;

/* 802.11 handle data define start..*/
#define BUF_80211_MAX	512
typedef struct _80211_buff
{
	uint   len;
	char   data[BUF_80211_MAX];
}s_80211_buff;

static struct _80211_buff _80211buf;
/* 802.11 handle data define end..*/

struct bss_descriptor   _g_connect_bss_;
struct bss_descriptor * g_connect_bss = NULL;

/* net data define start..*/
PBUF * lwip_unread = NULL;



void wl_iw_event(dhd_pub_t *pub, wl_event_msg_t *e, void* data);
int32 wl_escan_handler(const wl_event_msg_t *e, void *data);
/*
* global varibles.
*/
PBCMSDH_SDMMC_INSTANCE gInstance;
dhd_prot_t g_dhd_prot_t;
dhd_pub_t  g_dhd_pub_t;
static int MSG_WIFI_WPA_SH_OK = 0;
static uint32 ScanResultEventEnable = TRUE;

/*****************************bcmutils.c begin ***********************/
void SetScanEventOpen(int open)
{
	ScanResultEventEnable = open;
}

uint32 GetScanEventOpen(void)
{
	return ScanResultEventEnable;
}

void MSG_SET_WPA_SH(void)
{
	printf("wpa ok\n");
	MSG_WIFI_WPA_SH_OK = 1;
}

void MSG_CLR_WPA_SH(void)
{
	printf("wpa fail\n");
	MSG_WIFI_WPA_SH_OK = 0;
}

int MSG_LINK_DOWN(void)
{
	return (MSG_WIFI_WPA_SH_OK != 1);
}

/* iovar table lookup */
const bcm_iovar_t* bcm_iovar_lookup(const bcm_iovar_t *table, const char *name)
{
	const bcm_iovar_t *vi;
	const char *lookup_name;

	/* skip any ':' delimited option prefixes */
	lookup_name = strrchr(name, ':');
	if (lookup_name != NULL)
		lookup_name++;
	else
		lookup_name = name;

	ASSERT(table != NULL);

	for (vi = table; vi->name; vi++) 
	{
		if (!strcmp(vi->name, lookup_name))
			return vi;
	}
	/* ran to end of table */

	return NULL; /* var name not found */
}

int bcm_iovar_lencheck(const bcm_iovar_t *vi, void *arg, int len, bool set)
{
	int bcmerror = 0;

	/* length check on io buf */
	switch (vi->type) 
	{
	case IOVT_BOOL:
	case IOVT_INT8:
	case IOVT_INT16:
	case IOVT_INT32:
	case IOVT_UINT8:
	case IOVT_UINT16:
	case IOVT_UINT32:
		/* all integers are int32 sized args at the ioctl interface */
		if (len < (int)sizeof(int)) 
		{
			bcmerror = BCME_BUFTOOSHORT;
		}
		break;

	case IOVT_BUFFER:
		/* buffer must meet minimum length requirement */
		if (len < vi->minlen) 
		{
			bcmerror = BCME_BUFTOOSHORT;
		}
		break;

	case IOVT_VOID:
		if (!set) 
		{
			/* Cannot return nil... */
			bcmerror = BCME_UNSUPPORTED;
		} 
		else if (len) 
		{
			/* Set is an action w/o parameters */
			bcmerror = BCME_BUFTOOLONG;
		}
		break;

	default:
		/* unknown type for length check in iovar info */
		bcmerror = BCME_UNSUPPORTED;
		ASSERT(0);
	}

	return bcmerror;
}

/*****************************bcmutils.c end ***********************/

/*****************************bcmsdh_sdmmc.c begin ***********************/


uint sd_sdmode = SDIOH_MODE_SD4;	/* Use SD4 mode by default */
uint sd_divisor = 2;			/* Default 48MHz/2 = 24MHz */

uint sd_power = 1;		/* Default to SD Slot powered ON */
uint sd_clock = 1;		/* Default to SD Clock turned ON */
uint sd_hiok = FALSE;	/* Don't use hi-speed mode by default */
uint sd_msglevel = 0;
uint sd_use_dma = TRUE;

/* IOVar table */
enum 
{
	IOV_MSGLEVEL = 1,
	IOV_BLOCKMODE,
	IOV_BLOCKSIZE,
	IOV_DMA,
	IOV_USEINTS,
	IOV_NUMINTS,
	IOV_NUMLOCALINTS,
	IOV_HOSTREG,
	IOV_DEVREG,
	IOV_DIVISOR,
	IOV_SDMODE,
	IOV_HISPEED,
	IOV_HCIREGS,
	IOV_POWER,
	IOV_CLOCK,
	IOV_RXCHAIN
};


const bcm_iovar_t sdioh_iovars[] = {
	{"sd_msglevel", 	IOV_MSGLEVEL,		0,	IOVT_UINT32,	0},
	{"sd_blockmode", 	IOV_BLOCKMODE, 		0,	IOVT_BOOL,		0},
	{"sd_blocksize", 	IOV_BLOCKSIZE, 		0,	IOVT_UINT32,	0}, /* ((fn << 16) | size) */
	{"sd_dma",			IOV_DMA,			0,	IOVT_BOOL,		0},
	{"sd_ints", 		IOV_USEINTS,		0,	IOVT_BOOL,		0},
	{"sd_numints",		IOV_NUMINTS,		0,	IOVT_UINT32,	0},
	{"sd_numlocalints", IOV_NUMLOCALINTS, 	0, 	IOVT_UINT32,	0},
	{"sd_hostreg",		IOV_HOSTREG,		0,	IOVT_BUFFER,	sizeof(sdreg_t) },
	{"sd_devreg",		IOV_DEVREG, 		0,	IOVT_BUFFER,	sizeof(sdreg_t) },
	{"sd_divisor",		IOV_DIVISOR,		0,	IOVT_UINT32,	0},
	{"sd_power",		IOV_POWER,			0,	IOVT_UINT32,	0},
	{"sd_clock",		IOV_CLOCK,			0,	IOVT_UINT32,	0},
	{"sd_mode", 		IOV_SDMODE, 		0,	IOVT_UINT32,	100},
	{"sd_highspeed", 	IOV_HISPEED,		0,	IOVT_UINT32,	0},
	{"sd_rxchain",  	IOV_RXCHAIN,    	0, 	IOVT_BOOL,		0},
	{NULL, 0, 0, 0, 0 }
};

static int sdioh_iovar_op(sdioh_info_t *si, const char *name,
                           void *params, int plen, void *arg, int len, bool set)
{
	const bcm_iovar_t *vi = NULL;
	int bcmerror = 0;
	int val_size;
	int32 int_val = 0;
	uint32 actionid;

	ASSERT(name);
	ASSERT(len >= 0);

	/* Get must have return space; Set does not take qualifiers */
	ASSERT(set || (arg && len));
	ASSERT(!set || (!params && !plen));

	if ((vi = bcm_iovar_lookup(sdioh_iovars, name)) == NULL) 
	{
		bcmerror = BCME_UNSUPPORTED;
		goto exit;
	}

	if ((bcmerror = bcm_iovar_lencheck(vi, arg, len, set)) != 0)
		goto exit;

	/* Set up params so get and set can share the convenience variables */
	if (params == NULL) 
	{
		params = arg;
		plen = len;
	}

	if (vi->type == IOVT_VOID)
		val_size = 0;
	else if (vi->type == IOVT_BUFFER)
		val_size = len;
	else
		val_size = sizeof(int);

	if (plen >= (int)sizeof(int_val))
		bcopy(params, &int_val, sizeof(int_val));

	actionid = set ? IOV_SVAL(vi->varid) : IOV_GVAL(vi->varid);
	switch (actionid) 
	{
	case IOV_GVAL(IOV_MSGLEVEL):
		int_val = (int32)sd_msglevel;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_MSGLEVEL):
		sd_msglevel = int_val;
		break;

	case IOV_GVAL(IOV_BLOCKMODE):
		int_val = (int32)si->sd_blockmode;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_BLOCKMODE):
		si->sd_blockmode = (bool)int_val;
		/* Haven't figured out how to make non-block mode with DMA */
		break;

	case IOV_GVAL(IOV_BLOCKSIZE):
		if ((uint32)int_val > si->num_funcs) 
		{
			bcmerror = BCME_BADARG;
			break;
		}
		int_val = (int32)si->client_block_size[int_val];
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_BLOCKSIZE):
	{
		uint func = ((uint32)int_val >> 16);
		uint blksize = (uint16)int_val;
		uint maxsize;

		if (func > si->num_funcs) 
		{
			bcmerror = BCME_BADARG;
			break;
		}

		switch (func) 
		{
			case 0: maxsize = 32; break;
			case 1: maxsize = BLOCK_SIZE_4318; break;
			case 2: maxsize = BLOCK_SIZE_4328; break;
			default: maxsize = 0;
		}
		if (blksize > maxsize) 
		{
			bcmerror = BCME_BADARG;
			break;
		}
		if (!blksize) 
		{
			blksize = maxsize;
		}

		/* Now set it */
		si->client_block_size[func] = blksize;
		break;
	}

	case IOV_GVAL(IOV_RXCHAIN):
		int_val = FALSE;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_GVAL(IOV_DMA):
		int_val = (int32)si->sd_use_dma;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_DMA):
		si->sd_use_dma = (bool)int_val;
		break;

	case IOV_GVAL(IOV_USEINTS):
		int_val = (int32)si->use_client_ints;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_USEINTS):
		si->use_client_ints = (bool)int_val;
		if (si->use_client_ints)
			si->intmask |= CLIENT_INTR;
		else
			si->intmask &= ~CLIENT_INTR;

		break;

	case IOV_GVAL(IOV_DIVISOR):
		int_val = (uint32)sd_divisor;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_DIVISOR):
		sd_divisor = int_val;
		break;

	case IOV_GVAL(IOV_POWER):
		int_val = (uint32)sd_power;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_POWER):
		sd_power = int_val;
		break;

	case IOV_GVAL(IOV_CLOCK):
		int_val = (uint32)sd_clock;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_CLOCK):
		sd_clock = int_val;
		break;

	case IOV_GVAL(IOV_SDMODE):
		int_val = (uint32)sd_sdmode;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_SDMODE):
		sd_sdmode = int_val;
		break;

	case IOV_GVAL(IOV_HISPEED):
		int_val = (uint32)sd_hiok;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_SVAL(IOV_HISPEED):
		sd_hiok = int_val;
		break;

	case IOV_GVAL(IOV_NUMINTS):
		int_val = (int32)si->intrcount;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_GVAL(IOV_NUMLOCALINTS):
		int_val = (int32)0;
		bcopy(&int_val, arg, val_size);
		break;

	case IOV_GVAL(IOV_HOSTREG):
	{
		sdreg_t *sd_ptr = (sdreg_t *)params;

		if (sd_ptr->offset < SD_SysAddr || sd_ptr->offset > SD_MaxCurCap) 
		{
			bcmerror = BCME_BADARG;
			break;
		}

		if (sd_ptr->offset & 1)
			int_val = 8; /* sdioh_sdmmc_rreg8(si, sd_ptr->offset); */
		else if (sd_ptr->offset & 2)
			int_val = 16; /* sdioh_sdmmc_rreg16(si, sd_ptr->offset); */
		else
			int_val = 32; /* sdioh_sdmmc_rreg(si, sd_ptr->offset); */

		bcopy(&int_val, arg, sizeof(int_val));
		break;
	}

	case IOV_SVAL(IOV_HOSTREG):
	{
		sdreg_t *sd_ptr = (sdreg_t *)params;

		if (sd_ptr->offset < SD_SysAddr || sd_ptr->offset > SD_MaxCurCap) 
		{
			bcmerror = BCME_BADARG;
			break;
		}
		break;
	}

	case IOV_GVAL(IOV_DEVREG):
	{
		sdreg_t *sd_ptr = (sdreg_t *)params;
		uint8 data = 0;

		if (sdioh_cfg_read(si, sd_ptr->func, sd_ptr->offset, &data)) 
		{
			bcmerror = BCME_SDIO_ERROR;
			break;
		}

		int_val = (int)data;
		bcopy(&int_val, arg, sizeof(int_val));
		break;
	}

	case IOV_SVAL(IOV_DEVREG):
	{
		sdreg_t *sd_ptr = (sdreg_t *)params;
		uint8 data = (uint8)sd_ptr->value;

		if (sdioh_cfg_write(si, sd_ptr->func, sd_ptr->offset, &data)) 
		{
			bcmerror = BCME_SDIO_ERROR;
			break;
		}
		break;
	}

	default:
		bcmerror = BCME_UNSUPPORTED;
		break;
	}
exit:

	return bcmerror;
}

extern SDIOH_API_RC sdioh_cfg_read(sdioh_info_t *sd, uint fnc_num, uint32 addr, uint8 *data)
{
	SDIOH_API_RC status;
	/* No lock needed since sdioh_request_byte does locking */
	status = sdioh_request_byte(sd, SDIOH_READ, fnc_num, addr, data);
	return status;
}

extern SDIOH_API_RC sdioh_cfg_write(sdioh_info_t *sd, uint fnc_num, uint32 addr, uint8 *data)
{
	/* No lock needed since sdioh_request_byte does locking */
	SDIOH_API_RC status;
	status = sdioh_request_byte(sd, SDIOH_WRITE, fnc_num, addr, data);
	return status;
}

/* this function performs "abort" for both of host & device */
extern int sdioh_abort(sdioh_info_t *sd, uint func)
{
	uint8 t_func = (uint8) func;

	/* issue abort cmd52 command through F1 */    
	sdioh_request_byte(sd, SD_IO_OP_WRITE, SDIO_FUN_0, SDIOD_CCCR_IOABORT, &t_func);

	return SDIOH_API_RC_SUCCESS;
}


extern SDIOH_API_RC sdioh_request_byte(sdioh_info_t *sd, uint rw, uint func, uint regaddr, uint8 *byte)
{
	int err_ret;

	/* CMD52 Write */
	if(rw) 
	{
		if (func == 0) 
		{
			/* Can only directly write to some F0 registers.  Handle F2 enable
			 * as a special case.
			 */
			if (regaddr == SDIO_CCCR_IOEx) 
			{
				if (gInstance->func[2]) 
				{
					if (*byte & SDIO_FUNC_ENABLE_2) 
					{
						/* Enable Function 2 */
						err_ret = sdio_enable_func(gInstance->func[2]);
					} 
					else 
					{
						/* Disable Function 2 */
						err_ret = sdio_disable_func(gInstance->func[2]);
					}
				}
			}
			/* to allow abort command through F1 */
			else if (regaddr == SDIO_CCCR_ABORT) 
			{
				/*
				* this sdio_f0_writeb() can be replaced with another api
				* depending upon MMC driver change.
				* As of this time, this is temporaray one
				*/
                sdio_writeb(gInstance->func[func], *byte, regaddr, &err_ret);
			}
			else if (regaddr < 0xF0) 
			{
				BCMDEBUG("bcmsdh_sdmmc: F0 Wr:0x%02x: write disallowed\n", regaddr);
			} 
			else 
			{
				/* Claim host controller, perform F0 write, and release */
				sdio_f0_writeb(gInstance->func[func], *byte, regaddr, &err_ret);
			}
		} 
		else 
		{
			/* Claim host controller, perform Fn write, and release */
			sdio_writeb(gInstance->func[func], *byte, regaddr, &err_ret);
		}
	} 
	else 
	{ 
		/* CMD52 Read */
		if (func == 0) 
		{
			*byte = sdio_f0_readb(gInstance->func[func], regaddr, &err_ret);
		} 
		else 
		{
			*byte = sdio_readb(gInstance->func[func], regaddr, &err_ret);
		}
	}

	return ((err_ret == 0) ? SDIOH_API_RC_SUCCESS : SDIOH_API_RC_FAIL);
}


extern SDIOH_API_RC sdioh_request_word(sdioh_info_t *sd, uint cmd_type, uint rw, uint func, uint addr,
                                                 uint32 *word, uint nbytes)
{
	int err_ret = SDIOH_API_RC_FAIL;

	if (func == 0) 		
		return SDIOH_API_RC_FAIL;

	if(rw) 
	{ 
		/* CMD52 Write */
		if (nbytes == 4) 
		{
			sdio_writel(gInstance->func[func], *word, addr, &err_ret);
		} 
		else if (nbytes == 2)
		{
			sdio_writew(gInstance->func[func], (*word & 0xFFFF), addr, &err_ret);
		} 
		else 
		{
			BCMDEBUG("Invalid nbytes: %d\n", nbytes);
		}
	} 
	else 
	{ 
		/* CMD52 Read */
		if (nbytes == 4) 
		{
			*word = sdio_readl(gInstance->func[func], addr, &err_ret);
		} 
		else if (nbytes == 2) 
		{
			*word = sdio_readw(gInstance->func[func], addr, &err_ret) & 0xFFFF;
		} 
		else 
		{
			BCMDEBUG("Invalid nbytes: %d\n", nbytes);
		}
	}

	return ((err_ret == 0) ? SDIOH_API_RC_SUCCESS : SDIOH_API_RC_FAIL);
}

    
static SDIOH_API_RC
sdioh_request_packet(sdioh_info_t *sd, uint fix_inc, uint write, uint func, uint addr, void *pkt)
{
	bool fifo = (fix_inc == SDIOH_DATA_FIX);
	int err_ret = 0;
	uint pkt_len;

	ASSERT(pkt);
    
	pkt_len = PKTLEN(pkt);

	pkt_len += 3;
	pkt_len &= 0xFFFFFFFC;

	/* Make sure the packet is aligned properly. If it isn't, then this
	 * is the fault of sdioh_request_buffer() which is supposed to give
	 * us something we can work with.
	 */
	if ((write) && (!fifo)) 
	{
		err_ret = sdio_memcpy_toio(gInstance->func[func], addr, PKTDATA(pkt), pkt_len);
	} 
	else if (write) 
	{
		err_ret = sdio_writesb(gInstance->func[func], addr, PKTDATA(pkt), pkt_len);//fifo W
	} 
	else if (fifo) 
	{
		err_ret = sdio_readsb(gInstance->func[func], PKTDATA(pkt), addr, pkt_len);//fifo R
	} 
	else 
	{
		err_ret = sdio_memcpy_fromio(gInstance->func[func], PKTDATA(pkt), addr, pkt_len);
	}

	if (!fifo) 
	{
		addr += pkt_len;
	}

	return ((err_ret == 0) ? SDIOH_API_RC_SUCCESS : SDIOH_API_RC_FAIL);
}

/*
 * This function takes a buffer or packet, and fixes everything up so that in the
 * end, a DMA-able packet is created.
 *
 * A buffer does not have an associated packet pointer, and may or may not be aligned.
 * A packet may consist of a single packet, or a packet chain.  If it is a packet chain,
 * then all the packets in the chain must be properly aligned.  If the packet data is not
 * aligned, then there may only be one packet, and in this case, it is copied to a new
 * aligned packet.
 *
 */
extern SDIOH_API_RC
sdioh_request_buffer(sdioh_info_t *sd, uint pio_dma, uint fix_inc, uint write, uint func,
                     uint addr, uint reg_width, uint buflen_u, uint8 *buffer)
{
	SDIOH_API_RC Status;
    sk_buff  mypkt;

    mypkt.len = buflen_u;
    mypkt.data = buffer;
	Status = sdioh_request_packet(sd, fix_inc, write, func, addr, &mypkt);

	return Status;
}


/* bcmsdh_sdmmc interrupt handler */
void IRQHandler(struct sdio_func *func)
{
	sdioh_info_t *sd;

	sd = gInstance->sd;

	ASSERT(sd != NULL);

	if (sd->use_client_ints) 
	{
		sd->intrcount++;
		ASSERT(sd->intr_handler);
		ASSERT(sd->intr_handler_arg);
		(sd->intr_handler)(sd->intr_handler_arg);//dhdsdio_isr
	}
}

/* bcmsdh_sdmmc interrupt handler for F2 (dummy handler) */
void IRQHandlerF2(struct sdio_func *func)
{
	sdioh_info_t *sd;

	sd = gInstance->sd;

	ASSERT(sd != NULL);
}

/*****************************bcmsdh_sdmmc.c end ***********************/

/******************************bcmsdh.c begin****************************/

bcmsdh_info_t * l_bcmsdh = NULL;
bcmsdh_info_t g_bcmsdh_info_t;
sdioh_info_t  g_sdioh_info_t;



int bcmsdh_iovar_op(void *sdh, const char *name, void *params, int plen, void *arg, int len, bool set)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	return sdioh_iovar_op(bcmsdh->sdioh, name, params, plen, arg, len, set);
}

int bcmsdhsdio_set_sbaddr_window(void *sdh, uint32 address, bool force_set)
{
	int err = 0;
	uint bar0 = address & ~SBSDIO_SB_OFT_ADDR_MASK;
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;

	if (bar0 != bcmsdh->sbwad || force_set) 
	{
		bcmsdh_cfg_write(bcmsdh, SDIO_FUN_1, SBSDIO_FUNC1_SBADDRLOW,
			                    (address >> 8) & SBSDIO_SBADDRLOW_MASK, &err);
		if (!err)
			bcmsdh_cfg_write(bcmsdh, SDIO_FUN_1, SBSDIO_FUNC1_SBADDRMID,
				                 (address >> 16) & SBSDIO_SBADDRMID_MASK, &err);
		if (!err)
			bcmsdh_cfg_write(bcmsdh, SDIO_FUN_1, SBSDIO_FUNC1_SBADDRHIGH,
				                 (address >> 24) & SBSDIO_SBADDRHIGH_MASK, &err);

		if (!err)
			bcmsdh->sbwad = bar0;
		else
			bcmsdh->sbwad = 0;/* invalidate cached window var */
	}

	return err;
}

bool bcmsdh_regfail(void *sdh)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;

	if (!bcmsdh)
		bcmsdh = l_bcmsdh;

	return bcmsdh->regfail;
}
/**
 * Read from SDIO Configuration Space
 * @param sdh SDIO Host context.
 * @param func_num Function number to read from.
 * @param addr Address to read from.
 * @param err Error return.
 * @return value read from SDIO configuration space.
 */
uint8 bcmsdh_cfg_read(void *sdh, uint fnc_num, uint32 addr, int *err)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;
	int32 retry = 0;
	uint8 data = 0;

	if (!bcmsdh)
		bcmsdh = l_bcmsdh;

	ASSERT(bcmsdh->init_success);

	do 
	{
		if (retry)	/* wait for 1 ms till bus get settled down */
			DelayUs(1000);
		status = sdioh_cfg_read(bcmsdh->sdioh, fnc_num, addr, (uint8 *)&data);
	} while (!SDIOH_API_SUCCESS(status) && (retry++ < SDIOH_API_ACCESS_RETRY_LIMIT));
	
	if (err)
		*err = (SDIOH_API_SUCCESS(status) ? 0 : BCME_SDIO_ERROR);

	return data;
}

void bcmsdh_cfg_write(void *sdh, uint fnc_num, uint32 addr, uint8 data, int *err)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;
	int32 retry = 0;

	if (!bcmsdh)
		bcmsdh = l_bcmsdh;

	ASSERT(bcmsdh->init_success);

	do 
	{
		/* wait for 1 ms till bus get settled down */
		if (retry)	
			DelayUs(1000);
		status = sdioh_cfg_write(bcmsdh->sdioh, fnc_num, addr, (uint8 *)&data);
	} while (!SDIOH_API_SUCCESS(status) && (retry++ < SDIOH_API_ACCESS_RETRY_LIMIT));
	
	if (err)
	{
		*err = SDIOH_API_SUCCESS(status) ? 0 : BCME_SDIO_ERROR;
	}

	return;
}

int bcmsdh_recv_buf(void *sdh, uint32 addr, uint fn, uint flags, uint8 *buf, uint nbytes)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;
	uint incr_fix;
	uint width;
	int err = 0;

	if (flags & SDIO_REQ_ASYNC)
		return BCME_UNSUPPORTED;

   	err = bcmsdhsdio_set_sbaddr_window(bcmsdh, addr, FALSE);
	if (err)
	{   
		return err;
	}
	
	addr &= SBSDIO_SB_OFT_ADDR_MASK;
	incr_fix = (flags & SDIO_REQ_FIXED) ? SDIOH_DATA_FIX : SDIOH_DATA_INC;
	width = (flags & SDIO_REQ_4BYTE) ? 4 : 2;
	if (width == 4)
		addr |= SBSDIO_SB_ACCESS_2_4B_FLAG;

	status = sdioh_request_buffer(bcmsdh->sdioh, SDIOH_DATA_PIO, incr_fix,
	                              SDIOH_READ, fn, addr, width, nbytes, buf);

	return (SDIOH_API_SUCCESS(status) ? 0 : BCME_SDIO_ERROR);
}

int bcmsdh_send_buf(void  *sdh, uint32 addr, uint fn, uint flags, uint8 *buf, uint nbytes)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;
	uint incr_fix;
	uint width;
	int err = 0;

	/* Async not implemented yet */
	if (flags & SDIO_REQ_ASYNC)
		return BCME_UNSUPPORTED;

	err = bcmsdhsdio_set_sbaddr_window(bcmsdh, addr, FALSE);
	if (err)
		return err;

	addr &= SBSDIO_SB_OFT_ADDR_MASK;

	incr_fix = (flags & SDIO_REQ_FIXED) ? SDIOH_DATA_FIX : SDIOH_DATA_INC;
	width = (flags & SDIO_REQ_4BYTE) ? 4 : 2;
	if (width == 4)
		addr |= SBSDIO_SB_ACCESS_2_4B_FLAG;

	status = sdioh_request_buffer(bcmsdh->sdioh, SDIOH_DATA_PIO, incr_fix,
	                              SDIOH_WRITE, fn, addr, width, nbytes, buf);

	return (SDIOH_API_SUCCESS(status) ? 0 : BCME_ERROR);
}

int bcmsdh_abort(void *sdh, uint fn)
{
    #if 1
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;

	return sdioh_abort(bcmsdh->sdioh, fn);
    #else
    return 0;
    #endif
        
}

uint32 bcmsdh_cur_sbwad(void *sdh)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;

	if (!bcmsdh)
		bcmsdh = l_bcmsdh;

	return (bcmsdh->sbwad);
}

uint32 bcmsdh_reg_read(void *sdh, uint32 addr, uint size)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;
	uint32 word = 0;

	if (!bcmsdh)
		bcmsdh = l_bcmsdh;

	ASSERT(bcmsdh->init_success);

	if (bcmsdhsdio_set_sbaddr_window(bcmsdh, addr, FALSE))
		return 0xFFFFFFFF;

	addr &= SBSDIO_SB_OFT_ADDR_MASK;
	if (size == 4)
		addr |= SBSDIO_SB_ACCESS_2_4B_FLAG;

	status = sdioh_request_word(bcmsdh->sdioh, SDIOH_CMD_TYPE_NORMAL,
		                        SDIOH_READ, SDIO_FUN_1, addr, &word, size);

	bcmsdh->regfail = !(SDIOH_API_SUCCESS(status));

	/* if ok, return appropriately masked word */
	if (SDIOH_API_SUCCESS(status)) 
	{
		switch (size) 
		{
			case sizeof(uint8):
				return (word & 0xff);
				
			case sizeof(uint16):
				return (word & 0xffff);
				
			case sizeof(uint32):
				return word;
				
			default:
				bcmsdh->regfail = TRUE;
		}
	}

	/* otherwise, bad sdio access or invalid size */
	return 0xFFFFFFFF;
}


uint32 bcmsdh_reg_write(void *sdh, uint32 addr, uint size, uint32 data)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;
	int err = 0;

	if (!bcmsdh)
		bcmsdh = l_bcmsdh;

	ASSERT(bcmsdh->init_success);

	err = bcmsdhsdio_set_sbaddr_window(bcmsdh, addr, FALSE);
	if (err)
		return err;

	addr &= SBSDIO_SB_OFT_ADDR_MASK;
	if (size == 4)
		addr |= SBSDIO_SB_ACCESS_2_4B_FLAG;
	status = sdioh_request_word(bcmsdh->sdioh, SDIOH_CMD_TYPE_NORMAL, SDIOH_WRITE, SDIO_FUN_1,
	                            addr, &data, size);
	bcmsdh->regfail = !(SDIOH_API_SUCCESS(status));

	if (SDIOH_API_SUCCESS(status))
		return 0;

	return 0xFFFFFFFF;
}


/******************************bcmsdh.c end****************************/


/******************************dhd_common.c being ****************************/
int wl_host_event(dhd_pub_t *dhd_pub, int *ifidx, void *pktdata,
                     wl_event_msg_t *event, void **data_ptr)
{
	bcm_event_t *pvt_data = (bcm_event_t *)pktdata;
	uint8 *event_data;
	uint32 type, status, reason, datalen;
	uint16 flags;
	int evlen;

	if (bcmp(BRCM_OUI, (const void *)&pvt_data->bcm_hdr.oui[0], DOT11_OUI_LEN)) 
	{
		BCMDEBUG("mismatched OUI, bailing\n");
		return (BCME_ERROR);
	}

	/* BRCM event pkt may be unaligned - use xxx_ua to load user_subtype. */
	if (ntoh16_ua((void *)&pvt_data->bcm_hdr.usr_subtype) != BCMILCP_BCM_SUBTYPE_EVENT) 
	{
		BCMDEBUG("mismatched subtype, bailing\n");
		return (BCME_ERROR);
	}

	*data_ptr = &pvt_data[1];
	event_data = *data_ptr;

	/* memcpy since BRCM event pkt may be unaligned. */
	memcpy(event, &pvt_data->event, sizeof(wl_event_msg_t));

	type = ntoh32_ua((void *)&event->event_type);
	flags = ntoh16_ua((void *)&event->flags);
	status = ntoh32_ua((void *)&event->status);
	reason = ntoh32_ua((void *)&event->reason);
	datalen = ntoh32_ua((void *)&event->datalen);
	evlen = datalen + sizeof(bcm_event_t);

	switch (type) 
	{
		case WLC_E_IF://54
	    {   
			dhd_if_event_t *ifevent = (dhd_if_event_t *)event_data;
			
	        dhd_pub->ifIdx = ifevent->ifidx;
	        break;  
		}
		
		case WLC_E_NDIS_LINK://18
	    {
			uint32 temp;

			temp = ntoh32(WLC_E_NDIS_LINK);
			memcpy((void *)(&pvt_data->event.event_type), &temp, sizeof(pvt_data->event.event_type));
			break;
		}
		
		case WLC_E_DISASSOC_IND://12
		{
			ReTryAssocAP = 1;
			break;
		}
		
		default:
		{
			break;
		}
	}
	return (BCME_OK);
}

static int dhd_wl_host_event(dhd_pub_t *dhdp, int *ifidx, void *pktdata, wl_event_msg_t *event, void **data)
{
	int bcmerror = 0;

	bcmerror = wl_host_event(dhdp, ifidx, pktdata, event, data);
	if (bcmerror != BCME_OK)
	{
		return bcmerror;
	}

	if (event->bsscfgidx == 0) 
	{
		/*
		 * Wireless ext is on primary interface only
		 */
		wl_iw_event(dhdp, event, *data);
	}
	return (bcmerror);
}


void lwip_data_input(uint *data, uint len)
{
    PBUF  *__pstPbuf = NULL;
    PBUF  * unRead = NULL;

	#if 0
	if (arp_packet_rev_flag() == FALSE)
	{
		if (htons(((ETH_HDR *)data)->type) == ETHTYPE_ARP)
			return;
	}
	#endif

	#ifdef DBUGPBUF
	printf("@:");
	#endif
    __pstPbuf = pbuf_alloc(PBUF_PHY, PHY_FRAME_MAX_SIZE, PBUF_POOL);
	if (__pstPbuf == NULL)
	{
		return;
	}
	
    memcpy(__pstPbuf->payload, data, len);
     __pstPbuf->len = len;//Get wireless packet: len
    if (__pstPbuf->len == 0)
    {
       pbuf_free(__pstPbuf);
       return;
    }

    if (lwip_unread == NULL)
    {
        lwip_unread = __pstPbuf;
        __pstPbuf->next = NULL;
    }
    else
    {   
        unRead = lwip_unread;
        while (unRead->next)
        {
            unRead = unRead->next;
        } 
        unRead->next = __pstPbuf;
        __pstPbuf->next = NULL;
    }
}

void lwip_data_input_init(void)
{
   	lwip_unread = NULL;
	return;
}

/***/
void rk_80211_handle_init(void)
{
	_80211buf.len = 0;
}

int rk_80211_data_input(void *data, uint len)
{
	//printf("old.len=%d :%d\n", _80211buf.len, len);
	
	if (data)
	{
		memcpy(_80211buf.data, data, len);
	}
	_80211buf.len = len;
	
	return;
}

void rk_80211_data_handle(void)
{    
	int rk80211datalen;
	
	if (_80211buf.len <= 0)
		return;
	
	UserIsrDisable();
    ModuleOverlay(MODULE_ID_WIFI_WPA, MODULE_OVERLAY_CODE);

	//printf("802 handle=%d\n", _80211buf.len);

	rk80211datalen = _80211buf.len;
	_80211buf.len = 0;
    
    rk_wpa_auth_data_input(_80211buf.data, rk80211datalen);

	ModuleOverlay(MODULE_ID_WIFI_LWIP, MODULE_OVERLAY_CODE);   
	UserIsrEnable(1);

	return;
}

void dhd_rx_frame(dhd_pub_t *dhdp, int ifidx, void *pktbuf, int numpkt, uint8 chan)
{
    void *data;
    wl_event_msg_t event;
    int tout = DHD_PACKET_TIMEOUT_MS;
    struct ether_header *eh;
    struct dot11_llc_snap_header *lsh;

    eh = (struct ether_header *)PKTDATA(pktbuf);
    lsh = (struct dot11_llc_snap_header *)&eh[1];

    if ((ntoh16(eh->ether_type) < ETHER_TYPE_MIN) && (PKTLEN(pktbuf) >= RFC1042_HDR_LEN) &&
        (bcmp(lsh, BT_SIG_SNAP_MPROT, DOT11_LLC_SNAP_HDR_LEN - 2) == 0) && (lsh->type == HTON16(BTA_PROT_L2CAP)))
 	{
    	amp_hci_ACL_data_t *ACL_data = (amp_hci_ACL_data_t *)((uint8 *)eh + RFC1042_HDR_LEN);
    	ACL_data = NULL;
    }

    /* Get the protocol, maintain skb around eth_type_trans()
     * The main reason for this hack is for the limitation of
     * Linux 2.4 where 'eth_type_trans' uses the 'net->hard_header_len'
     * to perform skb_pull inside vs ETH_HLEN. Since to avoid
     * coping of the packet coming from the network stack to add
     * BDC, Hardware header etc, during network interface registration
     * we set the 'net->hard_header_len' to ETH_HLEN + extra space required
     * for BDC, Hardware header etc. and not just the ETH_HLEN
     */

	/* Strip header, count, deliver upward */
    /* Process special event packets and then discard them */
    if (ntoh16(eh->ether_type) == ETHER_TYPE_BRCM) 
	{
    	dhd_wl_host_event(dhdp, &ifidx, PKTDATA(pktbuf), &event, &data);
    }
    else
    {
        if ((ntoh16(eh->ether_type) == ETHER_TYPE_IP) || (ntoh16(eh->ether_type) == ETHER_TYPE_ARP))
        {
			if (MSG_LINK_DOWN())
			{
				return;
			}
            lwip_data_input(PKTDATA(pktbuf), PKTLEN(pktbuf));
        }
        else if (ntoh16(eh->ether_type) == ETHER_TYPE_802_1X)
        {
        	#ifdef WPA_AUTH
			rk_80211_data_input(PKTDATA(pktbuf), PKTLEN(pktbuf));
        	#endif
        }
		else
		{
			printf("err ft=%d\n", ntoh16(eh->ether_type));
		}
    }							  
    dhdp->rx_packets++; /* Local count */
	
	return;
}


/******************************dhd_linux.c end ****************************/



/******************************dhd_cdc.c begin ****************************/
void dhd_prot_hdrpush(dhd_pub_t *dhd, int ifidx, void *pktbuf)
{
	struct bdc_header *h;

	/* Push BDC header used to convey priority for buses that don't */
	PKTPUSH(pktbuf, BDC_HEADER_LEN);

	h = (struct bdc_header *)PKTDATA(pktbuf);
	h->flags = (BDC_PROTO_VER << BDC_FLAG_VER_SHIFT);
	h->flags2 = 0;
	h->dataOffset = 0;
	BDC_SET_IF_IDX(h, ifidx);
}

int dhd_prot_hdrpull(dhd_pub_t *dhd, int *ifidx, void *pktbuf)
{
	struct bdc_header *h;

	/* Pop BDC header used to convey priority for buses that don't */
	if (PKTLEN(pktbuf) < BDC_HEADER_LEN) 
	{
		BCMDEBUG(" rx data too short (%d < %d)\n",PKTLEN(pktbuf), BDC_HEADER_LEN);
		return BCME_ERROR;
	}

	h = (struct bdc_header *)PKTDATA(pktbuf);
	if ((*ifidx = BDC_GET_IF_IDX(h)) >= DHD_MAX_IFS) 
	{
		printf("rx data ifnum out of range (%d)\n", *ifidx);
		return BCME_ERROR;
	}

	if (((h->flags & BDC_FLAG_VER_MASK) >> BDC_FLAG_VER_SHIFT) != BDC_PROTO_VER) 
	{
		BCMDEBUG(" non-BDC packet received, flags = 0x%x\n", h->flags);
		if (((h->flags & BDC_FLAG_VER_MASK) >> BDC_FLAG_VER_SHIFT) == BDC_PROTO_VER_1)
			h->dataOffset = 0;
		else
			return BCME_ERROR;
	}

	if (h->flags & BDC_FLAG_SUM_GOOD) 
	{
		BCMDEBUG("BDC packet received with good rx-csum, flags 0x%x\n", h->flags);
	}

	PKTPULL(pktbuf, BDC_HEADER_LEN);

	if (PKTLEN(pktbuf) < (uint32)(h->dataOffset << 2)) 
	{
		BCMDEBUG("rx data too short (%d < %d)\n",PKTLEN(pktbuf), (h->dataOffset * 4));
		return BCME_ERROR;
	}

	PKTPULL( pktbuf, (h->dataOffset << 2));
	return 0;
}


/******************************dhd_cdc.c end ****************************/


/******************************sdio.c  begin *******************************/
dhd_bus_t g_dhd_bus_t;
dhd_bus_t * bcmdhd_bus;
dhd_pub_t * G_dhd;
si_info_t g_si_info_t;
bool dhd_alignctl;
bool retrydata;
bool sd1idle;

#define RETRYCHAN(chan) (((chan) == SDPCM_EVENT_CHANNEL) || retrydata)
const uint firstread = DHD_FIRSTREAD;

/* Retry count for register access failures */
const uint retry_limit = 2;

#define HDATLEN (firstread - (SDPCM_HDRLEN))
/* Force even SD lengths (some host controllers mess up on odd bytes) */
bool forcealign;

#define PKTALIGN(p, len, align)		\
	do {								\
		uint datalign;						\
		uint *datalignP;                    \
		datalignP = PKTDATA((p));           \
		datalign = (uint)datalignP;		    \
		datalign = ROUNDUP(datalign, (align)) - datalign;	\
		ASSERT(datalign < (align));				\
		ASSERT(PKTLEN((p)) >= ((len) + datalign));	\
		if (datalign)						\
			PKTPULL((p), datalign);			\
		PKTSETLEN((p), (len));				\
	} while (0)


/* Macros to get register read/write status */
/* NOTE: these assume a local dhdsdio_bus_t *bus! */
#define R_SDREG(regvar, regaddr, retryvar) \
do { \
	retryvar = 0; \
	do { \
		regvar = R_REG(regaddr); \
	} while (bcmsdh_regfail(bcmdhd_bus->sdh) && (++retryvar <= retry_limit)); \
	if (retryvar) { \
		bcmdhd_bus->regfails += (retryvar-1); \
		if (retryvar > retry_limit) { \
			regvar = 0; \
		} \
	} \
} while (0)

#define W_SDREG(regval, regaddr, retryvar) \
do { \
	retryvar = 0; \
	do { \
		W_REG(regaddr, regval); \
	} while (bcmsdh_regfail(bcmdhd_bus->sdh) && (++retryvar <= retry_limit)); \
	if (retryvar) { \
		bcmdhd_bus->regfails += (retryvar-1); \
	} \
} while (0)


#define SDIO_DEVICE_HMB_RXINT		0	/* default old way */
#define SDIO_DEVICE_RXDATAINT_MODE_0	1	/* from sdiod rev 4 */
#define SDIO_DEVICE_RXDATAINT_MODE_1	2	/* from sdiod rev 4 */

//dgl we select I_HMB_FRAME_IND..
#define FRAME_AVAIL_MASK(bus) 	\
	((bus->rxint_mode == SDIO_DEVICE_HMB_RXINT) ? I_HMB_FRAME_IND : I_XMTDATA_AVAIL)

#define PKT_AVAILABLE(bus, intstatus)	((intstatus) & (FRAME_AVAIL_MASK(bus)))

bool DHDBUS_DOWN(void)
{
	return (G_dhd->busstate == DHD_BUS_DOWN);
}

/* Turn backplane clock on or off */
static int dhdsdio_htclk(dhd_bus_t *bus, bool on, bool pendok)
{
	int err;
	uint8 clkctl, clkreq, devctl;
	bcmsdh_info_t *sdh;

	clkctl = 0;
	sdh = bus->sdh;

	if (on) 
	{
		/* Request HT Avail */
		clkreq = bus->alp_only ? SBSDIO_ALP_AVAIL_REQ : SBSDIO_HT_AVAIL_REQ;
		bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, clkreq, &err);
		if (err) 
			return BCME_ERROR;

		/* Check current status */
		clkctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, &err);
		if (err) 
			return BCME_ERROR;

		/* Go to pending and await interrupt if appropriate */
		if (!SBSDIO_CLKAV(clkctl, bus->alp_only) && pendok) 
		{
			/* Allow only clock-available interrupt */
			devctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, &err);
			if (err) 
				return BCME_ERROR;

			devctl |= SBSDIO_DEVCTL_CA_INT_ONLY;
			bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, devctl, &err);
			bus->clkstate = CLK_PENDING;
			printf("dhdsdio_htclk bus->clkstate = CLK_PENDING\n");
			return BCME_OK;
		} 
		else if (bus->clkstate == CLK_PENDING) 
		{
			/* Cancel CA-only interrupt filter */
			devctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, &err);
			devctl &= ~SBSDIO_DEVCTL_CA_INT_ONLY;
			bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, devctl, &err);
		}

		/* Otherwise, wait here (polling) for HT Avail */
		if (!SBSDIO_CLKAV(clkctl, bus->alp_only)) 
		{
			SPINWAIT(((clkctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, &err)),
			          !SBSDIO_CLKAV(clkctl, bus->alp_only)), PMU_MAX_TRANSITION_DLY);
		}
		if (err) 
			return BCME_ERROR;
		
		if (!SBSDIO_CLKAV(clkctl, bus->alp_only)) 
			return BCME_ERROR;


		/* Mark clock available */
		bus->clkstate = CLK_AVAIL;
		bus->activity = TRUE;
	} 
	else 
	{
		clkreq = 0;

		if (bus->clkstate == CLK_PENDING) 
		{
			/* Cancel CA-only interrupt filter */
			devctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, &err);
			devctl &= ~SBSDIO_DEVCTL_CA_INT_ONLY;
			bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, devctl, &err);
		}

		bus->clkstate = CLK_SDONLY;
		bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, clkreq, &err);
		if (err)
			return BCME_ERROR;
	}
	return BCME_OK;
}

/* Change idle/active SD state */
static int dhdsdio_sdclk(dhd_bus_t *bus, bool on)
{
	int err;
	int32 iovalue;

	if (on) 
	{
		if (bus->idleclock == DHD_IDLE_STOP) 
		{
			/* Turn on clock and restore mode */
			iovalue = 1;
			err = bcmsdh_iovar_op(bus->sdh, "sd_clock", NULL, 0,
			                      &iovalue, sizeof(iovalue), TRUE);
			if (err) 
				return BCME_ERROR;

			iovalue = bus->sd_mode;
			err = bcmsdh_iovar_op(bus->sdh, "sd_mode", NULL, 0,
			                      &iovalue, sizeof(iovalue), TRUE);
			if (err) 
				return BCME_ERROR;
		} 
		else if (bus->idleclock != DHD_IDLE_ACTIVE) 
		{
			/* Restore clock speed */
			iovalue = bus->sd_divisor;
			err = bcmsdh_iovar_op(bus->sdh, "sd_divisor", NULL, 0,
			                      &iovalue, sizeof(iovalue), TRUE);
			if (err) 
				return BCME_ERROR;
		}
		bus->clkstate = CLK_SDONLY;
	}
	else 
	{
		/* Stop or slow the SD clock itself */
		if ((bus->sd_divisor == -1) || (bus->sd_mode == -1)) 
			return BCME_ERROR;
		
		if (bus->idleclock == DHD_IDLE_STOP) 
		{
			if (sd1idle) 
			{
				/* Change to SD1 mode and turn off clock */
				iovalue = 1;
				err = bcmsdh_iovar_op(bus->sdh, "sd_mode", NULL, 0,
				                      &iovalue, sizeof(iovalue), TRUE);
				if (err) 
					return BCME_ERROR;
			}

			iovalue = 0;
			err = bcmsdh_iovar_op(bus->sdh, "sd_clock", NULL, 0,
			                      &iovalue, sizeof(iovalue), TRUE);
			if (err) 
				return BCME_ERROR;
		} 
		else if (bus->idleclock != DHD_IDLE_ACTIVE) 
		{
			/* Set divisor to idle value */
			iovalue = bus->idleclock;
			err = bcmsdh_iovar_op(bus->sdh, "sd_divisor", NULL, 0,
			                      &iovalue, sizeof(iovalue), TRUE);
			if (err) 
				return BCME_ERROR;
		}
		bus->clkstate = CLK_NONE;
	}

	return BCME_OK;
}

/* Transition SD and backplane clock readiness */
int dhdsdio_clkctl(dhd_bus_t *bus, uint target, bool pendok)
{
	int ret = BCME_OK;

	/* Early exit if we're already there */
	if (bus->clkstate == target) 
	{
		if (target == CLK_AVAIL) 
		{
			bus->activity = TRUE;
		}
		return ret;
	}

	switch (target) 
	{
	case CLK_AVAIL:
		/* Make sure SD clock is available */
		if (bus->clkstate == CLK_NONE)
			dhdsdio_sdclk(bus, TRUE);
		/* Now request HT Avail on the backplane */
		ret = dhdsdio_htclk(bus, TRUE, pendok);
		if (ret == BCME_OK) 
		{
			bus->activity = TRUE;
		}
		break;

	case CLK_SDONLY:
		/* Remove HT request, or bring up SD clock */
		if (bus->clkstate == CLK_NONE)
			ret = dhdsdio_sdclk(bus, TRUE);
		else if (bus->clkstate == CLK_AVAIL)
			ret = dhdsdio_htclk(bus, FALSE, FALSE);
		break;

	case CLK_NONE:
		/* Make sure to remove HT request */
		if (bus->clkstate == CLK_AVAIL)
			ret = dhdsdio_htclk(bus, FALSE, FALSE);
		/* Now remove the SD clock */
		ret = dhdsdio_sdclk(bus, FALSE);
		break;
	}

	return ret;
}


static void dhdsdio_rxfail(dhd_bus_t *bus, bool abort, bool rtx)
{
	bcmsdh_info_t *sdh = bus->sdh;
	sdpcmd_regs_t *regs = bus->regs;
	uint retries = 0;
	uint16 lastrbc;
	uint8 hi, lo;
	int err;

	if (abort) 
	{
		bcmsdh_abort(sdh, SDIO_FUN_2);
	}

	bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_FRAMECTRL, SFC_RF_TERM, &err);
	bus->f1regdata++;

	/* Wait until the packet has been flushed (device/FIFO stable) */
	for (lastrbc = retries = 0xffff; retries > 0; retries--) 
	{
		hi = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_RFRAMEBCHI, NULL);
		lo = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_RFRAMEBCLO, NULL);
		bus->f1regdata += 2;

		if ((hi == 0) && (lo == 0))
			break;

		if ((hi > (lastrbc >> 8)) && (lo > (lastrbc & 0x00ff))) 
		{
			;
		}
		lastrbc = (hi << 8) + lo;
	}

	if (rtx) 
	{
		bus->rxrtx++;
		W_SDREG(SMB_NAK, &regs->tosbmailbox, retries);
		bus->f1regdata++;
		if (retries <= retry_limit) 
		{
			bus->rxskip = TRUE;
		}
	}

	/* Clear partial in any case */
	bus->nextlen = 0;

	/* If we can't reach the device, signal failure */
	if (err || bcmsdh_regfail(sdh))
	{   
    	bus->dhd->busstate = DHD_BUS_DOWN;
	}
}

/*
dgl:the start position of receive must be align 32, 
	as we had sure is in rxbuf = get_global_cmdbuf(DATA_RXBUF_HEAD);
	so here clear the dhd_alignctl,for programe run faster.
*/
static void dhdsdio_read_control(dhd_bus_t *bus, uint8 *hdr, uint len, uint doff)
{
	bcmsdh_info_t *sdh = bus->sdh;
	uint rdlen, pad;
	int sdret;

	/* Set rxctl for frame (w/optional alignment) */
	bus->rxctl = bus->rxbuf;//bus->rxbuf = get_global_cmdbuf(DATA_RXBUF_HEAD);
	if (firstread % DHD_SDALIGN)
	{
		while(1);
	}

	/* Copy the already-read portion over */
	memcpy(bus->rxctl, hdr, firstread);
	if (len <= firstread)
		goto gotpkt;

	/* Raise rdlen to next SDIO block to avoid tail command */
	rdlen = len - firstread;
	if (bus->roundup && bus->blocksize && (rdlen > bus->blocksize)) 
	{
		pad = bus->blocksize - (rdlen % bus->blocksize);
		if ((pad <= bus->roundup) && (pad < bus->blocksize) && ((len + pad) < bus->dhd->maxctl))
		{
			rdlen += pad;
		}
	} 
	else if (rdlen % DHD_SDALIGN) 
	{
		rdlen += DHD_SDALIGN - (rdlen % DHD_SDALIGN);
	}

	/* Satisfy length-alignment requirements */
	if (forcealign && (rdlen & (ALIGNMENT - 1)))
	{
		rdlen = ROUNDUP(rdlen, ALIGNMENT);
	}

	/* Drop if the read is too big or it exceeds our maximum */
	if ((rdlen + firstread) > bus->dhd->maxctl) 
	{
		printf("%d ex %d buf\n", rdlen, bus->dhd->maxctl);
		bus->dhd->rx_errors++;
		dhdsdio_rxfail(bus, FALSE, FALSE);
		goto done;
	}

	/* Read remainder of frame body into the rxctl buffer */
	sdret = bcmsdh_recv_buf(bus->sdh, bcmsdh_cur_sbwad(sdh), SDIO_FUN_2, F2SYNC, bus->rxctl+firstread, rdlen);
	bus->f2rxdata++;

	/* Control frame failures need retransmission */
	if (sdret < 0) 
	{
		BCMDEBUG("read %d control bytes failed: %d\n",  rdlen, sdret);
		bus->rxc_errors++; /* dhd.rx_ctlerrs is higher level */
		dhdsdio_rxfail(bus, TRUE, TRUE);
		goto done;
	}
  
gotpkt:
	/* Point to valid data and indicate its length */
	bus->rxctl += doff;//move to the header of "cdc_ioctrl_t + msg" postion.
	bus->rxlen = len - doff;

    bus->is_get_ctrl_frame = 1; 
done:
	return;
}

static uint dhdsdio_readframes(dhd_bus_t *bus, uint maxframes, bool *finished)
{
	bcmsdh_info_t *sdh = bus->sdh;
	uint16 len, check;	/* Extracted hardware header fields */
	uint8 chan, seq, doff;	/* Extracted software header fields */
	uint8 fcbits;		/* Extracted fcbits from software header */
	uint8 delta;
	void *pkt = NULL;	/* Packet for event or data frames */
	uint16 pad;	/* Number of pad bytes to read */
	uint16 rdlen;	/* Total number of bytes to read */
	uint8 rxseq;	/* Next sequence number to expect */
	uint rxleft = 0;	/* Remaining number of frames allowed */
	int sdret;	/* Return code from bcmsdh calls */
	uint8 txmax;	/* Maximum tx sequence offered */
	int ifidx = 0;
	uint rxcount = 0; /* Total frames read */

	ASSERT(maxframes);

	/* Not finished unless we encounter no more frames indication */
	*finished = FALSE;

	for (rxseq = bus->rx_seq, rxleft = maxframes;
	     !bus->rxskip && rxleft && bus->dhd->busstate != DHD_BUS_DOWN;
	     rxseq++, rxleft--) 
	{
		/* Read frame header (hardware and software) */
		sdret = bcmsdh_recv_buf(bus->sdh, bcmsdh_cur_sbwad(sdh), SDIO_FUN_2, F2SYNC, bus->rxhdr, firstread);
		
		bus->f2rxhdrs++;
		ASSERT(sdret != BCME_PENDING);

		if (sdret < 0) 
		{
			BCMDEBUG(" RXHEADER FAILED: %d\n",sdret);
			bus->rx_hdrfail++;
			dhdsdio_rxfail(bus, TRUE, TRUE);
			continue;
		}

		/* Extract hardware header fields */
		len = ltoh16_ua(bus->rxhdr);
		check = ltoh16_ua(bus->rxhdr + sizeof(uint16));
		/* All zeros means no more frames */
		if (!(len|check)) 
		{
			*finished = TRUE;
			break;
		}

		/* Validate check bytes */
		if ((uint16)~(len^check)) 
		{
			BCMDEBUG(" HW hdr error: len/check 0x%04x/0x%04x\n", len, check);
			bus->rx_badhdr++;
			dhdsdio_rxfail(bus, FALSE, FALSE);
			continue;
		}

		/* Validate frame length */
		if (len < SDPCM_HDRLEN)
		{
			BCMDEBUG(" HW hdr length invalid: %d\n",  len);
			continue;
		}

		/* Extract software header fields */
		chan = SDPCM_PACKET_CHANNEL(&bus->rxhdr[SDPCM_FRAMETAG_LEN]);
		seq = SDPCM_PACKET_SEQUENCE(&bus->rxhdr[SDPCM_FRAMETAG_LEN]);
		doff = SDPCM_DOFFSET_VALUE(&bus->rxhdr[SDPCM_FRAMETAG_LEN]);
		txmax = SDPCM_WINDOW_VALUE(&bus->rxhdr[SDPCM_FRAMETAG_LEN]);

        BCMDEBUG("chan = %d, seq = %d, doff = %d, txmax = %d\n", chan, seq, doff, txmax);
		/* Validate data offset */
		if ((doff < SDPCM_HDRLEN) || (doff > len)) 
		{
			BCMDEBUG(" Bad data offset %d: HW len %d, min %d seq %d\n", doff, len, SDPCM_HDRLEN, seq);
			bus->rx_badhdr++;
			ASSERT(0);
			dhdsdio_rxfail(bus, FALSE, FALSE);
			continue;
		}

		/* Save the readahead length if there is one */
		bus->nextlen = bus->rxhdr[SDPCM_FRAMETAG_LEN + SDPCM_NEXTLEN_OFFSET];
		if ((bus->nextlen << 4) > MAX_RX_DATASZ) 
		{
			printf("(nextlen): (%d), seq %d\n",bus->nextlen, seq);
			bus->nextlen = 0;
		}

		/* Handle Flow Control */
		fcbits = SDPCM_FCMASK_VALUE(&bus->rxhdr[SDPCM_FRAMETAG_LEN]);

		delta = 0;
		if (~bus->flowcontrol & fcbits) 
		{
			bus->fc_xoff++;
			delta = 1;
		}
		if (bus->flowcontrol & ~fcbits) 
		{
			bus->fc_xon++;
			delta = 1;
		}

		if (delta) 
		{
			bus->fc_rcvd++;
			bus->flowcontrol = fcbits;
		}

		/* Check and update sequence number */
		if (rxseq != seq) 
		{
			BCMDEBUG("rx_seq %d, expected %d\n", seq, rxseq);
			bus->rx_badseq++;
			rxseq = seq;
		}

		/* Check window for sanity */
		if ((uint8)(txmax - bus->tx_seq) > 0x40) 
		{
			BCMDEBUG("got unlikely tx max %d with tx_seq %d\n", txmax, bus->tx_seq);
			txmax = bus->tx_max;
		}
		bus->tx_max = txmax;

		/* Call a separate function for control frames */
		if (chan == SDPCM_CONTROL_CHANNEL) 
		{
			dhdsdio_read_control(bus, bus->rxhdr, len, doff);
			break;//continue;
		}
		
		ASSERT((chan == SDPCM_EVENT_CHANNEL) || (chan == SDPCM_DATA_CHANNEL) ||
		       (chan == SDPCM_GLOM_CHANNEL)  || (chan == SDPCM_TEST_CHANNEL));
		if (wpadebugen)printf("chan\n");
		/* Length to read */
		rdlen = (len > firstread) ? (len - firstread) : 0;

		/* May pad read to blocksize for efficiency */
		if (bus->roundup && bus->blocksize && (rdlen > bus->blocksize)) 
		{
			pad = bus->blocksize - (rdlen % bus->blocksize);
			if ((pad <= bus->roundup) && (pad < bus->blocksize) && ((rdlen + pad + firstread) < MAX_RX_DATASZ))
			{
				rdlen += pad;
			}
		} 
		else if (rdlen % DHD_SDALIGN) 
		{
			rdlen += DHD_SDALIGN - (rdlen % DHD_SDALIGN);
		}
		
		/* Satisfy length-alignment requirements */
		if (forcealign && (rdlen & (ALIGNMENT - 1)))
		{
			rdlen = ROUNDUP(rdlen, ALIGNMENT);
		}

		if ((rdlen + firstread) > MAX_RX_DATASZ) 
		{
			/* Too long -- skip this frame */
			printf("too long: len %d rdlen %d\n",len, rdlen);
			bus->dhd->rx_errors++; 
			bus->rx_toolong++;
			dhdsdio_rxfail(bus, FALSE, FALSE);
			continue;
		}
		/*mallco the receive buffer for data...*/
		pkt = PKTGET(rdlen + firstread + DHD_SDALIGN);
		
		/* Leave room for what we already read, and align remainder */
		PKTPULL(pkt, firstread);
		PKTALIGN(pkt, rdlen, DHD_SDALIGN);
		/* Read the remaining frame data */
	    sdret = bcmsdh_recv_buf(bus->sdh, bcmsdh_cur_sbwad(sdh), SDIO_FUN_2, F2SYNC, PKTDATA(pkt), rdlen);
		bus->f2rxdata++;
		ASSERT(sdret != BCME_PENDING);

		if (sdret < 0) 
		{
			BCMDEBUG("read %d %s bytes failed: %d\n",rdlen,
			           ((chan == SDPCM_EVENT_CHANNEL) ? "event" :
			            ((chan == SDPCM_DATA_CHANNEL) ? "data" : "test")), sdret);

			bus->dhd->rx_errors++;
			dhdsdio_rxfail(bus, TRUE, RETRYCHAN(chan));
			continue;
		}

		/* Copy the already-read portion */
		PKTPUSH(pkt, firstread);
		bcopy(bus->rxhdr, PKTDATA(pkt), firstread);

		/* Fill in packet len and prio, deliver upward */
		PKTSETLEN(pkt, len);//dgl set the mymsg really length...
		PKTPULL(pkt, doff); //move to valid data.
		if (wpadebugen)printf("chan2\n");
		if (PKTLEN(pkt) == 0) 
		{
			PKTFREE(pkt);
			continue;
		} 
		else if (dhd_prot_hdrpull(bus->dhd, &ifidx, pkt) != 0) 
		{
			BCMDEBUG("rx protocol error\n");
			PKTFREE(pkt);
			bus->dhd->rx_errors++;
			continue;
		}
		/* Unlock during rx call */
		dhd_rx_frame(bus->dhd, ifidx, pkt, 1, chan);
        PKTFREE(pkt);
	}
	rxcount = maxframes - rxleft;
	//printf("processed %d frames\n", rxcount);
	
	/* Back off rxseq if awaiting rtx, update rx_seq */
	if (bus->rxskip)
		rxseq--;
	bus->rx_seq = rxseq;
	
	return rxcount;
}

static uint32 dhdsdio_hostmail(dhd_bus_t *bus)
{
	sdpcmd_regs_t *regs = bus->regs;
	uint32 intstatus = 0;
	uint32 hmb_data;
	uint8 fcbits;
	uint retries = 0;

	/* Read mailbox data and ack that we did so */
	R_SDREG(hmb_data, &regs->tohostmailboxdata, retries);
    printf("dhdsdio_hostmail=0x%04x.\n", hmb_data);
	//hmb_data = 0x40008. hmb_data = 0x40002.
	if (retries <= retry_limit)
	{
		W_SDREG(SMB_INT_ACK, &regs->tosbmailbox, retries);
	}
	bus->f1regdata += 2;

	/* Dongle recomposed rx frames, accept them again */
	if (hmb_data & HMB_DATA_NAKHANDLED) 
	{
		printf("Dongle reports NAK handled, expect rtx of %d\n", bus->rx_seq);
		if (!bus->rxskip) 
		{
			printf("unexpected NAKHANDLED!\n");
		}
		bus->rxskip = FALSE;
		intstatus |= FRAME_AVAIL_MASK(bus);
	}

	/*
	 * Flow Control has been moved into the RX headers and this out of band
	 * method isn't used any more.  Leave this here for possibly remaining backward
	 * compatible with older dongles
	 */
	if (hmb_data & HMB_DATA_FC) 
	{
		fcbits = (hmb_data & HMB_DATA_FCDATA_MASK) >> HMB_DATA_FCDATA_SHIFT;

		if (fcbits & ~bus->flowcontrol)
			bus->fc_xoff++;
		if (bus->flowcontrol & ~fcbits)
			bus->fc_xon++;

		bus->fc_rcvd++;
		bus->flowcontrol = fcbits;
	}


	/* Shouldn't be any others */
	if (hmb_data & ~(HMB_DATA_DEVREADY |
	                 HMB_DATA_FWHALT |
	                 HMB_DATA_NAKHANDLED |
	                 HMB_DATA_FC |
	                 HMB_DATA_FWREADY |
	                 HMB_DATA_FCDATA_MASK |
	                 HMB_DATA_VERSION_MASK)) 
	{
		BCMDEBUG("Unknown mailbox data content: 0x%02x\n", hmb_data);
	}

	return intstatus;
}

static bool dhdsdio_dpc(dhd_bus_t *bus)//process_sdio_pending_irqs
{
	bcmsdh_info_t *sdh = bus->sdh;
	sdpcmd_regs_t *regs = bus->regs;
	uint32 intstatus, newstatus = 0;
	uint retries = 0;
	uint rxlimit = DHD_RXBOUND; /* Rx frames to read before resched */
	uint txlimit = DHD_TXBOUND; /* Tx frames to send before resched */
	uint framecnt = 0;		  /* Temporary counter of tx/rx frames */
	bool rxdone = TRUE;		  /* Flag for no more read data */
	bool resched = FALSE;	  /* Flag indicating resched wanted */


	if (bus->dhd->busstate == DHD_BUS_DOWN) 
	{
		bus->intstatus = 0;
		return 0;
	}

	/* Start with leftover status bits */
	intstatus = bus->intstatus;

	/* If waiting for HTAVAIL, check status */
	if (bus->clkstate == CLK_PENDING) 
	{
		int err;
		uint8 clkctl, devctl = 0;

		/* Read CSR, if clock on switch to AVAIL, else ignore */
		clkctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, &err);
		if (err) 
		{
			BCMDEBUG("error reading CSR: %d\n", err);
			bus->dhd->busstate = DHD_BUS_DOWN;
		}

		BCMDEBUG("DPC: PENDING, devctl 0x%02x clkctl 0x%02x\n", devctl, clkctl);

		if (SBSDIO_HTAV(clkctl)) 
		{
			devctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, &err);
			if (err) 
			{
				BCMDEBUG(" error reading DEVCTL: %d\n", err);
				bus->dhd->busstate = DHD_BUS_DOWN;
			}
			devctl &= ~SBSDIO_DEVCTL_CA_INT_ONLY;
			bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, devctl, &err);
			if (err) 
			{
				BCMDEBUG("error writing DEVCTL: %d\n",err);
				bus->dhd->busstate = DHD_BUS_DOWN;
			}
			bus->clkstate = CLK_AVAIL;
		} 
		else 
		{
			goto clkwait;
		}
	}

	/* Make sure backplane clock is on */
	dhdsdio_clkctl(bus, CLK_AVAIL, TRUE);
	if (bus->clkstate != CLK_AVAIL)
	{
		goto clkwait;
	}

	/* Pending interrupt indicates new device status */
	if (bus->ipend) 
	{
		bus->ipend = FALSE;
		R_SDREG(newstatus, &regs->intstatus, retries);
		bus->f1regdata++;
		if (bcmsdh_regfail(bus->sdh))
			newstatus = 0;
		
		newstatus &= bus->hostintmask;
		bus->fcstate = !!(newstatus & I_HMB_FC_STATE);
		if (newstatus) 
		{
			bus->f1regdata++;
			W_SDREG(newstatus, &regs->intstatus, retries);
		}
	}

	/* Merge new bits with previous */
	intstatus |= newstatus;
	bus->intstatus = 0;

	//BCMDEBUG("intstatus = 0x%04x", intstatus);
	/* Handle flow-control change: read new state in case our ack
	 * crossed another change interrupt.  If change still set, assume
	 * FC ON for safety, let next loop through do the debounce.
	 */
	if (intstatus & I_HMB_FC_CHANGE) 
	{
		intstatus &= ~I_HMB_FC_CHANGE;
		W_SDREG(I_HMB_FC_CHANGE, &regs->intstatus, retries);
		R_SDREG(newstatus, &regs->intstatus, retries);
		bus->f1regdata += 2;
		bus->fcstate = !!(newstatus & (I_HMB_FC_STATE | I_HMB_FC_CHANGE));
		intstatus |= (newstatus & bus->hostintmask);
	}

	/* Just being here means nothing more to do for chipactive */
	if (intstatus & I_CHIPACTIVE) 
	{
		/* ASSERT(bus->clkstate == CLK_AVAIL); */
		intstatus &= ~I_CHIPACTIVE;
	}

	/* Handle host mailbox indication */
	if (intstatus & I_HMB_HOST_INT) 
	{
		intstatus &= ~I_HMB_HOST_INT;
		intstatus |= dhdsdio_hostmail(bus);
	}

	/* Generally don't ask for these, can get CRC errors... */
	if (intstatus & I_WR_OOSYNC) 
	{
		BCMDEBUG("Dongle reports WR_OOSYNC\n");
		intstatus &= ~I_WR_OOSYNC;
	}

	if (intstatus & I_RD_OOSYNC) 
	{
		BCMDEBUG("Dongle reports RD_OOSYNC\n");
		intstatus &= ~I_RD_OOSYNC;
	}

	if (intstatus & I_SBINT) 
	{
		BCMDEBUG("Dongle reports SBINT\n");
		intstatus &= ~I_SBINT;
	}

	/* Would be active due to wake-wlan in gSPI */
	if (intstatus & I_CHIPACTIVE) 
	{
		BCMDEBUG("Dongle reports CHIPACTIVE\n");
		intstatus &= ~I_CHIPACTIVE;
	}

	/* Ignore frame indications if rxskip is set */
	if (bus->rxskip) 
	{
		intstatus &= ~ FRAME_AVAIL_MASK(bus);
	}
	
	/* On frame indication, read available frames */
	if (PKT_AVAILABLE(bus, intstatus)) //I_HMB_FRAME_IND
	{
		framecnt = dhdsdio_readframes(bus, rxlimit, &rxdone); //rxlimit = 50;
		
		if (rxdone || bus->rxskip)
		{
			intstatus  &= ~ FRAME_AVAIL_MASK(bus);
		}
		rxlimit -= MIN(framecnt, rxlimit);
	}

	/* Keep still-pending events for next scheduling */
	bus->intstatus = intstatus;

clkwait:
	/* Re-enable interrupts to detect new device events (mailbox, rx frame)
	 * or clock availability.  (Allows tx loop to check ipend if desired.)
	 * (Unless register access seems hosed, as we may not be able to ACK...)
	 */
	if (bus->intr && bus->intdis && !bcmsdh_regfail(sdh)) 
	{
		BCMDEBUG(" enable SDIO interrupts, rxdone %d framecnt %d\n",rxdone, framecnt);
		bus->intdis = FALSE;
	}
	
	/* Resched if events or tx frames are pending, else await next interrupt */
	/* On failed register access, all bets are off: no resched or interrupts */
	if ((bus->dhd->busstate == DHD_BUS_DOWN) || bcmsdh_regfail(sdh)) 
	{
		BCMDEBUG( "failed backplane access over SDIO, halting operation %d \n",bcmsdh_regfail(sdh));
		bus->dhd->busstate = DHD_BUS_DOWN;
		bus->intstatus = 0;
	} 
	else if (bus->clkstate == CLK_PENDING) 
	{
		/* Awaiting I_CHIPACTIVE; don't resched */
	}
	else if (bus->intstatus || bus->ipend || PKT_AVAILABLE(bus, bus->intstatus)) 
	{  
		/* Read multiple frames */
		resched = TRUE;
	}

	bus->dpc_sched = resched;
	
	#if 0 //dgl no low power,
	/* If we're done for now, turn off clock request. */
	if ((bus->idletime == DHD_IDLE_IMMEDIATE) && (bus->clkstate != CLK_PENDING)) 
	{
		bus->activity = FALSE;
		dhdsdio_clkctl(bus, CLK_NONE, FALSE);
	}
	#endif

	return resched;
}

void dhdsdio_isr(void *arg)//process_sdio_pending_irqs
{
	dhd_bus_t *bus = (dhd_bus_t*)arg;
	
	if ((!bus) || (bus->dhd->busstate == DHD_BUS_DOWN) || (bus->sleeping))
	{
		return;
	}
	
	/* Count the interrupt call */
	bus->intrcount++;
	bus->ipend = TRUE;

	/* Disable additional interrupts (is this needed now)? */
	bus->intdis = TRUE;
	bus->dpc_sched = TRUE;
	
	while(bus->dpc_sched)
	{
	    dhdsdio_dpc(bus);
	} 
	
	return;
}

int dhdsdio_txpkt(dhd_bus_t *bus, void *pkt, uint chan, bool free_pkt)
{
	int ret;
	uint8 *frame;
	uint16 len, pad1 = 0;
	uint32 swheader;
	uint retries = 0;
	bcmsdh_info_t *sdh;
	void *new;
	int i;

	sdh = bus->sdh;

	if (bus->dhd->dongle_reset) 
	{
		ret = BCME_NOTREADY;
		goto done;
	}

	frame = (uint8*)PKTDATA(pkt);
	/* Add alignment padding, allocate new packet if needed */
	pad1 = (uint)frame % DHD_SDALIGN;
	if (pad1) 
	{
		PKTPUSH(pkt, pad1);
		frame = (uint8*)PKTDATA(pkt);
		bzero(frame, pad1 + SDPCM_HDRLEN);
	}

	/* Hardware tag: 2 byte len followed by 2 byte ~len check (all LE) */
	len = (uint16)PKTLEN(pkt);
	*(uint16*)frame = htol16(len);
	*(((uint16*)frame) + 1) = htol16(~len);

	/* Software tag: channel, sequence number, data offset */
	swheader = ((chan << SDPCM_CHANNEL_SHIFT) & SDPCM_CHANNEL_MASK) | bus->tx_seq |
	        (((pad1 + SDPCM_HDRLEN) << SDPCM_DOFFSET_SHIFT) & SDPCM_DOFFSET_MASK);
	htol32_ua_store(swheader, frame + SDPCM_FRAMETAG_LEN);
	htol32_ua_store(0, frame + SDPCM_FRAMETAG_LEN + sizeof(swheader));

	/* Raise len to next SDIO block to eliminate tail command */
	if (bus->roundup && bus->blocksize && (len > bus->blocksize)) 
	{
		uint16 pad2 = bus->blocksize - (len % bus->blocksize);
		if ((pad2 <= bus->roundup) && (pad2 < bus->blocksize))
		{
			len += pad2;
		}
	} 
	else if (len % DHD_SDALIGN) 
	{
		len += DHD_SDALIGN - (len % DHD_SDALIGN);
	}
	/* Some controllers have trouble with odd bytes -- round to even */
	if (forcealign && (len & (ALIGNMENT - 1))) 
	{
		len = ROUNDUP(len, ALIGNMENT);
    }
	
	do 
	{
		ret = bcmsdh_send_buf(bus->sdh, bcmsdh_cur_sbwad(sdh), SDIO_FUN_2, F2SYNC,frame, len);
       
        bus->f2txdata++;
		ASSERT(ret != BCME_PENDING);
		if (ret < 0) 
		{
			/* On failure, abort the command and terminate the frame */
			BCMDEBUG("%s: sdio error %d, abort command and terminate frame.\n", ret);
			bus->tx_sderrs++;

			bcmsdh_abort(sdh, SDIO_FUN_2);
			bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_FRAMECTRL, SFC_WF_TERM, NULL);
            bus->f1regdata++;

			for (i = 0; i < 3; i++) 
			{
				uint8 hi, lo;
				hi = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_WFRAMEBCHI, NULL);
				lo = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_WFRAMEBCLO, NULL);
				bus->f1regdata += 2;
				if ((hi == 0) && (lo == 0))
					break;
			}
		}
        if (ret == 0) 
		{
			bus->tx_seq = (bus->tx_seq + 1) % SDPCM_SEQUENCE_WRAP;
		}
	} while ((ret < 0) && retrydata && retries++ < TXRETRIES);


done:

	/* restore pkt buffer pointer before calling tx complete routine */
	PKTPULL(pkt, SDPCM_HDRLEN + pad1);

	return ret;
}


int dhd_senddata(dhd_pub_t *dhdp, char *buf, uint32 len)
{
	int ret;
    sk_buff *pktbuf;

	if (dhdp == NULL)
	{
		dhdp = G_dhd;
	}
    pktbuf = PKTGET(len);
    memcpy(PKTDATA(pktbuf), buf, len);

	/* If the protocol uses a data header, apply it */
	dhd_prot_hdrpush(dhdp, 0, pktbuf);  //增加协议头

	PKTPUSH (pktbuf, SDPCM_HDRLEN);  //增加 SDPCM_HDRLEN 空间

	ASSERT(ISALIGNED((uint)PKTDATA(pktbuf), 2));

	ret = dhdsdio_txpkt(dhdp->bus, pktbuf, SDPCM_DATA_CHANNEL, TRUE);

    PKTFREE(pktbuf);
	return ret;
}


void wl_iw_event(dhd_pub_t *pub, wl_event_msg_t *e, void* data)
{
	uint32 reason = ntoh32(e->reason);
	uint32 event_type = ntoh32(e->event_type);
	uint16 flags =  ntoh16(e->flags);
	
	switch (event_type) 
	{
		case WLC_E_LINK:// 16 =
		case WLC_E_NDIS_LINK:// 18
			if (!(flags & WLC_EVENT_MSG_LINK)) 
			{
				printf("Link Down Event %d,reason=0x%08x\n", event_type, reason);
				MSG_CLR_WPA_SH();
			}
			else 
			{
				printf("Link UP reason=0x%08x\n", reason);
			}
			break;

	    case WLC_E_ESCAN_RESULT:// 69
			if (ScanResultEventEnable)
			{
		        wl_escan_handler(e, data);
			}
	        break;

		default:
			printf("E %d\n", event_type);
			break;
	}
	
	return;
}

int rk_wifi_connect_pro(char * ssid, char * password, int pwd_len)
{   
    uint timeleft = 10000;//dgl set 10s is one loop...
	int conn_try, ret;
	int wpa_try_num = 3;

	MSG_CLR_WPA_SH();

    if (rk_wifi_connect_init(ssid, password, pwd_len))
    {
        return -1;
	}
    ModuleOverlay(MODULE_ID_WIFI_WPA, MODULE_OVERLAY_CODE);  
	rk_80211_handle_init();
	wpa_auth_init();    
    
	while (wpa_try_num--)
	{
		ModuleOverlay(MODULE_ID_WIFI_SCAN, MODULE_OVERLAY_CODE);

		conn_try = 10;
		while (conn_try--)
		{
			ret = rk_wifi_connect(ssid, password, pwd_len);
			if (0 == ret)
			{
				break;
			}
			else
			{	
				DelayMs(100);
			}
		}
	    if (ret)
	    {
	        return -1;
		}
    
	    if ((g_connect_bss->encode_type == ENCODE_TYPE_WEP) ||
	        (g_connect_bss->encode_type == ENCODE_TYPE_NO) )
	    {
	        return 0;
	    }
        
        timeleft = 10000;
	    while(timeleft)
	    {
            if (SDC_GetSdioEvent(0) == SDC_SUCCESS)
	        { 
	            process_sdio_pending_irqs();
	        }

            rk_80211_data_handle();
	        
			if (ReTryAssocAP)
			{
				scb_val_t scbval;
				uint8 * curbssid;

				printf("send WLC_DISASSOC cmd\n");
				curbssid = wifi_get_mac_addr(NULL);
				scbval.val = WLAN_REASON_DEAUTH_LEAVING;
				memcpy(&scbval.ea, curbssid, ETHER_ADDR_LEN);
				scbval.val = htod32(scbval.val);
				dhd_wl_ioctl_cmd(G_dhd, WLC_DISASSOC, &scbval, sizeof(scb_val_t), 1);
				DelayMs(500);
				
				ReTryAssocAP = 0;
				break;
			}
			
	        if (!MSG_LINK_DOWN())
	        {
	            return 0;
	        }
           
	        DelayMs(1);
	        if (--timeleft == 0)
	        {
	            printf("wpa connect timeout\n");
	        }
	    }
	}

    return -1;
}

uint bcm_mkiovar(char *name, char *data, uint datalen, char *buf, uint buflen)
{
	uint len;

	len = strlen(name) + 1;

	if ((len + datalen) > buflen)
		return 0;

	strncpy(buf, name, buflen);

	/* append data onto the end of the name string */
	memcpy(&buf[len], data, datalen);
	len += datalen;

	return len;
}

int32 wldev_iovar_setbuf(dhd_pub_t *pub, int8 *iovar_name, void *param, int32 paramlen, 
	                         void *buf, int32 buflen, void * buf_sync)
{
	int32 ret = 0;
	int32 iovar_len;

	iovar_len = bcm_mkiovar(iovar_name, param, paramlen, buf, buflen);
    ret = dhd_wl_ioctl_cmd(pub, WLC_SET_VAR, buf, iovar_len, 1);
	return ret;
}


