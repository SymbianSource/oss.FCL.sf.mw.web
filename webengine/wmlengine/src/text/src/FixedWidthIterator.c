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


#include "nw_text_fixedwidthiteratori.h"
#include "NW_Text_FixedWidthI.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_FixedWidthIterator_Class_t NW_Text_FixedWidthIterator_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_Text_Iterator_Class,
    /* querySecondary             */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Base             */
    /* secondaryList              */ NULL
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_Text_FixedWidthIterator_t),
    /* construct                  */ _NW_Text_Iterator_Construct,
    /* destruct                   */ NULL
  },
  { /* NW_Text_Iterator           */
    /* getNext                    */ _NW_Text_FixedWidthIterator_GetNext,
    /* getOffset                  */ _NW_Text_FixedWidthIterator_GetOffset,
    /* setIndex                   */ _NW_Text_FixedWidthIterator_SetIndex
  },
  { /* NW_Text_FixedWidthIterator */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   NW_Text_Iterator methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_FixedWidthIterator_GetNext (NW_Text_Iterator_t* iterator,
                                     NW_Ucs4* character)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator,
                                     &NW_Text_FixedWidthIterator_Class));

  NW_TRY (status) {
    /* ensure that there are more characters to get */
    status = NW_Text_Iterator_HasMore (iterator);
    if (status == KBrsrIterateDone) {
      NW_THROW (status);
    }
    NW_THROW_ON_ERROR (status);

    /* extract the character */
    *character = NW_Text_GetAt (iterator->text, iterator->index);
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
_NW_Text_FixedWidthIterator_GetOffset (NW_Text_Iterator_t* iterator,
                                       NW_Uint32* offset)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator,
                                     &NW_Text_FixedWidthIterator_Class));

  *offset =
    NW_Text_FixedWidth_GetCharacterSize (iterator->text) * iterator->index;
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_FixedWidthIterator_SetIndex (NW_Text_Iterator_t* iterator,
                                      NW_Text_Length_t index)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (iterator,
                                     &NW_Text_FixedWidthIterator_Class));

  /* update the index */
  iterator->index = index;

  /* successful completion */
  return KBrsrSuccess;
}
