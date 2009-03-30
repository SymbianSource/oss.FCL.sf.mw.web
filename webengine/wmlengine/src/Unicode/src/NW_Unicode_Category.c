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


#include "NW_Unicode_CategoryI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_Category_Class_t NW_Unicode_Category_Class = {
  { /* NW_Object_Core      */
    /* super               */ &NW_Object_Core_Class,
    /* queryInterface      */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category */
    /* tableSize           */ 0,
    /* table               */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_Unicode_Category_IsMemberOf (const NW_Unicode_Category_t* category,
				 NW_Ucs4 code)
{
  NW_Int32 index;
  NW_Int32 start;
  NW_Int32 end;
  const NW_Unicode_Category_TableEntry_t* table;

  /* since we require that the table is sorted, we can apply a binary search
     algorithm to find the entry for the supplied code */
  index = 0;
  start = 0;
  end = NW_Unicode_Category_GetClassPart(category).tableSize - 1;
  table = NW_Unicode_Category_GetClassPart(category).table;
  
  while (end >= start) {
    index = (end + start) / 2;
    if (code >= table[index].start && code <= table[index].end) {
      if (table[index].increment == 0 ||
          (code - table[index].start) % table[index].increment == 0) {
	return NW_TRUE;
      }
      return NW_FALSE;
    } else if (code < table[index].start) {
      end = index - 1;
    } else if (code > table[index].end) {
      start = index + 1;
    }
  }
  return NW_FALSE;
}
