/*
********************************************************************************
*                   Copyright (c) 2009,chenfen
*                         All rights reserved.
*
* File Name：   bulkonly.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             chenfen      2009-2-13          1.0
*    desc:    ORG.
********************************************************************************
*/


#ifndef _BULKONLY_H
#define _BULKONLY_H

#include "sysconfig.h"

/*
--------------------------------------------------------------------------------
  
                        Macro define  
  
--------------------------------------------------------------------------------
*/


//1可配置参数
#define     MAX_LUN     (FLASH_DISK0+FLASH_DISK1+SD_CARD_EN)  /*最大支持的逻辑设备数*/

#ifndef _NANDFLASH_ 
#define     USB_BUF_SIZE       1024*64                //USB BULKONLY BUFFER SIZE  
#else
#define     USB_BUF_SIZE       1024*16                //USB BULKONLY BUFFER SIZE  
#endif

//1常量定义

#define     MAX_CDBLEN											0x10        //最长的CBW长度

/*******************************************************************
CSW返回状态值
*******************************************************************/
#define     CSW_GOOD												0x00        //命令通过
#define     CSW_FAIL												0x01        //命令失败
#define     CSW_PHASE_ERROR                 0x02        //命令有误
/*******************************************************************
Bulk Only传输阶段
*******************************************************************/
#define     K_CommandPhase                  0x00
#define     K_OutDataPhase                  0x01
#define     K_OutDataPhaseWrite             0x02
#define     K_OutResponsePhase              0x03
#define     K_InDataPhase                   0x81
#define     K_InDataPhaseRead               0x82
#define     K_InResponsePhase               0x83
#define     K_InCompletionPhase             0x84
#define     K_XferDirMask                   0x80
#define     K_InCSWPhase                    0x08
/*******************************************************************
Bulk Only命令集
*******************************************************************/
#define     K_SCSICMD_TEST_UNIT_READY                   0x00
#define     K_SCSICMD_REZERO_UNIT                       0x01
#define     K_SCSICMD_REQUEST_SENSE                     0x03
#define     K_SCSICMD_FORMAT_UNIT                       0x04
#define     K_SCSICMD_INQUIRY                           0x12
#define     K_SCSICMD_MODE_SENSE_06                     0x1a
#define     K_SCSICMD_START_STOP_UNIT                   0x1b
#define     K_SCSICMD_SEND_DIAGNOSTIC                   0x1d
#define     K_SCSICMD_PREVENT_ALLOW_MEDIUM_REMOVAL      0x1e
#define     K_SCSICMD_READ_FORMAT_CAPACITIES            0x23
#define     K_SCSICMD_READ_CAPACITY                     0x25
#define     K_SCSICMD_READ_10                           0x28
#define     K_SCSICMD_WRITE_10                          0x2a
#define     K_SCSICMD_SEEK_10                           0x2b
#define     K_SCSICMD_WRITE_AND_VERIFY_10               0x2e
#define     K_SCSICMD_VERIFY_10                         0x2f
#define     K_SCSICMD_MODE_SELECT_06                    0x15
#define     K_SCSICMD_MODE_SELECT_10                    0x55                
#define     K_SCSICMD_MODE_SENSE_10                     0x5a               
#define     K_SCSICMD_READ_12                           0xff
#define     K_SCSICMD_WRITE_12                          0xaa

#define     K_RKCMD_CHECK_REQUEST                       0xe0

