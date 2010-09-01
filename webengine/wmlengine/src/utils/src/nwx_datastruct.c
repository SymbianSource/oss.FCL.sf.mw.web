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

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_defs.h"
#include "nwx_datastruct.h"
#include "BrsrStatusCodes.h"


/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/
static TBrowserStatusCode DarGrow(NW_Ds_DynamicArray_t *);

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*lint -save -e64 -e119 Type mismatch, Too many arguments*/

/*************************************************************************
**          Dynamic Array of Pointers Routines
**************************************************************************/

/*****************************************************************
**  Name:   NW_Ds_DarNew_DefaultHeap
**  Description:  Helper function which creates the array on the
**                default heap
**  Parameters:   size - number of elements in the initial
**                allocation
**  Return Value: pointer to the dynamic array or NULL if failure
******************************************************************/
NW_Ds_DynamicArray_t *NW_Ds_DarNew_DefaultHeap(const NW_Uint16 size)
{
  return NW_Ds_DarNew(size, NW_Mem_Malloc_Address, NW_Mem_Free_Address);
}

/*****************************************************************
**  Name:   NW_Ds_DarNew
**  Description:  Creates a dyamic array of pointers. The size of the
**                array returned is always greater then or equal to one.
**  Parameters:   size - number of elements in the initial allocation
**                *fpAlloc -- function pointer to the memory allocation routine
**                *fpFree --  function pointer to the free allocation routine
**  Return Value: pointer to the dynamic array or NULL if failure
******************************************************************/
NW_Ds_DynamicArray_t *NW_Ds_DarNew(const NW_Uint16 size,
                                   NW_Ds_Mem_MallocHandler *fpAlloc,
                                   NW_Ds_Mem_FreeHandler *fpFree)
{
  NW_Uint16 i;
  NW_Ds_DynamicArray_t *array;

  NW_ASSERT(fpAlloc != NULL);
  NW_ASSERT(fpFree != NULL);

#ifndef FEATURE_MEMORY_PROFILE
  array = (fpAlloc)(sizeof(NW_Ds_DynamicArray_t));
#else
  array = NW_Mem_Malloc(sizeof(NW_Ds_DynamicArray_t));
#endif
  if (array != NULL) {
    array->size = NW_UINT16_CAST(NW_MAX(size, 1));
    array->numElements = 0;
#ifndef FEATURE_MEMORY_PROFILE
    array->elements = (fpAlloc)(array->size * sizeof(void *));
#else
    array->elements = NW_Mem_Malloc(array->size * sizeof(void *));
#endif
    if (array->elements == NULL) {
#ifndef FEATURE_MEMORY_PROFILE
      (fpFree)(array);
#else
      NW_Mem_Free(array);
#endif

      return NULL;
    }
    for (i = 0; i < array->size; i++)
      array->elements[i] = NULL;
    array->mallocHandler = fpAlloc;
    array->freeHandler   = fpFree;
  }

  return array;
}

/*****************************************************************
**  Name:   NW_Ds_DarDelete
**  Description:  Deletes a dyamic array of pointers.
**  Parameters:   *array - a pointer to the dynamic array
**  Return Value: void
******************************************************************/
void NW_Ds_DarDelete(NW_Ds_DynamicArray_t *array)
{
 if ((array != NULL) && (array->elements != NULL) && (array->freeHandler != NULL))
  {
#ifndef FEATURE_MEMORY_PROFILE
  (array->freeHandler)(array->elements);
  (array->freeHandler)(array);
#else
  NW_Mem_Free(array->elements);
  NW_Mem_Free(array);
#endif
  }
  return;
}

/*****************************************************************
**  Name:   NW_Ds_DarAppend
**  Description:  adds a pointer to an element to the end of the array.
**                The array will grow as needed.
**  Parameters:   *array - a pointer to the dynamic array
**                *element - the data to append
**  Return Value: KBrsrSuccess if succeeds, else KBrsrOutOfMemory
******************************************************************/
TBrowserStatusCode NW_Ds_DarAppend(NW_Ds_DynamicArray_t *array, void *element)
{
  NW_ASSERT(array != NULL);
  NW_ASSERT(array->elements != NULL);

  if (array->numElements >= array->size) {
    if (DarGrow(array) == KBrsrOutOfMemory)
      return KBrsrOutOfMemory;
  }
  array->elements[array->numElements++] = element;

  return KBrsrSuccess;
}

/*****************************************************************
**  Name:   DarGrow
**  Description:  Grow the array by 1/2 the current size
**  Parameters:   *array - a pointer to the dynamic array
**  Return Value: KBrsrSuccess if succeeds, else KBrsrOutOfMemory
******************************************************************/
static TBrowserStatusCode DarGrow(NW_Ds_DynamicArray_t *array)
{
  NW_Int32 i;
  NW_Uint16 newsize;
  void **temp;

  NW_ASSERT(array != NULL);
  NW_ASSERT(array->elements != NULL);

  newsize = NW_UINT16_CAST(array->size + NW_MAX((array->size/2), 1));
#ifndef FEATURE_MEMORY_PROFILE
  temp = (array->mallocHandler)(newsize * sizeof(void *));
#else
  temp = NW_Mem_Malloc(newsize * sizeof(void *));
#endif
  if (temp == NULL) {
    return KBrsrOutOfMemory;
  } else {
    array->size = newsize;
    for (i = 0; i < array->numElements; i++)
      temp[i] = array->elements[i];
    for (i = array->numElements; i < array->size; i++)
      temp[i] = NULL;
#ifndef FEATURE_MEMORY_PROFILE
    (array->freeHandler)(array->elements);
#else
    NW_Mem_Free(array->elements);
#endif
    array->elements = temp;
    return KBrsrSuccess;
  }
}


/*****************************************************************
**  Name:   NW_Ds_DarGetElement
**  Description:  gets the pointer to the specified element
**  Parameters:   *array - a pointer to the dynamic array
**                index - the index of the element to get
**  Return Value: a pointer to the data or NULL if array out of bounds
******************************************************************/
void *NW_Ds_DarGetElement(const NW_Ds_DynamicArray_t *array, const NW_Uint32 index)
{
  NW_ASSERT(array != NULL);
  NW_ASSERT(array->elements != NULL);
  NW_ASSERT(index < array->numElements);

  if (index < array->numElements)
    return array->elements[index];
  else
    return NULL;
}
/*****************************************************************
**  Name:   NW_Ds_DarGetCount
**  Description:  gets the number of elements in the array
**  Parameters:   *array - a pointer to the dynamic array
**  Return Value: the actual number of elements stored in the array
******************************************************************/
NW_Uint16 NW_Ds_DarGetCount(const NW_Ds_DynamicArray_t *array)
{
  if(array != NULL)
    return array->numElements;
  return 0;
}

/*lint -restore*/
