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


#include "NW_Unicode_SymbolCurrencyI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_SymbolCurrency_Table[] = {
  { 0x00000024, 0x00000024, 0x00 },
  { 0x000000a2, 0x000000a5, 0x01 },
  { 0x000009f2, 0x000009f2, 0x00 },
  { 0x000009f3, 0x000009f3, 0x00 },
  { 0x00000e3f, 0x00000e3f, 0x00 },
  { 0x000017db, 0x000017db, 0x00 },
  { 0x000020a0, 0x000020af, 0x01 },
  { 0x0000fe69, 0x0000fe69, 0x00 },
  { 0x0000ff04, 0x0000ff04, 0x00 },
  { 0x0000ffe0, 0x0000ffe0, 0x00 },
  { 0x0000ffe1, 0x0000ffe1, 0x00 },
  { 0x0000ffe5, 0x0000ffe5, 0x00 },
  { 0x0000ffe6, 0x0000ffe6, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_SymbolCurrency_Class_t NW_Unicode_SymbolCurrency_Class = {
  { /* NW_Object_Core            */
    /* superclass                */ &NW_Unicode_Category_Class,
    /* queryInterface            */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category       */
    /* tableSize                 */ sizeof(_NW_Unicode_SymbolCurrency_Table)
	                                  / sizeof(*_NW_Unicode_SymbolCurrency_Table),
    /* table                     */ _NW_Unicode_SymbolCurrency_Table
  },
  { /* NW_Unicode_SymbolCurrency */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_SymbolCurrency_t NW_Unicode_SymbolCurrency = {
  { { &NW_Unicode_SymbolCurrency_Class } }
};
