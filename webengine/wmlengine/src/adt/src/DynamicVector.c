/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_adt_dynamicvectori.h"
#include <nwx_assert.h>
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_DynamicVector_Class_t NW_ADT_DynamicVector_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_ADT_Vector_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_ADT_DynamicVector_t),
    /* construct                 */ _NW_ADT_DynamicVector_Construct,
    /* destruct			             */ NULL
  },
  { /* NW_ADT_Vector		         */
    /* getElementSize            */ _NW_ADT_DynamicVector_GetElementSize,
    /* getSize                   */ _NW_ADT_DynamicVector_GetSize,
    /* addressAt		             */ NULL
  },
  { /* NW_ADT_DynamicVector      */
    /* resizeCapacity            */ NULL,
    /* moveElements		           */ _NW_ADT_DynamicVector_MoveElements,
    /* clear                     */ _NW_ADT_DynamicVector_Clear
  }
};


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_DynamicVector_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp)
{
  NW_ADT_DynamicVector_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_ADT_DynamicVector_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_ADT_DynamicVectorOf (dynamicObject);

  /* initialize the object */
  thisObj->elementSize = (NW_ADT_Vector_Metric_t )va_arg (*argp, NW_Uint32);
  thisObj->capacity = (NW_ADT_Vector_Metric_t )va_arg (*argp, NW_Uint32);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_ADT_Vector_Metric_t
_NW_ADT_DynamicVector_GetElementSize (const NW_ADT_Vector_t* vector)
{
  return NW_ADT_DynamicVectorOf (vector)->elementSize;
}

/* ------------------------------------------------------------------------- */
NW_ADT_Vector_Metric_t
_NW_ADT_DynamicVector_GetSize (const NW_ADT_Vector_t* vector)
{
  return NW_ADT_DynamicVectorOf (vector)->size;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_DynamicVector_MoveElements (NW_ADT_DynamicVector_t* dynamicVector,
                                    NW_ADT_Vector_Metric_t srcIndex,
                                    NW_ADT_Vector_Metric_t dstIndex)
{
  TBrowserStatusCode status;
  NW_Int32 sizeDelta;

  /* if the move needs a bigger array to complete, it is our duty to allocate
     that block now */
  if (dstIndex > srcIndex) {
    sizeDelta = dstIndex - srcIndex;
  } else {
    /*lint -e{650} Constant out of range for operator */
    sizeDelta = - NW_INT32_CAST(srcIndex - dstIndex);
  }
  if (dynamicVector->size + sizeDelta > dynamicVector->capacity) {
    status =
      NW_ADT_DynamicVector_ResizeCapacity (dynamicVector,
                                           (NW_ADT_Vector_Metric_t)
                                           (dynamicVector->size + sizeDelta));
    if (status != KBrsrSuccess) {
      return status;
    }
  }

  /* move the data */
  (void) NW_Mem_memmove (NW_ADT_Vector_AddressAt (&dynamicVector->super, dstIndex),
		         NW_ADT_Vector_AddressAt (&dynamicVector->super, srcIndex),
		         (dynamicVector->size - srcIndex)
                         * dynamicVector->elementSize);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_DynamicVector_Clear (NW_ADT_DynamicVector_t* dynamicVector)
{
  NW_Int32 index = dynamicVector->size;

  for (--index; index >= 0; index--) {
    TBrowserStatusCode status;
    
    status = NW_ADT_DynamicVector_RemoveAt (dynamicVector,
                                            (NW_ADT_Vector_Metric_t) index);
    NW_ASSERT (status == KBrsrSuccess);
		/* To fix TI compiler warning */
		(void) status; 
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void**
_NW_ADT_DynamicVector_ReplaceAt (NW_ADT_DynamicVector_t* dynamicVector,
                                 void* element,
                                 NW_ADT_Vector_Metric_t where)
{
  /* convert the where if NW_ADT_Vector_AtEnd is specified */
  if (where == NW_ADT_Vector_AtEnd) {
    where = (NW_ADT_Vector_Metric_t) (dynamicVector->size - 1);
  }

  /* make sure that the where index is not out of bounds */
  NW_ASSERT (where < dynamicVector->size);

  /* copy the element into vector */
  if (element != NULL) {
    (void) NW_Mem_memcpy (NW_ADT_Vector_AddressAt (NW_ADT_VectorOf (dynamicVector),
                                                   where),
                          element, dynamicVector->elementSize);
  }

  /* successful completion */
  return (void**)
    NW_ADT_Vector_AddressAt (NW_ADT_VectorOf (dynamicVector), where);
}

/* ------------------------------------------------------------------------- */
void**
_NW_ADT_DynamicVector_InsertAt (NW_ADT_DynamicVector_t* dynamicVector,
                                void* element,
                                NW_ADT_Vector_Metric_t where)
{
  TBrowserStatusCode status;

  /* convert the where if NW_ADT_Vector_AtEnd is specified */
  if (where == NW_ADT_Vector_AtEnd) {
    where = dynamicVector->size;
  }

  /* make sure that the where element is not out of bounds */
  NW_ASSERT (where <= dynamicVector->size);

  /* move all the elements up by one, if this fails we simply return
     the error code passed to us */
  status =
    NW_ADT_DynamicVector_MoveElements (dynamicVector, where,
                                       (NW_ADT_Vector_Metric_t) (where + 1));
  if (status != KBrsrSuccess) {
    return NULL;
  }

  /* copy the element into vector */
  if (element != NULL) {
    (void) NW_Mem_memcpy (NW_ADT_Vector_AddressAt (NW_ADT_VectorOf (dynamicVector),
                                                   where),
                          element, dynamicVector->elementSize);
  } else {
    /*
     * if element is NULL, then we need to zero out the memory block.  This is necessary
	 * because later code which fills in the values for this newly allocated vector
	 * element may leave some bytes in the memory block un-assigned due to padding.
	 */
    NW_Mem_memset ( NW_ADT_Vector_AddressAt ( NW_ADT_VectorOf ( dynamicVector ) , where ) ,
		0 , dynamicVector->elementSize ) ;
  }

  /* increment the size count */
  dynamicVector->size += 1;

  /* successful completion */
  return (void**)
    NW_ADT_Vector_AddressAt (NW_ADT_VectorOf (dynamicVector), where);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_DynamicVector_RemoveAt (NW_ADT_DynamicVector_t* dynamicVector,
                                NW_ADT_Vector_Metric_t index)
{
  TBrowserStatusCode status;

  /* convert the index if NW_ADT_Vector_AtEnd is specified */
  if (index == NW_ADT_Vector_AtEnd) {
    index = (NW_ADT_Vector_Metric_t) (dynamicVector->size -  1);
  }

  /* make sure that the index element is not out of bounds */
  if (index >= dynamicVector->size) {
    return KBrsrFailure;
  }

  /* don't bother to move anything if the resultant size is zero */
  if (dynamicVector->size > 1) {
    /* move all the elements down by one, if this fails we simply return
       the error code passed to us */
    status =
      NW_ADT_DynamicVector_MoveElements (dynamicVector,
                                         (NW_ADT_Vector_Metric_t) (index + 1),
                                         index);
    if (status != KBrsrSuccess) {
      return status;
    }
  }

  /* increment the size count */
  dynamicVector->size -= 1;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_DynamicVector_RemoveElement (NW_ADT_DynamicVector_t* dynamicVector,
                                     void* element)
{
  NW_ADT_Vector_Metric_t index;

  /* find the element in the vector */
  for (index = 0; index < dynamicVector->size; index++) {
    if (NW_Mem_memcmp (NW_ADT_Vector_ElementAt (&dynamicVector->super,
                                                   index),
                       element, dynamicVector->elementSize) == 0) {
      return NW_ADT_DynamicVector_RemoveAt (dynamicVector, index);
    }
  }

  /* unsuccessful completion */
  return KBrsrNotFound;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_DynamicVector_InsertVectorAt (NW_ADT_DynamicVector_t* dynamicVector,
                                     NW_ADT_Vector_t* vector,
                                     NW_ADT_Vector_Metric_t where)
{
  TBrowserStatusCode status;
  NW_Int32 index;
  NW_ADT_Vector_Metric_t numElements;

  /* be optimistic, assume that we succeed */
  status = KBrsrSuccess;

  /* iterate through our source vector's elements appending them one by one to
     our destination vector */
  numElements = NW_ADT_Vector_GetSize (vector);
  for (index = numElements - 1; index >= 0; index--) {
    void* element;

    element =
      NW_ADT_Vector_ElementAt (vector, (NW_ADT_Vector_Metric_t) index);
    if (element == NULL) {
      status = KBrsrFailure; /* TODO: return a more descriptive error code */
      break;
    }

    if (NW_ADT_DynamicVector_InsertAt (dynamicVector, element, where) == NULL) {
      status = KBrsrOutOfMemory; 
      break;
    }
  }

  /* if a failure occurred, clean up after us */
  if (status != KBrsrSuccess) {
    for (; index != 0; index--) {
      NW_ADT_DynamicVector_RemoveAt (dynamicVector, where);
    }
}

  /* successful completion */
  return status;
}

