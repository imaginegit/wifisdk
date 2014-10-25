#include "typedef.h"
#include "sysconfig.h"
#include "global.h"
#include "protocol.h"
#include "flashfile.h"
#include "def.h"
#include "dhcp.h"
#include "ip_addr.h"
#include "osinclude.h"
#include "driverInclude.h"

DMA_CFGX WiFiAudioCtrlDmaCfg = {DMA_CTLL_I2S_TX, DMA_CFGL_I2S_TX, DMA_CFGH_I2S_TX,0};
uint32 dma_value= 0;

int compare_string(uint8 *s, uint8 *m, uint16 len) {
	while(len--) {
		if(*s++ != *m++) return 1;
	}
	return 0;
}

uint16 get_string_length(uint8 *s, uint16 len) {
	uint8 *p;

	p = s + len - 1;
	while(len--) {
		if(*p--) return len + 1;
	}
	return 0;
}

int compare_string_n(uint8 *s, uint16 slen, uint8 *m, uint16 mlen) {
	slen = get_string_length(s, slen);
	mlen = get_string_length(m, mlen);
	if(slen != mlen) return 1;
	return compare_string(s, m, slen);
}

void copy_array(char *o, char *s, uint8 len) {
	while(len--) *o++ = *s++;
}

void copy_array_and_clear(char *o, uint8 olen, char *s, uint8 len) {
	uint8 *p;

	if(olen < len) return;
	p = o;
	while(olen--) *p++ = 0x00;
	copy_array(o, s, len);
}

/**
 * Get current device's type, such as RS_ALL, RS_CSC and so on.
 *
 * Read type from flash.
 *
 * @param none
 * @return currect device's type.
 */
uint8 get_self_type() {
	uint8 type = 0;
	read_private_file(FILE_TYPE_NAME, &type, 1);
	return type;
}

/**
 * Get current device's name.
 *
 * Read name from flash.
 *
 * @param value device's name.
 * @param len device's name max length.
 */
void get_device_name(uint8 *value, uint16 len) {
	read_private_file(FILE_LABEL_NAME, value, len);
}

/**
 * The detection equipment whether to process the data.
 * @param type the type in receive data.
 * @return process result
 * - GET_OK. Process. The device will be carried out to the data processing.
 * - GET_NONE. Not process. The device does not do any treatment.
 */
uint8 get_type_status(uint8 type) {
	if(!type) return GET_OK;
	if(type != get_self_type()) return GET_NONE;
	return GET_OK;
}

/**
 * Create response data.
 *
 * Give the command data plus protocol frame.
 *
 * @param output response data address.
 * @param len command data length.
 * @return response data length.
 */
uint create_response_data_n(uint8 *output, uint32 len, uint8 total) {
	HEADER *h;
	uint8 *p;
	uint16 *tail;

	h = (HEADER *)output;
	h->header = SWAP_HL(PROTOCOL_FRAME_HEADER);
	h->ver    = PROTOCOL_FRAME_VER;
	h->send   = get_self_type();
	h->rec    = 0x00;
	h->lenc   = total;//(len & 0xff0000) >> 16;
	h->lenh   = (len & 0xff00) >> 8;
	h->lenl   = len & 0xff;
	p = output + sizeof(HEADER) + len;
	*p++ = 0x00;
	*p++ = 0x00;
	*p++ = 0x00;
	tail = (uint16 *)p;
	*tail = SWAP_HL(PROTOCOL_FRAME_TAIL);
	return p - output + PROTOCOL_TAIL_LEN;
}

/**
 * Create the active command data.
 *
 * Active command is device sent to the controller.
 *
 * @param output command data.
 * @param command command byte.
 * @return command data length.
 */
int create_active_command_data_n(uint8 *output, uint8 command, uint8 count) {
        uint8 *temp;
        TYPENAME *t;

        temp = output + sizeof(HEADER);
        t = (TYPENAME *)temp;
        t->num     = count;
        t->lenh    = (FRAME_DEVICE_INFO_LEN >> 8) & 0xff;
        t->lenl    = FRAME_DEVICE_INFO_LEN & 0xff;
        t->command = command;
        t->type    = get_self_type();
        get_device_name(t->name, DEVICE_NAME_LEN);
        return create_response_data_n(output, sizeof(TYPENAME), 0x01);
}

