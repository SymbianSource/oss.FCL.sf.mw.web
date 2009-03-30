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


#include "nw_text_utf8i.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_UTF8_Class_t NW_Text_UTF8_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_Text_VariableWidth_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_Text_UTF8_t),
    /* construct             */ _NW_Text_Abstract_Construct,
    /* destruct              */ NULL
  },
  { /* NW_Text_Abstract      */
    /* createIterator        */ _NW_Text_VariableWidth_CreateIterator,
    /* getStorageSize        */ _NW_Text_VariableWidth_GetStorageSize,
    /* getCharacterAt        */ _NW_Text_VariableWidth_GetCharacterAt,
    /* getUCS2Buffer         */ _NW_Text_Abstract_GetUCS2Buffer,
    /* getSpaceAfter         */ _NW_Text_UTF8_GetSpaceAfter
  },
  { /* NW_Text_VariableWidth */
    /* readCharacter         */ _NW_Text_UTF8_ReadCharacter
  },
  { /* NW_Text_UTF8          */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   NW_Text_VariableWidth methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_UTF8_ReadCharacter (NW_Text_VariableWidth_t* textVariableWidth,
                             NW_Int32 offset,
                             NW_Ucs4* character,
                             NW_Uint32* numBytes)
{
  /* avoid "unreferenced formal parameter" warnings */
  NW_REQUIRED_PARAM (textVariableWidth);

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (textVariableWidth, &NW_Text_UTF8_Class));
  NW_ASSERT (character != NULL);
  NW_ASSERT (numBytes != NULL);

  return
    NW_Text_UTF8_Decode ((const NW_Uint8*) textVariableWidth->super.storage + offset,
                         character, numBytes);
}

/* ------------------------------------------------------------------------- */
NW_Text_UTF8_t*
NW_Text_UTF8_New (void* storage,
                  NW_Text_Length_t characterCount,
                  NW_Uint16 flags)
{
  NW_Text_UTF8_t*  text;

  text = (NW_Text_UTF8_t*)
    NW_Object_New (&NW_Text_UTF8_Class, storage, characterCount, (NW_Uint32)flags);

  /* if given ownership of storage but failed to create an object, delete the storage */
  if (text == NULL && flags & NW_Text_Flags_TakeOwnership) {
    NW_Mem_Free (storage);
  }

  return text;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Text_UTF8_Initialize (NW_Text_UTF8_t* textUTF8,
                         void* storage,
                         NW_Text_Length_t characterCount,
                         NW_Uint16 flags)
{
  return NW_Object_Initialize (&NW_Text_UTF8_Class, textUTF8, storage,
                               characterCount, (NW_Uint32)flags);
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Text_UTF8_Decode (const NW_Uint8* buffer,
                     NW_Ucs4* character,
                     NW_Uint32* numBytes)
{
  NW_Uint8 mask;

  mask = (NW_Uint8) (buffer[0] >> 4 & 0x0F);
  if (mask < 8) {
    /* 1 byte */
    *character = buffer[0];
    *numBytes = 1;
  } else if (mask == 0x0C || mask == 0x0D) {
    /* 2 bytes */
    if ((buffer[1] & 0xC0) != 0x80) {
      return KBrsrNotFound;
    }
    *character = ((buffer[0] & 0x1F) << 6) | (buffer[1] & 0x3F);
    *numBytes = 2;
  } else if (mask == 0x0E) {
    /* 3 bytes */
    if ((buffer[1] & 0xC0) != 0x80 && (buffer[2] & 0xC0) != 0x80) {
      return KBrsrNotFound;
    }
    *character =
      (buffer[0] & 0x0F) << 12 | ((buffer[1] & 0x3F) << 6) | (buffer[2] & 0x3F);
    *numBytes = 3;
  } else {
    return KBrsrNotFound;
  }

  /* successful completion */
  return KBrsrSuccess;
}

NW_Text_Length_t
_NW_Text_UTF8_GetSpaceAfter (const NW_Text_Abstract_t* abstractText,
                             NW_Text_Length_t index)
{
  NW_Text_Length_t actualIndex = index;
  NW_Uint8* storage;
  NW_Uint32 len;
  NW_ASSERT (NW_Object_IsInstanceOf (abstractText, &NW_Text_UTF8_Class));
  len = abstractText->characterCount;
  storage = (NW_Uint8*) abstractText->storage;
  for (actualIndex = index + 1; actualIndex < len; actualIndex++)
  {
    if (*(storage + actualIndex - 1) < 0x80 &&
      (*(storage + actualIndex) == 0x20 || *(storage + actualIndex) == 0x2E || *(storage + actualIndex) == 0x09))
      return ++actualIndex;
  }
  return actualIndex;
}
