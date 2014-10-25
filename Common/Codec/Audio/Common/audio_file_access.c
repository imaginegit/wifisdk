/*
********************************************************************************
*                   Copyright (c) 2008, Rockchip
*                         All rights reserved.
*
* File Name£º   audio_file_access.c
* 
* Description:  Audio File Operation Interface
*
* History:      <author>          <time>        <version>       
*             Vincent Hsiung    2009-01-08         1.0
*    desc:    ORG.
********************************************************************************
*/

//#include "../AudioConfig.h"
#include "audio_main.h"

#include <stdio.h>
#include <string.h>
#include "FsInclude.h"
#include "File.h"

//#define SIMULATION
//#define	MEMFILE_SIM

typedef unsigned int size_t;

#if 0//dgl
_ATTR_AUDIO_BSS_
size_t   (*RKFIO_FRead)(void * /*buffer*/, size_t /*length*/,FILE *) ;

_ATTR_AUDIO_BSS_
int      (*RKFIO_FSeek)(long int /*offset*/, int /*whence*/ , FILE * /*stream*/);

_ATTR_AUDIO_BSS_
long int (*RKFIO_FTell)(FILE * /*stream*/);

_ATTR_AUDIO_BSS_
size_t   (*RKFIO_FWrite)(void * /*buffer*/, size_t /*length*/,FILE * /*stream*/);

_ATTR_AUDIO_BSS_
unsigned long (*RKFIO_FLength)(FILE *in /*stream*/);
_ATTR_AUDIO_BSS_
int      (*RKFIO_FClose)(FILE * /*stream*/);

_ATTR_AUDIO_BSS_
FILE *pRawFileCache,*pFlacFileHandleBake;
#else
_ATTR_SYS_BSS_
size_t   (*RKFIO_FRead)(void * /*buffer*/, size_t /*length*/,FILE *) ;

_ATTR_SYS_BSS_
int      (*RKFIO_FSeek)(long int /*offset*/, int /*whence*/ , FILE * /*stream*/);

_ATTR_SYS_BSS_
long int (*RKFIO_FTell)(FILE * /*stream*/);

_ATTR_SYS_BSS_
size_t   (*RKFIO_FWrite)(void * /*buffer*/, size_t /*length*/,FILE * /*stream*/);

_ATTR_SYS_BSS_
unsigned long (*RKFIO_FLength)(FILE *in /*stream*/);

_ATTR_SYS_BSS_
int      (*RKFIO_FClose)(FILE * /*stream*/);

_ATTR_SYS_BSS_
FILE *pRawFileCache,*pFlacFileHandleBake;
#endif