/*******************************************************************
MMC-2命令集
*******************************************************************/
#define     K_SCSICMD_READ_SUB_CHANNEL                  0x42
#define     K_SCSICMD_READ_TOC                          0x43
#define     K_SCSICMD_GET_CONFIGURATION                 0x46
#define     K_SCSICMD_GET_EVENT                         0x4a
/*******************************************************************
Sense codes
*******************************************************************/
#define     SCSI_SENSE_NO_SENSE                         0x00
#define     SCSI_SENSE_RECOVERED_ERROR                  0x01
#define     SCSI_SENSE_NOT_READY                        0x02
#define     SCSI_SENSE_MEDIUM_ERROR                     0x03
#define     SCSI_SENSE_HARDWARE_ERROR                   0x04
#define     SCSI_SENSE_ILLEGAL_REQUEST                  0x05
#define     SCSI_SENSE_UNIT_ATTENTION                   0x06
#define     SCSI_SENSE_DATA_PROTECT                     0x07
#define     SCSI_SENSE_BLANK_CHECK                      0x08
#define     SCSI_SENSE_UNIQUE                           0x09
#define     SCSI_SENSE_COPY_ABORTED                     0x0A
#define     SCSI_SENSE_ABORTED_COMMAND                  0x0B
#define     SCSI_SENSE_EQUAL                            0x0C
#define     SCSI_SENSE_VOL_OVERFLOW                     0x0D
#define     SCSI_SENSE_MISCOMPARE                       0x0E
#define     SCSI_SENSE_RESERVED                         0x0F
/*******************************************************************
Additional Sense Codes(ASC)
*******************************************************************/
#define     SCSI_ADSENSE_NO_SENSE                       0x00
#define     SCSI_ADSENSE_LUN_NOT_READY                  0x04
#define     SCSI_ADSENSE_ILLEGAL_COMMAND                0x20
#define     SCSI_ADSENSE_ILLEGAL_BLOCK                  0x21
#define     SCSI_ADSENSE_INVALID_PARAMETER              0x26
#define     SCSI_ADSENSE_INVALID_LUN                    0x25
#define     SCSI_ADSENSE_INVALID_CDB                    0x24
#define     SCSI_ADSENSE_MUSIC_AREA                     0xA0
#define     SCSI_ADSENSE_DATA_AREA                      0xA1
#define     SCSI_ADSENSE_VOLUME_OVERFLOW                0xA7
#define     SCSI_ADSENSE_NO_MEDIA_IN_DEVICE             0x3A
#define     SCSI_ADSENSE_FORMAT_ERROR                   0x31
#define     SCSI_ADSENSE_CMDSEQ_ERROR                   0x2C
#define     SCSI_ADSENSE_MEDIUM_CHANGED                 0x28
#define     SCSI_ADSENSE_BUS_RESET                      0x29
#define     SCSI_ADWRITE_PROTECT                        0x27
#define     SCSI_ADSENSE_TRACK_ERROR                    0x14
#define     SCSI_ADSENSE_SAVE_ERROR                     0x39
#define     SCSI_ADSENSE_SEEK_ERROR                     0x15
#define     SCSI_ADSENSE_REC_DATA_NOECC                 0x17
#define     SCSI_ADSENSE_REC_DATA_ECC                   0x18
/*******************************************************************
 Additional sense code qualifier(ASCQ)
*******************************************************************/
#define     SCSI_SENSEQ_FORMAT_IN_PROGRESS              0x04
#define     SCSI_SENSEQ_INIT_COMMAND_REQUIRED           0x02
#define     SCSI_SENSEQ_MANUAL_INTERVENTION_REQUIRED    0x03
#define     SCSI_SENSEQ_BECOMING_READY                  0x01
#define     SCSI_SENSEQ_FILEMARK_DETECTED               0x01
#define     SCSI_SENSEQ_SETMARK_DETECTED                0x03
#define     SCSI_SENSEQ_END_OF_MEDIA_DETECTED           0x02
#define     SCSI_SENSEQ_BEGINNING_OF_MEDIA_DETECTED     0x04
/*******************************************************************
Mode Sense/Select page constants.
*******************************************************************/
#define     MODE_PAGE_RBC_DEVICE_PARAMETERS             0x06

#define     PAGECTRL_CURRENT                            0x00
#define     PAGECTRL_CHANGEABLE                         0x01
#define     PAGECTRL_DEFAULT                            0x02
#define     PAGECTRL_SAVED                              0x03

