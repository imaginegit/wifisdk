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
 * $Id: aiutils.c,v 1.26.2.1 2010-03-09 18:41:21 $
 */


#include "rk903.h"


static uint32 get_erom_ent(si_t *sih, uint32 **eromptr, uint32 mask, uint32 match)
{
	uint32 ent;
	uint inv = 0, nom = 0;

	while (TRUE) 
	{
		ent = R_REG(*eromptr);
		
		(*eromptr)++;

		if (mask == 0)
			break;

		if ((ent & ER_VALID) == 0) 
		{
			inv++;
			continue;
		}

		if (ent == (ER_END | ER_VALID))//end flag.dgl
			break;

		if ((ent & mask) == match)
			break;

		nom++;
	}

	return ent;
}

static uint32
get_asd(si_t *sih, uint32 **eromptr, uint sp, uint ad, uint st, uint32 *addrl, uint32 *addrh,
        uint32 *sizel, uint32 *sizeh)
{
	uint32 asd, sz, szd;

	asd = get_erom_ent(sih, eromptr, ER_VALID, ER_VALID);
	if (((asd & ER_TAG1) != ER_ADD) ||
	    (((asd & AD_SP_MASK) >> AD_SP_SHIFT) != sp) ||
	    ((asd & AD_ST_MASK) != st)) 
	{
		(*eromptr)--;
		return 0;
	}
	
	*addrl = asd & AD_ADDR_MASK;
	if (asd & AD_AG32)
	{
		*addrh = get_erom_ent(sih, eromptr, 0, 0);
	}
	else
	{
		*addrh = 0;
	}
	
	*sizeh = 0;
	sz = asd & AD_SZ_MASK;
	if (sz == AD_SZ_SZD) 
	{
		szd = get_erom_ent(sih, eromptr, 0, 0);
		*sizel = szd & SD_SZ_MASK;
		if (szd & SD_SG32)
		{
			*sizeh = get_erom_ent(sih, eromptr, 0, 0);
		}
	} 
	else
	{
		*sizel = AD_SZ_BASE << (sz >> AD_SZ_SHIFT);
	}

	return asd;
}

