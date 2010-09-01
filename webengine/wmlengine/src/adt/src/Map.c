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


#include "nw_adt_mapi.h"
#include "nw_adt_mapiterator.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_Map_Class_t NW_ADT_Map_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_ADT_Map_t),
    /* construct               */ _NW_ADT_Map_Construct,
    /* destruct                */ _NW_ADT_Map_Destruct
  },
  { /* NW_ADT_Map              */
    /* lookup                  */ _NW_ADT_Map_Lookup
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_Map_Construct (NW_Object_Dynamic_t* dynamicObject,
                       va_list* argp)
{
  NW_ADT_Map_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_ADT_Map_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_ADT_MapOf (dynamicObject);

  /* set the member variables */
  thisObj->keySize = va_arg (*argp, NW_Uint32);
  thisObj->valueSize = va_arg (*argp, NW_Uint32);
  thisObj->vector = va_arg (*argp, NW_ADT_DynamicVector_t*);
  if (thisObj->vector == NULL) {
    return  KBrsrFailure;
  }
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj->vector,
                                     &NW_ADT_DynamicVector_Class));

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_ADT_Map_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_ADT_Map_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_ADT_Map_Class));

  /* for convenience */
  thisObj = NW_ADT_MapOf (dynamicObject);

  /* deallocate our dynamic data */
  NW_Object_Delete (thisObj->vector);
}

/* ------------------------------------------------------------------------- */
NW_ADT_Vector_Metric_t
_NW_ADT_Map_Lookup (const NW_ADT_Map_t* thisObj,
                    const void* key)
{
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t start;
  NW_Int32 end;

  /* parameter assertions */
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_Map_Class));

  /* look for the entry using a binary search algorithm */
  index = 0;
  start = 0;
  end = ((NW_Int32) NW_ADT_Vector_GetSize (NW_ADT_VectorOf (thisObj->vector))) - 1;
  while (end >= start) {
    void* entry;
    NW_Int32 match;

    index = (NW_ADT_Vector_Metric_t) ((end + start) / 2);

    entry = NW_ADT_Vector_ElementAt (NW_ADT_VectorOf (thisObj->vector), index);
    match = NW_Mem_memcmp (entry, key, thisObj->keySize);
    if (!match) {
      return index;
    } else if (match > 0) {
      start = (NW_ADT_Vector_Metric_t) (index + 1);
    } else if (match < 0) {
      end = ((NW_Int32)index) - 1;
    }
  }

  /* we didn't find the entry we were looking for, but we return the index of
     the position at which it would have been had it existed */
  if (start > index) {
    return start;
  }
  return index;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_Map_Set (NW_ADT_Map_t* thisObj,
                const void* key,
                const void* value)
{
  NW_ADT_Vector_Metric_t index;
  NW_Uint8* entry;

  /* parameter assertions */
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_Map_Class));
  NW_ASSERT (key != NULL);
  NW_ASSERT (value != NULL);

  /* find the position in the vector where the key is located or should be
     located */
  index = NW_ADT_Map_Lookup (thisObj, key);

  /* get a reference to the existing entry or create a new if it doesn't
     exist */
  entry = (NW_Uint8*)
    NW_ADT_Vector_ElementAt (NW_ADT_VectorOf (thisObj->vector), index);
  if (entry == NULL || NW_Mem_memcmp (entry, key, thisObj->keySize)) {
    entry = (NW_Uint8*) NW_ADT_DynamicVector_InsertAt (thisObj->vector, NULL, index);
    if (entry == NULL) {
      return KBrsrOutOfMemory;
    }
    (void) NW_Mem_memcpy (entry, key, thisObj->keySize);
  }

  /* copy the value into the vector */
  (void) NW_Mem_memcpy (entry + thisObj->keySize, value, thisObj->valueSize);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_Map_Get (const NW_ADT_Map_t* thisObj,
                const void* key,
                void* value)
{
  void** entry;

  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_Map_Class));
  NW_ASSERT (key != NULL);

  /* get the entry pointer for the key */
  entry = NW_ADT_Map_GetUnsafe (thisObj, key);
  if (entry == NULL) {
    return KBrsrNotFound;
  }

  /* copy the contents into the supplied buffer */
  (void) NW_Mem_memcpy (value, entry, thisObj->valueSize);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void**
NW_ADT_Map_GetUnsafe (const NW_ADT_Map_t* thisObj,
                      const void* key)
{
  NW_ADT_Vector_Metric_t index;
  NW_Uint8  *entry;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_Map_Class));
  NW_ASSERT (key != NULL);

  /* find the position in the vector where the key is located or should be
     located */
  index = NW_ADT_Map_Lookup (thisObj, key);

  /* if there is no entry for the key we return NULL */
  entry = (NW_Uint8*)
    NW_ADT_Vector_ElementAt (NW_ADT_VectorOf (thisObj->vector), index);
  if (entry == NULL || NW_Mem_memcmp (entry, key, thisObj->keySize)) {
    return NULL;
  }

  /* return the unaligned pointer to the value portion of the vector entry */
  return (void**) (entry + thisObj->keySize);
} 

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_Map_Remove (NW_ADT_Map_t* thisObj,
                   const void* key)
{
  NW_ADT_Vector_Metric_t index;
  NW_Uint8  *entry;

  /* parameter assertions */
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_Map_Class));
  NW_ASSERT (key != NULL);

  /* find the position in the vector where the key is located or should be
     located */
  index = NW_ADT_Map_Lookup (thisObj, key);

  /* we can't delete the entry if it doesn't exist */
  if (index == NW_ADT_Vector_AtEnd) {
    return KBrsrFailure;
  }

  /* if there is no entry for the key we return KBrsrFailure */
  entry = (NW_Uint8*)
    NW_ADT_Vector_ElementAt (NW_ADT_VectorOf (thisObj->vector), index);
  if (entry == NULL || NW_Mem_memcmp (entry, key, thisObj->keySize)) {
    return KBrsrFailure;
  }

  /* delete the entry */
  return NW_ADT_DynamicVector_RemoveAt (thisObj->vector, index);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_Map_Copy (const NW_ADT_Map_t* thisObj, 
                 NW_ADT_Map_t* dest) 
{
  NW_ADT_MapIterator_t* iterator = NULL;

  NW_TRY (status) {
    void** entry;

    NW_ASSERT(thisObj != NULL);
    NW_ASSERT(dest != NULL);

    iterator = NW_ADT_MapIterator_New(thisObj);
    if (iterator == NULL) {
      return KBrsrOutOfMemory;
    }

    /* Copy all elements */
    while ((entry = NW_ADT_Iterator_NextElement(NW_ADT_IteratorOf(iterator))) != NULL) {
      status = NW_ADT_Map_Set(dest, (void*) entry, (char*) entry + thisObj->keySize);
      _NW_THROW_ON_ERROR(status);
    }

  } NW_CATCH (status) {
  } NW_FINALLY {
    NW_Object_Delete (iterator);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_ADT_Map_Clear (NW_ADT_Map_t* thisObj)
{
  /* parameter assertion */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_Map_Class));

  return NW_ADT_DynamicVector_Clear (thisObj->vector);
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_Map_t*
NW_ADT_Map_New (NW_Uint32 keySize,
                NW_Uint32 elementSize,
                NW_ADT_DynamicVector_t* dynamicVector)
{
  return (NW_ADT_Map_t*)
    NW_Object_New (&NW_ADT_Map_Class, keySize, elementSize, dynamicVector);
}
