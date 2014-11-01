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

uint8 prv_time = PRV_TIME_INIT;

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
	if(type == RS_ANON || type == RS_ALL_201409 || !type) return GET_OK;
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
#ifndef MUL_COMMAND
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
#else
uint16 create_response_data_mul(uint8 *output, uint32 len, uint8 total, uint8 rec, uint8 time) {
	HEADER *h;
	uint8 *p;
	uint16 *tail;

	h = (HEADER *)output;
	h->header = SWAP_HL(PROTOCOL_FRAME_HEADER);
	h->time  = time;
	h->ver    = PROTOCOL_FRAME_VER;
	h->send   = get_self_type();
	h->rec    = rec;
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
#endif

/**
 * Create the active command data.
 *
 * Active command is device sent to the controller.
 *
 * @param output command data.
 * @param command command byte.
 * @return command data length.
 */
#ifndef MUL_COMMAND
int create_active_command_data_n(uint8 *output, uint8 command, uint8 count) {
        uint8 *temp;
        TYPENAME *t;

        temp = output + sizeof(HEADER);
        t = (TYPENAME *)temp;
        t->num     = count;
        t->lenh    = (FRAME_DEVICE_INFO_LEN >> 8) & 0xff;
        t->lenl    = FRAME_DEVICE_INFO_LEN & 0xff;
        t->command = command;
        get_device_name(t->name, DEVICE_NAME_LEN);
        return create_response_data_n(output, sizeof(TYPENAME), 0x01);
}
#else
uint16 create_active_command_data_mul(uint8 *output, uint16 olen, uint8 command, uint8 count) {
        uint8 *temp;
        TYPENAME *t;

        temp = output + olen;
        t = (TYPENAME *)temp;
        t->num     = count;
        t->lenh    = ((FRAME_DEVICE_INFO_LEN - 1) >> 8) & 0xff;
        t->lenl    = (FRAME_DEVICE_INFO_LEN - 1) & 0xff;
        t->command = command;
        get_device_name(t->name, DEVICE_NAME_LEN);
        return sizeof(TYPENAME);
}
#endif

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

uint16 create_command_user_data_mul(uint8 *output, uint16 olen, uint16 len, uint8 num, uint8 count) {
	CHEADER *c;
	uint8 *p;

	len = len + 1;
	p = output + olen;
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
	len = create_active_command_data_mul(value, sizeof(HEADER), command, 0x01);
	len = create_response_data_mul(value, len, 0x01, 0x00, 0x00);
	send_user_data(get_udp_pcb(), &addr, USER_SERVER_PORT, value, len);
	free(value);
}

#ifndef MUL_COMMAND
int scan_devices(uint8 *output, uint8 *dat, uint16 len, uint8 count) {
	uint8 type;

	type = *dat;
	if(!get_type_status(type) || !len) {
		return create_active_command_data_n(output, C_AINLINE, count);
	}
	return NOT_RESPONSE;
}
#else
uint16 scan_devices_mul(uint8 *output, uint16 olen, uint8 *dat, uint16 len, uint8 count) {
	uint8 type;

	type = *dat;
	if(!get_type_status(type) || !len) {
		return create_active_command_data_mul(output, olen, C_AINLINE, count);
	}
	return RETURN_OK;
}
#endif

#ifndef MUL_COMMAND
int get_info_by_name_n(uint8 *output, uint8 *name, uint16 len, uint8 count) {
	uint8 *tname;
	TYPENAME *t;

	tname = output + sizeof(HEADER);
	t = (TYPENAME *)tname;
	t->num     = count;
	t->lenh    = (FRAME_DEVICE_INFO_LEN >> 8) & 0xff;
	t->lenl    = FRAME_DEVICE_INFO_LEN & 0xff;
	t->command = C_AINFO;
	if(!compare_string_n(name, len, ATTR_BY_NAME, sizeof(ATTR_BY_NAME) - 1)) {
		t->type  = get_self_type();
		get_device_name(t->name, DEVICE_NAME_LEN);
		return create_response_data_n(output, sizeof(TYPENAME), 0x01);
	}
	return ERROR_NOT_SUPPORT;
}
#else
uint16 get_info_by_name_mul(uint8 *output, uint16 olen, uint8 *name, uint16 len, uint8 count) {
	uint8 *tname;
	COHEADER *h;
	DEVINFO *d;
	uint16 attr_len;

	tname = output + olen;
	if(!compare_string_n(name, len, ATTR_BY_NAME, sizeof(ATTR_BY_NAME) - 1)) {
		h = (COHEADER *)tname;
		h->num = count;
		h->len   = SWAP_HL(FRAME_DEVICE_INFO_LEN);
		h->command = C_AINFO;
		tname += sizeof(COHEADER);
		d = (DEVINFO *)tname;
		d->type = get_self_type();
		get_device_name(d->name, DEVICE_NAME_LEN);
		tname += sizeof(DEVINFO);
		return tname - (output + olen);
	}
	if(!compare_string_n(name, len, ATTR_GPIO_NAME, sizeof(ATTR_GPIO_NAME) - 1)) {
		attr_len = get_gpio_info(tname, sizeof(COHEADER) + 1) + 1;
		h = (COHEADER *)tname;
		h->num = count;
		h->len   = SWAP_HL(attr_len);
		h->command = C_AINFO;
		tname += sizeof(COHEADER);
		*tname = get_self_type();
		return sizeof(COHEADER) + attr_len;
	}
	return RETURN_OK;
}
#endif

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
	return RETURN_OK;
}

