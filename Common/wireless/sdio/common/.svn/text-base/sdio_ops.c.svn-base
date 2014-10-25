/* Copyright (C) 2012 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: sdio_ops.c
Desc: 

Author: 
Date: 2012-07-09
Notes:

$Log: $
 *
 *
*/
/*-------------------------------- Includes ----------------------------------*/

#include "sdio_card.h"
#include "sysinclude.h"

/*------------------------------------ Defines -------------------------------*/

/*----------------------------------- Typedefs -------------------------------*/



/*-------------------------- Forward Declarations ----------------------------*/



/* ------------------------------- Globals ---------------------------------- */



/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/
void sdio_delay_ms(UINT32 ms)
{
	UINT32 us;

	us = ms * 1000;
    DelayUs(us);
}

int mmc_send_io_op_cond (void *pCardInfo, UINT32 ocr, UINT32 *rocr)
{
	SDM_CARD_INFO*   pCard;
	UINT32           i = 0;
	int              err = SDC_SUCCESS;
	uint32           status;

	pCard = (SDM_CARD_INFO*)pCardInfo;
	/**************************************************/
	// make card entery to Ready State
	// It define the longest time is one second,so in here, turn to number of cycles is 100, once is 10 ms almost.
	/**************************************************/
    
    for (i=0; i<100; i++)  
    {
        err = SDIOC_SendCmd(pCard->SDCPort, (SD_IO_SEND_OP_COND | SD_NODATA_OP | SD_RSP_R4 | WAIT_PREV), ocr, &status);
		if (err)
			break;

		/* if we're just probing, do a single pass */
		if (ocr == 0)
			break;

		/* otherwise wait until reset completes */
		if (status & R4_MEMORY_READY)
			break;

        sdio_delay_ms(10);
    }

	if (rocr)
		*rocr = status;

    return err;
}


int mmc_io_rw_direct(void *pCardInfo, int32 write, uint32 fn, uint32 addr, uint8 in, uint8 *out)
{
	SDM_CARD_INFO*   pCard;
	int              err;
	UINT32           status;
	UINT32           cmd_arg = 0;

	/* sanity check */
	if (addr & ~0x1FFFF)
		return -EINVAL;
		
 	pCard = (SDM_CARD_INFO*)pCardInfo;

	cmd_arg = write ? SD_IO_W : SD_IO_R;
	cmd_arg |= fn << 28;     // function number.
	cmd_arg |= (write && out) ? SD_IO_RAW_YES : SD_IO_RAW_NO;// read after write.
	cmd_arg |= addr << 9;    // r/w function insided address.
	cmd_arg |= in;           // if this handle is writing data,the in is the data that will be write to sdio.
 	
    err = SDIOC_SendCmd(pCard->SDCPort, (SD_IO_RW_DIRECT | SD_NODATA_OP | SD_RSP_R5 | WAIT_PREV), cmd_arg, &status);
    if (SDC_SUCCESS != err)
    {        
        return err;
    }    
	if (status & R5_ERROR)
		return -EIO;
	if (status & R5_FUNCTION_NUMBER)
		return -EINVAL;
	if (status & R5_OUT_OF_RANGE)
		return -ERANGE;

	if (out) 
		*out = status & 0xFF;
    
	return 0;
}

/**
 *	sdio_set_block_size - set the block size of an SDIO function
 *	@func: SDIO function to change
 *	@blksz: new block size or 0 to use the default.
 *
 *	The default block size is the largest supported by both the function
 *	and the host, with a maximum of 512 to ensure that arbitrarily sized
 *	data transfer use the optimal (least) number of commands.
 *
 *	A driver may call this to override the default block size set by the
 *	core. This can be used to set a block size greater than the maximum
 *	that reported by the card; it is the driver's responsibility to ensure
 *	it uses a value that the card supports.
 *
 *	Returns 0 on success, -EINVAL if the host does not support the
 *	requested block size, or -EIO (etc.) if one of the resultant FBR block
 *	size register writes failed.
 *
 */
int sdio_set_block_size(struct sdio_func *func, uint blksz)
{
	int ret;

	if (blksz == 0) 
	{
		blksz = min(func->max_blksize, 4095u);
		blksz = min(blksz, 512u);
	}

	ret = mmc_io_rw_direct(func->card, SDIO_W, SDIO_FUN_0,
							SDIO_FBR_BASE(func->num) + SDIO_FBR_BLKSIZE,
							blksz & 0xff, NULL);
	if (ret)
		return ret;
	ret = mmc_io_rw_direct(func->card, SDIO_W, SDIO_FUN_0,
							SDIO_FBR_BASE(func->num) + SDIO_FBR_BLKSIZE + 1,
							(blksz >> 8) & 0xff, NULL);
	if (ret)
		return ret;
	func->cur_blksize = blksz;
	return 0;
}

