/* Copyright (C) 2012 ROCK-CHIP FUZHOU. All Rights Reserved. */
/*
File: sdio.h
Desc: 

Author: 
Date: 12-07-03
Notes:

$Log: $
 *
 *
*/

#ifndef _SDIO_H
#define _SDIO_H


/*-------------------------------- Includes ----------------------------------*/


/*------------------------------ Global Defines ------------------------------*/
/* SDIO commands                         type  argument     response */
#define SD_IO_SEND_OP_COND          SD_CMD5 /* bcr  [23:0] OCR         R4  */
#define SD_IO_RW_DIRECT            SD_CMD52 /* ac   [31:0] See below   R5  */
#define SD_IO_RW_EXTENDED          SD_CMD53 /* adtc [31:0] See below   R5  */

/*
 * SD_IO_SEND_OP_COND argument format:
 *
 *      [24] S18R
 *      [23:0] I/O OCR
 */
#define SD_IO_OCR_20_21          (1 << 8) 
#define SD_IO_OCR_21_22          (1 << 9) 
#define SD_IO_OCR_22_23          (1 << 10) 
#define SD_IO_OCR_23_24          (1 << 11) 
#define SD_IO_OCR_24_25          (1 << 12) 
#define SD_IO_OCR_25_26          (1 << 13) 
#define SD_IO_OCR_26_27          (1 << 14) 
#define SD_IO_OCR_27_28          (1 << 15) 
#define SD_IO_OCR_28_29          (1 << 16) 
#define SD_IO_OCR_29_30          (1 << 17) 
#define SD_IO_OCR_30_31          (1 << 18) 
#define SD_IO_OCR_31_32          (1 << 19) 
#define SD_IO_OCR_32_33          (1 << 20) 
#define SD_IO_OCR_33_34          (1 << 21) 
#define SD_IO_OCR_34_35          (1 << 22) 
#define SD_IO_OCR_35_36          (1 << 23) 

#define SD_IO_OCR_S18R           (1 << 24) 

/*
 * SD_IO_RW_DIRECT argument format:
 *
 *      [31] R/W flag
 *      [30:28] Function number
 *      [27] RAW flag
 *      [25:9] Register address
 *      [7:0] Data
 */
#define SD_IO_W   0x80000000
#define SD_IO_R   0x00000000

#define SD_IO_RAW_YES 0x08000000
#define SD_IO_RAW_NO  0x00000000

#define SD_IO_INC_ADDR 0x04000000
#define SD_IO_FIX_ADDR 0x00000000

#define SD_IO_BLK_MOD 0x08000000


/*
 * SD_IO_RW_EXTENDED argument format:
 *
 *      [31] R/W flag
 *      [30:28] Function number
 *      [27] Block mode
 *      [26] Increment address
 *      [25:9] Register address
 *      [8:0] Byte/block count
 */

#define R4_MEMORY_PRESENT (1u << 27)
#define R4_MEMORY_READY   (1u << 31)
/*
  SDIO status in R5
  Type
	e : error bit
	s : status bit
	r : detected and set for the actual command response
	x : detected and set during command execution. the host must poll
            the card by sending status command in order to read these bits.
  Clear condition
	a : according to the card state
	b : always related to the previous command. Reception of
            a valid command will clear it (with a delay of one command)
	c : clear by read
 */

#define R5_COM_CRC_ERROR	(1 << 15)	/* er, b */
#define R5_ILLEGAL_COMMAND	(1 << 14)	/* er, b */
#define R5_ERROR		(1 << 11)	/* erx, c */
#define R5_FUNCTION_NUMBER	(1 << 9)	/* er, c */
#define R5_OUT_OF_RANGE		(1 << 8)	/* er, c */
#define R5_STATUS(x)		(x & 0xCB00)
#define R5_IO_CURRENT_STATE(x)	((x & 0x3000) >> 12) /* s, b */

/*
 * Card Common Control Registers (CCCR)
 */

#define SDIO_CCCR_CCCR		0x00

#define  SDIO_CCCR_REV_1_00	0	/* CCCR/FBR Version 1.00 */
#define  SDIO_CCCR_REV_1_10	1	/* CCCR/FBR Version 1.10 */
#define  SDIO_CCCR_REV_1_20	2	/* CCCR/FBR Version 1.20 */