#ifndef MUL_COMMAND
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
#endif

uint16 get_gpio_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;
	VALUETYPE *v;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), ATTR_GPIO_NAME, sizeof(ATTR_GPIO_NAME) - 1);
	h->command = C_GPIO;
	h->sere    = 0x03;
	h->type    = 0x01;
	h->typeext  = 0x02;
	p += sizeof(AHEADER);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT, sizeof(ATTR_GPIO_PORT) - 1);
	m->value   = 0x05;
	p += sizeof(MULITYPE);
	/*m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_GPIO_PORT_A0, sizeof(ATTR_GPIO_PORT_A0) - 1);
	m->value   = 0x00;
	p += sizeof(MULITYPE);*/
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
	h->command = C_UART;
	h->sere    = 0x03;
	h->type    = 0x00;
	h->typeext = 0x05;
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
	h->command = C_I2C;
	h->sere    = 0x03;
	h->type    = 0x01;
	h->typeext = 0x03;
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
	h->command = C_PWM;
	h->sere         = 0x02;
	h->type         = 0x02;
	h->typeext     = 0x02;
	p += sizeof(AHEADER);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_PWM_PORT, sizeof(ATTR_PWM_PORT) - 1);
	m->value      = 0x03;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_PWM_PORT_PWM0, sizeof(ATTR_PWM_PORT_PWM0) - 1);
	m->value      = 0x00;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_PWM_PORT_PWM1, sizeof(ATTR_PWM_PORT_PWM1) - 1);
	m->value      = 0x01;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), ATTR_PWM_PORT_PWM2, sizeof(ATTR_PWM_PORT_PWM2) - 1);
	m->value      = 0x02;
	p += sizeof(MULITYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), ATTR_PWM_FREQ, sizeof(ATTR_PWM_FREQ) - 1);
	v->max        = 0x0a;
	v->min         = 0x01;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), ATTR_PWM_DUTY, sizeof(ATTR_PWM_DUTY) - 1);
	v->max        = 0xff;
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
	h->command = C_DAC;
	h->sere         = 0x02;
	h->type         = 0x01;
	h->typeext    = 0x02;
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
	h->command = C_ADC;
	h->sere         = 0x01;
	h->type         = 0x01;
	h->typeext     = 0x02;
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

#ifdef MOUDLE_3LED
uint16 get_led_timing_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;
	VALUETYPE *v;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), "LEDTIMING", sizeof("LEDTIMING") - 1);
	h->command = C_LED_TIMING;
	h->sere         = 0x02;
	h->type         = 0x0a;
	h->typeext     = 0x00;
	p += sizeof(AHEADER);

	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Mode", sizeof("Mode") - 1);
	v->min = 0x00;
	v->max = 0xFF;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Absolute Hour", sizeof("Absolute Hour") - 1);
	v->min = 0x00;
	v->max = 0x17;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Absolute Minute", sizeof("Absolute Minute") - 1);
	v->min = 0x00;
	v->max = 0x3b;
	p += sizeof(VALUETYPE);

	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Turn On/Off", sizeof("Turn On/Off") - 1);
	v->min = 0x00;
	v->max = 0x01;
	p += sizeof(VALUETYPE);

	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Red", sizeof("Red") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Green", sizeof("Green") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Blue", sizeof("Blue") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);

	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Current Hour", sizeof("Current Hour") - 1);
	v->min = 0x00;
	v->max = 0x17;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Current Minute", sizeof("Current Minute") - 1);
	v->min = 0x00;
	v->max = 0x3b;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Current Second", sizeof("Current Second") - 1);
	v->min = 0x00;
	v->max = 0x3b;
	p += sizeof(VALUETYPE);

	return p - (output + len);
}

