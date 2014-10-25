/*
********************************************************************************
*                   Copyright (c) 2009,chenfen
*                         All rights reserved.
*
* File Name��   bulkonly.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             chenfen      2008-12-12          1.0
*    desc:    ORG.
********************************************************************************
*/

#define  _IN_BULKONLY_
#include "USBConfig.h"

#include "ModuleInfoTab.h"
#include "ModuleOverlay.h"
#include "driverlib_def.h"
#include "delay.h"

 extern uint32 ModuleOverlay(uint32 ModuleNum, uint8 Type)	;
 extern void UsbRKMSCHook(uint32 cmd, uint32 param);
 extern void UsbGetVetsion(PRKNANO_VERSION pVersion);
 extern void UsbGetMacAddr(uint8 *pAddr);

#ifdef USB 

#define     USB_BULK_IN        1
#define     USB_BULK_OUT       0
#define     MIN(x,y) ((x) < (y) ? (x) : (y))


//#ifdef USB_PERMIT
/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_Read10(void)
  Author        : chenfen
  Description   : ����:������0x28
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void USBMscDisconnect(void)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    CSWHandler(CASEOK, 12);
    SendCSW();
    Delay100cyc(6000);
    dev_regs->dctl |= 0x02;       //soft disconnect
    UsbConnected = 0;

}