#define  SDIO_SDIO_REV_1_00	0	/* SDIO Spec Version 1.00 */
#define  SDIO_SDIO_REV_1_10	1	/* SDIO Spec Version 1.10 */
#define  SDIO_SDIO_REV_1_20	2	/* SDIO Spec Version 1.20 */
#define  SDIO_SDIO_REV_2_00	3	/* SDIO Spec Version 2.00 */

#define SDIO_CCCR_SD		0x01

#define  SDIO_SD_REV_1_01	0	/* SD Physical Spec Version 1.01 */
#define  SDIO_SD_REV_1_10	1	/* SD Physical Spec Version 1.10 */
#define  SDIO_SD_REV_2_00	2	/* SD Physical Spec Version 2.00 */

#define SDIO_CCCR_IOEx		0x02
/* io_en */
#define SDIO_FUNC_ENABLE_1	0x02	/* function 1 I/O enable */
#define SDIO_FUNC_ENABLE_2	0x04	/* function 2 I/O enable */

#define SDIO_CCCR_IORx		0x03
/* io_rdys */
#define SDIO_FUNC_READY_1	0x02	/* function 1 I/O ready */
#define SDIO_FUNC_READY_2	0x04	/* function 2 I/O ready */

#define SDIO_CCCR_IENx		0x04	/* Function/Master Interrupt Enable */
#define SDIO_CCCR_INTx		0x05	/* Function Interrupt Pending */

#define SDIO_CCCR_ABORT		0x06	/* function abort/card reset */

#define SDIO_CCCR_IF		0x07	/* bus interface controls */

#define  SDIO_BUS_WIDTH_1BIT	0x00
#define  SDIO_BUS_WIDTH_4BIT	0x02
#define  SDIO_BUS_ECSI			0x20	/* Enable continuous SPI interrupt */
#define  SDIO_BUS_SCSI			0x40	/* Support continuous SPI interrupt */
#define  SDIO_BUS_ASYNC_INT		0x20
#define  SDIO_BUS_CD_DISABLE    0x80	/* disable pull-up on DAT3 (pin 1) */

#define SDIO_CCCR_CAPS		0x08

#define  SDIO_CCCR_CAP_SDC	0x01	/* can do CMD52 while data transfer */
#define  SDIO_CCCR_CAP_SMB	0x02	/* can do multi-block xfers (CMD53) */
#define  SDIO_CCCR_CAP_SRW	0x04	/* supports read-wait protocol */
#define  SDIO_CCCR_CAP_SBS	0x08	/* supports suspend/resume */
#define  SDIO_CCCR_CAP_S4MI	0x10	/* interrupt during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_E4MI	0x20	/* enable ints during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_LSC	0x40	/* low speed card */
#define  SDIO_CCCR_CAP_4BLS	0x80	/* 4 bit low speed card */

#define SDIO_CCCR_CIS		0x09	/* common CIS pointer (3 bytes) */

/* Following 4 regs are valid only if SBS is set */
#define SDIO_CCCR_SUSPEND	0x0c
#define SDIO_CCCR_SELx		0x0d
#define SDIO_CCCR_EXECx		0x0e
#define SDIO_CCCR_READYx	0x0f

#define SDIO_CCCR_BLKSIZE	0x10

#define SDIO_CCCR_POWER		0x12

#define SDIO_POWER_SMPC		0x01	/* Supports Master Power Control */
#define SDIO_POWER_EMPC		0x02	/* Enable Master Power Control */

#define SDIO_CCCR_SPEED		0x13

#define SDIO_SPEED_SHS		0x01	/* Supports High-Speed mode */
#define SDIO_SPEED_EHS		0x02	/* Enable High-Speed mode */

/*
 * Function Basic Registers (FBR)
 */
/* SDIO Device FBR Start Address  */
#define SDIOD_FBR_STARTADDR		0x100

/* SDIO Device FBR Size */
#define SDIOD_FBR_SIZE			0x100


#define SDIO_FBR_BASE(f)	((f) * 0x100) /* base of function f's FBRs */

/* Function register offsets */
#define SDIOD_FBR_DEVCTR		0x00	/* basic info for function */
#define SDIOD_FBR_EXT_DEV		0x01	/* extended I/O device code */
#define SDIOD_FBR_PWR_SEL		0x02	/* power selection bits */

/* SDIO Function CIS ptr offset */
#define SDIOD_FBR_CISPTR_0		0x09
#define SDIOD_FBR_CISPTR_1		0x0A
#define SDIOD_FBR_CISPTR_2		0x0B

