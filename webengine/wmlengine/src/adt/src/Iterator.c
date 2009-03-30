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


#include "nw_adt_iteratori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_ADT_Iterator_Class_t NW_ADT_Iterator_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Object_Dynamic_Class,
    /* queryInterface    */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base    */
    /* interfaceList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_ADT_Iterator_t),
    /* construct         */ NULL,
    /* destruct          */ NULL
  },
  { /* NW_ADT_Iterator   */
    /* hasMoreElements   */ NULL,
    /* nextElement       */ NULL,
    /* getElementSize    */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_Iterator_GetNextElement (NW_ADT_Iterator_t* iterator,
                                 void* element)
{
  void* location;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_ADT_Iterator_Class));

  /* get the location for the next element */
  location = NW_ADT_Iterator_NextElement (iterator);
  if (location == NULL) {
    return KBrsrNotFound;
  }

  /* copy the contents into the supplied buffer */
  (void) NW_Mem_memcpy (element, location,
                        NW_ADT_Iterator_GetElementSize (iterator));

  /* successful completion */
  return KBrsrSuccess;
}
