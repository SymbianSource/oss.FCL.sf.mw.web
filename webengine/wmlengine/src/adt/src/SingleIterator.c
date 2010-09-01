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


#include "nw_adt_singleiteratori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_SingleIterator_Class_t NW_ADT_SingleIterator_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_ADT_Iterator_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_ADT_SingleIterator_t),
    /* construct             */ _NW_ADT_SingleIterator_Construct,
    /* destruct              */ NULL
  },
  { /* NW_ADT_Iterator       */
    /* hasMoreElements       */ _NW_ADT_SingleIterator_HasMoreElements,
    /* nextElement           */ _NW_ADT_SingleIterator_NextElement,
    /* getElementSize        */ _NW_ADT_SingleIterator_GetElementSize
  },
  { /* NW_ADT_SingleIterator */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_SingleIterator_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp)
{
  NW_ADT_SingleIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_ADT_SingleIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_SingleIteratorOf (dynamicObject);

  /* initialize the member variables */
  thisObj->element = va_arg (*argp, void*);
  NW_ASSERT (thisObj->element != NULL);
  thisObj->elementSize = (NW_Uint16) va_arg (*argp, NW_Uint32); 

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Int32
_NW_ADT_SingleIterator_HasMoreElements (const NW_ADT_Iterator_t* iterator)
{
  NW_ADT_SingleIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_ADT_SingleIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_SingleIteratorOf (iterator);

  /* is the current index not equal to the 'end' index? */
  return thisObj->element != NULL;
}

/* ------------------------------------------------------------------------- */
void**
_NW_ADT_SingleIterator_NextElement (NW_ADT_Iterator_t* iterator)
{
  NW_ADT_SingleIterator_t* thisObj;
  void** element;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_ADT_SingleIterator_Class));

  /* for convenience */
  thisObj = NW_ADT_SingleIteratorOf (iterator);

  /* if there are no more elements to return, we must return NULL */
  if (!NW_ADT_Iterator_HasMoreElements (iterator)) {
    return NULL;
  }

  /* get the element pointer */
  element = (void**) thisObj->element;
  thisObj->element = NULL;

  /* return the element */
  return element;
}

/* ------------------------------------------------------------------------- */
NW_Uint32
_NW_ADT_SingleIterator_GetElementSize (NW_ADT_Iterator_t* iterator)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator, &NW_ADT_SingleIterator_Class));

  /* return the stored element size */
  return NW_ADT_SingleIteratorOf (iterator)->elementSize;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_SingleIterator_t*
NW_ADT_SingleIterator_New (void* element,
                           NW_Uint16 elementSize)
{
  return (NW_ADT_SingleIterator_t*)
    NW_Object_New (&NW_ADT_SingleIterator_Class, element, (NW_Uint32) elementSize);
}
