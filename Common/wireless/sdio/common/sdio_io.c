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

#if 0
#define ASSERT(x)	if (x == 0) while(1)

#define SWAP16(x)                      ((((x)&0x00FF)<<8)| \
                                        (((x)&0xFF00)>>8))

#define SWAP32(x)                      ((((x)&0xFF000000UL)>>24)| \
                                        (((x)&0x00FF0000UL)>> 8)| \
                                        (((x)&0x0000FF00UL)<< 8)| \
                                        (((x)&0x000000FFUL)<<24))
#endif
/*----------------------------------- Typedefs -------------------------------*/


/*-------------------------- Forward Declarations ----------------------------*/



/* ------------------------------- Globals ---------------------------------- */



/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/


/**
 *	sdio_enable_func - enables a SDIO function for usage
 *	@func: SDIO function to enable
 *
 *	Powers up and activates a SDIO function so that register
 *	access is possible.
 */
int sdio_enable_func(struct sdio_func *func)
{
	int ret, i;
	unsigned char reg;

	if (!func || !func->card)
		return -EPERM;

	ret = mmc_io_rw_direct(func->card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_IOEx, 0, &reg);
	if (ret)
		goto err;

	reg |= 1 << func->num;

	ret = mmc_io_rw_direct(func->card, SDIO_W, SDIO_FUN_0, SDIO_CCCR_IOEx, reg, NULL);
	if (ret)
		goto err;

	i = 0;
	while (1) 
	{
		ret = mmc_io_rw_direct(func->card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_IORx, 0, &reg);
		if (ret)
			goto err;
		if (reg & (1 << func->num))
			break;
			
		ret = -ETIME;
		if (i++ >= 200)
			break;
		
		DelayUs(1000);
	}

	return 0;

err:
	return ret;
}

/**
 *	sdio_disable_func - disable a SDIO function
 *	@func: SDIO function to disable
 *
 *	Powers down and deactivates a SDIO function. Register access
 *	to this function will fail until the function is reenabled.
 */
int sdio_disable_func(struct sdio_func *func)
{
	int ret;
	unsigned char reg;

	if (!func || !func->card)
		return -EPERM;

	ret = mmc_io_rw_direct(func->card, SDIO_R, SDIO_FUN_0, SDIO_CCCR_IOEx, 0, &reg);
	if (ret)
		goto err;

	reg &= ~(1 << func->num);

	ret = mmc_io_rw_direct(func->card, SDIO_W, SDIO_FUN_0, SDIO_CCCR_IOEx, reg, NULL);
	if (ret)
		goto err;

	return 0;

err:
	return -EIO;
}

/**
 *	sdio_readb - read a single byte from a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a single byte from the address space of a given SDIO
 *	function. If there is a problem reading the address, 0xff
 *	is returned and @err_ret will contain the error code.
 */
uint8 sdio_readb(struct sdio_func *func, unsigned int addr, int *err_ret)
{
	int ret;
	uint8 val;

	if (!func)
		return 0xFF;

	if (err_ret)
		*err_ret = 0;

	ret = mmc_io_rw_direct(func->card, SDIO_R, func->num, addr, 0, &val);
	if (ret) {
		if (err_ret)
			*err_ret = ret;
		return 0xFF;
	}

	return val;
}


/**
 *	sdio_readb_ext - read a single byte from a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *	@in: value to add to argument
 *
 *	Reads a single byte from the address space of a given SDIO
 *	function. If there is a problem reading the address, 0xff
 *	is returned and @err_ret will contain the error code.
 */
unsigned char sdio_readb_ext(struct sdio_func *func, unsigned int addr,
	int *err_ret, unsigned in)
{
	int ret;
	unsigned char val;

	if (!func)
		return 0xFF;

	if (err_ret)
		*err_ret = 0;

	ret = mmc_io_rw_direct(func->card, SDIO_R, func->num, addr, (uint8)in, &val);
	if (ret) 
	{
		if (err_ret)
			*err_ret = ret;
		return 0xFF;
	}

	return val;
}


/**
 *	sdio_writeb - write a single byte to a SDIO function
 *	@func: SDIO function to access
 *	@b: byte to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a single byte to the address space of a given SDIO
 *	function. @err_ret will contain the status of the actual
 *	transfer.
 */
void sdio_writeb(struct sdio_func *func, uint8 b, unsigned int addr, int *err_ret)
{
	int ret;

	if (!func)
		return;    

	ret = mmc_io_rw_direct(func->card, SDIO_W, func->num, addr, b, NULL);
	if (err_ret)
		*err_ret = ret;
}


