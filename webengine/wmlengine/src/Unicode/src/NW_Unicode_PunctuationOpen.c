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


#include "NW_Unicode_PunctuationOpenI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_PunctuationOpen_Table[] = {
  { 0x00000028, 0x00000028, 0x00 },
  { 0x0000005b, 0x0000005b, 0x00 },
  { 0x0000007b, 0x0000007b, 0x00 },
  { 0x00000f3a, 0x00000f3a, 0x00 },
  { 0x00000f3c, 0x00000f3c, 0x00 },
  { 0x0000169b, 0x0000169b, 0x00 },
  { 0x0000201a, 0x0000201a, 0x00 },
  { 0x0000201e, 0x0000201e, 0x00 },
  { 0x00002045, 0x00002045, 0x00 },
  { 0x0000207d, 0x0000207d, 0x00 },
  { 0x0000208d, 0x0000208d, 0x00 },
  { 0x00002329, 0x00002329, 0x00 },
  { 0x00003008, 0x00003010, 0x02 },
  { 0x00003014, 0x0000301a, 0x02 },
  { 0x0000301d, 0x0000301d, 0x00 },
  { 0x0000fd3e, 0x0000fd3e, 0x00 },
  { 0x0000fe35, 0x0000fe43, 0x02 },
  { 0x0000fe59, 0x0000fe5d, 0x02 },
  { 0x0000ff08, 0x0000ff08, 0x00 },
  { 0x0000ff3b, 0x0000ff3b, 0x00 },
  { 0x0000ff5b, 0x0000ff5b, 0x00 },
  { 0x0000ff62, 0x0000ff62, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationOpen_Class_t NW_Unicode_PunctuationOpen_Class = {
  { /* NW_Object_Core             */
    /* superclass                 */ &NW_Unicode_Category_Class,
    /* queryInterface             */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category        */
    /* tableSize                  */ sizeof(_NW_Unicode_PunctuationOpen_Table)
	                                  / sizeof(*_NW_Unicode_PunctuationOpen_Table),
    /* table                      */ _NW_Unicode_PunctuationOpen_Table
  },
  { /* NW_Unicode_PunctuationOpen */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationOpen_t NW_Unicode_PunctuationOpen = {
  { { &NW_Unicode_PunctuationOpen_Class } }
};
