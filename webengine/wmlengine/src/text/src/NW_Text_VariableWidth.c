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


#include "NW_Text_VariableWidthI.h"
#include "nw_text_variablewidthiterator.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Text_VariableWidth_Class_t NW_Text_VariableWidth_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_Text_Abstract_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_Text_VariableWidth_t),
    /* construct             */ _NW_Text_Abstract_Construct,
    /* destruct              */ NULL
  },
  { /* NW_Text_Abstract      */
    /* createIterator        */ _NW_Text_VariableWidth_CreateIterator,
    /* getStorageSize        */ _NW_Text_VariableWidth_GetStorageSize,
    /* getCharacterAt        */ _NW_Text_VariableWidth_GetCharacterAt,
    /* getUCS2Buffer         */ _NW_Text_Abstract_GetUCS2Buffer,
    /* getSpaceAfter         */ NULL
  },
  { /* NW_Text_VariableWidth */
    /* readCharacter         */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   NW_Text_Abstract methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Text_VariableWidth_CreateIterator (NW_Text_Abstract_t* abstractText,
                                       NW_Text_Direction_t direction,
                                       NW_Text_Iterator_t** iterator)
{
  *iterator = (NW_Text_Iterator_t*)
    NW_Object_New (&NW_Text_VariableWidthIterator_Class, abstractText,
                   direction);
  if (*iterator == NULL) {
    return KBrsrOutOfMemory;
  }
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint32
_NW_Text_VariableWidth_GetStorageSize (const NW_Text_Abstract_t* abstractText)
{
  NW_Uint32 size;
  NW_Text_Length_t index;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (abstractText,
                                     &NW_Text_VariableWidth_Class));

  for (size = 0, index = 0; index < abstractText->characterCount; index++) {
    TBrowserStatusCode status;
    NW_Ucs4 character;
    NW_Uint32 numBytes;

    status = NW_Text_VariableWidth_ReadCharacter (abstractText, size,
                                                  &character, &numBytes);
    NW_ASSERT (status == KBrsrSuccess);
    /* To fix TI compiler warning */
    (void) status;
    size += numBytes;
  }

  return size;
}

/* ------------------------------------------------------------------------- */
NW_Ucs4
_NW_Text_VariableWidth_GetCharacterAt (const NW_Text_Abstract_t* abstractText,
                                       NW_Text_Length_t index)
{
  NW_Ucs4 character;
  NW_Uint32 size;
  NW_Text_Length_t count;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (abstractText, &NW_Text_VariableWidth_Class));

  /* iterate through the buffer, reading a character at a time until 'index'
     characters have been read */
  character = '\0'; /* this will placate the compiler from complaining about
                       using a variable without an assignment */
  for (size = 0, count = 0; count <= index; count++) {
    TBrowserStatusCode status;
    NW_Uint32 numBytes;

    status = NW_Text_VariableWidth_ReadCharacter (abstractText, size,
                                                  &character, &numBytes);
    NW_ASSERT (status == KBrsrSuccess);
		/* To fix TI compiler warning */
		(void) status;
    size += numBytes;
  }

  return character;
}
