/* Copyright (C) 2012 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: sdio_func.h
Desc: 

Author: 
Date: 12-07-03
Notes:

$Log: $
 *
 *
*/

#ifndef MMC_SDIO_FUNC_H
#define MMC_SDIO_FUNC_H

typedef void (sdio_irq_handler_t)(struct sdio_func *);

/*
 * SDIO function CIS tuple (unknown to the core)
 */
struct sdio_func_tuple 
{
	struct sdio_func_tuple *next;
	uint8 code;
	uint8 size;
	uint8 data[1];
};


/*
 * SDIO function devices
 */
struct sdio_func 
{
	void *      card;  /*SDM_CARD_INFO the card this device belongs to */
	sdio_irq_handler_t * irq_handler;	/* IRQ callback */
	uint		num;		/* function number */

	uint8		func_class;		/* standard interface class */
	uint16		vendor;		/* vendor id */
	uint16		device;		/* device id */

	uint		max_blksize;	/* maximum block size */
	uint		cur_blksize;	/* current block size */

	uint		enable_timeout;	/* max enable timeout in msec */

	uint		state;		    /* function state */
#define SDIO_STATE_PRESENT	(1<<0)		/* present in sysfs */

	uint8		tmpbuf[4];	/* DMA:able scratch buffer */

	uint		num_info;	/* number of info strings */
	const char	**info;		/* info strings */

	struct sdio_func_tuple *tuples;

	void *driver_data;   /* struct if_sdio_card */
};

#define sdio_set_drvdata(f,d)	(func->driver_data = d)

/*
 * SDIO I/O operations
 */
extern int sdio_enable_func(struct sdio_func *func);
extern int sdio_disable_func(struct sdio_func *func);

extern int sdio_set_block_size(struct sdio_func *func, unsigned blksz);

extern int process_sdio_pending_irqs(void);
extern int sdio_claim_irq(struct sdio_func *func, sdio_irq_handler_t * handler);
extern int sdio_release_irq(struct sdio_func *func);

extern uint8 sdio_readb(struct sdio_func *func, uint addr, int *err_ret);
extern uint8 sdio_readb_ext(struct sdio_func *func,uint addr, int *err_ret, unsigned in);
extern uint16 sdio_readw(struct sdio_func *func, uint addr, int *err_ret);
extern uint32 sdio_readl(struct sdio_func *func, uint addr, int *err_ret);

extern int sdio_memcpy_fromio(struct sdio_func *func, void *dst, uint addr, int count);
extern int sdio_readsb(struct sdio_func *func, void *dst, uint addr, int count);

extern void sdio_writeb(struct sdio_func *func, uint8 b, uint addr, int *err_ret);
extern void sdio_writew(struct sdio_func *func, uint16 b, uint addr, int *err_ret);
extern void sdio_writel(struct sdio_func *func, uint32 b, uint addr, int *err_ret);

extern uint8 sdio_writeb_readb(struct sdio_func *func, uint8 write_byte, uint addr, int *err_ret);

extern int sdio_memcpy_toio(struct sdio_func *func, uint addr, void *src, int count);
extern int sdio_writesb(struct sdio_func *func, uint addr, void *src, int count);

extern uint8 sdio_f0_readb(struct sdio_func *func, uint addr, int *err_ret);
extern void sdio_f0_writeb(struct sdio_func *func, uint8 b, uint addr, int *err_ret);


#endif
