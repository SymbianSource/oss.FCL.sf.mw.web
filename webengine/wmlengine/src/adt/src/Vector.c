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


#include "nw_adt_vectori.h"

#include "nw_adt_dynamicvector.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_ADT_Vector_Class_t NW_ADT_Vector_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Dynamic_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize		 */ sizeof (NW_ADT_Vector_t),
    /* construct		 */ NULL,
    /* destruct			 */ NULL
  },
  { /* NW_ADT_Vector		 */
    /* getElementSize            */ NULL,
    /* getSize                   */ NULL,
    /* addressAt		 */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void**
_NW_ADT_Vector_ElementAt (const NW_ADT_Vector_t* vector,
			  NW_ADT_Vector_Metric_t index)
{
  if (index >= NW_ADT_Vector_GetSize (vector)) {
    return NULL;
  }
  return (void**) NW_ADT_Vector_AddressAt (vector, index);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_Vector_GetElementAt (const NW_ADT_Vector_t* vector,
			     NW_ADT_Vector_Metric_t index,
                             void* element)
{
  void* location;

  location = NW_ADT_Vector_ElementAt (vector, index);
  if (location == NULL) {
    return KBrsrNotFound;
  }
  (void) NW_Mem_memcpy (element, location,
                        NW_ADT_Vector_GetElementSize (vector));
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_ADT_Vector_Metric_t
_NW_ADT_Vector_GetElementIndex (const NW_ADT_Vector_t* vector,
                                void* target)
{
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t elementSize;
  NW_ADT_Vector_Metric_t numElements;

  /* cache the elementSize for efficiency */
  elementSize = NW_ADT_Vector_GetElementSize (vector);

  /* iterate throught the vector one by one */
  numElements = NW_ADT_Vector_GetSize (vector);
  for (index = 0; index < numElements; index++) {
    void* element;

    /* get and compare the element */
    element = NW_ADT_Vector_ElementAt (vector, index);
    if (NW_Mem_memcmp (target, element, elementSize) == 0) {
      return index;
    }
  }

  /* no match found, return NW_ADT_Vector_AtEnd */
  return NW_ADT_Vector_AtEnd;
}
