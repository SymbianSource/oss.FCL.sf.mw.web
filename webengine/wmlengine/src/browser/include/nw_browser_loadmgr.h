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


#ifndef NW_BROWSER_LOADMGR_H
#define NW_BROWSER_LOADMGR_H

#include <ooc.h>
#include "nw_object_dynamic.h"
#include "nw_mvc_iloadlistener.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- */
typedef struct NW_Browser_LoadMgr_Class_s NW_Browser_LoadMgr_Class_t;
typedef struct NW_Browser_LoadMgr_s NW_Browser_LoadMgr_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_Browser_LoadMgr_ClassPart_s {
  void** unused;
} NW_Browser_LoadMgr_ClassPart_t;

/* ------------------------------------------------------------------------- */
struct NW_Browser_LoadMgr_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Browser_LoadMgr_ClassPart_t NW_Browser_LoadMgr;
};

/* ------------------------------------------------------------------------- */
struct NW_Browser_LoadMgr_s {
  NW_Object_Dynamic_t super;

  /* interfaces */
  NW_MVC_ILoadListener_t NW_MVC_ILoadListener;

  /* member variables */
  objectData* isaObject;
  void* context;
};

/* ------------------------------------------------------------------------- */
#define NW_Browser_LoadMgr_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Browser_LoadMgr))

#define NW_Browser_LoadMgrOf(_object) \
  (NW_Object_Cast (_object, NW_Browser_LoadMgr))

/* ------------------------------------------------------------------------- */
extern const NW_Browser_LoadMgr_Class_t NW_Browser_LoadMgr_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
extern
NW_Browser_LoadMgr_t*
NW_Browser_LoadMgr_New (const objectData* isaObject, void* context);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_BROWSER_LOADMGR_H */