void ai_scan(si_t *sih, void *regs, uint devid)//regs == SI_ENUM_BASE/0x18000000
{
	si_info_t *sii = SI_INFO(sih);//dgl warn: the frist member parameter of sii is sih(struct si_pub pub)
	chipcregs_t *cc = (chipcregs_t *)regs;
	uint32 erombase, *eromptr, *eromlim;

	erombase = R_REG(&cc->eromptr);

	switch (sih->bustype) 
	{
		case SDIO_BUS:
			eromptr = (uint32 *)erombase;
			break;

		//case SI_BUS:
		//case PCI_BUS:
		//case SPI_BUS:
		//case PCMCIA_BUS:
		default:
			return;
	}
	eromlim = eromptr + (ER_REMAPCONTROL / sizeof(uint32));

	while (eromptr < eromlim) 
	{
		uint32 cia, cib, cid, mfg, crev, nmw, nsw, nmp, nsp;
		uint32 mpd, asd, addrl, addrh, sizel, sizeh;
		uint32 *base;
		uint i, j, idx;
		bool br;

		br = FALSE;

		
		cia = get_erom_ent(sih, &eromptr, ER_TAG, ER_CI);
		if (cia == (ER_END | ER_VALID)) 
		{
			return;
		}
		base = eromptr - 1;
		
		cib = get_erom_ent(sih, &eromptr, 0, 0);
		if ((cib & ER_TAG) != ER_CI) 
			goto error;

		cid =  (cia & CIA_CID_MASK) >> CIA_CID_SHIFT;
		mfg =  (cia & CIA_MFG_MASK) >> CIA_MFG_SHIFT;
		crev = (cib & CIB_REV_MASK) >> CIB_REV_SHIFT;
		nmw =  (cib & CIB_NMW_MASK) >> CIB_NMW_SHIFT;
		nsw =  (cib & CIB_NSW_MASK) >> CIB_NSW_SHIFT;
		nmp =  (cib & CIB_NMP_MASK) >> CIB_NMP_SHIFT;
		nsp =  (cib & CIB_NSP_MASK) >> CIB_NSP_SHIFT;

		MINDEBUG("Found component 0x%04x/0x%04x rev %d at erom addr 0x%p, with nmw = %d, "
		         "nsw = %d, nmp = %d & nsp = %d\n",
		         mfg, cid, crev, base, nmw, nsw, nmp, nsp);

		if (((mfg == MFGID_ARM) && (cid == DEF_AI_COMP)) || (nsp == 0))
			continue;
		
		if ((nmw + nsw == 0)) 
		{
			if (cid == OOB_ROUTER_CORE_ID) 
			{
				asd = get_asd(sih, &eromptr, 0, 0, AD_ST_SLAVE, &addrl, &addrh, &sizel, &sizeh);
				if (asd != 0) 
				{
					sii->oob_router = addrl;
				}
			}
			continue;
		}

		idx = sii->numcores;

		sii->cia[idx] = cia;
		sii->cib[idx] = cib;
		sii->coreid[idx] = cid;

		for (i = 0; i < nmp; i++) 
		{
			mpd = get_erom_ent(sih, &eromptr, ER_VALID, ER_VALID);
			if ((mpd & ER_TAG) != ER_MP) 
			{
				MINDEBUG("Not enough MP entries for component 0x%x\n", cid);
				goto error;
			}
		}

		asd = get_asd(sih, &eromptr, 0, 0, AD_ST_SLAVE, &addrl, &addrh, &sizel, &sizeh);
		if (asd == 0) 
		{
			asd = get_asd(sih, &eromptr, 0, 0, AD_ST_BRIDGE, &addrl, &addrh, &sizel, &sizeh);
			if (asd != 0)
				br = TRUE;
			else
			{
				if ((addrh != 0) || (sizeh != 0) || (sizel != SI_CORE_SIZE)) 
				{
					MINDEBUG("First Slave ASD for core 0x%04x malformed (0x%08x)\n", cid, asd);
					goto error;
				}
			}
		}
		sii->coresba[idx] = addrl;
		sii->coresba_size[idx] = sizel;
		
		j = 1;
		do 
		{
			asd = get_asd(sih, &eromptr, 0, j, AD_ST_SLAVE, &addrl, &addrh, &sizel, &sizeh);
			if ((asd != 0) && (j == 1) && (sizel == SI_CORE_SIZE)) 
			{
				sii->coresba2[idx] = addrl;
				sii->coresba2_size[idx] = sizel;
			}
			j++;
		} while (asd != 0);

		
		for (i = 1; i < nsp; i++) 
		{
			j = 0;
			do
			{
				asd = get_asd(sih, &eromptr, i, j++, AD_ST_SLAVE, &addrl, &addrh, &sizel, &sizeh);
			} while (asd != 0);
			if (j == 0) 
			{
				MINDEBUG(" SP %d has no address descriptors\n", i);
				goto error;
			}
		}

		
		for (i = 0; i < nmw; i++) 
		{
			asd = get_asd(sih, &eromptr, i, 0, AD_ST_MWRAP, &addrl, &addrh, &sizel, &sizeh);
			if (asd == 0) 
			{
				MINDEBUG("Missing descriptor for MW %d\n", i);
				goto error;
			}
			if ((sizeh != 0) || (sizel != SI_CORE_SIZE)) 
			{
				MINDEBUG("Master wrapper %d is not 4KB\n", i);
				goto error;
			}
			if (i == 0)
			{
				sii->wrapba[idx] = addrl;
			}
		}

		for (i = 0; i < nsw; i++) 
		{
			uint fwp = (nsp == 1) ? 0 : 1;
			asd = get_asd(sih, &eromptr, fwp + i, 0, AD_ST_SWRAP, &addrl, &addrh, &sizel, &sizeh);
			if (asd == 0) 
			{
				MINDEBUG("Missing descriptor for SW %d\n", i);
				goto error;
			}
			if ((sizeh != 0) || (sizel != SI_CORE_SIZE)) 
			{
				MINDEBUG("Slave wrapper %d is not 4KB\n", i);
				goto error;
			}
			if ((nmw == 0) && (i == 0))
			{
				sii->wrapba[idx] = addrl;
			}
		}

		if (br)
			continue;
		printf("core:%d id=0x%3x\n", idx, sii->coreid[idx]);
		sii->numcores++;
	}

	MINDEBUG("Reached end of erom without finding END");

error:
	sii->numcores = 0;
	return;
}


