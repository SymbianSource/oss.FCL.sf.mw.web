/*
* Copyright (c) 1999 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/***************************************************************************
**   File: nwx_memseg_epoc32.c
**   Purpose:  Memory segment management
**************************************************************************/

#include <string.h> // For memset
#include "nwx_defs.h"
#include "nwx_ctx.h"
#include "nwx_mem.h"
#include "nwx_memseg.h"
#include "BrsrStatusCodes.h"

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/
#define MINBLOCKSIZE  64          /* the minimum size block allocated by the system */
/*#define OVERHEADSIZE  16*/          /* the number of bytes consumed by system overhead */

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

typedef struct _mem_block_t mem_block_t;
struct _mem_block_t {
  mem_block_t   *pNext;
  NW_Uint32     offset;
  NW_Uint32     size;
  NW_Byte       buffer[4];
};


/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/

static mem_block_t *NewMemBlock(const NW_Uint32 size);
static void DeleteMemBlock(mem_block_t *pBlock);
static void *AllocFromBlock(mem_block_t *pBlock, const NW_Uint32 size);
static NW_Uint32 AdjustAllocSize(const NW_Uint32 size);


/*
**-------------------------------------------------------------------------
**  Global Variables in Context
**-------------------------------------------------------------------------
*/

typedef struct {
	NW_Mem_Segment_Id_t numSegments;
	mem_block_t **pSegments;
} MemSeg_Context_t;


/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
static MemSeg_Context_t *MemSeg_GetThis()
{
  MemSeg_Context_t *ctx = (MemSeg_Context_t*) NW_Ctx_Get(NW_CTX_MEM_SEG,0);
  if (ctx == NULL)
  {
    ctx = (MemSeg_Context_t*) NW_Mem_Malloc(sizeof(MemSeg_Context_t));
    if (ctx)
    {
      ctx->numSegments = 0;
      ctx->pSegments = NULL;
      NW_Ctx_Set(NW_CTX_MEM_SEG, 0, ctx);
    }
  }
  return ctx;
}

/* Create a new memory block */
static mem_block_t *NewMemBlock(const NW_Uint32 size)
{
  mem_block_t *pBlock;
  NW_Uint32 allocSize;
  NW_Uint32 blockSize;

  NW_ASSERT(size > 0);

  /*
   * Alloc a block large enough for the block header plus
   * the requested size. Note that the header size
   * includes 4 bytes of the buffer.
   */
  allocSize = AdjustAllocSize(size);

  /* Get the actual size of the memory block which the system
   * will allocate to hold the requested size, and bump
   * up the allocated size to use all of the available memory.
   */
  blockSize = AdjustAllocSize(sizeof(mem_block_t) - sizeof(NW_Byte[4]) + allocSize);
  allocSize = blockSize - (sizeof(mem_block_t) - sizeof(NW_Byte[4]));

  pBlock = (mem_block_t*) NW_Mem_Malloc(blockSize);
  if (pBlock != NULL) {
    pBlock->pNext   = NULL;
    pBlock->offset  = 0;
    pBlock->size    = allocSize;
  }

  return pBlock;
}



/* Delete a memory block */
static void DeleteMemBlock(mem_block_t *pBlock)
{
  NW_ASSERT(pBlock != NULL);

  pBlock->pNext   = NULL;
  pBlock->offset  = 0;
  pBlock->size    = 0;
  NW_Mem_Free(pBlock);

  return;
}


/* Allocate memory from current block, or return NULL */
static void *AllocFromBlock(mem_block_t *pBlock, const NW_Uint32 size)
{
  void *mem = NULL;
  NW_Uint32 allocSize;

  NW_ASSERT(pBlock != NULL);
  NW_ASSERT(size > 0);

  /* adjust allocation size to maintain memory allignment */
  allocSize = AdjustAllocSize(size);

  /* attempt to allocate memory from this block */
  if ((pBlock->size - pBlock->offset) >= allocSize) {
    mem = &pBlock->buffer[pBlock->offset];
    pBlock->offset = pBlock->offset + allocSize;
  }

  return mem;
}


/* Round up the size of the allocation to maintain memory alignment */
static NW_Uint32 AdjustAllocSize(const NW_Uint32 size)
{
  NW_Uint32 adjustedSize = size;

  /* Plato environment (ARM compiler) requires 4 NW_Byte alignment.
   * Always leave the offset at a 4 NW_Byte boundary. This may fragment
   * 3 bytes for each AllocFromBlock() call.
   */
  adjustedSize = ((adjustedSize + 3) & 0xFFFFFFFC);

  return adjustedSize;
}

