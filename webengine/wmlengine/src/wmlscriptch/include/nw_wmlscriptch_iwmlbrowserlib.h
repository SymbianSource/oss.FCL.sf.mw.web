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


#ifndef NW_WMLSCH_IWMLBROWSERLIB_H
#define NW_WMLSCH_IWMLBROWSERLIB_H

#include "nwx_http_header.h"
#include "nw_object_interface.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_WmlsCh_IWmlBrowserLib_Class_s NW_WmlsCh_IWmlBrowserLib_Class_t;
typedef struct NW_WmlsCh_IWmlBrowserLib_s NW_WmlsCh_IWmlBrowserLib_t;


/* ------------------------------------------------------------------------- *
   method type prototypes
 * ------------------------------------------------------------------------- */

 
typedef 
TBrowserStatusCode
(*NW_WmlsCh_IWmlBrowserLib_Init_t) (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowser);

typedef 
TBrowserStatusCode
(*NW_WmlsCh_IWmlBrowserLib_GetVar_t) (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowser,
                                      const NW_Ucs2 *var, NW_Ucs2 ** ret_string);

typedef 
TBrowserStatusCode
(*NW_WmlsCh_IWmlBrowserLib_SetVar_t) (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowser,
                                      const NW_Ucs2 *var, const NW_Ucs2 *value);

typedef 
TBrowserStatusCode
(*NW_WmlsCh_IWmlBrowserLib_Go_t) (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowser,
                                  const NW_Ucs2 *url, const NW_Ucs2 *param,
                                  NW_Http_Header_t *header);

typedef 
TBrowserStatusCode
(*NW_WmlsCh_IWmlBrowserLib_Prev_t) (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowser);


typedef 
TBrowserStatusCode
(*NW_WmlsCh_IWmlBrowserLib_NewContext_t) (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowser);

typedef 
TBrowserStatusCode
(*NW_WmlsCh_IWmlBrowserLib_Refresh_t) (NW_WmlsCh_IWmlBrowserLib_t *wmlBrowser);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_WmlsCh_IWmlBrowserLib_ClassPart_s {
   NW_WmlsCh_IWmlBrowserLib_Init_t   init;
   NW_WmlsCh_IWmlBrowserLib_GetVar_t getVar;
   NW_WmlsCh_IWmlBrowserLib_SetVar_t setVar;
   NW_WmlsCh_IWmlBrowserLib_Go_t     go;
   NW_WmlsCh_IWmlBrowserLib_Prev_t   prev;
   NW_WmlsCh_IWmlBrowserLib_NewContext_t newContext;
   NW_WmlsCh_IWmlBrowserLib_Refresh_t    refresh;
} NW_WmlsCh_IWmlBrowserLib_ClassPart_t;

struct NW_WmlsCh_IWmlBrowserLib_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_WmlsCh_IWmlBrowserLib_ClassPart_t NW_WmlsCh_IWmlBrowserLib;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_WmlsCh_IWmlBrowserLib_s {
  NW_Object_Interface_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_WmlsCh_IWmlBrowserLib_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_WmlsCh_IWmlBrowserLib))

#define NW_WmlsCh_IWmlBrowserLibOf(_object) \
  (NW_Object_Cast (_object, NW_WmlsCh_IWmlBrowserLib))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Object_Core_Class_t NW_WmlsCh_IWmlBrowserLib_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_WmlsCh_IWmlBrowserLib_Init(_wmlBrowser) \
  (NW_Object_Invoke ((_wmlBrowser), NW_WmlsCh_IWmlBrowserLib, init) \
    ((_wmlBrowser)))

#define NW_WmlsCh_IWmlBrowserLib_GetVar(_wmlBrowser, _var, _ret_string) \
  (NW_Object_Invoke ((_wmlBrowser), NW_WmlsCh_IWmlBrowserLib, getVar) \
    ((_wmlBrowser), (_var), (_ret_string)))

#define NW_WmlsCh_IWmlBrowserLib_SetVar(_wmlBrowser, _var, _value) \
  (NW_Object_Invoke ((_wmlBrowser), NW_WmlsCh_IWmlBrowserLib, setVar) \
    ((_wmlBrowser), (_var), (_value)))

#define NW_WmlsCh_IWmlBrowserLib_Go(_wmlBrowser, _url, _param, _header) \
  (NW_Object_Invoke ((_wmlBrowser), NW_WmlsCh_IWmlBrowserLib, go) \
    ((_wmlBrowser), (_url), (_param), (_header)))

#define NW_WmlsCh_IWmlBrowserLib_Prev(_wmlBrowser) \
  (NW_Object_Invoke ((_wmlBrowser), NW_WmlsCh_IWmlBrowserLib, prev) \
    ((_wmlBrowser)))

#define NW_WmlsCh_IWmlBrowserLib_NewContext(_wmlBrowser) \
  (NW_Object_Invoke ((_wmlBrowser), NW_WmlsCh_IWmlBrowserLib, newContext) \
    ((_wmlBrowser)))

#define NW_WmlsCh_IWmlBrowserLib_Refresh(_wmlBrowser) \
  (NW_Object_Invoke ((_wmlBrowser), NW_WmlsCh_IWmlBrowserLib, refresh) \
    ((_wmlBrowser)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WMLSCH_IWMLBROWSERLIB_H */
