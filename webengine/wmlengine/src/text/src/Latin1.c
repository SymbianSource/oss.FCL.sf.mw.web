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


#include "nw_text_latin1i.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_Latin1_Class_t NW_Text_Latin1_Class = {
  { /* NW_Object_Core     */
    /* super              */ &NW_Text_FixedWidth_Class,
    /* queryInterface     */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base     */
    /* interfaceList      */ NULL
  },
  { /* NW_Object_Dynamic  */
    /* instanceSize       */ sizeof (NW_Text_Latin1_t),
    /* construct          */ _NW_Text_Abstract_Construct,
    /* destruct           */ NULL
  },
  { /* NW_Text_Abstract   */
    /* createIterator     */ _NW_Text_FixedWidth_CreateIterator,
    /* getStorageSize     */ _NW_Text_FixedWidth_GetStorageSize,
    /* getCharacterAt     */ _NW_Text_Latin1_GetCharacterAt,
    /* getUCS2Buffer      */ _NW_Text_Abstract_GetUCS2Buffer,
    /* getSpaceAfter      */ _NW_Text_Latin1_GetSpaceAfter
  },
  { /* NW_Text_FixedWidth */
    /* characterSize      */ sizeof (NW_Uint8)
  },
  { /* NW_Text_Latin1     */
    /* unused             */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Ucs4
_NW_Text_Latin1_GetCharacterAt (const NW_Text_Abstract_t* abstractText,
                                NW_Text_Length_t index)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (abstractText, &NW_Text_Latin1_Class));

  /* return the character */
  return ((NW_Uint8*) abstractText->storage)[index];
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Text_Latin1_t*
NW_Text_Latin1_New (void* storage,
                    NW_Text_Length_t characterCount,
                    NW_Uint16 flags)
{
  NW_Text_Latin1_t*  text;

  text = (NW_Text_Latin1_t*)
    NW_Object_New (&NW_Text_Latin1_Class, storage, characterCount, (NW_Uint32)flags);

  /* if given ownership of storage but failed to create an object, delete the storage */
  if (text == NULL && flags & NW_Text_Flags_TakeOwnership) {
    NW_Mem_Free (storage);
  }

  return text;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Text_Latin1_Initialize (NW_Text_Latin1_t* textLatin1,
                           void* storage,
                           NW_Text_Length_t characterCount,
                           NW_Uint16 flags)
{
  return NW_Object_Initialize (&NW_Text_Latin1_Class, textLatin1, storage,
                               characterCount, (NW_Uint32)flags);
}

/* ------------------------------------------------------------------------- */
NW_Text_Length_t
_NW_Text_Latin1_GetSpaceAfter (const NW_Text_Abstract_t* textLatin1,
                               NW_Text_Length_t index)
{
  NW_Text_Length_t actualIndex = index;
  NW_Uint8* storage;
  NW_Uint32 len;
  NW_ASSERT (NW_Object_IsInstanceOf (textLatin1, &NW_Text_Latin1_Class));
  len = textLatin1->characterCount;
  storage = (NW_Uint8*) textLatin1->storage;
  for (actualIndex = index; actualIndex < len; actualIndex++)
  {
    if (*(storage + actualIndex) == 0x20 || *(storage + actualIndex) == 0x2E || *(storage + actualIndex) == 0x09)
        return ++actualIndex;
  }
  return actualIndex;
}
