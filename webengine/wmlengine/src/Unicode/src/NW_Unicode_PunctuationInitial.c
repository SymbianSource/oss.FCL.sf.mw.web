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


#include "NW_Unicode_PunctuationInitialI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_PunctuationInitial_Table[] = {
  { 0x000000ab, 0x000000ab, 0x00 },
  { 0x00002018, 0x00002018, 0x00 },
  { 0x0000201b, 0x0000201b, 0x00 },
  { 0x0000201c, 0x0000201c, 0x00 },
  { 0x0000201f, 0x0000201f, 0x00 },
  { 0x00002039, 0x00002039, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationInitial_Class_t NW_Unicode_PunctuationInitial_Class = {
  { /* NW_Object_Core                */
    /* superclass                    */ &NW_Unicode_Category_Class,
    /* queryInterface                */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category           */
    /* tableSize                     */ sizeof(_NW_Unicode_PunctuationInitial_Table)
	                                  / sizeof(*_NW_Unicode_PunctuationInitial_Table),
    /* table                         */ _NW_Unicode_PunctuationInitial_Table
  },
  { /* NW_Unicode_PunctuationInitial */
    /* unused                        */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationInitial_t NW_Unicode_PunctuationInitial = {
  { { &NW_Unicode_PunctuationInitial_Class } }
};
