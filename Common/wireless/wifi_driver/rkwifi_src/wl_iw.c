/*
 * Linux Wireless Extensions support
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
 * $Id: wl_iw.c,v 1.132.2.18 2011-02-05 01:44:47 $
 */
 
#include "rk903.h"

typedef void wlc_info_t;
typedef void wl_info_t;
typedef const struct si_pub  si_t;

/* message levels */
#define WL_ERROR_LEVEL	0x0001
#define WL_SCAN_LEVEL	0x0002
#define WL_ASSOC_LEVEL	0x0004
#define WL_INFORM_LEVEL	0x0008
#define WL_WSEC_LEVEL	0x0010
#define WL_PNO_LEVEL	0x0020
#define WL_COEX_LEVEL	0x0040
#define WL_SOFTAP_LEVEL	0x0080
#define WL_TRACE_LEVEL	0x0100

#define IW_WSEC_ENABLED(wsec)	((wsec) & (WEP_ENABLED | TKIP_ENABLED | AES_ENABLED))

//#define WL_IW_USE_ISCAN  1
#define ENABLE_ACTIVE_PASSIVE_SCAN_SUPPRESS  1

wl_iw_extra_params_t	g_wl_iw_params;


#define MAX_WLIW_IOCTL_LEN 1024


#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i
#define htodchanspec(i) i
#define dtohchanspec(i) i

//extern struct iw_statistics *dhd_get_wireless_stats(dhd_pub_t *pub);
//extern int dhd_wait_pend8021x(dhd_pub_t *pub);

//static void *g_scan = NULL;
//static volatile uint g_scan_specified_ssid;
//static wlc_ssid_t g_specific_ssid;
static wlc_ssid_t g_ssid;

uint8 isScanComplete = 0;
uint8 isEscanSuccess = 0;

static struct _bss_descriptor_info bss_descriptor_info;

extern struct bss_descriptor * g_connect_bss;
extern struct bss_descriptor  _g_connect_bss_;
extern dhd_pub_t *G_dhd;


int dhd_wl_ioctl_cmd(dhd_pub_t *dhd_pub, int cmd, void *arg, int len, uint8 set);

void bss_descriptor_info_init(void)
{
    memset(&bss_descriptor_info, 0, sizeof(bss_descriptor_info));//4579
    bss_descriptor_info.frist_scan = TRUE;
}

static void swap_key_from_BE(wl_wsec_key_t *key)
{
	key->index = htod32(key->index);
	key->len = htod32(key->len);
	key->algo = htod32(key->algo);
	key->flags = htod32(key->flags);
	key->rxiv.hi = htod32(key->rxiv.hi);
	key->rxiv.lo = htod16(key->rxiv.lo);
	key->iv_initialized = htod32(key->iv_initialized);
}

static void swap_key_to_BE(wl_wsec_key_t *key)
{
	key->index = dtoh32(key->index);
	key->len = dtoh32(key->len);
	key->algo = dtoh32(key->algo);
	key->flags = dtoh32(key->flags);
	key->rxiv.hi = dtoh32(key->rxiv.hi);
	key->rxiv.lo = dtoh16(key->rxiv.lo);
	key->iv_initialized = dtoh32(key->iv_initialized);
}

unsigned int compare_ether_addr(const unsigned char *addr1, const unsigned char *addr2)
{
    const unsigned char *a = (const unsigned char *) addr1;
    const unsigned char *b = (const unsigned char *) addr2;

	if (ETH_ALEN != 6)
		return -1;

    return (((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) | (a[3] ^ b[3]) | (a[4] ^ b[4]) | (a[5] ^ b[5])) != 0);
}

static  int is_same_network(struct bss_descriptor *src, struct bss_descriptor *dst)
{
	/* A network is only a duplicate if the channel, BSSID, and ESSID
	 * all match.  We treat all <hidden> with the same BSSID and channel
	 * as one network */
	return ((src->ssid_len == dst->ssid_len) &&
		(src->channel == dst->channel) &&
		!compare_ether_addr(src->bssid, dst->bssid) &&
		!memcmp(src->ssid, dst->ssid, src->ssid_len));
}

static int dev_wlc_intvar_get_reg(dhd_pub_t *pub,char *name,uint  reg,int *retval)
{
	union 
	{
		char buf[WLC_IOCTL_SMLEN];
		int val;
	} var;
	int error;

	uint len;
	len = bcm_mkiovar(name, (char *)(&reg), sizeof(reg), (char *)(&var), sizeof(var.buf));
	ASSERT(len);
	error = dhd_wl_ioctl_cmd(pub, WLC_GET_VAR, (void *)&var, len, 0);

	*retval = dtoh32(var.val);
	return (error);
}


static int dev_wlc_intvar_set_reg(dhd_pub_t *pub,char *name,char *addr,char * val)
{
	char reg_addr[8];

	memset(reg_addr, 0, sizeof(reg_addr));
	memcpy((char *)&reg_addr[0], (char *)addr, 4);
	memcpy((char *)&reg_addr[4], (char *)val, 4);

	return (dev_wlc_bufvar_set(pub, name,  (char *)&reg_addr[0], sizeof(reg_addr)));
}


static int dev_wlc_intvar_set(dhd_pub_t *pub,char *name,int val)
{
	char buf[WLC_IOCTL_SMLEN];
	uint len;

	val = htod32(val);
	len = bcm_mkiovar(name, (char *)(&val), sizeof(val), buf, sizeof(buf));

	return (dhd_wl_ioctl_cmd(pub, WLC_SET_VAR, buf, len, 0));
}



static int dev_wlc_bufvar_set(dhd_pub_t *pub,char *name,char *buf, int len)
{
	char ioctlbuf[MAX_WLIW_IOCTL_LEN];
	uint buflen;

	buflen = bcm_mkiovar(name, buf, len, ioctlbuf, sizeof(ioctlbuf));
	ASSERT(buflen);

	return (dhd_wl_ioctl_cmd(pub, WLC_SET_VAR, ioctlbuf, buflen, 0));
}



static int dev_wlc_bufvar_get(dhd_pub_t *pub,char *name,char *buf, int buflen)
{
	char ioctlbuf[MAX_WLIW_IOCTL_LEN];
	int error;
	uint len;

	len = bcm_mkiovar(name, NULL, 0, ioctlbuf, sizeof(ioctlbuf));
	ASSERT(len);
	error = dhd_wl_ioctl_cmd(pub, WLC_GET_VAR, (void*)ioctlbuf, MAX_WLIW_IOCTL_LEN, 0);
	if (!error)
		bcopy(ioctlbuf, buf, buflen);

	return (error);
}



static int dev_wlc_intvar_get(dhd_pub_t *pub, char *name, int *retval)
{
	union 
	{
		char buf[WLC_IOCTL_SMLEN];
		int val;
	} var;
	int error;
	uint len;
	uint data_null;

	len = bcm_mkiovar(name, (char *)(&data_null), 0, (char *)(&var), sizeof(var.buf));
	ASSERT(len);
	error = dhd_wl_ioctl_cmd(pub, WLC_GET_VAR, (void *)&var, len, 0);

	*retval = dtoh32(var.val);

	return (error);
}

int wf_mhz2channel(uint freq, uint start_factor)
{
	int ch = -1;
	uint base;
	int offset;

	if (start_factor == 0) 
	{
		if (freq >= 2400 && freq <= 2500)
		{
			start_factor = WF_CHAN_FACTOR_2_4_G;
		}
		else if (freq >= 5000 && freq <= 6000)
		{
			start_factor = WF_CHAN_FACTOR_5_G;
		}
	}

	if (freq == 2484 && start_factor == WF_CHAN_FACTOR_2_4_G)
		return 14;

	base = start_factor / 2;

	if ((freq < base) || (freq > base + 1000))
		return -1;

	offset = freq - base;
	ch = offset / 5;

	if (offset != (ch * 5))
		return -1;

	if (start_factor == WF_CHAN_FACTOR_2_4_G && (ch < 1 || ch > 13))
		return -1;

	return ch;
}


