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


#ifndef NW_HED_ILOADERLISTENER_H
#define NW_HED_ILOADERLISTENER_H

#include "nw_object_interface.h"
#include "NW_HED_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_ILoaderListener_Class_s NW_HED_ILoaderListener_Class_t;
typedef struct NW_HED_ILoaderListener_s NW_HED_ILoaderListener_t;

typedef struct NW_HED_Loader_s NW_HED_Loader_t;

/* ------------------------------------------------------------------------- *
   method type prototypes
 * ------------------------------------------------------------------------- */
typedef
void
(*NW_HED_ILoaderListener_IsLoading_t) (NW_HED_ILoaderListener_t* loaderListener,
                                       NW_HED_Loader_t* loader,
                                       NW_Bool state);

typedef
TBrowserStatusCode
(*NW_HED_ILoaderListener_LoadProgressOn_t)(NW_HED_ILoaderListener_t *loaderListener,
                                           NW_Uint16 transactionId);

typedef
TBrowserStatusCode
(*NW_HED_ILoaderListener_LoadProgressOff_t)(NW_HED_ILoaderListener_t *loaderListener,
                                            NW_Uint16 transactionId);
/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_ILoaderListener_ClassPart_s {
  NW_HED_ILoaderListener_IsLoading_t isLoading;
  NW_HED_ILoaderListener_LoadProgressOn_t loadProgressOn;
  NW_HED_ILoaderListener_LoadProgressOff_t loadProgressOff;
} NW_HED_ILoaderListener_ClassPart_t;

struct NW_HED_ILoaderListener_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_HED_ILoaderListener_ClassPart_t NW_HED_ILoaderListener;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_ILoaderListener_s {
  NW_Object_Interface_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_ILoaderListener_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_ILoaderListener))

#define NW_HED_ILoaderListenerOf(_object) \
  (NW_Object_Cast (_object, NW_HED_ILoaderListener))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_Object_Core_Class_t NW_HED_ILoaderListener_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_ILoaderListener_IsLoading(_loaderListener, _loader, _state) \
  (NW_Object_Invoke ((_loaderListener), NW_HED_ILoaderListener, isLoading) \
    ((_loaderListener), (_loader), (_state)))

#define NW_HED_ILoaderListener_LoadProgressOn(_loaderListener, _transId) \
  (NW_Object_Invoke ((_loaderListener), NW_HED_ILoaderListener, loadProgressOn) \
    ((_loaderListener), (_transId)))

#define NW_HED_ILoaderListener_LoadProgressOff(_loaderListener, _transId) \
  (NW_Object_Invoke ((_loaderListener), NW_HED_ILoaderListener, loadProgressOff) \
    ((_loaderListener), (_transId)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_ILOADERLISTENER_H */
