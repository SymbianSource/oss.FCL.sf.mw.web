/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines OOC class structures and public method prototypes.
*
*/


#ifndef NW_EVT_CLEARFIELDEVENT_H
#define NW_EVT_CLEARFIELDEVENT_H

//  INCLUDES
#include "nw_evt_event.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
typedef struct NW_Evt_ClearFieldEvent_Class_s NW_Evt_ClearFieldEvent_Class_t;
typedef struct NW_Evt_ClearFieldEvent_s NW_Evt_ClearFieldEvent_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   OOC class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_Evt_ClearFieldEvent_ClassPart_s {
    void** unused;
} NW_Evt_ClearFieldEvent_ClassPart_t;

struct NW_Evt_ClearFieldEvent_Class_s {
    NW_Object_Core_ClassPart_t NW_Object_Core;
    NW_Object_Base_ClassPart_t NW_Object_Base;
    NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
    NW_Evt_Event_ClassPart_t NW_Evt_Event;
    NW_Evt_ClearFieldEvent_ClassPart_t NW_Evt_ClearFieldEvent;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_Evt_ClearFieldEvent_s {
    NW_Evt_Event_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Evt_ClearFieldEvent_GetClassPart(_object) \
    (NW_Object_GetClassPart(_object, NW_Evt_ClearFieldEvent))

#define NW_Evt_ClearFieldEventOf(_object) \
    (NW_Object_Cast (_object, NW_Evt_ClearFieldEvent))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_EVT_EXPORT const NW_Evt_ClearFieldEvent_Class_t NW_Evt_ClearFieldEvent_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/**
* OOC method for creating a new instance of this class.
* @since 2.5
* @return Pointer to new instance of this class.
*/
NW_Evt_ClearFieldEvent_t*
NW_Evt_ClearFieldEvent_New (void);


/**
* OOC method for initializing this instance of this class.
* @since 2.5
* @param event Ptr to OOC class structure for this class.
* @return Browser status code.
*/
NW_EVT_EXPORT
TBrowserStatusCode
NW_Evt_ClearFieldEvent_Initialize( NW_Evt_ClearFieldEvent_t* event );

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

#endif // NW_EVT_CLEARFIELDEVENT_H
