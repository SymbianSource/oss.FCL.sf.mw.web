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


#include "nw_adt_segmentedvectori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_SegmentedVector_Class_t NW_ADT_SegmentedVector_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_ADT_DynamicVector_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		 */ sizeof (NW_ADT_SegmentedVector_t),
    /* construct		 */ _NW_ADT_SegmentedVector_Construct,
    /* destruct			 */ _NW_ADT_SegmentedVector_Destruct
  },
  { /* NW_ADT_Vector		 */
    /* getElementSize            */ _NW_ADT_DynamicVector_GetElementSize,
    /* getSize                   */ _NW_ADT_DynamicVector_GetSize,
    /* addressAt		 */ _NW_ADT_SegmentedVector_AddressAt
  },
  { /* NW_ADT_DynamicVector      */
    /* resizeCapacity            */ _NW_ADT_SegmentedVector_ResizeCapacity,
    /* moveElements		 */ _NW_ADT_SegmentedVector_MoveElements,
    /* clear                     */ _NW_ADT_DynamicVector_Clear
  },
  { /* NW_ADT_SegmentedVector	 */
    /* segmentListIncrement	 */ 4,
    /* allocateSegment           */ _NW_ADT_SegmentedVector_AllocateSegment
  },
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_SegmentedVector_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp)
{
  NW_ADT_SegmentedVector_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_ADT_SegmentedVectorOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_ADT_DynamicVector_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize the object */
  thisObj->segmentSize = (NW_ADT_Vector_Metric_t) va_arg (*argp, NW_Uint32);
  thisObj->segmentListSize =
    NW_ADT_SegmentedVector_GetSegmentListIncrement (thisObj);
  thisObj->segmentList = (NW_Uint8**)
    NW_Mem_Malloc (thisObj->segmentListSize * sizeof (*thisObj->segmentList));
  if (thisObj->segmentList == NULL) {
    return KBrsrOutOfMemory;
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_ADT_SegmentedVector_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_ADT_SegmentedVector_t* thisObj;
  NW_ADT_Vector_Metric_t index;

  /* for convenience */
  thisObj = NW_ADT_SegmentedVectorOf (dynamicObject);

  /* release our resources */
  for (index = 0; index < thisObj->numSegments; index++) {
    NW_Mem_Free (thisObj->segmentList[index]);
  }
  NW_Mem_Free (thisObj->segmentList);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_SegmentedVector_ResizeCapacity (NW_ADT_DynamicVector_t* dynamicVector,
                                        NW_ADT_Vector_Metric_t capacityNeeded)
{
  NW_ADT_SegmentedVector_t* thisObj;
  NW_ADT_Vector_Metric_t newNumSegments;
  NW_ADT_Vector_Metric_t newSegmentListSize;
  NW_ADT_Vector_Metric_t segmentListIncrement;
  NW_Uint8** newSegmentList;
  
  /* for convenience */
  thisObj = NW_ADT_SegmentedVectorOf (dynamicVector);

  /* calculate the new segmentList size */
  newNumSegments =
    (NW_ADT_Vector_Metric_t) ((capacityNeeded - 1) / thisObj->segmentSize + 1);
  segmentListIncrement = NW_ADT_SegmentedVector_GetSegmentListIncrement (thisObj);
  newSegmentListSize = (NW_ADT_Vector_Metric_t)
    (((newNumSegments - 1) / segmentListIncrement + 1) * segmentListIncrement);

  /* if we are shrinking the array, we must first deallocate all the segments
     that will be obsolete */
  while (thisObj->numSegments > newNumSegments) {
    NW_Mem_Free (thisObj->segmentList[--thisObj->numSegments]);
  }
  dynamicVector->capacity =
    (NW_ADT_Vector_Metric_t) (thisObj->numSegments * thisObj->segmentSize);
  
  /* allocate the new segmentList and copy the old segmentList entries into the
     new */
  newSegmentList =
    (NW_Uint8**) NW_Mem_Malloc (newSegmentListSize * sizeof (*newSegmentList));
  if (newSegmentList == NULL) {
    return KBrsrOutOfMemory; 
  }
  (void) NW_Mem_memcpy (newSegmentList, thisObj->segmentList,
                        thisObj->numSegments * sizeof (*newSegmentList));
  
  /* free the old segmentList and install the new */
  NW_Mem_Free (thisObj->segmentList);
  thisObj->segmentList = newSegmentList;
  thisObj->segmentListSize = newSegmentListSize;

  /* if we are growing the array we need to allocate the new segments */
  while (thisObj->numSegments < newNumSegments) {
    thisObj->segmentList[thisObj->numSegments] =
      NW_ADT_SegmentedVector_AllocateSegment(thisObj);
    if (thisObj->segmentList[thisObj->numSegments++] == NULL) {
      thisObj->numSegments -= 1;
      return KBrsrFailure; /* TODO: return more descriptive error */ 
    }
  }
  dynamicVector->capacity =
    (NW_ADT_Vector_Metric_t) (thisObj->numSegments * thisObj->segmentSize);
  
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint8*
_NW_ADT_SegmentedVector_AddressAt (const NW_ADT_Vector_t* vector,
                                   NW_ADT_Vector_Metric_t index)
{
  NW_ADT_SegmentedVector_t* thisObj;
  NW_ADT_Vector_Metric_t segmentIndex;

  /* cast the object into our class */
  thisObj = (NW_ADT_SegmentedVector_t*) vector;

  /* determine the segment index and return the offset into that segment */
  segmentIndex =
    (NW_ADT_Vector_Metric_t) (index / thisObj->segmentSize);
  return
    (NW_Uint8*) thisObj->segmentList[segmentIndex]
    + (index % thisObj->segmentSize)
    * NW_ADT_DynamicVectorOf (vector)->elementSize;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_SegmentedVector_MoveElements (NW_ADT_DynamicVector_t* dynamicVector,
                                      NW_ADT_Vector_Metric_t srcIndex,
                                      NW_ADT_Vector_Metric_t dstIndex)
{
  NW_Int32 sizeDelta;
  NW_ADT_Vector_Metric_t numElements;
  NW_Int32 index;

  /* */
  if (dstIndex > srcIndex) {
    sizeDelta = dstIndex - srcIndex;
  } else {
    sizeDelta = - (NW_Int32) (srcIndex - dstIndex);
  }

  if (dynamicVector->size + sizeDelta > dynamicVector->capacity) {
    TBrowserStatusCode status;

    status =
      NW_ADT_DynamicVector_ResizeCapacity (dynamicVector,
                                           (NW_ADT_Vector_Metric_t)
                                           (dynamicVector->size + sizeDelta));
    if (status != KBrsrSuccess) {
      return status;
    }
  }

  /* now do the actual move */
  /* TODO: this is a very inefficient way of moving the data, we will probably
     need to implement a block move capability */
  numElements = (NW_ADT_Vector_Metric_t) (dynamicVector->size - srcIndex);
  if (srcIndex > dstIndex) {
    for (index = 0; index < numElements; index++) {
      (void) NW_Mem_memcpy (NW_ADT_Vector_AddressAt (NW_ADT_VectorOf (dynamicVector),
                                                     (NW_ADT_Vector_Metric_t) (dstIndex + index)),
                            NW_ADT_Vector_AddressAt (NW_ADT_VectorOf (dynamicVector),
                                                     (NW_ADT_Vector_Metric_t) (srcIndex + index)),
                            dynamicVector->elementSize);
    }
  } else {
    for (index = numElements - 1; index >= 0; index--) {
      (void) NW_Mem_memcpy (NW_ADT_Vector_AddressAt (NW_ADT_VectorOf (dynamicVector),
                                                     (NW_ADT_Vector_Metric_t) (dstIndex + index)),
                            NW_ADT_Vector_AddressAt (NW_ADT_VectorOf (dynamicVector),
                                                     (NW_ADT_Vector_Metric_t) (srcIndex + index)),
                            dynamicVector->elementSize);
    }
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint8*
_NW_ADT_SegmentedVector_AllocateSegment (NW_ADT_SegmentedVector_t* thisObj)
{
  return (NW_Uint8*) NW_Mem_Malloc (thisObj->segmentSize * thisObj->super.elementSize);
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_SegmentedVector_t*
NW_ADT_SegmentedVector_New (NW_ADT_Vector_Metric_t elementSize,
                            NW_ADT_Vector_Metric_t segmentSize)
{
  return (NW_ADT_SegmentedVector_t*)
    NW_Object_New (&NW_ADT_SegmentedVector_Class, (NW_Uint32) elementSize, (NW_Uint32)0,
                   (NW_Uint32)segmentSize);
}
