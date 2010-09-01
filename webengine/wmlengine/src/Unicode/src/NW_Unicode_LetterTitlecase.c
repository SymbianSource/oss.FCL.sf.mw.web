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


#include "NW_Unicode_LetterTitlecaseI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_LetterTitlecase_Table[] = {
  { 0x000001c5, 0x000001cb, 0x03 },
  { 0x000001f2, 0x000001f2, 0x00 },
  { 0x00001f88, 0x00001f8f, 0x01 },
  { 0x00001f98, 0x00001f9f, 0x01 },
  { 0x00001fa8, 0x00001faf, 0x01 },
  { 0x00001fbc, 0x00001fbc, 0x00 },
  { 0x00001fcc, 0x00001fcc, 0x00 },
  { 0x00001ffc, 0x00001ffc, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_LetterTitlecase_Class_t NW_Unicode_LetterTitlecase_Class = {
  { /* NW_Object_Core             */
    /* superclass                 */ &NW_Unicode_Category_Class,
    /* queryInterface             */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category        */
    /* tableSize                  */ sizeof(_NW_Unicode_LetterTitlecase_Table)
	                                  / sizeof(*_NW_Unicode_LetterTitlecase_Table),
    /* table                      */ _NW_Unicode_LetterTitlecase_Table
  },
  { /* NW_Unicode_LetterTitlecase */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_LetterTitlecase_t NW_Unicode_LetterTitlecase = {
  { { &NW_Unicode_LetterTitlecase_Class } }
};
