#ifndef __FLASH_FILE_H__
#define __FLASH_FILE_H__

#include "Typedef.h"
#include "sysconfig.h"
#include "global.h"
#include "SysReservedOperation.h"

typedef struct file_info {
	uint32 content_start_addr;
	uint16 size;
} FILEINFO;

typedef struct file_header {
	uint16 file_name;
	uint16 file_size;
	uint16 file_ph;
	uint16 file_pl;
} FILEHEADER;

#define FILE_NAME_LEN     2
#define FILE_SIZE_LEN     2
#define FILE_P_LEN        4
#define FILE_HEADER_LEN   (FILE_NAME_LEN + FILE_SIZE_LEN + FILE_P_LEN)

#define FLASH_PAGE_SIZE     256
#define FILE_ALL_SIZE_ADDR  0x120000
#define FILE_START_ADDR     0x120004
#define FLASH_SIZE_LEN      (FILE_START_ADDR - FILE_ALL_SIZE_ADDR)

#define FILE_OS_ADDR        256

#define FILE_MAIN_NAME      0x5F00
#define FILE_TYPE_NAME      0x5F0E
#define FILE_LABEL_NAME     0x5F0B
#define FILE_KEY_NAME       0x5F0C

extern void create_info_file(uint16 filename, uint16 size, uint32 q);
extern FILEINFO select_file_n(uint16 filename);
extern uint8 read_private_file(uint16 filename, uint8 *dat, uint16 len);
extern void write_private_file(uint16 filename, uint8 *dat, uint16 len);

#endif