uint16 create_command_user_data(uint8 *output, uint32 len, uint8 num, uint8 count) {
	CHEADER *c;
	uint8 *p;

	len = len + 1;
	p = output + sizeof(HEADER);
	c = (CHEADER *)p;
	c->num         = count;
	c->lenh         = len >> 8 & 0xff;
	c->lenl          = len & 0xff;
	c->command = C_AATTR;
	c->tatol         = num & 0xff;

	return sizeof(CHEADER);
}

void upload_connect(uint8 command) {
	IP_ADDR addr, addr1;
	uint8 value[PROTOCOL_MIN_LEN + FRAME_TYPE_NAME_LEN];
	uint16 len;

	addr1 = get_ip_addr();
	IP4_ADDR(&addr, addr1.addr & 0xff, addr1.addr >> 8 & 0xff, addr1.addr >> 16 & 0xff, 255);
	memset(value, 0, PROTOCOL_MIN_LEN + FRAME_TYPE_NAME_LEN);
	len = create_active_command_data_n(value, command, 0x01);
	send_user_data(get_udp_pcb(), &addr, USER_SERVER_PORT, value, len);
	free(value);
}

int scan_devices(uint8 *output, uint8 *dat, uint16 len, uint8 count) {
	uint8 type;

	type = *dat;
	if(!get_type_status(type) || !len) {
		return create_active_command_data_n(output, C_AINLINE, count);
	}
	return NOT_RESPONSE;
}

int get_info_by_name_n(uint8 *output, uint8 *name, uint16 len) {
	uint8 *tname;
	TYPENAME *t;

	tname = output + sizeof(HEADER);
	t = (TYPENAME *)tname;
	t->num     = 0x01;
	t->lenh    = 0x00;
	t->lenl    = 0x15;
	t->command = C_AINFO;
	if(!compare_string_n(name, len, ATTR_BY_NAME, sizeof(ATTR_BY_NAME) - 1)) {
		t->type  = get_self_type();
		get_device_name(t->name, DEVICE_NAME_LEN);
		return create_response_data_n(output, sizeof(TYPENAME), 0x01);
	}
	return ERROR_NOT_SUPPORT;
}

int set_device_name(uint8 *name, uint16 len) {
	write_private_file(FILE_LABEL_NAME, name, len);
	return RETURN_OK;
}

int set_wifi_info(uint8 *dat, uint16 len) {
	WINFO *w;

	w = (WINFO *)dat;
	copy_array(gSysConfig.WiFiConfig.ssid, w->ssid, sizeof(w->ssid));
	copy_array(gSysConfig.WiFiConfig.password, w->pwd, sizeof(w->pwd));
	gSysConfig.WiFiConfig.password_len = get_string_length(w->pwd, sizeof(w->pwd));
	SendMsg(MSG_WIFI_SAVE_INFO);
	ClearMsg(MSG_QPW_ENABLE);
	return NOT_ANSWER;
}

int get_run_status_n(uint8 *output, uint8 value) {
	uint8 *temp;
	ONECOMMAND *p;

	temp = output + sizeof(HEADER);
	p = (ONECOMMAND *)temp;
	p->num     = 0x01;
	p->lenh    = 0x00;
	p->lenl    = 0x01;
	p->command = C_ANSWER;
	p->connext = value;
	return create_response_data_n(output, sizeof(ONECOMMAND), 0x01);
}

