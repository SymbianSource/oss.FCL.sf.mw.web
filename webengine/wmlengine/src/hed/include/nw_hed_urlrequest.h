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

#ifndef NW_HED_URLREQUEST_H
#define NW_HED_URLREQUEST_H

#include "nwx_defs.h"
#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "NW_Text_Abstract.h"
#include "urlloader_urlresponse.h"
#include "nwx_http_header.h"
#include "nwx_buffer.h"
#include "NW_HED_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum NW_HED_UrlRequest_LoadMode_e {
  NW_HED_UrlRequest_LoadNormal,
  NW_HED_UrlRequest_LoadLocal
} NW_HED_UrlRequest_LoadMode_t;

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_UrlRequest_Class_s NW_HED_UrlRequest_Class_t;
typedef struct NW_HED_UrlRequest_s NW_HED_UrlRequest_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_UrlRequest_ClassPart_s {
  void** unused;
} NW_HED_UrlRequest_ClassPart_t;

struct NW_HED_UrlRequest_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_UrlRequest_ClassPart_t NW_HED_UrlRequest;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_UrlRequest_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_Text_t*                    url;
  NW_Uint8                      method;
  NW_Http_Header_t*             header;
  NW_Buffer_t*                  body;
                            
  NW_Uint8                      reason;
                            
  NW_Ucs2*                      rawUrl;
  NW_Uint8                      ownRawUrl;

  NW_HED_UrlRequest_LoadMode_t  loadMode;
  NW_Cache_Mode_t               cacheMode;
  NW_Uint8                      loadType;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_UrlRequest_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_UrlRequest))

#define NW_HED_UrlRequestOf(_object) \
  (NW_Object_Cast (_object, NW_HED_UrlRequest))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_UrlRequest_Class_t NW_HED_UrlRequest_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_UrlRequest_t*
NW_HED_UrlRequest_New (NW_Text_t* url, NW_Uint8 method,
    NW_Http_Header_t* header, NW_Buffer_t* body, NW_Uint8 reason, NW_HED_UrlRequest_LoadMode_t loadMode, NW_Uint8 loadType);

NW_HED_EXPORT
NW_HED_UrlRequest_t*
NW_HED_UrlRequest_Clone (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
const NW_Text_t*
NW_HED_UrlRequest_GetUrl (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
const NW_Ucs2*
NW_HED_UrlRequest_GetRawUrl (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
NW_Uint8
NW_HED_UrlRequest_GetMethod (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
NW_Uint8
NW_HED_UrlRequest_GetReason (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
NW_Uint8
NW_HED_UrlRequest_GetLoadType (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
const NW_Http_Header_t*
NW_HED_UrlRequest_GetHeader (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
const NW_Buffer_t*
NW_HED_UrlRequest_GetBody (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
NW_Cache_Mode_t
NW_HED_UrlRequest_GetCacheMode(const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
NW_Bool
NW_HED_UrlRequest_IsTopLevelRequest (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
NW_Bool
NW_HED_UrlRequest_IsHistoricRequest (const NW_HED_UrlRequest_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_UrlRequest_UpdateUrl (NW_HED_UrlRequest_t* thisObj, const NW_Text_t* newUrl);

NW_HED_EXPORT
NW_Bool
NW_HED_UrlRequest_Equal (const NW_HED_UrlRequest_t* urlRequest1, 
    const NW_HED_UrlRequest_t* urlRequest2);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_UrlRequest_SetMethod (NW_HED_UrlRequest_t* thisObj, NW_Uint8 method);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_UrlRequest_SetReason (NW_HED_UrlRequest_t* thisObj, NW_Uint8 reason);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_UrlRequest_SetCacheMode(NW_HED_UrlRequest_t* thisObj, NW_Cache_Mode_t cacheMode);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_URLREQUEST_H */
