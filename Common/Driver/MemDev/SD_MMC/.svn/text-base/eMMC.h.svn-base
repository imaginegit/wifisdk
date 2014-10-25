/* Copyright (C) 2011 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: EMMC.h
Desc: 

Author: 
Date: 12-01-10
Notes:

$Log: $
 *
 *
*/

#ifndef _EMMC_H
#define _EMMC_H

/*-------------------------------- Includes ----------------------------------*/


/*------------------------------ Global Defines ------------------------------*/


/*------------------------------ Global Typedefs -----------------------------*/


/*----------------------------- External Variables ---------------------------*/
;    


/*------------------------- Global Function Prototypes -----------------------*/
extern uint32 EmmcInit(void);
extern uint32 EmmcDeInit(void);

extern uint32 EmmcBlkErase(uint32 blk);

extern uint32 MDEmmcRead(uint8 lun, uint32 sec, uint32 nSec, void *pBuf);
extern uint32 MDEmmcWrite(uint8 lun, uint32 sec, uint32 nSec, void *pBuf);
extern uint32 MDEmmcGetInfo(uint8 lun, pMEMDEV_INFO pDevInfo);

#endif