/*******************************************************************
Page field for Inquiry vital product data parameters
*******************************************************************/
#define     SUPPORTED_PAGES                             0x00
#define     SERIAL_NUMBER_PAGE                          0x80
#define     DEVICE_INDENIFICATION_PAGE                  0x83



#define     USB_XFER_IDLE           0x00
#define     USB_XFER_OUT_SUSPEND    0x01
#define     USB_XFER_IN_START       0x02
#define     USB_XFER_IN_SUSPEND     0x03
#define     USB_XFER_END_SUSPEND    0x04


#define     USB_XFER_END            0xff

/*
*-------------------------------------------------------------------------------
*
*                            Function Declaration
*
*-------------------------------------------------------------------------------
*/

//1函数原型声明

extern int32  MscReqAdd(uint8 cmd, uint32 lun, uint32 LBA, uint16 len, void *buf);

extern void BulkOutPacket(uint16 len);
extern void BulkInPacket(void);
extern void BulkOutHandle(void);
extern void BulkOnlyCmd(void) ;

extern void SCSICMD_Read12(void);
extern void SCSICMD_TestUnitReady(void);
extern void SCSICMD_RequestSense(void);
extern void SCSICMD_Inquiry(void);
extern void SCSICMD_PreventAllowMediumRemoval(void);
extern void SCSICMD_StartStopUnit(void);
extern void SCSICMD_ReadCapacity(void);
extern void SCSICMD_Read10(void);
extern void SCSICMD_Write10(void);
extern void SCSICMD_Verify10(void);
extern void SCSICMD_ModeSense10(void);
extern void SCSICMD_ReadFormatCapacities(void);
extern void SCSICMD_ModeSense06(void);                
extern void SCSICMD_ReadSubChannel(void);
extern void SCSICMD_ReadToc(void);
extern void SCSICMD_GetConfiguration(void);
extern void SCSICMD_GetEvent(void);
extern bool CBWValidVerify(void);
extern void CSWHandler(uint8 HostDevCase,uint16 DeviceTrDataLen);
extern void BuildSenseData(uint8 SenseKey, uint8 ASC, uint8 ASCQ);
extern void CopyFlashReside(void);
extern void SendCSW(void);

extern void USBFlashDiskProtectON(void);
extern void USBFlashDiskProtectOFF(void);
extern void USBSDDiskProtectON(void);
extern void USBSDDiskProtectOFF(void);

#define USBFileWriteProtect         USBFlashDiskProtectON
#define USBFileWriteUnProtect       USBFlashDiskProtectOFF
#define USBSDFileWriteProtect       USBSDDiskProtectON
#define USBSDFileWriteUnProtect     USBSDDiskProtectOFF     

//外部变量声明
extern  uint8   SCSI_INQUIRY[36];
extern  uint8   SCSI_INQUIRY_SD[];  
extern  uint8 SCSI_INQUIRY_CDROM[];

/*******************************************************************/

//1结构定义

typedef  volatile struct _USB_MSC_REQ
{

    uint8 cmd;

    uint16 len;
    
    uint32 LUN;

    uint32 LBA;

    void *buf;

} USB_MSC_REQ, *pUSB_MSC_REQ;

typedef volatile struct _USB_BULK_XFER
{
    uint32 LBA;
    uint32 NendLen;
    uint32 TransLen;
    uint32 BufLen;
    uint32 XferLen;
    uint16 XferCmd;
    //uint16 DmaLen;
    uint8 *buf;
} USB_BULK_XFER, *pUSB_BULK_XFER;


typedef struct _USBMSDC_CSW
{
    uint8   dCSWSignature[4];
    uint8   dCSWTag[4];
    uint8   dCSWDataResidue[4];
    uint8   bCSWStatus;
} USBMSDC_CSW;

typedef struct _USB_LUN
{
    uint8 LunNum;
    uint8 changed[3];
    uint32 DiskID[3];

} USB_LUN;

