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


#include "NW_Unicode_NumberOtherI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_NumberOther_Table[] = {
  { 0x000000b2, 0x000000b2, 0x00 },
  { 0x000000b3, 0x000000b3, 0x00 },
  { 0x000000b9, 0x000000b9, 0x00 },
  { 0x000000bc, 0x000000be, 0x01 },
  { 0x000009f4, 0x000009f9, 0x01 },
  { 0x00000bf0, 0x00000bf2, 0x01 },
  { 0x00000f2a, 0x00000f33, 0x01 },
  { 0x00001372, 0x0000137c, 0x01 },
  { 0x00002070, 0x00002070, 0x00 },
  { 0x00002074, 0x00002079, 0x01 },
  { 0x00002080, 0x00002089, 0x01 },
  { 0x00002153, 0x0000215f, 0x01 },
  { 0x00002460, 0x0000249b, 0x01 },
  { 0x000024ea, 0x000024ea, 0x00 },
  { 0x00002776, 0x00002793, 0x01 },
  { 0x00003192, 0x00003195, 0x01 },
  { 0x00003220, 0x00003229, 0x01 },
  { 0x00003280, 0x00003289, 0x01 },
  { 0x00010320, 0x00010323, 0x01 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_NumberOther_Class_t NW_Unicode_NumberOther_Class = {
  { /* NW_Object_Core         */
    /* superclass             */ &NW_Unicode_Category_Class,
    /* queryInterface         */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category    */
    /* tableSize              */ sizeof(_NW_Unicode_NumberOther_Table)
	                                  / sizeof(*_NW_Unicode_NumberOther_Table),
    /* table                  */ _NW_Unicode_NumberOther_Table
  },
  { /* NW_Unicode_NumberOther */
    /* unused                 */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_NumberOther_t NW_Unicode_NumberOther = {
  { { &NW_Unicode_NumberOther_Class } }
};
