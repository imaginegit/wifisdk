/*
 * DHD Bus Module for SDIO
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
 * $Id: dhd_sdio.c 309234 2012-01-19 01:44:16Z $
 */
#define RK903   1
#define RK901   2
#define AP6181  3

#define WIFI_MODULE AP6181

#include "rk903.h"
#include  "ModuleInfoTab.h"
#if (WIFI_MODULE == RK903)
#include  "fw_RK903.h"
#include  "nvram_RK903.h"
#elif (WIFI_MODULE == RK901)
#include  "fw_RK901.h"
#include  "nvram_RK901.h"
#elif (WIFI_MODULE == AP6181)
#include  "nvram_AP6181.h"
#include  "fw_AP6181.h"
#endif

/*rk903 firmware part...*/

extern uint32 Load$$WIFI_FIRMWARE_ONE$$Base;
extern uint32 Load$$WIFI_FIRMWARE_TWO$$Base;


struct wifi_model_fw
{
	const char *pfirmware;
	const char *pnvram;
};

static struct wifi_model_fw wifi_info = 
{
	wifi_fw,
	wifi_nvram,
};


#define MEMBLOCK			2048	/* Block size used for downloading of dongle image */
#define MAX_NVRAMBUF_SIZE	2048	/* max nvram buf size */

/* Maximum milliseconds to wait for F2 to come up */
#define DHD_WAIT_F2RDY	3000

/* Bump up limit on waiting for HT to account for first startup;
 * if the image is doing a CRC calculation before programming the PMU
 * for HT availability, it could take a couple hundred ms more, so
 * max out at a 1 second (1000000us).
 */
#if (PMU_MAX_TRANSITION_DLY <= 1000000)
#undef PMU_MAX_TRANSITION_DLY
#define PMU_MAX_TRANSITION_DLY 1000000
#endif

/* Value for ChipClockCSR during initial setup */
#define DHD_INIT_CLKCTL1	(SBSDIO_FORCE_HW_CLKREQ_OFF | SBSDIO_ALP_AVAIL_REQ)
#define DHD_INIT_CLKCTL2	(SBSDIO_FORCE_HW_CLKREQ_OFF | SBSDIO_FORCE_ALP)

/* Flags for SDH calls */
//#define F2SYNC	(SDIO_REQ_4BYTE | SDIO_REQ_FIXED)

/* Packet free applicable unconditionally for sdio and sdspi.  Conditional if
 * bufpool was present for gspi bus.
 */
#define DHD_NOPMU(dhd)	(FALSE)

static const uint watermark = 8;

#define FW_TYPE_STA     0
#define FW_TYPE_APSTA   1
#define FW_TYPE_P2P     2
#define FW_TYPE_MFG     3

#define BCM4330B1_CHIP_REV      3
#define BCM4330B2_CHIP_REV      4
#define BCM43362A0_CHIP_REV     0
#define BCM43362A2_CHIP_REV     1

//#define ALIGNMENT  4

/* Limit on rounding up frames */
static const uint max_roundup = 512;

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

#define BUS_WAKE(bus) \
	do { \
		if ((bus)->sleeping) \
			dhdsdio_bussleep((bus), FALSE); \
	} while (0);

/*
 * pktavail interrupts from dongle to host can be managed in 3 different ways
 * whenever there is a packet available in dongle to transmit to host.
 *
 * Mode 0:	Dongle writes the software host mailbox and host is interrupted.
 * Mode 1:	(sdiod core rev >= 4)
 *		Device sets a new bit in the intstatus whenever there is a packet
 *		available in fifo.  Host can't clear this specific status bit until all the
 *		packets are read from the FIFO.  No need to ack dongle intstatus.
 * Mode 2:	(sdiod core rev >= 4)
 *		Device sets a bit in the intstatus, and host acks this by writing
 *		one to this bit.  Dongle won't generate anymore packet interrupts
 *		until host reads all the packets from the dongle and reads a zero to
 *		figure that there are no more packets.  No need to disable host ints.
 *		Need to ack the intstatus.
 */

#define SDIO_DEVICE_HMB_RXINT		0	/* default old way */
#define SDIO_DEVICE_RXDATAINT_MODE_0	1	/* from sdiod rev 4 */
#define SDIO_DEVICE_RXDATAINT_MODE_1	2	/* from sdiod rev 4 */