static int wl_iw_set_freq(dhd_pub_t *pub, struct iw_request_info *info, struct iw_freq *fwrq, char *extra)
{
	int error, chan;
	uint sf = 0;

	BCMDEBUG("SIOCSIWFREQ\n");
	
	if (fwrq->e == 0 && fwrq->m < MAXCHANNEL) 
	{
		chan = fwrq->m;
	}
	else 
	{
		if (fwrq->e >= 6) 
		{
			fwrq->e -= 6;
			while (fwrq->e--)
			{
				fwrq->m *= 10;
			}
		} 
		else if (fwrq->e < 6) 
		{
			while (fwrq->e++ < 6)
			{
				fwrq->m /= 10;
			}
		}
		
		if (fwrq->m > 4000 && fwrq->m < 5000)
			sf = WF_CHAN_FACTOR_4_G; 

		chan = wf_mhz2channel(fwrq->m, sf);
	}

	chan = htod32(chan);

    error = dhd_wl_ioctl_cmd(pub, WLC_SET_CHANNEL, &chan, sizeof(chan), 0);
    if(error)
    {
        return error;
    }
    
	g_wl_iw_params.target_channel = chan;
	
	return -EINPROGRESS;
}

static int wl_iw_set_mode(dhd_pub_t *pub,struct iw_request_info *info,uint32 *uwrq,char *extra)
{
	int infra = 0, ap = 0, error = 0;

	switch (*uwrq) 
	{
	case IW_MODE_MASTER:
		infra = ap = 1;
		break;
	case IW_MODE_ADHOC:
	case IW_MODE_AUTO:
		break;
	case IW_MODE_INFRA:
		infra = 1;
		break;
	default:
		return -EINVAL;
	}
	
	infra = htod32(infra);
	ap = htod32(ap);

	if ((error = dhd_wl_ioctl_cmd(pub, WLC_SET_INFRA, &infra, sizeof(infra), 0)) ||
	    (error = dhd_wl_ioctl_cmd(pub, WLC_SET_AP, &ap, sizeof(ap), 0)))
		return error;
	
	return -EINPROGRESS;
}


static int wl_iw_ch_to_chanspec(int ch, wl_join_params_t *join_params, int *join_params_size)
{
	chanspec_t chanspec = 0;

	if (ch != 0) 
	{
		join_params->params.chanspec_num = 1;
		join_params->params.chanspec_list[0] = ch;

		if (join_params->params.chanspec_list[0])
			chanspec |= WL_CHANSPEC_BAND_2G;
		else
			chanspec |= WL_CHANSPEC_BAND_5G;

		chanspec |= WL_CHANSPEC_BW_20;
		chanspec |= WL_CHANSPEC_CTL_SB_NONE;

		
		*join_params_size += WL_ASSOC_PARAMS_FIXED_SIZE +
			join_params->params.chanspec_num * sizeof(chanspec_t);

		
		join_params->params.chanspec_list[0] &= WL_CHANSPEC_CHAN_MASK;
		join_params->params.chanspec_list[0] |= chanspec;
		join_params->params.chanspec_list[0] = htodchanspec(join_params->params.chanspec_list[0]);

		join_params->params.chanspec_num = htod32(join_params->params.chanspec_num);

		BCMDEBUG("join_params->params.chanspec_list[0]= %X\n",join_params->params.chanspec_list[0]);
	}
	return 1;
}

static bool ie_is_wpa_ie(uint8 **wpaie, uint8 **tlvs, int *tlvs_len)
{
	uint8 *ie = *wpaie;

	if ((ie[1] >= 6) &&
		!bcmp((const void *)&ie[2], (const void *)(WPA_OUI "\x01"), 4)) 
	{
		return TRUE;
	}

	ie += ie[1] + 2;
	
	*tlvs_len -= (int)(ie - *tlvs);
	
	*tlvs = ie;
	return FALSE;
}

static bool ie_is_wps_ie(uint8 **wpsie, uint8 **tlvs, int *tlvs_len)
{
	uint8 *ie = *wpsie;

	if ((ie[1] >= 4) &&  
		!bcmp((const void *)&ie[2], (const void *)(WPA_OUI "\x04"), 4)) 
	{
		return TRUE;
	}

	ie += ie[1] + 2;
	
	*tlvs_len -= (int)(ie - *tlvs);
	
	*tlvs = ie;
	return FALSE;
}



static int wl_iw_handle_scanresults_ies(struct bss_descriptor *bssp, char *end,
	                                           struct iw_request_info *info, wl_bss_info_t *bi)
{
	struct iw_event	iwe;
    int ret = -1;
    unsigned short i, CipherCnt;
	
	if (bi->ie_length) 
	{
		bcm_tlv_t *ie;
		uint8 *ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
		int ptr_len = bi->ie_length;

		/* WPA2*/
		/* tlv: id + len + data*/
		//id(1)+len(1)+num(2)+00-0F-AC-XX(g)+num(2)+00-0F-AC-XX(p)
		ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_RSN_ID);
		if (ie) 
		{
            //TKIP OR CMMP   
            memcpy((void*)bssp->rsn_ie, (void*)ie, ie->len + 2);
            bssp->rsn_ie_len = ie->len + 2;
            bssp->encode_type = ENCODE_TYPE_WPA2; 

			/* group cipher suite. */
            if (((char*)ie)[7] == 2) //TKIP 
            {
                bssp->gcipher_type = IW_AUTH_CIPHER_TKIP;
            }
            else if(((char*)ie)[7] == 4) //CCMP
            {
                bssp->gcipher_type = IW_AUTH_CIPHER_CCMP;
            }
            else
            {
                bssp->gcipher_type = IW_AUTH_CIPHER_CCMP;
            }       

           	/* pairwise cipher suite. */
            CipherCnt = ((unsigned short *)ie)[4];
            if(CipherCnt == 1)
            {
                if(((char*)ie)[13] == 2) //TKIP 
                {
                    bssp->pcipher_type = IW_AUTH_CIPHER_TKIP;
                }
                else if(((char*)ie)[13] == 4) //CCMP
                {
                    bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                }
                else
                {
                    bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                }       
            }
            else if(CipherCnt > 1)
            {
               for (i = 0; i < CipherCnt; i++)
               {                    
                    if(((unsigned char *)ie)[13 + i * 4] == 4) //CCMP
                    {
                        //exist ccmp
                        break;
                    }                   
               }

               if (i >= CipherCnt)
               {
                    if (((char*)ie)[7] == 2) //TKIP 
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_TKIP;
                    }
                    else if (((char*)ie)[7] == 4) //CCMP
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                    }
                    else
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                    }

                    #if 1
                    ((unsigned short *)bssp->rsn_ie)[4] = 1;
                    memcpy((void *)(&(((unsigned char *)bssp->rsn_ie)[10])),(void *)(&(((unsigned char *)bssp->rsn_ie)[4])), 4);
                    memcpy((void *)(&(((unsigned char *)bssp->rsn_ie)[14])), (void *)(&(((unsigned char *)ie)[14 + 4 * (CipherCnt - 1)])), bssp->rsn_ie_len - 8 - (CipherCnt - 1) * 4);

                    (((unsigned char *)bssp->rsn_ie)[1]) -=  ((CipherCnt - 1) * 4);
                    bssp->rsn_ie_len -=  ((CipherCnt - 1) * 4); 

