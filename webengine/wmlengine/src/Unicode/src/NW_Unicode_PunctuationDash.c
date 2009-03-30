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


#include "NW_Unicode_PunctuationDashI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_PunctuationDash_Table[] = {
  { 0x0000002d, 0x0000002d, 0x00 },
  { 0x000000ad, 0x000000ad, 0x00 },
  { 0x0000058a, 0x0000058a, 0x00 },
  { 0x00001806, 0x00001806, 0x00 },
  { 0x00002010, 0x00002015, 0x01 },
  { 0x0000301c, 0x0000301c, 0x00 },
  { 0x00003030, 0x00003030, 0x00 },
  { 0x0000fe31, 0x0000fe31, 0x00 },
  { 0x0000fe32, 0x0000fe32, 0x00 },
  { 0x0000fe58, 0x0000fe58, 0x00 },
  { 0x0000fe63, 0x0000fe63, 0x00 },
  { 0x0000ff0d, 0x0000ff0d, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationDash_Class_t NW_Unicode_PunctuationDash_Class = {
  { /* NW_Object_Core             */
    /* superclass                 */ &NW_Unicode_Category_Class,
    /* queryInterface             */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category        */
    /* tableSize                  */ sizeof(_NW_Unicode_PunctuationDash_Table)
	                                  / sizeof(*_NW_Unicode_PunctuationDash_Table),
    /* table                      */ _NW_Unicode_PunctuationDash_Table
  },
  { /* NW_Unicode_PunctuationDash */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationDash_t NW_Unicode_PunctuationDash = {
  { { &NW_Unicode_PunctuationDash_Class } }
};
