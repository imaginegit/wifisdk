/** @file
 */

#ifndef __LWIP_DHCP_H__
#define __LWIP_DHCP_H__

/** period (in seconds) of the application calling dhcp_coarse_tmr() */
#define DHCP_COARSE_TIMER_SECS 60 
/** period (in milliseconds) of the application calling dhcp_coarse_tmr() */
#define DHCP_COARSE_TIMER_MSECS (DHCP_COARSE_TIMER_SECS * 1000UL)
/** period (in milliseconds) of the application calling dhcp_fine_tmr() */
#define DHCP_FINE_TIMER_MSECS 500 

#define DHCP_MIN_OPTIONS_LEN 68U
/** make sure user does not configure this too small */
#if ((defined(DHCP_OPTIONS_LEN)) && (DHCP_OPTIONS_LEN < DHCP_MIN_OPTIONS_LEN))
#  undef DHCP_OPTIONS_LEN
#endif
/** allow this to be configured in lwipopts.h, but not too small */
#if (!defined(DHCP_OPTIONS_LEN))
/** set this to be sufficient for your options in outgoing DHCP msgs */
#  define DHCP_OPTIONS_LEN DHCP_MIN_OPTIONS_LEN

/** start DHCP configuration */
uint8 dhcp_start(NETIF *netif);
void udp_user_use(void);
UDP_PCB * get_udp_pcb();
IP_ADDR get_ip_addr();
void send_user_data(UDP_PCB *pcb, IP_ADDR *addr, uint16 port, char *dat, int len);
/** enforce early lease renewal (not needed normally)*/
uint8 dhcp_renew(NETIF *netif);
/** release the DHCP lease, usually called before dhcp_stop()*/
uint8 dhcp_release(NETIF *netif);
/** stop DHCP configuration */
void dhcp_stop(NETIF *netif);
/** inform server of our manual IP address */
void dhcp_inform(NETIF *netif);

/** if enabled, check whether the offered IP address is not in use, using ARP */
#if DHCP_DOES_ARP_CHECK
void dhcp_arp_reply(NETIF *netif, IP_ADDR *addr);
#endif

/** to be called every minute */
void dhcp_coarse_tmr(void);
/** to be called every half second */
void dhcp_fine_tmr(void);
 
/** DHCP message item offsets and length */
#define DHCP_MSG_OFS (UDP_DATA_OFS)  
  #define DHCP_OP_OFS (DHCP_MSG_OFS + 0)
  #define DHCP_HTYPE_OFS (DHCP_MSG_OFS + 1)
  #define DHCP_HLEN_OFS (DHCP_MSG_OFS + 2)
  #define DHCP_HOPS_OFS (DHCP_MSG_OFS + 3)
  #define DHCP_XID_OFS (DHCP_MSG_OFS + 4)
  #define DHCP_SECS_OFS (DHCP_MSG_OFS + 8)
  #define DHCP_FLAGS_OFS (DHCP_MSG_OFS + 10)
  #define DHCP_CIADDR_OFS (DHCP_MSG_OFS + 12)
  #define DHCP_YIADDR_OFS (DHCP_MSG_OFS + 16)
  #define DHCP_SIADDR_OFS (DHCP_MSG_OFS + 20)
  #define DHCP_GIADDR_OFS (DHCP_MSG_OFS + 24)
  #define DHCP_CHADDR_OFS (DHCP_MSG_OFS + 28)
  #define DHCP_SNAME_OFS (DHCP_MSG_OFS + 44)
  #define DHCP_FILE_OFS (DHCP_MSG_OFS + 108)
#define DHCP_MSG_LEN 236

#define DHCP_COOKIE_OFS (DHCP_MSG_OFS + DHCP_MSG_LEN)
#define DHCP_OPTIONS_OFS (DHCP_MSG_OFS + DHCP_MSG_LEN + 4)

#define DHCP_CLIENT_PORT 68  
#define DHCP_SERVER_PORT 67

#define USER_CLIENT_PORT 778//59123//78
#define USER_SERVER_PORT 59123//778//77

/** DHCP client states */
#define DHCP_REQUESTING 1
#define DHCP_INIT 2
#define DHCP_REBOOTING 3
#define DHCP_REBINDING 4
#define DHCP_RENEWING 5
#define DHCP_SELECTING 6
#define DHCP_INFORMING 7
#define DHCP_CHECKING 8
#define DHCP_PERMANENT 9
#define DHCP_BOUND 10
/** not yet implemented #define DHCP_RELEASING 11 */
#define DHCP_BACKING_OFF 12
#define DHCP_OFF 13

/** AUTOIP cooperatation flags */
#define DHCP_AUTOIP_COOP_STATE_OFF 0
#define DHCP_AUTOIP_COOP_STATE_ON 1
 
#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTREPLY 2

#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_REQUEST 3
#define DHCP_DECLINE 4
#define DHCP_ACK 5
#define DHCP_NAK 6
#define DHCP_RELEASE 7
#define DHCP_INFORM 8

#define DHCP_HTYPE_ETH 1

#define DHCP_HLEN_ETH 6

#define DHCP_BROADCAST_FLAG 15
#define DHCP_BROADCAST_MASK (1 << DHCP_FLAG_BROADCAST)

/** BootP options */
#define DHCP_OPTION_PAD 0
#define DHCP_OPTION_SUBNET_MASK 1 /* RFC 2132 3.3 */
#define DHCP_OPTION_ROUTER 3
#define DHCP_OPTION_DNS_SERVER 6 
#define DHCP_OPTION_HOSTNAME 12
#define DHCP_OPTION_IP_TTL 23
#define DHCP_OPTION_MTU 26
#define DHCP_OPTION_BROADCAST 28
#define DHCP_OPTION_TCP_TTL 37
#define DHCP_OPTION_END 255

/** DHCP options */
#define DHCP_OPTION_REQUESTED_IP 50 /* RFC 2132 9.1, requested IP address */
#define DHCP_OPTION_LEASE_TIME 51 /* RFC 2132 9.2, time in seconds, in 4 bytes */
#define DHCP_OPTION_OVERLOAD 52 /* RFC2132 9.3, use file and/or sname field for options */

#define DHCP_OPTION_MESSAGE_TYPE 53 /* RFC 2132 9.6, important for DHCP */
#define DHCP_OPTION_MESSAGE_TYPE_LEN 1


#define DHCP_OPTION_SERVER_ID 54 /* RFC 2132 9.7, server IP address */
#define DHCP_OPTION_PARAMETER_REQUEST_LIST 55 /* RFC 2132 9.8, requested option types */

#define DHCP_OPTION_MAX_MSG_SIZE 57 /* RFC 2132 9.10, message size accepted >= 576 */
#define DHCP_OPTION_MAX_MSG_SIZE_LEN 2

#define DHCP_OPTION_T1 58 /* T1 renewal time */
#define DHCP_OPTION_T2 59 /* T2 rebinding time */
#define DHCP_OPTION_US 60
#define DHCP_OPTION_CLIENT_ID 61
#define DHCP_OPTION_TFTP_SERVERNAME 66
#define DHCP_OPTION_BOOTFILE 67

/** possible combinations of overloading the file and sname fields with options */
#define DHCP_OVERLOAD_NONE 0
#define DHCP_OVERLOAD_FILE 1
#define DHCP_OVERLOAD_SNAME  2
#define DHCP_OVERLOAD_SNAME_FILE 3

#endif /* LWIP_DHCP */

#endif /*__LWIP_DHCP_H__*/
