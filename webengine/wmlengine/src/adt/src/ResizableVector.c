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


#include "nw_adt_resizablevectori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_ResizableVector_Class_t NW_ADT_ResizableVector_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_ADT_DynamicVector_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		           */ sizeof (NW_ADT_ResizableVector_t),
    /* construct                 */ _NW_ADT_ResizableVector_Construct,
    /* destruct                  */ _NW_ADT_ResizableVector_Destruct
  },
  { /* NW_ADT_Vector		         */
    /* getElementSize            */ _NW_ADT_DynamicVector_GetElementSize,
    /* getSize                   */ _NW_ADT_DynamicVector_GetSize,
    /* addressAt		             */ _NW_ADT_ResizableVector_AddressAt
  },
  { /* NW_ADT_DynamicVector      */
    /* resizeCapacity            */ _NW_ADT_ResizableVector_ResizeCapacity,
    /* moveElements		           */ _NW_ADT_DynamicVector_MoveElements,
    /* clear                     */ _NW_ADT_DynamicVector_Clear
  },
  { /* NW_ADT_ResizableVector	   */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_ResizableVector_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp)
{
  NW_ADT_ResizableVector_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_ADT_ResizableVectorOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_ADT_DynamicVector_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* initialize the object */
  thisObj->increment = (NW_ADT_Vector_Metric_t) va_arg (*argp, NW_Uint32);
  thisObj->array = NULL;
  if (thisObj->super.capacity) {
    thisObj->array = (NW_Uint8*) NW_Mem_Malloc (thisObj->super.capacity * thisObj->super.elementSize);
    if (thisObj->array == NULL) {
      return KBrsrOutOfMemory;
    }
  }
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_ADT_ResizableVector_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_ADT_ResizableVector_t* thisObj;

  /* for convenience */
  thisObj = NW_ADT_ResizableVectorOf (dynamicObject);

  /* release our resources */
  NW_Mem_Free (thisObj->array);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_ResizableVector_ResizeCapacity (NW_ADT_DynamicVector_t* dynamicVector,
                                        NW_ADT_Vector_Metric_t capacityNeeded)
{
  NW_ADT_ResizableVector_t* thisObj;
  NW_ADT_Vector_Metric_t newCapacity;
  void* newArray;

  
  /* for convenience */
  thisObj = NW_ADT_ResizableVectorOf (dynamicVector);

  /* CEI - This protects against underflow.          
           We don't currently support shrinking the array. */
  /*  TODO: support shrinking ? */
  if (capacityNeeded <= thisObj->super.capacity) {
      return KBrsrSuccess;
  }

  /* Note: This calculation could leave super.capacity out, if what we want is
           to maintain capacity as an even multiple of increment, as it appears. */

  /* calculate the size of the new array and allocate it */     
  newCapacity = (NW_ADT_Vector_Metric_t)
    (((capacityNeeded - thisObj->super.capacity - 1) / thisObj->increment + 1)
     * thisObj->increment + thisObj->super.capacity);
  newArray =
    NW_Mem_Malloc (newCapacity * dynamicVector->elementSize);
  if (newArray == NULL) {
    return KBrsrOutOfMemory;
  }
  
  if (thisObj->array) {
    /* copy the contents of the old array into the new */
    (void) NW_Mem_memmove (newArray, thisObj->array,
                           dynamicVector->size * dynamicVector->elementSize);
  }
  
  /* free the old array and set the member variables */
  NW_Mem_Free (thisObj->array);
  thisObj->array = (NW_Uint8*) newArray;
  thisObj->super.capacity = newCapacity;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint8*
_NW_ADT_ResizableVector_AddressAt (const NW_ADT_Vector_t* vector,
                                   NW_ADT_Vector_Metric_t index)
{
  NW_ADT_ResizableVector_t* thisObj;

  /* for convenience */
  thisObj = NW_ADT_ResizableVectorOf (vector);

  /* return the trivial calculation */
  return &thisObj->array[index * thisObj->super.elementSize];
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_ResizableVector_t*
NW_ADT_ResizableVector_New (NW_ADT_Vector_Metric_t elementSize,
                            NW_ADT_Vector_Metric_t initialCapacity,
                            NW_ADT_Vector_Metric_t capacityIncrement)
{
  return (NW_ADT_ResizableVector_t*)
    NW_Object_New (&NW_ADT_ResizableVector_Class, (NW_Uint32) elementSize,
                   (NW_Uint32) initialCapacity, (NW_Uint32) capacityIncrement);
}
