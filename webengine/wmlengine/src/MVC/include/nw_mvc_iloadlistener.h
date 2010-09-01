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


#ifndef _NW_MVC_ILoadListener_h_
#define _NW_MVC_ILoadListener_h_

#include "NW_MVC_EXPORT.h"
#include "nw_object_interface.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_MVC_ILoadListener_Class_s NW_MVC_ILoadListener_Class_t;
typedef struct NW_MVC_ILoadListener_s NW_MVC_ILoadListener_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
void
(*NW_MVC_ILoadListener_LoadComplete_t) (NW_MVC_ILoadListener_t* loadListener,
                                        NW_Bool success);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_MVC_ILoadListener_ClassPart_s {
  NW_MVC_ILoadListener_LoadComplete_t loadComplete;
} NW_MVC_ILoadListener_ClassPart_t;

struct NW_MVC_ILoadListener_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
  NW_MVC_ILoadListener_ClassPart_t NW_MVC_ILoadListener;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_MVC_ILoadListener_s {
  NW_Object_Secondary_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_MVC_ILoadListener_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_MVC_ILoadListener))

#define NW_MVC_ILoadListenerOf(_object) \
  (NW_Object_Cast (_object, NW_MVC_ILoadListener))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MVC_EXPORT const NW_MVC_ILoadListener_Class_t NW_MVC_ILoadListener_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_MVC_ILoadListener_LoadComplete(_object, _success) \
  (NW_Object_Invoke (_object, NW_MVC_ILoadListener, loadComplete) ( \
     NW_MVC_ILoadListenerOf (_object), (_success)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_MVC_ILoadListener_h_ */
