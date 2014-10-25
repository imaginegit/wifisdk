/* Copyright (C) 2012 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: sdio_cis.c
Desc: 

Author: 
Date: 12-07-07
Notes:

$Log: $
 *
 *
*/
/*-------------------------------- Includes ----------------------------------*/

#include "sdio_card.h"

/*------------------------------------ Defines -------------------------------*/

#define CISTPL_NULL            0x00 //Null
#define CISTPL_CHECKSUM        0x10 //Checksum control
#define CISTPL_VERS_1          0x15 //Level 1 version/product-information
#define CISTPL_ALTSTR          0x16 //The Alternate Language String Tuple
#define CISTPL_MANFID          0x20 //Manufacturer Identification String Tuple
#define CISTPL_FUNCID          0x21 //Function Identification Tuple
#define CISTPL_FUNCE           0x22 //Function Extensions
//80h-8Fh Vendor Unique Tuples
#define CISTPL_SDIO_STD        0x91 //Additional information for functions built to support application specifications for standard SDIO functions.
#define CISTPL_SDIO_EXT        0x92 //Reserved for future use with SDIO devices.
#define CISTPL_END             0xFF //The End-of-chain Tuple

/*----------------------------------- Typedefs -------------------------------*/

/*-------------------------- Forward Declarations ----------------------------*/



/* ------------------------------- Globals ---------------------------------- */



/*-------------------------------- Local Statics: ----------------------------*/

/*--------------------------- Local Function Prototypes ----------------------*/


/*------------------------ Function Implement --------------------------------*/

static int cistpl_vers_1(void *pCard, struct sdio_func *func, const uint8 *buf, uint size)
{
	SDM_CARD_INFO*   card;
	unsigned i, nr_strings;
	char **buffer, *string;
	int malloc_siz = 0;

	card = (SDM_CARD_INFO*)pCard;
	
	/* Find all null-terminated (including zero length) strings in
	   the TPLLV1_INFO field. Trailing garbage is ignored. */
	buf += 2;
	size -= 2;

	nr_strings = 0;
	for (i = 0; i < size; i++) 
	{
		if (buf[i] == 0xff)
			break;
		if (buf[i] == 0)
			nr_strings++;
	}
	if (nr_strings == 0)
		return 0;

	size = i;
	malloc_siz = sizeof(char*) * nr_strings + size;
	buffer = sys_malloc(malloc_siz);
	if (!buffer)
		return -ENOMEM;
	memset(buffer, 0, malloc_siz);
	
	string = (char*)(buffer + nr_strings);

	for (i = 0; i < nr_strings; i++) 
	{
		buffer[i] = string;
		strcpy(string, (char *)buf);
		string += strlen(string) + 1;
		buf += strlen((const char *)buf) + 1;
	}

	if (func) 
	{
		func->num_info = nr_strings;
		func->info = (const char**)buffer;
	} 
	else 
	{
		card->num_info = nr_strings;
		card->info = (const char**)buffer;
	}

	return 0;
}


static int cistpl_manfid(void *pCard, struct sdio_func *func, const uint8 *buf, uint size)
{
	SDM_CARD_INFO*   card;
	unsigned int vendor, device;

	card = (SDM_CARD_INFO*)pCard;

	/* TPLMID_MANF */
	vendor = buf[0] | (buf[1] << 8);//vendor id

	/* TPLMID_CARD */
	device = buf[2] | (buf[3] << 8);//device id : 0x4330

	printf("manfid:vendor:0x%x device:0x%x\n", vendor, device);
	
	if (func) 
	{
		func->vendor = vendor;
		func->device = device;
	} 
	else 
	{
		card->cis.vendor = vendor;
		card->cis.device = device;
	}

	return 0;
}

