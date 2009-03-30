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


#ifndef NW_ADT_BROADCASTER_H
#define NW_ADT_BROADCASTER_H

#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "NW_ADT_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_Broadcaster_Class_s NW_ADT_Broadcaster_Class_t;
typedef struct NW_ADT_Broadcaster_s NW_ADT_Broadcaster_t;

/* ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_ADT_Broadcaster_AddListener_t) (NW_ADT_Broadcaster_t* thisObj,
                                     const NW_Object_t* listener);

typedef
TBrowserStatusCode
(*NW_ADT_Broadcaster_RemoveListener_t) (NW_ADT_Broadcaster_t* thisObj,
                                        const NW_Object_t* listener);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_ADT_Broadcaster_ClassPart_s {
  NW_ADT_Broadcaster_AddListener_t addListener;
  NW_ADT_Broadcaster_RemoveListener_t removeListener;
} NW_ADT_Broadcaster_ClassPart_t;

struct NW_ADT_Broadcaster_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_ADT_Broadcaster_ClassPart_t NW_ADT_Broadcaster;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_ADT_Broadcaster_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_ADT_DynamicVector_t *listeners;
};

/* ------------------------------------------------------------------------- *
   public types
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_ADT_Broadcaster_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_ADT_Broadcaster))

#define NW_ADT_BroadcasterOf(_object) \
  (NW_Object_Cast (_object, NW_ADT_Broadcaster))

#define NW_ADT_Broadcaster_AddListener(_object, _Listener) \
  (NW_Object_Invoke (_object, NW_ADT_Broadcaster, addListener) \
   (NW_ADT_BroadcasterOf (_object), (_Listener)))

#define NW_ADT_Broadcaster_RemoveListener(_object, _Listener) \
  (NW_Object_Invoke (_object, NW_ADT_Broadcaster, removeListener) \
   (NW_ADT_BroadcasterOf (_object), (_Listener)))

 /* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT const NW_ADT_Broadcaster_Class_t NW_ADT_Broadcaster_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_ADT_EXPORT
NW_ADT_Broadcaster_t*
NW_ADT_Broadcaster_New (void);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ADT_BROADCASTER_H */
