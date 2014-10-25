/*
 *  BCMSDH interface glue
 *  implement bcmsdh API for SDIOH driver
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
 * $Id: bcmsdh.c 300445 2011-12-03 05:37:20Z $
 */

/**
 * @file bcmsdh.c
 */

/* ****************** BCMSDH Interface Functions *************************** */

#include <rk903.h>	

//#define SDIOH_API_ACCESS_RETRY_LIMIT	2
const uint bcmsdh_msglevel = BCMSDH_ERROR_VAL;


/* Attach BCMSDH layer to SDIO Host Controller Driver
 *
 * @param osh OSL Handle.
 * @param cfghdl Configuration Handle.
 * @param regsva Virtual address of controller registers.
 * @param irq Interrupt number of SDIO controller.
 *
 * @return bcmsdh_info_t Handle to BCMSDH context.
*/
bcmsdh_info_t * bcmsdh_attach(void *cfghdl, void **regsva, uint irq)
{
	bcmsdh_info_t *bcmsdh;

    bcmsdh = &g_bcmsdh_info_t;
	if (bcmsdh == NULL) 
		return NULL;

	/* save the handler locally */
	l_bcmsdh = bcmsdh;

	/* set 2 funs blocksize and enable them */
	bcmsdh->sdioh = sdioh_attach(cfghdl, irq);//cfghdl=null irq=0, no use.
	if (bcmsdh->sdioh == NULL) 
	{
		return NULL;
	}
	bcmsdh->init_success = TRUE;

	/* Read the vendor/device ID from the CIS */
	bcmsdh->vendevid = (VENDOR_BROADCOM << 16) | 0;

	bcmsdh->regfail = FALSE;

	/* Report the BAR, to fix if needed */
	bcmsdh->sbwad = SI_ENUM_BASE;

	*regsva = (uint32 *)SI_ENUM_BASE;

	return bcmsdh;
}

int bcmsdh_detach(void *sdh)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;

	if (bcmsdh != NULL) 
	{
		if (bcmsdh->sdioh) 
		{
			sdioh_detach(bcmsdh->sdioh);
			bcmsdh->sdioh = NULL;
		}
	}

	l_bcmsdh = NULL;
	return 0;
}

int bcmsdh_intr_reg(void *sdh, bcmsdh_cb_fn_t fn, void *argh)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;
	ASSERT(bcmsdh);

	status = sdioh_interrupt_register(bcmsdh->sdioh, fn, argh);
	return (SDIOH_API_SUCCESS(status) ? 0 : BCME_ERROR);
}

int bcmsdh_intr_dereg(void *sdh)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;
	ASSERT(bcmsdh);

	status = sdioh_interrupt_deregister(bcmsdh->sdioh);
	return (SDIOH_API_SUCCESS(status) ? 0 : BCME_ERROR);
}

int bcmsdh_rwdata(void *sdh, uint rw, uint32 addr, uint8 *buf, uint nbytes)
{
	bcmsdh_info_t *bcmsdh = (bcmsdh_info_t *)sdh;
	SDIOH_API_RC status;

	ASSERT(bcmsdh);
	ASSERT(bcmsdh->init_success);
	ASSERT((addr & SBSDIO_SBWINDOW_MASK) == 0);

	addr &= SBSDIO_SB_OFT_ADDR_MASK;
	addr |= SBSDIO_SB_ACCESS_2_4B_FLAG;

	status = sdioh_request_buffer(bcmsdh->sdioh, SDIOH_DATA_PIO, SDIOH_DATA_INC,
	                              (rw ? SDIOH_WRITE : SDIOH_READ), SDIO_FUN_1,
	                              addr, 4, nbytes, buf);

	return (SDIOH_API_SUCCESS(status) ? 0 : BCME_ERROR);
}

