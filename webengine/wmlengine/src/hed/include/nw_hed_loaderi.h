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


#ifndef NW_HED_LOADERI_H
#define NW_HED_LOADERI_H

#include "nw_object_dynamici.h"
#include "nw_hed_loader.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_Loader_LoadQEntry_s {
  void* clientData;
  NW_Uint16 transactionId;
  TBrowserStatusCode loadStatus;
} NW_HED_Loader_LoadQEntry_t;

typedef struct NW_HED_Loader_Context_s {
  NW_HED_UrlRequest_t* urlRequest;
  NW_HED_ILoadRecipient_t* loadRecipient;
  void* clientData;
} NW_HED_Loader_Context_t;

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_Loader_Construct (NW_Object_Dynamic_t* dynamicObject,
                          va_list* argp);

NW_HED_EXPORT
void
_NW_HED_Loader_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_HED_EXPORT
TBrowserStatusCode 
OssPartialRespCallback( TBrowserStatusCode aLoadStatus,
                        NW_Uint16 aTransactionId,
                        NW_Int32 aChunkIndex,
                        void* aLoadContext,
                        NW_Url_Resp_t* aResponse );

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_LOADERI_H */
