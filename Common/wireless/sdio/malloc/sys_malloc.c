/*--------------------------------------------------------------------------
MALLOC.C is part of the CARM Compiler package from Keil Software.
Copyright (c) 1995 - 2005 Keil Software.  All rights reserved.
$Log: memo.c,v $
--------------------------------------------------------------------------*/

#include "sdio_card.h"
/*
* dgl request add start.
*/
#define sys_malloc_free_mem_siz 0x2000//28k
static void * _gpAppHeapHandle;
static char  sys_malloc_free_mem[sys_malloc_free_mem_siz];
/*
* dgl request add end.
*/

#define MEMO_FLAGSIZE               (8) // 2 int32
#define MEMO_PTRFLAG                0XFEFEFE77 //flag 
#define MEMO_FREEFLAG               0XFEFEFE88 //pointer free flag. 



/*  Memory pool header:  __mp__ points to the first available.
 *
 *  Note that the list is maintained in address order.  __mp__ points to the
 *  block with the lowest address.  That block points to the block with the
 *  next higher address and so on.                                         
 */
struct __mp__                      /* memory pool */
{
    struct __mp__   *next;           /* single-linked list */
    unsigned int      rlen;
    unsigned int      len;           /* length of following block */
};

#define HLEN     ((sizeof(struct __mp__)) )  // 12 bytes
#define MIN_BLOCK_BIT   2  
#define MIN_BLOCK    (0X0001<<MIN_BLOCK_BIT)
#define ALIGNED( size )   (((((unsigned int)size+MIN_BLOCK-1))>>MIN_BLOCK_BIT)<<MIN_BLOCK_BIT)
#define ALIGNMIX( size )    (((((unsigned int)size))>>MIN_BLOCK_BIT)<<MIN_BLOCK_BIT)
#define MERGE( prev,next )  ((((char *)prev) + prev->len + HLEN) == (char *) next)



/* 
Initialize Block oriented memory pool;
heapStart: the start address of the memory pool.
heapEnd  : the end   address of the memory pool.
*/
void * init_mempool(void *heapStart, void *heapEnd)            
{
    /*  Set the __mp__ to point to the beginning of the pool and set the pool size.*/
    
    unsigned int size;
    struct __mp__ *   pHeapInfo;

    heapStart = (void *) ALIGNED(heapStart);   
    size = (unsigned int)((char*)heapEnd - (char*)heapStart);
    size = ALIGNMIX(size);   // only can small,not big, -1 

    pHeapInfo = (struct __mp__ *) heapStart;

    pHeapInfo->next = NULL;  
    pHeapInfo->len = size;
    pHeapInfo->rlen = size;

    pHeapInfo++;
    
    /*  Set the link of the block in the pool to NULL (since it's the only
     *  block) and initialize the size of its data area.                   */
    pHeapInfo->next       = NULL;
    pHeapInfo->len        = size - 2 * HLEN; 
    pHeapInfo->rlen       = size - 2 * HLEN;
    
    return pHeapInfo;
}



/*--------------------------------------------------------------------------
MALLOC.C is part of the CARM Compiler package from Keil Software.
Copyright (c) 1995 - 2005 Keil Software.  All rights reserved.
--------------------------------------------------------------------------*/

/*  Memory pool header:  __mp__ points to the first available.
 *
 *  Note that the list is maintained in address order.  __mp__ points to the
 *  block with the lowest address.  That block points to the block with the
 *  next higher address and so on.
 *
 *  Memory is laid out as follows:
 *
 *  {[NEXT | LEN] [BLOCK (LEN bytes)]} {[NEXT | LEN][BLOCK] } ...
 *
 *  Note that the size of a node is:
 *          mp.len + sizeof (struct mp_b)
 */



static void *Malloc(void *gpHeapFreeAddr  , unsigned int size)
{
    struct __mp__  *q;      /* ptr to free block */
    struct __mp__  *p;      /* q->next */
    unsigned int k;         /* space remaining in the allocated block */
    unsigned int sizeb = size;

    /*  Make sure that block is word aligned                                    */
    size = ALIGNED(size) ;

#if ( MEMO_FLAGSIZE > 0 ) 
    size += MEMO_FLAGSIZE;
#endif


    /*  Initialization:  Q is the pointer to the next available block.          */
    q = (struct __mp__ *) gpHeapFreeAddr ; //&__pfree;

    /*  End-Of-List:  P points to the next block.  If that block DNE (P==NULL),
     *  we are at the end of the list.                                          */

    while (1)
    {
        p = q->next;
        if (!p)  return (NULL); /* FAILURE */

        /*  Found Space:  If block is large enough, reserve if.  Otherwise, copy P
         *  to Q and try the next free block.                                       */
        if (p->len >= size) break;
        q = p;
    }


    /*  Reserve P:  Use at least part of the P block to satisfy the allocation
     *  request.  At this time, the following pointers are setup:
     *  P points to the block from which we allocate Q->next points to P        */

    k = p->len - size;  /* calc. remaining bytes in block */


#if ( MEMO_FLAGSIZE > 0 )   
    {
        uint32  *t = (uint32*)((char *) & p[1] + p->len - MEMO_FLAGSIZE);
        *t++ = MEMO_PTRFLAG;
        *t = MEMO_PTRFLAG;
    }
#endif


    if (k <= (HLEN + MEMO_FLAGSIZE + MIN_BLOCK)) /* rem. bytes too small for new block */
    {
        q->next = p->next;
#if ( MEMO_FLAGSIZE > 0 )   
        p->next = (struct __mp__ *)MEMO_PTRFLAG;
#endif
        p->rlen = sizeb;
        return (&p[1]);      /* SUCCESS */
    }

    /*  Split P Block:  If P is larger than we need, we split P into two blocks:
     *  the leftover space and the allocated space.  That means, we need to
     *  create a header in the allocated space.                                 */

    k -= (HLEN);
    p->len = k;

    q = (struct __mp__*)(((char *)(&p[1])) + k);
    q->len = size;
    q->rlen = sizeb;
    
#if ( MEMO_FLAGSIZE > 0 )
    q->next = (struct __mp__ *)MEMO_PTRFLAG;
#endif

    return (&q[1]);           /* SUCCESS */
}


