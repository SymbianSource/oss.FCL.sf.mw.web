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


#include "NW_Unicode_NumberLetterI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_NumberLetter_Table[] = {
  { 0x000016ee, 0x000016f0, 0x01 },
  { 0x00002160, 0x00002183, 0x01 },
  { 0x00003007, 0x00003007, 0x00 },
  { 0x00003021, 0x00003029, 0x01 },
  { 0x00003038, 0x0000303a, 0x01 },
  { 0x0001034a, 0x0001034a, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_NumberLetter_Class_t NW_Unicode_NumberLetter_Class = {
  { /* NW_Object_Core          */
    /* superclass              */ &NW_Unicode_Category_Class,
    /* queryInterface          */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category     */
    /* tableSize               */ sizeof(_NW_Unicode_NumberLetter_Table)
	                                  / sizeof(*_NW_Unicode_NumberLetter_Table),
    /* table                   */ _NW_Unicode_NumberLetter_Table
  },
  { /* NW_Unicode_NumberLetter */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_NumberLetter_t NW_Unicode_NumberLetter = {
  { { &NW_Unicode_NumberLetter_Class } }
};