                    #endif
               }
               else
               {
                    if(((char*)ie)[13 + i * 4] == 2) //TKIP 
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_TKIP;
                    }
                    else if(((char*)ie)[13 + i * 4] == 4) //CCMP
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                    }
                    else
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                    }

                    #if 1
                    ((unsigned short *)bssp->rsn_ie)[4] = 1;
                    memcpy((void *)(&(((unsigned char *)bssp->rsn_ie)[10])),(void *)(&(((unsigned char *)ie)[10 + i * 4])), 4);
                    memcpy((void *)(&(((unsigned char *)bssp->rsn_ie)[14])), (void *)(&(((unsigned char *)ie)[14 + 4 * (CipherCnt - 1)])), bssp->rsn_ie_len - 8 - (CipherCnt - 1) * 4);

                    (((unsigned char *)bssp->rsn_ie)[1]) -=  ((CipherCnt - 1) * 4);
                    bssp->rsn_ie_len -= ((CipherCnt - 1) * 4);

                    #endif
               }
               
            }
               
            
            ret = 0;
		}

		/* WPS*/
		ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
		while ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_WPA_ID))) 
		{
			if (ie_is_wps_ie(((uint8 **)&ie), &ptr, &ptr_len)) 
			{
	            /*
		        memcpy(bssp->wpa_ie, ie,ie->len + 2);
                bssp->wpa_ie_len = ie->len + 2;
                if(ENCODE_TYPE_WPA2 ==bssp->encode_type) //不接收wps认证方式
                {
                    bssp->encode_type = ENCODE_TYPE_WPA_WPA2;
                }
                else
                {
                    bssp->encode_type = ENCODE_TYPE_WPA;
                }
                */
				break;
			}
		}

		/* WPA */
		ptr = ((uint8 *)bi) + sizeof(wl_bss_info_t);
		ptr_len = bi->ie_length;
		while ((ie = bcm_parse_tlvs(ptr, ptr_len, DOT11_MNG_WPA_ID))) 
		{
			if (ie_is_wpa_ie(((uint8 **)&ie), &ptr, &ptr_len)) 
			{
				//iwe.cmd = IWEVGENIE;
        		memcpy((void*)bssp->wpa_ie, (void*)ie, ie->len + 2);
                bssp->wpa_ie_len = ie->len + 2;
				
                if (ENCODE_TYPE_WPA2 == bssp->encode_type)
                {
                    break;
                }
                else
                {
                    bssp->encode_type = ENCODE_TYPE_WPA;
                }

             
                if(((char*)ie)[11] == 2) //TKIP
                {
                    bssp->gcipher_type = IW_AUTH_CIPHER_TKIP;
                }
                else if(((char*)ie)[11] == 4) //CCMP
                {
                    bssp->gcipher_type = IW_AUTH_CIPHER_CCMP;
                }
                else
                {
                    bssp->gcipher_type = IW_AUTH_CIPHER_CCMP;
                }

                
                CipherCnt = ((unsigned short *)ie)[6];
                if(CipherCnt == 1)
                {
                    if(((char*)ie)[17] == 2) //TKIP 
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_TKIP;
                    }
                    else if(((char*)ie)[17] == 4) //CCMP
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                    }
                    else
                    {
                        bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                    }       
                }
                else if(CipherCnt > 1)
                {
                   for(i = 0; i < CipherCnt; i++)
                   {                    
                        if(((unsigned char *)ie)[17 + i * 4] == 4)
                        {
                            //exist ccmp
                            break;
                        }                   
                   }

                   if(i >= CipherCnt)
                   {
                        if(((char*)ie)[11] == 2) //TKIP 
                        {
                            bssp->pcipher_type = IW_AUTH_CIPHER_TKIP;
                        }
                        else if(((char*)ie)[11] == 4) //CCMP
                        {
                            bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                        }
                        else
                        {
                            bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                        }

                        #if 1
                        ((unsigned short *)bssp->wpa_ie)[6] = 1;
                        memcpy((void *)(&(((unsigned char *)bssp->wpa_ie)[14])),(void *)(&(((unsigned char *)bssp->wpa_ie)[8])), 4);
                        memcpy((void *)(&(((unsigned char *)bssp->wpa_ie)[18])), (void *)(&(((unsigned char *)ie)[18 + 4 * (CipherCnt - 1)])), bssp->wpa_ie_len - 8 - (CipherCnt - 1) * 4);

                        (((unsigned char *)bssp->wpa_ie)[1]) -=  ((CipherCnt - 1) * 4);
                        bssp->wpa_ie_len -=  ((CipherCnt - 1) * 4); 

                        #endif
                   }
                   else
                   {
                        if(((char*)ie)[17 + i * 4] == 2) //TKIP 
                        {
                            bssp->pcipher_type = IW_AUTH_CIPHER_TKIP;
                        }
                        else if(((char*)ie)[17 + i * 4] == 4) //CCMP
                        {
                            bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                        }
                        else
                        {
                            bssp->pcipher_type = IW_AUTH_CIPHER_CCMP;
                        }

                        #if 1
                        ((unsigned short *)bssp->wpa_ie)[6] = 1;
                        memcpy((void *)(&(((unsigned char *)bssp->wpa_ie)[14])),(void *)(&(((unsigned char *)ie)[14 + i * 4])), 4);
                        memcpy((void *)(&(((unsigned char *)bssp->wpa_ie)[18])), (void *)(&(((unsigned char *)ie)[18 + 4 * (CipherCnt - 1)])), bssp->wpa_ie_len - 8 - (CipherCnt - 1) * 4);

                        (((unsigned char *)bssp->wpa_ie)[1]) -=  ((CipherCnt - 1) * 4);
                        bssp->wpa_ie_len -=  ((CipherCnt - 1) * 4);

                        #endif
                   }
    			}
                ret = 0;
    		    break;
			}
        }    
    }       

    if ((ENCODE_TYPE_WEP == bssp->encode_type) || (ENCODE_TYPE_NO == bssp->encode_type))
    {
        ret = 0;
    }
	
	return ret;
}


int ieee80211_frequency_to_channel(int freq)
{
	/* see 802.11 17.3.8.3.2 and Annex J */
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else
		return (freq - 5000) / 5;
}

