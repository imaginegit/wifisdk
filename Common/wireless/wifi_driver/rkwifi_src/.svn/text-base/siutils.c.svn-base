/*
 * Misc utility routines for accessing chip-specific features
 * of the SiliconBackplane-based Broadcom chips.
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
 * $Id: siutils.c,v 1.813.2.36 2011-02-10 23:43:55 $
 */
 
#include "rk903.h"

/* local prototypes */
static si_info_t *si_doattach(si_info_t *sii, uint devid, void *regs,
                              uint bustype, void *sdh, char **vars, uint *varsz);
static bool si_buscore_prep(si_info_t *sii, uint bustype, uint devid, void *sdh);
static bool si_buscore_setup(si_info_t *sii, chipcregs_t *cc, uint bustype, uint32 savewin,
	uint *origidx, void *regs);

/*
 * Allocate a si handle.
 * devid - pci device id (used to determine chip#)
 * osh - opaque OS handle
 * regs - virtual address of initial core registers
 * bustype - pci/pcmcia/sb/sdio/etc
 * vars - pointer to a pointer area for "environment" variables
 * varsz - pointer to int to return the size of the vars
 */
si_t * si_attach(uint devid, void *regs,//regs == SI_ENUM_BASE
                       uint bustype, void *sdh, char **vars, uint *varsz)
{
	si_info_t *sii;

    sii = &g_si_info_t;
	if (si_doattach(sii, devid, regs, bustype, sdh, vars, varsz) == NULL) 
	{
		return NULL;
	}
	sii->vars = vars ? *vars : NULL;
	sii->varsz = varsz ? *varsz : 0;

	return (si_t *)sii;
}


static bool si_buscore_prep(si_info_t *sii, uint bustype, uint devid, void *sdh)
{
    int err;
    uint8 clkset;
	uint8 clkval;
	
	if (bustype != SDIO_BUS) 
		return FALSE;
	
	/* Try forcing SDIO core to do ALPAvail request only */
	clkset = SBSDIO_FORCE_HW_CLKREQ_OFF | SBSDIO_ALP_AVAIL_REQ;
	bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, clkset, &err);
	if (!err) 
	{
		/* If register supported, wait for ALPAvail and then force ALP */
		clkval = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, NULL);
		if ((clkval & ~SBSDIO_AVBITS) == clkset) 
		{
			SPINWAIT(((clkval = bcmsdh_cfg_read(sdh, SDIO_FUN_1, 
				      SBSDIO_FUNC1_CHIPCLKCSR, NULL)), !SBSDIO_ALPAV(clkval)), 
				      PMU_MAX_TRANSITION_DLY);
			
			if (!SBSDIO_ALPAV(clkval)) 
				return FALSE;
			
			clkset = SBSDIO_FORCE_HW_CLKREQ_OFF | SBSDIO_FORCE_ALP;
			bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_CHIPCLKCSR, clkset, &err);
			DelayUs(65);
		}
	}

	/* Also, disable the extra SDIO pull-ups */
	bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_SDIOPULLUP, 0, NULL);

	return TRUE;
}



