/*
* Copyright (c) 2000 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_lmgr_transientpropertylisti.h"
#include "nw_adt_resizablevector.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

const NW_Uint16 initialMapSize = 10;
const NW_Uint16 incrementalMapSizeIncrease = 10;

/* ------------------------------------------------------------------------- */
const NW_LMgr_TransientPropertyList_Class_t NW_LMgr_TransientPropertyList_Class = {
  { /* NW_Object_Core                */
    /* super                         */ &NW_LMgr_PropertyList_Class,
    /* queryInterface                */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                */
    /* interfaceList                 */ NULL
  },
  { /* NW_Object_Dynamic             */
    /* instanceSize                  */ sizeof (NW_LMgr_TransientPropertyList_t),
    /* construct                     */ _NW_LMgr_TransientPropertyList_Construct,
    /* destruct                      */ NULL
  },
  { /* NW_LMgr_PropertyList          */
    /* get                           */ _NW_LMgr_TransientPropertyList_Get,
    /* set                           */ _NW_LMgr_SimplePropertyList_Set,
    /* set                           */ _NW_LMgr_TransientPropertyList_Remove,
    /* clone                         */ _NW_LMgr_TransientPropertyList_Clone,
    /* clear                         */ _NW_LMgr_TransientPropertyList_Clear
  },
  { /* NW_LMgr_SimplePropertyList    */
    /* unused                        */ NW_Object_Unused
  },
  { /* NW_LMgr_TransientPropertyList */
    /* unused                        */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_TransientPropertyList_Construct (NW_Object_Dynamic_t* dynamicObject,
                                          va_list* argp)
{
  TBrowserStatusCode status;
  NW_LMgr_TransientPropertyList_t* thisObj;

  /* invoke our superclass constructor */
  status = _NW_LMgr_SimplePropertyList_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_LMgr_TransientPropertyList_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_LMgr_TransientPropertyListOf (dynamicObject);

  /* initialize the member variables */
  thisObj->basePropertyList = va_arg (*argp, NW_LMgr_PropertyList_t*);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj->basePropertyList,
                                     &NW_LMgr_PropertyList_Class));

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_TransientPropertyList_Get (const NW_LMgr_PropertyList_t* propertyList,
                                    NW_LMgr_PropertyName_t key,
                                    NW_LMgr_PropertyList_Entry_t* entry)
{
  TBrowserStatusCode status;
  NW_LMgr_PropertyList_Entry_t buffer;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_TransientPropertyList_Class));

  /* call our superclass method to find the property on the local list */
  status = _NW_LMgr_SimplePropertyList_Get (propertyList, key, &buffer);
  if (status == KBrsrSuccess) {
    /* if the type of the entry is set to zero, it really should be
       interpreted as a tag showing that the entry has been removed */
    if (buffer.type == 0) {
      return KBrsrNotFound;
    }

    /* copy the entry from the buffer and return */
    *entry = buffer;
    return KBrsrSuccess;
  }

  /* the property was not present on our local property list, so we must query
     the base list */
  propertyList =
    NW_LMgr_TransientPropertyList_GetBasePropertyList (propertyList);
  return NW_LMgr_PropertyList_Get (propertyList, key, entry);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_TransientPropertyList_Remove (NW_LMgr_PropertyList_t* propertyList,
                                       NW_LMgr_PropertyName_t key)
{
  TBrowserStatusCode status2;
  NW_LMgr_PropertyList_Entry_t entry;
  TBrowserStatusCode status1;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_TransientPropertyList_Class));

  /* check whether the property is set on our base property list - this is
     done ahead of the rest of the code to deal with errors in a sensible
     way */
  status2 = NW_LMgr_PropertyList_Get (propertyList, key, &entry);
  if (status2 != KBrsrSuccess && status2 != KBrsrNotFound) {
    return status2;
  }

  /* if the property is set on the local property list, we remove it */
  status1 = _NW_LMgr_SimplePropertyList_Remove (propertyList, key);
  if (status1 != KBrsrSuccess && status1 != KBrsrNotFound) {
    return status1;
  }

  /* we are done if the property is not set on our base property list */
  if (status2 == KBrsrNotFound) {
    return status1;
  }

  /* if we get this far, its because the property was removed from the local
     property list, but it still exists on the base property list - we
     therefore need to 'tag' the property as having been deleted */
  (void) NW_Mem_memset (&entry, 0, sizeof (entry));
  status2 = _NW_LMgr_SimplePropertyList_Set (propertyList, key, &entry);
  if (status2 != KBrsrSuccess) {
    return status2;
  }

  return status1;
}

/* ------------------------------------------------------------------------- */
NW_LMgr_PropertyList_t*
_NW_LMgr_TransientPropertyList_Clone (const NW_LMgr_PropertyList_t* propertyList)
{
  NW_LMgr_TransientPropertyList_t* thisObj;
  NW_LMgr_PropertyList_t* basePropertyList;
  NW_LMgr_PropertyList_t* newPropertyList;
  TBrowserStatusCode status;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_TransientPropertyList_Class));

  /* for convenience */
  thisObj = NW_LMgr_TransientPropertyListOf (propertyList);

  /* clone the base property list */
  basePropertyList = NULL;
  if (thisObj->basePropertyList != NULL) {
    basePropertyList = NW_LMgr_PropertyList_Clone (thisObj->basePropertyList);
    if (basePropertyList == NULL) {
      return NULL;
    }
  }

  /* instantiate a new TransientPropertyList object */
  newPropertyList = (NW_LMgr_PropertyList_t*)
    NW_LMgr_TransientPropertyList_New (initialMapSize,
                                       incrementalMapSizeIncrease,
                                       basePropertyList);
  if (newPropertyList == NULL) {
    NW_Object_Delete (basePropertyList);
    return NULL;
  }

  /* copy the property entries */
  status = NW_LMgr_SimplePropertyList_Copy (propertyList, newPropertyList);
  if (status != KBrsrSuccess) {
    NW_Object_Delete (newPropertyList);
    return NULL;
  }

  /* successful completion */
  return newPropertyList;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_TransientPropertyList_Clear (NW_LMgr_PropertyList_t* propertyList)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (propertyList,
                                     &NW_LMgr_TransientPropertyList_Class));

  propertyList = NW_LMgr_TransientPropertyList_GetBasePropertyList (propertyList);
  return NW_LMgr_PropertyList_Clear (propertyList);
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_TransientPropertyList_t*
NW_LMgr_TransientPropertyList_New (NW_ADT_Vector_Metric_t capacity,
                                   NW_ADT_Vector_Metric_t increment,
                                   NW_LMgr_PropertyList_t* basePropertyList)
{
  return (NW_LMgr_TransientPropertyList_t*)
    NW_Object_New (&NW_LMgr_TransientPropertyList_Class, capacity, increment,
                   basePropertyList);
}