/*
--------------------------------------------------------------------------------
  Function name : BulkUpdateXfer
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void BulkUpdateXfer(uint32 byte)
{
    pUSB_BULK_XFER Xfer = &UsbBulkXfer;

    Xfer->XferLen += byte;
    Xfer->TransLen += byte;

    if (Xfer->XferLen >= Xfer->BufLen)
    {
        uint32 len = Xfer->XferLen>>9;
        if (Xfer->XferCmd == USB_BULK_OUT)
        {
            MscReqAdd(K_SCSICMD_WRITE_10, USBDiskID,  Xfer->LBA, len, Xfer->buf);
            DISABLE_USB_INT;                        //���ж�, д���ڿ��ж�, ��֤buf������ȷ��

            if (Xfer->TransLen < Xfer->NendLen)
            {
                Xfer->LBA += len;
                Xfer->XferLen = 0;
                len = Xfer->NendLen-Xfer->TransLen;
                Xfer->BufLen = (len>USB_BUF_SIZE)? USB_BUF_SIZE : len;   
            }
        }
        else        //Xfer->XferCmd == USB_BULK_IN
        {
            if (Xfer->TransLen < Xfer->NendLen)
            {
                Xfer->LBA += len;
                //Xfer->XferLen = 0;
                len = Xfer->NendLen-Xfer->TransLen;
                len = (len>USB_BUF_SIZE)? USB_BUF_SIZE : len; 
            
                MscReqAdd(K_SCSICMD_READ_10, USBDiskID,  Xfer->LBA, len>>9, Xfer->buf);
                //DISABLE_USB_INT; 
            }
            else
            {
                G_ucBulkPhase = K_InCSWPhase;
            }
        }
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void BulkInPacket(void)
  Author        : chenfen
  Description   : �������ݴ���IN������
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void BulkInPacket(void)
{  
    if (G_ucBulkPhase == K_InDataPhase)
    {   
        uint32 byte;
        pUSB_BULK_XFER Xfer = &UsbBulkXfer;
        
        byte = Xfer->BufLen - Xfer->XferLen;
        if (byte)
        {
            byte = (byte > BulkEndpInSize) ? BulkEndpInSize : byte; 

            #ifdef DMA_BULKIN_EN
            //Xfer->DmaLen = byte; 
            UsbWriteEpDma(BULK_IN_EP, byte, Xfer->buf+Xfer->XferLen, NULL);
            #else
            UsbWriteEndpoint(BULK_IN_EP, byte, Xfer->buf+Xfer->XferLen);
            #endif
            BulkUpdateXfer(byte);
        }
    }   
    else if (G_ucBulkPhase== K_InCSWPhase)  // return the csw packet 
    {
        SendCSW();   
    }
    else
    {
        ;//UsbSetEpSts(1, BULK_IN_EP, 1);        
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void BulkOutPacket(void)
  Author        : chenfen
  Description   : �������ݴ���OUT������
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void BulkOutPacket(uint16 len)
{
    pUSB_BULK_XFER Xfer = &UsbBulkXfer;
    
    if (G_ucBulkPhase == K_CommandPhase)        //��������
    {
        CBWLen = len;
        UsbReadEndpoint(BULK_OUT_EP, len, EpBuf);       
    }
    else if (G_ucBulkPhase == K_OutDataPhase)//��������
    {

        if (Xfer->XferLen>=Xfer->BufLen)
            return;

        #ifdef DMA_BULKOUT_EN
        //Xfer->DmaLen = len;
        UsbReadEpDma(BULK_OUT_EP, len, Xfer->buf+Xfer->XferLen, NULL);
        #else
        UsbReadEndpoint(BULK_OUT_EP, len, Xfer->buf+Xfer->XferLen);
        #endif
        BulkUpdateXfer(len);
    }

}

/*
--------------------------------------------------------------------------------
  Function name : void BulkOutHandle(uint16 len)
  Author        : chenfen
  Description   : �������ݴ���BulkOnly�������
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void BulkOutHandle(void)
{
    
    if (G_ucBulkPhase == K_CommandPhase)
    {
        if (CBWValidVerify() == OK)
        {
            BulkOnlyCmd();
        }
        else
        {
            CSWHandler(CASECBW,0);
        }
    }    
    else if (G_ucBulkPhase == K_OutDataPhase)
    {         
    
    }
}

#ifdef USB_IF_TEST
/*
--------------------------------------------------------------------------------
  Function name : BulkOnlyChkCmd
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
uint32 BulkOnlyChkCmd(uint8 DevDir, uint32 DevLen)
{
    uint32 ret = OK;
    
    if (0 == dCBWDaTrLen)
    {   
        if (DevLen)
        {
            CSWHandler(DevDir? CASE2: CASE3, 0);
            ret = ERROR;
        }
        //else CSWHandler(CASE1,0);
    }
    else
    {
        uint8 HostDir  = (EpBuf[12]>>7) & 0x1;
        if (HostDir)
        {
            if (dCBWDaTrLen > DevLen)
            {
                CSWHandler(DevLen? CASE4 : CASE5, DevLen);
                ret = ERROR;
            }
            else if (HostDir != DevDir || dCBWDaTrLen < DevLen)
            {
                CSWHandler((HostDir != DevDir)? CASE8 : CASE7, DevLen);
                ret = ERROR;
            }
            // else CSWHandler(CASE6,DevLen);         
        }
        else
        {
            if (dCBWDaTrLen > DevLen)
            {
                CSWHandler(DevLen? CASE9 : CASE11, DevLen);
                ret = ERROR;
            }
            else if (HostDir != DevDir || dCBWDaTrLen < DevLen)
            {
                CSWHandler((HostDir != DevDir)? CASE10 : CASE13, DevLen);
                ret = ERROR;
            }
            // else CSWHandler(CASE12,DevLen);  
        }
    }

    if (OK != ret)
        G_ucBulkPhase =K_InCSWPhase;

    return ret;
}
#endif

/*
--------------------------------------------------------------------------------
  Function name : void BulkOnlyCmd(void)
  Author        : chenfen
  Description   : �������ݴ���BulkOnly�������
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void BulkOnlyCmd(void)
{ 
    uint16 flag;
    
    dCBWDaTrLen=((uint32)EpBuf[11] << 8)+EpBuf[10];
    dCBWDaTrLen <<= 16;
    dCBWDaTrLen +=((uint32)EpBuf[9] << 8)+EpBuf[8]; // copmpute the length

    switch (EpBuf[15])
    {
        //6-byte command
        case K_SCSICMD_TEST_UNIT_READY:     //0x00
            SCSICMD_TestUnitReady();
            break;
        case K_SCSICMD_REQUEST_SENSE:           //0x03
            SCSICMD_RequestSense();
            break;
        case K_SCSICMD_INQUIRY:             //0x12
            SCSICMD_Inquiry();
            break;
        case K_SCSICMD_MODE_SENSE_06:           //0x1a                
            SCSICMD_ModeSense06();
            break;
        case K_SCSICMD_PREVENT_ALLOW_MEDIUM_REMOVAL:        //0x1e
            SCSICMD_PreventAllowMediumRemoval();
            break;
        case K_SCSICMD_START_STOP_UNIT:     //0x1b   
            SCSICMD_StartStopUnit();            
            break;

        case K_SCSICMD_MODE_SELECT_06:          //0x15
        case K_SCSICMD_MODE_SELECT_10:          //0x55
            CSWHandler(CASECMDFAIL, 0);
            SendCSW(); 
            break;
#if 0
        case K_SCSICMD_MODE_SELECT_06:          //0x15   
            SCSICMD_ModeSelect06();
            G_ucBulkPhase = K_OutDataPhase;
            break;
        case K_SCSICMD_REZERO_UNIT:         //0x01
            SCSICMD_RezeroUnit();
            G_ucBulkPhase = K_InResponsePhase;
            break;
        case K_SCSICMD_FORMAT_UNIT:         //0x04
            SCSICMD_FormatUnit();
            G_ucBulkPhase = K_OutDataPhase;
            break;
        case K_SCSICMD_SEND_DIAGNOSTIC:     //0x1d   
            SCSICMD_SendDiagnostic();
            G_ucBulkPhase = K_InResponsePhase;
            break;
#endif
        //10-byte command
        case K_SCSICMD_READ_CAPACITY:           //0x25
            SCSICMD_ReadCapacity();
            break;
        case K_SCSICMD_READ_10:             //0x28         
            SCSICMD_Read10();
            break;
        case K_SCSICMD_WRITE_10:                //0x2a            
            SCSICMD_Write10();
            break;
        case K_SCSICMD_VERIFY_10:               //0x2f
            SCSICMD_Verify10();
            break;
        case K_SCSICMD_MODE_SENSE_10:           //0x5a   
            SCSICMD_ModeSense10();
            break;
#if 0
        case K_SCSICMD_SEEK_10:                 //0x2b
            SCSICMD_Seek10();
            G_ucBulkPhase = K_InResponsePhase;
            break;
        case K_SCSICMD_WRITE_AND_VERIFY_10:  //0x2e
            SCSICMD_WriteAndVerify10();
            G_ucBulkPhase = K_OutDataPhaseWrite;
            break;
        case K_SCSICMD_MODE_SELECT_10:          //0x55
            SCSICMD_ModeSelect10();
            G_ucBulkPhase = K_OutDataPhase;
            break;
#endif
        //12-byte command
        case K_SCSICMD_READ_FORMAT_CAPACITIES:          //0x23                              
            SCSICMD_ReadFormatCapacities();
            break;

//#ifdef USB_PERMIT
        case K_SCSICMD_READ_12:                 //0xff
            SCSICMD_Read12();
            break;
//#endif

#ifdef CD_ROM_EN
		case K_SCSICMD_READ_SUB_CHANNEL:		//0x42
			SCSICMD_ReadSubChannel();
			break;
		case K_SCSICMD_READ_TOC:				//0x43
			SCSICMD_ReadToc();
			break;
		case K_SCSICMD_GET_CONFIGURATION:		//0x46
			SCSICMD_GetConfiguration();
			break;
		case K_SCSICMD_GET_EVENT:				//0x4a
			SCSICMD_GetEvent();
			break;
#endif

        default:
            CSWHandler(CASECBW,0);
            BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST,SCSI_ADSENSE_ILLEGAL_COMMAND,0);
            G_ucBulkPhase =K_InCSWPhase;
            break;
      }                
}

#ifdef USB_DRM_EN
const unsigned char g_pmid[] =
{ 
    0x32, 0x33, 0x44, 0x45, 0x38, 0x45, 0x32, 0x41, 0x39, 0x34, 
	0x45, 0x44, 0x43, 0x43, 0x30, 0x46
};
/***************************************************************************
��������:�޸�DRM9 IDֵ
��ڲ���:
���ڲ���:
���ú���:
***************************************************************************/
IRAM_USB
void Modify_DRM9ID(void)
{
    UINT32 i;

    InquirySnData[3] = 16;

    for (i = 0;i < 16; i++)
    {
        InquirySnData[i+4] = g_pmid[i];
    }
}
#endif

