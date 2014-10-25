/*
 * Misc useful os-independent macros and functions.
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
 * $Id: bcmutils.h 294991 2011-11-09 00:17:28Z $
 */


#ifndef	_bcmutils_h_
#define	_bcmutils_h_

#define bcm_strcpy_s(dst, noOfElements, src)            strcpy((dst), (src))
#define bcm_strncpy_s(dst, noOfElements, src, count)    strncpy((dst), (src), (count))
#define bcm_strcat_s(dst, noOfElements, src)            strcat((dst), (src))

#ifdef __cplusplus
extern "C" {
#endif


#define _BCM_U	0x01	
#define _BCM_L	0x02	
#define _BCM_D	0x04	
#define _BCM_C	0x08	
#define _BCM_P	0x10	
#define _BCM_S	0x20	
#define _BCM_X	0x40	
#define _BCM_SP	0x80	

extern const unsigned char bcm_ctype[];
#define bcm_ismask(x)	(bcm_ctype[(int)(unsigned char)(x)])

#define bcm_isalnum(c)	((bcm_ismask(c)&(_BCM_U|_BCM_L|_BCM_D)) != 0)
#define bcm_isalpha(c)	((bcm_ismask(c)&(_BCM_U|_BCM_L)) != 0)
#define bcm_iscntrl(c)	((bcm_ismask(c)&(_BCM_C)) != 0)
#define bcm_isdigit(c)	((bcm_ismask(c)&(_BCM_D)) != 0)
#define bcm_isgraph(c)	((bcm_ismask(c)&(_BCM_P|_BCM_U|_BCM_L|_BCM_D)) != 0)
#define bcm_islower(c)	((bcm_ismask(c)&(_BCM_L)) != 0)
#define bcm_isprint(c)	((bcm_ismask(c)&(_BCM_P|_BCM_U|_BCM_L|_BCM_D|_BCM_SP)) != 0)
#define bcm_ispunct(c)	((bcm_ismask(c)&(_BCM_P)) != 0)
#define bcm_isspace(c)	((bcm_ismask(c)&(_BCM_S)) != 0)
#define bcm_isupper(c)	((bcm_ismask(c)&(_BCM_U)) != 0)
#define bcm_isxdigit(c)	((bcm_ismask(c)&(_BCM_D|_BCM_X)) != 0)
#define bcm_tolower(c)	(bcm_isupper((c)) ? ((c) + 'a' - 'A') : (c))
#define bcm_toupper(c)	(bcm_islower((c)) ? ((c) + 'A' - 'a') : (c))



struct bcmstrbuf 
{
	char *buf;	
	unsigned int size;	
	char *origbuf;	
	unsigned int origsize;	
};


#define SPINWAIT(exp, us) { \
	uint countdown = (us) + 9; \
	while ((exp) && (countdown >= 10)) {\
		DelayUs(10); \
		countdown -= 10; \
	} \
}

typedef struct bcm_iovar 
{
	const char *name;	
	uint16 varid;		
	uint16 flags;		
	uint16 type;		
	uint16 minlen;		
} bcm_iovar_t;


#define IOV_GET 0 
#define IOV_SET 1 


#define IOV_GVAL(id)		((id)*2)
#define IOV_SVAL(id)		(((id)*2)+IOV_SET)
#define IOV_ISSET(actionid)	((actionid & IOV_SET) == IOV_SET)
#define IOV_ID(actionid)	(actionid >> 1)

extern const bcm_iovar_t *bcm_iovar_lookup(const bcm_iovar_t *table, const char *name);
extern int bcm_iovar_lencheck(const bcm_iovar_t *table, void *arg, int len, bool set);


#define IOVT_VOID	0	
#define IOVT_BOOL	1	
#define IOVT_INT8	2	
#define IOVT_UINT8	3	
#define IOVT_INT16	4	
#define IOVT_UINT16	5	
#define IOVT_INT32	6	
#define IOVT_UINT32	7	
#define IOVT_BUFFER	8	
#define BCM_IOVT_VALID(type) (((unsigned int)(type)) <= IOVT_BUFFER)