static const unsigned char speed_val[16] =
	{ 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
static const unsigned int speed_unit[8] =
	{ 10000, 100000, 1000000, 10000000, 0, 0, 0, 0 };


typedef int (tpl_parse_t)(void *, struct sdio_func *, const unsigned char *, unsigned);

struct cis_tpl {
	unsigned char code;
	unsigned char min_size;
	tpl_parse_t *parse;
};

static int cis_tpl_parse(void *card, struct sdio_func *func,
			 const char *tpl_descr,
			 const struct cis_tpl *tpl, int tpl_count,//cis_tpl_list
			 unsigned char code,
			 const unsigned char *buf, unsigned size)
{
	int i, ret;

	/* look for a matching code in the table */
	for (i = 0; i < tpl_count; i++, tpl++) {
		if (tpl->code == code)
			break;
	}
	if (i < tpl_count) 
	{
		if (size >= tpl->min_size) 
		{
			if (tpl->parse)
				ret = tpl->parse(card, func, buf, size);//dgl cis_tpl_list
			else
				ret = -EILSEQ;	/* known tuple, not parsed */
		} 
		else 
		{
			/* invalid tuple */
			ret = -EINVAL;
		}
	} 
	else 
	{
		/* unknown tuple */
		ret = -ENOENT;
	}

	return ret;
}

static int cistpl_funce_common(void *pCard, struct sdio_func *func,
			       const unsigned char *buf, unsigned size)
{
	SDM_CARD_INFO*   card;
	
	/* Only valid for the common CIS (function 0) */
	if (func)
		return -EINVAL;

	card = (SDM_CARD_INFO*)pCard;

	/* TPLFE_FN0_BLK_SIZE */
	card->cis.blksize = buf[1] | (buf[2] << 8);

	/* TPLFE_MAX_TRAN_SPEED */
 	card->cis.max_dtr = speed_val[(buf[3] >> 3) & 15] * speed_unit[buf[3] & 7];

	return 0;
}


static int cistpl_funce_func(void *pCard, struct sdio_func *func,
			     const unsigned char *buf, unsigned size)
{
	uint32 vsn;
	uint32 min_size;

	/* Only valid for the individual function's CIS (1-7) */
	if (!func)
		return -EINVAL;

	/*
	 * This tuple has a different length depending on the SDIO spec
	 * version.
	 */
	vsn = ((SDM_CARD_INFO*)func->card)->cccr.sdio_vsn;
	min_size = (vsn == SDIO_SDIO_REV_1_00) ? 28 : 42;

	if (size < min_size)
		return -EINVAL;

	/* TPLFE_MAX_BLK_SIZE */
	func->max_blksize = buf[12] | (buf[13] << 8);

	/* TPLFE_ENABLE_TIMEOUT_VAL, present in ver 1.1 and above */
	if (vsn > SDIO_SDIO_REV_1_00)
		func->enable_timeout = (buf[28] | (buf[29] << 8)) * 10;
	else
		func->enable_timeout = 1000;//dgl 

	return 0;
}

/*
 * Known TPLFE_TYPEs table for CISTPL_FUNCE tuples.
 *
 * Note that, unlike PCMCIA, CISTPL_FUNCE tuples are not parsed depending
 * on the TPLFID_FUNCTION value of the previous CISTPL_FUNCID as on SDIO
 * TPLFID_FUNCTION is always hardcoded to 0x0C.
 */
static const struct cis_tpl cis_tpl_funce_list[] = {
	{	0x00,	4,	cistpl_funce_common		},
	{	0x01,	0,	cistpl_funce_func		},
	{	0x04,	1+1+6,	/* CISTPL_FUNCE_LAN_NODE_ID */	},
};

static int cistpl_funce(void *card, struct sdio_func *func, const uint8 *buf, uint size)
{
	if (size < 1)
		return -EINVAL;

	return cis_tpl_parse(card, func, "CISTPL_FUNCE",
					     cis_tpl_funce_list, ARRAY_SIZE(cis_tpl_funce_list),
					     buf[0], buf, size);
}


/* Known TPL_CODEs table for CIS tuples */
static const struct cis_tpl cis_tpl_list[] = 
{
	{	CISTPL_VERS_1,	3,	cistpl_vers_1		},//0x15
	{	CISTPL_MANFID,	4,	cistpl_manfid		},//0x20
	{	CISTPL_FUNCID,	2,	/* cistpl_funcid */	},//0x21
	{	CISTPL_FUNCE ,	0,	cistpl_funce		},//0x22
};

static int sdio_read_cis(void *pCard, struct sdio_func *func)
{
	SDM_CARD_INFO*   card;
	int ret;
	struct sdio_func_tuple *this, **prev;
	uint i, ptr = 0;
	uint malloc_siz = 0;
		
	card = (SDM_CARD_INFO*)pCard;

	/*
	 * Note that this works for the common CIS (function number 0) as
	 * well as a function's CIS * since SDIO_CCCR_CIS and SDIO_FBR_CIS
	 * have the same offset.
	 */
	for (i = 0; i < 3; i++) 
	{
		uint8 x, fn;

		if (func)
			fn = func->num;
		else
			fn = 0;

		ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, SDIO_FBR_BASE(fn) + SDIO_FBR_CIS + i, 0, &x);
		if (ret)
			return ret;
		ptr |= x << (i * 8); //make up the 24bits address.
	}
	
	if (func)
		prev = &func->tuples;
	else
		prev = &card->tuples;

	if (*prev != NULL)
		while(1);
		
	do 
	{
		unsigned char tpl_code, tpl_link;

		ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, ptr++, 0, &tpl_code);
		if (ret)
			break;

		/* 0xff means we're done */
		if (tpl_code == 0xff)
			break;

		/* null entries have no link field or data */
		if (tpl_code == 0x00)
			continue;

		ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, ptr++, 0, &tpl_link);
		if (ret)
			break;

		/* a size of 0xff also means we're done */
		if (tpl_link == 0xff)
			break;
			
		malloc_siz = sizeof(*this) + tpl_link;//tpl_link : This is the number of bytes in the tuple body. (n)
		this = sys_malloc(malloc_siz);
		if (!this)
			return -ENOMEM;
		memset((void *)this, 0, malloc_siz);
			
		for (i = 0; i < tpl_link; i++) 
		{
			ret = mmc_io_rw_direct(card, SDIO_R, SDIO_FUN_0, ptr + i, 0, &this->data[i]);//ptr:cis address pointer.
			if (ret)
				break;
		}
		if (ret) 
		{
			sys_free(this);
			break;
		}
		/* Try to parse the CIS tuple */
		ret = cis_tpl_parse(card, func, "CIS", 
				    		cis_tpl_list, ARRAY_SIZE(cis_tpl_list),
				    		tpl_code, this->data, tpl_link);
		if (ret == -EILSEQ || ret == -ENOENT) 
		{
			/*
			 * The tuple is unknown or known but not parsed.
			 * Queue the tuple for the function driver.
			 */
			this->next = NULL;
			this->code = tpl_code;
			this->size = tpl_link;
			*prev = this;
			prev = &this->next;

			if (ret == -ENOENT) 
			{
				/* warn about unknown tuples */
				// This is the normal exit procedure,rather than an error
				;
			}
			/* keep on analyzing tuples */
			ret = 0;
		}	
		else
		{
			/*
			 * We don't need the tuple anymore if it was
			 * successfully parsed by the SDIO core or if it is
			 * not going to be queued for a driver.
			 */
			sys_free(this);
		}
		ptr += tpl_link; //ptr: cis address pointer.
	}while (!ret);
	/*
	 * Link in all unknown tuples found in the common CIS so that
	 * drivers don't have to go digging in two places.
	 */
	if (func)
		*prev = card->tuples;

	return ret;
}
int sdio_read_common_cis(void *pCard)
{
	return sdio_read_cis(pCard, NULL);
}
void sdio_free_common_cis(void *pCard)
{
	SDM_CARD_INFO*   card;
	struct sdio_func_tuple *tuple, *victim;

	card = (SDM_CARD_INFO*)pCard;
	tuple = card->tuples;

	while (tuple) 
	{
		victim = tuple;
		tuple = tuple->next;
		sys_free(victim);
	}

	card->tuples = NULL;
}

int sdio_read_func_cis(struct sdio_func *func)
{
	int ret;

	ret = sdio_read_cis(func->card, func);
	if (ret)
		return ret;

	return 0;
}

void sdio_free_func_cis(struct sdio_func *func)
{
	struct sdio_func_tuple *tuple, *victim;

	tuple = func->tuples;

	while (tuple && tuple != ((SDM_CARD_INFO*)func->card)->tuples) 
	{
		victim = tuple;
		tuple = tuple->next;
		sys_free(victim);
	}

	func->tuples = NULL;
}