static bool si_buscore_setup(si_info_t *sii, chipcregs_t *cc, uint bustype, 
	                             uint32 savewin, uint *origidx, void *regs)
{
	uint i;

	cc = si_setcoreidx(&sii->pub, SI_CC_IDX);
	ASSERT((uint)cc);

	/* get chipcommon rev */
	sii->pub.ccrev = (int)si_corerev(&sii->pub);

	/* get chipcommon chipstatus */
	if (sii->pub.ccrev >= 11)
		sii->pub.chipst = bcmsdh_reg_read(NULL, (uint)(&cc->chipstatus), sizeof(cc->chipstatus));

	/* get chipcommon capabilites */
	sii->pub.cccaps = bcmsdh_reg_read(NULL, (uint)(&cc->capabilities), sizeof(cc->capabilities));
	/* get chipcommon extended capabilities */

	if (sii->pub.ccrev >= 35)
		sii->pub.cccaps_ext = bcmsdh_reg_read(NULL, (uint)(&cc->capabilities_ext), sizeof(cc->capabilities_ext));

	/* get pmu rev and caps */
	if (sii->pub.cccaps & CC_CAP_PMU) 
	{
		sii->pub.pmucaps = bcmsdh_reg_read(NULL, (uint)(&cc->pmucapabilities), sizeof(cc->pmucapabilities));
		sii->pub.pmurev = sii->pub.pmucaps & PCAP_REV_MASK;
	}

	/* figure out bus/orignal core idx */
	sii->pub.buscoretype = NODEV_CORE_ID;
	sii->pub.buscorerev = (uint)NOREV;
	sii->pub.buscoreidx = BADIDX;

	for (i = 0; i < sii->numcores; i++) 
	{
		uint cid, crev;

		si_setcoreidx(&sii->pub, i);
		cid = si_coreid(&sii->pub);
		crev = si_corerev(&sii->pub);

		/* Display cores found */
		if ((bustype == SDIO_BUS) && (cid == SDIOD_CORE_ID)) 
		{
			sii->pub.buscorerev = crev;
			sii->pub.buscoretype = cid;
			sii->pub.buscoreidx = i;
			//printf("buscoretype=0x%X\n", sii->pub.buscoretype);
		}

		/* find the core idx before entering this func. */
		if ((savewin && (savewin == sii->coresba[i])) || (regs == sii->regs[i]))
		{
			*origidx = i;
		}
	}

	/* Make sure any on-chip ARM is off (in case strapping is wrong), or downloaded code was
	 * already running.
	 */
	if (bustype == SDIO_BUS) 
	{
		if (si_setcore(&sii->pub, ARM7S_CORE_ID, 0) || si_setcore(&sii->pub, ARMCM3_CORE_ID, 0))
		{
			si_core_disable(&sii->pub, 0);
		}
	}

	/* return to the original core */
	si_setcoreidx(&sii->pub, *origidx);

	return TRUE;
}


static si_info_t * si_doattach(si_info_t *sii, uint devid, void *regs,
                       uint bustype, void *sdh, char **vars, uint *varsz)
{
	struct si_pub *sih = &sii->pub;
	uint32 w, savewin;
	chipcregs_t *cc;
	uint origidx;

	memset((unsigned char*)sii, 0, sizeof(si_info_t));

	savewin = 0;

	sih->buscoreidx = BADIDX;

	sii->curmap = regs;//SI_ENUM_BASE
	sii->sdh = sdh;

	/* find Chipcommon address */
	cc = (chipcregs_t *)sii->curmap;//SI_ENUM_BASE

	sih->bustype = bustype;

	/* bus/core/clk setup for register access */
	if (!si_buscore_prep(sii, bustype, devid, sdh))
		return NULL;

	/* ChipID recognition.
	 *   We assume we can read chipid at offset 0 from the regs arg.
	 *   If we add other chiptypes (or if we need to support old sdio hosts w/o chipcommon),
	 *   some way of recognizing them needs to be added here.
	 */
	w = bcmsdh_reg_read(NULL, (uint)(&cc->chipid), sizeof(cc->chipid));
	
	sih->socitype = (w & CID_TYPE_MASK) >> CID_TYPE_SHIFT;
	/* Might as wll fill in chip id rev & pkg */
	sih->chip = w & CID_ID_MASK;
	sih->chiprev = (w & CID_REV_MASK) >> CID_REV_SHIFT;
	sih->chippkg = (w & CID_PKG_MASK) >> CID_PKG_SHIFT;
	
	if (sih->chip == BCM4322_CHIP_ID && 
		(((sih->chipst & CST4322_SPROM_OTP_SEL_MASK) >> CST4322_SPROM_OTP_SEL_SHIFT)
		   == (CST4322_OTP_PRESENT | CST4322_SPROM_PRESENT))) 
	{
		BCMDEBUG("Invalid setting: both SPROM and OTP strapped.\n");
		return NULL;
	}

	if ((sih->chip == BCM4329_CHIP_ID) && (sih->chiprev == 0) &&
		(sih->chippkg != BCM4329_289PIN_PKG_ID)) 
	{
		sih->chippkg = BCM4329_182PIN_PKG_ID;
	}

	sih->issim = IS_SIM(sih->chippkg);

	/* scan for cores */
	if (sii->pub.socitype == SOCI_SB) 
	{
		BCMDEBUG("Found chip type SB (0x%08x)\n", w);
		//sb_scan(&sii->pub, regs, devid);
	} 
	else if (sii->pub.socitype == SOCI_AI) 
	{
		BCMDEBUG("Found chip type AI (0x%08x)\n", w);
		/* pass chipc address instead of original core base */
		ai_scan(&sii->pub, (void *)cc, devid);
	} 
	else if (sii->pub.socitype == SOCI_UBUS) 
	{
		BCMDEBUG("Found chip type UBUS (0x%08x), chip id = 0x%4x\n", w, sih->chip);
		/* pass chipc address instead of original core base */
		//ub_scan(&sii->pub, (void *)(uintptr)cc, devid);
	} 
	else 
	{
		BCMDEBUG("Found chip of unknown type (0x%08x)\n", w);
		return NULL;
	}
	printf("sii->numcores = %d\n", sii->numcores);
	/* no cores found, bail out */
	if (sii->numcores == 0) 
	{
		BCMDEBUG("si_doattach: could not find any cores\n");
		return NULL;
	}
	/* bus/core/clk setup */
	origidx = SI_CC_IDX;
	if (!si_buscore_setup(sii, cc, bustype, savewin, &origidx, regs)) 
	{
		BCMDEBUG("si_doattach: si_buscore_setup failed\n");
		goto exit;
	}
	//printf("origidx=%d ccrev=0x%x chip=0x%x\n", origidx, sii->pub.ccrev, sih->chip);
	//origidx=0 ccrev=0x27 chip=0xa962(901)	
	
	if (sii->pub.ccrev >= 20) 
	{
		cc = (chipcregs_t *)si_setcore(sih, CC_CORE_ID, 0);
		ASSERT(cc != NULL);
		bcmsdh_reg_write(NULL, (uint)(&cc->gpiopullup), sizeof(cc->gpiopullup), 0);
		bcmsdh_reg_write(NULL, (uint)(&cc->gpiopulldown), sizeof(cc->gpiopulldown), 0);
		si_setcoreidx(sih, origidx);
	}

	return (sii);

exit:
	return NULL;
}

