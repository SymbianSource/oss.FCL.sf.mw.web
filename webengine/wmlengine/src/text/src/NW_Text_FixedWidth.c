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


#include "NW_Text_FixedWidthI.h"
#include "nw_text_fixedwidthiterator.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Text_FixedWidth_Class_t NW_Text_FixedWidth_Class = {
  { /* NW_Object_Core     */
    /* super              */ &NW_Text_Abstract_Class,
    /* queryInterface     */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base     */
    /* interfaceList      */ NULL
  },
  { /* NW_Object_Dynamic  */
    /* instanceSize       */ sizeof (NW_Text_FixedWidth_t),
    /* construct          */ _NW_Text_Abstract_Construct,
    /* destruct           */ NULL
  },
  { /* NW_Text_Abstract   */
    /* createIterator     */ _NW_Text_FixedWidth_CreateIterator,
    /* getStorageSize     */ _NW_Text_FixedWidth_GetStorageSize,
    /* getCharacterAt     */ NULL,
    /* getUCS2Buffer      */ NULL,
    /* getSpaceAfter      */ NULL
  },
  { /* NW_Text_FixedWidth */
    /* characterSize      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   NW_Text_Abstract methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_FixedWidth_CreateIterator (NW_Text_Abstract_t* abstractText,
                                    NW_Text_Direction_t direction,
                                    NW_Text_Iterator_t** iterator)
{
  *iterator = (NW_Text_Iterator_t*)
    NW_Object_New (&NW_Text_FixedWidthIterator_Class, abstractText, direction);
  if (*iterator == NULL) {
    return KBrsrOutOfMemory;
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint32
_NW_Text_FixedWidth_GetStorageSize (const NW_Text_Abstract_t* abstractText)
{
  return abstractText->characterCount *
           NW_Text_FixedWidth_GetCharacterSize (abstractText);
}
