/*
********************************************************************************
*                   Copyright (c) 2008,ZhengYongzhi
*                         All rights reserved.
*
* File Name：   USB20.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             ZhengYongzhi      2008-9-13          1.0
*    desc:    ORG.
********************************************************************************
*/


#ifndef _USB20_H
#define _USB20_H

#include "global.h"

/*
*-------------------------------------------------------------------------------
*  
*                           Macro define
*  
*-------------------------------------------------------------------------------
*/
#define     USB_PERMIT_ENABLE          1
#define     USB_PERMIT_DISABLE         0

#define     ENABLE_USB_INT       *((volatile unsigned long*)(0xE000E100)) = 0x00000080   
#define     DISABLE_USB_INT      *((volatile unsigned long*)(0xE000E180)) = 0x00000080

#define         BULK_IN_EP          0x01
#define         BULK_OUT_EP         0x01

//1可配置参数
#if 1//MAX_LUN>1
    #define 		iManufactuer		1		/*厂商描述符字符串索引*/
    #define 		iProduct			2		/*产品描述符字符串索引*/
    #define 		iSerialNumber		3		/*设备序列号字符串索引*/
    #define 		iConfiguration		0// 4		/*配置描述符字符串索引*/
    #define 		iInterface			0// 5		/*接口描述符字符串索引*/
#else
    #define 		iManufactuer		0		/*厂商描述符字符串索引*/
    #define 		iProduct			0		/*产品描述符字符串索引*/
    #define 		iSerialNumber		0		/*设备序列号字符串索引*/
    #define 		iConfiguration		0// 3		/*配置描述符字符串索引*/
    #define 		iInterface			0// 4		/*接口描述符字符串索引*/
#endif

#ifndef 		FALSE
#define 		FALSE  	 		0
#endif

#ifndef 		TRUE
#define 		TRUE    			(!FALSE)
#endif

#define 		NUM_ENDPOINTS		2		//端点数除0外

//TestMode
#define 		TEST_FS					    (0x20 << 8)
#define 		TEST_HS					    (0x10 << 8)
#define 		TEST_PACKET				    (0x08 << 8)
#define 		TEST_K						(0x04 << 8)
#define 		TEST_J						(0x02 << 8)
#define 		TEST_NAK					(0x01 << 8)


//mainloop.h
#define 		MAX_ENDPOINTS      			(uint8)0x3
#define         EP0_PACKET_SIZE_FS          8
#define         EP0_PACKET_SIZE_HS          64
#define 		EP0_TX_FIFO_SIZE   			64
#define 		EP0_RX_FIFO_SIZE   			64
#define 		EP0_PACKET_SIZE    			64

#define 		FS_BULK_RX_SIZE    			64
#define 		FS_BULK_TX_SIZE    			64
#define 		HS_BULK_RX_SIZE    			512
#define 		HS_BULK_TX_SIZE    			512


#define 		USB_IDLE           		    0
#define 		USB_TRANSMIT       			1
#define 		USB_RECEIVE        			2

#define 		USB_CLASS_CODE_TEST_CLASS_DEVICE		0xdc
#define 		USB_SUBCLASS_CODE_TEST_CLASS_D12		0xa0
#define 		USB_PROTOCOL_CODE_TEST_CLASS_D12		0xb0


#define 		USB_RECIPIENT            		(uint8)0x1F
#define 		USB_RECIPIENT_DEVICE     	    (uint8)0x00
#define 		USB_RECIPIENT_INTERFACE  	    (uint8)0x01
#define 		USB_RECIPIENT_ENDPOINT   	    (uint8)0x02

#define 		USB_REQUEST_TYPE_MASK    	    (uint8)0x60
#define 		USB_STANDARD_REQUEST     	    (uint8)0x00
#define 		USB_CLASS_REQUEST        	    (uint8)0x20
#define 		USB_VENDOR_REQUEST       	    (uint8)0x40

#define 		USB_REQUEST_MASK         	    (uint8)0x0F
#define 		DEVICE_ADDRESS_MASK      	    0x7F

