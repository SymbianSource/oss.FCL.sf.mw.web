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


#include "NW_Unicode_SeparatorParagraphI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_SeparatorParagraph_Table[] = {
  { 0x00002029, 0x00002029, 0x00 },
  { 0x00002029, 0x00002029, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_SeparatorParagraph_Class_t NW_Unicode_SeparatorParagraph_Class = {
  { /* NW_Object_Core                */
    /* superclass                    */ &NW_Unicode_Category_Class,
    /* queryInterface                */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category           */
    /* tableSize                     */ sizeof(_NW_Unicode_SeparatorParagraph_Table)
	                                  / sizeof(*_NW_Unicode_SeparatorParagraph_Table),
    /* table                         */ _NW_Unicode_SeparatorParagraph_Table
  },
  { /* NW_Unicode_SeparatorParagraph */
    /* unused                        */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_SeparatorParagraph_t NW_Unicode_SeparatorParagraph = {
  { { &NW_Unicode_SeparatorParagraph_Class } }
};