static void wl_scan_prep(struct wl_scan_params *params, void *request)
{
	uint32 n_ssids;
	uint32 n_channels;
	uint16 channel;
	chanspec_t chanspec;
	int i, offset;
	char *ptr;
	wlc_ssid_t ssid;
	uint16 center_freq[14] = {2412, 2417, 2422, 2427, 2432,
	                          2437, 2442, 2447, 2452, 2457,
	                          2462, 2467, 2472, 2484};
	
	memset(&params->ssid, 0, sizeof(wlc_ssid_t));
	memcpy(&params->bssid, &ether_bcast, ETHER_ADDR_LEN);
	params->bss_type = DOT11_BSSTYPE_ANY;
	params->scan_type = 0;
	params->nprobes = -1;
	params->active_time = 200; // terence 20120609: extend active scan time interval
	params->passive_time = -1;
	params->home_time = -1;
	params->channel_num = 0;

	BCMDEBUG("Preparing Scan request\n");
	BCMDEBUG("nprobes=%d\n", params->nprobes);
	BCMDEBUG("active_time=%d\n", params->active_time);
	BCMDEBUG("passive_time=%d\n", params->passive_time);
	BCMDEBUG("home_time=%d\n", params->home_time);
	BCMDEBUG("scan_type=%d\n", params->scan_type);

	params->nprobes = htod32(params->nprobes);
	params->active_time = htod32(params->active_time);
	params->passive_time = htod32(params->passive_time);
	params->home_time = htod32(params->home_time);

	/* if request is null just exit so it will be all channel broadcast scan */
	n_ssids = 1;
	n_channels = 14;

	/* Copy channel array if applicable */
	BCMDEBUG("### List of channelspecs to scan ###\n");
	if (n_channels > 0) 
	{
		for (i = 0; i < n_channels; i++) 
		{
			chanspec = 0;
			channel = ieee80211_frequency_to_channel(center_freq[i]);
			#if 0
				if (request->channels[i]->band == IEEE80211_BAND_2GHZ)
					chanspec |= WL_CHANSPEC_BAND_2G;
				else
					chanspec |= WL_CHANSPEC_BAND_5G;

				if (request->channels[i]->flags & IEEE80211_CHAN_NO_HT40) 
				{
					chanspec |= WL_CHANSPEC_BW_20;
					chanspec |= WL_CHANSPEC_CTL_SB_NONE;
				} 
				else 
				{
					chanspec |= WL_CHANSPEC_BW_40;
					if (request->channels[i]->flags & IEEE80211_CHAN_NO_HT40PLUS)
						chanspec |= WL_CHANSPEC_CTL_SB_LOWER;
					else
						chanspec |= WL_CHANSPEC_CTL_SB_UPPER;
				}
	        #endif
			params->channel_list[i] = channel;
			params->channel_list[i] &= WL_CHANSPEC_CHAN_MASK;
			//params->channel_list[i] |= chanspec;
			BCMDEBUG("Chan : %d, Channel spec: %x \n",channel, params->channel_list[i]);
			params->channel_list[i] = htod16(params->channel_list[i]);
		}
	} 
	else 
	{
		BCMDEBUG("Scanning all channels\n");
	}

	/* Copy ssid array if applicable */
	BCMDEBUG("### List of SSIDs to scan ###\n");
	if (n_ssids > 0) 
	{
		offset = offsetof(wl_scan_params_t, channel_list) + n_channels * sizeof(uint16);
		offset = ROUNDUP(offset, sizeof(uint32));
		ptr = (char*)params + offset;
		for (i = 0; i < n_ssids; i++) 
		{
			memset(&ssid, 0, sizeof(wlc_ssid_t));
			#if 0
			ssid.SSID_len = request->ssids[i].ssid_len;
			memcpy(ssid.SSID, request->ssids[i].ssid, ssid.SSID_len);
			if (!ssid.SSID_len)
				WL_SCAN(("%d: Broadcast scan\n", i));
			else
				WL_SCAN(("%d: scan  for  %s size =%d\n", i,
				ssid.SSID, ssid.SSID_len));
			#endif
			memcpy(ptr, &ssid, sizeof(wlc_ssid_t));
			ptr += sizeof(wlc_ssid_t);
		}
	} 
	else 
	{
		BCMDEBUG("Broadcast scan\n");
	}
	/* Adding mask to channel numbers */
	params->channel_num = htod32((n_ssids << WL_SCAN_PARAMS_NSSID_SHIFT) | (n_channels & WL_SCAN_PARAMS_COUNT_MASK));
}

static int wl_iw_escan_set_scan(dhd_pub_t *pub)
{
	int ret = 0;
	uint32 n_channels = 14;
	uint32 n_ssids = 1;
	int params_size;
	wl_escan_params_t *params = NULL;
    char * escan_ioctl_buf= NULL;

    escan_ioctl_buf = sys_malloc(WLC_IOCTL_MEDLEN);

	/* WL_SCAN_PARAMS_FIXED_SIZE == sizeof(wl_scan_params_t)*/
	params_size = (OFFSETOF(wl_escan_params_t, params) + WL_SCAN_PARAMS_FIXED_SIZE);

	/* Allocate space for populating ssids in wl_iscan_params struct */
	/* If n_channels is odd, add a padd of u16 */
	if (n_channels % 2)
	{
		params_size += sizeof(uint16) * (n_channels + 1);
	}
	else
	{
		params_size += sizeof(uint16) * n_channels;
	}

	/* Allocate space for populating ssids in wl_iscan_params struct */
    params_size += sizeof(struct wlc_ssid) * n_ssids;
    params = (wl_escan_params_t *)sys_malloc(params_size);

	params->version = htod32(ESCAN_REQ_VERSION);
	params->action =  htod16(WL_SCAN_ACTION_START);
	params->sync_id = htod16(0x1234);
	
    /* parm scan setting..*/
	wl_scan_prep(&params->params, NULL);

	if (params_size + sizeof("escan") >= WLC_IOCTL_MEDLEN) 
	{
		BCMDEBUG("ioctl buffer length not sufficient\n");
		sys_free(params);
		ret = -ENOMEM;
		goto set_scan_end;
	}
	
	ret = wldev_iovar_setbuf(pub, "escan", params, params_size, escan_ioctl_buf, WLC_IOCTL_MEDLEN, NULL);
	if (ret)
	{
		BCMDEBUG("Escan set error (%d)\n", ret);
	}
	sys_free(params);

set_scan_end:
    sys_free(escan_ioctl_buf);
	return ret;
}



int32 wl_escan_handler(const wl_event_msg_t *e, void *data)
{
    int32 ret, status;
	wl_bss_info_t *bi;
	wl_escan_result_t *escan_result;
	uint32 bi_length;
	int i,j;
    struct bss_descriptor *bssnew = NULL;
    uint8 isFound = 0;

    bssnew = sys_calloc(sizeof(struct bss_descriptor));

	status = ntoh32(e->status);
	if (status == WLC_E_STATUS_PARTIAL)
    {
		escan_result = (wl_escan_result_t *)data;
		if (!escan_result) 
		{
			goto exit;
		}
		if (dtoh16(escan_result->bss_count) != 1) 
		{
			goto exit;
		}
		bi = escan_result->bss_info;
		if (bi == NULL) 
		{
			goto exit;
		}
		bi_length = dtoh32(bi->length);
		if (bi_length != (dtoh32(escan_result->buflen) - WL_ESCAN_RESULTS_FIXED_SIZE)) 
		{
			goto exit;
		}
		if (dtoh16(bi->capability) & DOT11_CAP_IBSS) //dgl Ignoring ADHOC IBSS
		{
			goto exit;
		}
		if (!bi->SSID_len) 
		{
			printf("hidden SSID:BSSID="MACSTR", chan=%d, RSSI=%d\n",
				   MAC2STR(bi->BSSID.octet), bi->ctl_ch, dtoh16(bi->RSSI));
			goto exit;//dgl no the hidden ssid....
		} 
		else
		{
			printf("BSSID="MACSTR", chan=%d, RSSI=%d, SSID=%s\n",
			         MAC2STR(bi->BSSID.octet), bi->ctl_ch, dtoh16(bi->RSSI), bi->SSID);
		} 

		
		/* AP mac addr*/
        memcpy((void*)bssnew->bssid, (void*)&bi->BSSID, ETHER_ADDR_LEN);
		
		/*scan AP SSID Name..*/
        bssnew->ssid_len = dtoh32(bi->SSID_len);
        memcpy((void*)bssnew->ssid, (void*)&bi->SSID, dtoh32(bi->SSID_len));

		bssnew->rssi = -bi->RSSI;
		bssnew->channel = bi->ctl_ch ? bi->ctl_ch : CHSPEC_CHANNEL(bi->chanspec);
		
		if (dtoh16(bi->capability) & (DOT11_CAP_ESS | DOT11_CAP_IBSS)) 
		{
			if (dtoh16(bi->capability) & DOT11_CAP_ESS)
			{
                bssnew->mode = IW_MODE_INFRA;
			}
			else
			{
                bssnew->mode = IW_MODE_ADHOC;
			}
		}

        bssnew->encode_type = ENCODE_TYPE_NO;

		if (dtoh16(bi->capability) & DOT11_CAP_PRIVACY)
		{
			 bssnew->encode_type = ENCODE_TYPE_WEP; //只要启动wep wpa wpa2 中任一种都会进来
             bssnew->encode_flag = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
		}
		else
		{
            bssnew->encode_flag = IW_ENCODE_DISABLED;
		}
		
		ret = wl_iw_handle_scanresults_ies(bssnew, NULL, NULL, bi);
        if(ret < 0)
        {
			printf("clear ssid:%s\n", bssnew->ssid);
            goto exit;  
        }

		/* filter the repeated bssid. */
		isFound = 0;
        for (j=0; j< bss_descriptor_info.total_num; j++)
        {
            if(is_same_network(bssnew, &bss_descriptor_info.bss[j]))
            {
				if (bss_descriptor_info.bss[j].rssi > bssnew->rssi)
				{
					bss_descriptor_info.bss[j].rssi = bssnew->rssi;
				}
                isFound = 1;
                break;
            }
        }
        if ((!isFound) && ((bss_descriptor_info.total_num + 1) < MAX_BSS))
        {
            memcpy(&(bss_descriptor_info.bss[bss_descriptor_info.total_num]), bssnew, sizeof(struct bss_descriptor));
			for (i = bss_descriptor_info.total_num; i > 0; i--)
			{
				if (bss_descriptor_info.bss[i].rssi < bss_descriptor_info.bss[i-1].rssi)
				{
					memcpy(bssnew, &(bss_descriptor_info.bss[i]), sizeof(struct bss_descriptor));
					memcpy(&(bss_descriptor_info.bss[i]), &(bss_descriptor_info.bss[i-1]), sizeof(struct bss_descriptor));
					memcpy(&(bss_descriptor_info.bss[i-1]), bssnew, sizeof(struct bss_descriptor));
				}
			}
			
            bss_descriptor_info.cur_id++;
            bss_descriptor_info.total_num++;
        }
	}
	else if ((status == WLC_E_STATUS_SUCCESS) || (status == WLC_E_STATUS_ABORT))
    {
		//	wl_inform_bss(wl);
		//	wl_notify_escan_complete(wl, ndev, false);
		//	mutex_unlock(&wl->usr_sync);
        isEscanSuccess = 1;
	}
	else
    {
        isEscanSuccess = 1;
	}
exit:
    sys_free(bssnew);
	
	return BCME_OK;
}

