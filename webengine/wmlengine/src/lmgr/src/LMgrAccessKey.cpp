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


#include "nw_lmgr_accesskeyi.h"
#include "nw_adt_resizablevector.h"
#include "nwx_string.h"
#include "nw_text_ucs2.h"
#include "nw_lmgr_propertylist.h"
#include "nw_lmgr_box.h"
#include "nw_lmgr_cssproperties.h"
#include "BrsrStatusCodes.h"
/* ------------------------------------------------------------------------- *
   private data
 * ------------------------------------------------------------------------- */

#define NW_LMgr_AccessKey_And 0x80
#define NW_LMgr_AccessKey_Mask 0x3f
#define NW_LMgr_AccessKey_ControlKey 0xf0

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
static
TBrowserStatusCode
NW_LMgr_AccessKey_AddValToList (NW_LMgr_AccessKey_t* thisObj, 
                               NW_Uint32 val, 
                               NW_Bool isAnd)
{
  NW_TRY (status) {
    NW_Bool isValidKeyVal;
    void** entry;

    /* be optimistic, let's assume that that method will succeed */
    status = KBrsrSuccess;

    isValidKeyVal = NW_LMgr_AccessKey_IsValidForOptionsList (thisObj, val);
    if (!isValidKeyVal) {
      return KBrsrBadInputParam;
    }

    if (thisObj->valueList == NULL) {
      thisObj->valueList = (NW_ADT_DynamicVector_t*)
         NW_ADT_ResizableVector_New (sizeof (NW_Uint32), 10, 5 );
      NW_THROW_OOM_ON_NULL (thisObj->valueList, status);
    }

    if (isAnd) {
      NW_Uint32 existing;

      existing = *(NW_Uint32*)
        NW_ADT_Vector_ElementAt (thisObj->valueList,  NW_ADT_Vector_AtEnd);

      existing = (NW_Uint32) (existing | NW_LMgr_AccessKey_And);
      NW_ADT_DynamicVector_ReplaceAt (thisObj->valueList, &existing,
                                      NW_ADT_Vector_AtEnd);
    }

    entry = NW_ADT_DynamicVector_InsertAt (thisObj->valueList, &val,
                                           NW_ADT_Vector_AtEnd);
    NW_THROW_OOM_ON_NULL (entry, status);

} NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_LMgr_AccessKey_Class_t NW_LMgr_AccessKey_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_LMgr_AccessKey_t),
    /* construct               */ _NW_LMgr_AccessKey_Construct,
    /* destruct                */ _NW_LMgr_AccessKey_Destruct
  },
  { /* NW_LMgr_AccessKey       */
    /* isValidForOptionsList   */ NULL,
    /* isValidKeyPress         */ NULL
  }
};

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_AccessKey_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_LMgr_AccessKey_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_LMgr_AccessKey_Class));
  NW_REQUIRED_PARAM(argp);

  /* for convenience */
  thisObj = NW_LMgr_AccessKeyOf (dynamicObject);

  thisObj->valueList = NULL;
  thisObj->label = NULL;

  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
void
_NW_LMgr_AccessKey_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_AccessKey_t* thisObj;

  /* for convenience */
  thisObj = NW_LMgr_AccessKeyOf (dynamicObject);

  /* Destroy our text object */
  if (thisObj->valueList != NULL) {
    NW_Object_Delete (thisObj->valueList);
  }
  if (thisObj->label != NULL){
    NW_Object_Delete(thisObj->label);
  }
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
NW_LMgr_AccessKey_SetFromTextAttr (NW_LMgr_AccessKey_t* accessKey, 
                                   NW_Text_t* val)
{
  NW_Uint8 freeNeeded = NW_FALSE;
  NW_Ucs2* storage = NULL;

  NW_TRY (status) {
    NW_Text_Length_t charCount;

    storage = NW_Text_GetUCS2Buffer (val, NW_Text_Flags_Aligned, &charCount,
                                     &freeNeeded);
    NW_THROW_OOM_ON_NULL (storage, status);

    status = NW_LMgr_AccessKey_AddVal (accessKey, storage, NW_FALSE);
    _NW_THROW_ON_ERROR (status);

  } NW_CATCH (status) {
  } NW_FINALLY {
    if (freeNeeded) {
      NW_Mem_Free (storage);
    }
    return status;
  } NW_END_TRY
}

