/*
 * Event loop based on select() loop
 * Copyright (c) 2002-2005, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "build_config.h"
#include "wpa_include.h"
#include "ws_common.h"
#include "sockets.h"
#include "wpa_supplicant.h"
#include "rk903.h"

#ifdef WPA_AUTH

#define MAXEAPOLBUFLEN 	300
void rk_wpa_auth_data_input(char * data, int len)
{
    struct ether_header *eh;
	char eapolbuf[MAXEAPOLBUFLEN];
	
	if (len >= MAXEAPOLBUFLEN)
	{
		printf("WPA data big\n");
	}

    memcpy(eapolbuf, data, len);
	eapolbuf[len] = 0;
	
    eh = (struct ether_header *)eapolbuf;
    if (memcmp((const char *)eh->ether_dhost, (const char *)wifi_get_mac_addr(0), ETH_ALEN))
    {
        return;
    }
    if (memcmp((const char *)eh->ether_shost, (const char *)g_connect_bss->bssid, ETH_ALEN))
    {
        return;
    }
    wpa_supplicant_rx_eapol(NULL, eh->ether_shost, (char *)&eh[1], len - sizeof(struct ether_header));

	return;
}

#endif /*WPA_AUTH*/