#define WL_JOIN_PARAMS_MAX 1600

static int wl_iw_set_essid(dhd_pub_t *pub,struct iw_request_info *info, struct iw_point *dwrq, char *extra)
{
	int error;
	wl_join_params_t *join_params;
	int join_params_size;

	if (!extra) 
		return -EINVAL;

	join_params = sys_malloc(WL_JOIN_PARAMS_MAX);
	if (!join_params) 
	{
		return -ENOMEM;
	}
	memset(join_params, 0, WL_JOIN_PARAMS_MAX);

	memset(&g_ssid, 0, sizeof(g_ssid));

	if (dwrq->length && extra) 
	{
		g_ssid.SSID_len = MIN(sizeof(g_ssid.SSID), dwrq->length);
		memcpy(g_ssid.SSID, extra, g_ssid.SSID_len);
	} 
	else 
	{
		g_ssid.SSID_len = 0;
	}
	g_ssid.SSID_len = htod32(g_ssid.SSID_len);
	
	memset(join_params, 0, sizeof(*join_params));
	join_params_size = sizeof(join_params->ssid);

	memcpy(join_params->ssid.SSID, g_ssid.SSID, g_ssid.SSID_len);
	join_params->ssid.SSID_len = htod32(g_ssid.SSID_len);
	memcpy(&(join_params->params.bssid), &ether_bcast, ETHER_ADDR_LEN);

	wl_iw_ch_to_chanspec(g_wl_iw_params.target_channel, join_params, &join_params_size);

	error = dhd_wl_ioctl_cmd(pub, WLC_SET_SSID, join_params, join_params_size, 0);
	if (error) 
	{
		BCMDEBUG("Invalid ioctl data=%d\n", error);
		return error;
	}

	if (g_ssid.SSID_len) 
	{
		BCMDEBUG("join SSID=%s ch=%d\n", g_ssid.SSID,  g_wl_iw_params.target_channel);
	}
	sys_free(join_params);
	
	return error;
}


int rk_join_bss(struct bss_descriptor* bss)
{
    wl_extjoin_params_t *ext_join_params;
	size_t join_params_size;
	uint32 chan_cnt = 1;
    int err = 0;
    char * join_ioctl_buf;

    join_ioctl_buf = sys_malloc(WLC_IOCTL_MEDLEN);
    /*
	 *  Join with specific BSSID and cached SSID
	 *  If SSID is zero join based on BSSID only
	 */
	join_params_size = WL_EXTJOIN_PARAMS_FIXED_SIZE + chan_cnt * sizeof(chanspec_t);
	ext_join_params =  (wl_extjoin_params_t*)sys_calloc(join_params_size);
	if (ext_join_params == NULL) 
	{
		err = -ENOMEM;
		goto exit;
	}

	memcpy(&ext_join_params->ssid.SSID, bss->ssid, bss->ssid_len);
	ext_join_params->ssid.SSID_len = htod32(bss->ssid_len);
	
	/* Set up join scan parameters */
	ext_join_params->scan.scan_type = -1;
	ext_join_params->scan.nprobes = 2;
	
	/* increate dwell time to receive probe response or detect Beacon
	* from target AP at a noisy air only during connect command
	*/
    #define WL_SCAN_ACTIVE_TIME	 40
    #define WL_SCAN_PASSIVE_TIME	130
	ext_join_params->scan.active_time = WL_SCAN_ACTIVE_TIME*3;
	ext_join_params->scan.passive_time = WL_SCAN_PASSIVE_TIME*3;
	ext_join_params->scan.home_time = -1;

	if (bss->bssid)
	{
		memcpy(&ext_join_params->assoc.bssid, bss->bssid, ETH_ALEN);
	}
	else
	{
		memcpy(&ext_join_params->assoc.bssid, &ether_bcast, ETH_ALEN);
	}
	
	ext_join_params->assoc.chanspec_num = chan_cnt;
	if (chan_cnt) 
	{
		uint16 channel, band, bw, ctl_sb;
		chanspec_t chspec;
		channel = bss->channel;
		band = (channel <= CH_MAX_2G_CHANNEL) ? WL_CHANSPEC_BAND_2G : WL_CHANSPEC_BAND_5G;
		bw = WL_CHANSPEC_BW_20;
		ctl_sb = WL_CHANSPEC_CTL_SB_NONE;
		chspec = (channel | band | bw | ctl_sb);
		ext_join_params->assoc.chanspec_list[0] &= WL_CHANSPEC_CHAN_MASK;
		ext_join_params->assoc.chanspec_list[0] |= chspec;
		ext_join_params->assoc.chanspec_list[0] = htodchanspec(ext_join_params->assoc.chanspec_list[0]);
	}
	ext_join_params->assoc.chanspec_num = htod32(ext_join_params->assoc.chanspec_num);
	if (ext_join_params->ssid.SSID_len < IEEE80211_MAX_SSID_LEN) 
	{
		BCMDEBUG("ssid \"%s\", len (%d)\n", ext_join_params->ssid.SSID, ext_join_params->ssid.SSID_len);
	}

	err = wldev_iovar_setbuf(G_dhd, "join", ext_join_params, join_params_size, 
		                     join_ioctl_buf, WLC_IOCTL_MEDLEN, NULL);
	printf("jb=%d\n", err);
exit:
    sys_free(join_ioctl_buf);

    return err;
}


static int wl_iw_set_txpow(dhd_pub_t *pub,struct iw_request_info *info,struct iw_param *vwrq,char *extra)
{
	int error, disable;
	uint16 txpwrmw = 180;//dgl 120 150 180(good)

	disable = WL_RADIO_SW_DISABLE << 16;
	disable = htod32(disable);
	error = dhd_wl_ioctl_cmd(pub, WLC_SET_RADIO, &disable, sizeof(disable), 1);

	error = dev_wlc_intvar_set(pub, "qtxpower", (int)(bcm_mw_to_qdbm(txpwrmw)));
	return error;
}

