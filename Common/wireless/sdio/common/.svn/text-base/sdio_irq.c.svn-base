/* Copyright (C) 2012 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: sdio_irq.c
Desc: 

Author: 
Date: 12-09-01
Notes:

$Log: $
 *
 *
*/

/*
 * SDIO Classes, Interface Types, Manufacturer IDs, etc.
 */

/*-------------------------------- Includes ----------------------------------*/
#include "sdio_card.h"

/*------------------------------------ Defines -------------------------------*/


/*----------------------------------- Typedefs -------------------------------*/

/*-------------------------- Forward Declarations ----------------------------*/



/* ------------------------------- Globals ---------------------------------- */

extern struct sdio_func *wifi_sdio_func;

/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

int process_sdio_pending_irqs(void)
{
	int i, ret = 0, count;
	uint8 pending;
	struct sdio_func *func;
	SDM_CARD_INFO* card;

	card = (SDM_CARD_INFO*)SDIOM_GetCardInfo(0);
	/*
	 * Optimization, if there is only 1 function interrupt registered
	 * call irq handler directly
	 */
	func = card->sdio_single_irq;
	if (func) 
	{
		func->irq_handler(func);
		return 1;
	}

	ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_INTx, 0, &pending);
	if (ret) 
	{
		RKDEBUG("error %d reading SDIO_CCCR_INTx", ret);
		return ret;
	}

	count = 0;
	for (i = 1; i <= 7; i++) 
	{
		if (pending & (1 << i)) 
		{
			func = card->sdio_func[i - 1];
			if (!func) 
			{
				RKDEBUG("no func");
				ret = -EINVAL;
			} 
			else if (func->irq_handler) 
			{
				/* IRQHandlerF2 IRQHandler->dhdsdio_isr->dhdsdio_dpc->dhdsdio_readframes */
				func->irq_handler(func);
				count++;
			} 
			else 
			{
				RKDEBUG("func %d: pending IRQ with no handler\n", func->num);
				ret = -EINVAL;
			}
		}
	}

	if (count)
		return count;

	return ret;
}

static int sdio_card_irq_get(SDM_CARD_INFO* card)
{
	card->sdio_irqs++; 
	return 0;
}

static int sdio_card_irq_put(SDM_CARD_INFO* card)
{
	if (card->sdio_irqs < 1)
		return -EPERM;
		
	--card->sdio_irqs;	

	return 0;
}

/* If there is only 1 function registered set sdio_single_irq */
static void sdio_single_irq_set(SDM_CARD_INFO* card)
{
	struct sdio_func *func;
	int i;

	card->sdio_single_irq = NULL;
	if (card->sdio_irqs == 1) 
	{
		for (i = 0; i < card->sdio_funcs; i++) 
		{
			func = card->sdio_func[i];
			if (func && func->irq_handler) 
			{
				card->sdio_single_irq = func;
				MINDEBUG("pcard = 0x%x", (uint)card);
				MINDEBUG("card->sdio_single_irq = 0x%x", (uint)func);
				break;
			}
		}
	}
}

/**
 *	sdio_claim_irq - claim the IRQ for a SDIO function
 *	@func: SDIO function
 *	@handler: IRQ handler callback
 *
 *	Claim and activate the IRQ for the given SDIO function. The provided
 *	handler will be called when that IRQ is asserted.  The host is always
 *	claimed already when the handler is called so the handler must not
 *	call sdio_claim_host() nor sdio_release_host().
 */
int sdio_claim_irq(struct sdio_func *func, sdio_irq_handler_t * handler)
{
	int ret;
	unsigned char reg;
	unsigned char r_a_w;

	if (!func || !handler)
		return -EPERM;
		
	if (func->irq_handler) 
	{
		return -EBUSY;
	}

	ret = mmc_io_rw_direct(func->card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_IENx, 0, &reg);
	if (ret)
		return ret;

	reg |= 1 << func->num;

	reg |= 1; /* Master interrupt enable */

	r_a_w = 0;
	
	ret = mmc_io_rw_direct(func->card, SDIO_W, SDIO_FUN_0, SDIO_CCCR_IENx, reg, &r_a_w);
	if (ret)
		return ret;

	MINDEBUG("reg = 0x%02x; r_a_w = 0x%02x", reg, r_a_w);
	func->irq_handler = handler;
	MINDEBUG("func = 0x%x, func->irq_handler = 0x%x", (uint)func, (uint)func->irq_handler);
	
	ret = sdio_card_irq_get(func->card);
	if (ret)
		func->irq_handler = NULL;
	sdio_single_irq_set(func->card);

	return ret;
}

/**
 *	sdio_release_irq - release the IRQ for a SDIO function
 *	@func: SDIO function
 *
 *	Disable and release the IRQ for the given SDIO function.
 */
int sdio_release_irq(struct sdio_func *func)
{
	int ret;
	uint8 reg;

	if (!func || !func->card)
		return -EPERM;
		
	if (func->irq_handler) {
		func->irq_handler = NULL;
		sdio_card_irq_put(func->card);
		sdio_single_irq_set(func->card);
	}

	ret = mmc_io_rw_direct(func->card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_IENx, 0, &reg);
	if (ret)
		return ret;

	reg &= ~(1 << func->num);

	/* Disable master interrupt with the last function interrupt */
	if (!(reg & 0xFE))
		reg = 0;

	ret = mmc_io_rw_direct(func->card, SDIO_W, SDIO_FUN_0, SDIO_CCCR_IENx, reg, NULL);
	if (ret)
		return ret;

	return 0;
}


