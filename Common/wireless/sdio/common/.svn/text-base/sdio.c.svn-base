/* Copyright (C) 2012 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: SDM.c
Desc: 

Author: dgl
Date: 12-07-03
Notes:

$Log: $
 *
 *
*/

/*-------------------------------- Includes ----------------------------------*/

#include "sdio_card.h"

/*------------------------------------ extern-------------------------------*/

/*------------------------------------ Defines -------------------------------*/

#define USER_SUPPORT_VOLTAGE     (SD_IO_OCR_27_28 | SD_IO_OCR_28_29 | SD_IO_OCR_29_30 | SD_IO_OCR_30_31 | SD_IO_OCR_31_32 | SD_IO_OCR_32_33 | SD_IO_OCR_33_34)


/*----------------------------------- Typedefs -------------------------------*/



/*-------------------------- Forward Declarations ----------------------------*/



/* ------------------------------- Globals ---------------------------------- */


/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

static int mmc_send_relative_addr(void *pCardInfo, UINT32 *rRCA)
{
	SDM_CARD_INFO*  pCard;
	int              err = SDC_SUCCESS;
	UINT32           status;

 	pCard = (SDM_CARD_INFO*)pCardInfo;
    err = SDIOC_SendCmd(pCard->SDCPort, (SD_SEND_RELATIVE_ADDR | SD_NODATA_OP | SD_RSP_R6 | WAIT_PREV), 0, &status);
    if (err != SDC_SUCCESS)
    {
        return err;
    }
    pCard->rca = (uint16)(status >> 16);//RCA:Relative Card Address register,set by arm when host send cmd3;

	if (rRCA)
		*rRCA = status;

    return err;
}


static int mmc_select_card(void *pCardInfo, UINT32 *select)
{
	SDM_CARD_INFO*   pCard;
	int              err = SDC_SUCCESS;
	UINT32           status;

 	pCard = (SDM_CARD_INFO*)pCardInfo;
 	
	/**************************************************/
	// make card entry into Cmd State
	// I/O State : stby => cmd By CMD7;
	/**************************************************/
    err = SDIOC_SendCmd(pCard->SDCPort, (SD_SELECT_DESELECT_CARD | SD_NODATA_OP | SD_RSP_R1B | WAIT_PREV), (pCard->rca << 16), &status);
    if (SDC_SUCCESS != err)
    {
        return err;
    }

	if (select)
		*select = status;

    return err;
}


static int sdio_read_cccr(void *pCardInfo)
{
	SDM_CARD_INFO*   card;
	int ret;
	int cccr_vsn;
	uint8 data = 0;
    
	card = (SDM_CARD_INFO*)pCardInfo;
	ret = mmc_io_rw_direct(pCardInfo, SDIO_R, SDIO_FUN_0, SDIO_CCCR_CCCR, 0, &data);
	if (ret)
		goto out;
	    
	cccr_vsn = data & 0x0f;//02h : CCCR/FBR defined in SDIO Version 2.00
	
	if (cccr_vsn > SDIO_CCCR_REV_1_20) 
		return -EINVAL;

	card->cccr.sdio_vsn = (data & 0xf0) >> 4;// 03h : SDIO Specification Version 2.00
	
	ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_CAPS, 0, &data);
	if (ret)
		goto out;

	if (data & SDIO_CCCR_CAP_SMB)   // Support Multiple Block Transfer CMD53
		card->cccr.multi_block = 1;
	if (data & SDIO_CCCR_CAP_LSC)   // Low-Speed Card
		card->cccr.low_speed = 1;
	if (data & SDIO_CCCR_CAP_4BLS)  // 4-bit Mode Support for Low-Speed Card
		card->cccr.wide_bus = 1;

	if (cccr_vsn >= SDIO_CCCR_REV_1_10) 
	{
		ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_POWER, 0, &data);
		if (ret)
			goto out;

		/*
		 Support Master Power Control.
		 SMPC=1: The total card power may exceed 720mW.
		         Controls of EMPC, SPS and EPS are available. 
		*/
		if (data & SDIO_POWER_SMPC)
			card->cccr.high_power = 1;
	}

	if (cccr_vsn >= SDIO_CCCR_REV_1_20) 
	{
		ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_SPEED, 0, &data);
		if (ret)
			goto out;

		if (data & SDIO_SPEED_SHS)//Support High-Speed : SHS=1: The card supports High-Speed mode...
			card->cccr.high_speed = 1;
	}

