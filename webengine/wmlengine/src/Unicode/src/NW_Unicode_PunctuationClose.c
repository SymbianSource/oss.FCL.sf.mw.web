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


#include "NW_Unicode_PunctuationCloseI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_PunctuationClose_Table[] = {
  { 0x00000029, 0x00000029, 0x00 },
  { 0x0000005d, 0x0000005d, 0x00 },
  { 0x0000007d, 0x0000007d, 0x00 },
  { 0x00000f3b, 0x00000f3b, 0x00 },
  { 0x00000f3d, 0x00000f3d, 0x00 },
  { 0x0000169c, 0x0000169c, 0x00 },
  { 0x00002046, 0x00002046, 0x00 },
  { 0x0000207e, 0x0000207e, 0x00 },
  { 0x0000208e, 0x0000208e, 0x00 },
  { 0x0000232a, 0x0000232a, 0x00 },
  { 0x00003009, 0x00003011, 0x02 },
  { 0x00003015, 0x0000301b, 0x02 },
  { 0x0000301e, 0x0000301e, 0x00 },
  { 0x0000301f, 0x0000301f, 0x00 },
  { 0x0000fd3f, 0x0000fd3f, 0x00 },
  { 0x0000fe36, 0x0000fe44, 0x02 },
  { 0x0000fe5a, 0x0000fe5e, 0x02 },
  { 0x0000ff09, 0x0000ff09, 0x00 },
  { 0x0000ff3d, 0x0000ff3d, 0x00 },
  { 0x0000ff5d, 0x0000ff5d, 0x00 },
  { 0x0000ff63, 0x0000ff63, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationClose_Class_t NW_Unicode_PunctuationClose_Class = {
  { /* NW_Object_Core              */
    /* superclass                  */ &NW_Unicode_Category_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category         */
    /* tableSize                   */ sizeof(_NW_Unicode_PunctuationClose_Table)
	                                  / sizeof(*_NW_Unicode_PunctuationClose_Table),
    /* table                       */ _NW_Unicode_PunctuationClose_Table
  },
  { /* NW_Unicode_PunctuationClose */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationClose_t NW_Unicode_PunctuationClose = {
  { { &NW_Unicode_PunctuationClose_Class } }
};