//#ifdef USB_PERMIT
/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_Read10(void)
  Author        : chenfen
  Description   : ����:������0x28
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_Read12(void)
{
	UINT8 tempbuf[512];
    UINT8  cbLun;
    UINT16  cbLen;
    uint32 Read12LBA;

    Read12LBA=((uint32)EpBuf[17] << 8)+EpBuf[18];
    Read12LBA <<= 16;
    Read12LBA += ((uint32)EpBuf[19] << 8)+EpBuf[20];   
     
    cbLun = EpBuf[16];
    cbLen = (UINT16)EpBuf[21]|(UINT16)(EpBuf[22]<<8);

    if (cbLun == 0XE0)
    {
        if (Read12LBA == 0xFFFFFFFF)    //GET VERSION.
        {
            if (cbLen == 12)            //�汾����
            {
                RKNANO_VERSION version;

                UsbGetVetsion(&version);
                
                CSWHandler(CASEOK, 12);
                UsbWriteEndpoint(BULK_IN_EP, 12, (uint8*)&version);
                SendCSW();
                return ;
            }
        }
        else if (Read12LBA == 0xFFFFFFFD)
        {
            ;
        }
        else if (Read12LBA == 0xFFFFFFFE)
            //||  Read12LBA == 0xFFFFFFF6)    // Show User Disk, not enum the cdrom
        {
            //DISABLE_USB_INT;
            USBMscDisconnect();
        }
        else if (Read12LBA == 0xFFFFFFEE) // Get mac address
        {
            uint8 MacAddr[6];

            UsbGetMacAddr(MacAddr);
            CSWHandler(CASEOK, 6);
            UsbWriteEndpoint(BULK_IN_EP, 6, MacAddr);
            SendCSW();
            return ;
        }
#if 0            
        else if (Read12LBA == 0xFFFFFFFB)  //ֻ�����û���
        {
            otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;
            //DISABLE_USB_INT;
 			CSWHandler(CASEOK, 12);
			SendCSW();
			Delay100cyc(6000);
			dev_regs->dctl |= 0x02;       //soft disconnect
			UsbConnected = 0;
        }    
        else if (Read12LBA == 0xFFFFFFF8) //�����û���
        {
            if (cbLen == 16)
            {
                //USBPermit = USB_PERMIT_ENABLE;
                FlashValid = 1;
                CSWHandler(CASEOK, 12);
                UsbWriteEndpoint(BULK_IN_EP, 16, tempbuf);
                SendCSW();
                return ;
            }

        }
        else if (Read12LBA == 0xFFFFFFF9)  //�����û���
        {
            if (cbLen == 16)
            {
                //USBPermit = USB_PERMIT_DISABLE;
                FlashValid = 0;
                CSWHandler(CASEOK, 12);
                UsbWriteEndpoint(BULK_IN_EP, 16, tempbuf);
                SendCSW();
                return ;
            }
        }
#endif        
        else if (Read12LBA == 0xFFFFFFFC)  //GETCHIPINFO
        {
            if (cbLen == 16)
            {
                strcpy(tempbuf, "RKNANOC");
                tempbuf[7] = '\0';
                CSWHandler(CASEOK, 12);
                UsbWriteEndpoint(BULK_IN_EP, 16, tempbuf);
                SendCSW();
                return ;
            }
        }
        else if (Read12LBA == 0xFFFFFFF6)		// Show User Disk, not enum the cdrom
        {
             USBMscDisconnect();
        }
 
        UsbRKMSCHook(Read12LBA, cbLen);

    }

    G_ucBulkPhase=K_InCSWPhase;
}
//#endif

