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


#include "nw_adt_valuelisti.h"

#include "nw_adt_mapiterator.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   public/protected global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_ValueList_Class_t NW_ADT_ValueList_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_ADT_ValueList_t),
    /* construct               */ _NW_ADT_ValueList_Construct,
    /* destruct                */ _NW_ADT_ValueList_Destruct
  },
  { /* NW_ADT_ValueList        */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_ValueList_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp)
{
  NW_ADT_ValueList_t* thisObj;
  NW_Uint32 keySize;
  NW_ADT_Vector_Metric_t capacity;
  NW_ADT_Vector_Metric_t increment;

  /* for convenience */
  thisObj = NW_ADT_ValueListOf (dynamicObject);

  /* initialize the object */
  keySize = va_arg (*argp, NW_Uint32);
  capacity = (NW_ADT_Vector_Metric_t) va_arg (*argp, NW_Uint32);
  increment = (NW_ADT_Vector_Metric_t) va_arg (*argp, NW_Uint32);

  /* map holds entry objects (not pointers) */
  thisObj->map =
    NW_ADT_ResizableMap_New (keySize, sizeof (NW_ADT_ValueList_Entry_t),
                             capacity, increment);
  if (thisObj->map == NULL) {
    return KBrsrOutOfMemory;
  }
  
  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_ADT_ValueList_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_ADT_ValueList_t* thisObj;
  NW_ADT_Iterator_t* iterator;

  /* for convenience */
  thisObj = NW_ADT_ValueListOf (dynamicObject);

  /* first we need to iterate through the map deallocating the properties that
     need to be freed */
  iterator = (NW_ADT_Iterator_t*) NW_ADT_MapIterator_New (thisObj->map);
  while (NW_ADT_Iterator_HasMoreElements (iterator)) {
    void* key;
    NW_ADT_ValueList_Entry_t entry;
    TBrowserStatusCode status;

    key = NW_ADT_Iterator_NextElement (iterator);
    status = NW_ADT_ValueList_Get (thisObj, key, &entry);
    NW_ASSERT (status == KBrsrSuccess);
		/* To fix TI compiler warning */
		(void) status;

    if (entry.freeNeeded) {
      if (entry.type == NW_ADT_ValueType_Pointer) {
        NW_Mem_Free (entry.value.pointer);
      } else if (entry.type & NW_ADT_ValueType_Object) {
        if (entry.value.object != NULL) {
          NW_Object_Delete (entry.value.object);
          entry.value.object = NULL;
        }
      }
    }
  }

  /* delete the iterator */
  NW_Object_Delete (iterator);

  /* free the map itself */
  NW_Object_Delete (thisObj->map);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_ADT_ValueList_Get (const NW_ADT_ValueList_t* thisObj,
                      void* key,
                      NW_ADT_ValueList_Entry_t* entry)
{
  NW_ADT_ValueList_Entry_t* element;

  /* parameter assertion block */
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_ValueList_Class));

  /* fetch a pointer to the value from the map */
  element = (NW_ADT_ValueList_Entry_t*) NW_ADT_Map_GetUnsafe (thisObj->map, key);
  if (element == NULL) {
    return KBrsrNotFound;
  }

  /* use byte-wise copy to get the unaligned data into the client's variable */
  if (entry != NULL) {
    (void) NW_Mem_memcpy (entry, element, sizeof *entry);
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_ValueList_Set (NW_ADT_ValueList_t* thisObj,
                      void* key,
                      const NW_ADT_ValueList_Entry_t* entry)
{
  NW_ADT_ValueList_Entry_t oldEntry;
  TBrowserStatusCode status;

  /* parameter assertion block */
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_ValueList_Class));
  NW_ASSERT (entry != NULL);
  NW_ASSERT (entry->type != 0);
  
  /* If the entry already exists, delete it */
  status = NW_ADT_ValueList_Get (thisObj, key, &oldEntry);
  if (status == KBrsrSuccess) {
    if (oldEntry.freeNeeded) {
      if (oldEntry.type == NW_ADT_ValueType_Pointer) {
        NW_Mem_Free (oldEntry.value.pointer);
        oldEntry.value.pointer = NULL;
      } else if (oldEntry.type & NW_ADT_ValueType_Object) {
        if (oldEntry.value.object != NULL) {
          NW_Object_Delete (oldEntry.value.object);
          oldEntry.value.object = NULL;
        }
      }
    }
  }

  /* set the entry in the map and return the result of the operation */
  return NW_ADT_Map_Set (thisObj->map, key, entry);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
NW_ADT_ValueList_Remove (const NW_ADT_ValueList_t* thisObj,
                         void* key)
{
  NW_ADT_ValueList_Entry_t oldEntry;
  TBrowserStatusCode status;

  /* parameter assertion block */
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_ValueList_Class));

  /* If the entry already exists, delete it */
  status = NW_ADT_ValueList_Get (thisObj, key, &oldEntry);
  if (status == KBrsrSuccess) {

    if (oldEntry.type == NW_ADT_ValueType_Pointer) {
      NW_Mem_Free (oldEntry.value.pointer);
      oldEntry.value.pointer = NULL;
    } else if ((oldEntry.type & NW_ADT_ValueType_Object) &&
               oldEntry.freeNeeded) {
      if (oldEntry.value.object != NULL) {
        NW_Object_Delete (oldEntry.value.object);
        oldEntry.value.object = NULL;
      }
    }

    /* remove the entry */
    return NW_ADT_Map_Remove (thisObj->map, key);
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_ValueList_Copy (const NW_ADT_ValueList_t* thisObj,
                       NW_ADT_ValueList_t* dest)
{
  NW_ADT_Iterator_t* iterator;
  TBrowserStatusCode status = KBrsrSuccess;

  iterator = (NW_ADT_Iterator_t*) NW_ADT_MapIterator_New (thisObj->map);
  while (NW_ADT_Iterator_HasMoreElements (iterator)) {
    void* key;
    NW_ADT_ValueList_Entry_t entry, newEntry;

    key = NW_ADT_Iterator_NextElement (iterator);
    status = NW_ADT_ValueList_Get (thisObj, key, &entry);
    NW_ASSERT (status == KBrsrSuccess);

    newEntry = entry;
    newEntry.freeNeeded = NW_FALSE;

    status = NW_ADT_ValueList_Set (dest, key, &newEntry);
  }

  /* delete the iterator */
  NW_Object_Delete (iterator);

  return status;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_ValueList_t*
NW_ADT_ValueList_New (NW_Uint32 keySize,
                      NW_ADT_Vector_Metric_t capacity,
                      NW_ADT_Vector_Metric_t increment)
{
  return (NW_ADT_ValueList_t*)
    NW_Object_New (&NW_ADT_ValueList_Class, keySize, (NW_Uint32) capacity, (NW_Uint32)increment);
}
