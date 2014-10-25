/*
 * Misc utility routines for accessing PMU corerev specific features
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
 * $Id: hndpmu.c,v 1.228.2.56 2011-02-11 22:49:07 $
 */

#include  "rk903.h"

/* SDIO Pad drive strength to select value mappings.
 * The last strength value in each table must be 0 (the tri-state value).
 */
typedef struct 
{
	uint8 strength;			/* Pad Drive Strength in mA */
	uint8 sel;			/* Chip-specific select value */
} sdiod_drive_str_t;



/* SDIO Drive Strength to sel value table for PMU Rev 11 (1.8v) */
static const sdiod_drive_str_t sdiod_drive_strength_tab4_1v8[] = 
{
	{32, 0x6},
	{26, 0x7},
	{22, 0x4},
	{16, 0x5},
	{12, 0x2},
	{8, 0x3},
	{4, 0x0},
	{0, 0x1} 
};

/* SDIO Drive Strength to sel value table for PMU Rev 13 (1.8v) */
static const sdiod_drive_str_t sdiod_drive_strength_tab5_1v8[] = {
	{6, 0x7},
	{5, 0x6},
	{4, 0x5},
	{3, 0x4},
	{2, 0x2},
	{1, 0x1},
	{0, 0x0} };

#define SDIOD_DRVSTR_KEY(chip, pmu)	(((chip) << 16) | (pmu))

void si_sdiod_drive_strength_init(si_t *sih, uint32 drivestrength)
{
	chipcregs_t *cc;
	uint origidx, intr_val = 0;
	sdiod_drive_str_t *str_tab = NULL;
	uint32 str_mask = 0;
	uint32 str_shift = 0;

	if (!(sih->cccaps & CC_CAP_PMU))
		return;

	/* Remember original core before switch to chipc */
	cc = (chipcregs_t *) si_switch_core(sih, CC_CORE_ID, &origidx, &intr_val);

	switch (SDIOD_DRVSTR_KEY(sih->chip, sih->pmurev)) 
	{
		case SDIOD_DRVSTR_KEY(BCM4330_CHIP_ID, 12):
			str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab4_1v8;
			str_mask = 0x00003800;
			str_shift = 11;
			break;
			
		case SDIOD_DRVSTR_KEY(BCM43362_CHIP_ID, 13):
			str_tab = (sdiod_drive_str_t *)&sdiod_drive_strength_tab5_1v8;
			str_mask = 0x00003800;
			str_shift = 11;
			break;
		default:
			break;
	}

	if (str_tab != NULL) 
	{
		uint32 cc_data_temp;
		int i;

		/* Pick the lowest available drive strength equal or greater than the
		 * requested strength.	Drive strength of 0 requests tri-state.
		 */
		for (i = 0; drivestrength < str_tab[i].strength; i++)
			;

		if (i > 0 && drivestrength > str_tab[i].strength)
			i--;

		bcmsdh_reg_write(NULL, (uint)(&cc->chipcontrol_addr), sizeof(cc->chipcontrol_addr), 1);
		
		cc_data_temp = bcmsdh_reg_read(NULL, (uint)(&cc->chipcontrol_data), sizeof(cc->chipcontrol_data));
		cc_data_temp &= ~str_mask;
		cc_data_temp |= str_tab[i].sel << str_shift;
		bcmsdh_reg_write(NULL, (uint)(&cc->chipcontrol_data), sizeof(cc->chipcontrol_data), cc_data_temp);
	}

	/* Return to original core */
	si_restore_core(sih, origidx, intr_val);
}