static int mmc_io_rw_extended(void *card, int write, uint fn, uint addr, int incr_addr, 
								uint8 *buf, uint blocks, uint blksz)
{
	SDM_CARD_INFO*   pCard;
	int              err;
	uint32           status;
	uint32           cmd_opcode = 0;
	uint32           cmd_arg = 0;
	int              sdioc_try = 2;

	if ((!card) || (fn > 7) || (blocks == 1 && blksz > 512) || (blocks == 0) || (blksz == 0))
		return -EINVAL;

	/* sanity check */
	if (addr & ~0x1FFFF)
		return -EINVAL;
	
 	pCard = (SDM_CARD_INFO*)card;
 	
	cmd_arg = write ? SD_IO_W : SD_IO_R;
	cmd_arg |= fn << 28;     // function number.
	cmd_arg |= incr_addr ? SD_IO_INC_ADDR : SD_IO_FIX_ADDR;// set address.
	cmd_arg |= addr << 9;    // r/w function insided address.

	if (blocks == 1 && blksz <= 512)
		cmd_arg |= (blksz == 512) ? 0 : blksz;	/* byte mode, set transfer bytes number.*/
	else
		cmd_arg |= SD_IO_BLK_MOD | blocks;		/* block mode,set block flag and block number.*/

	cmd_opcode = SD_IO_RW_EXTENDED;
	cmd_opcode |= (write ? SD_WRITE_OP : SD_READ_OP) | SD_RSP_R5 | WAIT_PREV;

	while (sdioc_try)
	{
		err = SDIOC_Request(pCard->SDCPort, cmd_opcode, cmd_arg, &status, blksz, blocks * blksz, buf);
	    if (SDC_SUCCESS == err)
	        break;
		sdioc_try--;
	}

    if (SDC_SUCCESS != err)
    {
        printf("sdc err\n");
        return err;
    }
	if (status & R5_ERROR)
	{
		return -EIO;
	}
	if (status & R5_FUNCTION_NUMBER)
	{
		return -EINVAL;
	}
	if (status & R5_OUT_OF_RANGE)
	{
		return -ERANGE;
	}	
	return SDC_SUCCESS;
}


/*
 * Calculate the maximum byte mode transfer size
 */
static uint sdio_max_byte_size(struct sdio_func *func)
{
	uint mval =	min(func->cur_blksize, func->max_blksize);

	return min(mval, 512u); /* maximum size for byte mode */
}

static int sdio_io_rw_ext_helper(struct sdio_func *func, int write, uint addr, 
										int incr_addr, uint8 *buf, uint size)
{
	SDM_CARD_INFO* card;
	uint remainder = size;
	uint max_blocks;
	int ret;

	card = (SDM_CARD_INFO*)func->card;
	
	/* Do the bulk of the transfer using block mode (if supported). */
	if (card->cccr.multi_block && (size > sdio_max_byte_size(func)))  /* maximum size for byte mode */
	{
		/* Blocks per command is limited by host count, host transfer
		 * size (we only use a single sg entry) and the maximum for
		 * IO_RW_EXTENDED of 511 blocks. */
		max_blocks = 511u;

		while (remainder > func->cur_blksize) 
		{
			uint blocks;

			blocks = remainder / func->cur_blksize;
			if (blocks > max_blocks)
				blocks = max_blocks;
			size = blocks * func->cur_blksize;

			ret = mmc_io_rw_extended(func->card, write,
                    				func->num, addr, incr_addr, buf,
                    				blocks, func->cur_blksize);
			if (ret)
			{   
				return ret;
			}
			remainder -= size;
			buf += size;
			if (incr_addr)
				addr += size;
		}
	}

	/* Write the remainder using byte mode. */
	while (remainder > 0) 
	{
		size = min(remainder, sdio_max_byte_size(func));

		ret = mmc_io_rw_extended(func->card, write, func->num, addr, incr_addr, buf, 1, size);
		if (ret)
		{
			return ret;
		}
		remainder -= size;
		buf += size;
		if (incr_addr)
			addr += size;
	}

	return 0;
}


/**
 *	sdio_memcpy_fromio - read a chunk of memory from a SDIO function
 *	@func: SDIO function to access
 *	@dst: buffer to store the data
 *	@addr: address to begin reading from
 *	@count: number of bytes to read
 *
 *	Reads from the address space of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
int sdio_memcpy_fromio(struct sdio_func *func, void *dst, uint addr, int count)
{
    return sdio_io_rw_ext_helper(func, SDIO_R, addr, 1, dst, count);
}


/**
 *	sdio_memcpy_toio - write a chunk of memory to a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to start writing to
 *	@src: buffer that contains the data to write
 *	@count: number of bytes to write
 *
 *	Writes to the address space of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
int sdio_memcpy_toio(struct sdio_func *func, uint addr, void *src, int count)
{
    return sdio_io_rw_ext_helper(func, SDIO_W, addr, 1, src, count);
}


/**
 *	sdio_readsb - read from a FIFO on a SDIO function
 *	@func: SDIO function to access
 *	@dst: buffer to store the data
 *	@addr: address of (single byte) FIFO
 *	@count: number of bytes to read
 *
 *	Reads from the specified FIFO of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
int sdio_readsb(struct sdio_func *func, void *dst, uint addr, int count)
{
    int status;

    status = sdio_io_rw_ext_helper(func, SDIO_R, addr, 0, dst, count);

    return status;
}


/**
 *	sdio_writesb - write to a FIFO of a SDIO function
 *	@func: SDIO function to access
 *	@addr: address of (single byte) FIFO
 *	@src: buffer that contains the data to write
 *	@count: number of bytes to write
 *
 *	Writes to the specified FIFO of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
int sdio_writesb(struct sdio_func *func, uint addr, void *src, int count)
{
    return sdio_io_rw_ext_helper(func, SDIO_W, addr, 0, src, count);
}

int sdio_reset(void *card)
{
	int ret;
	uint8 abort;

	/* SDIO Simplified Specification V2.0, 4.4 Reset for SDIO */

	ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_ABORT, 0, &abort);
	if (ret)
		abort = 0x08;
	else
		abort |= 0x08;

	ret = mmc_io_rw_direct(card, SDIO_W, SDIO_FUN_0, SDIO_CCCR_ABORT, abort, NULL);
	return ret;
}

