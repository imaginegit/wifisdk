/*
********************************************************************************
*                   Copyright (c) 2012,csun
*                         All rights reserved.
*
* File Name:	Wificontrol.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*                csun           2012-8-27          1.0
*    desc:    ORG.
********************************************************************************
*/
#define _IN_WIFI_CONTROL_

#include "sysinclude.h"
#include "wificontrol.h"
#include "def.h"
#include "netif.h"
#include "Eth.h"
#include "lwipopt.h"
#include "tcp.h"
#include "pBuf.h"
#include "rk903.h"
#include "ip_addr.h"
#include "udp.h"
#include "protocol.h"


#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i


static uint32 WifiLDOBak;
static uint32 timexxx = 0;
int    WiFiCurAPRssi;
BOOLEAN bWiFiAudioPause;

/*
********************************************************************************
*
*                        Macro Define 
*
********************************************************************************
*/
#define CONNECTAP_OK  		0
#define CONNECTAP_FAIL  	-1

#define AP_SCAN_TIMEOUT  	1000 // 1000 * 0.5MS == 10S
#define AP_SCAN_TAIL  		300  // 300 * 10MS == 2S
#define AP_SCAN_DISPLAY  	100  // 200 * 0.5MS == 1S
/*
********************************************************************************
*
*                         Variable Define
*
********************************************************************************
*/

int rk_get_cur_rssi(void)
{
	int i;
	char APRssi[5] = {30, 45, 57, 70, 80};

	for (i = 0; i < 5; i++)
	{
		if (WiFiCurAPRssi <= APRssi[i])
		{
			break;
		}
	}
	i = 5 - i;
	printf("rk_get_cur_rssi:i=%d rssi=%d\n", i, WiFiCurAPRssi);
	
	return i;
}


int wl_iw_get_wireless_stats(dhd_pub_t *pub, struct iw_statistics *wstats)
{
	int res = 0;
	wl_cnt_t cnt;
	int phy_noise;
	scb_val_t scb_val;

	if (pub == NULL)
	{
		pub = G_dhd;
	}

    #if 0
	phy_noise = 0;
	res = dhd_wl_ioctl_cmd(pub, WLC_GET_PHY_NOISE, &phy_noise, sizeof(phy_noise), 0);
	if (res)
		goto done;

	phy_noise = dtoh32(phy_noise);
	printf("wl_iw_get_wireless_stats phy noise=%d\n", phy_noise);
    #endif

	bzero(&scb_val, sizeof(scb_val_t));
    
	res = dhd_wl_ioctl_cmd(pub, WLC_GET_RSSI, &scb_val, sizeof(scb_val_t), 0);
	if (res)
		goto done;
	WiFiCurAPRssi = -dtoh32(scb_val.val);
	
    printf("WiFiCurAPRssi = %d\n", WiFiCurAPRssi);
	//printf("wl_iw_get_wireless_stats phy rssi=%d %02x:%02x:%02x:%02x:%02x:%02x\n", WiFiCurAPRssi, 
	//	  scb_val.ea.octet[0], scb_val.ea.octet[1], scb_val.ea.octet[2], scb_val.ea.octet[3],
	//	  scb_val.ea.octet[4], scb_val.ea.octet[5]);

done:
	return res;
}


/*
--------------------------------------------------------------------------------
  Function name : void WiFiInit(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
void WiFiInit(void *pArg)
{
    //系统初始化
    WifiLDOBak = ScuLDOSet(SCU_LDO_30);
    PMU_EnterModule(PMU_MAX);

    //SDIO初始化
    ModuleOverlay(MODULE_ID_WIFI_SCAN, MODULE_OVERLAY_ALL);
    
    __user_initial_stackheap();  
    if (SDIO_Module_Init())
	{
        SendMsg(MSG_WIFI_SCAN_ERR);
        SendMsg(MSG_WIFI_SDIO_ERR);
        return;
	}
    
    //WIFI驱动初始化    
 	ClearMsg(MSG_WIFI_SCAN);
	bss_descriptor_info_init();
	SetScanEventOpen(TRUE);//set dirver enable receive the scan result event.
    SendMsg(MSG_WIFI_SCAN_START);  
    pbuf_init();
}

/*
--------------------------------------------------------------------------------
  Function name : void WiFiDeInit(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
void WiFiDeInit(void)
{
    //WIFI驱动反初始化
    eth_tickuninit();       //里面有增减判断
    
    //SDIO反初始化
    if (FALSE == GetMsg(MSG_WIFI_SDIO_ERR))
    {
        ModuleOverlay(MODULE_ID_WIFI_SCAN, MODULE_OVERLAY_ALL);
        SDIO_Module_Deinit();
    }
    
    PMU_ExitModule(PMU_MAX);   
    ScuLDOSet(WifiLDOBak);
}

/*
--------------------------------------------------------------------------------
  Function name : void WiFiService(void)
  Author        : 
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>
               
  desc:         ORG
--------------------------------------------------------------------------------
*/
void rk_printf_save_ap(void)
{                                                          
    printf("ssid    = %s\n", gSysConfig.WiFiConfig.ssid);
    printf("password    = %s\n", gSysConfig.WiFiConfig.password);
    printf("passwordlen = %d, sizeof(password) = %d\n", gSysConfig.WiFiConfig.password_len, strlen(gSysConfig.WiFiConfig.password));
}