#define DHD_BUS			SDIO_BUS

#define HOSTINTMASK		(I_HMB_SW_MASK | I_CHIPACTIVE)

extern void si_sdiod_drive_strength_init(si_t *sih, uint32 drivestrength);
static bool dhdsdio_chipmatch(uint16 chipid);
static bool dhdsdio_probe_attach(dhd_bus_t *bus, void *sdh, void * regsva, uint16  devid);
static bool dhdsdio_probe_malloc(dhd_bus_t *bus, void *sdh);
static bool dhdsdio_probe_init(dhd_bus_t *bus, void *sdh);

/*
address is 32bit,div 3time,16bit 8bit 8bit
warn: first time 16bit.(address >> 8) & 0x80 32k align.
*/
static int dhdsdio_set_siaddr_window(dhd_bus_t *bus, uint32 address)
{
	int err = 0;
	
	bcmsdh_cfg_write(bus->sdh, SDIO_FUN_1, SBSDIO_FUNC1_SBADDRLOW,
	                 (address >> 8) & SBSDIO_SBADDRLOW_MASK, &err);
	if (!err)
		bcmsdh_cfg_write(bus->sdh, SDIO_FUN_1, SBSDIO_FUNC1_SBADDRMID,
		                 (address >> 16) & SBSDIO_SBADDRMID_MASK, &err);
	if (!err)
		bcmsdh_cfg_write(bus->sdh, SDIO_FUN_1, SBSDIO_FUNC1_SBADDRHIGH,
		                 (address >> 24) & SBSDIO_SBADDRHIGH_MASK, &err);
	
	return err;
}

static int dhdsdio_bussleep(dhd_bus_t *bus, bool sleep)
{
	bcmsdh_info_t *sdh = bus->sdh;
	sdpcmd_regs_t *regs = bus->regs;
	uint retries = 0;

	BCMDEBUG("dhdsdio_bussleep: request %s (currently %s)\n",
	          (sleep ? "SLEEP" : "WAKE"),
	          (bus->sleeping ? "SLEEP" : "WAKE"));

	/* Done if we're already in the requested state */
	if (sleep == bus->sleeping)
		return BCME_OK;

	/* Going to sleep: set the alarm and turn off the lights... */
	if (sleep) 
	{
		/* Don't sleep if something is pending */
		if (bus->dpc_sched || bus->rxskip)//(|| pktq_len(&bus->txq))
			return BCME_BUSY;

		/* Make sure the controller has the bus up */
		dhdsdio_clkctl(bus, CLK_AVAIL, FALSE);

		/* Tell device to start using OOB wakeup */
		W_SDREG(SMB_USE_OOB, &regs->tosbmailbox, retries);
		if (retries > retry_limit)
			BCMDEBUG("CANNOT SIGNAL CHIP, WILL NOT WAKE UP!!\n");

		/* Turn off our contribution to the HT clock request */
		dhdsdio_clkctl(bus, CLK_SDONLY, FALSE);

		bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, SBSDIO_FORCE_HW_CLKREQ_OFF, NULL);

		/* Isolate the bus */
		if (bus->sih->chip != BCM4329_CHIP_ID && bus->sih->chip != BCM4319_CHIP_ID) 
		{
			bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, SBSDIO_DEVCTL_PADS_ISO, NULL);
		}

		/* Change state */
		bus->sleeping = TRUE;

	} 
	else 
	{
		/* Waking up: bus power up is ok, set local state */
		bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, 0, NULL);

		/* Force pad isolation off if possible (in case power never toggled) */
		bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_DEVICE_CTL, 0, NULL);


		/* Make sure the controller has the bus up */
		dhdsdio_clkctl(bus, CLK_AVAIL, FALSE);

		/* Send misc interrupt to indicate OOB not needed */
		W_SDREG(0, &regs->tosbmailboxdata, retries);
		if (retries <= retry_limit)
			W_SDREG(SMB_DEV_INT, &regs->tosbmailbox, retries);

		if (retries > retry_limit)
			BCMDEBUG("CANNOT SIGNAL CHIP TO CLEAR OOB!!\n");

		/* Make sure we have SD bus access */
		dhdsdio_clkctl(bus, CLK_SDONLY, FALSE);

		/* Change state */
		bus->sleeping = FALSE;

		/* Enable interrupts again */
		if (bus->intr && (bus->dhd->busstate == DHD_BUS_DATA)) 
		{
			bus->intdis = FALSE;
		}
	}

	return BCME_OK;
}