uint16 get_led_action_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;
	VALUETYPE *v;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), "LEDACTION", sizeof("LEDACTION") - 1);
	h->command = C_LED_ACTION;
	h->sere         = 0x02;
	h->type         = 0x0a;
	h->typeext    = 0x02;
	p += sizeof(AHEADER);

	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), "Loop&Start Mode", sizeof("Loop&Start Mode") - 1);
	m->value      = 0x04;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), "NoLoopNoContain", sizeof("NoLoopNoContain") - 1);
	m->value      = 0x00;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), "NoLoop&Contain", sizeof("NoLoop&Contain") - 1);
	m->value      = 0x01;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), "Loop&NoContain", sizeof("Loop&NoContain") - 1);
	m->value      = 0x02;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), "Loop&Contain", sizeof("Loop&Contain") - 1);
	m->value      = 0x03;
	p += sizeof(MULITYPE);

	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Start Red", sizeof("Start Red") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Start Green", sizeof("Start Green") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Start Blue", sizeof("Start Blue") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);

	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Target Red", sizeof("Target Red") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Target Green", sizeof("Target Green") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Target Blue", sizeof("Target Blue") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);

	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Gradient TimeHH", sizeof("Gradient TimeHH") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Gradient TimeHL", sizeof("Gradient TimeHL") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Gradient TimeLH", sizeof("Gradient TimeLH") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);
	v = (VALUETYPE *)p;
	copy_array_and_clear(v->name, sizeof(v->name), "Gradient TimeLL", sizeof("Gradient TimeLL") - 1);
	v->min = 0x00;
	v->max = 0xff;
	p += sizeof(VALUETYPE);

	return p - (output + len);
}

uint16 get_led_power_info(uint8 *output, uint16 len) {
	uint8 *p;
	AHEADER *h;
	MULITYPE *m;

	p = output + len;
	h = (AHEADER *)p;
	copy_array_and_clear(h->name, sizeof(h->name), "LEDPOWER", sizeof("LEDPOWER") - 1);
	h->command = C_LED_POWER;
	h->sere         = 0x02;
	h->type         = 0x00;
	h->typeext    = 0x02;
	p += sizeof(AHEADER);

	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), "Power Status", sizeof("Power Status") - 1);
	m->value      = 0x02;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), "Power On", sizeof("Power On") - 1);
	m->value      = 0x00;
	p += sizeof(MULITYPE);
	m = (MULITYPE *)p;
	copy_array_and_clear(m->name, sizeof(m->name), "Power Off", sizeof("Power Off") - 1);
	m->value      = 0x01;
	p += sizeof(MULITYPE);

	return p - (output + len);
}
#endif

#ifndef MUL_COMMAND
uint16 create_attr_data(uint8 *output, uint8 *name, uint16 len, uint8 count) {
	uint16 size = 0;
	uint8 num = 0;
	uint8 type;
	uint16 start = sizeof(HEADER) + sizeof(CHEADER);

	if(!len) type = 0xfb;
	else type = *name;
	
	if(type & 0x01) {
		size += get_gpio_info(output, size + start);
		num++;
	}
	if(type & 0x02) {
		size += get_uart_info(output, size + start);
		num++;
	}
	if(type & 0x04) {
		size += get_i2c_info(output, size + start);
		num++;
	}
	if(type & 0x08) {
		size += get_pwm_info(output, size + start);
		num++;
	}
	if(type & 0x10) {
		size += get_dac_info(output, size + start);
		num++;
	}
	if(type & 0x20) {
		size += get_adc_info(output, size + start);
		num++;
	}

	size += create_command_user_data(output, size, num, count);

	return create_response_data_n(output, size, 0x01);
}
#else
uint16 create_attr_data_mul(uint8 *output, uint16 olen, uint8 *name, uint16 len, uint8 count) {
	uint16 size = 0;
	uint8 num = 0;
	uint8 type;
	uint16 start = olen + sizeof(CHEADER);

	if(!len) type = 0xfb;
	else type = *name;

#ifndef MOUDLE_3LED
	if(type & 0x01) {
		size += get_gpio_info(output, size + start);
		num++;
	}
	if(type & 0x02) {
		size += get_uart_info(output, size + start);
		num++;
	}
	if(type & 0x04) {
		size += get_i2c_info(output, size + start);
		num++;
	}
	if(type & 0x08) {
		size += get_pwm_info(output, size + start);
		num++;
	}
	if(type & 0x10) {
		size += get_dac_info(output, size + start);
		num++;
	}
	if(type & 0x20) {
		size += get_adc_info(output, size + start);
		num++;
	}
#else
	if(type & 0x40) {
		size += get_led_timing_info(output, size + start);
		num++;
	}
	if(type & 0x80) {
		size += get_led_action_info(output, size + start);
		num++;
	}
	if(type & 0x20) {
		size += get_led_power_info(output, size + start);
		num++;
	}
#endif

	size += create_command_user_data_mul(output, olen, size, num, count);

	return size;
}
#endif