/* Code Storage Area pointer */
#define SDIOD_FBR_CSA_ADDR_0		0x0C
#define SDIOD_FBR_CSA_ADDR_1		0x0D
#define SDIOD_FBR_CSA_ADDR_2		0x0E
#define SDIOD_FBR_CSA_DATA		0x0F

/* SDIO Function I/O Block Size */
#define SDIOD_FBR_BLKSIZE_0		0x10
#define SDIOD_FBR_BLKSIZE_1		0x11

/* devctr */
#define SDIOD_FBR_DEVCTR_DIC	0x0f	/* device interface code */
#define SDIOD_FBR_DECVTR_CSA	0x40	/* CSA support flag */
#define SDIOD_FBR_DEVCTR_CSA_EN	0x80	/* CSA enabled */
/* interface codes */
#define SDIOD_DIC_NONE		0	/* SDIO standard interface is not supported */
#define SDIOD_DIC_UART		1
#define SDIOD_DIC_BLUETOOTH_A	2
#define SDIOD_DIC_BLUETOOTH_B	3
#define SDIOD_DIC_GPS		4
#define SDIOD_DIC_CAMERA	5
#define SDIOD_DIC_PHS		6
#define SDIOD_DIC_WLAN		7
#define SDIOD_DIC_EXT		0xf	/* extended device interface, read ext_dev register */


#define SDIO_FBR_STD_IF		0x00

#define  SDIO_FBR_SUPPORTS_CSA	0x40	/* supports Code Storage Area */
#define  SDIO_FBR_ENABLE_CSA	0x80	/* enable Code Storage Area */

#define SDIO_FBR_STD_IF_EXT	0x01

#define SDIO_FBR_POWER		0x02

#define  SDIO_FBR_POWER_SPS	0x01	/* Supports Power Selection */
#define  SDIO_FBR_POWER_EPS	0x02	/* Enable (low) Power Selection */

#define SDIO_FBR_CIS		0x09	/* CIS pointer (3 bytes) */


#define SDIO_FBR_CSA		0x0C	/* CSA pointer (3 bytes) */

#define SDIO_FBR_CSA_DATA	0x0F

#define SDIO_FBR_BLKSIZE	0x10	/* block size (2 bytes) */

#define SDIO_R    0
#define SDIO_W    1

#define SDIO_FUN_0          0
#define SDIO_FUN_1          1
#define SDIO_FUN_2          2
#define SDIO_FUN_3          3
#define SDIO_FUN_4          4
#define SDIO_FUN_5          5
#define SDIO_FUN_6          6
#define SDIO_FUN_7          7

/* SDIO Device CCCR offsets */
#define SDIOD_CCCR_REV			0x00
#define SDIOD_CCCR_SDREV		0x01
#define SDIOD_CCCR_IOEN			0x02
#define SDIOD_CCCR_IORDY		0x03
#define SDIOD_CCCR_INTEN		0x04
#define SDIOD_CCCR_INTPEND		0x05
#define SDIOD_CCCR_IOABORT		0x06
#define SDIOD_CCCR_BICTRL		0x07
#define SDIOD_CCCR_CAPABLITIES		0x08
#define SDIOD_CCCR_CISPTR_0		0x09
#define SDIOD_CCCR_CISPTR_1		0x0A
#define SDIOD_CCCR_CISPTR_2		0x0B
#define SDIOD_CCCR_BUSSUSP		0x0C
#define SDIOD_CCCR_FUNCSEL		0x0D
#define SDIOD_CCCR_EXECFLAGS		0x0E
#define SDIOD_CCCR_RDYFLAGS		0x0F
#define SDIOD_CCCR_BLKSIZE_0		0x10
#define SDIOD_CCCR_BLKSIZE_1		0x11
#define SDIOD_CCCR_POWER_CONTROL	0x12
#define SDIOD_CCCR_SPEED_CONTROL	0x13
#define SDIOD_CCCR_UHSI_SUPPORT		0x14
#define SDIOD_CCCR_DRIVER_STRENGTH	0x15
#define SDIOD_CCCR_INTR_EXTN		0x16


#define SD_CARD_TYPE_UNKNOWN	0	/* bad type or unrecognized */
#define SD_CARD_TYPE_IO		1	/* IO only card */
#define SD_CARD_TYPE_MEMORY	2	/* memory only card */
#define SD_CARD_TYPE_COMBO	3	/* IO and memory combo card */

#define SDIO_MAX_BLOCK_SIZE	2048	/* maximum block size for block mode operation */
#define SDIO_MIN_BLOCK_SIZE	1	/* minimum block size for block mode operation */