void dhd_bus_clearcounts(dhd_pub_t *dhdp)
{
	dhd_bus_t *bus = (dhd_bus_t *)dhdp->bus;

	bus->intrcount = bus->lastintrs = bus->spurious = bus->regfails = 0;
	bus->rxrtx = bus->rx_toolong = bus->rxc_errors = 0;
	bus->rx_hdrfail = bus->rx_badhdr = bus->rx_badseq = 0;
	bus->tx_sderrs = bus->fc_rcvd = bus->fc_xoff = bus->fc_xon = 0;
	bus->rxglomfail = bus->rxglomframes = bus->rxglompkts = 0;
	bus->f2rxhdrs = bus->f2rxdata = bus->f2txdata = bus->f1regdata = 0;
	return;
}


static int dhdsdio_membytes(dhd_bus_t *bus, bool write, uint32 address, uint8 *data, uint size)
{
	int bcmerror = 0;
	uint32 sdaddr;
	uint dsize;

	/* Determine initial transfer parameters */
	sdaddr = address & SBSDIO_SB_OFT_ADDR_MASK;
	if ((sdaddr + size) & SBSDIO_SBWINDOW_MASK)
		dsize = (SBSDIO_SB_OFT_ADDR_LIMIT - sdaddr);
	else
		dsize = size;

	/* Set the backplane window to include the start address */
	bcmerror = dhdsdio_set_siaddr_window(bus, address);
	if (bcmerror) 
	{
		goto xfer_done;
	}

	/* Do the transfer(s) */
	while (size) 
	{
		bcmerror = bcmsdh_rwdata(bus->sdh, write, sdaddr, data, dsize);
		if (bcmerror) 
			break;

		/* Adjust for next transfer (if any) */
		if ((size -= dsize)) 
		{
			data += dsize;
			address += dsize;
			bcmerror = dhdsdio_set_siaddr_window(bus, address);
			if (bcmerror) 
				break;
			sdaddr = 0;
			dsize = MIN(SBSDIO_SB_OFT_ADDR_LIMIT, size);
		}
	}

xfer_done:
	/* Return the window to backplane enumeration space for core access */
	dhdsdio_set_siaddr_window(bus, bcmsdh_cur_sbwad(bus->sdh));

	return bcmerror;
}

int dhdsdio_downloadvars(dhd_bus_t *bus, void *arg, int len)
{
	int bcmerror = BCME_OK;

	/* Basic sanity checks */
	if (bus->dhd->up) 
	{
		bcmerror = BCME_NOTDOWN;
		goto err;
	}
	if (!len) 
	{
		bcmerror = BCME_BUFTOOSHORT;
		goto err;
	}

	/* Free the old ones and replace with passed variables */
	if (bus->vars)
	{
		sys_free(bus->vars);
		bus->vars = NULL;
	}

	bus->vars = sys_calloc(len);
	bus->varsz = bus->vars ? len : 0;
	if (bus->vars == NULL) 
	{
		bcmerror = BCME_NOMEM;
		goto err;
	}

	/* Copy the passed variables, which should include the terminating double-null */
	bcopy(arg, bus->vars, bus->varsz);
err:
	return bcmerror;
}

static int dhdsdio_write_vars(dhd_bus_t *bus)
{
	int bcmerror = 0;
	uint32 varsize;
	uint32 varaddr;
	uint8 *vbuffer;
	uint32 varsizew;

	/* Even if there are no vars are to be written, we still need to set the ramsize. */
	varsize = bus->varsz ? ROUNDUP(bus->varsz, 4) : 0;
	varaddr = (bus->ramsize - 4) - varsize;

	if (bus->vars) 
	{
		if ((bus->sih->buscoretype == SDIOD_CORE_ID) && (bus->sdpcmrev == 7)) 
		{
			if (((varaddr & 0x3C) == 0x3C) && (varsize > 4)) 
			{
				varsize += 4;
				varaddr -= 4;
			}
		}

		vbuffer = (uint8 *)sys_malloc(varsize);
		if (!vbuffer)
			return BCME_NOMEM;

		bzero(vbuffer, varsize);
		bcopy(bus->vars, vbuffer, bus->varsz);

		/* Write the vars list */
		bcmerror = dhdsdio_membytes(bus, TRUE, varaddr, vbuffer, varsize);
		sys_free(vbuffer);
	}

	/* adjust to the user specified RAM */
	varsize = ((bus->orig_ramsize - 4) - varaddr);

	/*
	 * Determine the length token:
	 * Varsize, converted to words, in lower 16-bits, checksum in upper 16-bits.
	 */
	if (bcmerror) 
	{
		varsizew = 0;
	} 
	else 
	{
		varsizew = varsize / 4;
		varsizew = (~varsizew << 16) | (varsizew & 0x0000FFFF);
		//varsizew = htol32(varsizew);
	}

	/* Write the length token to the last word */
	bcmerror = dhdsdio_membytes(bus, TRUE, (bus->orig_ramsize - 4), (uint8*)&varsizew, 4);

	return bcmerror;
}

