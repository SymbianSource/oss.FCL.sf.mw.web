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


#ifndef NW_MSG_MESSAGE_H
#define NW_MSG_MESSAGE_H

#include "nw_object_dynamic.h"
#include "NW_Msg_EXPORT.h"

#include "MSGMessageListener.h"
#include "nwx_msg_api.h"
#include <e32base.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Msg_Message_Class_s NW_Msg_Message_Class_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Msg_Message_ClassPart_s {
  void** unused;
} NW_Msg_Message_ClassPart_t;

struct NW_Msg_Message_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Msg_Message_ClassPart_t NW_Msg_Message;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Msg_Message_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_Msg_Type_t msgId;
  NW_Object_t* data;
  NW_Bool ownsData;
  CActive::TPriority iPriority;
  MMessageListener* messageTarget;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Msg_Message_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Msg_Message))

#define NW_Msg_MessageOf(_object) \
  (NW_Object_Cast (_object, NW_Msg_Message))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MSG_EXPORT const NW_Msg_Message_Class_t NW_Msg_Message_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Msg_Message_GetMessageId(_thisObj) \
  ((NW_Msg_Type_t)NW_Msg_MessageOf(_thisObj)->msgId)

#define NW_Msg_Message_GetData(_thisObj) \
  ((NW_Object_t*)NW_Msg_MessageOf(_thisObj)->data)

#define NW_Msg_Message_SetListener(_thisObj, _listener) \
  (NW_Msg_MessageOf(_thisObj)->messageTarget = (_listener))

#define NW_Msg_Message_GetListener(_thisObj) \
  ((MMessageListener*)NW_Msg_MessageOf(_thisObj)->messageTarget)

#define NW_Msg_Message_SetPriority( _thisObj, _priority ) \
  (NW_Msg_MessageOf(_thisObj)->iPriority = (_priority))

#define NW_Msg_Message_GetPriority( _thisObj ) \
  ((CActive::TPriority)NW_Msg_MessageOf(_thisObj)->iPriority)
/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_MSG_EXPORT
NW_Msg_Message_t*
NW_Msg_Message_New (NW_Msg_Type_t messageId, NW_Object_t* data, NW_Bool ownsData);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_MSG_MESSAGE_H */