/* Card registers: status bit position */
#define CARDREG_STATUS_BIT_OUTOFRANGE		31
#define CARDREG_STATUS_BIT_COMCRCERROR		23
#define CARDREG_STATUS_BIT_ILLEGALCOMMAND	22
#define CARDREG_STATUS_BIT_ERROR		19
#define CARDREG_STATUS_BIT_IOCURRENTSTATE3	12
#define CARDREG_STATUS_BIT_IOCURRENTSTATE2	11
#define CARDREG_STATUS_BIT_IOCURRENTSTATE1	10
#define CARDREG_STATUS_BIT_IOCURRENTSTATE0	9
#define CARDREG_STATUS_BIT_FUN_NUM_ERROR	4



#define SD_CMD_GO_IDLE_STATE		0	/* mandatory for SDIO */
#define SD_CMD_SEND_OPCOND		1
#define SD_CMD_MMC_SET_RCA		3
#define SD_CMD_IO_SEND_OP_COND		5	/* mandatory for SDIO */
#define SD_CMD_SELECT_DESELECT_CARD	7
#define SD_CMD_SEND_CSD			9
#define SD_CMD_SEND_CID			10
#define SD_CMD_STOP_TRANSMISSION	12
#define SD_CMD_SEND_STATUS		13
#define SD_CMD_GO_INACTIVE_STATE	15
#define SD_CMD_SET_BLOCKLEN		16
#define SD_CMD_READ_SINGLE_BLOCK	17
#define SD_CMD_READ_MULTIPLE_BLOCK	18
#define SD_CMD_WRITE_BLOCK		24
#define SD_CMD_WRITE_MULTIPLE_BLOCK	25
#define SD_CMD_PROGRAM_CSD		27
#define SD_CMD_SET_WRITE_PROT		28
#define SD_CMD_CLR_WRITE_PROT		29
#define SD_CMD_SEND_WRITE_PROT		30
#define SD_CMD_ERASE_WR_BLK_START	32
#define SD_CMD_ERASE_WR_BLK_END		33
#define SD_CMD_ERASE			38
#define SD_CMD_LOCK_UNLOCK		42
#define SD_CMD_IO_RW_DIRECT		52	/* mandatory for SDIO */
#define SD_CMD_IO_RW_EXTENDED		53	/* mandatory for SDIO */
#define SD_CMD_APP_CMD			55
#define SD_CMD_GEN_CMD			56
#define SD_CMD_READ_OCR			58
#define SD_CMD_CRC_ON_OFF		59	/* mandatory for SDIO */
#define SD_ACMD_SD_STATUS		13
#define SD_ACMD_SEND_NUM_WR_BLOCKS	22
#define SD_ACMD_SET_WR_BLOCK_ERASE_CNT	23
#define SD_ACMD_SD_SEND_OP_COND		41
#define SD_ACMD_SET_CLR_CARD_DETECT	42
#define SD_ACMD_SEND_SCR		51

/* argument for SD_CMD_IO_RW_DIRECT and SD_CMD_IO_RW_EXTENDED */
#define SD_IO_OP_READ		0   /* Read_Write: Read */
#define SD_IO_OP_WRITE		1   /* Read_Write: Write */
#define SD_IO_RW_NORMAL		0   /* no RAW */
#define SD_IO_RW_RAW		1   /* RAW */
#define SD_IO_BYTE_MODE		0   /* Byte Mode */
#define SD_IO_BLOCK_MODE	1   /* BlockMode */
#define SD_IO_FIXED_ADDRESS	0   /* fix Address */
#define SD_IO_INCREMENT_ADDRESS	1   /* IncrementAddress */

/* build SD_CMD_IO_RW_DIRECT Argument */
#define SDIO_IO_RW_DIRECT_ARG(rw, raw, func, addr, data) \
	((((rw) & 1) << 31) | (((func) & 0x7) << 28) | (((raw) & 1) << 27) | \
	 (((addr) & 0x1FFFF) << 9) | ((data) & 0xFF))

/* build SD_CMD_IO_RW_EXTENDED Argument */
#define SDIO_IO_RW_EXTENDED_ARG(rw, blk, func, addr, inc_addr, count) \
	((((rw) & 1) << 31) | (((func) & 0x7) << 28) | (((blk) & 1) << 27) | \
	 (((inc_addr) & 1) << 26) | (((addr) & 0x1FFFF) << 9) | ((count) & 0x1FF))