static int dhdsdio_download_state(dhd_bus_t *bus, bool enter)
{
	uint retries;
	int bcmerror = 0;

	/* To enter download state, disable ARM and reset SOCRAM.
	 * To exit download state, simply reset ARM (default is RAM boot).
	 */
	if (enter) 
	{
		bus->alp_only = TRUE;

		if (!(si_setcore(bus->sih, ARM7S_CORE_ID, 0)) &&
		    !(si_setcore(bus->sih, ARMCM3_CORE_ID, 0))) 
		{
			bcmerror = BCME_ERROR;
			goto fail;
		}

		si_core_disable(bus->sih, 0);
		if (bcmsdh_regfail(bus->sdh)) 
		{
			bcmerror = BCME_SDIO_ERROR;
			goto fail;
		}

		if (!(si_setcore(bus->sih, SOCRAM_CORE_ID, 0))) 
		{
			bcmerror = BCME_ERROR;
			goto fail;
		}

		si_core_reset(bus->sih, 0, 0);
		
		if (bcmsdh_regfail(bus->sdh)) 
		{
			bcmerror = BCME_SDIO_ERROR;
			goto fail;
		}

		/* Clear the top bit of memory */
		if (bus->ramsize) 
		{
			uint32 zeros = 0;
			if (dhdsdio_membytes(bus, TRUE, bus->ramsize - 4, (uint8*)&zeros, 4) < 0) 
			{
				bcmerror = BCME_SDIO_ERROR;
				goto fail;
			}
		}
	}
	else 
	{
		/** To exit download state, simply reset ARM (default is RAM boot).*/
		if (!(si_setcore(bus->sih, SOCRAM_CORE_ID, 0))) 
		{
			bcmerror = BCME_ERROR;
			goto fail;
		}

		if (!si_iscoreup(bus->sih)) 
		{
			bcmerror = BCME_ERROR;
			goto fail;
		}
	    
		bcmerror = dhdsdio_write_vars(bus);
		if (bcmerror) 
			goto fail;

		if (!si_setcore(bus->sih, SDIOD_CORE_ID, 0)) 
		{
			bcmerror = BCME_ERROR;
			goto fail;
		}
		W_SDREG(0xFFFFFFFF, &bus->regs->intstatus, retries);


		if (!(si_setcore(bus->sih, ARM7S_CORE_ID, 0)) &&
		    !(si_setcore(bus->sih, ARMCM3_CORE_ID, 0))) 
		{
			bcmerror = BCME_ERROR;
			goto fail;
		}

		si_core_reset(bus->sih, 0, 0);
		if (bcmsdh_regfail(bus->sdh)) 
		{
			bcmerror = BCME_SDIO_ERROR;
			goto fail;
		}

		/* Allow HT Clock now that the ARM is running. */
		bus->alp_only = FALSE;

		bus->dhd->busstate = DHD_BUS_LOAD;
	}

fail:
	/* Always return to SDIOD core */
	si_setcore(bus->sih, SDIOD_CORE_ID, 0);

	return bcmerror;
}


