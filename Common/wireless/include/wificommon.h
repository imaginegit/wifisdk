#ifndef WIFICOMMON_H
#define WIFICOMMON_H

#undef EXT
#ifdef _IN_WIFICOMMON_
#define EXT
#else
#define EXT extern
#endif

typedef struct _sk_buff
{
	uint   len;
	void * data;
}sk_buff;

#define CMD_BUF_MAX    		2048	//2048
#define CMD_RXBUF_HEAD 		160 	//== 32 * 5
#define DATA_RXBUF_HEAD 	160		//== 32 * 5
#define ALIGN(x, a)	(((x) + (a) - 1) & ~((a) - 1))

EXT void * PKTGET(uint len);
EXT void * PKTDATA(void * skbf);
EXT uint   PKTLEN(void * skbf);
EXT void   PKTFREE(void * skbf);
EXT void * PKTPUSH(void * skbf, unsigned int len);
EXT void * PKTPULL(void * skbf, unsigned int len);
EXT uint   PKTSETLEN(void * skbf, unsigned int len);

EXT void * get_sdio_func(int fn);
EXT void * get_instance(void);
EXT void * get_global_cmdbuf(uint32 pos);

EXT void ModuleOverlayLoadData(uint32 SrcAddr, uint32 DestAddr, uint32 Length);

#endif
