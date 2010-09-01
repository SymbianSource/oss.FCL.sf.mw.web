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


#include "NW_Unicode_PunctuationConnectorI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_TableEntry_t _NW_Unicode_PunctuationConnector_Table[] = {
  { 0x0000005f, 0x0000005f, 0x00 },
  { 0x0000203f, 0x0000203f, 0x00 },
  { 0x00002040, 0x00002040, 0x00 },
  { 0x000030fb, 0x000030fb, 0x00 },
  { 0x0000fe33, 0x0000fe33, 0x00 },
  { 0x0000fe34, 0x0000fe34, 0x00 },
  { 0x0000fe4d, 0x0000fe4f, 0x01 },
  { 0x0000ff3f, 0x0000ff3f, 0x00 },
  { 0x0000ff65, 0x0000ff65, 0x00 },
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationConnector_Class_t NW_Unicode_PunctuationConnector_Class = {
  { /* NW_Object_Core                  */
    /* superclass                      */ &NW_Unicode_Category_Class,
    /* queryInterface                  */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category             */
    /* tableSize                       */ sizeof(_NW_Unicode_PunctuationConnector_Table)
	                                  / sizeof(*_NW_Unicode_PunctuationConnector_Table),
    /* table                           */ _NW_Unicode_PunctuationConnector_Table
  },
  { /* NW_Unicode_PunctuationConnector */
    /* unused                          */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_PunctuationConnector_t NW_Unicode_PunctuationConnector = {
  { { &NW_Unicode_PunctuationConnector_Class } }
};
