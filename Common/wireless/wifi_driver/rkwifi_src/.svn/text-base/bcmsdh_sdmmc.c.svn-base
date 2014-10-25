/*
 * BCMSDH Function Driver for the native SDIO/MMC driver in the Linux Kernel
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
 * $Id: bcmsdh_sdmmc.c 309549 2012-01-20 01:13:15Z $
 */
#include    "rk903.h"

extern PBCMSDH_SDMMC_INSTANCE gInstance;


uint sd_f2_blocksize = 512;		/* Default blocksize */


void IRQHandler(struct sdio_func *func);
void IRQHandlerF2(struct sdio_func *func);

int sdioh_sdmmc_get_cisaddr(sdioh_info_t *sd, uint32 regaddr);
int sdioh_sdmmc_card_regread(sdioh_info_t *sd, int func, uint32 regaddr, int regsize, uint32 *data);

static int sdioh_sdmmc_card_enablefuncs(sdioh_info_t *sd)
{
	int err_ret;
	uint32 fbraddr;
	uint8 func;

	/* Get the Card's common CIS address */
	sd->com_cis_ptr = sdioh_sdmmc_get_cisaddr(sd, SDIO_CCCR_CIS);
	sd->func_cis_ptr[0] = sd->com_cis_ptr;//901=0x00001070

	/* Get the Card's function CIS (for each function) */
	for (fbraddr = SDIOD_FBR_STARTADDR, func = 1; 
	     func <= sd->num_funcs; func++, fbraddr += SDIOD_FBR_SIZE) 
	{
		sd->func_cis_ptr[func] = sdioh_sdmmc_get_cisaddr(sd, SDIOD_FBR_CISPTR_0 + fbraddr);
		//printf("func_cis_ptr[%d]=0x%08x\n", func, sd->func_cis_ptr[func]);
		//901 func_cis_ptr[1]=0x00001000
		//901 func_cis_ptr[2]=0x00001038
	}

	/* Enable Function 1 */
	err_ret = sdio_enable_func(gInstance->func[1]);
	if (err_ret) 
	{
		BCMDEBUG("bcmsdh_sdmmc: Failed to enable F1 Err: 0x%08x", err_ret);
	}

	return FALSE;
}

/*
 *	Public entry points & extern's
 */
extern sdioh_info_t * sdioh_attach(void *bar0, uint irq)
{
	sdioh_info_t *sd;
	int err_ret;

	if (gInstance == NULL) 
		return NULL;

    memset(&g_sdioh_info_t, 0, sizeof(sdioh_info_t));
    sd = &g_sdioh_info_t;
	if (sd == NULL) 
		return NULL;

	sd->num_funcs = 2;
	sd->sd_blockmode = TRUE;
	sd->use_client_ints = TRUE;
	sd->client_block_size[0] = 64;
	
	sd->client_block_size[1] = 64;
	err_ret = sdio_set_block_size(gInstance->func[1], 64);
	if (err_ret) 
	{
		BCMDEBUG("bcmsdh_sdmmc: Failed to set F1 blocksize\n");
	}

	if (gInstance->func[2]) 
	{
		sd->client_block_size[2] = sd_f2_blocksize;
		err_ret = sdio_set_block_size(gInstance->func[2], sd_f2_blocksize);
		if (err_ret) 
		{
			BCMDEBUG("bcmsdh_sdmmc: Failed to set F2 blocksize\n");
		}
	}

	sdioh_sdmmc_card_enablefuncs(sd);
	
	gInstance->sd = sd;

	return sd;
}


extern SDIOH_API_RC sdioh_detach(sdioh_info_t *sd)
{
	if (sd) 
	{
		/* Disable Function 2 */
		sdio_disable_func(gInstance->func[2]);

		/* Disable Function 1 */
		if (gInstance->func[1])
		{
			sdio_disable_func(gInstance->func[1]);
		}

		gInstance->func[1] = NULL;
		gInstance->func[2] = NULL;
	}
	return SDIOH_API_RC_SUCCESS;
}


/* Configure callback to client when we recieve client interrupt */
SDIOH_API_RC sdioh_interrupt_register(sdioh_info_t *sd, sdioh_cb_fn_t fn, void *argh)
{
	if (fn == NULL) 
	{
		return SDIOH_API_RC_FAIL;
	}
	sd->intr_handler = fn;//dhdsdio_isr
	sd->intr_handler_arg = argh;
	sd->intr_handler_valid = TRUE;

	/* register and unmask irq */
	if (gInstance->func[2])
	{
		sdio_claim_irq(gInstance->func[2], IRQHandlerF2);
	}

	if (gInstance->func[1]) 
	{
		sdio_claim_irq(gInstance->func[1], IRQHandler);
	}

	return SDIOH_API_RC_SUCCESS;
}

extern SDIOH_API_RC sdioh_interrupt_deregister(sdioh_info_t *sd)
{
	if (gInstance->func[1]) 
	{
		/* register and unmask irq */
		sdio_release_irq(gInstance->func[1]);
	}

	if (gInstance->func[2]) 
	{
		/* Claim host controller F2 */
		sdio_release_irq(gInstance->func[2]);
		/* Release host controller F2 */
	}

	sd->intr_handler_valid = FALSE;
	sd->intr_handler = NULL;
	sd->intr_handler_arg = NULL;
	return SDIOH_API_RC_SUCCESS;
}

static int sdioh_sdmmc_get_cisaddr(sdioh_info_t *sd, uint32 regaddr)
{
	/* read 24 bits and return valid 17 bit addr */
	int i;
	uint32 scratch, regdata;
	uint8 *ptr = (uint8 *)&scratch;
	
	for (i = 0; i < 3; i++) 
	{
		if ((sdioh_sdmmc_card_regread (sd, 0, regaddr, 1, &regdata)) != SUCCESS)
			BCMDEBUG("Can't read!\n");

		*ptr++ = (uint8) regdata;
		regaddr++;
	}

	/* Only the lower 17-bits are valid */
	scratch &= 0x0001FFFF;
	
	return (scratch);
}


/* Read client card reg */
int sdioh_sdmmc_card_regread(sdioh_info_t *sd, int func, uint32 regaddr, int regsize, uint32 *data)
{
	if ((func == 0) || (regsize == 1)) 
	{
		uint8 temp = 0;

		sdioh_request_byte(sd, SDIOH_READ, func, regaddr, &temp);
		*data = temp;
		*data &= 0xff;
	} 
	else 
	{
		sdioh_request_word(sd, 0, SDIOH_READ, func, regaddr, data, regsize);
		if (regsize == 2)
			*data &= 0xffff;
	}

	return SUCCESS;
}



