/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides interfaces to data structure routines.
*                These routines support a number of different data structures.
*        
*                NW_Ds_Dar - DynamicArray of pointers. Creates an array of pointers
*                to void *. Elements are inserted and retrieved by index.
*                The array is fixed size put will automatically grow if
*                the append function is called and the array needs to grow.
*
*/

#ifndef NWX_DATASTRUCT_H
#define NWX_DATASTRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/
#include "nwx_defs.h"
#include "BrsrStatusCodes.h"

/*************************************************************************
**          Dynamic Array of Pointers Routines
**************************************************************************/

/*
** Type Definitions
*/ 

typedef void* NW_Ds_Mem_MallocHandler();
typedef void NW_Ds_Mem_FreeHandler();

typedef struct {
  NW_Uint16                    size;             /* number of elements allocated */
  NW_Uint16                    numElements;      /* number of elements in use */
  void                      **elements;       /* array of pointers to the elements */
  NW_Ds_Mem_MallocHandler   *mallocHandler;   /* memory allocation routine */
  NW_Ds_Mem_FreeHandler     *freeHandler;     /* memory deallocation routine */
} NW_Ds_DynamicArray_t;


/*
** Global Function Declarations
*/

/* create a dynamic array on the default heap */
NW_Ds_DynamicArray_t *NW_Ds_DarNew_DefaultHeap(const NW_Uint16 size);

/* create a dynamic array */
NW_Ds_DynamicArray_t *NW_Ds_DarNew(const NW_Uint16 size, 
                   NW_Ds_Mem_MallocHandler *alloc, 
                   NW_Ds_Mem_FreeHandler *free);

/* deletes a dynamic array */
void NW_Ds_DarDelete(NW_Ds_DynamicArray_t *array);

/* append pointer to new element to end of array */
TBrowserStatusCode NW_Ds_DarAppend(NW_Ds_DynamicArray_t *array, void *element);

/* get the pointer to the specified element */
void *NW_Ds_DarGetElement(const NW_Ds_DynamicArray_t *array,
                          const NW_Uint32 index);


/* return count of elements */
NW_Uint16 NW_Ds_DarGetCount(const NW_Ds_DynamicArray_t *array);

/*************************************************************************
**          Single Linked List Routines
**************************************************************************/

/*
** Type Declarations
*/

typedef struct _NW_Node_t NW_Node_t;
struct _NW_Node_t {
  NW_Node_t   *next;
  void        *data;
};

/*************************************************************************
**          Collection Routines
**************************************************************************/

/*
** Type Declarations
*/

typedef NW_Bool NW_Ds_CollectionMatcher_t (const void *key, const void *data);

typedef struct _NW_Ds_Collection_t NW_Ds_Collection_t;

typedef const struct {
  TBrowserStatusCode (*add)(NW_Ds_Collection_t *coll, void *value);
  void * (*lookup)(NW_Ds_Collection_t *coll, const void *key,
                   NW_Ds_CollectionMatcher_t matcher);
  void * (*remove)(NW_Ds_Collection_t *coll, const void *key,
                   NW_Ds_CollectionMatcher_t matcher);
  void   (*reset)(NW_Ds_Collection_t *coll);
  void * (*getNext)(NW_Ds_Collection_t *coll);
  void   (*free_current)(NW_Ds_Collection_t *coll);
  NW_Bool  (*empty)(NW_Ds_Collection_t *coll);
} NW_Ds_CollectionApi_t;

struct _NW_Ds_Collection_t {
  NW_Ds_CollectionApi_t *api;
  void                  *data;
};


/*
** Global Function Declarations
*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif  /* NWX_DATASTRUCT_H */


