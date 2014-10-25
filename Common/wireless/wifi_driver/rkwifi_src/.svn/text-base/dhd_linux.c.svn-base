/*
 * Broadcom Dongle Host Driver (DHD), Linux-specific network interface
 * Basically selected code segments from usb-cdc.c and usb-rndis.c
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
 * $Id: dhd_linux.c 311494 2012-01-30 13:19:04Z $
 */


#include  "rk903.h"
//#include   "wifi_emi.h"

#define HTSF_MINLEN 200    /* min. packet length to timestamp */
#define HTSF_BUS_DELAY 150 /* assume a fix propagation in us  */
#define TSMAX  1000        /* max no. of timing record kept   */
#define NUMBIN 34

/* Set INIT_COUNTRY_CODE 
 * "US" ---> 11 channels, this is default setting. 
 * "EU" ---> 13 channels
 * "JP" ---> 14 channels
 */
char INIT_COUNTRY_CODE[] = "EU";

//static uint32 tsidx = 0;
//static uint32 htsf_seqnum = 0;
//uint32 tsfsync;
//static uint32 tsport = 5010;

typedef struct histo_ {
	uint32 bin[NUMBIN];
} histo_t;

extern bool ap_fw_loaded;


/* Idle timeout for backplane clock */
int dhd_idletime = DHD_IDLETIME_TICKS;

/* Use interrupts */
uint dhd_intr = TRUE;

/* SDIO Drive Strength (in milliamps) */
uint dhd_sdiod_drive_strength = 6;

/* Global Pkt filter enable control */
uint dhd_pkt_filter_enable = TRUE;
/* Control fw roaming */
uint dhd_roam_disable = 1;

extern dhd_prot_t g_dhd_prot_t;
extern dhd_pub_t  g_dhd_pub_t;

dhd_pub_t *dhd_attach(struct dhd_bus *bus, uint bus_hdrlen)
{
	dhd_pub_t * dhdp = &g_dhd_pub_t;
	
	memset(dhdp, 0, sizeof(dhd_pub_t));
	
	dhdp->bus = bus;
	memset(&g_dhd_prot_t, 0, sizeof(dhd_prot_t));
	dhdp->prot = &g_dhd_prot_t;
	dhdp->maxctl = WLC_IOCTL_MAXLEN;

	return dhdp;
}

/* For Android ICS MR2 release, the concurrent mode is enabled by default and the firmware
 * name would be fw_bcmdhd.bin. So we need to determine whether P2P is enabled in the STA
 * firmware and accordingly enable concurrent mode (Apply P2P settings). SoftAP firmware
 * would still be named as fw_bcmdhd_apsta.
 */
static uint32 dhd_concurrent_fw(dhd_pub_t *dhd)
{
	int ret = 0;
	char buf[WLC_IOCTL_SMLEN];

	/* Given path is for the STA firmware. Check whether P2P support is present in
	 * the firmware. If so, set mode as P2P (concurrent support).
	 */
	memset(buf, 0, sizeof(buf));
	bcm_mkiovar("p2p", 0, 0, buf, sizeof(buf));
	ret = dhd_wl_ioctl_cmd(dhd, WLC_GET_VAR, buf, sizeof(buf), 0);
	if (ret < 0) 
	{
		//printf("Get P2P failed (error=%d)\n", ret);
	} 
	else if (buf[0] == 1) 
	{
		//printf("P2P is supported\n");
		return 1;
	}
	return 0;
}

