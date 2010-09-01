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


#include "NW_Unicode_OtherPrivateUseI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_OtherPrivateUse_Table[] = {
  { 0x0000e000, 0x0000e000, 0x00 },
  { 0x0000f8ff, 0x0000f8ff, 0x00 },
  { 0x000f0000, 0x000f0000, 0x00 },
  { 0x000ffffd, 0x000ffffd, 0x00 },
  { 0x00100000, 0x00100000, 0x00 },
  { 0x0010fffd, 0x0010fffd, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_OtherPrivateUse_Class_t NW_Unicode_OtherPrivateUse_Class = {
  { /* NW_Object_Core             */
    /* superclass                 */ &NW_Unicode_Category_Class,
    /* queryInterface             */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category        */
    /* tableSize                  */ sizeof(_NW_Unicode_OtherPrivateUse_Table)
	                                  / sizeof(*_NW_Unicode_OtherPrivateUse_Table),
    /* table                      */ _NW_Unicode_OtherPrivateUse_Table
  },
  { /* NW_Unicode_OtherPrivateUse */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_OtherPrivateUse_t NW_Unicode_OtherPrivateUse = {
  { { &NW_Unicode_OtherPrivateUse_Class } }
};
