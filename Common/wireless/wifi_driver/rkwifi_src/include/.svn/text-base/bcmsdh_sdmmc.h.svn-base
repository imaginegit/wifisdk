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
 * $Id: bcmsdh_sdmmc.h 277737 2011-08-16 17:54:59Z $
 */

#ifndef __BCMSDH_SDMMC_H__
#define __BCMSDH_SDMMC_H__

/* message levels */
#define SD_ERROR_LEVEL	0x0001
#define SD_TRACE_LEVEL	0x0002
#define SD_INFO_LEVEL	0x0004
#define SD_DEBUG_LEVEL	0x0008
#define SD_DATA_LEVEL	0x0010
#define SD_CTRL_LEVEL	0x0020

extern uint sd_msglevel;

#define BLOCK_SIZE_4318 64
#define BLOCK_SIZE_4328 512

/* internal return code */
#define SUCCESS	0
// #define ERROR	1

/* private bus modes */
#define SDIOH_MODE_SD4		2
#define CLIENT_INTR 		0x100	/* Get rid of this! */

struct sdioh_info 
{
	bool		client_intr_enabled;	/* interrupt connnected flag */
	bool		intr_handler_valid;	/* client driver interrupt handler valid */
	sdioh_cb_fn_t	intr_handler;		/* registered interrupt handler */
	void		*intr_handler_arg;	/* argument to call interrupt handler */
	uint16		intmask;		/* Current active interrupts */

	uint 		irq;			/* Client irq */
	int 		intrcount;		/* Client interrupts */

	bool		sd_use_dma;		/* DMA on CMD53 */
	bool 		sd_blockmode;		/* sd_blockmode == FALSE => 64 Byte Cmd 53s. */
						/*  Must be on for sd_multiblock to be effective */
	bool 		use_client_ints;	/* If this is false, make sure to restore */
	int 		sd_mode;		/* SD1/SD4/SPI */
	int 		client_block_size[SDIOD_MAX_IOFUNCS];		/* Blocksize */
	uint8 		num_funcs;		/* Supported funcs on client */
	uint32 		com_cis_ptr;
	uint32 		func_cis_ptr[SDIOD_MAX_IOFUNCS];
	uint		max_dma_len;
	uint		max_dma_descriptors;	/* DMA Descriptors supported by this controller. */
};


/**************************************************************
 * Internal interfaces: bcmsdh_sdmmc.c references to per-port code
 */

/* Register mapping routines */

/* Interrupt (de)registration routines */

typedef struct _BCMSDH_SDMMC_INSTANCE 
{
	sdioh_info_t	 * sd;
	struct sdio_func * func[SDIOD_MAX_IOFUNCS];
} BCMSDH_SDMMC_INSTANCE, *PBCMSDH_SDMMC_INSTANCE;

#endif /* __BCMSDH_SDMMC_H__ */
