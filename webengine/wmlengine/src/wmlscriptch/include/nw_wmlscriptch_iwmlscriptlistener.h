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


#ifndef NW_WMLSCH_IWMLSCRIPTLISTENER_H
#define NW_WMLSCH_IWMLSCRIPTLISTENER_H

#include "nw_object_interface.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_WmlsCh_IWmlScriptListener_Class_s NW_WmlsCh_IWmlScriptListener_Class_t;
typedef struct NW_WmlsCh_IWmlScriptListener_s NW_WmlsCh_IWmlScriptListener_t;


/* ------------------------------------------------------------------------- *
   method type prototypes
 * ------------------------------------------------------------------------- */

typedef 
TBrowserStatusCode
(*NW_WmlsCh_IWmlScriptListener_Start_t) (NW_WmlsCh_IWmlScriptListener_t *wmlScrListener,
                                          const NW_Ucs2 *url);

typedef
TBrowserStatusCode 
(*NW_WmlsCh_IWmlScriptListener_Finish_t)(NW_WmlsCh_IWmlScriptListener_t *wmlScrListener,
                                        const TBrowserStatusCode status, const NW_Ucs2 *message);


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_WmlsCh_IWmlScriptListener_ClassPart_s {
   NW_WmlsCh_IWmlScriptListener_Start_t  start;
   NW_WmlsCh_IWmlScriptListener_Finish_t finish;
} NW_WmlsCh_IWmlScriptListener_ClassPart_t;

struct NW_WmlsCh_IWmlScriptListener_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_WmlsCh_IWmlScriptListener_ClassPart_t NW_WmlsCh_IWmlScriptListener;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_WmlsCh_IWmlScriptListener_s {
  NW_Object_Interface_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_WmlsCh_IWmlScriptListener_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_WmlsCh_IWmlScriptListener))

#define NW_WmlsCh_IWmlScriptListenerOf(_object) \
  (NW_Object_Cast (_object, NW_WmlsCh_IWmlScriptListener))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Object_Core_Class_t NW_WmlsCh_IWmlScriptListener_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_WmlsCh_IWmlScriptListener_Start(_wmlScrListener, _url) \
  (NW_Object_Invoke ((_wmlScrListener), NW_WmlsCh_IWmlScriptListener, start) \
    ((_wmlScrListener), (_url)))

#define NW_WmlsCh_IWmlScriptListener_Finish(_wmlScrListener, _status, _message) \
  (NW_Object_Invoke ((_wmlScrListener), NW_WmlsCh_IWmlScriptListener, finish) \
    ((_wmlScrListener), (_status), (_message)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WMLSCH_IWMLSCRIPTLISTENER_H */