int dhd_bus_init(dhd_pub_t *dhdp)
{
	dhd_bus_t *bus = dhdp->bus;
	uint timeout;
	uint retries = 0;
	uint8 ready, enable;
	int err;//, ret = 0
	uint8 saveclk;

	ASSERT(bus->dhd);
	if (!bus->dhd)
		return 0;

	/* Make sure backplane clock is on, needed to generate F2 interrupt */
	dhdsdio_clkctl(bus, CLK_AVAIL, FALSE);
	if (bus->clkstate != CLK_AVAIL) 
		goto exit;

	/* Force clocks on backplane to be sure F2 interrupt propagates */
	saveclk = bcmsdh_cfg_read(bus->sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, &err);
	if (!err) 
	{
		bcmsdh_cfg_write(bus->sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR,
		                 (saveclk | SBSDIO_FORCE_HT), &err);
	}
	if (err) 
		goto exit;

	/* Enable function 2 (frame transfers) */
	W_SDREG((SDPCM_PROT_VERSION << SMB_DATA_VERSION_SHIFT), &bus->regs->tosbmailboxdata, retries);
	enable = (SDIO_FUNC_ENABLE_1 | SDIO_FUNC_ENABLE_2);
	bcmsdh_cfg_write(bus->sdh, SDIO_FUN_0, SDIO_CCCR_IOEx, enable, NULL);//enable in this function..dgl

	/* Give the dongle some time to do its thing and set IOR2 */
	ready = 0;
	timeout = 0;
	while (ready != enable)
	{
		ready = bcmsdh_cfg_read(bus->sdh, SDIO_FUN_0, SDIO_CCCR_IORx, NULL);
		if (timeout++ > DHD_WAIT_F2RDY)
			break;
		DelayUs(1000);
	}

	/* If F2 successfully enabled, set core and enable interrupts */
	if (ready == enable) 
	{
		/* Make sure we're talking to the core. */
		bus->regs = si_setcore(bus->sih, SDIOD_CORE_ID, 0);
		ASSERT(bus->regs != NULL);

		/* Set up the interrupt mask and enable interrupts */
		bus->hostintmask = HOSTINTMASK;
		W_SDREG(bus->hostintmask, &bus->regs->hostintmask, retries);

		bcmsdh_cfg_write(bus->sdh, SDIO_FUN_1, SBSDIO_WATERMARK, (uint8)watermark, &err);

		/* Set bus state according to enable result */
		dhdp->busstate = DHD_BUS_DATA;

		bus->intdis = FALSE;
	}
	else 
	{
		/* Disable F2 again */
		enable = SDIO_FUNC_ENABLE_1;
		bcmsdh_cfg_write(bus->sdh, SDIO_FUN_0, SDIO_CCCR_IOEx, enable, NULL);
	}

	/* Restore previous clock setting */
	bcmsdh_cfg_write(bus->sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, saveclk, &err);


	/* If we didn't come up, turn off backplane clock */
	if (dhdp->busstate != DHD_BUS_DATA)
		dhdsdio_clkctl(bus, CLK_NONE, FALSE);

exit:
	
	return err;
}




static bool dhdsdio_chipmatch(uint16 chipid)
{
	if (chipid == BCM4330_CHIP_ID)//rk903
		return TRUE;
	if (chipid == BCM43362_CHIP_ID)//rk901
		return TRUE;

	return FALSE;
}


void * dhdsdio_probe(uint16 venid, uint16 devid, void *regsva, void *sdh)
{
	int ret;
	dhd_bus_t *bus;
	
	/* Init global variables at run-time, not as part of the declaration.
	 * This is required to support init/de-init of the driver. Initialization
	 * of globals as part of the declaration results in non-deterministic
	 * behavior since the value of the globals may be different on the
	 * first time that the driver is initialized vs subsequent initializations.
	 */
	dhd_alignctl = TRUE;
	sd1idle = TRUE;
	retrydata = FALSE;
	bcmdhd_bus = NULL;//DGL ADD.for R_SDREG and W_SDREG Marco.
	forcealign = TRUE;

	/* We make assumptions about address window mappings */
	if((unsigned int)regsva != SI_ENUM_BASE)
		return NULL;

	/* Allocate private bus interface state */
    memset(&g_dhd_bus_t, 0, sizeof(dhd_bus_t));
    bus = &g_dhd_bus_t;
	if (bus == NULL)
		goto fail;

	bcmdhd_bus = bus;
	
	bus->sdh = sdh;
	//bus->cl_devid = (uint16)devid;
	bus->tx_seq = SDPCM_SEQUENCE_WRAP - 1;

	/* dgl:init the bus member varibles. attempt to attach to the dongle */
	if (!(dhdsdio_probe_attach(bus, sdh, regsva, devid))) //dgl regsva == SI_ENUM_BASE
	{
		goto fail;
	}

	/* Attach to the dhd/OS/network interface */
	bus->dhd = dhd_attach(bus, SDPCM_RESERVE);
	if (!(bus->dhd)) 
		goto fail;
    G_dhd = bus->dhd;

	/* Allocate buffers */
	if (!(dhdsdio_probe_malloc(bus, sdh))) 
		goto fail;

	if (!(dhdsdio_probe_init(bus, sdh))) 
		goto fail;

	if (bus->intr) 
	{
		/* Register interrupt callback, but mask it (not operational yet). */
		ret = bcmsdh_intr_reg(sdh, dhdsdio_isr, bus);
	 	if (ret) 
	 	{
	 		goto fail;
		}
	} 

	/* if firmware path present try to download and bring up bus */
	ret = dhd_bus_start(bus->dhd);
	if (ret == BCME_NOTUP)
		goto fail;

	return bus;

fail:
	//rk903 dhdsdio_release(bus, osh);
	return NULL;
}