/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_Inquiry(void)
  Author        : chenfen
  Description   : ����:��ѯ0x12
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_Inquiry(void)
{
    uint16 residue;
    //if (OK != BulkOnlyChkCmd(1, EpBuf[19]))
    //    return;
    if (0 == EpBuf[19])
    {
        CSWHandler(CASE6, EpBuf[19]);
        SendCSW();
        return;
    }

    BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);

    #ifdef USB_DRM_EN
    if (EpBuf[16] & 0x01)   //EVPD
    {
        if (EpBuf[17] == SUPPORTED_PAGES)
        {
            residue = MIN(sizeof(InquirySupportedData), EpBuf[19]);
            CSWHandler(CASE6, residue); 
            UsbWriteEndpoint(BULK_IN_EP, residue, InquirySupportedData);
        }
        #if 1
        else if (EpBuf[17]== SERIAL_NUMBER_PAGE)
        {
            residue = MIN(sizeof(InquirySnData), EpBuf[19]);
            CSWHandler(CASE6, residue);
            Modify_DRM9ID();
            UsbWriteEndpoint(BULK_IN_EP, residue, InquirySnData);
        }
        #endif
        else if (EpBuf[17] == DEVICE_INDENIFICATION_PAGE)
        {
            residue = MIN(sizeof(InquiryDeviceData), EpBuf[19]);
            CSWHandler(CASE6, residue);
            UsbWriteEndpoint(BULK_IN_EP, residue, InquiryDeviceData);
        }
        else
        {
            BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_INVALID_CDB, 0);
            CSWHandler(CASECBW, 0);
        }
    }
    else
    #endif        
    {
        CSWHandler(CASE6, EpBuf[19]);   //modify by lxs @2007.03.13, for Mac OS 9.2.2

        if (MD_MAJOR(USBDiskID) == MEM_DEV_NAND
           || MD_MAJOR(USBDiskID) == MEM_DEV_EMMC)
        {
            uint8 *pInquiry = ((UsbShowCdrom()==1)&&(MD_MINOR(USBDiskID)==1))? SCSI_INQUIRY_CDROM : SCSI_INQUIRY;
            residue= MIN(sizeof(SCSI_INQUIRY), EpBuf[19]);          
            UsbWriteEndpoint(BULK_IN_EP, residue, pInquiry);
        }        
        else if (MD_MAJOR(USBDiskID) == MEM_DEV_SD)
        {
            //uint16 residue = MIN(sizeof(SCSI_INQUIRY_SD), EpBuf[19]);
			CSWHandler(CASE6, EpBuf[19]);//modify by lxs @2008.01.21, for Mac OS 9.2.2
            UsbWriteEndpoint(BULK_IN_EP, EpBuf[19], SCSI_INQUIRY_SD);
        }
    }

    G_ucBulkPhase = K_InCSWPhase;
}


/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_ReadFormatCapacities(void)
  Author        : chenfen
  Description   : ����:���ɸ�ʽ������0x23
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_ReadFormatCapacities(void)
{
    uint8 capacity[12];
    uint32 TotalSecs;
    uint16 BytsPerSec;

    if (MDChkValid(USBDiskID))
    {
        TotalSecs = MDGetCapacity(USBDiskID);
        BytsPerSec=512;
    }
    else
    {
        BuildSenseData(SCSI_SENSE_NOT_READY, SCSI_ADSENSE_NO_MEDIA_IN_DEVICE, 0);
        CSWHandler(CASECBW, 0);
        return;
    }
    
    CSWHandler(CASE6,12);
    BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
    capacity[0]=0;
    capacity[1]=0;
    capacity[2]=0;
    capacity[3]=8;
    capacity[4]=(uint8)((TotalSecs >> 24) & 0x00ff);
    capacity[5]=(uint8)((TotalSecs >> 16) & 0x00ff);
    capacity[6]=(uint8)((TotalSecs >> 8) & 0x00ff);
    capacity[7]=(uint8)(TotalSecs & 0x00ff);
    capacity[8]=0x01;       
    capacity[9]=0x00;
    capacity[10]=(uint8)(BytsPerSec >> 8);      //bytes/block
    capacity[11]=(uint8)(BytsPerSec & 0x00ff);
    
    UsbWriteEndpoint(BULK_IN_EP, 12, capacity);
    G_ucBulkPhase=K_InCSWPhase;
}

