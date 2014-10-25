/*
 * DHD Protocol Module for CDC and BDC.
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
 * $Id: dhd_cdc.c 303389 2011-12-16 09:30:48Z $
 *
 * BDC is like CDC, except it includes a header for data packets to convey
 * packet priority over the bus, and flags (e.g. to indicate checksum status
 * for dongle offload.)
 */

#include "rk903.h"

static int dhd_bus_txctl(struct dhd_bus *bus, unsigned char *msg, uint msglen)
{
	uint8 *frame;
	uint16 flen;
	uint32 swheader;
	uint retries = 0;
	bcmsdh_info_t *sdh = bus->sdh;
	uint8 doff = 0;
	int ret = -1;
	int i;

	if (bus->dhd->dongle_reset)
		return -EIO;

	/* Back the pointer to make a room for bus header */
	frame = msg - SDPCM_HDRLEN;
	msglen += SDPCM_HDRLEN;
	
	/* Add alignment padding (optional for ctl frames) */
	if (dhd_alignctl) 
	{
		doff = (uint)frame % DHD_SDALIGN;
		if (doff) 
		{
			frame -= doff;
			msglen += doff;
			bzero(frame, doff + SDPCM_HDRLEN);
		}
	}
	doff += SDPCM_HDRLEN;

	flen = msglen;
	/* Round send length to next SDIO block */
	if (bus->roundup && bus->blocksize && (flen > bus->blocksize)) //dgl blocksize is fun2 512
	{
		uint16 pad = bus->blocksize - (flen % bus->blocksize);
		if ((pad <= bus->roundup) && (pad < bus->blocksize))
			flen += pad;
	} 
	else if (flen % DHD_SDALIGN) 
	{
		flen += DHD_SDALIGN - (flen % DHD_SDALIGN);
	}

	/* Satisfy length-alignment requirements */
	if (forcealign && (flen & (ALIGNMENT - 1)))
	{
		flen = ROUNDUP(flen, ALIGNMENT);
	}

	/* Make sure backplane clock is on */
	dhdsdio_clkctl(bus, CLK_AVAIL, FALSE);

	/* Hardware tag: 2 byte len followed by 2 byte ~len check (all LE) */
	*(uint16*)frame = htol16((uint16)msglen);
	*(((uint16*)frame) + 1) = htol16(~msglen);

	/* Software tag: channel, sequence number, data offset */
	swheader = ((SDPCM_CONTROL_CHANNEL << SDPCM_CHANNEL_SHIFT) & SDPCM_CHANNEL_MASK)
	            | bus->tx_seq | ((doff << SDPCM_DOFFSET_SHIFT) & SDPCM_DOFFSET_MASK);
	htol32_ua_store(swheader, frame + SDPCM_FRAMETAG_LEN);
	htol32_ua_store(0, frame + SDPCM_FRAMETAG_LEN + sizeof(swheader));

	do 
	{
		ret = bcmsdh_send_buf(bus->sdh, bcmsdh_cur_sbwad(sdh), SDIO_FUN_2, F2SYNC, frame, flen);
		if (ret < 0) 
		{
			/* On failure, abort the command and terminate the frame */
			printf("dhd_bus_txctl err=%d.\n", ret);
			bus->tx_sderrs++;

			bcmsdh_abort(sdh, SDIO_FUN_2);

			bcmsdh_cfg_write(sdh, SDIO_FUN_1, SBSDIO_FUNC1_FRAMECTRL, SFC_WF_TERM, NULL);

			bus->f1regdata++;

			for (i = 0; i < 3; i++) 
			{
				uint8 hi, lo;
				hi = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_WFRAMEBCHI, NULL);
				lo = bcmsdh_cfg_read(sdh, SDIO_FUN_1, SBSDIO_FUNC1_WFRAMEBCLO, NULL);
				bus->f1regdata += 2;
				if ((hi == 0) && (lo == 0))
				{
					break;
				}
			}
		}
		if (ret == 0) 
		{
			bus->tx_seq = (bus->tx_seq + 1) % SDPCM_SEQUENCE_WRAP;
		}
	} while ((ret < 0) && retries++ < TXRETRIES);

done:
	#if 0 //dgl no low power,
	if ((bus->idletime == DHD_IDLE_IMMEDIATE) && !bus->dpc_sched) 
	{
		bus->activity = FALSE;
		dhdsdio_clkctl(bus, CLK_NONE, TRUE);
	}
	#endif

	return ret ? -EIO : 0;
}