static bool dhdsdio_probe_attach(struct dhd_bus *bus, void *sdh, void *regsva, uint16 devid)
{
	int err = 0;
	uint8 clkctl = 0;
	uint32 val;

	bus->alp_only = TRUE;

	/* Return the window to backplane enumeration space for core access */
	err = dhdsdio_set_siaddr_window(bus, SI_ENUM_BASE);
	if (err) 
		goto fail;

	/* Force PLL off until si_attach() programs PLL control regs */
	bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, DHD_INIT_CLKCTL1, &err);
	if (!err)
	{
		clkctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, &err);
	}

	if (err || ((clkctl & ~SBSDIO_AVBITS) != DHD_INIT_CLKCTL1)) 
		goto fail;


	/* Make sure ALP is available before trying to read CIS */
	SPINWAIT(((clkctl = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, NULL)),
	          !SBSDIO_ALPAV(clkctl)), PMU_MAX_TRANSITION_DLY);

	/* Now request ALP be put on the bus */
	bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, DHD_INIT_CLKCTL2, &err);
	DelayUs(65);

    
    /* si_attach() will provide an SI handle and scan the backplane */
	bus->sih = si_attach((uint)devid, regsva, DHD_BUS, sdh, &bus->vars, &bus->varsz);
	if (!bus->sih) 
	{
		BCMDEBUG("si_attach failed! \n");
		goto fail;
	}

	if (!dhdsdio_chipmatch((uint16)bus->sih->chip)) 
	{
		BCMDEBUG("dhdsdio_chipmatch failed! \n");
		goto fail;
	}

	si_sdiod_drive_strength_init(bus->sih, dhd_sdiod_drive_strength);//6ºÁ°²

	/* Get info on the ARM and SOCRAM cores... */
	if (!DHD_NOPMU(bus)) 
	{
		if ((si_setcore(bus->sih, ARM7S_CORE_ID, 0)) ||
		    (si_setcore(bus->sih, ARMCM3_CORE_ID, 0))) 
		{
			bus->armrev = si_corerev(bus->sih);
		} 
		else
		{
			goto fail;
		}
		bus->orig_ramsize = si_socram_size(bus->sih);
		if (!bus->orig_ramsize) 
		{
			goto fail;
		}
		bus->ramsize = bus->orig_ramsize;//dgl:240k
		//printf("ramsize = 0x%x\n", bus->ramsize);
	}

	/* ...but normally deal with the SDPCMDEV core */
	bus->regs = si_setcore(bus->sih, SDIOD_CORE_ID, 0);
	if (!bus->regs) 
	{
		BCMDEBUG("failed to find SDIODEV core!\n");
		goto fail;
	}
	
	bus->sdpcmrev = si_corerev(bus->sih);
	//printf("sdpcmrev=%d\n", bus->sdpcmrev);//901 10
	
	/* Set core control so an SDIO reset does a backplane reset */
	val = bcmsdh_reg_read(NULL, (uint)(&bus->regs->corecontrol), sizeof(bus->regs->corecontrol));
	err = bcmsdh_reg_write(NULL, (uint)(&bus->regs->corecontrol), sizeof(bus->regs->corecontrol), val|CC_BPRESEN);
	if (err)
		goto fail;
	
	bus->rxint_mode = SDIO_DEVICE_HMB_RXINT;

	/* Locate an appropriately-aligned portion of hdrbuf */
	bus->rxhdr = (uint8 *)ROUNDUP((uint)&bus->hdrbuf[0], DHD_SDALIGN);

	/* Set the poll and/or interrupt flags */
	bus->intr = dhd_intr;
	bus->poll = FALSE;

	return TRUE;