static int wl_iw_get_txpow(dhd_pub_t *pub, struct iw_request_info *info,struct iw_param *vwrq, char *extra)
{
	int error, disable, txpwrdbm;
	uint8 result;

	if ((error = dhd_wl_ioctl_cmd(pub, WLC_GET_RADIO, &disable, sizeof(disable), 0)) ||
	    (error = dev_wlc_intvar_get(pub, "qtxpower", &txpwrdbm)))
	{
		return error;
	}

	disable = dtoh32(disable);
	result = (uint8)(txpwrdbm & ~WL_TXPWR_OVERRIDE);

	return 0;
}

static int wl_iw_set_encode(dhd_pub_t *pub, struct iw_request_info *info, struct iw_point *dwrq, char *extra)
{
	wl_wsec_key_t key;
	int error, val, wsec;

	BCMDEBUG("SIOCSIWENCODE index %d, len %d, flags %04x (%s%s%s%s%s)\n",
              dwrq->flags & IW_ENCODE_INDEX, dwrq->length, dwrq->flags,
              dwrq->flags & IW_ENCODE_NOKEY ? "NOKEY" : "",
              dwrq->flags & IW_ENCODE_DISABLED ? " DISABLED" : "",
              dwrq->flags & IW_ENCODE_RESTRICTED ? " RESTRICTED" : "",
              dwrq->flags & IW_ENCODE_OPEN ? " OPEN" : "",
              dwrq->flags & IW_ENCODE_TEMP ? " TEMP" : "");

	memset(&key, 0, sizeof(key));
	
    if (dwrq->flags & IW_ENCODE_DISABLED)
    {
        goto encode_disable;
    }  
    
    if ((dwrq->flags & IW_ENCODE_INDEX) == 0) 
	{
        for (key.index = 1; key.index <= DOT11_MAX_DEFAULT_KEYS; key.index++) 
		{
			val = htod32(key.index);
			error = dhd_wl_ioctl_cmd(pub, WLC_GET_KEY_PRIMARY, &val, sizeof(val), 0);
            //printf("error = %d, val = %d\n", error, val);
            if (error)
			{                
                return error;
			}
			val = dtoh32(val);
			if (val)
			{
				break;
			}
        }
        
		if (key.index == DOT11_MAX_DEFAULT_KEYS)
		{
			key.index = 0;
		}
	} 
	else 
	{
		key.index = (dwrq->flags & IW_ENCODE_INDEX) - 1;
		if (key.index >= DOT11_MAX_DEFAULT_KEYS)
			return -EINVAL;
	}

    printf("key.index = %d\n", key.index);
	
	if (!extra || !dwrq->length || (dwrq->flags & IW_ENCODE_NOKEY)) 
	{
		val = htod32(key.index);
		error = dhd_wl_ioctl_cmd(pub, WLC_SET_KEY_PRIMARY, &val, sizeof(val), 1);        
		if (error)
		{          
            return error;
		}
	} 
	else 
	{
		key.len = dwrq->length;

		if (dwrq->length > sizeof(key.data))
			return -EINVAL;

		memcpy(key.data, extra, dwrq->length);//copy password..

		key.flags = WL_PRIMARY_KEY;
		switch (key.len) 
		{
		case WEP1_KEY_SIZE:
			key.algo = CRYPTO_ALGO_WEP1;
			break;
			
		case WEP128_KEY_SIZE:
			key.algo = CRYPTO_ALGO_WEP128;
			break;
			
		case TKIP_KEY_SIZE:
			key.algo = CRYPTO_ALGO_TKIP;
			break;
			
		case AES_KEY_SIZE:
			key.algo = CRYPTO_ALGO_AES_CCM;
			break;
			
		default:
			return -EINVAL;
		}

		swap_key_from_BE(&key);
		error = dhd_wl_ioctl_cmd(pub, WLC_SET_KEY, &key, sizeof(key), 0);
		if (error)
		{
            printf("set key error\n");
            return error;
		}
	}

encode_disable:
	val = (dwrq->flags & IW_ENCODE_DISABLED) ? 0 : WEP_ENABLED;
	
	error = dev_wlc_intvar_get(pub, "wsec", &wsec);
	if (error)
		return error;

	wsec &= ~(WEP_ENABLED);
	wsec |= val;
	
	error = dev_wlc_intvar_set(pub, "wsec", wsec);
	if (error)
		return error;
	
	val = (dwrq->flags & IW_ENCODE_RESTRICTED) ? 1 : 0;
	val = htod32(val);
	error = dhd_wl_ioctl_cmd(pub, WLC_SET_AUTH, &val, sizeof(val), 0);
	if (error)
		return error;

	return 0;
}


static uint32 wl_iw_create_wpaauth_wsec(wl_iw_t *iw)
{
	uint32 wsec;
	
	if (iw->pcipher & (IW_AUTH_CIPHER_WEP40 | IW_AUTH_CIPHER_WEP104))
		wsec = WEP_ENABLED;
	else if (iw->pcipher & IW_AUTH_CIPHER_TKIP)
		wsec = TKIP_ENABLED;
	else if (iw->pcipher & IW_AUTH_CIPHER_CCMP)
		wsec = AES_ENABLED;
	else
		wsec = 0;

	
	if (iw->gcipher & (IW_AUTH_CIPHER_WEP40 | IW_AUTH_CIPHER_WEP104))
		wsec |= WEP_ENABLED;
	else if (iw->gcipher & IW_AUTH_CIPHER_TKIP)
		wsec |= TKIP_ENABLED;
	else if (iw->gcipher & IW_AUTH_CIPHER_CCMP)
		wsec |= AES_ENABLED;
	
	if (wsec == 0 && iw->privacy_invoked)
		wsec = WEP_ENABLED;

	BCMDEBUG("returning wsec of %d\n", wsec);

	return wsec;
}

