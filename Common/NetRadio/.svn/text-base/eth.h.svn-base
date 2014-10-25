#ifndef _ETH_H_
#define _ETH_H_


//----- DEFINES -----
#define DEFAULT_MAC                    "00:01:23:45:67:89"
#define DEFAULT_DHCP                   (1)
#define DEFAULT_IP                     "192.168.000.050"
#define DEFAULT_NETMASK                "255.255.255.000"
#define DEFAULT_ROUTER                 "192.168.000.001"
#define DEFAULT_DNS                    "192.168.000.001"
#define DEFAULT_NTP                    "078.046.194.189" //078.046.194.189 = 0.de.pool.ntp.org
#define DEFAULT_TIMEDIFF               (3600) //seconds (3600sec = 1h = GMT+1)
#define DEFAULT_SUMMER                 (0)    //summer time on

#define ETH_TIMEOUT                    (15) //seconds (ARP / DHCP / DNS request...)
#define ETH_USE_DSCP                   //use Differentiated Services Code Point (QoS -> DSCP)

#define TCP_TIMEOUT                    (3) //seconds
#define TCP_MAXERROR                   (3) //try x times
#define TCP_CLOSED                     (0)
#define TCP_OPENED                     (1)
#define TCP_OPEN                       (2)
#define TCP_ABORT                      (3)
#define TCP_CLOSE                      (4)
#define TCP_FINSH                      (5)

#define UDP_ENTRIES                    (5) //max UDP Table Entries
#define UDP_TIMEOUT                    (4) //seconds
#define UDP_CLOSED                     (0)
#define UDP_OPENED                     (1)

//dgl add start
#define ARP_TMR_5000MS   			0x01
#define DNS_TMR_1000MS   			0x02
#define TCP_FTMR_250MS   			0x04
#define TCP_STMR_500MS   			0x08
#define DHCP_FNT_500MS   			0x10
#define DHCP_CST_60S   				0x20
//dgl add end

typedef void *                      TCPPCB;

//----- GLOBALS -----
extern void eth_tickinit(void);
extern void eth_tickuninit(void);
extern void eth_service(int MovMemPool);
extern void eth_tmr_mask(UINT16 mbit, UINT16 bmask);
//----- PROTOTYPES -----

#endif //_ETH_H_
