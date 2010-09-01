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

#ifndef NW_HED_URLRESPONSE_H
#define NW_HED_URLRESPONSE_H

#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "nw_text_abstract.h"
#include "urlloader_urlresponse.h"
#include "nw_hed_export.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_UrlResponse_Class_s NW_HED_UrlResponse_Class_t;
typedef struct NW_HED_UrlResponse_s NW_HED_UrlResponse_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_UrlResponse_ClassPart_s {
  void** unused;
} NW_HED_UrlResponse_ClassPart_t;

struct NW_HED_UrlResponse_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_UrlResponse_ClassPart_t NW_HED_UrlResponse;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_UrlResponse_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_Text_t* url;

  NW_Ucs2* rawUrl;
  NW_Uint8 ownRawUrl;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_UrlResponse_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_UrlResponse))

#define NW_HED_UrlResponseOf(_object) \
  (NW_Object_Cast (_object, NW_HED_UrlResponse))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_UrlResponse_Class_t NW_HED_UrlResponse_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_UrlResponse_t*
NW_HED_UrlResponse_New (const NW_Url_Resp_t* response);

NW_HED_EXPORT
const NW_Text_t*
NW_HED_UrlResponse_GetUrl (const NW_HED_UrlResponse_t* thisObj);

NW_HED_EXPORT
const NW_Ucs2*
NW_HED_UrlResponse_GetRawUrl (const NW_HED_UrlResponse_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_UrlResponse_UpdateUrl (NW_HED_UrlResponse_t* thisObj, const NW_Text_t* newUrl);

NW_HED_EXPORT
NW_Bool
NW_HED_UrlResponse_Equal (const NW_HED_UrlResponse_t* urlResponse1, 
    const NW_HED_UrlResponse_t* urlResponse2);

NW_HED_EXPORT
NW_HED_UrlResponse_t*
NW_HED_UrlResponse_Clone (const NW_HED_UrlResponse_t* thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_URLRESPONSE_H */
