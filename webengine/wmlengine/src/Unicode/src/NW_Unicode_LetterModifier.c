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


#include "NW_Unicode_LetterModifierI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_LetterModifier_Table[] = {
  { 0x000002b0, 0x000002b8, 0x01 },
  { 0x000002bb, 0x000002c1, 0x01 },
  { 0x000002d0, 0x000002d0, 0x00 },
  { 0x000002d1, 0x000002d1, 0x00 },
  { 0x000002e0, 0x000002e4, 0x01 },
  { 0x000002ee, 0x000002ee, 0x00 },
  { 0x0000037a, 0x0000037a, 0x00 },
  { 0x00000559, 0x00000559, 0x00 },
  { 0x00000640, 0x00000640, 0x00 },
  { 0x000006e5, 0x000006e5, 0x00 },
  { 0x000006e6, 0x000006e6, 0x00 },
  { 0x00000e46, 0x00000e46, 0x00 },
  { 0x00000ec6, 0x00000ec6, 0x00 },
  { 0x00001843, 0x00001843, 0x00 },
  { 0x00003005, 0x00003005, 0x00 },
  { 0x00003031, 0x00003035, 0x01 },
  { 0x0000309d, 0x0000309d, 0x00 },
  { 0x0000309e, 0x0000309e, 0x00 },
  { 0x000030fc, 0x000030fe, 0x01 },
  { 0x0000ff70, 0x0000ff70, 0x00 },
  { 0x0000ff9e, 0x0000ff9e, 0x00 },
  { 0x0000ff9f, 0x0000ff9f, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_LetterModifier_Class_t NW_Unicode_LetterModifier_Class = {
  { /* NW_Object_Core            */
    /* superclass                */ &NW_Unicode_Category_Class,
    /* queryInterface            */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category       */
    /* tableSize                 */ sizeof(_NW_Unicode_LetterModifier_Table)
	                                  / sizeof(*_NW_Unicode_LetterModifier_Table),
    /* table                     */ _NW_Unicode_LetterModifier_Table
  },
  { /* NW_Unicode_LetterModifier */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_LetterModifier_t NW_Unicode_LetterModifier = {
  { { &NW_Unicode_LetterModifier_Class } }
};
