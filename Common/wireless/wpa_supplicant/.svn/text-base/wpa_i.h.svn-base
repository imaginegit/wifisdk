/*
 * wpa_supplicant - Internal WPA state machine definitions
 * Copyright (c) 2004-2006, Jouni Malinen <j@w1.fi>
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

#ifndef WPA_I_H
#define WPA_I_H

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif /* _MSC_VER */

/**
 * struct wpa_ptk - WPA Pairwise Transient Key
 * IEEE Std 802.11i-2004 - 8.5.1.2 Pairwise key hierarchy
 */
struct wpa_ptk 
{
	u8 kck[16]; /* EAPOL-Key Key Confirmation Key (KCK) */
	u8 kek[16]; /* EAPOL-Key Key Encryption Key (KEK) */
	u8 tk1[16]; /* Temporal Key 1 (TK1) */
	union 
	{
		u8 tk2[16]; /* Temporal Key 2 (TK2) */
		struct 
		{
			u8 tx_mic_key[8];
			u8 rx_mic_key[8];
		}auth;
	} u;
} STRUCT_PACKED;

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */


#ifdef CONFIG_PEERKEY
#define PEERKEY_MAX_IE_LEN 80
struct wpa_peerkey {
	struct wpa_peerkey *next;
	int initiator; /* whether this end was initator for SMK handshake */
	u8 addr[ETH_ALEN]; /* other end MAC address */
	u8 inonce[WPA_NONCE_LEN]; /* Initiator Nonce */
	u8 pnonce[WPA_NONCE_LEN]; /* Peer Nonce */
	u8 rsnie_i[PEERKEY_MAX_IE_LEN]; /* Initiator RSN IE */
	size_t rsnie_i_len;
	u8 rsnie_p[PEERKEY_MAX_IE_LEN]; /* Peer RSN IE */
	size_t rsnie_p_len;
	u8 smk[PMK_LEN];
	int smk_complete;
	u8 smkid[PMKID_LEN];
	u32 lifetime;
	os_time_t expiration;
	int cipher; /* Selected cipher (WPA_CIPHER_*) */
	u8 replay_counter[WPA_REPLAY_COUNTER_LEN];
	int replay_counter_set;

	struct wpa_ptk stk, tstk;
	int stk_set, tstk_set;
};
#else /* CONFIG_PEERKEY */
struct wpa_peerkey;
#endif /* CONFIG_PEERKEY */


/**
 * struct wpa_sm - Internal WPA state machine data
 */
struct wpa_sm 
{
	u8 pmk[PMK_LEN];
	size_t pmk_len;
	struct wpa_ptk ptk, tptk;
	int ptk_set, tptk_set;
	u8 snonce[WPA_NONCE_LEN];
	u8 anonce[WPA_NONCE_LEN]; /* ANonce from the last 1/4 msg */
	int renew_snonce;
	u8 rx_replay_counter[WPA_REPLAY_COUNTER_LEN];
	int rx_replay_counter_set;
	u8 request_counter[WPA_REPLAY_COUNTER_LEN];

	struct wpa_sm_ctx *ctx;

	void *scard_ctx; /* context for smartcard callbacks */
	int fast_reauth; /* whether EAP fast re-authentication is enabled */

	struct wpa_ssid *cur_ssid;

	u8 own_addr[ETH_ALEN];
	u8 bssid[ETH_ALEN];

	unsigned int dot11RSNAConfigPMKLifetime;
	unsigned int dot11RSNAConfigPMKReauthThreshold;
	unsigned int dot11RSNAConfigSATimeout;

	unsigned int dot11RSNA4WayHandshakeFailures;

	/* Selected configuration (based on Beacon/ProbeResp WPA IE) */
	unsigned int proto;  //wpa   rsn
	unsigned int pairwise_cipher; //tkip 
	unsigned int group_cipher;
	unsigned int key_mgmt;  //psk
	unsigned int mgmt_group_cipher;

	u8 *assoc_wpa_ie; /* Own WPA/RSN IE from (Re)AssocReq */
	size_t assoc_wpa_ie_len;
	u8 *ap_wpa_ie, *ap_rsn_ie;
	size_t ap_wpa_ie_len, ap_rsn_ie_len;
};

#endif /* WPA_I_H */
