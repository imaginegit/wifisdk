/*
********************************************************************************
*                   Copyright (c) 2008, Rockchip
*                         All rights reserved.
*
* File Name£º   audio_file_access.h
* 
* Description:  
*
* History:      <author>          <time>        <version>       
*             Vincent Hsiung     2009-1-8          1.0
*    desc:    ORG.
********************************************************************************
*/

#ifndef _AUDIO_FILE_ACCESS_H_
#define _AUDIO_FILE_ACCESS_H_

#include <stdio.h>

#define RKNANO_FS

#define SEEK_SET 0 /* start of stream (see fseek) */
#define SEEK_CUR 1 /* current position in stream (see fseek) */
#define SEEK_END 2 /* end of stream (see fseek) */
#define NULL 0

typedef unsigned int size_t;

extern size_t        (*RKFIO_FRead)(void * /*buffer*/, size_t /*length*/,FILE *) ;
extern int           (*RKFIO_FSeek)(long int /*offset*/, int /*whence*/ ,FILE * /*stream*/);
extern long int      (*RKFIO_FTell)(FILE * /*stream*/);
extern size_t        (*RKFIO_FWrite)(void * /*buffer*/, size_t /*length*/,FILE * /*stream*/);
extern unsigned long (*RKFIO_FLength)(FILE *in /*stream*/);
extern int           (*RKFIO_FClose)(FILE * /*stream*/);

extern FILE *pRawFileCache,*pFlacFileHandleBake;

#endif
