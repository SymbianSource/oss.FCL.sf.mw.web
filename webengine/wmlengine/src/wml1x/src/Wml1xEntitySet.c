/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_wml1x_wml1xentityseti.h"

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* Note: This array must be sorted by entity name.  The lookup function
         does a binary search.  */
static
const NW_HED_EntitySet_Entry_t _NW_Wml1x_EntitySet_Array[] = {
  { "amp", 38 },
  { "apos", 39 },
  { "gt", 62 },
  { "lt", 60 },
  { "nbsp", 160 },
  { "quot", 34 },
  { "shy", 173 }
};

/* ------------------------------------------------------------------------- */
const
NW_Wml1x_EntitySet_Class_t NW_Wml1x_EntitySet_Class = {
  { /* NW_Object_Core				*/
    /* superclass					*/ &NW_HED_EntitySet_Class,
    /* queryInterface				*/ _NW_Object_Core_QueryInterface
  },
  { /* NW_HED_EntitySet				*/
    /* numEntries					*/ sizeof (_NW_Wml1x_EntitySet_Array)
										/ sizeof (_NW_Wml1x_EntitySet_Array[0]),
    /* numCaseInsensitiveEntries    */ 0,
    /* entries						*/ &_NW_Wml1x_EntitySet_Array[0],
    /* createContentHandler			*/ _NW_HED_EntitySet_GetEntityChar
  },
  { /* NW_Wml1x_EntitySet			*/
    /* unused						*/ 0
  }
};

/* ------------------------------------------------------------------------- */
const NW_Wml1x_EntitySet_t NW_Wml1x_EntitySet = {
   { { &NW_Wml1x_EntitySet_Class } } 
};