int create_net_file(uint8 *dat, uint16 len) {
	FILEHEADER *p;
	
	if(len != sizeof(FILEHEADER)) return;
	p = (FILEHEADER *)dat;
	create_info_file(SWAP_HL(p->file_name), SWAP_HL(p->file_size), (SWAP_HL(p->file_ph) << 16) | SWAP_HL(p->file_pl));
	return RETURN_OK;
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

#ifdef MOUDLE_3LED
uint32 get_time(uint8 h, uint8 m, uint8 s) {
	return h * HOUR_TO_SECOND + m * MINUTE_TO_SECOND + s;
}

void set_bl_en(uint8 en) {
	Gpio_SetPinLevel(LED_BL_EN, en);
}

uint8 byte_to_not(uint8 value) {
	return ~value & 0xff;
}

void save_color_data(uint8 red, uint8 green, uint8 blue, uint8 rf, uint8 gf, uint8 bf) {
	ColorSave[LED_RED_PORT]     = red;
	ColorSave[LED_GREEN_PORT] = green;
	ColorSave[LED_BLUE_PORT]    = blue;
	FreqSave[LED_RED_PORT]      = rf;
	FreqSave[LED_GREEN_PORT]   = gf;
	FreqSave[LED_BLUE_PORT]      = bf;
}

void save_color_to_flash() {
	uint8 value[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8 *d;
	COLORFILE *c;

	read_private_file(FILE_3LED_STATUS, value, sizeof(value));
	d = value;
	c = (COLORFILE *)d;
	if(ColorSave[LED_RED_PORT] != c->color[LED_RED_PORT] || c->freq[LED_RED_PORT] != FreqSave[LED_RED_PORT] ||
	    c->color[LED_GREEN_PORT] != ColorSave[LED_GREEN_PORT] || c->freq[LED_GREEN_PORT] != FreqSave[LED_GREEN_PORT] ||
	    c->color[LED_BLUE_PORT] != ColorSave[LED_BLUE_PORT] || c->freq[LED_BLUE_PORT] != FreqSave[LED_BLUE_PORT]) {
		c->color[LED_RED_PORT]     = ColorSave[LED_RED_PORT];
		c->freq[LED_RED_PORT]      = FreqSave[LED_RED_PORT];
		c->color[LED_GREEN_PORT] = ColorSave[LED_GREEN_PORT];
		c->freq[LED_GREEN_PORT]  = FreqSave[LED_GREEN_PORT];
		c->color[LED_BLUE_PORT]    = ColorSave[LED_BLUE_PORT];
		c->freq[LED_BLUE_PORT]     = FreqSave[LED_BLUE_PORT];
		write_private_file(FILE_3LED_STATUS, value, sizeof(value));
	}
}

void save_timing_to_flash() {
	uint8 value[] = {0x00, 0x00, 0x00, 0x00};
	TIMINGSAVEINFO *t;
	uint8 *p;

	read_private_file(FILE_3LED_TIMING, value, sizeof(value));
	p = value;
	t = (TIMINGSAVEINFO *)p;
	if(OpenTimer != get_time(t->shour, t->sminute, 0) * TIME_BASE || CloseTimer != get_time(t->thour, t->tminute, 0) * TIME_BASE) {
		t->shour      = (OpenTimer / HOUR_TO_SECOND) % 24;
		t->sminute   = (OpenTimer / MINUTE_TO_SECOND) % 60;
		t->thour      = (CloseTimer / HOUR_TO_SECOND) % 24;
		t->tminute   = (CloseTimer / MINUTE_TO_SECOND) % 60;
		write_private_file(FILE_3LED_TIMING, value, sizeof(value));
	}
}

void save_timing_flg_to_flash() {
	uint8 flg;

	read_private_file(FILE_3LED_TIMING, &flg, 1);
	if(flg != TimingMark) {
		flg = TimingMark;
		write_private_file(FILE_3LED_TIMING, &flg, 1);
	}
}

void set_led_color(COLORFILE *c, uint8 flg) {
	if(!c->freq[LED_RED_PORT] || !c->freq[LED_GREEN_PORT] || !c->freq[LED_BLUE_PORT]) return;
	PwmRateSet(LED_RED_PORT, byte_to_not(c->color[LED_RED_PORT]), c->freq[LED_RED_PORT] * 1000);
	PwmRateSet(LED_GREEN_PORT, byte_to_not(c->color[LED_GREEN_PORT]), c->freq[LED_GREEN_PORT] * 1000);
	PwmRateSet(LED_BLUE_PORT, byte_to_not(c->color[LED_BLUE_PORT]), c->freq[LED_BLUE_PORT] * 1000);
	if(flg)
		save_color_data(c->color[LED_RED_PORT], c->color[LED_GREEN_PORT], c->color[LED_BLUE_PORT], c->freq[LED_RED_PORT], c->freq[LED_GREEN_PORT], c->freq[LED_BLUE_PORT]);
}

void set_led_color_n(uint8 red, uint8 green, uint8 blue, uint8 rf, uint8 gf, uint8 bf) {
	if(!rf || !gf || !bf) return;
	PwmRateSet(LED_RED_PORT, byte_to_not(red), rf * 1000);
	PwmRateSet(LED_GREEN_PORT, byte_to_not(green), gf * 1000);
	PwmRateSet(LED_BLUE_PORT, byte_to_not(blue), bf * 1000);
	save_color_data(red, green, blue, rf, gf, bf);
}

void led_init() {
	uint8 value[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8 *p;
	COLORFILE *c;

	read_private_file(FILE_3LED_STATUS, value, sizeof(value));
	p = value;
	c = (COLORFILE *)p;
	if(c->freq[LED_RED_PORT] && c->freq[LED_GREEN_PORT] && c->freq[LED_BLUE_PORT]) {
		set_led_color(c, LED_TRUE);
		set_bl_en(LED_BL_OPEN);
		return;
	}
	set_bl_en(LED_BL_CLOSE);
}

void set_timing_clock(uint8 mode, uint16 atime, uint8 status, uint8 red, uint8 green, uint8 blue, uint32 time) {
	TimingMark = 0;
	CurrentTimer = time * TIME_BASE;
	if(status) {
		OpenTimer = atime * TIME_M_BASE;
		save_color_data(red, green, blue, LED_NORMAL_FREQ, LED_NORMAL_FREQ, LED_NORMAL_FREQ);
		save_color_to_flash();
	} else {
		CloseTimer = atime * TIME_M_BASE;
	}
	if(OpenTimer != CloseTimer) {
		TimingMark = 1;
		if(!status) {
			save_timing_flg_to_flash();
			save_timing_to_flash();
		}
	}
	printf("OpenTimer = %d, CloseTimer = %d, rgb = %02x%02x%02x\n", OpenTimer, CloseTimer, red, green, blue);
}

void open_led() {
	uint8 value[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8 *p;
	COLORFILE *c;

	p = value;
	c = (COLORFILE *)p;
	read_private_file(FILE_3LED_STATUS, value, sizeof(value));
	if(!c->freq[LED_RED_PORT] || !c->freq[LED_GREEN_PORT] || !c->freq[LED_BLUE_PORT]) {
		if(FreqSave[LED_RED_PORT] && FreqSave[LED_GREEN_PORT] && FreqSave[LED_BLUE_PORT]) {
			c->color[LED_RED_PORT] = ColorSave[LED_RED_PORT];
			c->color[LED_GREEN_PORT] = ColorSave[LED_GREEN_PORT];
			c->color[LED_BLUE_PORT] = ColorSave[LED_BLUE_PORT];
			c->freq[LED_RED_PORT]  = FreqSave[LED_RED_PORT];
			c->freq[LED_GREEN_PORT]  = FreqSave[LED_GREEN_PORT];
			c->freq[LED_BLUE_PORT]  = FreqSave[LED_BLUE_PORT];
		} else {
			c->color[LED_RED_PORT] = LED_NORMAL_COLOR;
			c->color[LED_GREEN_PORT] = LED_NORMAL_COLOR;
			c->color[LED_BLUE_PORT] = LED_NORMAL_COLOR;
			c->freq[LED_RED_PORT]  = LED_NORMAL_FREQ;
			c->freq[LED_GREEN_PORT]  = LED_NORMAL_FREQ;
			c->freq[LED_BLUE_PORT]  = LED_NORMAL_FREQ;
		}
	}
	set_led_color(c, LED_TRUE);
	set_bl_en(LED_BL_OPEN);
}

void close_led() {
#ifdef OLD_CLOSE_LED
	COLORFILE c;

	c.color[LED_RED_PORT] = LED_OFF;
	c.color[LED_GREEN_PORT] = LED_OFF;
	c.color[LED_BLUE_PORT] = LED_OFF;
	c.freq[LED_RED_PORT] = FreqSave[LED_RED_PORT];
	c.freq[LED_GREEN_PORT] = FreqSave[LED_GREEN_PORT];
	c.freq[LED_BLUE_PORT] = FreqSave[LED_BLUE_PORT];
	set_led_color(&c, LED_FALSE);
#else
	ColorMark = 0;
	set_bl_en(LED_BL_CLOSE);
#endif
}

void make_timing() {
	if(TimingMark) {
		if(GetClock() == OpenTimer) {
			open_led();
		}
		if(GetClock() == CloseTimer) {
			close_led();
		}
	}
}

void save_single_color(uint8 color, uint8 count) {
	if(count >= sizeof(ColorSave)) return;
	ColorSave[count] = color;
}

uint16 get_color_mark(uint8 count, uint8 flg) {
	uint16 x = 0x01;
	x = x << (3 * count + 2 + flg);
	return x;
}
	
uint16 get_not_value(uint16 num) {
	return ~num;
}

void get_action_color(uint8 *dat, uint8 red, uint8 green, uint8 blue) {
	*dat++ = red;
	*dat++ = green;
	*dat++ = blue;
}

void make_init_data(uint8 count, uint32 time) {
	uint8 space;
	if(StartColor[count] != EndColor[count]) ColorMark |= get_color_mark(count, LED_ACTION_ON);
	else return;
	space = (StartColor[count] > EndColor[count]) ? (StartColor[count] - EndColor[count]) : (EndColor[count] - StartColor[count]);
	if(time >= space) {
		ColorSpace[count] = time / space;
		MarkTime[count]   = ColorSpace[count];
	} else {
		ColorSpace[count] = space / time;
		MarkTime[count]   = 0;
	}
	if(StartColor[count] < EndColor[count]) ColorMark |= get_color_mark(count, LED_ADD_SUB);
	if(MarkTime[count]) ColorMark |= get_color_mark(count, LED_TIME_SPACE);
	MarkTime[count] = 0;
}

/**
 * Set led's color action.
 *
 * Init color's data.
 *
 * @param scolor start color data.
 * @param ecolor end color data.
 * @param time space time,unit ms.
 */
void set_color_action(uint8 cycflg, uint8 sred, uint8 sgreen, uint8 sblue, uint8 ered, uint8 egreen, uint8 eblue, uint32 time) {
	if(cycflg & 0x01) {
		get_action_color(StartColor, sred, sgreen, sblue);
	} else {
		get_action_color(StartColor, ColorSave[LED_RED_PORT], ColorSave[LED_GREEN_PORT], ColorSave[LED_BLUE_PORT]);
	}
	get_action_color(EndColor, ered, egreen, eblue);
	ColorMark = 0;
	time = time / 10;
	if(!time) {
		printf("fle---set color = %d\n", EndColor[0]);
		set_led_color_n(EndColor[LED_RED_PORT], EndColor[LED_GREEN_PORT], EndColor[LED_BLUE_PORT], FreqSave[LED_RED_PORT], FreqSave[LED_GREEN_PORT], FreqSave[LED_BLUE_PORT]);
		save_color_to_flash();
		return;
	}

	if(cycflg & 0x02) {
		ColorMark   |= 2;
	} else {
		ColorMark   |= 1;
	}

	make_init_data(LED_RED_PORT, time);
	make_init_data(LED_GREEN_PORT, time);
	make_init_data(LED_BLUE_PORT, time);
}

void make_color_single(uint8 count) {
	if(!MarkTime[count]) {
		if(ColorMark & get_color_mark(count, LED_TIME_SPACE))
			MarkTime[count] = ColorSpace[count];
		if(ColorMark & get_color_mark(count, LED_ADD_SUB)) {
			if(StartColor[count] > EndColor[count]) {
				ColorMark &= get_not_value(get_color_mark(count, LED_ACTION_ON));
				return;
			} else {
				save_single_color(StartColor[count], count);
				//printf("fle --> count %d:start++ = %d\n", count, StartColor[count]);
			}
			if(ColorMark & get_color_mark(count, LED_TIME_SPACE)) {
				if(StartColor[count] > 0xff - 1) {
					ColorMark &= get_not_value(get_color_mark(count, LED_ACTION_ON));
					return;
				}
				StartColor[count]++;
			} else {
				if(StartColor[count] > 0xff - ColorSpace[count]) {
					ColorMark &= get_not_value(get_color_mark(count, LED_ACTION_ON));
					return;
				}
				StartColor[count] += ColorSpace[count];
			}
		} else {
			if(StartColor[count] < EndColor[count]) {
				ColorMark &= get_not_value(get_color_mark(count, LED_ACTION_ON));
				return;
			} else {
				save_single_color(StartColor[count], count);
				//printf("fle --> count %d:start-- = %d\n", count, StartColor[count]);
			}
			if(ColorMark & get_color_mark(count, LED_TIME_SPACE)) {
				if(StartColor[count] < 1) {
					ColorMark &= get_not_value(get_color_mark(count, LED_ACTION_ON));
					return;
				}
				StartColor[count]--;
			} else {
				if(StartColor[count] < ColorSpace[count]) {
					ColorMark &= get_not_value(get_color_mark(count, LED_ACTION_ON));
					return;
				}
				StartColor[count] -= ColorSpace[count];
			}
		}
	}
	if(ColorMark & get_color_mark(count, LED_TIME_SPACE))
		MarkTime[count]--;
}

void make_color() {
	uint16 rmark, gmark, bmark;
	if(ColorMark & 0x03) {
		rmark = get_color_mark(LED_RED_PORT, LED_ACTION_ON);
		gmark = get_color_mark(LED_GREEN_PORT, LED_ACTION_ON);
		bmark = get_color_mark(LED_BLUE_PORT, LED_ACTION_ON);
		if(ColorMark & rmark) make_color_single(LED_RED_PORT);
		if(ColorMark & gmark) make_color_single(LED_GREEN_PORT);
		if(ColorMark & bmark) make_color_single(LED_BLUE_PORT);
		set_led_color_n(ColorSave[LED_RED_PORT], ColorSave[LED_GREEN_PORT], ColorSave[LED_BLUE_PORT], FreqSave[LED_RED_PORT], FreqSave[LED_GREEN_PORT], FreqSave[LED_BLUE_PORT]);
		if(!(ColorMark & (rmark | gmark | bmark))) {
			ColorMark = 0x00;
			save_color_to_flash();
		}
	}
}
#endif

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
	if(p->port >= CHN_MAX) return;
	PwmRateSet(p->port, p->duty, p->freq * 1000);
}

void set_dac_action(uint8 *dat, uint16 len) {
	
}

#ifdef MOUDLE_3LED
uint32 get_number(uint8 h, uint8 s, uint8 t, uint8 l) {
	return (h << 24) | (s << 16) | (t << 8) | l;
}

void set_timing(uint8 *dat, uint16 len) {
	TIMINGINFO *t;

	if(len != sizeof(TIMINGINFO)) return;
	t = (TIMINGINFO *)dat;
	set_timing_clock(t->mode, t->atime[HOUR_BYTE] * MINUTE_TO_SECOND + t->atime[MINUTE_BYTE], t->status, t->color[LED_RED_PORT], t->color[LED_GREEN_PORT], t->color[LED_BLUE_PORT], get_time(t->ctime[HOUR_BYTE], t->ctime[MINUTE_BYTE], t->ctime[SECOND_BYTE]));
}

void set_action(uint8 *dat, uint16 len) {
	ACTIONINFO *a;

	if(ColorMark & 0x03) return;
	if(len != sizeof(ACTIONINFO)) return;
	a = (ACTIONINFO *)dat;
	set_color_action(a->cycleflg, a->cred, a->cgreen, a->cblue, a->tred, a->tgreen, a->tblue, get_number(a->htime, a->stime, a->ttime, a->ltime));
}

void set_power(uint8 *dat, uint16 len) {
	LEDPOWER *l;

	if(len != sizeof(LEDPOWER)) return;
	if(l->status) {
		open_led();
	} else {
		close_led();
	}
}

void set_color(uint8 *dat, uint16 len) {
	LEDCOLOR *l;

	if(len != sizeof(LEDCOLOR)) return;
	if(!l->mode) {
		set_led_color_n(l->red, l->green, l->blue, LED_NORMAL_FREQ, LED_NORMAL_FREQ, LED_NORMAL_FREQ);
		save_color_to_flash();
	}
}
#endif

/*********** private command andalytical end  ***********/

int check_frame(uint8 *dat, uint16 len) {
	HEADER *h;

	h = (HEADER *)dat;
	if(len < PROTOCOL_MIN_LEN) return ERROR_LEN_NO_ENOUGH;
	if(SWAP_HL(h->header) != PROTOCOL_FRAME_HEADER) return ERROR_FRAME_HEADER;
	if(SWAP_HL(*((uint16 *)(dat + len - 2))) != PROTOCOL_FRAME_TAIL) return ERROR_FRAME_TAIL;
	if(get_type_status(h->rec)) return NOT_RESPONSE;
	if(h->ver != PROTOCOL_FRAME_VER) return ERROR_VERSION;
	return RETURN_OK;
}

int andalytical_private_command(uint8 command, uint8 *dat, uint16 len) {
	void (* command_exe)(uint8 *, uint16) = NULL;
	switch(command) {
		case C_GPIO:
			command_exe = set_gpio_action;
			break;
		case C_UART:
			break;
		case C_I2C:
			break;
		case C_PWM:
			command_exe = set_pwm_action;
			break;
		case C_DAC:
			command_exe = set_dac_action;
			break;
		case C_ADC:
			break;
	#ifdef MOUDLE_3LED
		case C_LED_TIMING:
			command_exe = set_timing;
			break;
		case C_LED_ACTION:
			command_exe = set_action;
			break;
		case C_LED_POWER:
			command_exe = set_power;
			break;
	#endif
	}
	if(command_exe) {
		command_exe(dat, len);
	}
}

#ifdef MUL_COMMAND
uint16 make_mul_command(uint8 *output, uint16 olen, uint8 *dat, uint16 len, uint8 command, uint8 count) {
	int (* return_function)(uint8 *, uint16, uint8 *, uint16, uint8) = NULL;
	int (* set_function)(uint8 *, uint16) = NULL;

	switch(command) {
		case C_UPLOAD:     //0x01
		case C_LEAVE:      //0x02
		case C_AINLINE:    //0x04
		case C_AINFO:      //0x06
		case C_QPW:        //0x0A
			break;
		case C_SCAN:       //0x03
			return_function = scan_devices_mul;
			break;
		case C_QINFO:      //0x05
			return_function = get_info_by_name_mul;
			break;
		case C_SBNAME:     //0x07
			set_function = set_device_name;
			break;
		case C_QATTR:      //0x08
			return_function = create_attr_data_mul;
			break;
		case C_AATTR:      //0x09
			break;
		case C_APW:        //0x0B
			set_function = set_wifi_info;
			break;
		case C_SRECNET:    //0x0C
			ClearMsg(MSG_WIFI_TCP_CONNECTING);
			SendMsg(MSG_WIFI_SCAN_ERR);
			break;
		case C_CREATE:     //0xcf
			set_function = create_net_file;
			break;
		case C_READ:
			read_net_file();
			break;
		case C_WRITE:
			write_private_file(*dat << 8 | *(dat + 1), dat + 2,  len - 2);
			break;
		case 0xcc:
			led_init();
			break;
		default:
			andalytical_private_command(command, dat, len);
			break;
	}
	if(return_function) return return_function(output, olen, dat, len, count);
	if(set_function) return set_function(dat, len);
	return RETURN_OK;
}
#endif

#ifndef MUL_COMMAND
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
			result = get_info_by_name_n(output, dat + 3, *(dat) << 8 | *(dat + 1), 0x01);
			break;
		case C_SBNAME:     //0x07
			result = set_device_name(dat + 3, *(dat) << 8 | *(dat + 1));
			break;
		case C_QATTR:      //0x08
			result = create_attr_data(output, dat + 3, *(dat) << 8 | *(dat + 1), 0x01);
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
#else
uint16 mul_command(uint8 *output, uint8 number, uint8 *dat, uint8 rec, uint8 time) {
	COHEADER *co;
	uint16 size = 0, all_size = 0;
	uint8 count = 0;
	
	while(number--) {
		co = (COHEADER *)dat;
		size = make_mul_command(output, all_size + sizeof(HEADER), dat + sizeof(COHEADER), SWAP_HL(co->len), co->command, count);
		if(size) count++;
		all_size += size;
		dat += (sizeof(COHEADER) + SWAP_HL(co->len));
	}

	if(all_size) return create_response_data_mul(output, all_size, count, rec, time);
	return all_size;
}

uint16 create_answer_data(uint8 *output, uint8 crch, uint8 crcl, uint8 total, uint8 rec, uint8 time) {
	uint8 *d;
	COHEADER *co;

	d = output + sizeof(HEADER);
	co = (COHEADER *)d;
	co->num         = 0x01;
	co->len           = SWAP_HL(0x02);
	co->command = C_ANSWER;
	d += sizeof(COHEADER);
	*d++ = crch;
	*d++ = crcl;

	return create_response_data_mul(output, d - (output + sizeof(HEADER)), total, rec, time);
}

uint16 create_error_version_data(uint8 *output, uint8 total, uint8 rec, uint8 time) {
	uint8 *d;
	COHEADER *co;

	d = output + sizeof(HEADER);
	co = (COHEADER *)d;
	co->num         = 0x01;
	co->len           = 0x0000;
	co->command = C_VERSIONERR;
	d += sizeof(COHEADER);

	return create_response_data_mul(output, d - (output + sizeof(HEADER)), total, rec, time);
}

uint8 check_crc_and_answer(uint8 *dat, uint16 len) {
	HEADER *h;

	h = (HEADER *)dat;
	if(h->time != prv_time) {
		prv_time = h->time;
		return GET_OK;
	}
	if(*(dat + len - 5) != ANSY) return GET_OK;
	return GET_NONE;
}

uint16 analytical_command(uint8 *output, uint8 *dat, uint16 len) {
	HEADER *h;
	uint16 size;
	uint8 *p;

	if(check_frame(dat, len) == ERROR_VERSION) return create_error_version_data(output, 0x01, h->rec, h->time);
	if(check_frame(dat, len) != RETURN_OK) return 0;

	p = dat;
	h = (HEADER *)dat;
	dat += sizeof(HEADER);
	size = mul_command(output, h->lenc, dat, h->rec, h->time);
	if(check_crc_and_answer(p, len)) size += create_answer_data(output + size, *(p + len - 4), *(p + len - 3), 0x01, h->rec, h->time);
	return size;
}
#endif