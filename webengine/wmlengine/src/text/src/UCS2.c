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


#include "nw_text_ucs2i.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_UCS2_Class_t NW_Text_UCS2_Class = {
  { /* NW_Object_Core     */
    /* super              */ &NW_Text_FixedWidth_Class,
    /* queryInterface     */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base     */
    /* interfaceList      */ NULL
  },
  { /* NW_Object_Dynamic  */
    /* instanceSize       */ sizeof (NW_Text_UCS2_t),
    /* construct          */ _NW_Text_Abstract_Construct,
    /* destruct           */ NULL
  },
  { /* NW_Text_Abstract   */
    /* createIterator     */ _NW_Text_FixedWidth_CreateIterator,
    /* getStorageSize     */ _NW_Text_FixedWidth_GetStorageSize,
    /* getCharacterAt     */ _NW_Text_UCS2_GetCharacterAt,
    /* getUCS2Buffer      */ _NW_Text_UCS2_GetUCS2Buffer,
    /* getSpaceAfter      */ _NW_Text_UCS2_GetSpaceAfter
  },
  { /* NW_Text_FixedWidth */
    /* characterSize      */ sizeof (NW_Ucs2)
  },
  { /* NW_Text_UCS2       */
    /* unused             */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Ucs4
_NW_Text_UCS2_GetCharacterAt (const NW_Text_Abstract_t* abstractText,
                              NW_Text_Length_t index)
{
  NW_Ucs2 character;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (abstractText, &NW_Text_UCS2_Class));

  /* return the character */
  ((NW_Uint8*) &character)[0] =
    ((NW_Uint8*) abstractText->storage)[index * sizeof (NW_Ucs2)];
  ((NW_Uint8*) &character)[1] =
    ((NW_Uint8*) abstractText->storage)[index * sizeof (NW_Ucs2) + 1];
  return character;
}

/* ------------------------------------------------------------------------- */
NW_Ucs2*
_NW_Text_UCS2_GetUCS2Buffer (const NW_Text_Abstract_t* textAbstract,
                             NW_Uint16 flags,
                             NW_Text_Length_t* charCount,
                             NW_Bool* freeNeeded)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (textAbstract, &NW_Text_UCS2_Class));

  if (flags & NW_Text_Flags_Copy ||
      textAbstract->flags & NW_Text_Flags_CompressWhitespace ||
      flags & NW_Text_Flags_CompressWhitespace ||
      (flags & NW_Text_Flags_Aligned &&
          (unsigned long) textAbstract->storage % sizeof (NW_Ucs2) != 0) ||
      (flags & NW_Text_Flags_NullTerminated &&
        !(textAbstract->flags & NW_Text_Flags_NullTerminated)) ) {
    return _NW_Text_Abstract_GetUCS2Buffer (textAbstract, flags, charCount,
                                            freeNeeded);
  }

  /* we simply return the storage pointer */
  if (charCount != NULL) {
    *charCount = textAbstract->characterCount;
  }
  if (freeNeeded != NULL) {
    *freeNeeded = NW_FALSE;
  }
  return (NW_Ucs2*) textAbstract->storage;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Text_UCS2_t*
NW_Text_UCS2_New (void* storage,
                  NW_Text_Length_t characterCount,
                  NW_Uint16 flags)
{
  NW_Text_UCS2_t*  text;

  text = (NW_Text_UCS2_t*)
    NW_Object_New (&NW_Text_UCS2_Class, storage, characterCount, (NW_Uint32) flags);

  /* if given ownership of storage but failed to create an object, delete the storage */
  if (text == NULL && flags & NW_Text_Flags_TakeOwnership) {
    NW_Mem_Free (storage);
  }

  return text;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Text_UCS2_Initialize (NW_Text_UCS2_t* textUCS2,
                         void* storage,
                         NW_Text_Length_t characterCount,
                         NW_Uint16 flags)
{
  return NW_Object_Initialize (&NW_Text_UCS2_Class, textUCS2, storage,
                               characterCount, (NW_Uint32)flags);
}

/* ------------------------------------------------------------------------- */
NW_Text_Length_t
_NW_Text_UCS2_GetSpaceAfter (const NW_Text_Abstract_t* textUCS2,
                             NW_Text_Length_t index)
{
  NW_Text_Length_t actualIndex = index;
  NW_Uint8* storage;
  NW_Uint32 len;
  NW_ASSERT (NW_Object_IsInstanceOf (textUCS2, &NW_Text_UCS2_Class));
  len = NW_Text_GetCharCount(textUCS2);
  storage = (NW_Uint8*)textUCS2->storage;
  for (actualIndex = index; actualIndex < len; actualIndex++)
  {
    if ((storage[actualIndex * sizeof (NW_Ucs2) + 1] == 0) && 
        ((storage[actualIndex * sizeof (NW_Ucs2)] == 0x20) ||
         (storage[actualIndex * sizeof (NW_Ucs2)] == 0x2E) ||
         (storage[actualIndex * sizeof (NW_Ucs2)] == 0x09))) 
        return ++actualIndex;
  }
  return actualIndex;
}