#define BCM_IOV_TYPE_INIT { \
	"void", \
	"bool", \
	"int8", \
	"uint8", \
	"int16", \
	"uint16", \
	"int32", \
	"uint32", \
	"buffer", \
	"" }

#define BCM_IOVT_IS_INT(type) (\
	(type == IOVT_BOOL) || \
	(type == IOVT_INT8) || \
	(type == IOVT_UINT8) || \
	(type == IOVT_INT16) || \
	(type == IOVT_UINT16) || \
	(type == IOVT_INT32) || \
	(type == IOVT_UINT32))

#define BCME_STRLEN 		64	
#define VALID_BCMERROR(e)  ((e <= 0) && (e >= BCME_LAST))


#define BCME_OK				0	
#define BCME_ERROR			-1	
#define BCME_BADARG			-2	
#define BCME_BADOPTION			-3	
#define BCME_NOTUP			-4	
#define BCME_NOTDOWN			-5	
#define BCME_NOTAP			-6	
#define BCME_NOTSTA			-7	
#define BCME_BADKEYIDX			-8	
#define BCME_RADIOOFF 			-9	
#define BCME_NOTBANDLOCKED		-10	
#define BCME_NOCLK			-11	
#define BCME_BADRATESET			-12	
#define BCME_BADBAND			-13	
#define BCME_BUFTOOSHORT		-14	
#define BCME_BUFTOOLONG			-15	
#define BCME_BUSY			-16	
#define BCME_NOTASSOCIATED		-17	
#define BCME_BADSSIDLEN			-18	
#define BCME_OUTOFRANGECHAN		-19	
#define BCME_BADCHAN			-20	
#define BCME_BADADDR			-21	
#define BCME_NORESOURCE			-22	
#define BCME_UNSUPPORTED		-23	
#define BCME_BADLEN			-24	
#define BCME_NOTREADY			-25	
#define BCME_EPERM			-26	
#define BCME_NOMEM			-27	
#define BCME_ASSOCIATED			-28	
#define BCME_RANGE			-29	
#define BCME_NOTFOUND			-30	
#define BCME_WME_NOT_ENABLED		-31	
#define BCME_TSPEC_NOTFOUND		-32	
#define BCME_ACM_NOTSUPPORTED		-33	
#define BCME_NOT_WME_ASSOCIATION	-34	
#define BCME_SDIO_ERROR			-35	
#define BCME_DONGLE_DOWN		-36	
#define BCME_VERSION			-37 	
#define BCME_TXFAIL			-38 	
#define BCME_RXFAIL			-39	
#define BCME_NODEVICE			-40 	
#define BCME_NMODE_DISABLED		-41 	
#define BCME_NONRESIDENT		-42 
#define BCME_LAST			BCME_NONRESIDENT


#define BCMERRSTRINGTABLE {		\
	"OK",				\
	"Undefined error",		\
	"Bad Argument",			\
	"Bad Option",			\
	"Not up",			\
	"Not down",			\
	"Not AP",			\
	"Not STA",			\
	"Bad Key Index",		\
	"Radio Off",			\
	"Not band locked",		\
	"No clock",			\
	"Bad Rate valueset",		\
	"Bad Band",			\
	"Buffer too short",		\
	"Buffer too long",		\
	"Busy",				\
	"Not Associated",		\
	"Bad SSID len",			\
	"Out of Range Channel",		\
	"Bad Channel",			\
	"Bad Address",			\
	"Not Enough Resources",		\
	"Unsupported",			\
	"Bad length",			\
	"Not Ready",			\
	"Not Permitted",		\
	"No Memory",			\
	"Associated",			\
	"Not In Range",			\
	"Not Found",			\
	"WME Not Enabled",		\
	"TSPEC Not Found",		\
	"ACM Not Supported",		\
	"Not WME Association",		\
	"SDIO Bus Error",		\
	"Dongle Not Accessible",	\
	"Incorrect version",		\
	"TX Failure",			\
	"RX Failure",			\
	"Device Not Present",		\
	"NMODE Disabled",		\
	"Nonresident overlay access", \
}

#ifndef ABS
#define	ABS(a)			(((a) < 0)?-(a):(a))
#endif 