#define 		IN_TOKEN_DMA 				1
#define 		OUT_TOKEN_DMA 			    0

#define 		DMA_BUFFER_SIZE			    256

#define 		DMA_IDLE					0
#define 		DMA_RUNNING				    1
#define 		DMA_PENDING				    2

//厂商请求代码
#define 		SETUP_DMA_REQUEST 		    0x0471
#define 		GET_FIRMWARE_VERSION    	0x0472
#define 		GET_SET_TWAIN_REQUEST   	0x0473
#define 		GET_BUFFER_SIZE		    	0x0474


//usb100.h
#define 		MAXIMUM_USB_STRING_LENGTH 				255

// values for the bits returned by the USB GET_STATUS command
#define 		USB_GETSTATUS_SELF_POWERED                	    0x01
#define 		USB_GETSTATUS_REMOTE_WAKEUP_ENABLED	            0x02


#define 		USB_DEVICE_DESCRIPTOR_TYPE                		0x01
#define 		USB_CONFIGURATION_DESCRIPTOR_TYPE         	    0x02
#define 		USB_STRING_DESCRIPTOR_TYPE                		0x03
#define 		USB_INTERFACE_DESCRIPTOR_TYPE             	    0x04
#define 		USB_ENDPOINT_DESCRIPTOR_TYPE              	    0x05
#define 		USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE 	        0x06
#define 		USB_OTHER_SPEED_CONF_DESCRIPTOR_TYPE 	        0x07
#define 		USB_INTERFACE_POWER_DESCRIPTOR_TYPE 	        0x08
#define 		USB_OTG_DESCRIPTOR_TYPE 					    0x09
#define 		USB_DEBUG_DESCRIPTOR_TYPE 				        0x0A
#define 		USB_IF_ASSOCIATION_DESCRIPTOR_TYPE 		        0x0B

#define 		USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(d, i) ((uint16)((uint16)d<<8 | i))

// Values for bmAttributes field of an
// endpoint descriptor

#define 		USB_ENDPOINT_TYPE_MASK                    		0x03
#define 		USB_ENDPOINT_TYPE_CONTROL                 		0x00
#define 		USB_ENDPOINT_TYPE_ISOCHRONOUS             	    0x01
#define 		USB_ENDPOINT_TYPE_BULK                    		0x02
#define 		USB_ENDPOINT_TYPE_INTERRUPT               		0x03

// definitions for bits in the bmAttributes field of a 
// configuration descriptor.
#define 		USB_CONFIG_POWERED_MASK                   		0xc0
#define 		USB_CONFIG_BUS_POWERED                    		0x80
#define 		USB_CONFIG_SELF_POWERED                   		0x40
#define 		USB_CONFIG_REMOTE_WAKEUP                  		0x20

// Endpoint direction bit, stored in address
#define 		USB_ENDPOINT_DIRECTION_MASK               	    0x80

// test direction bit in the bEndpointAddress field of
// an endpoint descriptor.
#define 		USB_ENDPOINT_DIRECTION_OUT(addr)          	(!((addr) & USB_ENDPOINT_DIRECTION_MASK))
#define 		USB_ENDPOINT_DIRECTION_IN(addr)           	((addr) & USB_ENDPOINT_DIRECTION_MASK)

// USB defined request codes
#define 		USB_REQUEST_GET_STATUS                    		0x00
#define 		USB_REQUEST_CLEAR_FEATURE                 		0x01
#define 		USB_REQUEST_SET_FEATURE                   		0x03
#define 		USB_REQUEST_SET_ADDRESS                   		0x05
#define 		USB_REQUEST_GET_DESCRIPTOR                		0x06
#define 		USB_REQUEST_SET_DESCRIPTOR                		0x07
#define 		USB_REQUEST_GET_CONFIGURATION             	    0x08
#define 		USB_REQUEST_SET_CONFIGURATION             	    0x09
#define 		USB_REQUEST_GET_INTERFACE                 		0x0A
#define 		USB_REQUEST_SET_INTERFACE                 		0x0B
#define 		USB_REQUEST_SYNC_FRAME                    		0x0C