/*--------------------------------------------------------------------------
FREE.C is part of the CARM Compiler package from Keil Software.
Copyright (c) 1995 - 2005 Keil Software.  All rights reserved.
--------------------------------------------------------------------------*/

/*  Memory pool header:  __mp__ points to the first available.
 *
 *  Note that the list is maintained in address order.  __mp__ points to the
 *  block with the lowest address.  That block points to the block with the
 *  next higher address and so on.                                          */

static int Free(void *gpHeapFreeAddr  , void *memp)
{
    /*  FREE attempts to organize Q, P0, and P so that Q < P0 < P.  Then, P0 is
     *  inserted into the free list so that the list is maintained in address
     *  order.
     *
     *  FREE also attempts to consolidate small blocks into the largest block
     *  possible.  So, after allocating all memory and freeing all memory,
     *  you will have a single block that is the size of the memory pool.  The
     *  overhead for the merge is very minimal.                                */
    struct __mp__  **pfree;

    struct __mp__ *q;                    /* ptr to free block */
    struct __mp__ *p;                    /* q->next           */
#define p0 ((struct __mp__ *) memp)    /* block to free */

    /*  If the user tried to free NULL, get out now.  Otherwise, get the address
     *  of the header of the memp block (P0).  Then, try to locate Q and P such
     *  that Q < P0 < P.                                                       */

    memp = &p0 [-1];                     /* get address of header */

#if (MEMO_FLAGSIZE > 0 )    //add mark in the end.
    {
        uint32  *t;
        if ((uint32)p0->next != MEMO_PTRFLAG)
        {
            return 0;
        }

        t = (uint32*)((char *) & p0[1] + p0->len - MEMO_FLAGSIZE);
        if (t[0] != MEMO_PTRFLAG || t[1] != MEMO_PTRFLAG)
        {
            return 0;
        }
        
        t[0] = MEMO_FREEFLAG;
        t[1] = MEMO_FREEFLAG;
    }
    
#endif

    pfree = (struct __mp__  **)gpHeapFreeAddr;
    /*  Initialize.  Q = Location of first available block.                    */
	//q = __pfree;

    q = *pfree;
    if (!q)
    {
        // __pfree   = p0;
        *pfree = p0;
        p0->next = NULL;
        return 0;
    }


    /*  B1. When P0 before Q the block to free is before the first available
     *  block. Set P as Q, clear Q, initialize P0 as first available block.    */
    if (q > p0)
    {
        // __pfree   = p0;
        *pfree = p0;
        p      = q;
        q      = NULL;
    }
    else
    {
        /*  B2. Advance P. Hop through the list until we find a free block that is
         *  located in memory AFTER the block we're trying to free.                */
        while (1)
        {
            p = q->next;
            
            if ((!p) || (p > p0)) 
            	break;

            q = p;
        }
    }

    /*  B3. Check upper bound. If P0 and P are contiguous, merge block P into
     *  block P0.                                                              */
    if (p && MERGE(p0, p))
    {
        p0->len += p->len + HLEN;
        p0->next = p->next;
    }
    else
    {
        p0->next = p;
    }

    /*  B4. Check lower bound. If Q and P0 are contiguous, merge P0 into Q.  */
    if (q)
    {
        if (MERGE(q, p0))
        {
            q->len += p0->len + HLEN;
            q->next = p0->next;
        }
        else
        {
            q->next = p0;
        }
    }
	return 1;
}

//use for extern or extern only can see.
static void sys_mem_init(void *memstart, void *memend)
{
    _gpAppHeapHandle = (void *)init_mempool(memstart, memend);
}

void __user_initial_stackheap(void)
{
	sys_mem_init(&sys_malloc_free_mem[0], &sys_malloc_free_mem[sys_malloc_free_mem_siz-1]);
}

void* sys_malloc(uint32 size)
{
    void * ptr;

    if (!_gpAppHeapHandle)
		return NULL;
       
    ptr = (void*)Malloc((void*) &_gpAppHeapHandle , size);

    if (!ptr)
    {
    	while(1);
    }

    return ptr;
}


void* sys_calloc(uint32 size)
{
    void * ptr;

    if(!_gpAppHeapHandle)
       return NULL;
       
    ptr = (void*)Malloc((void*) &_gpAppHeapHandle , size);

    if (!ptr)
    {
    	while(1);
    }
	memset(ptr, 0, size);

    return ptr;
}

void sys_free(void *__ptr)
{
    if (!__ptr)
        return;

    Free((void*)&_gpAppHeapHandle, __ptr);
}