UINT32 WiFiService(void)
{
	static uint APScanStartTime;
    uint32 ret = -1;
    int temp;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //WIFI Scan
    if (GetMsg(MSG_WIFI_SCAN_START))
    {
		if (wl_do_escan())
		{
			SendMsg(MSG_WIFI_SCAN_ERR);
		}
		else
		{
			SendMsg(MSG_WIFI_SCAN);
			APScanStartTime = SysTickCounter;//SysTickCounter unit is 0.5ms
			printf("APScanStartTime:%d\n", APScanStartTime);
		}
    }
	
	if (CheckMsg(MSG_WIFI_SCAN))
	{
		if (GetScanEventOpen())
		{
	        if (SDC_GetSdioEvent(0) == SDC_SUCCESS)
	        { 
	            process_sdio_pending_irqs();
	        }

			/* check the scan process..*/
			temp = rk_scan_work();
			if ((temp > 0) || 
				/* check the scan timeout.*/
				(SysTickCounter > APScanStartTime + AP_SCAN_TIMEOUT))
			{
				printf("scan tout\n");
				ClearMsg(MSG_WIFI_SCAN);
				SendMsg(MSG_WIFI_SCAN_OK);
			}
		}
		else
		{
			ClearMsg(MSG_WIFI_SCAN);
		}
	}
    
    //WIFI Scan end
    ////////////////////////////////////////////////////////////////////////////////////////////////
        
    ////////////////////////////////////////////////////////////////////////////////////////////////
    if (GetMsg(MSG_WIFI_CONNECT))
    {
		int APScanclrTail;

		ClearMsg(MSG_WIFI_APPW_JUMP);
		ClearMsg(MSG_WIFI_SCAN);
		
        ModuleOverlay(MODULE_ID_WIFI_WPA,  MODULE_OVERLAY_DATA | MODULE_OVERLAY_BSS); //goto scan code
        rk_printf_save_ap();
        if (0 != rk_wifi_connect_pro(gSysConfig.WiFiConfig.ssid, 
                                     gSysConfig.WiFiConfig.password, 
                                     strlen(gSysConfig.WiFiConfig.password)))
    	{		
    		SendMsg(MSG_WIFI_CONN_FAILURE);
    	}
        else
        {
            NETIF  *netif;
        	uint32 timeout;
        	IP_ADDR3 * pIp;	
			int i;

			if (GetMsg(MSG_WIFI_NO_ENCODE))
			{
				MSG_SET_WPA_SH();
			}

	        ModuleOverlay(MODULE_ID_WIFI_LWIP, MODULE_OVERLAY_ALL);
            ModuleOverlay(MODULE_ID_WIFI_DHCP, MODULE_OVERLAY_ALL);
			for (i=0; i<3; i++)
			{
	            tcp_pcb_init();
	        	udp_pcb_init();
	        	arp_list_init();
	            netif = netif_add(NULL, NULL, NULL);
	            
	        	eth_tickinit();
	           	
	        	dhcp_start(netif);

			udp_user_use();
	            
	        	dns_init();

	        	timeout = SysTickCounter;
	        	while(1)
	        	{									  
	        		if (netif_is_up())
	        		{
	                    netif->dhcp = NULL;
	                    SendMsg(MSG_WIFI_CONN_SUCCESS);
	        			return TRUE;
	        		}
	                if(SysTickCounter > (timeout + 2000))
	                {
	        			break;
	                }        
	        		/* Ethernet data packet received procession....*/
					//printf("fle-->eth service\n");
	        		eth_service(FALSE);
	        	}  
			}
            SendMsg(MSG_WIFI_CONN_FAILURE);
        }
    }

    if (CheckMsg(MSG_WIFI_TCP_CONNECTING))
    {       
    
        UserIsrDisable();

    		eth_service(0);

        UserIsrEnable(1);
    }

	if(CheckMsg(MSG_QPW_ENABLE)) {
		QpwNumber++;
		if(QpwNumber > 0x780000) {  // 1 min
			QpwNumber = 0;
			upload_connect(C_QPW);
		}
	}
    
	return ret;
}

/*
********************************************************************************
*
*                         End of Wificontrol.c
*
********************************************************************************
*/

