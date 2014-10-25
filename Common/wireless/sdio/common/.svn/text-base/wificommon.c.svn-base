/*------------------------------------ Defines -------------------------------*/
#define _IN_WIFICOMMON_

#include  "sdio_card.h"

#define INSTANCESIZ 35
#define	ETHER_MAC_LEN		6

__align(32) static char cur_cmd_cmdbuf[CMD_BUF_MAX];//160 == 32 * 5
struct sdio_func sdio_func_0;
static struct sdio_func sdio_func_grp[SDIOD_MAX_IOFUNCS];
static unsigned int skbufcount = 0;
static sk_buff _skbuftg;
static char Instance[INSTANCESIZ];//35==sizeof(BCMSDH_SDMMC_INSTANCE)
static char g_mac[ETHER_MAC_LEN] = {0};

#if 0
static bool arp_rev_en_flag = TRUE;/*clear arp packet!!*/
void arp_packet_rev_en(void)
{
	arp_rev_en_flag = TRUE;
}

void arp_packet_rev_clr(void)
{
	arp_rev_en_flag = FALSE;
}
bool arp_packet_rev_flag(void)
{
	return arp_rev_en_flag;
}
#endif


/* -- mac function -- */
char * wifi_get_mac_addr(unsigned char *buf)
{   
    if (buf)
    {
	    memcpy(buf, g_mac, ETHER_MAC_LEN);
	}

    return g_mac;
    
}

int wifi_set_mac_addr(unsigned char *buf)
{
    memcpy(g_mac, buf, ETHER_MAC_LEN);

    return 0;
}


/*------------------------ Function Implement start--------------------------------*/
void * get_sdio_func(int fn)
{
    return &sdio_func_grp[fn];
}

void * get_instance(void)
{
    memset(Instance, 0, INSTANCESIZ);
	return &(Instance[0]);
}

void * get_global_cmdbuf(uint32 pos)
{
	void * ptr = NULL;

	if (pos >= CMD_BUF_MAX)
	{
		while(1);
		return NULL;
	}

	ptr = &cur_cmd_cmdbuf[pos];
	
	return ptr;
}

void * PKTGET(uint len)
{
	sk_buff * skbp;

	skbufcount++;
	
	skbp = (sk_buff *)(&_skbuftg);
	memset(skbp, 0, sizeof(sk_buff));
	skbp->len = len;
	
	skbp->data = get_global_cmdbuf(DATA_RXBUF_HEAD);
	memset(skbp->data, 0, len);

	return skbp;
}

void * PKTDATA(void * skbf)
{
	sk_buff * skbp = (sk_buff *)skbf;
	if (!skbf)
		return NULL;
	
	return skbp->data;
}
uint PKTLEN(void * skbf)
{
	sk_buff * skbp = (sk_buff *)skbf;
	if (!skbf)
		return 0;
	
	return skbp->len;
}

uint PKTSETLEN(void * skbf, unsigned int len)
{
    sk_buff * skbp = (sk_buff *)skbf;
    if (!skbf)
		return 0;
	skbp->len = len;

    return len;
}

void * PKTPUSH(void * skbf, unsigned int len)
{
    sk_buff * skbp = (sk_buff *)skbf;
    if (!skbf)
		return 0;
	skbp->data = (uint8 *)(skbp->data) - len;
	skbp->len  += len;

	return skbp->data;
}

void * PKTPULL(void * skbf, unsigned int len)
{
    sk_buff * skbp = (sk_buff *)skbf;
    if (!skbf)
		return 0;
	skbp->data = (uint8 *)(skbp->data) + len;
	skbp->len -= len;

	return skbp->data;
}

void PKTFREE(void * skbf)
{
	skbufcount = 0;
}

//string printf function..
#if 0
void cmd_cont_printf(UINT16 *cmd, int cmd_siz, int radix)
{
	int i;
	
	for (i=0; i<cmd_siz; i++) 
	{
		if ((i>0)&&((i % 10) == 0))
			printf("\n");
		
		if (radix == 10)
			printf(" %d", cmd[i]);
		else
		 	printf(" %04X", cmd[i]);
	}
	printf("\r\n");
}
#endif

//print the stack function..
#if 0
void stackprintf(void)
{
    uint32 *pStack;
    uint32 i, j;

    (uint32)pStack = __MRS_MSP();
    while((uint32)pStack < 0x03018000)
    {
        printf("        Stack = 0x%08x: ", (uint32)pStack);
        for (i = 0 ; i < 8; i++)
        {
            printf("0x%08x ", *(uint32*)pStack++);
            if ((uint32)pStack >= 0x03018000)
            {
                break;
            }
        }
        printf("\n");
    }
}
#endif

//compute the run time function.
#if 0
uint32 time,Timecnt,timestart,timeend;
void time_cnt_start(void)
{
    timestart = nvic->SysTick.Value;
    Timecnt = SysTickCounter;    
}

void time_cnt_end(void)
{
    timeend =  nvic->SysTick.Value;
    if(Timecnt == SysTickCounter)
    {
        time = timestart - timeend;
    }
    else
    {
        time = (SysTickCounter - Timecnt-1) * (nvic->SysTick.Reload + 1) + timestart + (nvic->SysTick.Reload - timeend + 1);
    }    
    printf("Reload:0x%x %x\n", (nvic->SysTick.Reload + 1), time);
}
#endif
/*------------------------ Function Implement end--------------------------------*/

