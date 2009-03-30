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


#ifndef NW_HED_LOADER_H
#define NW_HED_LOADER_H

#include "nw_object_dynamic.h"
#include "nw_hed_iloaderlistener.h"
#include "nw_hed_iloadrecipient.h"
#include "nw_hed_urlrequest.h"
#include "nw_adt_dynamicvector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_Loader_Class_s NW_HED_Loader_Class_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_Loader_ClassPart_s {
  void** unused;
} NW_HED_Loader_ClassPart_t;

struct NW_HED_Loader_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_Loader_ClassPart_t NW_HED_Loader;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_Loader_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_HED_ILoaderListener_t* loaderListener;
  NW_ADT_DynamicVector_t* requestList;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_Loader_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_Loader))

#define NW_HED_LoaderOf(_object) \
  (NW_Object_Cast (_object, NW_HED_Loader))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_Loader_Class_t NW_HED_Loader_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_Loader_SetLoaderListener(_loader, _loaderListener) \
  ((NW_HED_LoaderOf (_loader)->loaderListener = (_loaderListener)), \
   KBrsrSuccess)

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Loader_StartLoad (NW_HED_Loader_t* loader,
                    const NW_Text_t* url,
                    NW_Uint8 reason,
                    NW_HED_ILoadRecipient_t* loadRecipient,
                    void* clientData,
                    NW_Uint16* transactionId,
                    NW_Uint8 loadType,
                    NW_Cache_Mode_t cacheMode);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Loader_StartRequest (NW_HED_Loader_t* loader,
                    const NW_HED_UrlRequest_t* urlRequest,
                    NW_HED_ILoadRecipient_t* loadRecipient,
                    void* clientData,
                    NW_Uint16* transactionId, void* aLoadContext );

NW_HED_EXPORT
NW_ADT_Vector_Metric_t
NW_HED_Loader_GetRequestListSize (NW_HED_Loader_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Loader_DeleteLoad (NW_HED_Loader_t* loader,
                          NW_Uint16 transactionId,
                          void* clientData);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Loader_CancelLoad (NW_HED_Loader_t* loader,
                          NW_Uint16 transactionId, 
                          TBrowserStatusCode cancelReason );

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Loader_RemoveTransFromLoadList( NW_HED_Loader_t* thisObj, 
                                       NW_Uint16 transactionId );

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_Loader_CancelAllLoads (NW_HED_Loader_t* loader);

NW_HED_EXPORT
TBrowserStatusCode 
OssPartialRespCallback( TBrowserStatusCode aLoadStatus,
                        NW_Uint16 aTransactionId,
                        NW_Int32 aChunkIndex,
                        void* aLoadContext,
                        NW_Url_Resp_t* aResponse );

NW_HED_EXPORT
NW_ADT_DynamicVector_t* 
GetNW_HED_Loader_LoadQueue();

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_Loader_t*
NW_HED_Loader_New (void);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_LOADER_H */
