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


#include "nw_adt_broadcasteri.h"
#include "nw_adt_resizablevector.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   public consts
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   internal methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_ADT_Broadcaster_Class_t NW_ADT_Broadcaster_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_ADT_Broadcaster_t),
    /* construct               */ _NW_ADT_Broadcaster_Construct,
    /* destruct                */ _NW_ADT_Broadcaster_Destruct
  },
  { /* NW_ADT_Broadcaster      */
    /* addListener             */ _NW_ADT_Broadcaster_AddListener,
    /* removeListener          */ _NW_ADT_Broadcaster_RemoveListener,
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_Broadcaster_Construct (NW_Object_Dynamic_t* dynamicObject,
                               va_list* argp)
{
  NW_ADT_Broadcaster_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  /* avoid 'unreferenced formal parameter' warnings */
  NW_REQUIRED_PARAM (argp);

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_ADT_Broadcaster_Class));

  /* for convenience */
  thisObj = NW_ADT_BroadcasterOf (dynamicObject);

  /* create the variable list */
  thisObj->listeners = (NW_ADT_DynamicVector_t *) (NW_ADT_ResizableVector_New (
      sizeof (NW_Object_Core_t*), 1, 1));

  if (!thisObj->listeners) {
    status = KBrsrOutOfMemory;
  }

  /* successful completion */
  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_ADT_Broadcaster_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_ADT_Broadcaster_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_ADT_Broadcaster_Class));

  /* for convenience */
  thisObj = NW_ADT_BroadcasterOf (dynamicObject);

  /* deallocate our dynamic data */
  NW_Object_Delete (thisObj->listeners);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_Broadcaster_AddListener (NW_ADT_Broadcaster_t* thisObj,
                                 const NW_Object_t* listener)
{
  void** temp;

  /* parameter assertions */
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_Broadcaster_Class));

  /* add the listener */
  temp = NW_ADT_DynamicVector_InsertAt (thisObj->listeners, 
      (void *) &listener, NW_ADT_Vector_AtEnd);

  return (temp != NULL) ? KBrsrSuccess: KBrsrFailure;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_ADT_Broadcaster_RemoveListener (NW_ADT_Broadcaster_t* thisObj,
                                    const NW_Object_t* listener)
{
  TBrowserStatusCode status;

  /* parameter assertions */
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_ADT_Broadcaster_Class));

  /* add the listener */
  status = _NW_ADT_DynamicVector_RemoveElement (thisObj->listeners, 
      (void *) &listener);

  return status;
}

/* ------------------------------------------------------------------------- *
   final functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_ADT_Broadcaster_t*
NW_ADT_Broadcaster_New (void)
{
  return (NW_ADT_Broadcaster_t*) NW_Object_New (&NW_ADT_Broadcaster_Class);
}

