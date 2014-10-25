/*
 * WPA Supplicant
 * Copyright (c) 2003-2008, Jouni Malinen <j@w1.fi>
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
 * This file implements functions for registering and unregistering
 * %wpa_supplicant interfaces. In addition, this file contains number of
 * functions for managing network connections.
 */


#include "build_config.h"
#include "wpa_include.h"
#include "ws_common.h"
#include "wpa.h"
#include "wpa_supplicant.h"
#include "config_ssid.h"
#include "wpa_i.h"
#include "rk903.h"
#include "wpa_sha1.h"

#ifdef WPA_AUTH

_ATTR_WPA_DATA_ struct wpa_sm _g_wpa_sm;
_ATTR_WPA_DATA_ struct wpa_sm *g_wpa_sm;

uint8 * wpa_alloc_eapol(uint8 type, const void *data, u16 data_len,
                            size_t *msg_len, void **data_pos)
{
	struct ieee802_1x_hdr *hdr;

	*msg_len = sizeof(*hdr) + data_len;
	
	hdr = os_malloc(*msg_len);
	if (hdr == NULL)
		return NULL;

	hdr->version = 1;
	hdr->type = type;
	hdr->length = host_to_be16(data_len);

	if (data)
		os_memcpy(hdr + 1, data, data_len);
	else
		os_memset(hdr + 1, 0, data_len);

	if (data_pos)
	{
		*data_pos = hdr + 1;
	}

	return (uint8 *)hdr;
}


/**
 * wpa_ether_send - Send Ethernet frame
 * @wpa_s: Pointer to wpa_supplicant data
 * @dest: Destination MAC address
 * @proto: Ethertype in host byte order
 * @buf: Frame payload starting from IEEE 802.1X header
 * @len: Frame payload length
 * Returns: >=0 on success, <0 on failure
 */
int wpa_ether_send(const uint8 *dest, u16 proto, const uint8 *buf, size_t len)
{
	int ret;
    unsigned char  tempbuf[256];

    memset(tempbuf, 0, 256);
    memcpy(tempbuf, dest, 6);
    wifi_get_mac_addr(tempbuf+6);
    tempbuf[12] = 0x88;
    tempbuf[13] = 0x8e;
    memcpy(tempbuf+14, buf, len);

    len += 14;   /*14:ethernet header*/

    ret = dhd_senddata(NULL, tempbuf, len);
    if (ret)
    {
        printf("eapol_host2card error\n");
	}

	return ret;	
}
	
void wpa_supplicant_rx_eapol(void *ctx, uint8 *src_addr, uint8 *buf, size_t len)
{
	struct wpa_sm * wpa = ctx;
	
	if (wpa == NULL)
	{
		wpa = g_wpa_sm;
	}

	/*if the type is not psk, Ignored it. */
	if (wpa->key_mgmt != WPA_KEY_MGMT_PSK)
		return;

	wpa_sm_rx_eapol(wpa, src_addr, buf, len);  

	return;
}


int wpa_auth_init(void)
{
    memset((void *)&_g_wpa_sm, 0, sizeof(struct wpa_sm));

    g_wpa_sm = &_g_wpa_sm;

    g_wpa_sm->renew_snonce = 1;
    g_wpa_sm->ap_rsn_ie = (uint8 *)g_connect_bss->rsn_ie;
    g_wpa_sm->ap_rsn_ie_len = g_connect_bss->rsn_ie_len;
    g_wpa_sm->ap_wpa_ie = (uint8 *)g_connect_bss->wpa_ie;
    g_wpa_sm->ap_wpa_ie_len = g_connect_bss->wpa_ie_len;
	
	if(g_connect_bss->rsn_ie_len)
    {
        g_wpa_sm->assoc_wpa_ie = (uint8 *)g_connect_bss->rsn_ie;
        g_wpa_sm->assoc_wpa_ie_len = g_connect_bss->rsn_ie_len;
    }
    else if(g_connect_bss->wpa_ie_len)
    {
        g_wpa_sm->assoc_wpa_ie = (uint8 *)g_connect_bss->wpa_ie;
        g_wpa_sm->assoc_wpa_ie_len = g_connect_bss->wpa_ie_len;
    }

    wifi_get_mac_addr(g_wpa_sm->own_addr);
    memcpy(g_wpa_sm->bssid , (const char*)g_connect_bss->bssid, ETH_ALEN);
    g_wpa_sm->key_mgmt = WPA_KEY_MGMT_PSK;

	g_wpa_sm->pairwise_cipher = g_connect_bss->pcipher_type << 1;
    g_wpa_sm->group_cipher = g_connect_bss->gcipher_type << 1;

    if (g_connect_bss->encode_type == ENCODE_TYPE_WPA)
    {
        g_wpa_sm->proto = WPA_PROTO_WPA;
    }
    else
    {
        g_wpa_sm->proto = WPA_PROTO_RSN;
    }

    g_wpa_sm->pmk_len = 32;

    pbkdf2_sha1((const char *)g_connect_bss->password, (const char *)g_connect_bss->ssid, 
		         g_connect_bss->ssid_len, 4096, g_wpa_sm->pmk, 32);

}

#endif	 /*WPA_AUTH*/
