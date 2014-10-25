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
 * $Id: bcmsdh_sdmmc_linux.c 310626 2012-01-25 05:47:55Z $
 */
/*
 * inluce header files.
 */
#include  "rk903.h"

extern struct sdio_func sdio_func_0;

extern int bcmsdh_probe(void);
/*
* implement functions
*/
int bcmsdh_sdmmc_probe(struct sdio_func *func)
{
	int ret = 0;

	if (func->num == 1) //io fun1
	{
		sdio_func_0.num = 0;
		sdio_func_0.card = func->card;
		gInstance->func[0] = &sdio_func_0;
		if(func->device == 0x4) //device is device id 
		{   
			return -1;/* 4318, dgl;do not support 4318 so exit*/
		}
	}

	gInstance->func[func->num] = func;

	if (func->num == 2)
	{
		ret = bcmsdh_probe();
	}

	return ret;
}


int sdio_function_init(void * pCard)
{
	SDM_CARD_INFO * card;
	int i, error = 0;

	if (!pCard)
	{
		return -ENOMEM;
	}
	card = (SDM_CARD_INFO *)pCard;
	gInstance = get_instance();

	for (i=0; i<card->sdio_funcs; i++)
	{
		error = bcmsdh_sdmmc_probe(card->sdio_func[i]);
		if (error)
			break;
	}

	return error;
}