out:
	return ret;
}

/*
 * If desired, disconnect the pull-up resistor on CD/DAT[3] (pin 1)
 * of the card. This may be required on certain setups of boards,
 * controllers and embedded sdio device which do not need the card's
 * pull-up. As a result, card detection is disabled and power is saved.
 */
static int sdio_disable_cd(void *pCardInfo)
{
    SDM_CARD_INFO*   card;
	int ret;
	uint8 ctrl;
    
    card = (SDM_CARD_INFO*)pCardInfo;

	ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_IF, 0, &ctrl);
	if (ret)
		return ret;

	ctrl |= SDIO_BUS_CD_DISABLE;

	return mmc_io_rw_direct(card, SDIO_W, SDIO_FUN_0, SDIO_CCCR_IF, ctrl, NULL);
}


/*
 * Test if the card supports high-speed mode and, if so, switch to it.
 */
static int mmc_sdio_switch_hs(void *pCardInfo, int enable)
{
	SDM_CARD_INFO*   card;
	int ret;
	uint8 speed;

	card = (SDM_CARD_INFO*)pCardInfo;

	if (!card->cccr.high_speed)
		return 0;

	ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_SPEED, 0, &speed);
	if (ret)
		return ret;

	if (enable)
		speed |= SDIO_SPEED_EHS;
	else
		speed &= ~SDIO_SPEED_EHS;

	ret = mmc_io_rw_direct(card, SDIO_W, SDIO_FUN_0, SDIO_CCCR_SPEED, speed, NULL);
	if (ret)
		return ret;

	return 1;
}


/*
 * Enable SDIO/combo card's high-speed mode. Return 0/1 if [not]supported.
 */
static int sdio_enable_hs(void *card)
{
	return mmc_sdio_switch_hs(card, TRUE);
}



static int sdio_enable_wide(void *pCardInfo)
{
    SDM_CARD_INFO*   card;
	int ret;
	uint8 ctrl;
    HOST_BUS_WIDTH   wide = BUS_WIDTH_INVALID;
    
    card = (SDM_CARD_INFO*)pCardInfo;

    /*
     * whether is card internal support wide bus
     */
	if (card->cccr.low_speed && !card->cccr.wide_bus)
		return 0;

    /*
     * whether is SDC iomux config wide bus.
     */
    ret = SDC_GetBusWidth(card->SDCPort, &wide);
    if ((ret != SDC_SUCCESS) || (wide != BUS_WIDTH_4_BIT))
        return 0;

	ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_IF, 0, &ctrl);
	if (ret)
		return ret;

	ctrl |= SDIO_BUS_WIDTH_4BIT;

	ret = mmc_io_rw_direct(card, SDIO_W, SDIO_FUN_0, SDIO_CCCR_IF, ctrl, NULL);
	if (ret)
		return ret;

    /*
     * config SDC is 4 bits bus width.
     */
    ret = SDIOC_SetBusWidth(card->SDCPort, BUS_WIDTH_4_BIT);
    if (SDC_SUCCESS != ret)
        return ret;

	return 1;
}

static int sdio_enable_4bit_bus(void *card)
{
	return sdio_enable_wide(card);				 
}

/*
 * Handle the detection and initialisation of a card.
 *
 * In the case of a resume, "oldcard" will contain the card
 * we're trying to reinitialise.
 */
