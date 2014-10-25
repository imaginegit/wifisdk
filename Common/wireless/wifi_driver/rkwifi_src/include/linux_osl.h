/*
 * Linux OS Independent Layer
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
 * $Id: linux_osl.h 301794 2011-12-08 20:41:35Z $
 */


#ifndef _linux_osl_h_
#define _linux_osl_h_


#define	DMA_TX	1	
#define	DMA_RX	2	

#define	bcopy(src, dst, len)	memcpy((dst), (src), (len))
#define	bzero(b, len)			memset((b), '\0', (len))
#define	bcmp(b1, b2, len)      memcmp((b1), (b2), (len))

#define OSL_WRITE_REG(r, v) (bcmsdh_reg_write(NULL, (uint)(r), sizeof(*(r)), (v)))
#define OSL_READ_REG(r) (bcmsdh_reg_read(NULL, (uint)(r), sizeof(*(r))))

#define R_REG(r)     OSL_READ_REG(r)
#define W_REG(r, v)  OSL_WRITE_REG(r, v)

#define	AND_REG(r, v)		W_REG((r), R_REG(r) & (v))
#define	OR_REG(r, v)		W_REG((r), R_REG(r) | (v))

#endif	