static int
wl_iw_set_wpaauth(dhd_pub_t *pub,struct iw_request_info *info,struct iw_param *vwrq,char *extra)
{
	int error = 0;
	int paramid;
	int paramval;
	int val = 0;
    wl_iw_t iw_temp;
	wl_iw_t *iw =&iw_temp;

    memset(iw, 0, sizeof(wl_iw_t));

    if(g_connect_bss->encode_type == ENCODE_TYPE_WPA)
    {
        iw->wpaversion =  IW_AUTH_WPA_VERSION_WPA;
    }
    else if(g_connect_bss->encode_type == ENCODE_TYPE_WPA2)
    {
        iw->wpaversion = IW_AUTH_WPA_VERSION_WPA2;
    }
    else if(g_connect_bss->encode_type == ENCODE_TYPE_WPA_WPA2)
    {
        iw->wpaversion = IW_AUTH_WPA_VERSION_WPA2;
    }
    else
    {
        iw->wpaversion = WPA_AUTH_DISABLED;
    }

	paramid = vwrq->flags & IW_AUTH_INDEX;
	paramval = vwrq->value;

	BCMDEBUG("SIOCSIWAUTH, %s(%d), paramval = 0x%0x\n",
		paramid == IW_AUTH_WPA_VERSION ? "IW_AUTH_WPA_VERSION" :
		paramid == IW_AUTH_CIPHER_PAIRWISE ? "IW_AUTH_CIPHER_PAIRWISE" :
		paramid == IW_AUTH_CIPHER_GROUP ? "IW_AUTH_CIPHER_GROUP" :
		paramid == IW_AUTH_KEY_MGMT ? "IW_AUTH_KEY_MGMT" :
		paramid == IW_AUTH_TKIP_COUNTERMEASURES ? "IW_AUTH_TKIP_COUNTERMEASURES" :
		paramid == IW_AUTH_DROP_UNENCRYPTED ? "IW_AUTH_DROP_UNENCRYPTED" :
		paramid == IW_AUTH_80211_AUTH_ALG ? "IW_AUTH_80211_AUTH_ALG" :
		paramid == IW_AUTH_WPA_ENABLED ? "IW_AUTH_WPA_ENABLED" :
		paramid == IW_AUTH_RX_UNENCRYPTED_EAPOL ? "IW_AUTH_RX_UNENCRYPTED_EAPOL" :
		paramid == IW_AUTH_ROAMING_CONTROL ? "IW_AUTH_ROAMING_CONTROL" :
		paramid == IW_AUTH_PRIVACY_INVOKED ? "IW_AUTH_PRIVACY_INVOKED" :
		"UNKNOWN",
		paramid, paramval);


	switch (paramid) 
	{
	case IW_AUTH_WPA_VERSION:
		
		if (paramval & IW_AUTH_WPA_VERSION_DISABLED)
			val = WPA_AUTH_DISABLED;
		else if (paramval & (IW_AUTH_WPA_VERSION_WPA))
			val = WPA_AUTH_PSK | WPA_AUTH_UNSPECIFIED;
		else if (paramval & IW_AUTH_WPA_VERSION_WPA2)
			val = WPA2_AUTH_PSK | WPA2_AUTH_UNSPECIFIED;
		BCMDEBUG("setting wpa_auth to 0x%0x\n", val);
		if ((error = dev_wlc_intvar_set(pub, "wpa_auth", val)))
			return error;
		break;

	case IW_AUTH_CIPHER_PAIRWISE:
		iw->pcipher = paramval;
		val = wl_iw_create_wpaauth_wsec(iw);
		if ((error = dev_wlc_intvar_set(pub, "wsec", val)))
			return error;
		break;

	case IW_AUTH_CIPHER_GROUP://DGL step 2
		iw->gcipher = paramval;
		val = wl_iw_create_wpaauth_wsec(iw);
		if ((error = dev_wlc_intvar_set(pub, "wsec", val)))
			return error;
		break;

	case IW_AUTH_KEY_MGMT://DGL step 3
		if ((error = dev_wlc_intvar_get(pub, "wpa_auth", &val)))
			return error;

		if (val & (WPA_AUTH_PSK | WPA_AUTH_UNSPECIFIED)) 
		{
			if (paramval & IW_AUTH_KEY_MGMT_PSK)
				val = WPA_AUTH_PSK;
			else
				val = WPA_AUTH_UNSPECIFIED;
			if (paramval & 0x04)
				val |= WPA2_AUTH_FT;
		}
		else if (val & (WPA2_AUTH_PSK | WPA2_AUTH_UNSPECIFIED)) 
		{
			if (paramval & IW_AUTH_KEY_MGMT_PSK)
				val = WPA2_AUTH_PSK;
			else
				val = WPA2_AUTH_UNSPECIFIED;
			if (paramval & 0x04)
				val |= WPA2_AUTH_FT;
		}
		else if (paramval & IW_AUTH_KEY_MGMT_PSK) 
		{
			if (iw->wpaversion == IW_AUTH_WPA_VERSION_WPA)
				val = WPA_AUTH_PSK;
			else if (iw->wpaversion == IW_AUTH_WPA_VERSION_WPA2)
				val = WPA2_AUTH_PSK;
			else 
				val = WPA_AUTH_DISABLED;
		} 
		else if (paramval & IW_AUTH_KEY_MGMT_802_1X) 
		{
			if (iw->wpaversion == IW_AUTH_WPA_VERSION_WPA)
				val = WPA_AUTH_UNSPECIFIED;
			else if (iw->wpaversion == IW_AUTH_WPA_VERSION_WPA2)
				val = WPA2_AUTH_UNSPECIFIED;
			else 
				val = WPA_AUTH_DISABLED;
		}
		else
			val = WPA_AUTH_DISABLED;

		BCMDEBUG("setting wpa_auth to %d\n", val);
		if ((error = dev_wlc_intvar_set(pub, "wpa_auth", val)))
			return error;
		break;

	case IW_AUTH_TKIP_COUNTERMEASURES:
		dev_wlc_bufvar_set(pub, "tkip_countermeasures", (char *)&paramval, 1);
		break;

	case IW_AUTH_80211_AUTH_ALG:////DGL step 1
		BCMDEBUG("Setting the D11auth %d\n", paramval);
		if (paramval == IW_AUTH_ALG_OPEN_SYSTEM)
			val = 0;
		else if (paramval == IW_AUTH_ALG_SHARED_KEY)
			val = 1;
		else if (paramval == (IW_AUTH_ALG_OPEN_SYSTEM | IW_AUTH_ALG_SHARED_KEY))
			val = 2;
		else
			error = 1;
		if (!error && (error = dev_wlc_intvar_set(pub, "auth", val)))
			return error;
		break;

	case IW_AUTH_WPA_ENABLED:
		if (paramval == 0) 
		{
			iw->privacy_invoked = 0; 
			iw->pcipher = 0;
			iw->gcipher = 0;
			val = wl_iw_create_wpaauth_wsec(iw);
			if ((error = dev_wlc_intvar_set(pub, "wsec", val)))
				return error;
			BCMDEBUG("setting wpa_auth to %d, wsec to %d\n",paramval, val);
			dev_wlc_intvar_set(pub, "wpa_auth", paramval);
			return error;
		}
		break;

	case IW_AUTH_DROP_UNENCRYPTED:
		if ((error = dev_wlc_intvar_set(pub, "wsec_restrict", paramval)))
			return error;
		break;

	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		dev_wlc_bufvar_set(pub, "rx_unencrypted_eapol", (char *)&paramval, 1);
		break;

	case IW_AUTH_ROAMING_CONTROL:
		BCMDEBUG("IW_AUTH_ROAMING_CONTROL\n");
		break;

	case IW_AUTH_PRIVACY_INVOKED:
		iw->privacy_invoked = paramval;
		val = wl_iw_create_wpaauth_wsec(iw);
		if ((error = dev_wlc_intvar_set(pub, "wsec", val)))
			return error;
		break;

	default:
		break;
	}
	return 0;
}


int wl_do_escan(void)
{
	int err = BCME_OK;
	int passive_scan = 0;//dgl set active scan..

	if (bss_descriptor_info.frist_scan == TRUE)
	{
		bss_descriptor_info.frist_scan = FALSE;
	    wl_iw_set_txpow(G_dhd, NULL, NULL, NULL);
	    wl_iw_get_txpow(G_dhd, NULL, NULL, NULL);
	}

	err = dhd_wl_ioctl_cmd(G_dhd, WLC_SET_PASSIVE_SCAN, &passive_scan, sizeof(passive_scan), 0);
    if (!err)
	{
		err = wl_iw_escan_set_scan(G_dhd);
		if (!err)
		{
			isEscanSuccess = 0;
		}
		else
		{
			printf("wl_do_escan 1ret = %d\n", err);
		}
	}
	else
	{
	    printf("wl_do_escan0 ret = %d\n", err);
	}
	
    return BCME_OK;
}

int rk_scan_work(void)
{  
	int err = BCME_OK;

	if (isEscanSuccess)
	{
		isEscanSuccess = 0;
		#if 0
		int i;
		for (i=0; i<bss_descriptor_info.total_num; i++)
		{
			printf("ssid=%s rssi=%d\n", bss_descriptor_info.bss[i].ssid, bss_descriptor_info.bss[i].rssi);
		}
		#endif
		printf("scan:%d\n", bss_descriptor_info.total_num);
		return bss_descriptor_info.total_num;
	}
	
	return -1;
}

int rk_get_scan_ssid(void* ssid, int index, UINT8 bwc)
{
	int i = 0;
	char * pssid8 = ssid;
	UINT16 * pssid16 = ssid;

	if (!ssid || index<0 || (index >= bss_descriptor_info.total_num))
	{
		return -1;
	}

	while ((i < IW_ESSID_MAX_SIZE) && (bss_descriptor_info.bss[index].ssid[i]))
	{
		if (bwc)
		{
			pssid16[i] = bss_descriptor_info.bss[index].ssid[i];
			pssid16[i+1] = 0;
		}
		else
		{
			pssid8[i] = bss_descriptor_info.bss[index].ssid[i];
			pssid8[i+1] = 0;
		}
		i++;
	};
	
	return 0;
}

