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


#include "NW_Unicode_SymbolMathI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_SymbolMath_Table[] = {
  { 0x0000002b, 0x0000002b, 0x00 },
  { 0x0000003c, 0x0000003e, 0x01 },
  { 0x0000007c, 0x0000007c, 0x00 },
  { 0x0000007e, 0x0000007e, 0x00 },
  { 0x000000ac, 0x000000ac, 0x00 },
  { 0x000000b1, 0x000000b1, 0x00 },
  { 0x000000d7, 0x000000d7, 0x00 },
  { 0x000000f7, 0x000000f7, 0x00 },
  { 0x00002044, 0x00002044, 0x00 },
  { 0x0000207a, 0x0000207c, 0x01 },
  { 0x0000208a, 0x0000208c, 0x01 },
  { 0x00002190, 0x00002194, 0x01 },
  { 0x0000219a, 0x0000219a, 0x00 },
  { 0x0000219b, 0x0000219b, 0x00 },
  { 0x000021a0, 0x000021a6, 0x03 },
  { 0x000021ae, 0x000021ae, 0x00 },
  { 0x000021ce, 0x000021ce, 0x00 },
  { 0x000021cf, 0x000021cf, 0x00 },
  { 0x000021d2, 0x000021d2, 0x00 },
  { 0x000021d4, 0x000021d4, 0x00 },
  { 0x00002200, 0x000022f1, 0x01 },
  { 0x00002308, 0x0000230b, 0x01 },
  { 0x00002320, 0x00002320, 0x00 },
  { 0x00002321, 0x00002321, 0x00 },
  { 0x000025b7, 0x000025b7, 0x00 },
  { 0x000025c1, 0x000025c1, 0x00 },
  { 0x0000266f, 0x0000266f, 0x00 },
  { 0x0000fb29, 0x0000fb29, 0x00 },
  { 0x0000fe62, 0x0000fe62, 0x00 },
  { 0x0000fe64, 0x0000fe66, 0x01 },
  { 0x0000ff0b, 0x0000ff0b, 0x00 },
  { 0x0000ff1c, 0x0000ff1e, 0x01 },
  { 0x0000ff5c, 0x0000ff5c, 0x00 },
  { 0x0000ff5e, 0x0000ff5e, 0x00 },
  { 0x0000ffe2, 0x0000ffe2, 0x00 },
  { 0x0000ffe9, 0x0000ffec, 0x01 },
  { 0x0001d6c1, 0x0001d6c1, 0x00 },
  { 0x0001d6db, 0x0001d6db, 0x00 },
  { 0x0001d6fb, 0x0001d6fb, 0x00 },
  { 0x0001d715, 0x0001d715, 0x00 },
  { 0x0001d735, 0x0001d735, 0x00 },
  { 0x0001d74f, 0x0001d74f, 0x00 },
  { 0x0001d76f, 0x0001d76f, 0x00 },
  { 0x0001d789, 0x0001d789, 0x00 },
  { 0x0001d7a9, 0x0001d7a9, 0x00 },
  { 0x0001d7c3, 0x0001d7c3, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_SymbolMath_Class_t NW_Unicode_SymbolMath_Class = {
  { /* NW_Object_Core        */
    /* superclass            */ &NW_Unicode_Category_Class,
    /* queryInterface        */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category   */
    /* tableSize             */ sizeof(_NW_Unicode_SymbolMath_Table)
	                                  / sizeof(*_NW_Unicode_SymbolMath_Table),
    /* table                 */ _NW_Unicode_SymbolMath_Table
  },
  { /* NW_Unicode_SymbolMath */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_SymbolMath_t NW_Unicode_SymbolMath = {
  { { &NW_Unicode_SymbolMath_Class } }
};