uint si_coreid(si_t *sih)
{
	si_info_t *sii;

	sii = SI_INFO(sih);
	return sii->coreid[sii->curidx];
}

uint si_coreidx(si_t *sih)
{
	si_info_t *sii;

	sii = SI_INFO(sih);
	return sii->curidx;
}

uint si_corerev(si_t *sih)
{
	if (sih->socitype == SOCI_SB)
		return 0;//dgl sb_corerev(sih);
	else if (sih->socitype == SOCI_AI)
		return ai_corerev(sih);
	else if (sih->socitype == SOCI_UBUS)
		return 0;//dgl  ub_corerev(sih);

	return 0;
}

/* return index of coreid or BADIDX if not found */
uint si_findcoreidx(si_t *sih, uint coreid, uint coreunit)
{
	si_info_t *sii;
	uint found;
	uint i;

	sii = SI_INFO(sih);

	found = 0;

	for (i = 0; i < sii->numcores; i++)
	{
		if (sii->coreid[i] == coreid) 
		{
			if (found == coreunit)
				return (i);
			found++;
		}
	}

	return (BADIDX);
}


/*
 * This function changes logical "focus" to the indicated core;
 * must be called with interrupts off.
 * Moreover, callers should keep interrupts off during switching out of and back to d11 core
 */
void * si_setcore(si_t *sih, uint coreid, uint coreunit)
{
	uint idx;

	idx = si_findcoreidx(sih, coreid, coreunit);
	if (!GOODIDX(idx))
		return (NULL);

	if (sih->socitype == SOCI_SB)
		;//dgl return sb_setcoreidx(sih, idx);
	else if (sih->socitype == SOCI_AI)
		return ai_setcoreidx(sih, idx);
	else if (sih->socitype == SOCI_UBUS)
		;//dgl return ub_setcoreidx(sih, idx);

	return NULL;
}


void * si_setcoreidx(si_t *sih, uint coreidx)
{
	if (sih->socitype == SOCI_SB)
		;//return sb_setcoreidx(sih, coreidx);
	else if (sih->socitype == SOCI_AI)
		return ai_setcoreidx(sih, coreidx);
	else if (sih->socitype == SOCI_UBUS)
		;//return ub_setcoreidx(sih, coreidx);

	return NULL;
}


/* Turn off interrupt as required by sb_setcore, before switch core */
void * si_switch_core(si_t *sih, uint coreid, uint *origidx, uint *intr_val)
{
	void *cc;
	si_info_t *sii;

	sii = SI_INFO(sih);
	INTR_OFF(sii, *intr_val);
	*origidx = sii->curidx;
	cc = si_setcore(sih, coreid, 0);
	ASSERT(cc != NULL);

	return cc;
}


/* restore coreidx and restore interrupt */
void si_restore_core(si_t *sih, uint coreid, uint intr_val)
{
	si_info_t *sii;

	sii = SI_INFO(sih);
	si_setcoreidx(sih, coreid);
	INTR_RESTORE(sii, intr_val);
}


