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


#ifndef NW_WML1X_EVENTHANDLER_H
#define NW_WML1X_EVENTHANDLER_H

#include "nw_wml1x_wml1xcontenthandler.h"
#include "nw_lmgr_eventhandler.h"
#include "nw_hed_documentnode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Wml1x_EventHandler_Class_s NW_Wml1x_EventHandler_Class_t;
typedef struct NW_Wml1x_EventHandler_s NW_Wml1x_EventHandler_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Wml1x_EventHandler_ClassPart_s {
  void** unused;
} NW_Wml1x_EventHandler_ClassPart_t;

struct NW_Wml1x_EventHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_EventHandler_ClassPart_t NW_LMgr_EventHandler;
  NW_Wml1x_EventHandler_ClassPart_t NW_EventHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Wml1x_EventHandler_s {
  NW_LMgr_EventHandler_t super;

  /* member variables */
  NW_Wml1x_ContentHandler_t* ch;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Wml1x_EventHandler_ClassPartOf(_object) \
  (((NW_Wml1x_EventHandler_Class_t*) \
   NW_Object_ClassOf(_object))->NW_Wml1x_EventHandler_DeviceContext)

#define NW_Wml1x_EventHandlerOf(_object) \
  ((NW_Wml1x_EventHandler_t*) (_object))

 /* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Wml1x_EventHandler_Class_t NW_Wml1x_EventHandler_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_Wml1x_EventHandler_t*
NW_Wml1x_EventHandler_New (NW_Wml1x_ContentHandler_t* ch);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1X_EVENTHANDLER_H */