/* SDIO response parameters */
#define SD_RSP_NO_NONE			0
#define SD_RSP_NO_1			1
#define SD_RSP_NO_2			2
#define SD_RSP_NO_3			3
#define SD_RSP_NO_4			4
#define SD_RSP_NO_5			5
#define SD_RSP_NO_6			6

	/* Modified R6 response (to CMD3) */
#define SD_RSP_MR6_COM_CRC_ERROR	0x8000
#define SD_RSP_MR6_ILLEGAL_COMMAND	0x4000
#define SD_RSP_MR6_ERROR		0x2000

	/* Modified R1 in R4 Response (to CMD5) */
#define SD_RSP_MR1_SBIT			0x80
#define SD_RSP_MR1_PARAMETER_ERROR	0x40
#define SD_RSP_MR1_RFU5			0x20
#define SD_RSP_MR1_FUNC_NUM_ERROR	0x10
#define SD_RSP_MR1_COM_CRC_ERROR	0x08
#define SD_RSP_MR1_ILLEGAL_COMMAND	0x04
#define SD_RSP_MR1_RFU1			0x02
#define SD_RSP_MR1_IDLE_STATE		0x01

	/* R5 response (to CMD52 and CMD53) */
#define SD_RSP_R5_COM_CRC_ERROR		0x80
#define SD_RSP_R5_ILLEGAL_COMMAND	0x40
#define SD_RSP_R5_IO_CURRENTSTATE1	0x20
#define SD_RSP_R5_IO_CURRENTSTATE0	0x10
#define SD_RSP_R5_ERROR			0x08
#define SD_RSP_R5_RFU			0x04
#define SD_RSP_R5_FUNC_NUM_ERROR	0x02
#define SD_RSP_R5_OUT_OF_RANGE		0x01

#define SD_RSP_R5_ERRBITS		0xCB


/* ------------------------------------------------
 *  SDIO Commands and responses
 *
 *  I/O only commands are:
 *      CMD0, CMD3, CMD5, CMD7, CMD14, CMD15, CMD52, CMD53
 * ------------------------------------------------
 */

/* SDIO Commands */
#define SDIOH_CMD_0		0
#define SDIOH_CMD_3		3
#define SDIOH_CMD_5		5
#define SDIOH_CMD_7		7
#define SDIOH_CMD_11		11
#define SDIOH_CMD_14		14
#define SDIOH_CMD_15		15
#define SDIOH_CMD_19		19
#define SDIOH_CMD_52		52
#define SDIOH_CMD_53		53
#define SDIOH_CMD_59		59

/* SDIO Command Responses */
#define SDIOH_RSP_NONE		0
#define SDIOH_RSP_R1		1
#define SDIOH_RSP_R2		2
#define SDIOH_RSP_R3		3
#define SDIOH_RSP_R4		4
#define SDIOH_RSP_R5		5
#define SDIOH_RSP_R6		6

/*
 *  SDIO Response Error flags
 */
#define SDIOH_RSP5_ERROR_FLAGS	0xCB

/* ------------------------------------------------
 * SDIO Command structures. I/O only commands are:
 *
 * 	CMD0, CMD3, CMD5, CMD7, CMD15, CMD52, CMD53
 * ------------------------------------------------
 */

#define CMD5_OCR_M		BITFIELD_MASK(24)
#define CMD5_OCR_S		0

#define CMD5_S18R_M		BITFIELD_MASK(1)
#define CMD5_S18R_S		24

#define CMD7_RCA_M		BITFIELD_MASK(16)
#define CMD7_RCA_S		16

#define CMD14_RCA_M		BITFIELD_MASK(16)
#define CMD14_RCA_S		16
#define CMD14_SLEEP_M		BITFIELD_MASK(1)
#define CMD14_SLEEP_S		15

#define CMD_15_RCA_M		BITFIELD_MASK(16)
#define CMD_15_RCA_S		16

#define CMD52_DATA_M		BITFIELD_MASK(8)  /* Bits [7:0]    - Write Data/Stuff bits of CMD52
						   */
#define CMD52_DATA_S		0
#define CMD52_REG_ADDR_M	BITFIELD_MASK(17) /* Bits [25:9]   - register address */
#define CMD52_REG_ADDR_S	9
#define CMD52_RAW_M		BITFIELD_MASK(1)  /* Bit  27       - Read after Write flag */
#define CMD52_RAW_S		27
#define CMD52_FUNCTION_M	BITFIELD_MASK(3)  /* Bits [30:28]  - Function number */
#define CMD52_FUNCTION_S	28
#define CMD52_RW_FLAG_M		BITFIELD_MASK(1)  /* Bit  31       - R/W flag */
#define CMD52_RW_FLAG_S		31


