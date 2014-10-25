/* Copyright (C) 2012 ROCK-CHIPS FUZHOU . All Rights Reserved. */
/*
File   : memo.h
Desc   : define system heap mange interface function.

Author : 
Date   : 2012-07-10
Notes  :

*
*/
#ifndef _ROCK_HEAPS_H_
#define _ROCK_HEAPS_H_

extern void __user_initial_stackheap(void);
extern void* sys_malloc(uint32 size);
extern void* sys_calloc(uint32 size);
extern void  sys_free(void *__ptr);
#endif 
