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


#include "nw_hed_childiteratori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_ChildIterator_Class_t NW_HED_ChildIterator_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_ADT_MapIterator_Class,
    /* querySecondary        */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Base        */
    /* secondaryList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_HED_ChildIterator_t),
    /* construct             */ _NW_ADT_MapIterator_Construct,
    /* destruct              */ NULL
  },
  { /* NW_ADT_Iterator       */
    /* hasMoreElements       */ _NW_ADT_VectorIterator_HasMoreElements,
    /* nextElement           */ _NW_HED_ChildIterator_NextElement,
    /* getElementSize        */ _NW_HED_ChildIterator_GetElementSize
  },
  { /* NW_ADT_VectorIterator */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_ADT_MapIterator    */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_HED_ChildIterator  */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   NW_ADT_Iterator methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
void**
_NW_HED_ChildIterator_NextElement (NW_ADT_Iterator_t* iterator)
{
  void** entry;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_HED_ChildIterator_Class));

  /* get the next entry from the iterator */
  entry = NW_ADT_MapIterator_Class.NW_ADT_Iterator.nextElement (iterator);
  if (entry == NULL) {
    return NULL;
  }

  /* we know that the map is an array of packed pointers */
  return &entry[1];
}

/* ------------------------------------------------------------------------- */
NW_Uint32
_NW_HED_ChildIterator_GetElementSize (NW_ADT_Iterator_t* iterator)
{
  NW_ADT_MapIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_HED_ChildIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_MapIteratorOf (iterator);

  return NW_ADT_Map_GetValueSize (thisObj->map);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_ChildIterator_Initialize (NW_HED_ChildIterator_t* iterator,
                                  NW_ADT_Map_t* children)
{
  return
    NW_Object_Initialize (&NW_HED_ChildIterator_Class, iterator,
                          children->vector, 0, NW_ADT_Vector_AtEnd,
                          NW_ADT_IteratorDirection_Increment, children);
}

/* ------------------------------------------------------------------------- */
NW_HED_ChildIterator_t*
_NW_HED_ChildIterator_New (NW_ADT_Map_t* children)
{
  return (NW_HED_ChildIterator_t*)
    NW_Object_New (&NW_HED_ChildIterator_Class, children->vector, 0,
                   NW_ADT_Vector_AtEnd, NW_ADT_IteratorDirection_Increment,
                   children);
}
