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


#include "nw_object_sharedi.h"
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
NW_Object_Shared_Class_t NW_Object_Shared_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* querySecondary          */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Object_Shared_t),
    /* construct               */ _NW_Object_Shared_Construct,
    /* destruct                */ _NW_Object_Shared_Destruct
  },
  { /* NW_Onject_Shared        */
    /* reserve                 */ _NW_Object_Shared_Reserve,
    /* release                 */ _NW_Object_Shared_Release,
    /* lockObject              */ _NW_Object_Shared_LockObject,
    /* unlockObject            */ _NW_Object_Shared_UnlockObject,
    /* getObject               */ _NW_Object_Shared_GetObject,
    /* adoptObject             */ _NW_Object_Shared_AdoptObject,
    /* orphanObject            */ _NW_Object_Shared_OrphanObject
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Shared_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argp)
{
  NW_Object_Shared_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_Object_Shared_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_Object_SharedOf (dynamicObject);

  /* extract the object to be shared */
  thisObj->sharedObject = (NW_Object_Dynamic_t*) va_arg (*argp, NW_Object_Dynamic_t*);

  thisObj->deleteAllowed = NW_FALSE;

  /* successful completion */
  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_Object_Shared_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_Object_Shared_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_Object_Shared_Class));

  /* for convenience */
  thisObj = NW_Object_SharedOf (dynamicObject);

  /* 
  ** never call NW_Object_Delete directly (unless it was never reserved), 
  ** use NW_Object_Shared_Release instead 
  */
  NW_ASSERT ((thisObj->deleteAllowed == NW_TRUE) || (thisObj->refCount == 0));

  /* deallocate our dynamic data */
  NW_Object_Delete (thisObj->sharedObject);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Shared_Reserve (NW_Object_Shared_t* thisObj)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Object_Shared_Class));

  thisObj->refCount++;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_Object_Shared_Release (NW_Object_Shared_t* thisObj)
{
  NW_Bool deleted = NW_FALSE;

  if (thisObj != NULL) {
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Object_Shared_Class));

    thisObj->refCount--;

    NW_ASSERT (thisObj->refCount >= 0);

    /* delete our self if the ref-count reaches zero */
    if (thisObj->refCount <= 0) {
      thisObj->deleteAllowed = NW_TRUE;
      NW_Object_Delete (thisObj);

      deleted = NW_TRUE;
    }
  }

  return deleted;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Shared_LockObject (NW_Object_Shared_t* thisObj)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Object_Shared_Class));

  thisObj->locked = NW_TRUE;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Shared_UnlockObject (NW_Object_Shared_t* thisObj)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Object_Shared_Class));

  thisObj->locked = NW_FALSE;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Shared_GetObject (NW_Object_Shared_t* thisObj,
                             NW_Object_Dynamic_t** sharedObject)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Object_Shared_Class));
  NW_ASSERT (sharedObject);
  NW_ASSERT (NW_Object_IsInstanceOf (*sharedObject, &NW_Object_Dynamic_Class));

  *sharedObject = thisObj->sharedObject;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Shared_AdoptObject (NW_Object_Shared_t* thisObj,
                               NW_Object_Dynamic_t* sharedObject)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Object_Shared_Class));
  
  if (sharedObject != NULL) {
    NW_ASSERT (NW_Object_IsInstanceOf (sharedObject, &NW_Object_Dynamic_Class));
  }

  if (thisObj->locked == NW_TRUE) {
    return KBrsrFailure;
  }

  /* delete the old object */
  if (sharedObject != thisObj->sharedObject) {
    NW_Object_Delete (thisObj->sharedObject);
  }

  thisObj->sharedObject = sharedObject;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Shared_OrphanObject (NW_Object_Shared_t* thisObj,
                                NW_Object_Dynamic_t** sharedObject)
{
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Object_Shared_Class));
  NW_ASSERT (sharedObject);

  if (thisObj->locked == NW_TRUE) {
    return KBrsrFailure;
  }

  *sharedObject = thisObj->sharedObject;
  thisObj->sharedObject = NULL;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Object_Shared_t*
NW_Object_Shared_New (NW_Object_Dynamic_t* object)
{
  return (NW_Object_Shared_t*) NW_Object_New (&NW_Object_Shared_Class,
      object);
}

