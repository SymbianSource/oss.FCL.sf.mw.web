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


#include "NW_Unicode_OtherFormatI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_OtherFormat_Table[] = {
  { 0x0000070f, 0x0000070f, 0x00 },
  { 0x0000180b, 0x0000180e, 0x01 },
  { 0x0000200c, 0x0000200f, 0x01 },
  { 0x0000202a, 0x0000202e, 0x01 },
  { 0x0000206a, 0x0000206f, 0x01 },
  { 0x0000feff, 0x0000feff, 0x00 },
  { 0x0000fff9, 0x0000fffb, 0x01 },
  { 0x0001d173, 0x0001d17a, 0x01 },
  { 0x000e0001, 0x000e0001, 0x00 },
  { 0x000e0020, 0x000e007f, 0x01 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_OtherFormat_Class_t NW_Unicode_OtherFormat_Class = {
  { /* NW_Object_Core         */
    /* superclass             */ &NW_Unicode_Category_Class,
    /* queryInterface         */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category    */
    /* tableSize              */ sizeof(_NW_Unicode_OtherFormat_Table)
	                                  / sizeof(*_NW_Unicode_OtherFormat_Table),
    /* table                  */ _NW_Unicode_OtherFormat_Table
  },
  { /* NW_Unicode_OtherFormat */
    /* unused                 */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_OtherFormat_t NW_Unicode_OtherFormat = {
  { { &NW_Unicode_OtherFormat_Class } }
};
