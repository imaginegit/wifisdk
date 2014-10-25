/*
 * wpa_supplicant - WPA definitions
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
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

#ifndef WPA_H
#define WPA_H

#include "wpa_defs.h"
#include "wpa_common.h"

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

#define WPA_CAPABILITY_PREAUTH BIT(0)
#define WPA_CAPABILITY_MGMT_FRAME_PROTECTION BIT(6)
#define WPA_CAPABILITY_PEERKEY_ENABLED BIT(9)

#define GENERIC_INFO_ELEM 0xdd
#define RSN_INFO_ELEM 0x30

#ifndef ETH_P_EAPOL
#define ETH_P_EAPOL 0x888e
#endif

#ifndef ETH_P_RSN_PREAUTH
#define ETH_P_RSN_PREAUTH 0x88c7
#endif

enum 
{
	REASON_UNSPECIFIED = 1,
	REASON_DEAUTH_LEAVING = 3,
	REASON_INVALID_IE = 13,
	REASON_MICHAEL_MIC_FAILURE = 14,
	REASON_4WAY_HANDSHAKE_TIMEOUT = 15,
	REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
	REASON_IE_IN_4WAY_DIFFERS = 17,
	REASON_GROUP_CIPHER_NOT_VALID = 18,
	REASON_PAIRWISE_CIPHER_NOT_VALID = 19,
	REASON_AKMP_NOT_VALID = 20,
	REASON_UNSUPPORTED_RSN_IE_VERSION = 21,
	REASON_INVALID_RSN_IE_CAPAB = 22,
	REASON_IEEE_802_1X_AUTH_FAILED = 23,
	REASON_CIPHER_SUITE_REJECTED = 24
};

#define PMKID_LEN 16


struct wpa_sm;
struct wpa_ssid;
struct wpa_config_blob;

struct wpa_sm_ctx {
	void *ctx; /* pointer to arbitrary upper level context */

	void (*set_state)(void *ctx, wpa_states state);
	wpa_states (*get_state)(void *ctx);
	void (*req_scan)(void *ctx, int sec, int usec);
	void (*deauthenticate)(void * ctx, int reason_code); 
	void (*disassociate)(void *ctx, int reason_code);
	int (*set_key)(void *ctx, wpa_alg alg,
		       const u8 *addr, int key_idx, int set_tx,
		       const u8 *seq, size_t seq_len,
		       const u8 *key, size_t key_len);
	void (*scan)(void *eloop_ctx, void *timeout_ctx);
	struct wpa_ssid * (*get_ssid)(void *ctx);
	int (*get_bssid)(void *ctx, u8 *bssid);
	int (*ether_send)(void *ctx, const u8 *dest, u16 proto, const u8 *buf,
			  size_t len);
	int (*get_beacon_ie)(void *ctx);
	void (*cancel_auth_timeout)(void *ctx);
	u8 * (*alloc_eapol)(void *ctx, u8 type, const void *data, u16 data_len,
			    size_t *msg_len, void **data_pos);
	int (*add_pmkid)(void *ctx, const u8 *bssid, const u8 *pmkid);
	int (*remove_pmkid)(void *ctx, const u8 *bssid, const u8 *pmkid);
	void (*set_config_blob)(void *ctx, struct wpa_config_blob *blob);
	const struct wpa_config_blob * (*get_config_blob)(void *ctx,
							  const char *name);
	int (*mlme_setprotection)(void *ctx, const u8 *addr,
				  int protection_type, int key_type);
};


enum wpa_sm_conf_params {
	RSNA_PMK_LIFETIME /* dot11RSNAConfigPMKLifetime */,
	RSNA_PMK_REAUTH_THRESHOLD /* dot11RSNAConfigPMKReauthThreshold */,
	RSNA_SA_TIMEOUT /* dot11RSNAConfigSATimeout */,
	WPA_PARAM_PROTO,
	WPA_PARAM_PAIRWISE,
	WPA_PARAM_GROUP,
	WPA_PARAM_KEY_MGMT,
	WPA_PARAM_MGMT_GROUP
};

struct wpa_ie_data {
	int proto;
	int pairwise_cipher;
	int group_cipher;
	int key_mgmt;
	int capabilities;
	int num_pmkid;
	const u8 *pmkid;
	int mgmt_group_cipher;
};

int wpa_sm_rx_eapol(struct wpa_sm *sm, const u8 *src_addr, u8 *buf, size_t len);

#endif /* WPA_H */
