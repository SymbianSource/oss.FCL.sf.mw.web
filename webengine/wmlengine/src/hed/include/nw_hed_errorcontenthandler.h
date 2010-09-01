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

#ifndef NW_HED_ERRORCONTENTHANDLER_H
#define NW_HED_ERRORCONTENTHANDLER_H

#include "nw_hed_contenthandler.h"
#include "nw_text_ucs2.h"
#include "urlloader_urlresponse.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- */
typedef struct NW_HED_ErrorContentHandler_Class_s NW_HED_ErrorContentHandler_Class_t;
typedef struct NW_HED_ErrorContentHandler_s NW_HED_ErrorContentHandler_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_HED_ErrorContentHandler_ClassPart_s {
  NW_Uint8 foo;
} NW_HED_ErrorContentHandler_ClassPart_t;

struct NW_HED_ErrorContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
  NW_HED_ErrorContentHandler_ClassPart_t NW_HED_ErrorContentHandler;
};

/* ------------------------------------------------------------------------- */
struct NW_HED_ErrorContentHandler_s {
  NW_HED_ContentHandler_t super;

  /* member variables */
  NW_Text_t* title;
  TBrowserStatusCode status;
  NW_Uint8    wsp_status;     /* encoded HTTP return status */

};

/* ------------------------------------------------------------------------- */
#define NW_HED_ErrorContentHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_ErrorContentHandler))

#define NW_HED_ErrorContentHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_HED_ErrorContentHandler))

/* ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_ErrorContentHandler_Class_t NW_HED_ErrorContentHandler_Class;

/* ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_ErrorContentHandler_t*
NW_HED_ErrorContentHandler_New (NW_HED_DocumentNode_t* parent,
                                NW_HED_UrlRequest_t* urlRequest,
                                TBrowserStatusCode status,
                                NW_Uint8 wsp_status);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_ERRORCONTENTHANDLER_H */
