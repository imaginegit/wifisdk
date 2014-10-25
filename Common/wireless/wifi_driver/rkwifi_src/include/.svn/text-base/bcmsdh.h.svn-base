/*
 * SDIO host client driver interface of Broadcom HNBU
 *     export functions to client drivers
 *     abstract OS and BUS specific details of SDIO
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
 * $Id: bcmsdh.h 300017 2011-12-01 20:30:27Z $
 */

#ifndef	_bcmsdh_h_
#define	_bcmsdh_h_

#define BCMSDH_ERROR_VAL	0x0001 /* Error */
#define BCMSDH_INFO_VAL		0x0002 /* Info */


#define SDIOH_API_ACCESS_RETRY_LIMIT	2

/**
 * BCMSDH API context
 */
struct bcmsdh_info
{
	bool	init_success;	/* underlying driver successfully attached */
	void	*sdioh;		/* handler for sdioh */
	uint32  vendevid;	/* Target Vendor and Device ID on SD bus */
	bool	regfail;	/* Save status of last reg_read/reg_write call */
	uint32	sbwad;		/* Save backplane window address */
};
/* forward declarations */
typedef struct bcmsdh_info bcmsdh_info_t;
typedef void (*bcmsdh_cb_fn_t)(void *);

/* Attach and build an interface to the underlying SD host driver.
 *  - Allocates resources (structs, arrays, mem, OS handles, etc) needed by bcmsdh.
 *  - Returns the bcmsdh handle and virtual address base for register access.
 *    The returned handle should be used in all subsequent calls, but the bcmsh
 *    implementation may maintain a single "default" handle (e.g. the first or
 *    most recent one) to enable single-instance implementations to pass NULL.
 */
extern bcmsdh_info_t *bcmsdh_attach(void *cfghdl, void **regsva, uint irq);
/* Detach - freeup resources allocated in attach */
extern int bcmsdh_detach(void *sdh);

/* Register/deregister device interrupt handler. */
extern int bcmsdh_intr_reg(void *sdh, bcmsdh_cb_fn_t fn, void *argh);
extern int bcmsdh_intr_dereg(void *sdh);


/* Register a callback to be called if and when bcmsdh detects
 * device removal. No-op in the case of non-removable/hardwired devices.
 */

/* Access SDIO address space (e.g. CCCR) using CMD52 (single-byte interface).
 *   fn:   function number
 *   addr: unmodified SDIO-space address
 *   data: data byte to write
 *   err:  pointer to error code (or NULL)
 */
extern uint8 bcmsdh_cfg_read(void *sdh, uint func, uint32 addr, int *err);
extern void bcmsdh_cfg_write(void *sdh, uint func, uint32 addr, uint8 data, int *err);

/* Synchronous access to device (client) core registers via CMD53 to F1.
 *   addr: backplane address (i.e. >= regsva from attach)
 *   size: register width in bytes (2 or 4)
 *   data: data for register write
 */
extern uint32 bcmsdh_reg_read(void *sdh, uint32 addr, uint size);
extern uint32 bcmsdh_reg_write(void *sdh, uint32 addr, uint size, uint32 data);

/* Indicate if last reg read/write failed */
extern bool bcmsdh_regfail(void *sdh);

/* Buffer transfer to/from device (client) core via cmd53.
 *   fn:       function number
 *   addr:     backplane address (i.e. >= regsva from attach)
 *   flags:    backplane width, address increment, sync/async
 *   buf:      pointer to memory data buffer
 *   nbytes:   number of bytes to transfer to/from buf
 *   pkt:      pointer to packet associated with buf (if any)
 *   complete: callback function for command completion (async only)
 *   handle:   handle for completion callback (first arg in callback)
 * Returns 0 or error code.
 * NOTE: Async operation is not currently supported.
 */
typedef void (*bcmsdh_cmplt_fn_t)(void *handle, int status, bool sync_waiting);

extern int bcmsdh_send_buf(void *sdh, uint32 addr, uint fn, uint flags, uint8 *buf, uint nbytes);
extern int bcmsdh_recv_buf(void *sdh, uint32 addr, uint fn, uint flags, uint8 *buf, uint nbytes);


/* Flags bits */
#define SDIO_REQ_4BYTE	0x1	/* Four-byte target (backplane) width (vs. two-byte) */
#define SDIO_REQ_FIXED	0x2	/* Fixed address (FIFO) (vs. incrementing address) */
#define SDIO_REQ_ASYNC	0x4	/* Async request (vs. sync request) */

/* Pending (non-error) return code */
#define BCME_PENDING	1

/* Read/write to memory block (F1, no FIFO) via CMD53 (sync only).
 *   rw:       read or write (0/1)
 *   addr:     direct SDIO address
 *   buf:      pointer to memory data buffer
 *   nbytes:   number of bytes to transfer to/from buf
 * Returns 0 or error code.
 */
extern int bcmsdh_rwdata(void *sdh, uint rw, uint32 addr, uint8 *buf, uint nbytes);

/* Issue an abort to the specified function */
extern int bcmsdh_abort(void *sdh, uint fn);

/* Start SDIO Host Controller communication */


/* Stop SDIO Host Controller communication */


/* Wait system lock free */


/* Returns the number of IO functions reported by the device */


/* Miscellaneous knob tweaker. */
extern int bcmsdh_iovar_op(void *sdh, const char *name,
                           void *params, int plen, void *arg, int len, bool set);

/* Reset and reinitialize the device */


/* helper functions */


/* platform specific/high level functions */



/* Function to pass device-status bits to DHD. */


/* Function to return current window addr */
extern uint32 bcmsdh_cur_sbwad(void *sdh);

/* Function to pass chipid and rev to lower layers for controlling pr's */


/* GPIO support */


#endif	/* _bcmsdh_h_ */