uint16 get_gpio_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;
	VALUETYPE *v;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), ATTR_GPIO_NAME, sizeof(ATTR_GPIO_NAME) - 1);
	h->command = 0x11;
	h->sere    = 0x03;
	h->type    = 0x12;
	p += sizeof(AHEADER);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT, sizeof(ATTR_GPIO_PORT) - 1);
	m->value   = 0x06;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT_A0, sizeof(ATTR_GPIO_PORT_A0) - 1);
	m->value   = 0x00;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT_A1, sizeof(ATTR_GPIO_PORT_A1) - 1);
	m->value   = 0x01;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT_A2, sizeof(ATTR_GPIO_PORT_A2) - 1);
	m->value   = 0x02;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT_B0, sizeof(ATTR_GPIO_PORT_B0) - 1);
	m->value   = 0x08;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT_B3, sizeof(ATTR_GPIO_PORT_B3) - 1);
	m->value   = 0x0b;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT_B4, sizeof(ATTR_GPIO_PORT_B4) - 1);
	m->value   = 0x0c;
	p += sizeof(MULITYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), ATTR_GPIO_STATE, sizeof(ATTR_GPIO_STATE) - 1);
	v->min = 0x00;
	v->max = 0x01;
	p += sizeof(VALUETYPE);
	return p - (output + len);
}

uint16 get_uart_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), ATTR_UART_NAME, sizeof(ATTR_UART_NAME) - 1);
	h->command = 0x12;
	h->sere    = 0x03;
	h->type    = 0x21;
	p += sizeof(AHEADER);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_UART_PORT, sizeof(ATTR_UART_PORT) - 1);
	m->value   = 0x01;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_UART_PORT_COM0, sizeof(ATTR_UART_PORT_COM0) - 1);
	m->value   = 0x00;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_UART_BAUD, sizeof(ATTR_UART_BAUD) - 1);
	m->value   = 0x02;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_UART_BAUD_9600, sizeof(ATTR_UART_BAUD_9600) - 1);
	m->value   = 0x01;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_UART_BAUD_115200, sizeof(ATTR_UART_BAUD_115200) - 1);
	m->value   = 0x02;
	p += sizeof(MULITYPE);

	return p - (output + len);
}

uint16 get_i2c_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;
	VALUETYPE *v;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), ATTR_I2C_NAME, sizeof(ATTR_I2C_NAME) - 1);
	h->command = 0x13;
	h->sere    = 0x03;
	h->type    = 0x13;
	p += sizeof(AHEADER);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_I2C_PORT, sizeof(ATTR_I2C_PORT) - 1);
	m->value   = 0x01;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_I2C_PORT_I2C0, sizeof(ATTR_I2C_PORT_I2C0) - 1);
	m->value   = 0x00;
	p += sizeof(MULITYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), ATTR_I2C_ADDR, sizeof(ATTR_I2C_ADDR) - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	
	return p - (output + len);
}

uint16 get_pwm_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;
	VALUETYPE *v;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), ATTR_PWM_NAME, sizeof(ATTR_PWM_NAME) - 1);
	h->command = 0x14;
	h->sere         = 0x02;
	h->type         = 0x14;
	p += sizeof(AHEADER);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_PWM_PORT, sizeof(ATTR_PWM_PORT) - 1);
	m->value      = 0x02;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_PWM_PORT_PWM1, sizeof(ATTR_PWM_PORT_PWM1) - 1);
	m->value      = 0x00;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_PWM_PORT_PWM2, sizeof(ATTR_PWM_PORT_PWM2) - 1);
	m->value      = 0x01;
	p += sizeof(MULITYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), ATTR_PWM_FREQ, sizeof(ATTR_PWM_FREQ) - 1);
	v->max        = 0x0a;
	v->min         = 0x01;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), ATTR_PWM_DUTY, sizeof(ATTR_PWM_DUTY) - 1);
	v->max        = 0x64;
	v->min         = 0x00;
	p += sizeof(VALUETYPE);

	return p - (output + len);
}

uint16 get_dac_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;
	VALUETYPE *v;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), ATTR_DAC_NAME, sizeof(ATTR_DAC_NAME) - 1);
	h->command = 0x15;
	h->sere         = 0x02;
	h->type         = 0x12;
	p += sizeof(AHEADER);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_DAC_PORT, sizeof(ATTR_DAC_PORT) - 1);
	m->value      = 0x02;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_DAC_PORT_DAC0, sizeof(ATTR_DAC_PORT_DAC0) - 1);
	m->value      = 0x00;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_DAC_PORT_DAC1, sizeof(ATTR_DAC_PORT_DAC1) - 1);
	m->value      = 0x01;
	p += sizeof(MULITYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), ATTR_DAC_VALUE, sizeof(ATTR_DAC_VALUE) - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);

	return p - (output + len);
}

