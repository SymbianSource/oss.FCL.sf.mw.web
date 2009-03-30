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


#include "nw_adt_textmapi.h"

#include "nw_text_ucs2.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_TextMap_Class_t NW_ADT_TextMap_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_ADT_TextMap_t),
    /* construct               */ _NW_ADT_Map_Construct,
    /* destruct                */ NULL
  },
  { /* NW_ADT_Map              */
    /* lookup                  */ _NW_ADT_TextMap_Lookup
  },
  { /* NW_ADT_TextMap          */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_Vector_Metric_t
_NW_ADT_TextMap_Lookup (const NW_ADT_Map_t* map,
                        const void* key)
{
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t start;
  NW_Int32 end;
  const NW_Ucs2* keyStorage;
  NW_Bool keyFreeNeeded;
  const NW_Ucs2* entryStorage;
  NW_Bool entryFreeNeeded;

  /* parameter assertions */
  NW_ASSERT (map != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (map, &NW_ADT_TextMap_Class));

  /* initialize the 'freeNeeded' flags */
  keyFreeNeeded = NW_FALSE;
  keyStorage = NULL;
  entryFreeNeeded = NW_FALSE;
  entryStorage = NULL;

  /* look for the entry using a binary search algorithm */
  index = 0;
  start = 0;
  end = NW_ADT_Vector_GetSize (NW_ADT_VectorOf (map->vector)) - 1;
  while (end >= start) {
    void* entry;
    NW_Text_Length_t keyLength;
    NW_Text_Length_t entryLength;
    NW_Int32 match;

    /* get the entry from the vector */
    index = (NW_ADT_Vector_Metric_t) ((end + start) / 2);
    entry = NW_ADT_Vector_ElementAt (NW_ADT_VectorOf (map->vector), index);

    /* normalize the stored entry and the provided key to Unicode Text objects
       before comparing */
    keyStorage = NW_Text_GetUCS2Buffer (key, 0, &keyLength, &keyFreeNeeded);
    entryStorage = NW_Text_GetUCS2Buffer (*(NW_Text_t**) entry, 0, &entryLength,
                                          &entryFreeNeeded);

    /* there is no need to compare the strings if the lengths are not equal */
    match = keyLength - entryLength;
    if (match == 0) {
      match = NW_Mem_memcmp (keyStorage, entryStorage, keyLength * sizeof (NW_Ucs2));
    }
    if (!match) {
      break;
    } else if (match > 0) {
      start = (NW_ADT_Vector_Metric_t) (index + 1);
    } else if (match < 0) {
      end = index - 1;
    }
  }

  /* free the storage if necessary */
  if (keyFreeNeeded) {
    NW_Mem_Free ((NW_Ucs2*) keyStorage);
  }
  if (entryFreeNeeded) {
    NW_Mem_Free ((NW_Ucs2*) entryStorage);
  }

  /* we didn't find the entry we were looking for, but we return the index of
     the position at which it would have been had it existed */
  if (start > index) {
    return start;
  }
  return index;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_TextMap_t*
NW_ADT_TextMap_New (NW_Uint32 elementSize,
                    NW_ADT_DynamicVector_t* dynamicVector)
{
  return (NW_ADT_TextMap_t*)
    NW_Object_New (&NW_ADT_TextMap_Class, sizeof (NW_Text_t*), elementSize,
                   dynamicVector);
}