/*********************************************************
**  Name:   NW_Mem_Segment_ShutDown
**  Description:  Shutsdown the memory segment manager and
**  frees all the memory segements. These memory segments 
**  should not be referenced after this function completes.
**  Parameters:   none
**  Return Value: KBrsrSuccess
**********************************************************/
static TBrowserStatusCode ShutDown()
{
  MemSeg_Context_t *ctx = MemSeg_GetThis();
  if (ctx == NULL)
    return KBrsrSuccess;

  if (ctx->pSegments != NULL) {
    NW_Mem_Free(ctx->pSegments);
  }

  ctx->pSegments = NULL;
  ctx->numSegments = 0;

  return KBrsrSuccess;
}

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*********************************************************
**  Name:   NW_Mem_Segment_New
**  Description:  Create and intialize a new memory segment.
**  Parameters:   none
**  Return Value: ID of the new segment or 0 if failure 
**********************************************************/
NW_Mem_Segment_Id_t NW_Mem_Segment_New(void)
{
  NW_Mem_Segment_Id_t i;
  NW_Mem_Segment_Id_t newNumSegments;
  mem_block_t **newSegments;

  MemSeg_Context_t *ctx = MemSeg_GetThis();
  if (ctx == NULL)
    return 0;

  /* Initialize array of segments if necessary */
  if (ctx->pSegments == NULL) {
    ctx->numSegments = NW_MEM_SEGMENT_LAST * 2;
    ctx->pSegments   = (mem_block_t **) NW_Mem_Malloc(ctx->numSegments * sizeof(mem_block_t*));
    if (ctx->pSegments == NULL) {
      return 0;
    }

    for (i = 0; i < ctx->numSegments; i++) {
      ctx->pSegments[i] = NULL;
    }
  }

  /* Find an unused segment, if any */
  for (i = NW_MEM_SEGMENT_LAST; i < ctx->numSegments; i++) {
    if (ctx->pSegments[i] == NULL) {
      /* Init the segment to prevent another "new" prior to 1st allocation */
      ctx->pSegments[i] = NewMemBlock(MINBLOCKSIZE);
      return i;
    }
  }

  /* Enlarge the array of segments */
  newNumSegments  = ctx->numSegments + 4;
  newSegments     = (mem_block_t **) NW_Mem_Malloc(newNumSegments * sizeof(mem_block_t*));
  if (newSegments == NULL) {
    return 0;
  }

  for (i = 0; i < ctx->numSegments; i++) {
    newSegments[i] = ctx->pSegments[i];
  }
  for (i = ctx->numSegments; i < newNumSegments; i++) {
    newSegments[i] = NULL;
  }
  NW_Mem_Free(ctx->pSegments);

  i = ctx->numSegments;
  ctx->numSegments = newNumSegments;
  ctx->pSegments = newSegments;

  /* Return ID of the 1st new segment */
  /* Init the segment to prevent another "new" prior to 1st allocation */
  ctx->pSegments[i] = NewMemBlock(MINBLOCKSIZE);
  return i;
}


/*********************************************************
**  Name:   NW_Mem_Segment_Free
**  Description:  Frees the memory allocated from the specified memory segment.
**  Parameters:   sid - the segment ID number
**  Return Value: none
**********************************************************/
void NW_Mem_Segment_Free(const NW_Mem_Segment_Id_t sid)
{
  mem_block_t *pBlock;
  mem_block_t *pTemp;
  NW_Bool shutdown = NW_TRUE;
  NW_Mem_Segment_Id_t i; 

  MemSeg_Context_t *ctx = MemSeg_GetThis();
  if (ctx == NULL)
    return;

  NW_ASSERT(sid < ctx->numSegments);
  NW_ASSERT(ctx->pSegments != NULL);
  if ((ctx->pSegments == NULL) || (sid >= ctx->numSegments))
    return;
  /* Handle the special segment */
  if (sid == NW_MEM_SEGMENT_MANUAL) {
    return;
  }

  /* Release all the memory blocks of the segment */
  pBlock = ctx->pSegments[sid];
  while (pBlock != NULL) {
    pTemp   = pBlock;
    pBlock  = pBlock->pNext;
    DeleteMemBlock(pTemp);
  }

  /* Mark the segment as unused */
  ctx->pSegments[sid] = NULL;

  /* Shutdown memory segment if all of the segments have been deleted */
  for (i = 0; i < ctx->numSegments; i++) {
    if (ctx->pSegments[i] != NULL) {
      shutdown = NW_FALSE;
      break;
    }
  }
  if (shutdown == NW_TRUE) {
    ShutDown();
  }

  return;
}


/*********************************************************
**  Name:   NW_Mem_Segment_Malloc
**  Description:  Allocate a block of the requested size from
**                the specified memory segment.
**  Parameters:   sid - the segment ID number
**  Return Value: pointer to the allocated block or NULL
**********************************************************/
void* NW_Mem_Segment_Malloc(const NW_Uint32  size, const NW_Mem_Segment_Id_t sid)
{
  mem_block_t *pBlock;
  mem_block_t *pTemp = NULL;
  void *mem;

  MemSeg_Context_t *ctx = MemSeg_GetThis();
  if (ctx == NULL)
    return 0;

  NW_ASSERT(size > 0);
  NW_ASSERT(sid < ctx->numSegments);
  NW_ASSERT(ctx->pSegments != NULL);

  /* Handle the special segment */
  if (sid == NW_MEM_SEGMENT_MANUAL) {
    return NW_Mem_Malloc(size);
  }

  /* Find a block which has enough unused memory */
  pBlock = ctx->pSegments[sid];
  while (pBlock != NULL) {
    mem = AllocFromBlock(pBlock, size);
    if (mem != NULL) {
      return mem;
    }
    pTemp   = pBlock;
    pBlock  = pBlock->pNext;
  }

  /* Add another block to the segment */
  pBlock = NewMemBlock(size);
  if (pBlock == NULL) {
    return NULL;
  }

  if (pTemp == NULL) {
    ctx->pSegments[sid] = pBlock;
  } else {
    pTemp->pNext = pBlock;
  }

  /* Allocate memory from the new block */
  mem = AllocFromBlock(pBlock, size);
  return mem;
}

/************************************************************************
Function: NW_Mem_Segment_Shutdown()
Purpose:
Clean up memory segment context
Paramters:      NONE
Return Value:   NONE
**************************************************************************/
TBrowserStatusCode NW_Mem_Segment_ShutDown(void)
{
  MemSeg_Context_t *ctx = MemSeg_GetThis();
  if (ctx != NULL)
  {
    NW_Mem_Free(ctx);
    NW_Ctx_Set(NW_CTX_MEM_SEG, 0, NULL);
  }

  return KBrsrSuccess; 
}