uint16 get_adc_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;
	VALUETYPE *v;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), ATTR_ADC_NAME, sizeof(ATTR_ADC_NAME) - 1);
	h->command = 0x16;
	h->sere         = 0x01;
	h->type         = 0x12;
	p += sizeof(AHEADER);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_ADC_PORT, sizeof(ATTR_ADC_PORT) - 1);
	m->value      = 0x03;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_ADC_PORT_ADC0, sizeof(ATTR_ADC_PORT_ADC0) - 1);
	m->value      = 0x00;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_ADC_PORT_ADC1, sizeof(ATTR_ADC_PORT_ADC1) - 1);
	m->value      = 0x01;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_ADC_PORT_ADC2, sizeof(ATTR_ADC_PORT_ADC2) - 1);
	m->value      = 0x02;
	p += sizeof(MULITYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), ATTR_ADC_VALUE, sizeof(ATTR_ADC_VALUE) - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);

	return p - (output + len);
}

uint16 create_attr_data(uint8 *output, uint16 len, uint8 count) {
	uint16 size = 0;
	uint8 num = 0;
	uint16 start = sizeof(HEADER) + sizeof(CHEADER);

	if(!len) count = 0xfb;
	if(count & 0x01) {
		size += get_gpio_info(output, size + start);
		num++;
	}
	if(count & 0x02) {
		size += get_uart_info(output, size + start);
		num++;
	}
	if(count & 0x04) {
		size += get_i2c_info(output, size + start);
		num++;
	}
	if(count & 0x08) {
		size += get_pwm_info(output, size + start);
		num++;
	}
	if(count & 0x10) {
		size += get_dac_info(output, size + start);
		num++;
	}
	if(count & 0x20) {
		size += get_adc_info(output, size + start);
		num++;
	}

	size += create_command_user_data(output, size, num, 0x01);

	return create_response_data_n(output, size, 0x01);
}

void create_net_file(uint8 *dat, uint16 len) {
	FILEHEADER *p;
	
	if(len != sizeof(FILEHEADER)) return;
	p = (FILEHEADER *)dat;
	create_info_file(SWAP_HL(p->file_name), SWAP_HL(p->file_size), (SWAP_HL(p->file_ph) << 16) | SWAP_HL(p->file_pl));
}

void read_net_file() {
	uint8 value[WIFI_APPW_BLKSIZE];
	uint16 len = WIFI_APPW_BLKSIZE;
	uint8 *p;
	
	ReadReservedData(USERFILE_BLK, value, WIFI_APPW_BLKSIZE);
	p = value;
	while(len--) {
		printf("%02x ", *p++);
	}
	printf("\n");
}

/*********** private command andalytical start ***********/

void set_gpio_action(uint8 *dat, uint16 len) {
	GPIOINFO *g;

	g = (GPIOINFO *)dat;
	g->port = g->port >= 32 ? 0 : g->port;

	if(len >= sizeof(GPIOINFO)) {
		GpioMuxSet(g->port, Type_Gpio);
		Gpio_SetPinDirection(g->port, GPIO_OUT);
		Gpio_SetPinLevel(g->port, g->state);
	}
}

void set_pwm_action(uint8 *dat, uint16 len) {
	PWMINFO *p;

	if(len != sizeof(PWMINFO)) return;
	p = (PWMINFO *)dat;
	PwmRateSet(p->port, p->duty, p->freq * 1000);
}

/*void dac_Handler(void) {
	printf("fle-->dma callback = %d\n", dma_value);
	DmaReStart(DMA_CHN2, (UINT32)(&dma_value), (uint32)(&(I2s_Reg->I2S_TXDR)),  
		     1, &WiFiAudioCtrlDmaCfg, dac_Handler);
	I2s_Reg->I2S_TXDR[0] = dma_value;
        UserIsrRequest();
}*/

