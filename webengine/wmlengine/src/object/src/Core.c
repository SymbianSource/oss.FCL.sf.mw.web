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


#include "nw_object_corei.h"
#include "nw_object_dynamici.h"
#include "nw_object_aggregatei.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
static
const NW_Object_Secondary_Class_t* const*
NW_Object_Core_GetSecondaryList (const NW_Object_Class_t* objClass)
{
  if (NW_Object_IsDerivedFrom (objClass, &NW_Object_Base_Class)) {
    return (const NW_Object_Secondary_Class_t* const*)
      ((NW_Object_Base_Class_t*) objClass)->NW_Object_Base.secondaryList;
  } else if (NW_Object_IsDerivedFrom (objClass, &NW_Object_Aggregate_Class)) {
    return (const NW_Object_Secondary_Class_t* const*)
      ((NW_Object_Aggregate_Class_t*) objClass)->NW_Object_Aggregate.secondaryList;
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_Object_Core_InitializeSecondaryList (NW_Object_t* outer,
                                        const NW_Object_Secondary_Class_t* const* secondaryList,
                                        va_list* argList)
{
  NW_TRY (status) {
    NW_Uint32 index;

    /* iterate through the secondaryList */
    for (index = 0; secondaryList[index] != NULL; index++) {
      const NW_Object_Secondary_Class_t* secondaryClass;
      NW_Object_Core_t* secondary;

      secondaryClass = secondaryList[index];
      secondary = (NW_Object_Core_t*)
        ((char*) outer + secondaryClass->NW_Object_Secondary.offset);

      /* we only initialize secondaries if they haven't already been
         initialized */
      if (secondary->objClass == NULL) {
        status = NW_Object_Core_CallInitialize (secondaryClass, secondary, argList);
        NW_THROW_ON_ERROR (status);
      }
    } 

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
static
void
NW_Object_Core_TerminateSecondaryList (NW_Object_t* outer,
                                       const NW_Object_Secondary_Class_t* const* secondaryList)
{
  NW_Uint32 index;

  /* iterate through the secondaryList */
  for (index = 0; secondaryList[index] != NULL; index++) {
    const NW_Object_Secondary_Class_t* secondaryClass;
    NW_Object_Core_t* secondary;

    secondaryClass = secondaryList[index];
    secondary = (NW_Object_Core_t*)
      ((char*) outer + secondaryClass->NW_Object_Secondary.offset);

    /* we only terminate secondaries if they haven't already been
       terminated */
    if (secondary->objClass != NULL) {
      NW_Object_Core_CallTerminate (secondary);
    }
  } 
}

/* ------------------------------------------------------------------------- */
static
NW_Object_Secondary_t*
NW_Object_Core_QuerySecondaryList (const NW_Object_t* outer,
                                   const NW_Object_Secondary_Class_t* const* secondaryList,
                                   const NW_Object_Class_t* queryClass)
{
  NW_Uint32 index;
  NW_Object_Secondary_t* secondary;

  /* iterate through the secondaryList */
  secondary = NULL;
  for (index = 0; secondary == NULL && secondaryList[index] != NULL; index++) {
    const NW_Object_Secondary_Class_t* secondaryClass;

    secondaryClass = secondaryList[index];
    secondary = (NW_Object_Secondary_t*)
      ((char*) outer + secondaryClass->NW_Object_Secondary.offset);

    /* we only query secondaries if they haven't been terminated */
    if (secondary->super.objClass != NULL) {
      secondary = (NW_Object_Secondary_t*) NW_Object_Core_QuerySecondary (secondary, queryClass);
    }
  }

  if (secondary != NULL && secondary->super.objClass != NULL) {
    return secondary;
  }

  return NULL;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Object_Core_Class_t NW_Object_Core_Class = {
  { /* NW_Object_Core */
    /* super          */ NULL,
    /* querySecondary */ _NW_Object_Core_QuerySecondary
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Core_Initialize (NW_Object_Core_t* core,
                            va_list* argList)
{
  NW_TRY (status) {
    const NW_Object_Core_Class_t* objClass = (NW_Object_Core_Class_t*) NW_Object_GetClass (core);

    /* iterate through the class hierarchy */
    while (((NW_Object_Class_t*) objClass) != &NW_Object_Core_Class) {
      const NW_Object_Secondary_Class_t* const* secondaryList;

      /* get the secondary list of the class */
      secondaryList = NW_Object_Core_GetSecondaryList (objClass);
      if (secondaryList != NULL) {
        status =
          NW_Object_Core_InitializeSecondaryList (core, secondaryList, argList);
        NW_THROW_ON_ERROR (status);
      }

      objClass = (const NW_Object_Core_Class_t*) objClass->NW_Object_Core.super;
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_Object_Core_Terminate (NW_Object_Core_t* core)
{
  const NW_Object_Core_Class_t* objClass = (NW_Object_Core_Class_t*) NW_Object_GetClass (core);

  /* we start with the class of the object */
  objClass = (const NW_Object_Core_Class_t*) NW_Object_GetClass (core);

  /* iterate through the class hierarchy */
  while (((NW_Object_Class_t*) objClass) != &NW_Object_Core_Class) {
    const NW_Object_Secondary_Class_t* const* secondaryList;

    /* get the secondary list of the class */
    secondaryList = NW_Object_Core_GetSecondaryList (objClass);
    if (secondaryList != NULL) {
      NW_Object_Core_TerminateSecondaryList (core, secondaryList);
    }

    objClass = (const NW_Object_Core_Class_t*) objClass->NW_Object_Core.super;
  }
}

/* ------------------------------------------------------------------------- */
NW_Object_t*
_NW_Object_Core_QuerySecondary (const NW_Object_Core_t* core,
                                const NW_Object_Class_t* queryClass)
{
  const NW_Object_Core_Class_t* objClass;

  /* check ourselves first */
  if (NW_Object_IsInstanceOf (core, queryClass)) {
    return (NW_Object_t*) core;
  }

  /* iterate through the class hierarchy */
  objClass = (const NW_Object_Core_Class_t*) NW_Object_GetClass (core);
  while (((NW_Object_Class_t*) objClass) != &NW_Object_Core_Class) {
    const NW_Object_Secondary_Class_t* const* secondaryList;

    /* get the interface list of the class */
    secondaryList = NW_Object_Core_GetSecondaryList (objClass);
    if (secondaryList != NULL) {
      NW_Object_t* secondary;

      secondary =
        NW_Object_Core_QuerySecondaryList (core, secondaryList, queryClass);
      if (secondary != NULL) {
        return secondary;
      }
    }

    /* the interface implementation was not found at this level of the class
       hierarchy, let's try the superclass */
    objClass = (const NW_Object_Core_Class_t*) objClass->NW_Object_Core.super;
  }
  return NULL;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Object_Core_CallInitialize (const NW_Object_Class_t* objClass,
                                NW_Object_Core_t* core,
                                va_list* argList)
{
  /* set the class pointer */
  core->objClass = objClass;

  if (NW_Object_IsDerivedFrom (objClass, &NW_Object_Dynamic_Class)) {
    return _NW_Object_Dynamic_Initialize (core, argList);
  }
  if (NW_Object_IsDerivedFrom (objClass, &NW_Object_Aggregate_Class)) {
    return _NW_Object_Aggregate_Initialize (core, argList);
  }
  return _NW_Object_Core_Initialize (core, argList);
}

/* ------------------------------------------------------------------------- */
void
_NW_Object_Core_CallTerminate (NW_Object_Core_t* core)
{
  if (NW_Object_IsInstanceOf (core, &NW_Object_Dynamic_Class)) {
    _NW_Object_Dynamic_Terminate (core);
  } else if (NW_Object_IsInstanceOf (core, &NW_Object_Aggregate_Class)) {
    _NW_Object_Aggregate_Terminate (core);
  } else {
    _NW_Object_Core_Terminate (core);
  }

  /* set the class pointer */
  *(NW_Object_Class_t**) core = NULL;
}
