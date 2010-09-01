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


#include "nw_text_iteratori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_Iterator_Class_t NW_Text_Iterator_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Object_Dynamic_Class,
    /* querySecondary    */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Base    */
    /* secondaryList     */ NULL
  },
  { /* NW_Object_Dynamic */
    /* instanceSize      */ sizeof (NW_Text_Iterator_t),
    /* construct         */ _NW_Text_Iterator_Construct,
    /* destruct          */ NULL
  },
  { /* NW_Text_Iterator  */
    /* getNext           */ NULL,
    /* getOffset         */ NULL,
    /* setIndex          */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   NW_Object_Dynamic methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_Iterator_Construct (NW_Object_Dynamic_t* dynamic,
                             va_list* argList)
{
  NW_Text_Iterator_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamic, &NW_Text_Iterator_Class));
  NW_ASSERT (argList != NULL);

  /* for convenience */
  thisObj = NW_Text_IteratorOf (dynamic);

  /* initialize the member variables */
  thisObj->text = va_arg (*argList, NW_Text_Abstract_t*);
  thisObj->direction = va_arg (*argList, NW_Text_Direction_t);

  /* validate the input parameters */
  if (thisObj->text == NULL ||
      (thisObj->direction != NW_Text_Direction_Forward &&
      thisObj->direction != NW_Text_Direction_Backward)) {
    return KBrsrBadInputParam;
  }

  /* adjust the index if we are a reverse iterator */
  if (thisObj->direction == NW_Text_Direction_Backward) {
    thisObj->index =
      (NW_Text_Length_t) (NW_Text_GetCharCount (thisObj->text) - 1);
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   NW_Text_Iterator methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_Iterator_HasMore (NW_Text_Iterator_t* thisObj)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Text_Iterator_Class));

  /* check to see if we've run out of entries */
  if (thisObj->index < NW_Text_GetCharCount (thisObj->text)) {
    return KBrsrSuccess;
  }

  /* no more entries */
  return KBrsrIterateDone;
}