int dhd_preinit_ioctls(dhd_pub_t *dhd)
{
	int ret = 0;
	char eventmask[WL_EVENTING_MASK_LEN];
	char iovbuf[WL_EVENTING_MASK_LEN + 12];	/*  Room for "event_msgs" + '\0' + bitvec  */
	uint up = 0;
	uint power_mode = PM_FAST;
	uint32 dongle_align = DHD_SDALIGN;
	uint32 glom = 0;
	uint bcn_timeout = 10;
    uint retry_max = 15;
	//int arpoe = 1;
	int scan_assoc_time = DHD_SCAN_ACTIVE_TIME;
	int scan_unassoc_time = 40;
	int scan_passive_time = DHD_SCAN_PASSIVE_TIME;
	char buf[WLC_IOCTL_SMLEN];
	char *ptr;
	uint32 listen_interval = LISTEN_INTERVAL; /* Default Listen Interval in Beacons */
	//uint dtim = 1;
	uint32 mpc = 0; /* Turn MPC off for AP/APSTA mode */
	uint32 roamvar;
	uint32 sgi_tx, sgi_rx;
	uint32 apsta = 0; /* Enable APSTA mode */
	int res;

	dhd->op_mode = 0;
	
	/* Get the default device MAC address directly from firmware */
	memset(buf, 0, sizeof(buf));
	bcm_mkiovar("cur_etheraddr", 0, 0, buf, sizeof(buf));
	if ((ret = dhd_wl_ioctl_cmd(dhd, WLC_GET_VAR, buf, sizeof(buf), 0)) < 0) 
		return BCME_NOTUP;

	/* Update public MAC address after reading from Firmware */
	memcpy((void *)dhd->mac.octet, (void *)buf, ETHER_ADDR_LEN);
    wifi_set_mac_addr(buf);

	/* Check if firmware with WFD support used */
	if (dhd_concurrent_fw(dhd))
	{
		bcm_mkiovar("apsta", (char *)&apsta, 4, iovbuf, sizeof(iovbuf));
		ret = dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);
		if (ret < 0) 
		{
			BCMDEBUG("APSTA for WFD failed ret= %d\n", ret);
		} 
		else 
		{
			dhd->op_mode |= WFD_MASK;
			dhd_pkt_filter_enable = FALSE;
		}
	}

	/* Check if firmware with HostAPD support used */
	if ((dhd->op_mode != WFD_MASK) && (dhd->op_mode != HOSTAPD_MASK)) 
	{
		/* STA only operation mode */
		dhd->op_mode |= STA_MASK;
		dhd_pkt_filter_enable = TRUE;
	}

	printf("Firmware up: op_mode=%d, mac=%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
			dhd->op_mode,
			dhd->mac.octet[0], dhd->mac.octet[1], dhd->mac.octet[2],
			dhd->mac.octet[3], dhd->mac.octet[4], dhd->mac.octet[5]);

	/* Set Country code  */
	/*
	if (dhd->dhd_cspec.ccode[0] == 0) {
			memcpy(dhd->dhd_cspec.country_abbrev, "ALL", WLC_CNTRY_BUF_SZ);
			memcpy(dhd->dhd_cspec.ccode, "ALL", WLC_CNTRY_BUF_SZ);
			dhd->dhd_cspec.rev = 0;
	}*/
	
	memcpy(dhd->dhd_cspec.ccode, INIT_COUNTRY_CODE, WLC_CNTRY_BUF_SZ);
	if (dhd->dhd_cspec.ccode[0] != 0) 
	{
		bcm_mkiovar("country", (char *)&dhd->dhd_cspec, sizeof(wl_country_t), iovbuf, sizeof(iovbuf));
		ret = dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);
		if (ret < 0)
		{
			//printf("country code setting failed\n");
		}
		//printf("Wifi driver initialize Country Code : %s\n", INIT_COUNTRY_CODE);
	}

	/* Set Listen Interval */
	bcm_mkiovar("assoc_listen", (char *)&listen_interval, 4, iovbuf, sizeof(iovbuf));
	ret = dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);
	if (ret < 0)
	{
		//printf("assoc_listen failed %d\n", ret);
	}

	/* Set PowerSave mode */
	dhd_wl_ioctl_cmd(dhd, WLC_SET_PM, (char *)&power_mode, sizeof(power_mode), 1);

	/* Match Host and Dongle rx alignment */
	bcm_mkiovar("bus:txglomalign", (char *)&dongle_align, 4, iovbuf, sizeof(iovbuf));
	dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);

	/* disable glom option per default */
	bcm_mkiovar("bus:txglom", (char *)&glom, 4, iovbuf, sizeof(iovbuf));
	dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);

	/* Setup timeout if Beacons are lost and roam is off to report link down */
	bcm_mkiovar("bcn_timeout", (char *)&bcn_timeout, 4, iovbuf, sizeof(iovbuf));
	dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);

	/* Setup assoc_retry_max count to reconnect target AP in dongle */
	printf("0 retry max = %d\n", retry_max);
	bcm_mkiovar("assoc_retry_max", (char *)&retry_max, 4, iovbuf, sizeof(iovbuf));
	while(dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1))
	{
		DelayMs(5);
	    printf("retry max = %d\n", retry_max);
	}
	
	/* Turn off MPC in AP mode */
	bcm_mkiovar("mpc", (char *)&mpc, 4, iovbuf, sizeof(iovbuf));
	dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);

	bcm_mkiovar("apsta", (char *)&apsta, 4, iovbuf, sizeof(iovbuf));
	dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);

	/* Set Keep Alive : be sure to use FW with -keepalive */
	if (ap_fw_loaded == FALSE)
	{
		res = dhd_keep_alive_onoff(dhd);
		if (res < 0)
			BCMDEBUG("set keeplive failed %d\n", res);
	}

	/* Read event_msgs mask */
	bcm_mkiovar("event_msgs", eventmask, WL_EVENTING_MASK_LEN, iovbuf, sizeof(iovbuf));
	ret  = dhd_wl_ioctl_cmd(dhd, WLC_GET_VAR, iovbuf, sizeof(iovbuf), 0);
	if (ret < 0) 
	{
		//printf("read Event mask failed %d\n", ret);
		goto done;
	}
	bcopy(iovbuf, eventmask, WL_EVENTING_MASK_LEN);

	/* Setup event_msgs */
	setbit(eventmask, WLC_E_SET_SSID);
	setbit(eventmask, WLC_E_PRUNE);
	setbit(eventmask, WLC_E_AUTH);
	setbit(eventmask, WLC_E_REASSOC);
	setbit(eventmask, WLC_E_REASSOC_IND);
	setbit(eventmask, WLC_E_DEAUTH);
	setbit(eventmask, WLC_E_DEAUTH_IND);
	setbit(eventmask, WLC_E_DISASSOC_IND);
	setbit(eventmask, WLC_E_DISASSOC);
	setbit(eventmask, WLC_E_JOIN);
	setbit(eventmask, WLC_E_ASSOC_IND);
	setbit(eventmask, WLC_E_PSK_SUP);
	setbit(eventmask, WLC_E_LINK);
	setbit(eventmask, WLC_E_NDIS_LINK);
	setbit(eventmask, WLC_E_MIC_ERROR);
	setbit(eventmask, WLC_E_ASSOC_REQ_IE);
	setbit(eventmask, WLC_E_ASSOC_RESP_IE);
	setbit(eventmask, WLC_E_PMKID_CACHE);
	//setbit(eventmask, WLC_E_TXFAIL);
	setbit(eventmask, WLC_E_JOIN_START);
	setbit(eventmask, WLC_E_SCAN_COMPLETE);
	setbit(eventmask, WLC_E_PFN_NET_FOUND);
	/* enable dongle roaming event */
	//setbit(eventmask, WLC_E_ROAM); //turning off  roaming (gwl)
	setbit(eventmask, WLC_E_ASSOCREQ_IE);

	setbit(eventmask, WLC_E_ESCAN_RESULT);
	if ((dhd->op_mode & WFD_MASK) == WFD_MASK) 
	{
		setbit(eventmask, WLC_E_ACTION_FRAME_RX);
		setbit(eventmask, WLC_E_ACTION_FRAME_COMPLETE);
		setbit(eventmask, WLC_E_ACTION_FRAME_OFF_CHAN_COMPLETE);
		//setbit(eventmask, WLC_E_P2P_PROBREQ_MSG);//wp too much
		setbit(eventmask, WLC_E_P2P_DISC_LISTEN_COMPLETE);
	}
	/* Write updated Event mask */
	bcm_mkiovar("event_msgs", eventmask, WL_EVENTING_MASK_LEN, iovbuf, sizeof(iovbuf));
	if ((ret = dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1)) < 0) 
	{
		//printf("Set Event mask failed %d\n", ret);
		goto done;
	}


    BCMDEBUG("WLC_SET_SCAN_CHANNEL_TIME");
	dhd_wl_ioctl_cmd(dhd, WLC_SET_SCAN_CHANNEL_TIME, (char *)&scan_assoc_time, sizeof(scan_assoc_time), 1);
	dhd_wl_ioctl_cmd(dhd, WLC_SET_SCAN_UNASSOC_TIME, (char *)&scan_unassoc_time, sizeof(scan_unassoc_time), 1);
	dhd_wl_ioctl_cmd(dhd, WLC_SET_SCAN_PASSIVE_TIME, (char *)&scan_passive_time, sizeof(scan_passive_time), 1);

	/*dgl move to here for Force STA UP will fail.*/
	roamvar = dhd_roam_disable;
	bcm_mkiovar("roam_off", (char *)&roamvar, 4, iovbuf, sizeof(iovbuf));
	dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);

	/* Force STA UP */
	if ((ret = dhd_wl_ioctl_cmd(dhd, WLC_UP, (char *)&up, sizeof(up), 1)) < 0) 
	{
		printf("Setting WL UP failed %d\n", ret);
		goto done;
	}

	/* query for 'ver' to get version info from firmware */
	memset(buf, 0, sizeof(buf));
	ptr = buf;
	bcm_mkiovar("ver", (char *)&buf, 4, buf, sizeof(buf));
	ret = dhd_wl_ioctl_cmd(dhd, WLC_GET_VAR, buf, sizeof(buf), 0);
	if (ret < 0)
	{
		printf("failed %d\n", ret);
	}
	else 
	{
		//bcmstrtok(&ptr, "\n", 0);
		/* Print fw version info */
		//printf("Firmware version = %s\n", buf);

		//Firmware version = wl0: Jul 31 2012 10:45:17 version 5.90.195.26.3.1_edon/off_0xb8bf FWID 01-ff91e1fa
	}
	#if 0
	roamvar = dhd_roam_disable;
	bcm_mkiovar("roam_off", (char *)&roamvar, 4, iovbuf, sizeof(iovbuf));
	dhd_wl_ioctl_cmd(dhd, WLC_SET_VAR, iovbuf, sizeof(iovbuf), 1);
	#endif
