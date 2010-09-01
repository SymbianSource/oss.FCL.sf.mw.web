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


#ifndef NW_WML1X_EPOC32CONTENTHANDLERI_H
#define NW_WML1X_EPOC32CONTENTHANDLERI_H

#include "nw_wml1x_wml1xcontenthandleri.h"
#include "nw_wml1x_epoc32contenthandler.h"
#include "nw_markup_imagevieweri.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected data
 * ------------------------------------------------------------------------- */
extern const NW_Markup_ImageViewer_Class_t NW_Wml1x_Epoc32ContentHandler_ImageViewer_Class;
extern const NW_Object_Class_t* const NW_Wml1x_Epoc32_InterfaceList[];


/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode
_NW_Wml1x_Epoc32ContentHandler_PartialLoadCallback( NW_HED_DocumentNode_t* documentNode,
                                                    TBrowserStatusCode loadStatus,
                                                    NW_Int32 chunkIndex,
                                                    NW_HED_UrlRequest_t* urlRequest,
                                                    NW_Url_Resp_t* response,
                                                    NW_Uint16 transactionId,
                                                    void* context );

extern
void
_NW_Wml1x_Epoc32ContentHandler_HandleLoadComplete(NW_HED_DocumentNode_t* documentNode,
                                        const NW_HED_UrlRequest_t *urlRequest,
                                        NW_Int16 errorClass,
                                        TBrowserStatusCode error,
                                        NW_Uint16 transactionId,
                                        void* context );

extern
TBrowserStatusCode
_NW_Wml1x_Epoc32ContentHandler_ImageViewer_Load (NW_Markup_ImageViewer_t* imageViewer);

extern
NW_Bool
_NW_Wml1x_Epoc32ContentHandler_ImageViewer_IsNoSave (NW_Markup_ImageViewer_t* imageViewer,
                                                     NW_LMgr_Box_t* imageBox);

extern
NW_Ucs2*
_NW_Wml1x_Epoc32ContentHandler_ImageViewer_GetAltText (NW_Markup_ImageViewer_t* imageViewer,
                                                       NW_LMgr_Box_t* imageBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1X_EPOC32CONTENTHANDLERI_H */
