#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define PROTOCOL_FRAME_HEADER 0x3135
#define PROTOCOL_FRAME_VER    0x13
#define PROTOCOL_FRAME_TAIL   0x5313

#define PROTOCOL_HEADER_LEN     0x02
#define PROTOCOL_RESERVED_LEN 0x03
#define PROTOCOL_VERSION_LEN    0x01
#define PROTOCOL_SEND_LEN         0x01
#define PROTOCOL_REC_LEN           0x01
#define PROTOCOL_DLEN_LEN        0x03
#define PROTOCOL_ASWER_LEN     0x01
#define PROTOCOL_CHECK_LEN      0x02
#define PROTOCOL_TAIL_LEN     0x02
#define PROTOCOL_FIRST_ADDR   PROTOCOL_HEADER_LEN + PROTOCOL_RESERVED_LEN + PROTOCOL_VERSION_LEN + PROTOCOL_SEND_LEN + PROTOCOL_REC_LEN + PROTOCOL_DLEN_LEN + PROTOCOL_ASWER_LEN + PROTOCOL_CHECK_LEN
#define PROTOCOL_MIN_LEN     PROTOCOL_FIRST_ADDR + PROTOCOL_TAIL_LEN

#define DEVICE_NAME_LEN       20
#define WIFI_SSID_LEN         20
#define WIFI_PWD_LEN          20

#define C_UPLOAD              0x01
#define C_LEAVE               0x02
#define C_SCAN                0x03
#define C_AINLINE             0x04
#define C_QINFO               0x05
#define C_AINFO               0x06
#define C_SBNAME              0x07
#define C_QATTR               0x08
#define C_AATTR               0x09
#define C_QPW                 0x0a
#define C_APW                 0x0b
#define C_SRECNET             0x0c
#define C_ANSWER              0x0d

/****************************************************/
/*  private command */
#define C_GPIO                  0x11
#define C_UART                  0x12
#define C_I2C                    0x13
#define C_PWM                  0x14
#define C_DAC                   0x15
#define C_ADC                   0x16

#define C_CREATE              0xcf
#define C_WRITE                0xbf
#define C_READ                  0xdf

#define ATTR_BY_NAME          "byname"

/** attr info start **/
/** gpio **/
#define ATTR_GPIO_NAME            "GPIOCTRL"
#define ATTR_GPIO_PORT             "GPIOPORT"
#define ATTR_GPIO_PORT_A0       "GPIOAPORT0"
#define ATTR_GPIO_PORT_A1       "GPIOAPORT1"
#define ATTR_GPIO_PORT_A2       "GPIOAPORT2"
#define ATTR_GPIO_PORT_B0       "GPIOBPORT0"
#define ATTR_GPIO_PORT_B3       "GPIOBPORT3"
#define ATTR_GPIO_PORT_B4       "GPIOBPORT4"
#define ATTR_GPIO_STATE           "GPIOSTATE"

/** uart **/
#define ATTR_UART_NAME                  "UARTCTRL"
#define ATTR_UART_PORT                   "UARTPort"
#define ATTR_UART_PORT_COM0        "com0"
#define ATTR_UART_BAUD                  "baud"
#define ATTR_UART_BAUD_9600        "9600"
#define ATTR_UART_BAUD_115200    "115200"

/** i2c **/
#define ATTR_I2C_NAME               "I2CCTRL"
#define ATTR_I2C_PORT                "I2CPort"
#define ATTR_I2C_PORT_I2C0       "i2c0"
#define ATTR_I2C_ADDR                "devaddr"

/** pwm **/
#define ATTR_PWM_NAME             "PWMCTRL"
#define ATTR_PWM_PORT              "PWMPort"
#define ATTR_PWM_PORT_PWM1  "PWM1"
#define ATTR_PWM_PORT_PWM2  "PWM2"
#define ATTR_PWM_FREQ              "frequency(kHz)"
#define ATTR_PWM_DUTY              "Duty Ratio"

/** dac **/
#define ATTR_DAC_NAME              "DACCTRL"
#define ATTR_DAC_PORT               "DACPort"
#define ATTR_DAC_PORT_DAC0     "DAC0"
#define ATTR_DAC_PORT_DAC1     "DAC1"
#define ATTR_DAC_VALUE             "DAC Value"

/** adc **/
#define ATTR_ADC_NAME              "ADCCTRL"
#define ATTR_ADC_PORT               "ADCPort"
#define ATTR_ADC_PORT_ADC0     "ADC0"
#define ATTR_ADC_PORT_ADC1     "ADC1"
#define ATTR_ADC_PORT_ADC2     "ADC2"
#define ATTR_ADC_VALUE             "ADC Value"
/** attr info end  **/

#define SWAP_HL(x)            (((x) << 8 & 0xff00) | ((x) >> 8 & 0xff))

#define FRAME_DEVICE_INFO_LEN DEVICE_NAME_LEN + 0x01
typedef struct _device_info {
        uint8 type;
        uint8 name[DEVICE_NAME_LEN];
} DEVINFO;

#define RESERVE_BYTE_LEN       0x03
#define FRAME_HEADER_LEN      0x08
typedef __packed struct _header {
	uint16 header;
	uint8 reservebyte[RESERVE_BYTE_LEN];
	uint8  ver;
	uint8  send;
	uint8  rec;
	uint8  lenc;
	uint8  lenh;
	uint8  lenl;
} HEADER;

#define FRAME_TYPE_NAME_LEN   DEVICE_NAME_LEN + 0x05
typedef struct _type_name {
	uint8  num;
	uint8  lenh;
	uint8  lenl;
	uint8  command;
	uint8  type;
	uint8  name[DEVICE_NAME_LEN];
} TYPENAME;

#define FRAME_WIFI_INFO_LEN   WIFI_SSID_LEN + WIFI_PWD_LEN + 0x01
typedef struct _wifi_info {
	uint8  type;
	char  ssid[WIFI_SSID_LEN];
	char  pwd[WIFI_PWD_LEN];
} WINFO;

#define FRAME_COMMAND_LEN     0x05
typedef struct _command_byte {
	uint8  num;
	uint8  lenh;
	uint8  lenl;
	uint8  command;
	uint8  connext;
} ONECOMMAND;

#define FRAME_COMMANDS_LEN    DEVICE_NAME_LEN + 0x04
typedef struct _command_bytes {
	uint8  num;
	uint8  lenh;
	uint8  lenl;
	uint8  command;
	uint8  name[DEVICE_NAME_LEN];
} MULCOMMAND;

#define TYPE_NAME_LEN    0x0f
typedef __packed struct _muli_type {
	uint8 name[TYPE_NAME_LEN];
	uint8 value;
} MULITYPE;

typedef __packed struct _value_type {
	uint8 name[TYPE_NAME_LEN];
	uint8 min;
	uint8 max;
} VALUETYPE;

typedef __packed struct _attr_header {
	uint8 name[TYPE_NAME_LEN];
	uint8 command;
	uint8 sere;
	uint8 type;
} AHEADER;

typedef __packed struct _command_header {
	uint8  num;
	uint8  lenh;
	uint8  lenl;
	uint8  command;
	uint8  tatol;
} CHEADER;

/***** private command struct *****/
typedef __packed struct _gpio_info {
	uint8 port;
	uint8 state;
} GPIOINFO;

typedef __packed struct _pwm_info {
	uint8 port;
	uint8 freq;
	uint8 duty;
} PWMINFO;

#endif