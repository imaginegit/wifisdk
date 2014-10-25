#include "SysInclude.h"

#include "sdio_card.h"

#include "lwipopt.h"
#include "def.h"
#include "err.h"
#include "pbuf.h"
#include "ip_addr.h"
#include "tcp.h"
#include "etharp.h"
#include "Autoip.h"
#include "netif.h"
#include "eth.h"

static int tcpTickNum = 0;
static SYSTICK_LIST tcpTick = {0};
static volatile uint tcpTimer250msTick = 0;
static UINT16 EthTmrFuse;
static UINT16 EthTmrFuseMask;
void eth_time_servic_fuse(int curtcptick);
	
static void TcpTickHandler(void)
{
    tcpTimer250msTick++;
	eth_time_servic_fuse(tcpTimer250msTick);
	return;
}


void eth_tickinit(void)
{
	if (tcpTickNum > 0)
		return;
	tcpTickNum++;

	EthTmrFuse = 0;
	EthTmrFuseMask = 0xFFFF;
	
    tcpTick.Counter = 0;//current count value.
    tcpTick.Reload = 25;//reload value.unit is 10ms
    tcpTick.Times  = 0; //if bigger than zero.it decrease every tick,it will be cleared by system. 
    tcpTick.pHandler = TcpTickHandler;
    SystickTimerStart(&tcpTick);
}


void eth_tickuninit(void)
{
	if (tcpTick.pHandler)
	{
	    SystickTimerStop(&tcpTick);
		tcpTick.pHandler = NULL;
		tcpTickNum = 0;
		EthTmrFuse = 0;
		EthTmrFuseMask = 0;
	}
}

void  eth_tmr_mask(UINT16 mbit, UINT16 bmask)
{
	if (bmask)
	{
		EthTmrFuseMask &= ~mbit;
	}
	else
	{
		EthTmrFuseMask |= mbit;
	}
}

static void eth_time_servic_fuse(int curtcptick)
{
	if (curtcptick % 20 == 0)
	{
		EthTmrFuse |= ARP_TMR_5000MS;
	}
	
    if (curtcptick % 4 == 0)
    {
		if (!dns_jump_this_tmr())
		{
			EthTmrFuse |= DNS_TMR_1000MS;
		}
    }
	
    /* Call tcp_fasttmr() every 250 ms */
	EthTmrFuse |= TCP_FTMR_250MS;
	
    if (curtcptick % 2 == 0)// 500ms tick..
    {
        /* for tcp tick. */
		EthTmrFuse |= TCP_STMR_500MS;

		/* for DHCP tick. */
		EthTmrFuse |= DHCP_FNT_500MS;
    	if ((curtcptick % 240) == 0)// 60s tick..
    	{
			EthTmrFuse |= DHCP_CST_60S;
    	}
    } 
	
	/* close the mask bit for servise.*/
	EthTmrFuse &= EthTmrFuseMask;

	return;
}

static void eth_time_servic(void)
{
	if (!EthTmrFuse)
		return;
    
	if (netif_is_up())
	{
		if (EthTmrFuse & ARP_TMR_5000MS)
		{
			EthTmrFuse &= ~ARP_TMR_5000MS;
			etharp_tmr();//5 s one loop...		
		}
		
        if (EthTmrFuse & DNS_TMR_1000MS)
        {
			EthTmrFuse &= ~DNS_TMR_1000MS;
          	dns_tmr();// 1 s one loop...	  
        }
		
	    /* Call tcp_fasttmr() every 250 ms */
        if (EthTmrFuse & TCP_FTMR_250MS)
        {
			EthTmrFuse &= ~TCP_FTMR_250MS;
          	tcp_fasttmr();// 1 s one loop...	  
        }
		
        if (EthTmrFuse & TCP_STMR_500MS)
        {
			EthTmrFuse &= ~TCP_STMR_500MS;
	        /* Call tcp_tmr() every 500 ms, i.e., every other timer tcp_tmr() is called. */
	    	tcp_slowtmr();
		}
	}
    else
    {
        if (EthTmrFuse & DHCP_FNT_500MS)
        {
			EthTmrFuse &= ~DHCP_FNT_500MS;
            dhcp_fine_tmr();
			
	        if (EthTmrFuse & DHCP_CST_60S)
	        {
				EthTmrFuse &= ~DHCP_CST_60S;
                //dhcp_coarse_tmr();
        	}
        } 
    }

	return;
}

void eth_service(int MovMemPool)
{
	/*time tick handle functions....*/
	eth_time_servic();
	
    /* Ethernet data packet received procession....*/
    if (SDC_GetSdioEvent(0) == SDC_SUCCESS)//check if has data came,if not,return just now.
    {
		process_sdio_pending_irqs();
    }
    rk_80211_data_handle();
	
    ethernetif_input();

	if (MovMemPool)
	{
		Http_Recive_Data(tcp_curget());
	}
}

