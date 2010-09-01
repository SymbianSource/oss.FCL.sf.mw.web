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


#include "nw_object_dynamici.h"
#include "nw_object_secondaryi.h"
#include "nw_object_factory.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Object_Initialize (const NW_Object_Class_t* objClass,
                      NW_Object_t* object, ...)
{
  NW_TRY (status) {
    va_list argList;

    va_start (argList, object);
    status =
      NW_Object_Factory_InitializeObject (&NW_Object_Factory, objClass,
                                          object, &argList);
    va_end (argList);
    NW_THROW_ON_ERROR (status);

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
NW_Object_Terminate (NW_Object_t* object)
{
  (void) NW_Object_Factory_TerminateObject (&NW_Object_Factory, object);
}

/* ------------------------------------------------------------------------- */
NW_Object_t*
NW_Object_New (const NW_Object_Class_t* objClass, ...)
{
  TBrowserStatusCode status;
  NW_Object_t* object;
  va_list argList;

  va_start (argList, objClass);
  status = NW_Object_Factory_NewObject (&NW_Object_Factory, objClass, &argList,
                                        &object);
  va_end (argList);
  if (status != KBrsrSuccess) {
    return NULL;
  }
  return object;
}

/* ------------------------------------------------------------------------- */
void
NW_Object_Delete (NW_Object_t* object)
{
  (void) NW_Object_Factory_DeleteObject (&NW_Object_Factory, object);
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_Object_IsDerivedFrom (const NW_Object_Class_t* objClass,
		         const NW_Object_Class_t* superClass)
{
  while (objClass != NULL && objClass != superClass) {
    objClass =
      ((NW_Object_Core_Class_t*) objClass)->NW_Object_Core.super;
  }
  return (NW_Bool) (objClass == superClass);
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_Object_IsInstanceOf (const NW_Object_t* object,
		        const NW_Object_Class_t* superClass)
{
  /* deal with NULL objects */
  if (object == NULL || *(NW_Object_Class_t**) object == NULL) {
    return NW_FALSE;
  }
  return NW_Object_IsDerivedFrom (*(NW_Object_Class_t**) object, superClass);
}

/* ------------------------------------------------------------------------- */
NW_Object_t*
NW_Object_QuerySecondary (const NW_Object_t* object,
                          const NW_Object_Class_t* secondaryClass)
{
  NW_Object_t* primary;

  /* we always perform queries on the primary outer object */
  primary = NW_Object_GetPrimaryOuter (object);
  return NW_Object_Core_QuerySecondary (primary, secondaryClass);
}

/* ------------------------------------------------------------------------- */
NW_Object_t*
NW_Object_GetOuter (const NW_Object_t* object)
{
  if (NW_Object_IsInstanceOf (object, &NW_Object_Secondary_Class)) {
    return (char*) object - NW_Object_Secondary_GetClassPart (object).offset;
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
NW_Object_t*
NW_Object_GetPrimaryOuter (const NW_Object_t* object)
{
  NW_Object_t* outer;

  if ((outer = NW_Object_GetOuter (object)) != NULL) {
    return NW_Object_GetPrimaryOuter (outer);
  }
  return (NW_Object_t*) object;
}
