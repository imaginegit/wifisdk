/*
 * WPA Supplicant - driver interaction with generic Linux Wireless Extensions
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 *
 * This file implements a driver interface for the Linux Wireless Extensions.
 * When used with WE-18 or newer, this interface can be used as-is with number
 * of drivers. In addition to this, some of the common functions in this file
 * can be used by other driver interface implementations that use generic WE
 * ioctls, but require private ioctls for some of the functionality.
 */

#include "build_config.h"

#include "rk903.h"
#include "ws_common.h"
#include "wpadriver.h"
#include "wpa_supplicant.h"
#include "priv_netlink.h"
#include "driver_wext.h"
#include "wpa.h"

#ifdef WPA_AUTH

#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i

int wl_iw_set_encodeext(dhd_pub_t *pub,struct iw_request_info *info, struct iw_point *dwrq,char *extra)
{
	wl_wsec_key_t key;
	int error;
	struct iw_encode_ext *iwe;
    char encodeext_ioctl_buf[WLC_IOCTL_MEDLEN];

	if (!extra) 
		return -EINVAL;

    if (pub == NULL)
    {
        pub = G_dhd;
    }

	memset(&key, 0, sizeof(key));
	iwe = (struct iw_encode_ext *)extra;

	if (dwrq->flags & IW_ENCODE_DISABLED) 
	{
		;
	}
	key.index = 0;
	if (dwrq->flags & IW_ENCODE_INDEX)
	{
		key.index = (dwrq->flags & IW_ENCODE_INDEX) - 1;
	}

	key.len = iwe->key_len;

	
	if (!ETHER_ISMULTI(iwe->addr.sa_data))
		bcopy((void *)&iwe->addr.sa_data, (char *)&key.ea, ETHER_ADDR_LEN);

	//printf("key.len = %d\n", key.len); == 16
	if (key.len == 0) 
	{
		if (iwe->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) 
		{
			BCMDEBUG("Changing the the primary Key to %d\n", key.index);
			
			key.index = htod32(key.index);
			error = dhd_wl_ioctl_cmd(pub, WLC_SET_KEY_PRIMARY, &key.index, sizeof(key.index), 0);
			if (error)
				goto exit;
		}
		else 
		{
			dhd_wl_ioctl_cmd(pub, WLC_SET_KEY, &key, sizeof(key), 0);
		}
	}
	else 
	{
		if (iwe->key_len > sizeof(key.data))
			return -EINVAL;

		BCMDEBUG("Setting the key index %d\n", key.index);
		if (iwe->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) 
		{
			BCMDEBUG("key is a Primary Key\n");
			key.flags = WL_PRIMARY_KEY;
		}

		bcopy((void *)iwe->key, key.data, iwe->key_len);

		if (iwe->alg == IW_ENCODE_ALG_TKIP) 
		{
			uint8 keybuf[8];
			bcopy(&key.data[24], keybuf, sizeof(keybuf));
			bcopy(&key.data[16], &key.data[24], sizeof(keybuf));
			bcopy(keybuf, &key.data[16], sizeof(keybuf));
		}

		
/*		if (iwe->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID) {
			uchar *ivptr;
			ivptr = (uchar *)iwe->rx_seq;
			key.rxiv.hi = (ivptr[5] << 24) | (ivptr[4] << 16) |
				(ivptr[3] << 8) | ivptr[2];
			key.rxiv.lo = (ivptr[1] << 8) | ivptr[0];
			key.iv_initialized = TRUE;
		}
*/
		switch (iwe->alg) 
		{
			case IW_ENCODE_ALG_NONE:
				key.algo = CRYPTO_ALGO_OFF;
				break;
			case IW_ENCODE_ALG_WEP:
				if (iwe->key_len == WEP1_KEY_SIZE)
					key.algo = CRYPTO_ALGO_WEP1;
				else
					key.algo = CRYPTO_ALGO_WEP128;
				break;
			case IW_ENCODE_ALG_TKIP:
				key.algo = CRYPTO_ALGO_TKIP;
				break;
			case IW_ENCODE_ALG_CCMP:
				key.algo = CRYPTO_ALGO_AES_CCM;
				break;

			default:
				break;
		}
	    error = wldev_iovar_setbuf(G_dhd, "wsec_key", &key, sizeof(key),
								   encodeext_ioctl_buf, WLC_IOCTL_MEDLEN, NULL);

		if (error)
			goto exit;
	}
exit:
	return error;
}

extern int wpadebugen;

int wpa_driver_wext_set_key_ext(void *priv, wpa_alg alg, const u8 *addr, int key_idx,
				       int set_tx, const u8 *seq, size_t seq_len, const u8 *key, size_t key_len)
{
	struct iwreq iwr;
	int ret = 0, i;
	struct iw_encode_ext *ext;
	uint32 totallen;

	if (seq_len > IW_ENCODE_SEQ_MAX_SIZE) 
	{
		return -1;
	}
	
	totallen = sizeof(*ext) + key_len;
	ext = os_malloc(totallen);
	if (ext == NULL)
		return -1;
	
	os_memset(&iwr, 0, sizeof(iwr));
	iwr.u.encoding.flags = key_idx + 1;
	if (alg == WPA_ALG_NONE)
	{
		iwr.u.encoding.flags |= IW_ENCODE_DISABLED;
	}
	iwr.u.encoding.pointer = (void *)ext;
	iwr.u.encoding.length = totallen;

	if (addr == NULL || os_memcmp(addr, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0)
	{
		ext->ext_flags |= IW_ENCODE_EXT_GROUP_KEY;
	}
	if (set_tx)
	{
		ext->ext_flags |= IW_ENCODE_EXT_SET_TX_KEY;
	}

	ext->addr.sa_family = ARPHRD_ETHER;
	if (addr)
	{
		os_memcpy(ext->addr.sa_data, addr, ETH_ALEN);
	}
	else
	{
		os_memset(ext->addr.sa_data, 0xff, ETH_ALEN);
	}
		
	if (key && key_len) 
	{
		os_memcpy(&ext->key[0], key, key_len);
		ext->key_len = key_len;
	}
	switch (alg) 
	{
	case WPA_ALG_NONE:
		ext->alg = IW_ENCODE_ALG_NONE;
		break;
	case WPA_ALG_WEP:
		ext->alg = IW_ENCODE_ALG_WEP;
		break;
	case WPA_ALG_TKIP:
		ext->alg = IW_ENCODE_ALG_TKIP;
		break;
	case WPA_ALG_CCMP:
		ext->alg = IW_ENCODE_ALG_CCMP;
		break;
	default:
		os_free(ext);
		return -1;
	}

	if (seq && seq_len) 
	{
		ext->ext_flags |= IW_ENCODE_EXT_RX_SEQ_VALID;
		os_memcpy(ext->rx_seq, seq, seq_len);
	}
	wpadebugen = 1;
    wl_iw_set_encodeext(NULL, NULL, &(iwr.u.data), iwr.u.data.pointer);
	wpadebugen = 0;
	os_free(ext);
	return ret;
}

#endif /*WPA_AUTH*/

