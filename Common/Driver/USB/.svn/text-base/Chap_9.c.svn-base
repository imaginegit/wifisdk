/*
********************************************************************************
*                   Copyright (c) 2009,chenfen
*                         All rights reserved.
*
* File Name：   chap_9.c
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             chenfen      2009-2-13          1.0
*    desc:    ORG.
********************************************************************************
*/

#include "USBConfig.h"

#define _IN_CHAP_

#ifdef USB 

/*
--------------------------------------------------------------------------------
  Function name : void reserved(void)
  Author        : chenfen
  Description   : 
                  USB Protocol Layer
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void reserved(void)
{
//  stall_ep0();
}

/*
--------------------------------------------------------------------------------
  Function name : get_status
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : TRUE

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void get_status(void)
{
    uint8 bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;
    uint16 status;

    if (bRecipient == USB_RECIPIENT_ENDPOINT)
    {
 
        uint8 dir = ControlData.DeviceRequest.wIndex & (uint8)USB_ENDPOINT_DIRECTION_MASK; 
        uint8 EpNum = (uint8)(ControlData.DeviceRequest.wIndex & MAX_ENDPOINTS);
    
        status = UsbGetEpSts(dir , EpNum);
        UsbWriteEp0(2, &status);
    }
    else if (bRecipient == USB_RECIPIENT_DEVICE)
    {
        status = (uint16)(bEPPflags.bits.remote_wakeup<<1);
        UsbWriteEp0(2, &status);
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void set_feature(void)
  Author        : chenfen
  Description   : 
                  特性清除
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void set_feature(void)
{
    uint32 reg;
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;
    uint8 endp,dir;
    uint8 bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;

    ep0in_ack();
    if (bRecipient == USB_RECIPIENT_DEVICE)
    {
        if (ControlData.DeviceRequest.wValue == USB_FEATURE_REMOTE_WAKEUP)
        {
            bEPPflags.bits.remote_wakeup = 1;
        }
        else if (ControlData.DeviceRequest.wValue == USB_FEATURE_TEST_MODE)
        {
            switch (ControlData.DeviceRequest.wIndex>>8)
            {
                reg = dev_regs->dctl & (~(0x07<<4));
                case 0x01:  //test J
                case 0x02:  //test K
                case 0x03:  //test SE0
                case 0x04:  //test packet
                case 0x05:  //Test Force Enable
                    Delay100cyc(12);        //延时10us @100MHz
                    dev_regs->dctl=(dev_regs->dctl & (~(0x07<<4))) | (((ControlData.DeviceRequest.wIndex>>8)%8)<<4);
                    break;
                default:
                    break;
            }
        }
    }
    else if (bRecipient == USB_RECIPIENT_ENDPOINT       //清除端点stall
        && ControlData.DeviceRequest.wValue == USB_FEATURE_ENDPOINT_STALL)
    {
        endp = (uint8)(ControlData.DeviceRequest.wIndex & MAX_ENDPOINTS);
        dir = ControlData.DeviceRequest.wIndex & (uint8)USB_ENDPOINT_DIRECTION_MASK;
        UsbSetEpSts(dir, endp, 1); 
    }

}

/*
--------------------------------------------------------------------------------
  Function name : void clear_feature(void)
  Author        : chenfen
  Description   : 
                  特性清除
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void clear_feature(void)
{
    uint8 endp,dir;
    uint8 bRecipient = ControlData.DeviceRequest.bmRequestType & USB_RECIPIENT;

    if (bRecipient == USB_RECIPIENT_DEVICE              //清除远程唤醒功能
        && ControlData.DeviceRequest.wValue == USB_FEATURE_REMOTE_WAKEUP) 
    {
        bEPPflags.bits.remote_wakeup = 0;
        ep0in_ack();
    }
    else if (bRecipient == USB_RECIPIENT_ENDPOINT       //清除端点stall
        && ControlData.DeviceRequest.wValue == USB_FEATURE_ENDPOINT_STALL) 
    {
        endp = (uint8)(ControlData.DeviceRequest.wIndex & MAX_ENDPOINTS);
        dir = ControlData.DeviceRequest.wIndex & (uint8)USB_ENDPOINT_DIRECTION_MASK;
        UsbSetEpSts(dir, endp, 0);     // clear RX/TX stall for OUT/IN on EPn.
        ep0in_ack();
        if (endp > 0)
        {
            if ( dir > 0)
                ResetInEpDataToggle(endp);
            else
                ResetOutEpDataToggle(endp);

            if (G_ucBulkPhase == K_InCSWPhase)
                SendCSW();
        }
    }
    else
        stall_ep0();
}

/*
--------------------------------------------------------------------------------
  Function name : void set_address(void)
  Author        : chenfen
  Description   : 
                  设置地址
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void set_address(void)
{
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;

    dev_regs->dcfg = (dev_regs->dcfg & (~0x07f0)) | (ControlData.DeviceRequest.wValue << 4);  //reset device addr
    ep0in_ack();
}

//uint8 IsUsbCopyDev = 0;
/*
--------------------------------------------------------------------------------
  Function name : void get_descriptor(void)
  Author        : chenfen
  Description   : 
                  获取描述符
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void get_descriptor(void)
{
    uint8 length;
    uint8 bDescriptor = (ControlData.DeviceRequest.wValue >> 8) & 0xff;

    switch (bDescriptor)
    {
        case USB_DEVICE_DESCRIPTOR_TYPE:                //获取设备描述符

            UsbConnected=1;     //枚举完,已连接上

        //获取设备描述符
            length = sizeof(USB_DEVICE_DESCRIPTOR);

            if (length >= ControlData.DeviceRequest.wLength)
                length = ControlData.DeviceRequest.wLength;

            ControlData.wLength = length;
      
            if (bEPPflags.bits.HS_FS_State == FULL_SPEED)
                code_transmit((uint8 *)&DeviceDescr, length);
            else
                code_transmit((uint8 *)&HSDeviceDescr, length);
            break;
            
        case USB_CONFIGURATION_DESCRIPTOR_TYPE:         //获取配置描述符
            length=0x09;
            if (ControlData.DeviceRequest.wLength > CONFIG_DESCRIPTOR_LENGTH)
                length = CONFIG_DESCRIPTOR_LENGTH;
            else
                length = ControlData.DeviceRequest.wLength;

             ControlData.wLength = length;

            if (bEPPflags.bits.HS_FS_State == FULL_SPEED)
            {
                code_transmit((uint8 *)&FSConfigDescrs, length);//ControlData.DeviceRequest.wLength);
            }
            else
            {
                code_transmit((uint8 *)&HSConfigDescrs, length);//ControlData.DeviceRequest.wLength);
            }
            break;
            
        case USB_STRING_DESCRIPTOR_TYPE:                //获取字符串描述符
            switch (ControlData.DeviceRequest.wValue & 0x00ff)
            {
                case 0:
                    length=0x04;
                    if (length >= ControlData.DeviceRequest.wLength)
                        length = ControlData.DeviceRequest.wLength;
                    code_transmit((uint8 *)&LangId, length);
                    break;
                #if iProduct > 0
                case iProduct:
                    length=0x12;
                    if (length >= ControlData.DeviceRequest.wLength)
                        length = ControlData.DeviceRequest.wLength;
                    code_transmit((uint8 *)&StringProduct, length);
                    break;
                #endif
                
                #if iSerialNumber > 0
                case iSerialNumber:
                   length=0x12;
                    if (length >= ControlData.DeviceRequest.wLength)
                        length = ControlData.DeviceRequest.wLength;
                    
                    //length = ControlData.DeviceRequest.wLength;
                    ControlData.wLength = length;
                    code_transmit((uint8 *)&StringSerialNumbel, length);
                    break;
                #endif
                
                #if iConfiguration > 0
                case iConfiguration:
                    code_transmit((uint8 *)&StringConfig, 18);
                    break;
                #endif
                
                
                #if iInterface > 0
                case iInterface:
                    code_transmit((uint8 *)&StringInterface, 18);
                    break;
                #endif

                #if iManufactuer > 0
                case iManufactuer:
                #endif
                default:
                    length=0x12;
                    if (length >= ControlData.DeviceRequest.wLength)
                        length = ControlData.DeviceRequest.wLength;
                    code_transmit((uint8 *)&StringManufacture, length);
                    break;
            }
            break;
            
        case USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE:
            length=0x0A;
            if (length >= ControlData.DeviceRequest.wLength)
                length = ControlData.DeviceRequest.wLength;
            code_transmit((uint8 *)&HS_Device_Qualifier, length);
            break;
            
        case USB_OTHER_SPEED_CONF_DESCRIPTOR_TYPE:
            if (bEPPflags.bits.HS_FS_State == FULL_SPEED)
            {
                stall_ep0();
            }
            else
            {
                length=0x09;
                if (ControlData.DeviceRequest.wLength > CONFIG_DESCRIPTOR_LENGTH)
                    length = CONFIG_DESCRIPTOR_LENGTH;
                else
                    length = ControlData.DeviceRequest.wLength;

                ControlData.wLength = length;
                //code_transmit((uint8 *)&Other_Speed_Config_Descriptor, length);
                memcpy(EpBuf, (uint8*)&FSConfigDescrs, length);
                EpBuf[1] = 0x07;									//Other speed configuration Type	
                code_transmit(EpBuf, length);
            }
            break;
            
        default:
            stall_ep0();
            break;
    }
}

/*
--------------------------------------------------------------------------------
  Function name : void get_configuration(void)
  Author        : chenfen
  Description   : 
                  获取配置状态
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void get_configuration(void)
{
    uint8 config = bEPPflags.bits.configuration;
    UsbWriteEp0(1, &config);
}

/*
--------------------------------------------------------------------------------
  Function name : void set_configuration(void)
  Author        : chenfen
  Description   : 
                  设置配置状态
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void set_configuration(void)
{
    bEPPflags.bits.configuration = ControlData.DeviceRequest.wValue & 0x1;
    ResetInEpDataToggle(BULK_IN_EP);
    ResetOutEpDataToggle(BULK_OUT_EP);
    ep0in_ack();
}


/*
--------------------------------------------------------------------------------
  Function name : void set_interface(void)
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void set_interface(void)
{
    bEPPflags.bits.Alter_Interface = ControlData.DeviceRequest.wValue & 0x1;
    //ResetInEpDataToggle(BULK_IN_EP);
    //ResetOutEpDataToggle(BULK_OUT_EP);
    ep0in_ack();
}


/*
--------------------------------------------------------------------------------
  Function name : void set_interface(void)
  Author        : chenfen
  Description   : 
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void get_interface(void)
{
    uint8 intf = bEPPflags.bits.Alter_Interface;
    UsbWriteEp0(1, &intf);
}

/*
--------------------------------------------------------------------------------
  Function name : void stall_ep0(void)
  Author        : chenfen
  Description   : 返回stall应答
                  
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/07/21         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void stall_ep0(void)
{
	//write_XDATA(USB_CSR0, CSR0_OUT_SEVD | CSR0_SEND_STALL); 	//send stall
    otg_dev_regs_t *dev_regs = (otg_dev_regs_t *)USB_DEV_BASE;  

    dev_regs->out_ep[0].doepctl |= 1<<21;  //send OUT0 stall handshack
    dev_regs->in_ep[0].diepctl |= 1<<21;   //send IN0 stall handshack
}


/*
--------------------------------------------------------------------------------
  Function name : void ep0in_ack(void)
  Author        : chenfen
  Description   : 
                  端点0 IN应答
  Input         : 
  Return        : 

  History:     <author>         <time>         <version>       
             chenfen     2008/12/12         Ver1.0
  desc:         ORG
--------------------------------------------------------------------------------
*/
IRAM_USB
void ep0in_ack(void)
{
    UsbWriteEp0(0, NULL);
}
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

