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


#ifndef _NW_Object_Factory_h_
#define _NW_Object_Factory_h_

#include "nw_object_core.h"
#include <stdarg.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Factory_Class_s NW_Object_Factory_Class_t;
typedef struct NW_Object_Factory_s NW_Object_Factory_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Object_Factory_InitializeObject_t) (NW_Object_Factory_t* factory,
                                         const NW_Object_Class_t* objClass,
                                         NW_Object_t* object,
                                         va_list* argList);

typedef
TBrowserStatusCode
(*NW_Object_Factory_TerminateObject_t) (NW_Object_Factory_t* factory,
                                        NW_Object_t* object);

typedef
TBrowserStatusCode
(*NW_Object_Factory_NewObject_t) (NW_Object_Factory_t* factory,
                                  const NW_Object_Class_t* objClass,
                                  va_list* argList,
                                  NW_Object_t** object);

typedef
TBrowserStatusCode
(*NW_Object_Factory_DeleteObject_t) (NW_Object_Factory_t* factory,
                                     NW_Object_t* object);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Factory_ClassPart_s {
  NW_Object_Factory_InitializeObject_t initializeObject;
  NW_Object_Factory_TerminateObject_t terminateObject;
  NW_Object_Factory_NewObject_t newObject;
  NW_Object_Factory_DeleteObject_t deleteObject;
} NW_Object_Factory_ClassPart_t;

struct NW_Object_Factory_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Factory_ClassPart_t NW_Object_Factory;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Object_Factory_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Object_Factory_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Object_Factory))

#define NW_Object_FactoryOf(_object) \
  (NW_Object_Cast (_object, NW_Object_Factory))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT const NW_Object_Factory_Class_t NW_Object_Factory_Class;
NW_OBJECT_EXPORT const NW_Object_Factory_t NW_Object_Factory;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Object_Factory_InitializeObject(_factory, _objClass, _object, _argList) \
  (NW_Object_Invoke (_factory, NW_Object_Factory, initializeObject) (\
     NW_Object_FactoryOf (_factory), (_objClass), (_object), (_argList)))

#define NW_Object_Factory_TerminateObject(_factory, _object) \
  (NW_Object_Invoke (_factory, NW_Object_Factory, terminateObject) (\
     NW_Object_FactoryOf (_factory), (_object)))

#define NW_Object_Factory_NewObject(_factory, _objClass, _argList, _object) \
  (NW_Object_Invoke (_factory, NW_Object_Factory, newObject) (\
     NW_Object_FactoryOf (_factory), (_objClass), (_argList), (_object)))

#define NW_Object_Factory_DeleteObject(_factory, _object) \
  (NW_Object_Invoke (_factory, NW_Object_Factory, deleteObject) (\
     NW_Object_FactoryOf (_factory), (_object)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Object_Factory_h_ */