/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_ReadCapacity(void)
  Author        : chenfen
  Description   : ����:������0x25
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_ReadCapacity(void)
{
    uint8 capacity[8];
    uint32 TotalSecs;
    uint16 BytsPerSec;

    if (MDChkValid(USBDiskID))
    {
        TotalSecs = MDGetCapacity(USBDiskID)-1;
        BytsPerSec=512;
    }
    else
    {
        //BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_INVALID_CDB, 0);
        BuildSenseData(SCSI_SENSE_NOT_READY, SCSI_ADSENSE_NO_MEDIA_IN_DEVICE, 0); 
        CSWHandler(CASECBW, 0);
        return;
    }

    CSWHandler(CASE6,8);
    BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);

    capacity[0]=(uint8)((TotalSecs >> 24) & 0x00ff);
    capacity[1]=(uint8)((TotalSecs >> 16) & 0x00ff);
    capacity[2]=(uint8)(TotalSecs >> 8) & 0x00ff;
    capacity[3]=(uint8)(TotalSecs & 0x00ff);
    capacity[4]=0x00;       
    capacity[5]=0x00;
    capacity[6]=(uint8)(BytsPerSec >> 8);       //bytes/block
    capacity[7]=(uint8)(BytsPerSec & 0x00ff);

    UsbWriteEndpoint(BULK_IN_EP, 8, capacity);
    G_ucBulkPhase=K_InCSWPhase;
}


/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_Read10(void)
  Author        : chenfen
  Description   : ����:������0x28
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_Read10(void)
{
    uint16 nSec;
    uint32 byte;
    pUSB_BULK_XFER Xfer = &UsbBulkXfer;
    
    if (! MDChkValid(USBDiskID))
    {
        BuildSenseData(SCSI_SENSE_NOT_READY, SCSI_ADSENSE_NO_MEDIA_IN_DEVICE, 0);
        CSWHandler(CASECBW, 0);
        return;
    }

    CSWHandler(CASE6,(uint16)dCBWDaTrLen);
    BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);

    nSec = ((uint16)EpBuf[22] << 8) + EpBuf[23];
    byte = (uint32)nSec<<9;//sec to byte

    #ifdef USB_IF_TEST
    if (OK != BulkOnlyChkCmd(1, byte))
        return;
    #endif        
    
    Xfer->LBA =((uint32)EpBuf[17] << 8)+EpBuf[18];
    Xfer->LBA <<= 16;
    Xfer->LBA += ((uint32)EpBuf[19] << 8)+EpBuf[20];
    Xfer->NendLen = byte;
    Xfer->TransLen = 0;
    Xfer->XferLen = Xfer->BufLen = 0;
    nSec = (nSec > USB_BUF_SIZE/512)? USB_BUF_SIZE/512:nSec;

    Xfer->XferCmd = USB_BULK_IN;
    G_ucBulkPhase=K_InDataPhase;

    MscReqAdd(K_SCSICMD_READ_10, USBDiskID, Xfer->LBA, nSec, Xfer->buf);

}