fail:
	bus->sih = NULL;

	return FALSE;
}


static bool dhdsdio_probe_malloc(dhd_bus_t *bus, void *sdh)
{
	if (bus->dhd->maxctl) 
	{
		bus->rxblen = ROUNDUP((bus->dhd->maxctl + SDPCM_HDRLEN), ALIGNMENT) + DHD_SDALIGN;
	    bus->rxbuf = get_global_cmdbuf(DATA_RXBUF_HEAD);
	}
	
	return TRUE;
}


static bool dhdsdio_probe_init(dhd_bus_t *bus, void *sdh)
{
	int32 fnum;

	/* Disable F2 to clear any intermediate frame state on the dongle */
	bcmsdh_cfg_write(sdh, SDIO_FUN_0, SDIO_CCCR_IOEx, SDIO_FUNC_ENABLE_1, NULL);

	bus->dhd->busstate = DHD_BUS_DOWN;
	bus->sleeping = FALSE;
	bus->rxflow = FALSE;
	bus->prev_rxlim_hit = 0;


	/* Done with backplane-dependent accesses, can drop clock... */
	bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, 0, NULL);

	/* ...and initialize clock/power states */
	bus->clkstate = CLK_SDONLY;
	bus->idletime = (int32)dhd_idletime;
	bus->idleclock = DHD_IDLE_ACTIVE;

	/* Query the SD clock speed */
	if (bcmsdh_iovar_op(sdh, "sd_divisor", NULL, 0, 
	                    &bus->sd_divisor, sizeof(int32), FALSE) != BCME_OK) 
	{
		bus->sd_divisor = -1;
	}

	/* Query the SD bus mode */
	if (bcmsdh_iovar_op(sdh, "sd_mode", NULL, 0,
	                    &bus->sd_mode, sizeof(int32), FALSE) != BCME_OK) 
	{
		bus->sd_mode = -1;
	} 

	/* Query the F2 block size, set roundup accordingly */
	fnum = 2;
	if (bcmsdh_iovar_op(sdh, "sd_blocksize", &fnum, sizeof(int32),
	                    &bus->blocksize, sizeof(int32), FALSE) != BCME_OK) 
	{
		bus->blocksize = 0;
	}
	bus->roundup = MIN(max_roundup, bus->blocksize);

	/* Query if bus module supports packet chaining, default to use if supported */
	if (bcmsdh_iovar_op(sdh, "sd_rxchain", NULL, 0,
	                    &bus->sd_rxchain, sizeof(int32), FALSE) != BCME_OK) 
	{
		bus->sd_rxchain = FALSE;
	} 
	bus->use_rxchain = (bool)bus->sd_rxchain;

	return TRUE;
}


/*
 * Compute load start address of data by load base address.
 */
uint32 get_load_start_addr(uint32 base_addr)
{
#if 1
    uint8  flash_bf[512] = {0};
    uint32 load_start = 0;
	FIRMWARE_INFO_T *fw_module_info;
	
    //read the firmware start relative address frist.
    MDReadData(SysDiskID, CodeLogicAddress, 512, flash_bf);
    fw_module_info = (FIRMWARE_INFO_T *)flash_bf;
    load_start = fw_module_info->LoadStartBase;

	//load 0x01000750 0x01000000 0x00000200
	//printf("load 0x%08x 0x%08x 0x%08x\n", base_addr, load_start, CodeLogicAddress);

	return (base_addr - load_start + CodeLogicAddress);
#else
	return 1;
#endif
}