static int32 mmc_sdio_init_card(void *card)
{
    SDM_CARD_INFO*   pCard;
    int              err = SDC_SUCCESS;

    pCard = (SDM_CARD_INFO*)card;

	/*
	 * Sanity check the voltages that the card claims to support.
	 */
	if (pCard->ocr & 0xFF) 
		pCard->ocr &= ~0xFF;

	/*
	 * Inform the card of the voltage
	 */
	err = mmc_send_io_op_cond(card, pCard->ocr&USER_SUPPORT_VOLTAGE, NULL);
	if (err)
	    goto remove;
	    
	/*
	 * For native busses:  set card RCA and quit open drain mode.
	 */
	err = mmc_send_relative_addr(card, NULL);
	if (err)
		goto remove;

	/*
	 * Select card, as all following commands rely on that.
	 */
	err = mmc_select_card(card, NULL);
	if (err)
		goto remove;

	/*
	* Read the common registers.
	*/
	err = sdio_read_cccr(card);
	if (err)
	    goto remove;

	/*
	 * Read the common CIS tuples.
	 */
	err = sdio_read_common_cis(card);
	if (err)
		goto remove;

	/*
	 * If needed, disconnect card detection pull-up resistor.
	 */
	err = sdio_disable_cd(card);
	if (err)
		goto remove;

	/*
	 * Switch to high-speed (if supported).
	 * err == 0/1 [not]support high speed.
	 */
	err = sdio_enable_hs(card);
	if (err > 0)
		mmc_card_set_highspeed(pCard); // success to enable high speed. 
	else if (err)
		goto remove;
	
	/*
	 * Change to the card's maximum speed.
	 */
    err = SDIOC_UpdateFreq(pCard->SDCPort, SD_FPP_FREQ);//SD_FPP_FREQ SDHC_FPP_FREQ
    if (SDC_SUCCESS != err)
        goto remove;
	
    pCard->TranSpeed = SD_FPP_FREQ;
    pCard->WorkMode |= SDM_HIGH_SPEED_MODE;

	/*
	* Switch to wider bus (if supported).
	*/
	err = sdio_enable_4bit_bus(card);
	if (err > 0)
		; //success to switch 4 bits wide.
	else if (err)
		goto remove;
	
	return 0;

remove:
	return err;
}


/*
 * Allocate and initialise a new SDIO function structure.
 */
static struct sdio_func *sdio_alloc_func(void *pCard, unsigned int fn)
{
	struct sdio_func *func;

	func = get_sdio_func(fn);
	memset(func, 0, sizeof(struct sdio_func));	

	func->card = pCard;

	return func;
}


static int sdio_read_fbr(struct sdio_func *func)
{
	int ret;
	uint8 data;

 	ret = mmc_io_rw_direct(func->card, SDIO_R, SDIO_FUN_0, SDIO_FBR_BASE(func->num) + SDIO_FBR_STD_IF, 0, &data);
	if (ret)
		goto out;

	data &= 0x0f;// The SDIO Standard Function code identifies those I/O functions

	if (data == SDIO_CLASS_NONE)
		return 0;

	if (data == 0x0f) 
	{
		ret = mmc_io_rw_direct(func->card, SDIO_R, SDIO_FUN_0, SDIO_FBR_BASE(func->num) + SDIO_FBR_STD_IF_EXT, 0, &data);
		if (ret)
			goto out;
	}

	func->func_class = data;

out:
	return ret;
}

static int sdio_init_func(void *pCard, unsigned int fn)
{
	int ret;
    SDM_CARD_INFO*   card;
	struct sdio_func *func;

	if (fn > SDIOD_MAX_IOFUNCS)
		return EPERM;
		
	card = (SDM_CARD_INFO*)pCard;
    
	func = sdio_alloc_func(card, fn);
	if (!func)
		return EPERM;

	func->num = fn;

	ret = sdio_read_fbr(func);
	if (ret)
		goto fail;

	ret = sdio_read_func_cis(func);
	if (ret)
		goto fail;

	card->sdio_func[fn - 1] = func;

	return 0;

fail:
	/*
	 * It is okay to remove the function here even though we hold
	 * the host lock as we haven't registered the device yet.
	 */
	return ret;
}

/*
 * Starting point for SDIO card init.
 */
int mmc_attach_sdio(void *pCard)
{
	SDM_CARD_INFO*   card;
	int err, i, funcs;

	if (pCard == NULL)
		return -EPERM;

	card = (SDM_CARD_INFO*)pCard;

	/*
	 * Detect and init the card.
	 */
	err = mmc_sdio_init_card(pCard);
	if (err)
		goto remove;

	/*
	 * The number of functions on the card is encoded inside
	 * the ocr.
	 */
	funcs = (card->ocr & 0x70000000) >> 28;
	card->sdio_funcs = 0;

	/*
	 * Initialize (but don't add) all present functions.
	 */
	for (i = 0; i < funcs; i++, card->sdio_funcs++) 
	{
		err = sdio_init_func(pCard, i + 1);
		if (err)
			goto remove;
	}
	card->type = SDIO;
	
	return 0;
	
remove:	
	return err;
}

