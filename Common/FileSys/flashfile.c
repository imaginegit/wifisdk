#include "flashfile.h"

void read_data(uint32 addr, uint8 *dat, uint16 len) {
	uint8 value[WIFI_APPW_BLKSIZE];
	uint8 *p;
	
	ReadReservedData(USERFILE_BLK, value, WIFI_APPW_BLKSIZE);
	p = value + addr - FILE_ALL_SIZE_ADDR;
	while(len--) {
		*dat++ = *p++;
	}
}

void write_page_data(uint32 addr, uint8 *pdata, uint32 size) {
	uint8 value[WIFI_APPW_BLKSIZE];
	uint8 *p;
	
	ReadReservedData(USERFILE_BLK, value, WIFI_APPW_BLKSIZE);
	p = value + addr - FILE_ALL_SIZE_ADDR;
	while(size--) {
		*p++ = *pdata++;
	}
	WriteReservedData(USERFILE_BLK, value, WIFI_APPW_BLKSIZE);
}

void read_file_header(int start_addr, uint8 *header, int len) {
	read_data(start_addr, header, len);
}

uint32 get_file_all_size() {
	uint8 size[FLASH_SIZE_LEN];
	uint32 *p;

	read_data(FILE_ALL_SIZE_ADDR, size, FLASH_SIZE_LEN);
	p = (uint32 *)size;

	return *p;
}

void set_file_all_size(uint32 addr) {
	uint8 size[FLASH_SIZE_LEN];
	uint32 *p;
	p = (uint32 *)size;
	*p = addr;
	write_page_data(FILE_ALL_SIZE_ADDR, size, sizeof(size));
}

void create_file_header(uint8 *header, uint16 len, uint16 filename, uint16 size, uint32 q) {
	FILEHEADER *p;

	p = (FILEHEADER *)header;
	p->file_name = filename;
	p->file_size = size;
	p->file_ph   = (q >> 16) & 0xffff;
	p->file_pl   = q & 0xffff;
}

FILEINFO select_file_n(uint16 filename) {
	uint8 header[FILE_HEADER_LEN];
	uint32 start_addr = FILE_START_ADDR;
	uint32 size = 0;
	FILEHEADER *p;
	uint32 len;
	FILEINFO res;
	
	res.content_start_addr = 0;
	len = get_file_all_size();
	while(start_addr - FILE_START_ADDR < len) {
		read_file_header(start_addr, header, sizeof(header));
		p = (FILEHEADER *)header;
		size = p->file_size;
		if(!size) break;
		if(p->file_name == filename) {
			res.content_start_addr = start_addr + FILE_HEADER_LEN;
			res.size = size;
			return res;
		}
		start_addr += size + FILE_HEADER_LEN;
	}
	return res;
}

void create_info_file(uint16 filename, uint16 size, uint32 q) {
	uint8 header[FILE_HEADER_LEN];
	uint32 addr;
	FILEINFO info;

	info = select_file_n(filename);
	if(info.content_start_addr && filename != FILE_MAIN_NAME) return;

	printf("fle--> filename = %04x, size = %04x, q = %08x\n", filename, size, q);

	create_file_header(header, sizeof(header), filename, size, q);
	if(filename == FILE_MAIN_NAME) {
		addr = 0;
	} else {
		addr = get_file_all_size();
	}
	write_page_data(addr + FILE_START_ADDR, header, sizeof(header));
	set_file_all_size(addr + size + FILE_HEADER_LEN);
}

void write_private_file(uint16 filename, uint8 *dat, uint16 len) {
	FILEINFO info;

	info = select_file_n(filename);
	if(!info.content_start_addr) return;
	len = (len > info.size) ? info.size : len;
	write_page_data(info.content_start_addr, dat, len);
}

uint8 read_private_file(uint16 filename, uint8 *dat, uint16 len) {
	FILEINFO info;

	info = select_file_n(filename);
	if(!info.content_start_addr) return;
	len = (len > info.size) ? info.size : len;
	read_data(info.content_start_addr, dat, len);
}