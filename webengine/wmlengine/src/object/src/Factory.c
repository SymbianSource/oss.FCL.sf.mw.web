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


#include "nw_object_factoryi.h"
#include "nw_object_dynamic.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Object_Factory_Class_t NW_Object_Factory_Class = {
  { /* NW_Object_Core    */
    /* super             */ &NW_Object_Core_Class,
    /* queryInterface    */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Object_Factory */
    /* initializeObject  */ _NW_Object_Factory_InitializeObject,
    /* terminateObject   */ _NW_Object_Factory_TerminateObject,
    /* newObject         */ _NW_Object_Factory_NewObject,
    /* deleteObject      */ _NW_Object_Factory_DeleteObject
  }
};

/* ------------------------------------------------------------------------- */
const NW_Object_Factory_t NW_Object_Factory = {
  { &NW_Object_Factory_Class }
};

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Factory_InitializeObject (NW_Object_Factory_t* factory,
                                     const NW_Object_Class_t* objClass,
                                     NW_Object_t* object,
                                     va_list* argList)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (factory, &NW_Object_Factory_Class));
  NW_ASSERT (NW_Object_IsDerivedFrom (objClass, &NW_Object_Dynamic_Class));
  NW_ASSERT (object != NULL);
  NW_ASSERT (argList != NULL);

  /* avoid unreferenced formal parameter warnings */
  (void) factory;

  return NW_Object_Core_CallInitialize (objClass, object, argList);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Factory_TerminateObject (NW_Object_Factory_t* factory,
                                    NW_Object_t* object)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (factory, &NW_Object_Factory_Class));

  /* avoid unreferenced formal parameter warnings */
  (void) factory;

  /* ok, lets be forgiving and ignore NULL pointers */
  if (object == NULL) {
    return KBrsrBadInputParam;
  }

  /* ensure that we have the pointer to the implementation object and that it
     is of type NW_Object_Dynamic */
  if (!NW_Object_IsInstanceOf (object, &NW_Object_Dynamic_Class)) {
    object = NW_Object_GetPrimaryOuter (object);
    if (!NW_Object_IsInstanceOf (object, &NW_Object_Dynamic_Class)) {
      return KBrsrBadInputParam;
    }
  }

  NW_Object_Core_CallTerminate (object);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Factory_NewObject (NW_Object_Factory_t* factory,
                              const NW_Object_Class_t* objClass,
                              va_list* argList,
                              NW_Object_t** object)
{
  NW_Object_t* newObject;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (factory, &NW_Object_Factory_Class));
  NW_ASSERT (NW_Object_IsDerivedFrom (objClass, &NW_Object_Dynamic_Class));
  NW_ASSERT (argList != NULL);
  NW_ASSERT (object != NULL);

  NW_TRY (status) {
    NW_Uint32 instanceSize;

    /* allocate the object */
    instanceSize =
      ((NW_Object_Dynamic_Class_t*) objClass)->NW_Object_Dynamic.instanceSize;
    newObject = NW_Mem_Malloc (instanceSize);
    NW_THROW_OOM_ON_NULL (newObject, status);

    /* initialize the newly allocated object */
    status = NW_Object_Factory_InitializeObject (factory, objClass,
                                                 newObject, argList);
    NW_THROW_ON_ERROR (status);

    *object = newObject;
  } NW_CATCH (status) {
    (void) NW_Object_Factory_DeleteObject (factory, newObject);
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Factory_DeleteObject (NW_Object_Factory_t* factory,
                                 NW_Object_t* object)
{
  TBrowserStatusCode status;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (factory, &NW_Object_Factory_Class));

  status = NW_Object_Factory_TerminateObject (factory, object);
  if (status != KBrsrBadInputParam) {
    NW_Mem_Free (object);
  }
  return status;
}
