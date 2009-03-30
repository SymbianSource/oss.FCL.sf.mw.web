/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides an interface observe/track load requests.
*
*/



#ifndef MHEDLOADOBSERVER_H
#define MHEDLOADOBSERVER_H

// INCLUDES
#include "BrsrStatusCodes.h"

// FORWARD DECLARATIONS

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct NW_Url_Resp_s NW_Url_Resp_t;
typedef struct NW_HED_UrlRequest_s NW_HED_UrlRequest_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */


// CLASS DECLARATION

class MHEDLoadObserver
    {
    public:  // Interface
        /**
        * Passes a HEAD response to the observer.
        *
        * @param aTransactionId the transaction id.
        * @param aResponse the HEAD response to be processed.
        * @param aUrlRequest the url request.
        * @param aClientData the client data past with the request.
        * @return void.
        */
        virtual void HeadCompleted(TUint16 aTransactionId, const NW_Url_Resp_t& aResponse, 
                const NW_HED_UrlRequest_t& aUrlRequest, void* aClientData) = 0;

        /**
        * Passes a chunk of a response to the observer.
        *
        * @param aTransactionId the transaction id.
        * @param aChunkIndex the chunk count of this call, aChunkIndex > 0
        * @param aResponse the HEAD response to be processed.
        * @param aUrlRequest the url request.
        * @param aClientData the client data past with the request.
        * @return void.
        */
        virtual void ChunkReceived(TUint16 aTransactionId, TUint32 aChunkIndex, 
                const NW_Url_Resp_t& aResponse, const NW_HED_UrlRequest_t& aUrlRequest, 
                void* aClientData) = 0;

        /**
        * Passes a chunk of a response to the observer.
        *
        * @param aStatusClass the type of status-code found in aStatus -- 
                 BRSR_STAT_CLASS_HTTP or BRSR_STAT_CLASS_GENERAL.
        * @param aStatus the final load status of the request.
        * @param aTransactionId the transaction id.
        * @param aUrlRequest the url request.
        * @param aClientData the client data past with the request.
        * @return void.
        */
        virtual void LoadCompleted(TInt16 aStatusClass, TBrowserStatusCode aStatus, 
                TUint16 aTransactionId, const NW_HED_UrlRequest_t& aUrlRequest, 
                void* aClientData) = 0;
    };

#endif  // MHEDLOADOBSERVER_H
