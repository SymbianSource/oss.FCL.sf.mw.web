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


#include "nw_adt_vectoriteratori.h"

#include "nw_adt_dynamicvector.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_VectorIterator_Class_t NW_ADT_VectorIterator_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_ADT_Iterator_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_ADT_VectorIterator_t),
    /* construct             */ _NW_ADT_VectorIterator_Construct,
    /* destruct              */ NULL
  },
  { /* NW_ADT_Iterator       */
    /* hasMoreElements       */ _NW_ADT_VectorIterator_HasMoreElements,
    /* nextElement           */ _NW_ADT_VectorIterator_NextElement,
    /* getElementSize        */ _NW_ADT_VectorIterator_GetElementSize
  },
  { /* NW_ADT_VectorIterator */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_VectorIterator_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp)
{
  NW_ADT_VectorIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_ADT_VectorIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_VectorIteratorOf (dynamicObject);

  /* initialize the member variables */
  thisObj->vector = va_arg (*argp, const NW_ADT_Vector_t*);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj->vector, &NW_ADT_Vector_Class));

  thisObj->index = (NW_ADT_Vector_Metric_t) va_arg (*argp, NW_Uint32);
  if (thisObj->index == NW_ADT_Vector_AtEnd) {
    thisObj->index = NW_ADT_Vector_GetSize (thisObj->vector);
  }
  NW_ASSERT (thisObj->index <= NW_ADT_Vector_GetSize (thisObj->vector));

  thisObj->end = (NW_ADT_Vector_Metric_t) va_arg (*argp, NW_Uint32);
  if (thisObj->end == NW_ADT_Vector_AtEnd) {
    thisObj->end = NW_ADT_Vector_GetSize (thisObj->vector);
  }
  NW_ASSERT (thisObj->end <= NW_ADT_Vector_GetSize (thisObj->vector));

  thisObj->direction = va_arg (*argp, NW_ADT_IteratorDirection_t);
  NW_ASSERT ( (thisObj->direction == NW_ADT_IteratorDirection_Increment &&
               thisObj->index <= thisObj->end) ||
              (thisObj->direction == NW_ADT_IteratorDirection_Decrement &&
               thisObj->index >= thisObj->end) );

  /* 
  ** TODO Warning! NW_ADT_IteratorDirection_Decrement doesn't work.  There are
  **      are bugs in the edge cases for Vector and NW_ADT_VectorIterator_HasMoreElements
  **      do not work correctly when iterating backwards.
  */
  NW_ASSERT (thisObj->direction != NW_ADT_IteratorDirection_Decrement);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Int32
_NW_ADT_VectorIterator_HasMoreElements (const NW_ADT_Iterator_t* iterator)
{
  NW_ADT_VectorIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_ADT_VectorIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_VectorIteratorOf (iterator);

  /* is the current index not equal to the 'end' index? */
  return thisObj->index != thisObj->end;
}

/* ------------------------------------------------------------------------- */
void**
_NW_ADT_VectorIterator_NextElement (NW_ADT_Iterator_t* iterator)
{
  NW_ADT_VectorIterator_t* thisObj;
  void** element;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_ADT_VectorIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_VectorIteratorOf (iterator);

  /* if there are no more elements to return, we must return NULL */
  if (!NW_ADT_Iterator_HasMoreElements (iterator)) {
    return NULL;
  }

  /* get the element pointer */
  element = NW_ADT_Vector_ElementAt (thisObj->vector, thisObj->index);

  /* advance the index by the direction delta */
  thisObj->index = (NW_ADT_Vector_Metric_t) (thisObj->index + thisObj->direction);

  /* return the element */
  return element;
}

/* ------------------------------------------------------------------------- */
NW_Uint32
_NW_ADT_VectorIterator_GetElementSize (NW_ADT_Iterator_t* iterator)
{
  NW_ADT_VectorIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_ADT_VectorIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_VectorIteratorOf (iterator);

  return NW_ADT_Vector_GetElementSize (thisObj->vector);
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_VectorIterator_t*
_NW_ADT_VectorIterator_Initialize (NW_ADT_VectorIterator_t* vectorIterator,
                                   const NW_ADT_Vector_t* vector,
                                   NW_ADT_Vector_Metric_t start,
                                   NW_ADT_Vector_Metric_t end,
                                   NW_ADT_IteratorDirection_t direction)
{
  return (NW_ADT_VectorIterator_t*)
    NW_Object_Initialize (&NW_ADT_VectorIterator_Class, vectorIterator,
                          vector, start, end, direction);
}

/* ------------------------------------------------------------------------- */
NW_ADT_VectorIterator_t*
_NW_ADT_VectorIterator_New (const NW_ADT_Vector_t* vector,
                            NW_ADT_Vector_Metric_t start,
                            NW_ADT_Vector_Metric_t end,
                            NW_ADT_IteratorDirection_t direction)
{
  return (NW_ADT_VectorIterator_t*)
    NW_Object_New (&NW_ADT_VectorIterator_Class, vector, (NW_Uint32)start, (NW_Uint32)end,
                   direction);
}
