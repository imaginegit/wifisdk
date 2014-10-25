#ifndef _WIFI_CONTROL_H_
#define _WIFI_CONTROL_H_

#undef  EXT
#ifdef _IN_WIFI_CONTROL_
#define EXT
#else
#define EXT extern
#endif

void rk_80211_handle_init(void);
int  rk_80211_data_input(void *data, uint len);
void rk_80211_data_handle(void);
int  rk_get_cur_rssi(void);

void WiFiInit(void *pArg);
UINT32 WiFiService(void);
void WiFiDeInit(void);

#ifdef _IN_WIFI_CONTROL_
EXT THREAD WiFiThread = {

    NULL,
    NULL,
    
    WiFiInit,
    WiFiService,
    WiFiDeInit,
    
    NULL                           
};
#else
EXT THREAD WiFiThread;
#endif
#endif