static int dhd_bus_rxctl(struct dhd_bus *bus, uchar *msg, uint msglen)
{
	int timeleft, i, clen;
	uint rxlen = 0;

	if (bus->dhd->dongle_reset)
		return -EIO;

	/* Wait until control frame is available */
	timeleft = 400;
    bus->is_get_ctrl_frame = 0;
    while(!bus->is_get_ctrl_frame)//dhdsdio_isr
    { 
        if (SDC_GetSdioEvent(0) == SDC_SUCCESS)
        {
            process_sdio_pending_irqs();
        }
        
        DelayMs(5);
        if(--timeleft == 0)
        {  
           BCMDEBUG("wait ctrl timeout");
           return -ETIMEDOUT;
        }
    }	
    bus->is_get_ctrl_frame = 0;
	
	rxlen = bus->rxlen;
	bus->rxlen = 0;

	clen = MIN(msglen, rxlen);
	for (i = 0; i < clen; i++)
	{
		/*can not use bcopy, because the msg and rxctl both 
		pointer to one global varilbe in different position.
		bus->rxctl += doff;=>move to the header of "cdc_ioctrl_t+msg" postion.*/
		msg[i] = bus->rxctl[i];//msg == bus->dhd->prot->msg.
	}

	return rxlen ? (int)rxlen : -EIO;
}


static int dhdcdc_msg(dhd_pub_t *dhd)
{
	int err = 0;
	dhd_prot_t *prot = dhd->prot;
	int cdc_len = ltoh32(prot->msg->len) + sizeof(cdc_ioctl_t);

	if (cdc_len > CDC_MAX_MSG_SIZE)
		cdc_len = CDC_MAX_MSG_SIZE;

	/* Send request */
	err = dhd_bus_txctl(dhd->bus, (uint8*)prot->msg, cdc_len);

	return err;
}

static int dhdcdc_cmplt(dhd_pub_t *dhd, uint32 id, uint32 len)
{
	int ret;
	int cdc_len = len + sizeof(cdc_ioctl_t);
	dhd_prot_t *prot = dhd->prot;

	do 
	{
		ret = dhd_bus_rxctl(dhd->bus, (uint8*)prot->msg, cdc_len);
		if (ret < 0)
		{
            BCMDEBUG("dhd_bus_rxctl err");
            break;
        }

	} while (CDC_IOC_ID(ltoh32(prot->msg->flags)) != id);

	return ret;
}

static int dhdcdc_send_ioctl(dhd_pub_t *dhd, uint cmd, void *buf, uint len, uint8 action)
{
	sk_buff * skbp;
	dhd_prot_t *prot = dhd->prot;
	int ret;
	uint32 flags;
    cdc_ioctl_t *msg = PKTPUSH(buf, sizeof(cdc_ioctl_t));
	
    dhd->prot->msg = msg;
	
	msg->cmd = htol32(cmd);
	msg->len = htol32(len);
	msg->flags = (++prot->reqid << CDCF_IOC_ID_SHIFT);
	CDC_SET_IF_IDX(msg, 0);//dgl int ifidx ==0 always
	if (action & WL_IOCTL_ACTION_SET)
	{
		action &= WL_IOCTL_ACTION_MASK;/* add additional action bits */
		msg->flags |= (action << CDCF_IOC_ACTION_SHIFT) | CDCF_IOC_SET;
	}
	else 
	{
		action &= WL_IOCTL_ACTION_MASK;/* add additional action bits */
		msg->flags |= (action << CDCF_IOC_ACTION_SHIFT);
	}
	msg->flags = htol32(msg->flags);
	msg->status = 0;

	ret = dhdcdc_msg(dhd);
	if (ret < 0) 
	{        
        dhd->busstate = DHD_BUS_DOWN; 
		goto done;
	}

	/* wait for interrupt and get first fragment */
	ret = dhdcdc_cmplt(dhd, prot->reqid, len);
	if (ret < 0)
	{       
        goto done;
	}

	/* Check info buffer*/
	skbp = (sk_buff *)buf;
	if (ret < (int)len)
	{
		len = ret;
	}
	/*bus->rxctl += doff;=>move to the header of "cdc_ioctrl_t+msg" postion.*/
	skbp->data = (uint8 *)(msg) + sizeof(cdc_ioctl_t);
	skbp->len = len;

	/* Check the ERROR flag */
	flags = ltoh32(msg->flags);
	if (flags & CDCF_IOC_ERROR)
	{
		ret = ltoh32(msg->status);
		dhd->dongle_error = ret;//Cache error from dongle 
	}

done:
	return ret;
}

int dhd_wl_ioctl_cmd(dhd_pub_t *dhd_pub, int cmd, void *arg, int len, uint8 set)
{
    int ret;
    sk_buff pkt;
	dhd_prot_t * prot = dhd_pub->prot;

	if ((dhd_pub->busstate == DHD_BUS_DOWN) || (len > WLC_IOCTL_MAXLEN)) 
	{
		return -1;
	}
	
    pkt.data = get_global_cmdbuf(CMD_RXBUF_HEAD);
    memcpy(pkt.data, arg, len);
    pkt.len = len;

	ret = dhdcdc_send_ioctl(dhd_pub, cmd, &pkt, len, set);//send command
	if (ret > 0)
	{
		/* Too many programs assume ioctl() returns 0 on success */
		memcpy(arg, pkt.data, pkt.len);
		ret = 0;
	}
	
	return ret;
}


