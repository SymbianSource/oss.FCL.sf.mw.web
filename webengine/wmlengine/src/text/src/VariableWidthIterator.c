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


#include "nw_text_variablewidthiteratori.h"
#include "NW_Text_VariableWidthI.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Text_VariableWidthIterator_StepBack (NW_Text_VariableWidthIterator_t* iterator)
{
  char* storage;
  NW_Int32 delta;

  storage = (char*) NW_Text_Abstract_GetStorage (iterator->super.text);
  NW_ASSERT (storage != NULL);

  for (delta = -1; delta > -4 && (storage[delta] & 0xC0) == 0x80; delta--) {
    /* Do nothing */
  }

  if (delta < -1 && (storage[delta] & 0xC0) != 0xC0) {
    return KBrsrUnexpectedError;
  }

  iterator->offset -= delta;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_VariableWidthIterator_Class_t NW_Text_VariableWidthIterator_Class = {
  { /* NW_Object_Core                */
    /* super                         */ &NW_Text_Iterator_Class,
    /* querySecondary                */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Base                */
    /* secondaryList                 */ NULL
  },
  { /* NW_Object_Dynamic             */
    /* instanceSize                  */ sizeof (NW_Text_VariableWidthIterator_t),
    /* construct                     */ _NW_Text_VariableWidthIterator_Construct,
    /* destruct                      */ NULL
  },
  { /* NW_Text_Iterator              */
    /* getNext                       */ _NW_Text_VariableWidthIterator_GetNext,
    /* getOffset                     */ _NW_Text_VariableWidthIterator_GetOffset,
    /* setIndex                      */ _NW_Text_VariableWidthIterator_SetIndex
  },
  { /* NW_Text_VariableWidthIterator */
    /* unused                        */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_VariableWidthIterator_Construct (NW_Object_Dynamic_t* dynamic,
                                          va_list* argList)
{
  NW_Text_VariableWidthIterator_t* thisObj;
  TBrowserStatusCode status;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamic,
                                     &NW_Text_VariableWidthIterator_Class));
  NW_ASSERT (argList != NULL);

  /* invoke our superclass constructor */
  status = _NW_Text_Iterator_Construct (dynamic, argList);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* for convenience */
  thisObj = NW_Text_VariableWidthIteratorOf (dynamic);

  /* initialize the member variables */
  if (thisObj->super.direction == NW_Text_Direction_Backward) {
    thisObj->offset = NW_Text_Abstract_GetStorageSize (thisObj->super.text);
    status = NW_Text_VariableWidthIterator_StepBack (thisObj);
    if (status != KBrsrSuccess) {
      return KBrsrUnexpectedError;
    }
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   NW_Text_Iterator methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_VariableWidthIterator_GetNext (NW_Text_Iterator_t* iterator,
                                        NW_Ucs4* character)
{
  NW_Text_VariableWidthIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator,
                                     &NW_Text_VariableWidthIterator_Class));

  /* for convenience */
  thisObj = NW_Text_VariableWidthIteratorOf (iterator);

  NW_TRY (status) {
    NW_Uint32 numBytes;

    /* ensure that there are more characters to get */
    status = NW_Text_Iterator_HasMore (iterator);
    if (status == KBrsrIterateDone) {
      NW_THROW (status);
    }
    NW_THROW_ON_ERROR (status);

    /* get the character */
    status =
      NW_Text_VariableWidth_ReadCharacter (iterator->text, thisObj->offset,
                                           character, &numBytes);
    NW_THROW_ON_ERROR (status);

    /* adjust the pointer and index */
    if (iterator->direction == NW_Text_Direction_Forward) {
      thisObj->offset += numBytes;
    } else if (iterator->index > 0) {
      status = NW_Text_VariableWidthIterator_StepBack (thisObj);
      NW_THROW_ON_ERROR (status);
    }
    iterator->index =
      (NW_Text_Length_t) (iterator->index + iterator->direction);
  }

  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_VariableWidthIterator_GetOffset (NW_Text_Iterator_t* iterator,
                                          NW_Uint32* offset)
{
  NW_Text_VariableWidthIterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator,
                                     &NW_Text_VariableWidthIterator_Class));

  /* for convenience */
  thisObj = NW_Text_VariableWidthIteratorOf (iterator);

  /* set the offset */
  *offset = thisObj->offset;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_VariableWidthIterator_SetIndex (NW_Text_Iterator_t* iterator,
                                         NW_Text_Length_t index)
{
  NW_Text_Direction_t oldDirection;
  
  oldDirection = iterator->direction;
  NW_TRY (status) {
    NW_Ucs4 character;

    iterator->direction = NW_Text_Direction_Forward;
    while (index > iterator->index) {
      status = NW_Text_Iterator_GetNext (iterator, &character);
      NW_THROW_ON_ERROR (status);
    }

    iterator->direction = NW_Text_Direction_Backward;
    while (index < iterator->index) {
      status = NW_Text_Iterator_GetNext (iterator, &character);
      NW_THROW_ON_ERROR (status);
    }
  }

  NW_CATCH (status) { /* finally */ }

  NW_FINALLY {
    iterator->direction = oldDirection;
    return status;
  } NW_END_TRY
}