/* --------------------------------------------------------------------------*/
TBrowserStatusCode
NW_LMgr_AccessKey_AddVal (NW_LMgr_AccessKey_t* accessKey, 
                          NW_Ucs2* val,
                          NW_Bool andOp)
{
  NW_TRY (status) {
    NW_Uint32 charCount;
    NW_Uint32 accessKeyVal;
    NW_Ucs2* p;

    if (val == NULL) {
      return KBrsrMissingInputParam;
    }

    charCount = NW_Str_Strlen (val);
    if (charCount != 1) {
      /* we should have a key map to return the int value - for now ignore it */

      /* storage has more than 1 character - check to see if it's space */
      p = NW_Str_Trim (val, NW_Str_Both);
      NW_THROW_OOM_ON_NULL (p, status);
      charCount = NW_Str_Strlen (p);
      accessKeyVal = *p;
      NW_Mem_Free (p);
      if (charCount != 1) {      
        return KBrsrBadInputParam;
      }    
    } else {
      accessKeyVal = *val;
    }

    status = NW_LMgr_AccessKey_AddValToList (accessKey, accessKeyVal, andOp);
    _NW_THROW_ON_ERROR (status);

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* --------------------------------------------------------------------------*/
NW_Bool
NW_LMgr_AccessKey_MatchKey (NW_LMgr_AccessKey_t* accessKey, 
                           NW_Uint32 v,
                           NW_Bool isControlKey)
{
  NW_ADT_Vector_Metric_t size;
  NW_ADT_Vector_Metric_t index;

  size = NW_ADT_Vector_GetSize (accessKey->valueList);

  for (index = 0; index < size; index++)
  {
    NW_Uint32 val;
    NW_Bool cntrlKey;

    val = *(NW_Uint32*)
      NW_ADT_Vector_ElementAt (accessKey->valueList, index);

    if ((val & NW_LMgr_AccessKey_ControlKey) == NW_LMgr_AccessKey_ControlKey) {
      cntrlKey = NW_TRUE;
    }
    else {
      cntrlKey = NW_FALSE;
    }
    val = val & NW_LMgr_AccessKey_Mask;
    if ((val == v) && (cntrlKey == isControlKey)) {
        if (NW_LMgr_AccessKey_IsValidKeyPress (accessKey, val)) {
            return NW_TRUE;              
        }
    }
  }
  return NW_FALSE;
}

/* --------------------------------------------------------------------------*/

NW_Bool
NW_LMgr_AccessKey_GetDirectKey (NW_LMgr_AccessKey_t* accessKey, NW_Uint32* v)
{
  NW_ADT_Vector_Metric_t size;

  size = NW_ADT_Vector_GetSize (accessKey->valueList);

  /* Expect exactly one entry in key list for a directkey */
  if (size == 1)
  {
    NW_Uint32 val;
    val = *(NW_Uint32*)
      NW_ADT_Vector_ElementAt (accessKey->valueList, 0);
    *v = val;
    return NW_TRUE;
  }
  return NW_FALSE;
}

/* --------------------------------------------------------------------------*/
NW_Bool
NW_LMgr_AccessKey_SetAvailableKey (NW_LMgr_AccessKey_t* accessKey, 
                                  NW_ADT_DynamicVector_t* activeBoxList)
{
  TBrowserStatusCode status;
  NW_LMgr_Property_t prop;
  NW_ADT_Vector_Metric_t size;
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t keyListSize;
  NW_ADT_Vector_Metric_t keyListIndex;

  size = NW_ADT_Vector_GetSize (activeBoxList);

  keyListIndex = 0;
  keyListSize = NW_ADT_Vector_GetSize (accessKey->valueList);

  /* Check every access key in the list. Is that key already in use? */
  while (keyListSize > 0)
  {
    NW_Uint32 val;
    NW_Bool matchFound;
    NW_Bool isControlKey;

    matchFound = NW_FALSE;
    isControlKey = NW_FALSE;

    val = *(NW_Uint32*)
      NW_ADT_Vector_ElementAt (accessKey->valueList, 0);

    if ((val & NW_LMgr_AccessKey_ControlKey) == NW_LMgr_AccessKey_ControlKey) {
      isControlKey = NW_TRUE;
    }
    val = val & NW_LMgr_AccessKey_Mask;

    /* Compare it with the accesskey for each active box. */
    for (index = 0; index < size; index++)
    {
      NW_LMgr_Box_t* box;
      NW_LMgr_AccessKey_t* ak;

      box = *(NW_LMgr_Box_t**)
        NW_ADT_Vector_ElementAt (activeBoxList, index);

      status = NW_LMgr_Box_GetPropertyFromList (box,NW_CSS_Prop_accesskey, &prop);
      if (status != KBrsrSuccess)
        continue;

      ak = (NW_LMgr_AccessKey_t*)(prop.value.object);

      /* Special case. If a single link is split into multiple lines,
       * all the parts point to the same access key. Deleting for
       * one will mess up all. Return no new access key set.
       */
      if (ak == accessKey)
        return NW_FALSE;

      matchFound = NW_LMgr_AccessKey_MatchKey (ak, val, isControlKey);

      /* If key is aleady in use, it's not allowed. Remove it from the list */
      if (matchFound) {
        NW_ADT_DynamicVector_RemoveAt (accessKey->valueList, 0);
        break;
      }
    }
    if (!matchFound)
      break;

    keyListSize = NW_ADT_Vector_GetSize (accessKey->valueList);
  }

  /* Return no new access key set. */
  if (keyListSize == 0) {
    return NW_FALSE;
  }

  /* Now comes the task of removing extra accessKey values. */
  while (keyListIndex < keyListSize)
  {
    NW_Uint32 val;

    val = *(NW_Uint32*)
      NW_ADT_Vector_ElementAt (accessKey->valueList, keyListIndex);

    keyListIndex++;
    if ((val & NW_LMgr_AccessKey_And) != NW_LMgr_AccessKey_And)
      break;
  }
  while (keyListIndex < keyListSize)
  {
    NW_ADT_DynamicVector_RemoveAt (accessKey->valueList, keyListIndex);
    keyListSize = NW_ADT_Vector_GetSize (accessKey->valueList);
  }

  /* Yes, a new access key was set. */
  return NW_TRUE;
}
