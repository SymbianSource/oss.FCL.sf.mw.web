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


#ifndef NW_MSG_MESSAGEDISPATCHER_H
#define NW_MSG_MESSAGEDISPATCHER_H

#include "nw_object_base.h"
#include "NW_Msg_EXPORT.h"

#include "nw_msg_message.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Msg_MessageDispatcher_Class_s NW_Msg_MessageDispatcher_Class_t;
typedef struct NW_Msg_MessageDispatcher_s NW_Msg_MessageDispatcher_t;

                                          
/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Msg_MessageDispatcher_ClassPart_s {
  void** unused;
} NW_Msg_MessageDispatcher_ClassPart_t;

struct NW_Msg_MessageDispatcher_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Msg_MessageDispatcher_ClassPart_t NW_Msg_MessageDispatcher;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
 struct NW_Msg_MessageDispatcher_s {
  NW_Object_Core_t super;

#ifdef NEEDS_SYMBIAN_IMPLEMENTATION
/*  NW_Bool isBrowserRunning;  Kimono can't have non-const static data */
#endif

};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Msg_MessageDispatcher_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_Msg_MessageDispatcher))

#define NW_Msg_MessageDispatcherOf(_object) \
  (NW_Object_Cast (_object, NW_Msg_MessageDispatcher))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MSG_EXPORT const NW_Msg_MessageDispatcher_Class_t NW_Msg_MessageDispatcher_Class;
NW_MSG_EXPORT const NW_Msg_MessageDispatcher_t NW_Msg_MessageDispatcher;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
 
#ifdef NEEDS_SYMBIAN_IMPLEMENTATION	
  RBCore uses non-const static data for isBrowserRunning, which does not work with
  the SymbianOS.  
  
#define NW_Msg_MessageDispatcher_SetBrowserState(_thisObj, _state) \
  (NW_Msg_MessageDispatcherOf(_thisObj)->isBrowserRunning = (_state))

#define NW_Msg_MessageDispatcher_GetBrowserState(_thisObj) \
  ((NW_Bool)NW_Msg_MessageDispatcherOf(_thisObj)->isBrowserRunning)

#endif
  
NW_MSG_EXPORT
TBrowserStatusCode
NW_Msg_MessageDispatcher_DispatchMessage (const NW_Msg_MessageDispatcher_t* thisObj,
                                          NW_Msg_Message_t* message, 
                                          MMessageListener* listener);

NW_MSG_EXPORT
TBrowserStatusCode
NW_Msg_MessageDispatcher_MessageHandler (NW_Msg_t* msg);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_MSG_MESSAGEDISPATCHER_H */
