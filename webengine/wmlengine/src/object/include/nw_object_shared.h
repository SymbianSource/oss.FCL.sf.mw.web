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


#ifndef NW_OBJECT_SHARED_H
#define NW_OBJECT_SHARED_H

#include "nw_object_dynamic.h"
#include "nwx_defs.h"
#include <stdarg.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  



/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Shared_Class_s NW_Object_Shared_Class_t;
typedef struct NW_Object_Shared_s NW_Object_Shared_t;

/* ------------------------------------------------------------------------- *
   method type prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Object_Shared_Reserve_t) (NW_Object_Shared_t* object);

typedef
NW_Bool
(*NW_Object_Shared_Release_t) (NW_Object_Shared_t* object);

typedef
TBrowserStatusCode
(*NW_Object_Shared_LockObject_t) (NW_Object_Shared_t* object);

typedef
TBrowserStatusCode
(*NW_Object_Shared_UnlockObject_t) (NW_Object_Shared_t* object);

typedef
TBrowserStatusCode
(*NW_Object_Shared_GetObject_t) (NW_Object_Shared_t* object,
                                 NW_Object_Dynamic_t** sharedObject);

typedef
TBrowserStatusCode
(*NW_Object_Shared_AdoptObject_t) (NW_Object_Shared_t* object,
                                   NW_Object_Dynamic_t* sharedObject);

typedef
TBrowserStatusCode
(*NW_Object_Shared_OrphanObject_t) (NW_Object_Shared_t* object,
                                    NW_Object_Dynamic_t** sharedObject);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Shared_ClassPart_s {
  NW_Object_Shared_Reserve_t reserve;
  NW_Object_Shared_Release_t release;
  NW_Object_Shared_LockObject_t lockObject;
  NW_Object_Shared_UnlockObject_t unlockObject;
  NW_Object_Shared_GetObject_t getObject;
  NW_Object_Shared_AdoptObject_t adoptObject;
  NW_Object_Shared_OrphanObject_t orphanObject;
} NW_Object_Shared_ClassPart_t;

struct NW_Object_Shared_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Object_Shared_ClassPart_t NW_Object_Shared;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Object_Shared_s {
  NW_Object_Base_t super;

  NW_Bool deleteAllowed;
  NW_Int16 refCount;
  NW_Bool locked;
  NW_Object_Dynamic_t* sharedObject;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Object_Shared_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_Object_Shared))

#define NW_Object_SharedOf(_object) \
  (NW_Object_Cast ((_object), NW_Object_Shared))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT const NW_Object_Shared_Class_t NW_Object_Shared_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Object_Shared_Reserve(_object) \
  (NW_Object_Invoke ((_object), NW_Object_Shared, reserve) \
   ((_object)))

#define NW_Object_Shared_Release(_object) \
  (NW_Object_Invoke ((_object), NW_Object_Shared, release) \
   ((_object)))

#define NW_Object_Shared_LockObject(_object) \
  (NW_Object_Invoke ((_object), NW_Object_Shared, lockObject) \
   ((_object)))

#define NW_Object_Shared_UnlockObject(_object) \
  (NW_Object_Invoke ((_object), NW_Object_Shared, unlockObject) \
   ((_object)))

#define NW_Object_Shared_GetObject(_object, _sharedObject) \
  (NW_Object_Invoke ((_object), NW_Object_Shared, getObject) \
   ((_object), (_sharedObject)))

#define NW_Object_Shared_AdoptObject(_object, _sharedObject) \
  (NW_Object_Invoke ((_object), NW_Object_Shared, adoptObject) \
   ((_object), (_sharedObject)))

#define NW_Object_Shared_OrphanObject(_object, _sharedObject) \
  (NW_Object_Invoke ((_object), NW_Object_Shared, orphanObject) \
   ((_object), (_sharedObject)))

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_OBJECT_EXPORT
NW_Object_Shared_t*
NW_Object_Shared_New (NW_Object_Dynamic_t* object);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_OBJECT_SHARED_H */
