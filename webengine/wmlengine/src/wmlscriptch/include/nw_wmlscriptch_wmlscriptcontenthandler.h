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


#ifndef NW_WMLSCRIPT_CONTENTHANDLER_H
#define NW_WMLSCRIPT_CONTENTHANDLER_H

#include "nw_hed_contenthandler.h"
#include "nw_hed_documentroot.h"
#include "nw_text_ucs2.h"
#include "nw_adt_dynamicvector.h"
#include "nw_wmlscriptch_iwmlbrowserlib.h"
#include "nw_wmlscriptch_iwmlscriptlistener.h"
#include "nwx_http_header.h"
#include "nw_scrproxy.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- */
typedef const struct NW_WmlScript_ContentHandler_Class_s NW_WmlScript_ContentHandler_Class_t;
typedef struct NW_WmlScript_ContentHandler_s NW_WmlScript_ContentHandler_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_WmlScript_ContentHandler_ClassPart_s {
  void** unused;
} NW_WmlScript_ContentHandler_ClassPart_t;

struct NW_WmlScript_ContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
  NW_WmlScript_ContentHandler_ClassPart_t NW_WmlScript_ContentHandler;
};

/* ------------------------------------------------------------------------- */
struct NW_WmlScript_ContentHandler_s {
  NW_HED_ContentHandler_t super;

  /* member variables */
  const NW_Ucs2                         *url;
  NW_ScrProxy_t                   *scrProxy;
  NW_WmlsCh_IWmlBrowserLib_t      *wmlBrowserLib;  /* Wml Browser Lib Interface */
  NW_WmlsCh_IWmlScriptListener_t  *wmlScrListener; /* Wml Script Listener Interface */
};

/* ------------------------------------------------------------------------- */
#define NW_WmlScript_ContentHandler_ClassPartOf(_object) \
  (NW_Object_GetClassPart (_object, NW_WmlScript_ContentHandler))

#define NW_WmlScript_ContentHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_WmlScript_ContentHandler))

/* ------------------------------------------------------------------------- */
extern NW_WmlScript_ContentHandler_Class_t NW_WmlScript_ContentHandler_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
const NW_Ucs2* NW_WmlScript_ContentHandler_Get_Url (NW_WmlScript_ContentHandler_t *thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WMLSCRIPT_CONTENTHANDLER_H */