// Host Device Disagreement Matrix
enum _HOST_DEV_DISAGREE
{
    CASEOK = 0,
    CASE1,
    CASE2,
    CASE3,
    CASE4,
    CASE5,
    CASE6,
    CASE7,
    CASE8,
    CASE9,
    CASE10,
    CASE11,
    CASE12,
    CASE13,
    CASECBW,
    CASECMDFAIL
};

typedef __packed struct _RKNANO_VERSION{
    uint16 dwBootVer; // Loader version
    uint32 dwFirmwareVer; // System version
    uint32 Reserved; 
}RKNANO_VERSION, *PRKNANO_VERSION; //RKNANO device version struct

//1全局变量
#ifdef USB
#undef  EXT
#ifdef  _IN_BULKONLY_
        #define EXT
#else
        #define EXT     extern
#endif      

         
DRAM_USB    EXT     uint8                   G_ucBulkPhase;   //BULK命令阶段状态字? 
DRAM_USB    EXT     USB_MSC_REQ             UsbMscReq;
DRAM_USB    EXT     USB_MSC_REQ             UsbMscPendReq;
DRAM_USB    EXT     USB_BULK_XFER           UsbBulkXfer;
DRAM_USB    EXT     uint32                  dCBWDaTrLen;   //命令长度
DRAM_USB    EXT     USBMSDC_CSW             pCSW;                		

DRAM_USB    EXT     uint8                   SCSIDATA_RequestSense[18];
DRAM_USB    EXT     USB_LUN                 USBDisk;
DRAM_USB    EXT     uint32                  USBDiskID;

#endif

//1表格定义
#ifdef USB
#ifdef _IN_BULKONLY_
#ifdef USB_DRM_EN
DRAM_USB_TABLE
uint8 InquirySupportedData[] =
{
    0x00,
    SUPPORTED_PAGES,
    0x00,
    0x03,
    SERIAL_NUMBER_PAGE,
    DEVICE_INDENIFICATION_PAGE
 };

DRAM_USB_TABLE
uint8 InquirySnData[] =
{
    0x00,
    SERIAL_NUMBER_PAGE,
    0x00,
    20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20,
    0x20
 };

DRAM_USB_TABLE
uint8 InquiryDeviceData[] =
{
    0x00,
    DEVICE_INDENIFICATION_PAGE,
    0x00,
    26,
    0x02,
    0x01,
    0x00,
    22,
    'R', 'o', 'c', 'k', 'c', 'h', 'i', 'p', ' ', 
    'U', 's', 'b', ' ', 'M', 'p', '3', ' ', 
    ' ', ' ', ' ', ' '
 };
#endif


//#if (SD_CARD_EN == 1)
DRAM_USB_TABLE
uint8  SCSIDATA_Sense_SD[] =
{
    0x03,0x00,0x00,0x00,    //第3字节为0x80表示写保护
    0x01,0x0a,0x00,0x10,
    0x00,0x00,0x00,0x00
};
//#endif
DRAM_USB_TABLE
uint8  SCSIDATA_Sense_FLASH[] =
{
    0x03,0x00,0x00,0x00,    //第3字节为0x80表示写保护
    0x01,0x0a,0x00,0x10,
    0x00,0x00,0x00,0x00
};

#else
    extern  uint8   InquirySupportedData[];
    extern  uint8   InquirySnData[];
    extern  uint8   InquiryDeviceData[];
    extern  uint8   SCSI_INQUIRY_CDROM[];
    extern  uint8   Toc[];
    extern  uint8   SCSIDATA_ReadFormatCapacities[];
    extern  uint8   SCSIDATA_ReadCapacities[];
    extern  uint8   SCSIDATA_Sense_CDROM[];
    extern  uint8   SCSIDATA_Sense_SD[];
    extern  uint8   SCSIDATA_Sense_FLASH[];
#endif
#endif
#endif

/*
********************************************************************************
*
*                         End of bulkonly.h
*
********************************************************************************
*/

