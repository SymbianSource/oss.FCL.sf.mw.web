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


#include "nw_adt_mapiteratori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_MapIterator_Class_t NW_ADT_MapIterator_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_ADT_VectorIterator_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_ADT_MapIterator_t),
    /* construct             */ _NW_ADT_MapIterator_Construct,
    /* destruct              */ NULL
  },
  { /* NW_ADT_Iterator       */
    /* hasMoreElements       */ _NW_ADT_VectorIterator_HasMoreElements,
    /* nextElement           */ _NW_ADT_VectorIterator_NextElement,
    /* getElementSize        */ _NW_ADT_MapIterator_GetElementSize
  },
  { /* NW_ADT_VectorIterator */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_ADT_MapIterator    */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   NW_ADT_MapIterator methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_MapIterator_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argList)
{
  NW_ADT_MapIterator_t* thisObj;
  TBrowserStatusCode status;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_ADT_MapIterator_Class));

  /* call out superclass method */
  status = NW_ADT_VectorIterator_Class.NW_Object_Dynamic.
    construct (dynamicObject, argList);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* for convenience */
  thisObj = NW_ADT_MapIteratorOf (dynamicObject);

  /* initialize our member variables */
  thisObj->map = va_arg (*argList, NW_ADT_Map_t*);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint32
_NW_ADT_MapIterator_GetElementSize (NW_ADT_Iterator_t* iterator)
{
  NW_ADT_MapIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_ADT_MapIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_MapIteratorOf (iterator);

  return NW_ADT_Map_GetKeySize (thisObj->map);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_MapIterator_Initialize (NW_ADT_MapIterator_t* iterator,
                               const NW_ADT_Map_t* map)
{
  return
    NW_Object_Initialize (&NW_ADT_MapIterator_Class, iterator, map->vector, 0,
                          NW_ADT_Vector_AtEnd,
                          NW_ADT_IteratorDirection_Increment, map);
}

/* ------------------------------------------------------------------------- */
NW_ADT_MapIterator_t*
NW_ADT_MapIterator_New (const NW_ADT_Map_t* map)
{
  return (NW_ADT_MapIterator_t*)
    NW_Object_New (&NW_ADT_MapIterator_Class, map->vector, 0,
                   NW_ADT_Vector_AtEnd, NW_ADT_IteratorDirection_Increment,
                   map);
}