bool si_iscoreup(si_t *sih)
{
	if (sih->socitype == SOCI_SB)
		;//return sb_iscoreup(sih);
	else if (sih->socitype == SOCI_AI)
		return ai_iscoreup(sih);
	else if (sih->socitype == SOCI_UBUS)
		;//return ub_iscoreup(sih);

	return FALSE;
}


void si_core_disable(si_t *sih, uint32 bits)
{
	if (sih->socitype == SOCI_SB)
		;//sb_core_disable(sih, bits);
	else if (sih->socitype == SOCI_AI)
		ai_core_disable(sih, bits);
	else if (sih->socitype == SOCI_UBUS)
		;//ub_core_disable(sih, bits);
}


void si_core_reset(si_t *sih, uint32 bits, uint32 resetbits)
{
	if (sih->socitype == SOCI_SB)
		;//sb_core_reset(sih, bits, resetbits);
	else if (sih->socitype == SOCI_AI)
		ai_core_reset(sih, bits, resetbits);
	else if (sih->socitype == SOCI_UBUS)
		;//ub_core_reset(sih, bits, resetbits);
}


/* Return the size of the specified SOCRAM bank */
static uint socram_banksize(si_info_t *sii, sbsocramregs_t *regs, uint8 index, uint8 mem_type)
{
	uint banksize, bankinfo;
	uint bankidx = index | (mem_type << SOCRAM_BANKIDX_MEMTYPE_SHIFT);

	ASSERT(mem_type <= SOCRAM_MEMTYPE_DEVRAM);

	bcmsdh_reg_write(NULL, (uint)(&regs->bankidx), sizeof(regs->bankidx), bankidx);
	bankinfo = bcmsdh_reg_read(NULL, (uint)(&regs->bankinfo), sizeof(regs->bankinfo));
	banksize = SOCRAM_BANKINFO_SZBASE * ((bankinfo & SOCRAM_BANKINFO_SZMASK) + 1);
	return banksize;
}



/* Return the RAM size of the SOCRAM core */
uint32 si_socram_size(si_t *sih)
{
	si_info_t *sii;
	uint origidx;
	uint intr_val = 0;

	sbsocramregs_t *regs;
	bool wasup;
	uint corerev;
	uint32 coreinfo;
	uint memsize = 0;

	sii = SI_INFO(sih);

	/* Block ints and save current core */
	INTR_OFF(sii, intr_val);
	origidx = si_coreidx(sih);

	/* Switch to SOCRAM core */
	regs = si_setcore(sih, SOCRAM_CORE_ID, 0);
	if (!regs)
		goto done;

	/* Get info for determining size */
	wasup = si_iscoreup(sih);
	if (!wasup)
		si_core_reset(sih, 0, 0);
	
	corerev = si_corerev(sih);
	coreinfo = bcmsdh_reg_read(NULL, (uint)(&regs->coreinfo), sizeof(regs->coreinfo));

	/* Calculate size from coreinfo based on rev */
	if (corerev == 0)
	{
		memsize = 1 << (16 + (coreinfo & SRCI_MS0_MASK));
	}
	else if (corerev < 3) 
	{
		memsize = 1 << (SR_BSZ_BASE + (coreinfo & SRCI_SRBSZ_MASK));
		memsize *= (coreinfo & SRCI_SRNB_MASK) >> SRCI_SRNB_SHIFT;
	} 
	else if ((corerev <= 7) || (corerev == 12)) 
	{
		uint nb = (coreinfo & SRCI_SRNB_MASK) >> SRCI_SRNB_SHIFT;
		uint bsz = (coreinfo & SRCI_SRBSZ_MASK);
		uint lss = (coreinfo & SRCI_LSS_MASK) >> SRCI_LSS_SHIFT;
		if (lss != 0)
			nb --;
		memsize = nb * (1 << (bsz + SR_BSZ_BASE));
		if (lss != 0)
			memsize += (1 << ((lss - 1) + SR_BSZ_BASE));
	} 
	else 
	{
		uint8 i;
		uint nb = (coreinfo & SRCI_SRNB_MASK) >> SRCI_SRNB_SHIFT;
		for (i = 0; i < nb; i++)
			memsize += socram_banksize(sii, regs, i, SOCRAM_MEMTYPE_RAM);
	}

	/* Return to previous state and core */
	if (!wasup)
		si_core_disable(sih, 0);
	si_setcoreidx(sih, origidx);

done:
	INTR_RESTORE(sii, intr_val);

	return memsize;
}