void * ai_setcoreidx(si_t *sih, uint coreidx)
{
	si_info_t *sii = SI_INFO(sih);
	uint32 addr = sii->coresba[coreidx];
	uint32 wrap = sii->wrapba[coreidx];
	void *regs;

	if (coreidx >= sii->numcores)
		return (NULL);
	
	switch (sih->bustype) 
	{
		case SDIO_BUS:
			regs = (void *)(addr);
			sii->curwrap = (void *)(wrap);
			break;

		case SI_BUS:
		case SPI_BUS:
		case PCMCIA_BUS:
		default:
			regs = NULL;
			break;
	}

	sii->curmap = regs;
	sii->curidx = coreidx;

	return regs;
}


int ai_numaddrspaces(si_t *sih)
{
	return 2;
}


uint32 ai_addrspace(si_t *sih, uint asidx)
{
	si_info_t *sii;
	uint cidx;

	sii = SI_INFO(sih);
	cidx = sii->curidx;

	if (asidx == 0)
		return sii->coresba[cidx];
	else if (asidx == 1)
		return sii->coresba2[cidx];
	else 
	{
		MINDEBUG("Need to parse the erom again to find addr space %d\n", asidx);
		return 0;
	}
}


uint32 ai_addrspacesize(si_t *sih, uint asidx)
{
	si_info_t *sii;
	uint cidx;

	sii = SI_INFO(sih);
	cidx = sii->curidx;

	if (asidx == 0)
		return sii->coresba_size[cidx];
	else if (asidx == 1)
		return sii->coresba2_size[cidx];
	else 
	{
		MINDEBUG("Need to parse the erom again to find addr space %d\n", asidx);
		return 0;
	}
}

uint
ai_flag(si_t *sih)
{
	si_info_t *sii;
	aidmp_t *ai;

	sii = SI_INFO(sih);
	ai = sii->curwrap;

	return (R_REG(&ai->oobselouta30) & 0x1f);
}

void
ai_setint(si_t *sih, int siflag)
{
}

uint
ai_wrap_reg(si_t *sih, uint32 offset, uint32 mask, uint32 val)
{
	si_info_t *sii = SI_INFO(sih);
	uint32 *map = (uint32 *) sii->curwrap;

	if (mask || val) 
	{
		uint32 w = R_REG(map+(offset/4));
		w &= ~mask;
		w |= val;
		W_REG(map+(offset/4), val);
	}

	return (R_REG(map+(offset/4)));
}

uint ai_corevendor(si_t *sih)
{
	si_info_t *sii;
	uint32 cia;

	sii = SI_INFO(sih);
	cia = sii->cia[sii->curidx];
	return ((cia & CIA_MFG_MASK) >> CIA_MFG_SHIFT);
}

uint ai_corerev(si_t *sih)
{
	si_info_t *sii;
	uint32 cib;

	sii = SI_INFO(sih);
	cib = sii->cib[sii->curidx];
	return ((cib & CIB_REV_MASK) >> CIB_REV_SHIFT);
}

bool ai_iscoreup(si_t *sih)
{
	si_info_t *sii;
	aidmp_t *ai;

	sii = SI_INFO(sih);
	ai = sii->curwrap;

	return (((R_REG(&ai->ioctrl) & (SICF_FGC | SICF_CLOCK_EN)) == SICF_CLOCK_EN) &&
	        ((R_REG(&ai->resetctrl) & AIRC_RESET) == 0));
}