// defined USB device classes
#define 		USB_DEVICE_CLASS_RESERVED           			0x00
#define 		USB_DEVICE_CLASS_AUDIO              			0x01	//音频设备
#define 		USB_DEVICE_CLASS_COMMUNICATIONS     		    0x02	//通讯设备
#define 		USB_DEVICE_CLASS_HUMAN_INTERFACE    		    0x03	//人机接口
#define 		USB_DEVICE_CLASS_MONITOR            			0x04	//显示器
#define 		USB_DEVICE_CLASS_PHYSICAL_INTERFACE 	        0x05	//物理接口
#define 		USB_DEVICE_CLASS_POWER              			0x06	//电源
#define 		USB_DEVICE_CLASS_PRINTER            			0x07	//打印机
#define 		USB_DEVICE_CLASS_STORAGE            			0x08	//海量存储
#define 		USB_DEVICE_CLASS_HUB                			0x09	//HUB
#define 		USB_DEVICE_CLASS_VENDOR_SPECIFIC    		    0xFF	//

//define	USB subclass
#define		USB_SUBCLASS_CODE_RBC					            0x01
#define		USB_SUBCLASS_CODE_SFF8020I				            0x02
#define		USB_SUBCLASS_CODE_QIC157					        0x03
#define		USB_SUBCLASS_CODE_UFI					            0x04
#define		USB_SUBCLASS_CODE_SFF8070I				            0x05
#define		USB_SUBCLASS_CODE_SCSI					            0x06

//define	USB protocol
#define		USB_PROTOCOL_CODE_CBI0					            0x00
#define		USB_PROTOCOL_CODE_CBI1					            0x01
#define		USB_PROTOCOL_CODE_BULK					            0x50


// USB defined Feature selectors
#define 		USB_FEATURE_ENDPOINT_STALL          		    0x0000
#define 		USB_FEATURE_REMOTE_WAKEUP           		    0x0001
#define 		USB_FEATURE_TEST_MODE                			0x0002
#define 		USB_FEATURE_POWER_D1                			0x0003
#define 		USB_FEATURE_POWER_D2                			0x0004
#define 		USB_FEATURE_POWER_D3                			0x0005

//values for bmAttributes Field in USB_CONFIGURATION_DESCRIPTOR
#define 		BUS_POWERED                           				0x80
#define 		SELF_POWERED                          				0x40
#define 		REMOTE_WAKEUP                         				0x20

// USB power descriptor added to core specification
#define 		USB_SUPPORT_D0_COMMAND      				0x01
#define 		USB_SUPPORT_D1_COMMAND      				0x02
#define 		USB_SUPPORT_D2_COMMAND      				0x04
#define 		USB_SUPPORT_D3_COMMAND      				0x08
#define 		USB_SUPPORT_D1_WAKEUP       				0x10
#define 		USB_SUPPORT_D2_WAKEUP       				0x20

#define         USB_BULK_CB_WRAP_LEN	31