void set_dac_action(uint8 *dat, uint16 len) {
	if(*dat == 1) dma_value = 0x00;
	else if(*dat == 2) dma_value = 0xffffffff;
	else dma_value = 0x1234;
	printf("fle-->dac value = %d\n", dma_value);
	//DmaChannel2Disable(1);
	//DmaStart(DMA_CHN2, (UINT32)(&dma_value), (uint32)(&(I2s_Reg->I2S_TXDR)),  
		     // 1, &WiFiAudioCtrlDmaCfg, dac_Handler);
	I2SStart(I2S_START_TX);
	I2s_Reg->I2S_TXDR[0] = dma_value;
}

/*********** private command andalytical end  ***********/

int check_frame(uint8 *dat, uint16 len) {
	HEADER *h;

	h = (HEADER *)dat;
	if(len < PROTOCOL_MIN_LEN) return ERROR_LEN_NO_ENOUGH;
	if(SWAP_HL(h->header) != PROTOCOL_FRAME_HEADER) return ERROR_FRAME_HEADER;
	if(SWAP_HL(*((uint16 *)(dat + len - 2))) != PROTOCOL_FRAME_TAIL) return ERROR_FRAME_TAIL;
	if(get_type_status(h->rec)) return NOT_RESPONSE;
	return RETURN_OK;
}

int andalytical_private_command(uint8 command, uint8 *dat, uint16 len) {
	switch(command) {
		case C_GPIO:
			set_gpio_action(dat, len);
			break;
		case C_UART:
			break;
		case C_I2C:
			break;
		case C_PWM:
			set_pwm_action(dat, len);
			break;
		case C_DAC:
			set_dac_action(dat, len);
			break;
		case C_ADC:
			break;
	}
}

int analytical_command(uint8 *output, uint8 *dat, uint16 len) {
	int result = 0;
	
	result = check_frame(dat, len);
	if(result != RETURN_OK) goto END_LABEL;
	
	dat += sizeof(HEADER) + 1;
	
	switch(*(dat + 2)) {
		case C_UPLOAD:     //0x01
		case C_LEAVE:      //0x02
		case C_AINLINE:    //0x04
		case C_AINFO:      //0x06
		case C_QPW:        //0x0A
			result = 0;
			break;
		case C_SCAN:       //0x03
			result = scan_devices(output, dat + 3, *(dat) << 8 | *(dat + 1), 0x01);
			break;
		case C_QINFO:      //0x05
			result = get_info_by_name_n(output, dat + 3, *(dat) << 8 | *(dat + 1));
			break;
		case C_SBNAME:     //0x07
			result = set_device_name(dat + 3, *(dat) << 8 | *(dat + 1));
			break;
		case C_QATTR:      //0x08
			result = create_attr_data(output, *(dat) << 8 | *(dat + 1), *(dat + 3));
			break;
		case C_AATTR:      //0x09
			break;
		case C_APW:        //0x0B
			result = set_wifi_info(dat + 3, *(dat) << 8 | *(dat + 1));
			break;
		case C_SRECNET:    //0x0C
			ClearMsg(MSG_WIFI_TCP_CONNECTING);
			SendMsg(MSG_WIFI_SCAN_ERR);
			break;
		case C_CREATE:     //0xcf
			create_net_file(dat + 3, *(dat) << 8 | *(dat + 1));
			break;
		case C_READ:
			read_net_file();
			break;
		case C_WRITE:
			write_private_file(*(dat + 3) << 8 | *(dat + 4), dat + 5,  *(dat) << 8 | *(dat + 1) - 2);
			break;
		default:
			andalytical_private_command(*(dat + 2), dat + 3, *(dat) << 8 | *(dat + 1));
			break;
	}
END_LABEL:
	if(result < PROTOCOL_MIN_LEN && result != NOT_RESPONSE) {
		result = get_run_status_n(output, result);
	} else if(result == NOT_RESPONSE) {
		result = 0;
	}
	return result;
}