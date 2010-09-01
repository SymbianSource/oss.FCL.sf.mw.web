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


#ifndef NW_ACTIVEBOX_H
#define NW_ACTIVEBOX_H

#include "nw_lmgr_box.h"
#include "NW_LMgr_IEventListener.h"
#include "nw_lmgr_eventhandler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ActiveBox_Class_s NW_LMgr_ActiveBox_Class_t;
typedef struct NW_LMgr_ActiveBox_s NW_LMgr_ActiveBox_t;

/* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ActiveBox_ClassPart_s {
  void** unused;
} NW_LMgr_ActiveBox_ClassPart_t;
  
struct NW_LMgr_ActiveBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_LMgr_ActiveBox_s {
  NW_LMgr_Box_t super;

  /* implemented interfaces */
  NW_LMgr_IEventListener_t NW_LMgr_IEventListener;

  /* member variables */
  NW_LMgr_EventHandler_t* eventHandler;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_ActiveBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ActiveBox))

#define NW_LMgr_ActiveBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_ActiveBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_ActiveBox_Class_t NW_LMgr_ActiveBox_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_ACTIVEBOX_H */