#ifndef MIN
#define	MIN(a, b)		(((a) < (b))?(a):(b))
#endif 

#ifndef MAX
#define	MAX(a, b)		(((a) > (b))?(a):(b))
#endif 

#define CEIL(x, y)		(((x) + ((y)-1)) / (y))
#define	ROUNDUP(x, y)		((((x)+((y)-1))/(y))*(y))
#define	ISALIGNED(a, x)		(((unsigned int)(a) & ((x)-1)) == 0)
#define ALIGN_ADDR(addr, boundary) (void *)(((unsigned int)(addr) + (boundary) - 1) \
	                                         & ~((boundary) - 1))

#define	ISPOWEROF2(x)		((((x)-1)&(x)) == 0)
#define VALID_MASK(mask)	!((mask) & ((mask) + 1))

#ifndef OFFSETOF
#define	OFFSETOF(type, member)	((uint)&((type *)0)->member)
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)		(sizeof(a)/sizeof(a[0]))
#endif


extern void *_bcmutils_dummy_fn;
#define REFERENCE_FUNCTION(f)	(_bcmutils_dummy_fn = (void *)(f))


#ifndef setbit
#ifndef NBBY		      
#define	NBBY	8	
#endif 
#define	setbit(a, i)	(((uint8 *)a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define	clrbit(a, i)	(((uint8 *)a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define	isset(a, i)	(((const uint8 *)a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define	isclr(a, i)	((((const uint8 *)a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)
#endif 

#define	NBITS(type)	(sizeof(type) * 8)
#define NBITVAL(nbits)	(1 << (nbits))
#define MAXBITVAL(nbits)	((1 << (nbits)) - 1)
#define	NBITMASK(nbits)	MAXBITVAL(nbits)
#define MAXNBVAL(nbyte)	MAXBITVAL((nbyte) * 8)


#define MUX(pred, true, false) ((pred) ? (true) : (false))


#define MODDEC(x, bound) MUX((x) == 0, (bound) - 1, (x) - 1)
#define MODINC(x, bound) MUX((x) == (bound) - 1, 0, (x) + 1)


#define MODDEC_POW2(x, bound) (((x) - 1) & ((bound) - 1))
#define MODINC_POW2(x, bound) (((x) + 1) & ((bound) - 1))


#define MODADD(x, y, bound) \
    MUX((x) + (y) >= (bound), (x) + (y) - (bound), (x) + (y))
#define MODSUB(x, y, bound) \
    MUX(((int)(x)) - ((int)(y)) < 0, (x) - (y) + (bound), (x) - (y))


#define MODADD_POW2(x, y, bound) (((x) + (y)) & ((bound) - 1))
#define MODSUB_POW2(x, y, bound) (((x) - (y)) & ((bound) - 1))


#define CRC8_INIT_VALUE  0xff		
#define CRC8_GOOD_VALUE  0x9f		
#define CRC16_INIT_VALUE 0xffff		
#define CRC16_GOOD_VALUE 0xf0b8		
#define CRC32_INIT_VALUE 0xffffffff	
#define CRC32_GOOD_VALUE 0xdebb20e3	


typedef struct bcm_bit_desc {
	uint32	bit;
	const char* name;
} bcm_bit_desc_t;


typedef struct bcm_tlv {
	uint8	id;
	uint8	len;
	uint8	data[1];
} bcm_tlv_t;


#define bcm_valid_tlv(elt, buflen) ((buflen) >= 2 && (int)(buflen) >= (int)(2 + (elt)->len))


#define ETHER_ADDR_STR_LEN	18	


typedef  uint32 (*bcmutl_rdreg_rtn)(void *arg0, uint arg1, uint32 offset);



#define SSID_FMT_BUF_LEN	((4 * DOT11_MAX_SSID_LEN) + 1)

extern unsigned int process_nvram_vars(char *varbuf, unsigned int len);
extern bcm_tlv_t * bcm_parse_tlvs(void *buf, int buflen, uint key);

extern uint16 bcm_qdbm_to_mw(uint8 qdbm);
extern uint8 bcm_mw_to_qdbm(uint16 mw);

#ifdef __cplusplus
	}
#endif

#endif	
