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


#include "nw_hed_entityseti.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_EntitySet_Class_t NW_HED_EntitySet_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Base_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_HED_EntitySet          */
    /* numEntries                */ 0,
    /* numCaseInsensitiveEntries */ 0,
    /* entries                   */ NULL,
    /* getEntityChar	         */ _NW_HED_EntitySet_GetEntityChar
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Ucs2
_NW_HED_EntitySet_GetEntityChar (const NW_HED_EntitySet_t* entitySet,
                                 NW_Ucs2 *name)
{
  NW_Int32 index;
  const NW_HED_EntitySet_Entry_t* entry;

  /* First do a binary search search in the case sensitive part of the array */
  NW_Int32 low = 0;
  NW_Int32 high = NW_HED_EntitySet_GetClassPart(entitySet).numEntries 
				  - NW_HED_EntitySet_GetClassPart(entitySet).numCaseInsensitiveEntries - 1;
  NW_Int32 res = 0;

  while (low <= high ) {
    index = (high + low) / 2;
    entry = & (NW_HED_EntitySet_GetClassPart(entitySet).entries[index]);

    /* do a case sensitive string comparison */
    res = NW_Str_StrcmpConst( name, entry->name );

    if ( res > 0 ) {
      /* name is ahead of this slot.  Increase low bound. */
      low = index + 1;
    } else if ( res < 0 ) {
      /* name is behind this slot.  Decrease high bound. */
      high = index - 1;
    } else {
      /* Found the entity name.  Return its value. */
      return entry->value;
    }
  }

  /* if no match was found search in the case insensitive part of the table. */
  low = NW_HED_EntitySet_GetClassPart(entitySet).numEntries 
				 - NW_HED_EntitySet_GetClassPart(entitySet).numCaseInsensitiveEntries;
  high = NW_HED_EntitySet_GetClassPart(entitySet).numEntries - 1;
  res = 0;
  while (low <= high ) {
    index = (high + low) / 2;
    entry = & (NW_HED_EntitySet_GetClassPart(entitySet).entries[index]);

    // do a case insensitive string comparison
    res = NW_Str_StricmpConst( name, entry->name );

    if ( res > 0 ) {
      /* name is ahead of this slot.  Increase low bound. */
      low = index + 1;
    } else if ( res < 0 ) {
      /* name is behind this slot.  Decrease high bound. */
      high = index - 1;
    } else {
      /* Found the entity name.  Return its value. */
      return entry->value;
    }
  }

  /* if no match were found we return 0 */
  return 0;
}