/**
 *	sdio_writeb_readb - write and read a byte from SDIO function
 *	@func: SDIO function to access
 *	@write_byte: byte to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Performs a RAW (Read after Write) operation as defined by SDIO spec -
 *	single byte is written to address space of a given SDIO function and
 *	response is read back from the same address, both using single request.
 *	If there is a problem with the operation, 0xff is returned and
 *	@err_ret will contain the error code.
 */
uint8 sdio_writeb_readb(struct sdio_func *func, uint8 write_byte, unsigned int addr, int *err_ret)
{
	int ret;
	uint8 val;

	ret = mmc_io_rw_direct(func->card, SDIO_W, func->num, addr, write_byte, &val);
	if (err_ret)
		*err_ret = ret;
	if (ret)
		val = 0xff;

	return val;
}

/**
 *	sdio_readw - read a 16 bit integer from a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a 16 bit integer from the address space of a given SDIO
 *	function. If there is a problem reading the address, 0xffff
 *	is returned and @err_ret will contain the error code.
 */
uint16 sdio_readw(struct sdio_func *func, unsigned int addr, int *err_ret)
{
	int ret;

	if (err_ret)
		*err_ret = 0;

	ret = sdio_memcpy_fromio(func, func->tmpbuf, addr, 2);
	if (ret) {
		if (err_ret)
			*err_ret = ret;
		return 0xFFFF;
	}

	return (*(uint16 *)func->tmpbuf);
}

/**
 *	sdio_writew - write a 16 bit integer to a SDIO function
 *	@func: SDIO function to access
 *	@b: integer to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a 16 bit integer to the address space of a given SDIO
 *	function. @err_ret will contain the status of the actual
 *	transfer.
 */
void sdio_writew(struct sdio_func *func, uint16 b, unsigned int addr, int *err_ret)
{
	int ret;

	*(uint16 *)func->tmpbuf = b;

	ret = sdio_memcpy_toio(func, addr, func->tmpbuf, 2);
	if (err_ret)
		*err_ret = ret;
}


/**
 *	sdio_readl - read a 32 bit integer from a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a 32 bit integer from the address space of a given SDIO
 *	function. If there is a problem reading the address,
 *	0xffffffff is returned and @err_ret will contain the error
 *	code.
 */
uint32 sdio_readl(struct sdio_func *func, unsigned int addr, int *err_ret)
{
	int ret;

	if (err_ret)
		*err_ret = 0;

	ret = sdio_memcpy_fromio(func, func->tmpbuf, addr, 4);
	if (ret) 
	{
		if (err_ret)
			*err_ret = ret;
		return 0xFFFFFFFF;
	}

	return (*(uint32*)func->tmpbuf);
}

/**
 *	sdio_writel - write a 32 bit integer to a SDIO function
 *	@func: SDIO function to access
 *	@b: integer to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a 32 bit integer to the address space of a given SDIO
 *	function. @err_ret will contain the status of the actual
 *	transfer.
 */
void sdio_writel(struct sdio_func *func, uint32 b, unsigned int addr, int *err_ret)
{
	int ret;

	*(uint32 *)func->tmpbuf = b;

	ret = sdio_memcpy_toio(func, addr, func->tmpbuf, 4);
	if (err_ret)
		*err_ret = ret;
}


/**
 *	sdio_f0_readb - read a single byte from SDIO function 0
 *	@func: an SDIO function of the card
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a single byte from the address space of SDIO function 0.
 *	If there is a problem reading the address, 0xff is returned
 *	and @err_ret will contain the error code.
 */
unsigned char sdio_f0_readb(struct sdio_func *func, unsigned int addr, int *err_ret)
{
	int ret;
	unsigned char val;

	if (err_ret)
		*err_ret = 0;

	ret = mmc_io_rw_direct(func->card, SDIO_R, SDIO_FUN_0, addr, 0, &val);
	if (ret) 
	{
		if (err_ret)
			*err_ret = ret;
		return 0xFF;
	}

	return val;
}


/**
 *	sdio_f0_writeb - write a single byte to SDIO function 0
 *	@func: an SDIO function of the card
 *	@b: byte to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a single byte to the address space of SDIO function 0.
 *	@err_ret will contain the status of the actual transfer.
 *
 *	Only writes to the vendor specific CCCR registers (0xF0 -
 *	0xFF) are permiited; @err_ret will be set to -EINVAL for *
 *	writes outside this range.
 */
void sdio_f0_writeb(struct sdio_func *func, unsigned char b, unsigned int addr, int *err_ret)
{
	int ret;

	if (addr < 0xF0 || addr > 0xFF) 
	{
		if (err_ret)
			*err_ret = -EINVAL;
		return;
	}

	ret = mmc_io_rw_direct(func->card, SDIO_W, SDIO_FUN_0, addr, b, NULL);
	if (err_ret)
		*err_ret = ret;
}