static int dhdsdio_download_code_file(struct dhd_bus *bus, int fwsize)
{
	int bcmerror = -1;
	int offset;
	uint len;
	uint8 *memblock = NULL, *memptr;
	uint32 page_addr;
	uint32 firmware_base = (uint32)(&Load$$WIFI_FIRMWARE_ONE$$Base);

	if (fwsize <= 0)
		goto err;

	memptr = memblock = sys_calloc(MEMBLOCK + DHD_SDALIGN);
	if (memblock == NULL) 
		goto err;
	if ((uint)memblock % DHD_SDALIGN)
		memptr += (DHD_SDALIGN - ((uint)memblock % DHD_SDALIGN));

	/* Download image */
	#if 1
	page_addr = get_load_start_addr(firmware_base);
	#else
	page_addr = 0x00000F04;
	#endif
	printf("0x%08x\n", page_addr);
	offset = 0;
	while (offset < fwsize)
	{
		len = min((fwsize - offset), MEMBLOCK);
		ModuleOverlayLoadData(page_addr + offset, (uint32)memptr, len);
		
		bcmerror = dhdsdio_membytes(bus, TRUE, offset, memptr, len);
		if (bcmerror) 
			goto err;
		offset += len;
	}
err:
	if (memblock)
		sys_free(memblock);

	return bcmerror;
}

static int
dhdsdio_download_nvram(struct dhd_bus *bus)
{
	int bcmerror = -1;
	uint len;
	uint32 page_addr;
	char * memblock = NULL;
	char *bufp;
	int pnvpathfmsiz = bus->nv_path_fmsize;
	uint32 nvram_base = (uint32)(&Load$$WIFI_FIRMWARE_TWO$$Base);

	if (pnvpathfmsiz <= 0)
		return 0;

	memblock = sys_malloc(MAX_NVRAMBUF_SIZE); 
	if (memblock == NULL) 
		return 0;

	/* Download variables */
#if 1
	page_addr = get_load_start_addr(nvram_base);
#else
	page_addr = 0x0003B01C;
#endif
	printf("0x%08x\n", page_addr);
	ModuleOverlayLoadData(page_addr, (uint32)memblock, pnvpathfmsiz);
	len = pnvpathfmsiz;

	if (len > 0 && len < MAX_NVRAMBUF_SIZE) 
	{
		bufp = (char *)memblock;
		bufp[len] = 0;
		len = process_nvram_vars(bufp, len);
		if (len % 4) 
		{
			len += 4 - (len % 4);
		}
		bufp += len;
		*bufp++ = 0;
		if (len)
		{
			bcmerror = dhdsdio_downloadvars(bus, memblock, len + 1);
		}
	}
	else 
	{
		bcmerror = BCME_SDIO_ERROR;
	}

	if (memblock)
		sys_free(memblock);

	return bcmerror;
}


static int _dhdsdio_download_firmware(struct dhd_bus *bus)
{
	int bcmerror = -1;

	bool dlok = FALSE;	/* download firmware succeeded */

	/* Out immediately if no image to download */
	if ((bus->fw_path_fmsize <= 0) || (bus->nv_path_fmsize <= 0)) 
	{
		return 0;
	}
	/* Keep arm in reset */
	if (dhdsdio_download_state(bus, TRUE)) 
	{
		goto err;
	}

	if ((NULL == wifi_info.pfirmware) || (NULL == wifi_info.pnvram))
	{
		BCMDEBUG("No firmeware.\n");
		goto err;
	}
	
	/* External image takes precedence if specified */
	if (dhdsdio_download_code_file(bus, bus->fw_path_fmsize)) 
	{
		goto err;
	}
	else 
	{
		/*successful download firmware..*/
		dlok = TRUE;
	}
	
	if (!dlok)
	{
		goto err;
	}

	/* EXAMPLE: nvram_array */
	/* If a valid nvram_arry is specified as above, it can be passed down to dongle */
	/* dhd_bus_set_nvram_params(bus, (char *)&nvram_array); */

	/* External nvram takes precedence if specified */
	if (dhdsdio_download_nvram(bus)) 
	{
		goto err;
	}

	/* Take arm out of reset */
	if (dhdsdio_download_state(bus, FALSE)) 
	{
		goto err;
	}

	bcmerror = 0;

err:
	return bcmerror;
}


bool dhdsdio_download_firmware(struct dhd_bus *bus)
{
	bool ret;

	/* Download the firmware */
	//dgl add interrupt disable
	//....

	bus->fw_path_fmsize = sizeof(wifi_fw);
	bus->nv_path_fmsize = sizeof(wifi_nvram);
	
	dhdsdio_clkctl(bus, CLK_AVAIL, FALSE);
	
	ret = _dhdsdio_download_firmware(bus) == 0;

	dhdsdio_clkctl(bus, CLK_SDONLY, FALSE);
	//dgl add interrupt enable
	//....

	return ret;
}



