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


#include "nw_hed_contexti.h"

#include "nw_adt_resizablevector.h"
#include "nwx_statuscodeconvert.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   public consts
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   internal methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static void
NW_HED_Variable_Delete (NW_HED_Context_Variable_t *var)
{
  if (var) {
    NW_String_deleteStorage (&(var->name));
    NW_String_deleteStorage (&(var->value));

    NW_Mem_Free (var);
  }
}

/* ------------------------------------------------------------------------- */
static NW_HED_Context_Variable_t *
NW_HED_Variable_New (const NW_String_t *name, const NW_String_t *value)
{
  NW_HED_Context_Variable_t *var = NULL;

  NW_ASSERT (name);
  NW_ASSERT (value);

  var = (NW_HED_Context_Variable_t*) NW_Mem_Malloc (sizeof (NW_HED_Context_Variable_t));

  if (var) {
    NW_String_deepCopy ((NW_String_t *) &(var->name), (NW_String_t *) name);
    NW_String_deepCopy ((NW_String_t *) &(var->value), (NW_String_t *) value);

    if ((var->name.storage == NULL) || (var->value.storage == NULL)) {
      NW_HED_Variable_Delete (var);
      var = NULL;
    }
  }

  return var;
}

/* ------------------------------------------------------------------------- */
static NW_HED_Context_Variable_t *
NW_HED_Context_Find (const NW_HED_Context_t *thisObj, 
                     const NW_String_t *name)
{
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_t *v;
  void** element;

  NW_ASSERT (thisObj);
  NW_ASSERT (name);

  v = NW_ADT_VectorOf (thisObj->vars);

  for (index = 0; index < NW_ADT_Vector_GetSize (v); index++) {
    element = NW_ADT_Vector_ElementAt (v, index);
    NW_ASSERT (element != NULL);

    if (NW_String_equals (name, &((NW_HED_Context_Variable_t *) *element)->name))
      return (NW_HED_Context_Variable_t *) *element;
  }
  
  return NULL;
}

/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_HED_Context_SetVariable (NW_HED_Context_t *thisObj, 
                            const NW_String_t *name, 
                            const NW_String_t *value)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_HED_Context_Variable_t *var;

  NW_ASSERT (thisObj);
  NW_ASSERT (name);
  
  if ((var = NW_HED_Context_Find (thisObj, name)) != NULL) {
    NW_String_deleteStorage (&(var->value));
    
    status = StatusCodeConvert(NW_String_deepCopy(&(var->value), (NW_String_t *) value));
  }
  else {
    /* the var didn't exist */
    NW_ASSERT (0);
  }

  return status;
}

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_Context_Class_t NW_HED_Context_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_HED_Context_t),
    /* construct               */ _NW_HED_Context_Construct,
    /* destruct                */ _NW_HED_Context_Destruct
  },
  { /* NW_HED_Context          */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_Context_Construct (NW_Object_Dynamic_t* dynamicObject,
                           va_list* argp)
{
  NW_HED_Context_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_Context_Class));
  NW_ASSERT (argp != NULL);

  /* avoid 'unreferenced formal parameter' warnings */
  NW_REQUIRED_PARAM (argp);

  /* for convenience */
  thisObj = NW_HED_ContextOf (dynamicObject);

  /* create the variable list */
  thisObj->vars = (NW_ADT_DynamicVector_t *) (NW_ADT_ResizableVector_New (
      sizeof (NW_HED_Context_Variable_t*), 10, 5));

  if (!thisObj->vars) {
    status = KBrsrOutOfMemory;
  }

  /* successful completion */
  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_Context_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_HED_Context_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_HED_Context_Class));

  /* for convenience */
  thisObj = NW_HED_ContextOf (dynamicObject);

  /* deallocate our dynamic data */
  NW_HED_Context_Reset (thisObj);
  NW_Object_Delete (thisObj->vars);
}

/* ------------------------------------------------------------------------- *
   final functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_Context_Reset (NW_HED_Context_t *thisObj)
{
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_t *v;
  void** element;

  if (thisObj == NULL) {
    return KBrsrSuccess;
  }

  if (thisObj->vars == NULL) {
    return KBrsrSuccess;
  }

  v = NW_ADT_VectorOf (thisObj->vars);

  /* delete each item */
  for (index = 0; index < NW_ADT_Vector_GetSize (v); index++) {
    element = NW_ADT_Vector_ElementAt (v, index);
    NW_ASSERT (element != NULL);

    NW_HED_Variable_Delete ((NW_HED_Context_Variable_t *) *element);
  }

  /* set the vector length to zero */  
  return NW_ADT_DynamicVector_Clear (thisObj->vars);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_Context_SetOrAddVariable (NW_HED_Context_t *thisObj, 
                            const NW_String_t *name, 
                            const NW_String_t *value)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_HED_Context_Variable_t* var;

  NW_ASSERT (name);

  if (thisObj == NULL) {
    return KBrsrFailure;
  }

  /* don't allow the variable to be added more than once */
  if (NW_HED_Context_Find (thisObj, name) != NULL) {
    return NW_HED_Context_SetVariable (thisObj, name, value);
  }

  /* create and append the var */
  var = NW_HED_Variable_New (name, value);
  if (var != NULL) {
    void** address;

    address = NW_ADT_DynamicVector_InsertAt (thisObj->vars, &var, NW_ADT_Vector_AtEnd);
    if (!address) {
      NW_HED_Variable_Delete (var);
      status = KBrsrOutOfMemory;
    }
  }
  else {
    status = KBrsrOutOfMemory;
  }

  return status;
}


/* ------------------------------------------------------------------------- */
const NW_String_t *
NW_HED_Context_GetVariable (const NW_HED_Context_t *thisObj, 
                            const NW_String_t *name)
{
  NW_HED_Context_Variable_t *var;

  NW_ASSERT (name);
  
  if (thisObj == NULL) {
    return NULL;
  }

  if ((var = NW_HED_Context_Find (thisObj, name)) != NULL) {
    return &(var->value);
  }
  else {
    return NULL;
  }
}

/* ------------------------------------------------------------------------- */
const NW_HED_Context_Variable_t *
NW_HED_Context_GetEntry (const NW_HED_Context_t *thisObj, 
                         NW_Uint32 index)
{
  NW_ADT_Vector_Metric_t size;

  if (thisObj == NULL) {
    return NULL;
  }

  size = NW_ADT_Vector_GetSize (thisObj->vars);

  if (index >= size) {
    return NULL;
  }

  return (const NW_HED_Context_Variable_t *) (*NW_ADT_Vector_ElementAt (
      thisObj->vars, (NW_ADT_Vector_Metric_t) index));
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_HED_Context_RemoveEntry (NW_HED_Context_t *thisObj, 
                            NW_Uint32 index)
{
  NW_ADT_Vector_Metric_t size;

  if (thisObj == NULL) {
    return KBrsrFailure;
  }

  size = NW_ADT_Vector_GetSize (thisObj->vars);

  if (index >= size) {
    return KBrsrFailure;
  }

  return NW_ADT_DynamicVector_RemoveAt (thisObj->vars, (NW_ADT_Vector_Metric_t) index);
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_Context_t*
NW_HED_Context_New (void)
{
  return (NW_HED_Context_t*) NW_Object_New (&NW_HED_Context_Class);
}