done:
	return ret;
}


int dhd_bus_start(dhd_pub_t *dhdp)
{
	int ret = -1;
    int timeleft = 200;
	wlc_rev_info_t revinfo = {0};

	/* try to download image and nvram to the dongle */
	if  (dhdp->busstate == DHD_BUS_DOWN)
	{
		/* wake lock moved to dhdsdio_download_firmware */
		if (!(dhdsdio_download_firmware(dhdp->bus))) 
		{
		    //printf("Download err!");
			return -1;
		}
	}
	if (dhdp->busstate != DHD_BUS_LOAD) 
	{
	    //printf("busstate err!");
		return -ENETDOWN;
	}
    DelayMs(500);
    
	/* Bring up the bus */
	ret = dhd_bus_init(dhdp);
	if (ret)
	{
		return ret;
	}

	/*wait for 3 times interrupts.*/
    while(timeleft)
    {   
        if (SDC_GetSdioEvent(0) == SDC_SUCCESS)
        {
            process_sdio_pending_irqs();
        }
        
        DelayMs(5);
        if(--timeleft == 0)
        {  
           break;
        }
    }

	/* If bus is not ready, can't come up */
	if (dhdp->busstate != DHD_BUS_DATA) 
	{
		return -ENODEV;
	}

	/* Bus is ready, do any protocol initialization */
	ret = dhd_wl_ioctl_cmd(dhdp, WLC_GET_REVINFO, &revinfo, sizeof(revinfo), 0);
	if (ret >= 0)
	{
		ret = dhd_preinit_ioctls(dhdp);
		if (ret < 0)
		{
			return ret;
		}
	}

	return 0;
}


