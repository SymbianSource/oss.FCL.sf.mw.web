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


#include "nw_adt_fixedvectori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_FixedVector_Class_t NW_ADT_FixedVector_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_ADT_DynamicVector_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		 */ sizeof (NW_ADT_FixedVector_t),
    /* construct		 */ _NW_ADT_DynamicVector_Construct,
    /* destruct			 */ NULL
  },
  { /* NW_ADT_Vector		 */
    /* getElementSize            */ _NW_ADT_DynamicVector_GetElementSize,
    /* getSize                   */ _NW_ADT_DynamicVector_GetSize,
    /* addressAt		 */ _NW_ADT_FixedVector_AddressAt
  },
  { /* NW_ADT_DynamicVector      */
    /* resizeCapacity            */ _NW_ADT_FixedVector_ResizeCapacity,
    /* moveElements		 */ _NW_ADT_DynamicVector_MoveElements,
    /* clear                     */ _NW_ADT_FixedVector_Clear
  },
  { /* NW_ADT_FixedVector	 */
    /* unused                    */ 0
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Uint8*
_NW_ADT_FixedVector_AddressAt (const NW_ADT_Vector_t* vector,
                               NW_ADT_Vector_Metric_t index)
{
  NW_ADT_FixedVector_t* thisObj;

  /* for convenience */
  thisObj = NW_ADT_FixedVectorOf (vector);

  /*lint -e{573} Signed-unsigned mix with divide */
  return (NW_Uint8*) thisObj
    + index * thisObj->super.elementSize
    + sizeof (NW_Int32)
    * ((NW_Object_Dynamic_GetClassPart (thisObj).instanceSize - 1)
       / sizeof (NW_Int32) + 1);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_FixedVector_ResizeCapacity (NW_ADT_DynamicVector_t* dynamicVector,
                                    NW_ADT_Vector_Metric_t capacityNeeded)
{
  /* the FixedVector will never resize its capacity, so if the requested
     capacity is greater than what we've got we must return an error */
  if (capacityNeeded > dynamicVector->capacity) {
    return KBrsrFailure; /* TODO: return a more descriptive error code */
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_FixedVector_Clear (NW_ADT_DynamicVector_t* dynamicVector)
{
  /* set the size to zero */
  dynamicVector->size = 0;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_FixedVector_t*
NW_ADT_FixedVector_New (NW_ADT_Vector_Metric_t elementSize,
                        NW_ADT_Vector_Metric_t capacity)
{
  NW_ADT_FixedVector_t* fixedVector;
  TBrowserStatusCode status;

  /* allocate and initialize the object */
  /*lint -e{573} Signed-unsigned mix with divide */
  fixedVector = (NW_ADT_FixedVector_t*)
    NW_Mem_Malloc (capacity * elementSize + sizeof (NW_Int32) * 
                   ((NW_ADT_FixedVector_Class.
                       NW_Object_Dynamic.instanceSize - 1)
                    / sizeof (NW_Int32) + 1));
  if (fixedVector == NULL) {
    return NULL;
  }
  status = NW_Object_Initialize (&NW_ADT_FixedVector_Class, fixedVector,
                                 elementSize, capacity);
  if (status != KBrsrSuccess) {
    NW_Mem_Free (fixedVector);
    return NULL;
  }

  /* successful completion */
  return fixedVector;
}
