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


#include "NW_Unicode_PunctuationOtherI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_PunctuationOther_Table[] = {
  { 0x00000021, 0x00000023, 0x01 },
  { 0x00000025, 0x00000027, 0x01 },
  { 0x0000002a, 0x0000002e, 0x02 },
  { 0x0000002f, 0x0000002f, 0x00 },
  { 0x0000003a, 0x0000003a, 0x00 },
  { 0x0000003b, 0x0000003b, 0x00 },
  { 0x0000003f, 0x0000003f, 0x00 },
  { 0x00000040, 0x00000040, 0x00 },
  { 0x0000005c, 0x0000005c, 0x00 },
  { 0x000000a1, 0x000000a1, 0x00 },
  { 0x000000b7, 0x000000b7, 0x00 },
  { 0x000000bf, 0x000000bf, 0x00 },
  { 0x0000037e, 0x0000037e, 0x00 },
  { 0x00000387, 0x00000387, 0x00 },
  { 0x0000055a, 0x0000055f, 0x01 },
  { 0x00000589, 0x00000589, 0x00 },
  { 0x000005be, 0x000005be, 0x00 },
  { 0x000005c0, 0x000005c0, 0x00 },
  { 0x000005c3, 0x000005c3, 0x00 },
  { 0x000005f3, 0x000005f3, 0x00 },
  { 0x000005f4, 0x000005f4, 0x00 },
  { 0x0000060c, 0x0000060c, 0x00 },
  { 0x0000061b, 0x0000061b, 0x00 },
  { 0x0000061f, 0x0000061f, 0x00 },
  { 0x0000066a, 0x0000066d, 0x01 },
  { 0x000006d4, 0x000006d4, 0x00 },
  { 0x00000700, 0x0000070d, 0x01 },
  { 0x00000964, 0x00000964, 0x00 },
  { 0x00000965, 0x00000965, 0x00 },
  { 0x00000970, 0x00000970, 0x00 },
  { 0x00000df4, 0x00000df4, 0x00 },
  { 0x00000e4f, 0x00000e4f, 0x00 },
  { 0x00000e5a, 0x00000e5a, 0x00 },
  { 0x00000e5b, 0x00000e5b, 0x00 },
  { 0x00000f04, 0x00000f12, 0x01 },
  { 0x00000f85, 0x00000f85, 0x00 },
  { 0x0000104a, 0x0000104f, 0x01 },
  { 0x000010fb, 0x000010fb, 0x00 },
  { 0x00001361, 0x00001368, 0x01 },
  { 0x0000166d, 0x0000166d, 0x00 },
  { 0x0000166e, 0x0000166e, 0x00 },
  { 0x000016eb, 0x000016ed, 0x01 },
  { 0x000017d4, 0x000017da, 0x01 },
  { 0x000017dc, 0x000017dc, 0x00 },
  { 0x00001800, 0x00001805, 0x01 },
  { 0x00001807, 0x0000180a, 0x01 },
  { 0x00002016, 0x00002016, 0x00 },
  { 0x00002017, 0x00002017, 0x00 },
  { 0x00002020, 0x00002027, 0x01 },
  { 0x00002030, 0x00002038, 0x01 },
  { 0x0000203b, 0x0000203e, 0x01 },
  { 0x00002041, 0x00002043, 0x01 },
  { 0x00002048, 0x0000204d, 0x01 },
  { 0x00003001, 0x00003003, 0x01 },
  { 0x0000fe30, 0x0000fe30, 0x00 },
  { 0x0000fe49, 0x0000fe4c, 0x01 },
  { 0x0000fe50, 0x0000fe52, 0x01 },
  { 0x0000fe54, 0x0000fe57, 0x01 },
  { 0x0000fe5f, 0x0000fe61, 0x01 },
  { 0x0000fe68, 0x0000fe68, 0x00 },
  { 0x0000fe6a, 0x0000fe6a, 0x00 },
  { 0x0000fe6b, 0x0000fe6b, 0x00 },
  { 0x0000ff01, 0x0000ff03, 0x01 },
  { 0x0000ff05, 0x0000ff07, 0x01 },
  { 0x0000ff0a, 0x0000ff0e, 0x02 },
  { 0x0000ff0f, 0x0000ff0f, 0x00 },
  { 0x0000ff1a, 0x0000ff1a, 0x00 },
  { 0x0000ff1b, 0x0000ff1b, 0x00 },
  { 0x0000ff1f, 0x0000ff1f, 0x00 },
  { 0x0000ff20, 0x0000ff20, 0x00 },
  { 0x0000ff3c, 0x0000ff3c, 0x00 },
  { 0x0000ff61, 0x0000ff61, 0x00 },
  { 0x0000ff64, 0x0000ff64, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationOther_Class_t NW_Unicode_PunctuationOther_Class = {
  { /* NW_Object_Core              */
    /* superclass                  */ &NW_Unicode_Category_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category         */
    /* tableSize                   */ sizeof(_NW_Unicode_PunctuationOther_Table)
	                                  / sizeof(*_NW_Unicode_PunctuationOther_Table),
    /* table                       */ _NW_Unicode_PunctuationOther_Table
  },
  { /* NW_Unicode_PunctuationOther */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationOther_t NW_Unicode_PunctuationOther = {
  { { &NW_Unicode_PunctuationOther_Class } }
};