uint ai_corereg(si_t *sih, uint coreidx, uint regoff, uint mask, uint val)
{
	uint origidx = 0;
	uint32 *r = NULL;
	uint w;
	uint intr_val = 0;
	bool fast = FALSE;
	si_info_t *sii;

	sii = SI_INFO(sih);

	ASSERT(GOODIDX(coreidx));
	ASSERT(regoff < SI_CORE_SIZE);
	ASSERT((val & ~mask) == 0);

	if (coreidx >= SI_MAXCORES)
		return 0;

	if (sih->bustype == SI_BUS) 
	{
		fast = TRUE;
	} 
	else if (sih->bustype == PCI_BUS) 
	{
		fast = TRUE;
	}

	if (!fast) 
	{
		INTR_OFF(sii, intr_val);
		origidx = si_coreidx(&sii->pub);
		r = (uint32*) ((uint8*) ai_setcoreidx(&sii->pub, coreidx) + regoff);
	}
	ASSERT(r != NULL);

	if (mask || val) 
	{
		w = (R_REG(r) & ~mask) | val;
		W_REG(r, w);
	}
	
	w = R_REG(r);

	if (!fast) 
	{
		
		if (origidx != coreidx)
			ai_setcoreidx(&sii->pub, origidx);

		INTR_RESTORE(sii, intr_val);
	}

	return (w);
}

void ai_core_disable(si_t *sih, uint32 bits)
{
	si_info_t *sii;
	volatile uint32 dummy;
	aidmp_t *ai;

	sii = SI_INFO(sih);

	ASSERT(GOODREGS(sii->curwrap));
	ai = sii->curwrap;

	if (R_REG(&ai->resetctrl) & AIRC_RESET)
		return;

	W_REG(&ai->ioctrl, bits);
	dummy = R_REG(&ai->ioctrl);
	DelayUs(10);

	W_REG(&ai->resetctrl, AIRC_RESET);
	DelayUs(1);
}


void ai_core_reset(si_t *sih, uint32 bits, uint32 resetbits)
{
	si_info_t *sii;
	aidmp_t *ai;
	volatile uint32 dummy;

	sii = SI_INFO(sih);
	ASSERT(GOODREGS(sii->curwrap));
	ai = sii->curwrap;

	ai_core_disable(sih, (bits | resetbits));

	W_REG(&ai->ioctrl, (bits | SICF_FGC | SICF_CLOCK_EN));
	dummy = R_REG(&ai->ioctrl);
	W_REG(&ai->resetctrl, 0);
	DelayUs(1);

	W_REG(&ai->ioctrl, (bits | SICF_CLOCK_EN));
	dummy = R_REG(&ai->ioctrl);
	DelayUs(1);
}


void
ai_core_cflags_wo(si_t *sih, uint32 mask, uint32 val)
{
	si_info_t *sii;
	aidmp_t *ai;
	uint32 w;

	sii = SI_INFO(sih);
	ASSERT(GOODREGS(sii->curwrap));
	ai = sii->curwrap;

	ASSERT((val & ~mask) == 0);

	if (mask || val) 
	{
		w = ((R_REG(&ai->ioctrl) & ~mask) | val);
		W_REG(&ai->ioctrl, w);
	}
}

uint32
ai_core_cflags(si_t *sih, uint32 mask, uint32 val)
{
	si_info_t *sii;
	aidmp_t *ai;
	uint32 w;

	sii = SI_INFO(sih);
	ASSERT(GOODREGS(sii->curwrap));
	ai = sii->curwrap;

	ASSERT((val & ~mask) == 0);

	if (mask || val) 
	{
		w = ((R_REG(&ai->ioctrl) & ~mask) | val);
		W_REG(&ai->ioctrl, w);
	}

	return R_REG(&ai->ioctrl);
}

uint32
ai_core_sflags(si_t *sih, uint32 mask, uint32 val)
{
	si_info_t *sii;
	aidmp_t *ai;
	uint32 w;

	sii = SI_INFO(sih);
	ASSERT(GOODREGS(sii->curwrap));
	ai = sii->curwrap;

	ASSERT((val & ~mask) == 0);
	ASSERT((mask & ~SISF_CORE_BITS) == 0);

	if (mask || val) 
	{
		w = ((R_REG(&ai->iostatus) & ~mask) | val);
		W_REG(&ai->iostatus, w);
	}

	return R_REG(&ai->iostatus);
}
