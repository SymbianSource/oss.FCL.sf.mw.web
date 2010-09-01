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


#include "nw_xhtml_variablesi.h"
#include "nw_adt_segmentedvector.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_Variables_ReplaceValue (NW_XHTML_Variables_Entry_t* entry,
                                 const NW_Text_t* value)
{
  NW_Ucs2* newValue;

  /* parameter assertions */
  NW_ASSERT (entry != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (value, &NW_Text_Abstract_Class));

  /* get the storage for the value and create a copy */
  newValue = NW_Text_GetUCS2Buffer (value,
                                    NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                                    NULL, NULL);

  /* free the old value and set the new */
  NW_Mem_Free (entry->value);
  entry->value = newValue;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static
NW_ADT_Vector_Metric_t
NW_XHTML_Variables_Lookup (const NW_XHTML_Variables_t* thisObj,
                           const NW_Ucs2* name)
{
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t start;
  NW_ADT_Vector_Metric_t end;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_Variables_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (name, &NW_Text_Abstract_Class));

  /* look for the entry using a binary search algorithm */
  index = 0;
  start = 0;
  end =  (NW_ADT_Vector_Metric_t)
    (NW_ADT_Vector_GetSize (thisObj->vector) - 1);
  while (end >= start) {
    NW_XHTML_Variables_Entry_t* entry;
    NW_Int32 match;

    index = (NW_ADT_Vector_Metric_t) ((end + start) / 2);

    entry = (NW_XHTML_Variables_Entry_t*)
      NW_ADT_Vector_ElementAt (thisObj->vector, index);
    match = NW_Str_Strcmp (name, entry->name);

    if (!match) {
      return index;
    } else if (match > 0) {
      start = (NW_ADT_Vector_Metric_t) (index + 1);
    } else if (match < 0) {
      end = (NW_ADT_Vector_Metric_t) (index - 1);
    }
  }

  /* we didn't find the entry we were looking for, but we return the index of
     the position at which it would have been */
  return index;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_Variables_Class_t NW_XHTML_Variables_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_XHTML_Variables_t),
    /* construct               */ _NW_XHTML_Variables_Construct,
    /* destruct                */ _NW_XHTML_Variables_Destruct
  },
  { /* NW_XHTML_Variables      */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_Variables_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argp)
{
  NW_XHTML_Variables_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_XHTML_Variables_Class));
  NW_ASSERT (argp != NULL);

  /* avoid 'unreferenced formal parameter' warnings */
  NW_REQUIRED_PARAM (argp);

  /* for convenience */
  thisObj = NW_XHTML_VariablesOf (dynamicObject);

  /* allocate the vector */
  thisObj->vector = (NW_ADT_DynamicVector_t*)
    NW_ADT_SegmentedVector_New (sizeof (NW_XHTML_Variables_Entry_t), 16);
  if (thisObj->vector == NULL) {
    return  KBrsrOutOfMemory;
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_XHTML_Variables_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_XHTML_Variables_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_XHTML_Variables_Class));

  /* for convenience */
  thisObj = NW_XHTML_VariablesOf (dynamicObject);

  /* deallocate our dynamic data */
  NW_Object_Delete (thisObj->vector);
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_Variables_Set (NW_XHTML_Variables_t* thisObj,
                        const NW_Text_t* name,
                        const NW_Text_t* value)
{
  const NW_Ucs2* storage;
  NW_ADT_Vector_Metric_t index;
  NW_XHTML_Variables_Entry_t* entry;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_Variables_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (name, &NW_Text_Abstract_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (value, &NW_Text_Abstract_Class));

  /* convert the supplied name to Unicode and get its storage */
  storage = NW_Text_GetUCS2Buffer (name,
                                   NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                                   NULL, NULL);
  if (storage == NULL) {
    return KBrsrFailure;
  }

  /* find the entry in the list that either matches name or is the place to
     put name using whatever sort/search algorithm in place */
  index = NW_XHTML_Variables_Lookup (thisObj, storage);
  entry = (NW_XHTML_Variables_Entry_t*)
    NW_ADT_Vector_ElementAt (thisObj->vector, index);
  if (entry == NULL) {
    NW_Mem_Free ((NW_Ucs2*) storage);
    return KBrsrFailure;
  }

  /* if the names dont match, we'll have to create a new entry */
  if (NW_Str_Strcmp (entry->name, storage)) {
    /* insert a blank entry into the vector */
    entry = (NW_XHTML_Variables_Entry_t*)
      NW_ADT_DynamicVector_InsertAt (thisObj->vector, NULL,
                                     index);
    if (entry == NULL) {
      NW_Mem_Free ((NW_Ucs2*) storage);
      return KBrsrOutOfMemory;
    }

    /* create a new copy of the name and put it into the entry */
    entry->name = (NW_Ucs2*) storage;
    storage = NULL;
  }

  /* clean up the temporary text object (if we have one) */
  NW_Mem_Free ((NW_Ucs2*) storage);

  /* finally insert the value into the entry */
  return NW_XHTML_Variables_ReplaceValue (entry, value);
}

/* ------------------------------------------------------------------------- */
NW_Text_t*
NW_XHTML_Variables_Get (const NW_XHTML_Variables_t* thisObj,
                        const NW_Text_t* name)
{
  const NW_Ucs2* storage;
  NW_Uint8 freeNeeded;
  NW_ADT_Vector_Metric_t index;
  NW_XHTML_Variables_Entry_t* entry;
  NW_Text_UCS2_t* value;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_Variables_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (name, &NW_Text_Abstract_Class));

  /* convert the supplied name to Unicode and get its storage */
  storage =
    NW_Text_GetUCS2Buffer (name,  NW_Text_Flags_Aligned, NULL, &freeNeeded);
  if (storage == NULL) {
    return NULL;
  }

  /* find the entry in the list that either matches name or is the place to
     put name using whatever sort/search algorithm in place */
  index = NW_XHTML_Variables_Lookup (thisObj, storage);
  entry = (NW_XHTML_Variables_Entry_t*)
    NW_ADT_Vector_ElementAt (thisObj->vector, index);
  if (entry == NULL) {
    if (freeNeeded) {
      NW_Mem_Free ((NW_Ucs2*) storage);
    }
    return NULL;
  }

  /* if we find a match, create a text object to wrap it */
  value = NULL;

  if (!NW_Str_Strcmp (entry->name, storage)) {
    value =
      NW_Text_UCS2_New (NW_Str_Newcpy (entry->value), 0, NW_Text_Flags_TakeOwnership);
  }

  /* clean up after us */
  if (freeNeeded) {
    NW_Mem_Free ((NW_Ucs2*) storage);
  }

  /* return the value */
  return NW_TextOf (value);
}