/*
*-------------------------------------------------------------------------------
*  
*                           Struct define
*  
*-------------------------------------------------------------------------------
*/
//1结构定义
//设备描述符结构
typedef __packed struct _USB_DEVICE_DESCRIPTOR 
{
    uint8		bLength;
    uint8		bDescriptorType;
    uint16 		bcdUSB;
    uint8 		bDeviceClass;
    uint8 		bDeviceSubClass;
    uint8 		bDeviceProtocol;
    uint8 		bMaxPacketSize0;
    uint16 		idVendor;
    uint16 		idProduct;
    uint16 		bcdDevice;
    uint8 		iiManufacturer;
    uint8 		iiProduct;
    uint8 		iiSerialNumber;
    uint8 		bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

//端点描述符结构
typedef __packed struct _USB_ENDPOINT_DESCRIPTOR 
{
    uint8 		bLength;
    uint8 		bDescriptorType;
    uint8 		bEndpointAddress;
    uint8 		bmAttributes;	
    uint16 		wMaxPacketSize;
    uint8 		bInterval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;


//配置描述符结构
typedef __packed struct _USB_CONFIGURATION_DESCRIPTOR 
{
    uint8 		bLength;
    uint8 		bDescriptorType;
    uint16		wTotalLength;
    uint8 		bNumInterfaces;
    uint8 		bConfigurationValue;
    uint8 		iiConfiguration;
    uint8 		bmAttributes;
    uint8 		MaxPower;
} USB_CONFIGURATION_DESCRIPTOR, *PUSB_CONFIGURATION_DESCRIPTOR;

//高速设备的其它速度配置描述符
typedef __packed struct _OTHER_SPEED_CONFIG_DESCRIPTOR
{
    uint8 		bLength;
    uint8 		bDescriptorType;	
    uint16		wTotalLength;
    uint8 		bNumInterfaces;
    uint8 		bConfigurationValue;
    uint8 		iiConfiguration;
    uint8 		bmAttributes;
    uint8 		MaxPower;
}OTHER_SPEED_CONFIG_DESCRIPTOR;


//接口描述符结构
typedef __packed struct _USB_INTERFACE_DESCRIPTOR 
{
    uint8 		bLength;
    uint8 		bDescriptorType;
    uint8 		bInterfaceNumber;
    uint8 		bAlternateSetting;
    uint8 		bNumEndpoints;
    uint8 		bInterfaceClass;
    uint8 		bInterfaceSubClass;
    uint8 		bInterfaceProtocol;
    uint8 		iiInterface;
} USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;

//配置描述符集合描述符结构
typedef __packed struct _USB_CONFIGS_DESCRIPTOR 
{
    USB_CONFIGURATION_DESCRIPTOR Config;
    USB_INTERFACE_DESCRIPTOR Interface;
    USB_ENDPOINT_DESCRIPTOR BulkIn;
    USB_ENDPOINT_DESCRIPTOR BulkOut;
} USB_CONFIGS_DESCRIPTOR;


//字符串描述符结构
typedef __packed struct _USB_STRING_DESCRIPTOR 
{
    uint8 		bLength;
    uint8 		bDescriptorType;
    uint8 		bString[1];
} USB_STRING_DESCRIPTOR, *PUSB_STRING_DESCRIPTOR;


//高速设备限制描述符
typedef __packed struct _HS_DEVICE_QUALIFIER
{
	uint8	bLength;			//length of HS Device Descriptor
	uint8	bQualifier; 			//HS Device Qualifier Type	
	uint16	wVersion;			// USB 2.0 version
	uint8	bDeviceClass;		//Device class
	uint8	bDeviceSubClasss;	//Device SubClass
	uint8	bProtocol;			//Device Protocol Code
	uint8	MaxPktSize;			//Maximum Packet SIze for other speed
	uint8	bOther_Config;		//Number of Other speed configurations
	uint8	Reserved;			//Reserved
}HS_DEVICE_QUALIFIER;

//电源描述符结构
typedef __packed struct _USB_POWER_DESCRIPTOR
{
    uint8 		bLength;
    uint8 		bDescriptorType;
    uint8 		bCapabilitiesFlags;
    uint16 		EventNotification;
    uint16 		D1LatencyTime;
    uint16 		D2LatencyTime;
    uint16		D3LatencyTime;
    uint8 		PowerUnit;
    uint16 		D0PowerConsumption;
    uint16 		D1PowerConsumption;
    uint16 		D2PowerConsumption;
} USB_POWER_DESCRIPTOR, *PUSB_POWER_DESCRIPTOR;


//通用描述符结构
typedef struct _USB_COMMON_DESCRIPTOR 
{
    uint8 		bLength;
    uint8 		bDescriptorType;
} USB_COMMON_DESCRIPTOR, *PUSB_COMMON_DESCRIPTOR;


//标准HUB描述符结构
typedef struct _USB_HUB_DESCRIPTOR 
{
    uint8        	bDescriptorLength;      	// Length of this descriptor
    uint8        	bDescriptorType;        	// Hub configuration type
    uint8        	bNumberOfPorts;         	// number of ports on this hub
    uint16	    	wHubCharacteristics;    	// Hub Charateristics
    uint8       	bPowerOnToPowerGood;  // port power on till power good in 2ms
    uint8       	bHubControlCurrent;     	// max current in mA
    // room for 255 ports power control and removable bitmask
    uint8        	bRemoveAndPowerMask[64];
} USB_HUB_DESCRIPTOR, *PUSB_HUB_DESCRIPTOR;


#define		FULL_SPEED		0
#define		HIGH_SPEED		1
//事件组标志结构
typedef union _epp_flags
{
	struct _flags
	{
		uint8 HS_FS_State	:1;
		uint8 bus_reset           :1;
		uint8 suspend             	:1;
		uint8 setup_packet  	:1;
		uint8 remote_wakeup	:1;
		uint8 Alter_Interface	:1;
		uint8 control_state	:2;

		uint8 configuration	:1;
		uint8 WdtUsbEn		:1;
		uint8 ep1_rxdone		:1;
		uint8 setup_dma		:2;	 // V2.3
		uint8 dma_state		:2;
		uint8 power_down	:1;	 // Smart Board
	} bits;
	uint16 value;
} EPPFLAGS;

//设备请求结构
typedef struct _device_request
{
	uint8 	bmRequestType;
	uint8 	bRequest;	
	uint16	wValue;
	uint16	wIndex;
	uint16	wLength;
} DEVICE_REQUEST;

//I/O请求结构(用于DMA)
typedef struct _IO_REQUEST 
{
	uint16	uAddressL;
	uint8	bAddressH;
	uint16	uSize;
	uint8	bCommand;
} IO_REQUEST, *PIO_REQUEST;


#define MAX_CONTROLDATA_SIZE	8		//控制管道最大包长
//带数据的设备请求结构
typedef struct _control_xfer
{
	DEVICE_REQUEST DeviceRequest;	
	uint16 	wLength;
	uint16 	wCount;
	uint8 	*pData;
	//uint8 	dataBuffer[MAX_CONTROLDATA_SIZE];
} CONTROL_XFER;


typedef struct _TWAIN_FILEINFO 
{
	uint8	bPage;		// bPage bit 7 - 5 map to uSize bit 18 - 16
	uint8	uSizeH;		// uSize bit 15 - 8
	uint8	uSizeL;		// uSize bit 7 - 0
} TWAIN_FILEINFO, *PTWAIN_FILEINFO;


#define 		CONFIG_DESCRIPTOR_LENGTH    sizeof(USB_CONFIGURATION_DESCRIPTOR) \
											+ sizeof(USB_INTERFACE_DESCRIPTOR) \
											+ (NUM_ENDPOINTS * sizeof(USB_ENDPOINT_DESCRIPTOR))


/*
*-------------------------------------------------------------------------------
*  
*                           Variable define
*  
*-------------------------------------------------------------------------------
*/

//1全局变量
#undef	EXT
#ifdef	IN_USB_ISR
		#define	EXT
#else
		#define	EXT		extern
#endif		
		
#ifdef USB
DRAM_USB EXT		CONTROL_XFER 	        ControlData;
DRAM_USB EXT		EPPFLAGS 		        bEPPflags;
DRAM_USB EXT		uint8                   Ep0PktSize;
DRAM_USB EXT		uint16 			        BulkEndpInSize;
DRAM_USB EXT		uint16 			        BulkEndpOutSize;
DRAM_USB EXT		uint16 			        CBWLen;
DRAM_USB EXT		__align(4) uint8        EpBuf[64];
_ATTR_SYS_BSS_   EXT		bool 	volatile	    UsbConnected;

DRAM_USB EXT		bool 	volatile	    UsbUpdate;
//DRAM_USB EXT		bool 	volatile	    UsbHsEn;


#ifdef USB_PERMIT
DRAM_USB EXT		uint8 		            USBPermit;
#endif
//DRAM_USB EXT		__align(8) uint8        Ep0Buf[64];

#endif

/*
--------------------------------------------------------------------------------
  
   Functon Declaration 
  
--------------------------------------------------------------------------------
*/

//1函数原型声明
//mainloop.c

//usbCtrl.c
extern	void 	USBIdle(void);
extern	void    USBInit(bool ReConnecte);
//extern	void 	USBInit(void);
extern	void 	USBDisconnect(void);
extern	void 	USBConnect(bool FullSpeed);
extern	void 	USBReconnect_(void);
extern	void 	code_transmit(uint8 * pRomData, uint16 len);
extern	void 	control_handler(void);

//chap_9.c
extern	void 	get_status(void);
extern	void 	clear_feature(void);
extern	void 	set_feature(void);
extern	void 	set_address(void);
extern	void 	get_descriptor(void);
extern	void 	get_configuration(void);
extern	void 	set_configuration(void);
extern	void 	get_interface(void);
extern	void 	set_interface(void);
extern	void 	reserved(void);
extern	void 	stall_ep0(void);
extern	void 	ep0in_ack(void);

//d12ci.c
extern  void    ResetInEpDataToggle(uint8 ep);
extern  void    ResetOutEpDataToggle(uint8 ep);
extern	void 	UsbSetEpSts(uint8 dir, uint8 EpNum, uint8 status);
extern	uint8 	UsbGetEpSts(uint8 dir, uint8 EpNum);

extern  void    UsbWriteEp0(uint16 len, void* buf);
extern  void    UsbReadEp0(uint16 len, void* buf);
extern	uint16 	UsbWriteEndpoint(uint8 endp, uint16 len, void * buf);
extern	uint16 	UsbReadEndpoint(uint8 endp, uint16 len, void * buf);
extern	uint16 	UsbWriteEpDma(uint8 endp, uint16 len, void * buf, pFunc CallBack);
extern	uint16 	UsbReadEpDma(uint8 endp, uint16 len, void * buf, pFunc CallBack);


//isr.c
extern  void usb_handle_intr(void);
extern  void usb_in_ep_intr(void);
extern  void usb_out_ep_intr(void);
extern  void usb_enum_done_intr(void );
extern  void usb_rx_level_intr(void );
extern  void usb_reset_intr(void);
extern  void usb_Setup0(uint16 len);


//protodma.c
extern 	void 	get_firmware_version(void);
extern 	void 	get_buffer_size(void);
extern 	void 	read_write_register(void);

//epphal.c

/*
--------------------------------------------------------------------------------
  
  Description:  表格定义
  
--------------------------------------------------------------------------------
*/

extern	uint8 StringProduct[18];
extern	USB_DEVICE_DESCRIPTOR 			DeviceDescr;
extern	USB_DEVICE_DESCRIPTOR 			HSDeviceDescr;
extern	uint8 StringManufacture[18];
extern	uint8 StringSerialNumbel[18];

//1表格定义
#ifdef USB
#ifdef	IN_USB_ISR
//配置描述符集合
DRAM_USB_TABLE
USB_CONFIGS_DESCRIPTOR FSConfigDescrs =
{
	sizeof(USB_CONFIGURATION_DESCRIPTOR),		//描述符的大小9(1B)
	USB_CONFIGURATION_DESCRIPTOR_TYPE,		//描述符的类型02(1B)
	CONFIG_DESCRIPTOR_LENGTH,
	1,											//配置所支持的接口数(1B)
	1,											//作为Set configuration的一个参数选择配置值(1B)
	iConfiguration,								//用于描述配置字符串的索引(1B)
	0x80,										//位图,总线供电&远程唤醒(1B)
	200,											//最大消耗电流*2mA(1B)

//接口描述符
	sizeof(USB_INTERFACE_DESCRIPTOR),			//描述符的大小9(1B)
	USB_INTERFACE_DESCRIPTOR_TYPE,				//描述符的类型04(1B)
	0,											//接口的编号(1B)
	0,											//用于为上一个字段可供替换的设置(1B)
	NUM_ENDPOINTS,								//使用的端点数(端点0除外)(1B)
	USB_DEVICE_CLASS_STORAGE,					//1类型代码(由USB分配)(1B),USB_DEVICE_CLASS_STORAGE=Mass Storage
	USB_SUBCLASS_CODE_SCSI,					//1子类型代码(由USB分配)(1B),"0x06=Reduced Block Commands(RBC)"
	USB_PROTOCOL_CODE_BULK,					//1协议代码(由USB分配)(1B),"0X50=Mass Storage Class Bulk-Only Transport"
	iInterface,										//字符串描述的索引(1B)

//端点描述符
	sizeof(USB_ENDPOINT_DESCRIPTOR),
	USB_ENDPOINT_DESCRIPTOR_TYPE,
	BULK_IN_EP|0x80,
	USB_ENDPOINT_TYPE_BULK,
	FS_BULK_RX_SIZE,
	0,		//bulk trans invailed

//端点描述符
	sizeof(USB_ENDPOINT_DESCRIPTOR),
	USB_ENDPOINT_DESCRIPTOR_TYPE,
	BULK_OUT_EP,
	USB_ENDPOINT_TYPE_BULK,
	FS_BULK_RX_SIZE,
	0		//bulk trans invailed
};

//配置描述符集合
DRAM_USB_TABLE
USB_CONFIGS_DESCRIPTOR HSConfigDescrs =
{
	sizeof(USB_CONFIGURATION_DESCRIPTOR),		//描述符的大小9(1B)
	USB_CONFIGURATION_DESCRIPTOR_TYPE,		//描述符的类型02(1B)
	CONFIG_DESCRIPTOR_LENGTH,
	1,											//配置所支持的接口数(1B)
	1,											//作为Set configuration的一个参数选择配置值(1B)
	iConfiguration,								//用于描述配置字符串的索引(1B)
	0x80,										//位图,总线供电&远程唤醒(1B)
	200,											//最大消耗电流*2mA(1B)

//接口描述符
	sizeof(USB_INTERFACE_DESCRIPTOR),			//描述符的大小9(1B)
	USB_INTERFACE_DESCRIPTOR_TYPE,				//描述符的类型04(1B)
	0,											//接口的编号(1B)
	0,											//用于为上一个字段可供替换的设置(1B)
	NUM_ENDPOINTS,								//使用的端点数(端点0除外)(1B)
	USB_DEVICE_CLASS_STORAGE,					//1类型代码(由USB分配)(1B),USB_DEVICE_CLASS_STORAGE=Mass Storage
	USB_SUBCLASS_CODE_SCSI,					//1子类型代码(由USB分配)(1B),"0x06=Reduced Block Commands(RBC)"
	USB_PROTOCOL_CODE_BULK,					//1协议代码(由USB分配)(1B),"0X50=Mass Storage Class Bulk-Only Transport"
	iInterface,										//字符串描述的索引(1B)

//端点描述符
	sizeof(USB_ENDPOINT_DESCRIPTOR),
	USB_ENDPOINT_DESCRIPTOR_TYPE,
	BULK_IN_EP|0x80,
	USB_ENDPOINT_TYPE_BULK,
	HS_BULK_TX_SIZE,//HS_BULK_RX_SIZE,
	0,		//bulk trans invailed

//端点描述符
	sizeof(USB_ENDPOINT_DESCRIPTOR),
	USB_ENDPOINT_DESCRIPTOR_TYPE,
	BULK_OUT_EP,
	USB_ENDPOINT_TYPE_BULK,
	HS_BULK_RX_SIZE,//HS_BULK_RX_SIZE,
	0		//bulk trans invailed
};


//其它速度配置描述符
DRAM_USB_TABLE
OTHER_SPEED_CONFIG_DESCRIPTOR Other_Speed_Config_Descriptor=
{
	sizeof(OTHER_SPEED_CONFIG_DESCRIPTOR),	//length of other speed configuration descriptor
	0x07,									//Other speed configuration Type	
	CONFIG_DESCRIPTOR_LENGTH,		//返回整个数据的长度(配置,接口,端点和类型或供应商)(2B)
	1,										//配置所支持的接口数(1B)
	1,										//作为Set configuration的一个参数选择配置值(1B)
	iConfiguration,							//用于描述配置字符串的索引(1B)
	0x80,									//位图,总线供电&远程唤醒(1B)
	200										//最大消耗电流*2mA(1B)
};

//高速设备限制描述符
DRAM_USB_TABLE
HS_DEVICE_QUALIFIER HS_Device_Qualifier=
{
	sizeof(HS_DEVICE_QUALIFIER),		//length of HS Device Descriptor
	0x06,							//HS Device Qualifier Type	
	0x0200,					        // USB 2.0 version
	USB_DEVICE_CLASS_STORAGE,		//Device class
	USB_SUBCLASS_CODE_SCSI,		//Device SubClass
	USB_PROTOCOL_CODE_BULK, 		//Device Protocol Code
	FS_BULK_RX_SIZE,				//Maximum Packet SIze for other speed
	0x01,							//Number of Other speed configurations
	0x00							//Reserved
};

//字符串描述符,LangId
DRAM_USB_TABLE
uint8 LangId[]=
{
	0x04,
	0x03,

	0x09, 0x04		//English--0x0409
//	0x04, 0x08		//Chinese--0x0804
};

#if iConfiguration > 0
DRAM_USB_TABLE
uint8 StringConfig[]=
{
	18,
	0x03,

	'C', 0x00,
	'O', 0x00,
	'N', 0x00,
	'F', 0x00,
	'I', 0x00,
	'G', 0x00,
	' ', 0x00,
	' ', 0x00
};
#endif

#if iInterface > 0
DRAM_USB_TABLE
uint8 StringInterface[] =
{
	18,
	0x03,

	'I', 0x00,
	'N', 0x00,
	'T', 0x00,
	'E', 0x00,
	'R', 0x00,
	'F', 0x00,
	'A', 0x00,
	'C', 0x00
};
#endif

/**************************************************************************
 USB protocol function pointer arrays
**************************************************************************/
//USB标准请求
DRAM_USB_TABLE
void (*StandardDeviceRequest[])(void) =
{
	get_status,
	clear_feature,
	reserved,
	set_feature,
	reserved,
	set_address,
	get_descriptor,
	reserved,
	get_configuration,
	set_configuration,
	get_interface,
	set_interface,
	reserved,
	reserved,
	reserved,
	reserved
};

#if 0
//用户厂商请求
DRAM_USB_TABLE
void (*VendorDeviceRequest[])(void) =
{
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,//read_write_register,
	reserved,
	reserved,
	reserved
};
#endif

#else
//extern	USB_CONFIGURATION_DESCRIPTOR 	ConfigDescr;
extern	USB_INTERFACE_DESCRIPTOR 		InterfaceDescr;
extern	USB_ENDPOINT_DESCRIPTOR 		MainEP_TXDescr;
extern	USB_ENDPOINT_DESCRIPTOR 		MainEP_RXDescr;

extern	USB_CONFIGURATION_DESCRIPTOR 	HSConfigDescr;
extern	USB_INTERFACE_DESCRIPTOR 		HSInterfaceDescr;
extern	USB_ENDPOINT_DESCRIPTOR 		HSMainEP_TXDescr;
extern	USB_ENDPOINT_DESCRIPTOR 		HSMainEP_RXDescr;

extern	OTHER_SPEED_CONFIG_DESCRIPTOR Other_Speed_Config_Descriptor;
extern	HS_DEVICE_QUALIFIER 			HS_Device_Qualifier;
extern	void (*StandardDeviceRequest[])(void);
extern	void (*VendorDeviceRequest[])(void);
extern	uint8 LangId[4];
extern	uint8 StringConfig[18];
extern	uint8 StringInterface[18];

extern	uint8 HSConfigDescrs[];
extern	uint8 FSConfigDescrs[];

#endif
#endif

/*
********************************************************************************
*
*                         End of USB20.h
*
********************************************************************************
*/

#endif