#define CMD53_BYTE_BLK_CNT_M	BITFIELD_MASK(9) /* Bits [8:0]     - Byte/Block Count of CMD53 */
#define CMD53_BYTE_BLK_CNT_S	0
#define CMD53_REG_ADDR_M	BITFIELD_MASK(17) /* Bits [25:9]   - register address */
#define CMD53_REG_ADDR_S	9
#define CMD53_OP_CODE_M		BITFIELD_MASK(1)  /* Bit  26       - R/W Operation Code */
#define CMD53_OP_CODE_S		26
#define CMD53_BLK_MODE_M	BITFIELD_MASK(1)  /* Bit  27       - Block Mode */
#define CMD53_BLK_MODE_S	27
#define CMD53_FUNCTION_M	BITFIELD_MASK(3)  /* Bits [30:28]  - Function number */
#define CMD53_FUNCTION_S	28
#define CMD53_RW_FLAG_M		BITFIELD_MASK(1)  /* Bit  31       - R/W flag */
#define CMD53_RW_FLAG_S		31

/* ------------------------------------------------------
 * SDIO Command Response structures for SD1 and SD4 modes
 *  -----------------------------------------------------
 */
#define RSP4_IO_OCR_M		BITFIELD_MASK(24) /* Bits [23:0]  - Card's OCR Bits [23:0] */
#define RSP4_IO_OCR_S		0

#define RSP4_S18A_M			BITFIELD_MASK(1) /* Bits [23:0]  - Card's OCR Bits [23:0] */
#define RSP4_S18A_S			24

#define RSP4_STUFF_M		BITFIELD_MASK(3)  /* Bits [26:24] - Stuff bits */
#define RSP4_STUFF_S		24
#define RSP4_MEM_PRESENT_M	BITFIELD_MASK(1)  /* Bit  27      - Memory present */
#define RSP4_MEM_PRESENT_S	27
#define RSP4_NUM_FUNCS_M	BITFIELD_MASK(3)  /* Bits [30:28] - Number of I/O funcs */
#define RSP4_NUM_FUNCS_S	28
#define RSP4_CARD_READY_M	BITFIELD_MASK(1)  /* Bit  31      - SDIO card ready */
#define RSP4_CARD_READY_S	31

#define RSP6_STATUS_M		BITFIELD_MASK(16) /* Bits [15:0]  - Card status bits [19,22,23,12:0]
						   */
#define RSP6_STATUS_S		0
#define RSP6_IO_RCA_M		BITFIELD_MASK(16) /* Bits [31:16] - RCA bits[31-16] */
#define RSP6_IO_RCA_S		16

#define RSP1_AKE_SEQ_ERROR_M	BITFIELD_MASK(1)  /* Bit 3       - Authentication seq error */
#define RSP1_AKE_SEQ_ERROR_S	3
#define RSP1_APP_CMD_M		BITFIELD_MASK(1)  /* Bit 5       - Card expects ACMD */
#define RSP1_APP_CMD_S		5
#define RSP1_READY_FOR_DATA_M	BITFIELD_MASK(1)  /* Bit 8       - Ready for data (buff empty) */
#define RSP1_READY_FOR_DATA_S	8
#define RSP1_CURR_STATE_M	BITFIELD_MASK(4)  /* Bits [12:9] - State of card
						   * when Cmd was received
						   */
#define RSP1_CURR_STATE_S	9
#define RSP1_EARSE_RESET_M	BITFIELD_MASK(1)  /* Bit 13   - Erase seq cleared */
#define RSP1_EARSE_RESET_S	13
#define RSP1_CARD_ECC_DISABLE_M	BITFIELD_MASK(1)  /* Bit 14   - Card ECC disabled */
#define RSP1_CARD_ECC_DISABLE_S	14
#define RSP1_WP_ERASE_SKIP_M	BITFIELD_MASK(1)  /* Bit 15   - Partial blocks erased due to W/P */
#define RSP1_WP_ERASE_SKIP_S	15
#define RSP1_CID_CSD_OVERW_M	BITFIELD_MASK(1)  /* Bit 16   - Illegal write to CID or R/O bits
						   * of CSD
						   */