int rk_get_ap_rssi(int i)
{
	return (bss_descriptor_info.bss[i].rssi);
}



int rk_get_encode_type(int index)
{
    if (index<0 || (index >= bss_descriptor_info.total_num))
	{
		return -1;
    }

    if(bss_descriptor_info.bss[index].encode_type == ENCODE_TYPE_NO)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

extern int    WiFiCurAPRssi;
int rk_check_scan_ssid(uint8 *ssid)
{
    int i;

    for(i = 0; i < bss_descriptor_info.total_num; i++)
    {    
        if (strcmp(bss_descriptor_info.bss[i].ssid, ssid) == 0)
        {
			WiFiCurAPRssi = dtoh32(bss_descriptor_info.bss[i].rssi);
            return i;
        }
    }
    return (int)(-1);
}

int rk_get_scan_ssid_total(void)
{
	return bss_descriptor_info.total_num;
}


#define WEP_KEY_INDEX_1 1
#define WEP_KEY_INDEX_2 2
#define WEP_KEY_INDEX_3 3
#define WEP_KEY_INDEX_4 4

int rk_wep_connect(struct bss_descriptor * bss, char * password, int pwd_len)
{
	int ret;
	uint32 wifi_mode = bss->mode;
    struct iw_point dwrq;
    struct iw_freq fwrq;
	
    wl_iw_set_mode(G_dhd, NULL, &wifi_mode ,NULL);  

    //if (pwd_len == 0)
    if(bss->encode_type == ENCODE_TYPE_NO)
    {
        dwrq.flags = IW_ENCODE_DISABLED;
    }
    else
    {
        dwrq.flags = 0;
        dwrq.flags |=  WEP_KEY_INDEX_1 ;
        dwrq.flags |= IW_ENCODE_RESTRICTED;
        dwrq.length = pwd_len;
    }

    wl_iw_set_encode(G_dhd, NULL, &dwrq, password);

    fwrq.e = 0;
    fwrq.m = bss->channel;
    BCMDEBUG("wl_iw_set_freq");
    wl_iw_set_freq(G_dhd, NULL, &fwrq, NULL);

    dwrq.length = bss->ssid_len;
	ret = wl_iw_set_essid(G_dhd, NULL, &dwrq, (char*)bss->ssid);
	//ret = 0;
    return ret;
}


int rk_wpa_connect(struct bss_descriptor * bss, char * password, int pwd_len)
{
    int err;
    uint32 wifi_mode;
    struct iw_point dwrq;
    struct iw_freq fwrq;
	struct iw_param vwrq;

	wifi_mode = bss->mode;//IW_MODE_INFRA
    err = wl_iw_set_mode(G_dhd, NULL, &wifi_mode, NULL);
	if (err)
	{
		printf("set_m=%d\n", err);
	}

    dwrq.flags = IW_ENCODE_DISABLED;
    err = wl_iw_set_encode(G_dhd, NULL, &dwrq, NULL); //disable wep
	if (err)
	{
		printf("set_e=%d\n", err);
	}
	
    fwrq.e = 0;
    fwrq.m = bss->channel;
    err = wl_iw_set_freq(G_dhd, NULL, &fwrq, NULL);
	if (err)
	{
		printf("set_f=%d\n", err);
	}

    if(bss->encode_type == ENCODE_TYPE_WPA)
    {   
        err = dev_wlc_bufvar_set(G_dhd, "wpaie", bss->wpa_ie, bss->wpa_ie_len);
    }
    else
    {
        err = dev_wlc_bufvar_set(G_dhd, "wpaie", bss->rsn_ie, bss->rsn_ie_len);
    }
	if (err)
	{
		printf("wl_iw_set_wpaie = %d\n", err);
	}
    
    if(bss->encode_type == ENCODE_TYPE_WPA)
    {
        BCMDEBUG("encode type is wpa");
    }
    else if(bss->encode_type == ENCODE_TYPE_WPA2)
    {
        BCMDEBUG("encode type is wpa2");
    }
    else if(bss->encode_type == ENCODE_TYPE_WPA_WPA2)
    {
        BCMDEBUG("encode type is wpa-wpa2");
    }

    if(bss->gcipher_type == IW_AUTH_CIPHER_CCMP)
    {
        BCMDEBUG("IW_AUTH_CIPHER_CCMP");     
    }
    else if(bss->gcipher_type == IW_AUTH_CIPHER_TKIP)
    {
        BCMDEBUG("IW_AUTH_CIPHER_TKIP");   
    }
    else
    {
        BCMDEBUG("no cipher type");   
    }
	
	//set auth type.open_system shared_key "802.11 AUTH"
    vwrq.flags = IW_AUTH_80211_AUTH_ALG;
    vwrq.value = IW_AUTH_ALG_OPEN_SYSTEM;   
    err = wl_iw_set_wpaauth(G_dhd, NULL, &vwrq, NULL);
	if (err)
	{
		printf("wl_iw_set_wpaauth-0 = %d\n", err);
	}

    //set cipher type.WEP TKIP CCMP
    vwrq.flags = IW_AUTH_CIPHER_PAIRWISE;
    vwrq.value =  bss->pcipher_type;//IW_AUTH_CIPHER_TKIP IW_AUTH_CIPHER_CCMP
    err = wl_iw_set_wpaauth(G_dhd, NULL, &vwrq, NULL);
	if (err)
	{
		printf("wl_iw_set_wpaauth-1 = %d\n", err);
	}
	
    //set cipher type.WEP TKIP CCMP
    vwrq.flags = IW_AUTH_CIPHER_GROUP;
    vwrq.value =  bss->gcipher_type;//IW_AUTH_CIPHER_TKIP IW_AUTH_CIPHER_CCMP
    err = wl_iw_set_wpaauth(G_dhd, NULL, &vwrq, NULL);
	if (err)
	{
		printf("wl_iw_set_wpaauth-1 = %d\n", err);   
	}
    
	//
    vwrq.flags = IW_AUTH_KEY_MGMT;
    vwrq.value = IW_AUTH_KEY_MGMT_PSK;
    err = wl_iw_set_wpaauth(G_dhd, NULL, &vwrq, NULL);
	if (err)
	{
		printf("wl_iw_set_wpaauth-2 = %d\n", err);
	}
    
    err = rk_join_bss(bss);
    if (err == BCME_UNSUPPORTED) 
    {
		BCMDEBUG("join iovar is not supported\n");
        dwrq.flags = 0;
        dwrq.length = bss->ssid_len;
        return wl_iw_set_essid(G_dhd, NULL, &dwrq, bss->ssid);
	} 

    return err;
}


int rk_wifi_connect_init(char * ssid, char * password, int pwd_len)
{   
    int i;

    for(i=0; i< bss_descriptor_info.total_num ;i++)
    {
        if(! strcmp(ssid, bss_descriptor_info.bss[i].ssid) )
        {
            break;
        }
    }

    if (i == bss_descriptor_info.total_num)
    {
        BCMDEBUG("can not find bss\n");
        return -1;
    }

    memcpy(&_g_connect_bss_, &bss_descriptor_info.bss[i], sizeof(struct bss_descriptor));
	g_connect_bss = &_g_connect_bss_;
	
    memcpy(g_connect_bss->password, password, MAX_PASSWORD_LEN);
    g_connect_bss->password[pwd_len] = 0;

    return 0;
}

int rk_wifi_connect(char * ssid, char * password, int pwd_len)  
{
	printf("et:%d\n", g_connect_bss->encode_type);
    if (g_connect_bss->encode_type == ENCODE_TYPE_WEP || g_connect_bss->encode_type == ENCODE_TYPE_NO)
    {
        return rk_wep_connect(g_connect_bss, password, pwd_len);        
    }
    else
    {
        return rk_wpa_connect(g_connect_bss, password, pwd_len);
    }
}

