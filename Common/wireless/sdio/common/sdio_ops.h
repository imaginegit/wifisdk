/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: sdio_ops.h
Desc: 

Author: 
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/
#ifndef _MMC_SDIO_OPS_H
#define _MMC_SDIO_OPS_H

extern int mmc_send_io_op_cond (void *pCardInfo, UINT32 ocr, UINT32 *rocr);
extern int mmc_io_rw_direct(void *pCardInfo, int32 write, uint32 fn, uint32 addr, uint8 in, uint8 *out);

extern int sdio_reset(void *card);

#endif