#define RSP1_CID_CSD_OVERW_S	16
#define RSP1_ERROR_M		BITFIELD_MASK(1)  /* Bit 19   - General/Unknown error */
#define RSP1_ERROR_S		19
#define RSP1_CC_ERROR_M		BITFIELD_MASK(1)  /* Bit 20   - Internal Card Control error */
#define RSP1_CC_ERROR_S		20
#define RSP1_CARD_ECC_FAILED_M	BITFIELD_MASK(1)  /* Bit 21   - Card internal ECC failed
						   * to correct data
						   */
#define RSP1_CARD_ECC_FAILED_S	21
#define RSP1_ILLEGAL_CMD_M	BITFIELD_MASK(1)  /* Bit 22   - Cmd not legal for the card state */
#define RSP1_ILLEGAL_CMD_S	22
#define RSP1_COM_CRC_ERROR_M	BITFIELD_MASK(1)  /* Bit 23   - CRC check of previous command failed
						   */
#define RSP1_COM_CRC_ERROR_S	23
#define RSP1_LOCK_UNLOCK_FAIL_M	BITFIELD_MASK(1)  /* Bit 24   - Card lock-unlock Cmd Seq error */
#define RSP1_LOCK_UNLOCK_FAIL_S	24
#define RSP1_CARD_LOCKED_M	BITFIELD_MASK(1)  /* Bit 25   - Card locked by the host */
#define RSP1_CARD_LOCKED_S	25
#define RSP1_WP_VIOLATION_M	BITFIELD_MASK(1)  /* Bit 26   - Attempt to program
						   * write-protected blocks
						   */
#define RSP1_WP_VIOLATION_S	26
#define RSP1_ERASE_PARAM_M	BITFIELD_MASK(1)  /* Bit 27   - Invalid erase blocks */
#define RSP1_ERASE_PARAM_S	27
#define RSP1_ERASE_SEQ_ERR_M	BITFIELD_MASK(1)  /* Bit 28   - Erase Cmd seq error */
#define RSP1_ERASE_SEQ_ERR_S	28
#define RSP1_BLK_LEN_ERR_M	BITFIELD_MASK(1)  /* Bit 29   - Block length error */
#define RSP1_BLK_LEN_ERR_S	29
#define RSP1_ADDR_ERR_M		BITFIELD_MASK(1)  /* Bit 30   - Misaligned address */
#define RSP1_ADDR_ERR_S		30
#define RSP1_OUT_OF_RANGE_M	BITFIELD_MASK(1)  /* Bit 31   - Cmd arg was out of range */
#define RSP1_OUT_OF_RANGE_S	31


#define RSP5_DATA_M		BITFIELD_MASK(8)  /* Bits [0:7]   - data */
#define RSP5_DATA_S		0
#define RSP5_FLAGS_M		BITFIELD_MASK(8)  /* Bit  [15:8]  - Rsp flags */
#define RSP5_FLAGS_S		8
#define RSP5_STUFF_M		BITFIELD_MASK(16) /* Bits [31:16] - Stuff bits */
#define RSP5_STUFF_S		16

/* ----------------------------------------------
 * SDIO Command Response structures for SPI mode
 * ----------------------------------------------
 */
#define SPIRSP4_IO_OCR_M	BITFIELD_MASK(16) /* Bits [15:0]    - Card's OCR Bits [23:8] */
#define SPIRSP4_IO_OCR_S	0
#define SPIRSP4_STUFF_M		BITFIELD_MASK(3)  /* Bits [18:16]   - Stuff bits */
#define SPIRSP4_STUFF_S		16
#define SPIRSP4_MEM_PRESENT_M	BITFIELD_MASK(1)  /* Bit  19        - Memory present */
#define SPIRSP4_MEM_PRESENT_S	19
#define SPIRSP4_NUM_FUNCS_M	BITFIELD_MASK(3)  /* Bits [22:20]   - Number of I/O funcs */
#define SPIRSP4_NUM_FUNCS_S	20
#define SPIRSP4_CARD_READY_M	BITFIELD_MASK(1)  /* Bit  23        - SDIO card ready */
#define SPIRSP4_CARD_READY_S	23
#define SPIRSP4_IDLE_STATE_M	BITFIELD_MASK(1)  /* Bit  24        - idle state */
#define SPIRSP4_IDLE_STATE_S	24
#define SPIRSP4_ILLEGAL_CMD_M	BITFIELD_MASK(1)  /* Bit  26        - Illegal Cmd error */
#define SPIRSP4_ILLEGAL_CMD_S	26
#define SPIRSP4_COM_CRC_ERROR_M	BITFIELD_MASK(1)  /* Bit  27        - COM CRC error */
#define SPIRSP4_COM_CRC_ERROR_S	27
#define SPIRSP4_FUNC_NUM_ERROR_M	BITFIELD_MASK(1)  /* Bit  28        - Function number error
							   */