/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_Write10(void)
  Author        : chenfen
  Description   : ����:д����0x2a
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_Write10(void)
{
    uint16 nSec;
    uint32 byte;
    pUSB_BULK_XFER Xfer = &UsbBulkXfer;
    
    if (! MDChkValid(USBDiskID))
    {
        BuildSenseData(SCSI_SENSE_NOT_READY, SCSI_ADSENSE_NO_MEDIA_IN_DEVICE, 0);
        CSWHandler(CASECBW, 0);
        return;
    }

    CSWHandler(CASE12,(uint16)dCBWDaTrLen);
    BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);

    nSec =((uint16)EpBuf[22]<<8)+EpBuf[23];
    byte = (uint32)nSec<<9;//sec to byte

    #ifdef USB_IF_TEST
    if (OK != BulkOnlyChkCmd(0, byte))
        return;
    #endif

    Xfer->LBA=((uint32)EpBuf[17]<<8)+EpBuf[18];
    Xfer->LBA <<= 16;
    Xfer->LBA +=((uint32)EpBuf[19]<<8)+EpBuf[20];
    
    Xfer->NendLen = byte;            //sec to byte
    Xfer->TransLen = 0;

    Xfer->XferLen = 0;
    Xfer->BufLen = (Xfer->NendLen>USB_BUF_SIZE)? USB_BUF_SIZE : Xfer->NendLen;

    Xfer->XferCmd = USB_BULK_OUT;
    G_ucBulkPhase = K_OutDataPhase;

    #if (NAND_DRIVER == 1)
    if (MD_MAJOR(USBDiskID)==MEM_DEV_NAND)
    {
        uint16 len, offset, PageSize = FtlGetPageSize();
        USB_MSC_REQ * PendReq = &UsbMscPendReq;

        offset = Xfer->LBA & (PageSize-1);
        len = PageSize-offset;
        if (PendReq->cmd == K_SCSICMD_WRITE_10)
        {
            if(USBDiskID==PendReq->LUN && Xfer->LBA==(PendReq->LBA+PendReq->len))
            {
                Xfer->LBA = PendReq->LBA;
                Xfer->XferLen = PendReq->len<<9;
                Xfer->BufLen = ((nSec>len)? PageSize : PendReq->len+nSec)<<9;
                PendReq->cmd = 0;
                offset = 0;
            }
            else
            {
                MscReqAdd(K_RKCMD_CHECK_REQUEST, 0, 0, 0, NULL);    //����ִ�� Pend request, �ͷ�buf
                DISABLE_USB_INT;                        //���ж�, д���ڿ��ж�, ��֤buf������ȷ��
            }
        }

        /*Ҫд��������ʼ��ַ����page����, ��д������Ĳ��ֵ�����, 
        �������ݾͶ���page�����, �����������ٶ��Ż�*/
        if (offset && (nSec > len))
        {
            Xfer->BufLen = len<<9;
        }
    }
    #endif
}


/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_ModeSense06(void)
  Author        : chenfen
  Description   : ����:��ģʽ0x1a
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_ModeSense06(void)
{
    uint8* pSenseData;
    
    CSWHandler(CASEOK, 0x04);
    if (! MDChkValid(USBDiskID))
    {
        BuildSenseData(SCSI_SENSE_NOT_READY, SCSI_ADSENSE_NO_MEDIA_IN_DEVICE, 0);
        CSWHandler(CASECBW, 0);
        return;
    }

    pSenseData = (uint8*)((MD_MAJOR(USBDiskID) == MEM_DEV_SD)? SCSIDATA_Sense_SD : SCSIDATA_Sense_FLASH);
    UsbWriteEndpoint(BULK_IN_EP, 4, pSenseData);

    G_ucBulkPhase = K_InCSWPhase;
}

/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_TestUnitReady(void)
  Author        : chenfen
  Description   : ����:����׼��0x00
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_TestUnitReady(void)
{
    #ifdef USB_IF_TEST
    if (OK != BulkOnlyChkCmd(1, 0))
        return;
    #endif
    
    if (! MDChkValid(USBDiskID))
    {
        BuildSenseData(SCSI_SENSE_NOT_READY, SCSI_ADSENSE_NO_MEDIA_IN_DEVICE, 0);
        CSWHandler(CASE1,0);
        pCSW.bCSWStatus = CSW_FAIL;
    }
    else
    {
        uint8 lun = EpBuf[13];
        if (USBDisk.changed[lun])
        {
            USBDisk.changed[lun] = 0;
            BuildSenseData(SCSI_SENSE_UNIT_ATTENTION, SCSI_ADSENSE_MEDIUM_CHANGED, 0);
            CSWHandler(CASE1,0);
            pCSW.bCSWStatus = CSW_FAIL;
        }
        else
        {
            CSWHandler(CASE1,0);
            BuildSenseData(SCSI_SENSE_NO_SENSE, 0, 0);
        }
    }

    SendCSW();

}

/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_RequestSense(void)
  Author        : chenfen
  Description   : ����:����0x03
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_RequestSense(void)
{
    uint16 residue= MIN (sizeof(SCSIDATA_RequestSense), EpBuf[19]); 

    CSWHandler(CASE6,residue);
    if (0==EpBuf[19])
    {
        SendCSW();
        return;        
    }
  
    UsbWriteEndpoint(BULK_IN_EP, residue, SCSIDATA_RequestSense);

    G_ucBulkPhase = K_InCSWPhase;
}

/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_PreventAllowMediumRemoval(void)
  Author        : chenfen
  Description   : ����:��ֹ�����Ƴ�0x1e
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_PreventAllowMediumRemoval(void)
{
#if 1
    if ((EpBuf[19] & 0x01) == 0x01)
    {//���vista �� mac����U���˲����������⣬���ǻ����˳�U�̺���û�������״̬�ġ�
        //BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_INVALID_PARAMETER, SCSI_SENSEQ_INIT_COMMAND_REQUIRED);
        BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_INVALID_CDB, 0);
        CSWHandler(CASECMDFAIL,(uint16)dCBWDaTrLen);         //�����Ƴ��豸
		pCSW.bCSWStatus = CSW_FAIL; // comment by hwg, 07-06-30
    }
    else
    {
        BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
        CSWHandler(CASE1,(uint16)dCBWDaTrLen);          //�����Ƴ��豸
        //MscReQAdd(K_SCSICMD_PREVENT_ALLOW_MEDIUM_REMOVAL, USBDiskID, 0, 0, NULL);
    }
