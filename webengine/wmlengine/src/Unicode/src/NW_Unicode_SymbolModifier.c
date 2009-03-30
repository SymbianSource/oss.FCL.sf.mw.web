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


#include "NW_Unicode_SymbolModifierI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_SymbolModifier_Table[] = {
  { 0x0000005e, 0x0000005e, 0x00 },
  { 0x00000060, 0x00000060, 0x00 },
  { 0x000000a8, 0x000000a8, 0x00 },
  { 0x000000af, 0x000000af, 0x00 },
  { 0x000000b4, 0x000000b4, 0x00 },
  { 0x000000b8, 0x000000b8, 0x00 },
  { 0x000002b9, 0x000002b9, 0x00 },
  { 0x000002ba, 0x000002ba, 0x00 },
  { 0x000002c2, 0x000002cf, 0x01 },
  { 0x000002d2, 0x000002df, 0x01 },
  { 0x000002e5, 0x000002ed, 0x01 },
  { 0x00000374, 0x00000374, 0x00 },
  { 0x00000375, 0x00000375, 0x00 },
  { 0x00000384, 0x00000384, 0x00 },
  { 0x00000385, 0x00000385, 0x00 },
  { 0x00001fbd, 0x00001fbd, 0x00 },
  { 0x00001fbf, 0x00001fc1, 0x01 },
  { 0x00001fcd, 0x00001fcf, 0x01 },
  { 0x00001fdd, 0x00001fdf, 0x01 },
  { 0x00001fed, 0x00001fef, 0x01 },
  { 0x00001ffd, 0x00001ffd, 0x00 },
  { 0x00001ffe, 0x00001ffe, 0x00 },
  { 0x0000309b, 0x0000309b, 0x00 },
  { 0x0000309c, 0x0000309c, 0x00 },
  { 0x0000ff3e, 0x0000ff3e, 0x00 },
  { 0x0000ff40, 0x0000ff40, 0x00 },
  { 0x0000ffe3, 0x0000ffe3, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_SymbolModifier_Class_t NW_Unicode_SymbolModifier_Class = {
  { /* NW_Object_Core            */
    /* superclass                */ &NW_Unicode_Category_Class,
    /* queryInterface            */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category       */
    /* tableSize                 */ sizeof(_NW_Unicode_SymbolModifier_Table)
	                                  / sizeof(*_NW_Unicode_SymbolModifier_Table),
    /* table                     */ _NW_Unicode_SymbolModifier_Table
  },
  { /* NW_Unicode_SymbolModifier */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_SymbolModifier_t NW_Unicode_SymbolModifier = {
  { { &NW_Unicode_SymbolModifier_Class } }
};