#define SPIRSP4_FUNC_NUM_ERROR_S	28
#define SPIRSP4_PARAM_ERROR_M	BITFIELD_MASK(1)  /* Bit  30        - Parameter Error Bit */
#define SPIRSP4_PARAM_ERROR_S	30
#define SPIRSP4_START_BIT_M	BITFIELD_MASK(1)  /* Bit  31        - Start Bit */
#define SPIRSP4_START_BIT_S	31

#define SPIRSP5_DATA_M			BITFIELD_MASK(8)  /* Bits [23:16]   - R/W Data */
#define SPIRSP5_DATA_S			16
#define SPIRSP5_IDLE_STATE_M		BITFIELD_MASK(1)  /* Bit  24        - Idle state */
#define SPIRSP5_IDLE_STATE_S		24
#define SPIRSP5_ILLEGAL_CMD_M		BITFIELD_MASK(1)  /* Bit  26        - Illegal Cmd error */
#define SPIRSP5_ILLEGAL_CMD_S		26
#define SPIRSP5_COM_CRC_ERROR_M		BITFIELD_MASK(1)  /* Bit  27        - COM CRC error */
#define SPIRSP5_COM_CRC_ERROR_S		27
#define SPIRSP5_FUNC_NUM_ERROR_M	BITFIELD_MASK(1)  /* Bit  28        - Function number error
							   */
#define SPIRSP5_FUNC_NUM_ERROR_S	28
#define SPIRSP5_PARAM_ERROR_M		BITFIELD_MASK(1)  /* Bit  30        - Parameter Error Bit */
#define SPIRSP5_PARAM_ERROR_S		30
#define SPIRSP5_START_BIT_M		BITFIELD_MASK(1)  /* Bit  31        - Start Bit */
#define SPIRSP5_START_BIT_S		31

/* RSP6 card status format; Pg 68 Physical Layer spec v 1.10 */
#define RSP6STAT_AKE_SEQ_ERROR_M	BITFIELD_MASK(1)  /* Bit 3	- Authentication seq error
							   */
#define RSP6STAT_AKE_SEQ_ERROR_S	3
#define RSP6STAT_APP_CMD_M		BITFIELD_MASK(1)  /* Bit 5	- Card expects ACMD */
#define RSP6STAT_APP_CMD_S		5
#define RSP6STAT_READY_FOR_DATA_M	BITFIELD_MASK(1)  /* Bit 8	- Ready for data
							   * (buff empty)
							   */
#define RSP6STAT_READY_FOR_DATA_S	8
#define RSP6STAT_CURR_STATE_M		BITFIELD_MASK(4)  /* Bits [12:9] - Card state at
							   * Cmd reception
							   */
#define RSP6STAT_CURR_STATE_S		9
#define RSP6STAT_ERROR_M		BITFIELD_MASK(1)  /* Bit 13  - General/Unknown error Bit 19
							   */
#define RSP6STAT_ERROR_S		13
#define RSP6STAT_ILLEGAL_CMD_M		BITFIELD_MASK(1)  /* Bit 14  - Illegal cmd for
							   * card state Bit 22
							   */
#define RSP6STAT_ILLEGAL_CMD_S		14
#define RSP6STAT_COM_CRC_ERROR_M	BITFIELD_MASK(1)  /* Bit 15  - CRC previous command
							   * failed Bit 23
							   */
#define RSP6STAT_COM_CRC_ERROR_S	15

#define SDIOH_XFER_TYPE_READ    SD_IO_OP_READ
#define SDIOH_XFER_TYPE_WRITE   SD_IO_OP_WRITE

/* command issue options */
#define CMD_OPTION_DEFAULT	0
#define CMD_OPTION_TUNING	1

/*------------------------------ Global Typedefs -----------------------------*/


/*----------------------------- External Variables ---------------------------*/

/*------------------------- Global Function Prototypes -----------------------*/

extern int mmc_attach_sdio(void *pCard);

extern int sdio_function_init(void * pCard);
extern void sdio_function_cleanup(void);
#endif