#endif

    SendCSW();   

}


/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_StartStopUnit(void)
  Author        : chenfen
  Description   : ����:��ͣ����0x1b
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_StartStopUnit(void)
{
#if 1
    if (0)//(EpBuf[19] & 0x01) == 0x01)
    {
        BuildSenseData(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ADSENSE_ILLEGAL_COMMAND, 0);
        CSWHandler(CASECMDFAIL,(uint16)dCBWDaTrLen);         //�����Ƴ��豸
    }
    else
    {
        BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
        CSWHandler(CASE1,(uint16)dCBWDaTrLen);          //�����Ƴ��豸
    }
#endif
    SendCSW();     
    //G_ucBulkPhase = K_CommandPhase;
    
    if(EpBuf[19] & 0x02)
    {
    	UsbConnected = 0;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_Verify10(void)
  Author        : chenfen
  Description   : ����:У��0x2f
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_Verify10(void)
{
    if (dCBWDaTrLen == 0)
    {
        CSWHandler(CASE1,0);
        BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
        SendCSW();
    }
    else
    {
        CSWHandler(CASE12,(uint16)dCBWDaTrLen);
        BuildSenseData(SCSI_SENSE_NO_SENSE,0,0);
        G_ucBulkPhase =K_OutDataPhase;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void SCSICMD_ModeSense10(void)
  Author        : chenfen
  Description   : ����:0x5a
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SCSICMD_ModeSense10(void)
{
    uint16 len;
    uint8* pSenseData;
    
    len = EpBuf[8]; //modify by lxs @2007.03.13, for Mac OS 10.3.4
    CSWHandler(CASEOK, len);

    pSenseData = (uint8*)((MD_MAJOR(USBDiskID) == MEM_DEV_SD)? SCSIDATA_Sense_SD : SCSIDATA_Sense_FLASH);

    UsbWriteEndpoint(BULK_IN_EP, len, pSenseData);

    G_ucBulkPhase = K_InCSWPhase;
}

/*
--------------------------------------------------------------------------------
  Function name : bool CBWValidVerify(void)
  Author        : chenfen
  Description   : 
  ����:�������ЧУ��
  ���:��
  ����:OK=������ʽ��ȷ,ERROR=������ʽ����
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
bool CBWValidVerify(void)
{
    USBDiskID = USBDisk.DiskID[EpBuf[13]];
    
    if (CBWLen==USB_BULK_CB_WRAP_LEN && EpBuf[0]==0x55 && EpBuf[1]==0x53 && EpBuf[2]==0x42 && EpBuf[3]==0x43    //CBW_SIGNATUREУ��
        && EpBuf[13] < USBDisk.LunNum && EpBuf[14] <= MAX_CDBLEN)                  //LUN������鳤��У��
    {
        return (OK);
        //return(ERROR);
    }
    else
    {
        G_ucBulkPhase = K_InCSWPhase;
        return(ERROR);  //��Ч����
    }
}

/*
--------------------------------------------------------------------------------
  Function name : CSWHandler
  Author        : chenfen
  Description   : 
  ����:CSW����
  ���:HostDevCase=״̬,DeviceTrDataLen=�豸Ҫ���͵����ݳ���
  ����:��
                
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void CSWHandler(uint8 HostDevCase,uint16 DeviceTrDataLen)
{
    uint8 i;

    pCSW.dCSWSignature[0]=0x55;
    pCSW.dCSWSignature[1]=0x53;
    pCSW.dCSWSignature[2]=0x42;
    pCSW.dCSWSignature[3]=0x53;
   
    for (i=0; i<=3; i++)    
    {
        pCSW.dCSWTag[i]=EpBuf[i+4];
    }

    pCSW.dCSWDataResidue[0]=(uint8)(dCBWDaTrLen-DeviceTrDataLen);
    pCSW.dCSWDataResidue[1]=(uint8)((dCBWDaTrLen-DeviceTrDataLen)>>8);
    pCSW.dCSWDataResidue[2]=0;
    pCSW.dCSWDataResidue[3]=0;

    switch(HostDevCase)
    {
        case CASEOK:
        case CASE1:     /* Hn=Dn*/
        case CASE6:     /* Hi=Di*/
        case CASE12:    /* Ho=Do*/
            pCSW.bCSWStatus = CSW_GOOD;
            break;

        case CASE4:     /* Hi>Dn*/
        case CASE5:     /* Hi>Di*/
            UsbSetEpSts(1, BULK_IN_EP, 1);
            pCSW.bCSWStatus = CSW_FAIL; //CSW_GOOD or CSW_FAIL
            break;

        case CASE2:     /* Hn<Di*/
        case CASE3:     /* Hn<Do*/
        case CASE7:     /* Hi<Di*/
        case CASE8:     /* Hi<>Do */
            //MUSBHSFC_SetEndpointStatus(3, 1);       //stall IN endpoint
            UsbSetEpSts(1, BULK_IN_EP, 1);
            pCSW.bCSWStatus = CSW_PHASE_ERROR;
            break;
            
        case CASE9:     /* Ho>Dn*/
        case CASE11:    /* Ho>Do*/
            UsbSetEpSts(0, BULK_OUT_EP, 1);
            pCSW.bCSWStatus = CSW_FAIL;
            break;
        case CASE10:    /* Ho<>Di */    
        case CASE13:    /* Ho<Do*/
            //MUSBHSFC_SetEndpointStatus(2, 1);
            UsbSetEpSts(0, BULK_OUT_EP, 1);
            pCSW.bCSWStatus = CSW_PHASE_ERROR;
            break;
        case CASECMDFAIL:
            pCSW.bCSWStatus = CSW_FAIL;
            break;
        case CASECBW:   /* invalid CBW */
            //MUSBHSFC_SetEndpointStatus(3, 1);
            UsbSetEpSts(1, BULK_IN_EP, 1);
            if (dCBWDaTrLen && !(EpBuf[12]&0x80))
                UsbSetEpSts(0, BULK_OUT_EP, 1);
            pCSW.bCSWStatus = CSW_FAIL;
            G_ucBulkPhase = K_InCSWPhase;
            break;

        default:
            break;
    }
}


/*
--------------------------------------------------------------------------------
  Function name : void SendCSW(void)
  Author        : chenfen
  Description   : 
  ����:����CSW
           
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void SendCSW(void)
{
    UsbWriteEndpoint(BULK_IN_EP, 13, (uint8*)&pCSW);

    G_ucBulkPhase = K_CommandPhase;  //add by lxs
}

/*
--------------------------------------------------------------------------------
  Function name : BuildSenseData
  Author        : chenfen
  Description   : 
  ����:���� sense
           
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void BuildSenseData(uint8 SenseKey, uint8 ASC, uint8 ASCQ)
{
    SCSIDATA_RequestSense[0]=0x70;      //information filed is invalid=0x70,information filed is valid=0xf0,
    SCSIDATA_RequestSense[1]=0x00;      //reserve
    SCSIDATA_RequestSense[2]=SenseKey;
    
    SCSIDATA_RequestSense[3]=0x00;      //information
    SCSIDATA_RequestSense[4]=0x00;
    SCSIDATA_RequestSense[5]=0x00;
    SCSIDATA_RequestSense[6]=0x00;
    
    SCSIDATA_RequestSense[7]=0x0a;      //Addition Sense Length
    
    SCSIDATA_RequestSense[8]=0x00;      //reserve
    SCSIDATA_RequestSense[9]=0x00;
    SCSIDATA_RequestSense[10]=0x00;
    SCSIDATA_RequestSense[11]=0x00;
    
    SCSIDATA_RequestSense[12]=ASC;
    SCSIDATA_RequestSense[13]=ASCQ;
    
    SCSIDATA_RequestSense[14]=0x00;     //reserve
    SCSIDATA_RequestSense[15]=0x00;
    SCSIDATA_RequestSense[16]=0x00;
    SCSIDATA_RequestSense[17]=0x00;
}

#if 0
/*
--------------------------------------------------------------------------------
  Function name : USBFlashDiskProtectON
  Author        : chenfen
  Description   : 
  ����:USBFileWriteProtect
           
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void USBFlashDiskProtectON(void)
{
//	if (FileSysSetup(0) == OK)  	//����д��������
		SCSIDATA_Sense_FLASH[2]|=0x80;//����д��������
}

/*
--------------------------------------------------------------------------------
  Function name : USBFlashDiskProtectOFF
  Author        : chenfen
  Description   : 
  ����:USBFileWriteUnProtect
           
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void USBFlashDiskProtectOFF(void)
{
//	if (FileSysSetup(0) == OK)  	//�ر�д��������
		SCSIDATA_Sense_FLASH[2]&=0x7f;//�ر�д��������
}

/*
--------------------------------------------------------------------------------
  Function name : USBSDDiskProtectON
  Author        : chenfen
  Description   : 
  ����:USBFileWriteProtect
           
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void USBSDDiskProtectON(void)
{
//	if (FileSysSetup(1) == OK)  	//����д��������
		SCSIDATA_Sense_SD[2]|=0x80;//����д��������
}

/*
--------------------------------------------------------------------------------
  Function name : USBSDDiskProtectOFF
  Author        : chenfen
  Description   : 
  ����:USBFileWriteUnProtect
           
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2009/02/13         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void USBSDDiskProtectOFF(void)
{
//	if (FileSysSetup(1) == OK)  	//�ر�д��������
		SCSIDATA_Sense_SD[2]&=0x7f;//�ر�д��������
}
#endif

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

