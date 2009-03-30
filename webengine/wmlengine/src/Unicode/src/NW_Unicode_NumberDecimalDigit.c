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


#include "NW_Unicode_NumberDecimalDigitI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_NumberDecimalDigit_Table[] = {
  { 0x00000030, 0x00000039, 0x01 },
  { 0x00000660, 0x00000669, 0x01 },
  { 0x000006f0, 0x000006f9, 0x01 },
  { 0x00000966, 0x0000096f, 0x01 },
  { 0x000009e6, 0x000009ef, 0x01 },
  { 0x00000a66, 0x00000a6f, 0x01 },
  { 0x00000ae6, 0x00000aef, 0x01 },
  { 0x00000b66, 0x00000b6f, 0x01 },
  { 0x00000be7, 0x00000bef, 0x01 },
  { 0x00000c66, 0x00000c6f, 0x01 },
  { 0x00000ce6, 0x00000cef, 0x01 },
  { 0x00000d66, 0x00000d6f, 0x01 },
  { 0x00000e50, 0x00000e59, 0x01 },
  { 0x00000ed0, 0x00000ed9, 0x01 },
  { 0x00000f20, 0x00000f29, 0x01 },
  { 0x00001040, 0x00001049, 0x01 },
  { 0x00001369, 0x00001371, 0x01 },
  { 0x000017e0, 0x000017e9, 0x01 },
  { 0x00001810, 0x00001819, 0x01 },
  { 0x0000ff10, 0x0000ff19, 0x01 },
  { 0x0001d7ce, 0x0001d7ff, 0x01 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_NumberDecimalDigit_Class_t NW_Unicode_NumberDecimalDigit_Class = {
  { /* NW_Object_Core                */
    /* superclass                    */ &NW_Unicode_Category_Class,
    /* queryInterface                */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category           */
    /* tableSize                     */ sizeof(_NW_Unicode_NumberDecimalDigit_Table)
	                                  / sizeof(*_NW_Unicode_NumberDecimalDigit_Table),
    /* table                         */ _NW_Unicode_NumberDecimalDigit_Table
  },
  { /* NW_Unicode_NumberDecimalDigit */
    /* unused                        */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_NumberDecimalDigit_t NW_Unicode_NumberDecimalDigit = {
  { { &NW_Unicode_NumberDecimalDigit_Class } }
};
