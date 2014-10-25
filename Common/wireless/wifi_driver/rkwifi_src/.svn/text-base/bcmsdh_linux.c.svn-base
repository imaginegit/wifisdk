/*
 * SDIO access interface for drivers - linux specific (pci only)
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
 * $Id: bcmsdh_linux.c 310626 2012-01-25 05:47:55Z $
 */

/**
 * @file bcmsdh_linux.c
 */
/*
 * inluce header files.
 */
#include  "rk903.h"

extern uint sd_power;	/* 0 = SD Power OFF, 1 = SD Power ON. */
extern uint sd_f2_blocksize;
extern uint sd_divisor;	/* Divisor (-1 means external clock) */

extern void * dhdsdio_probe(uint16 venid, uint16 devid, void *regsva, void *sdh);

int bcmsdh_probe(void)
{
	unsigned long regs;/* SDIO Host Controller address */
	bcmsdh_info_t *sdh;/* SDIO Host Controller handle */
	void *ch;          /* SDIO Host dhd_bus_t */
	int irq = 0;

	/* init the struct sdh and set 2 funs blocksize and enable two func. */
	sdh = bcmsdh_attach(NULL, (void **)&regs, irq);
	if (!sdh) 
		goto err;


	/* try to attach to the target device:dhd_bus_t */
	ch = dhdsdio_probe((sdh->vendevid >> 16), (sdh->vendevid & 0xFFFF), (void *)regs, sdh);
	if (ch == NULL)
		goto err;

	return 0;

	/* error handling */
err:
	bcmsdh_detach(sdh);

	return -ENODEV;
}
