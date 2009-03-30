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


#ifndef NW_HED_EVENTHANDLER_H
#define NW_HED_EVENTHANDLER_H

#include "nw_lmgr_eventhandler.h"
#include "nw_hed_documentnode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_EventHandler_Class_s NW_HED_EventHandler_Class_t;
typedef struct NW_HED_EventHandler_s NW_HED_EventHandler_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_EventHandler_ClassPart_s {
  void** unused;
} NW_HED_EventHandler_ClassPart_t;

struct NW_HED_EventHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_EventHandler_ClassPart_t NW_LMgr_EventHandler;
  NW_HED_EventHandler_ClassPart_t NW_EventHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_EventHandler_s {
  NW_LMgr_EventHandler_t super;

  /* member variables */
  NW_HED_DocumentNode_t* documentNode;
  void* context;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_EventHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_EventHandler))

#define NW_HED_EventHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_HED_EventHandler))

 /* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_EventHandler_Class_t NW_HED_EventHandler_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EventHandler_t*
NW_HED_EventHandler_New (NW_HED_DocumentNode_t* documentNode,
                         void* context);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_EVENTHANDLER_H */
