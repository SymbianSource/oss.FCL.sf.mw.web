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


#include "NW_Unicode_MarkSpacingCombiningI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_MarkSpacingCombining_Table[] = {
  { 0x00000903, 0x00000903, 0x00 },
  { 0x0000093e, 0x00000940, 0x01 },
  { 0x00000949, 0x0000094c, 0x01 },
  { 0x00000982, 0x00000982, 0x00 },
  { 0x00000983, 0x00000983, 0x00 },
  { 0x000009be, 0x000009c0, 0x01 },
  { 0x000009c7, 0x000009c7, 0x00 },
  { 0x000009c8, 0x000009c8, 0x00 },
  { 0x000009cb, 0x000009cb, 0x00 },
  { 0x000009cc, 0x000009cc, 0x00 },
  { 0x000009d7, 0x000009d7, 0x00 },
  { 0x00000a3e, 0x00000a40, 0x01 },
  { 0x00000a83, 0x00000a83, 0x00 },
  { 0x00000abe, 0x00000ac0, 0x01 },
  { 0x00000ac9, 0x00000ac9, 0x00 },
  { 0x00000acb, 0x00000acb, 0x00 },
  { 0x00000acc, 0x00000acc, 0x00 },
  { 0x00000b02, 0x00000b02, 0x00 },
  { 0x00000b03, 0x00000b03, 0x00 },
  { 0x00000b3e, 0x00000b3e, 0x00 },
  { 0x00000b40, 0x00000b40, 0x00 },
  { 0x00000b47, 0x00000b47, 0x00 },
  { 0x00000b48, 0x00000b48, 0x00 },
  { 0x00000b4b, 0x00000b4b, 0x00 },
  { 0x00000b4c, 0x00000b4c, 0x00 },
  { 0x00000b57, 0x00000b57, 0x00 },
  { 0x00000b83, 0x00000b83, 0x00 },
  { 0x00000bbe, 0x00000bbe, 0x00 },
  { 0x00000bbf, 0x00000bbf, 0x00 },
  { 0x00000bc1, 0x00000bc1, 0x00 },
  { 0x00000bc2, 0x00000bc2, 0x00 },
  { 0x00000bc6, 0x00000bc8, 0x01 },
  { 0x00000bca, 0x00000bcc, 0x01 },
  { 0x00000bd7, 0x00000bd7, 0x00 },
  { 0x00000c01, 0x00000c03, 0x01 },
  { 0x00000c41, 0x00000c44, 0x01 },
  { 0x00000c82, 0x00000c82, 0x00 },
  { 0x00000c83, 0x00000c83, 0x00 },
  { 0x00000cbe, 0x00000cbe, 0x00 },
  { 0x00000cc0, 0x00000cc4, 0x01 },
  { 0x00000cc7, 0x00000cc7, 0x00 },
  { 0x00000cc8, 0x00000cc8, 0x00 },
  { 0x00000cca, 0x00000cca, 0x00 },
  { 0x00000ccb, 0x00000ccb, 0x00 },
  { 0x00000cd5, 0x00000cd5, 0x00 },
  { 0x00000cd6, 0x00000cd6, 0x00 },
  { 0x00000d02, 0x00000d02, 0x00 },
  { 0x00000d03, 0x00000d03, 0x00 },
  { 0x00000d3e, 0x00000d40, 0x01 },
  { 0x00000d46, 0x00000d48, 0x01 },
  { 0x00000d4a, 0x00000d4c, 0x01 },
  { 0x00000d57, 0x00000d57, 0x00 },
  { 0x00000d82, 0x00000d82, 0x00 },
  { 0x00000d83, 0x00000d83, 0x00 },
  { 0x00000dcf, 0x00000dd1, 0x01 },
  { 0x00000dd8, 0x00000ddf, 0x01 },
  { 0x00000df2, 0x00000df2, 0x00 },
  { 0x00000df3, 0x00000df3, 0x00 },
  { 0x00000f3e, 0x00000f3e, 0x00 },
  { 0x00000f3f, 0x00000f3f, 0x00 },
  { 0x00000f7f, 0x00000f7f, 0x00 },
  { 0x0000102c, 0x0000102c, 0x00 },
  { 0x00001031, 0x00001031, 0x00 },
  { 0x00001038, 0x00001038, 0x00 },
  { 0x00001056, 0x00001056, 0x00 },
  { 0x00001057, 0x00001057, 0x00 },
  { 0x000017b4, 0x000017b6, 0x01 },
  { 0x000017be, 0x000017c5, 0x01 },
  { 0x000017c7, 0x000017c7, 0x00 },
  { 0x000017c8, 0x000017c8, 0x00 },
  { 0x0001d165, 0x0001d165, 0x00 },
  { 0x0001d166, 0x0001d166, 0x00 },
  { 0x0001d16d, 0x0001d172, 0x01 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_MarkSpacingCombining_Class_t NW_Unicode_MarkSpacingCombining_Class = {
  { /* NW_Object_Core                  */
    /* superclass                      */ &NW_Unicode_Category_Class,
    /* queryInterface                  */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category             */
    /* tableSize                       */ sizeof(_NW_Unicode_MarkSpacingCombining_Table)
	                                  / sizeof(*_NW_Unicode_MarkSpacingCombining_Table),
    /* table                           */ _NW_Unicode_MarkSpacingCombining_Table
  },
  { /* NW_Unicode_MarkSpacingCombining */
    /* unused                          */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_MarkSpacingCombining_t NW_Unicode_MarkSpacingCombining = {
  { { &NW_Unicode_MarkSpacingCombining_Class } }
};
