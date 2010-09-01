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
* Description:  Handler for loading file scheme.
*
*/



#ifndef CFileLoader_H
#define CFileLoader_H

//  INCLUDES
#include "urlloader_urlloaderint.h"
#include "urlloader_murlschemehandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  File loader implementation class.
*
*  @lib fileloader.lib
*  @since 2.0
*/

class CKFileLoader : public MUrlSchemeHandler
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CKFileLoader* NewL();

    /**
    * Destructor.
    */
    virtual ~CKFileLoader();
 
public: // Functions from base classes

    /**
    * From MUrlSchemeHandler Handles requests for file scheme loading.
    * @since 2.0
    * @param aUrl The request URL.
    * @param aHeader The request headers or NULL if no request specific headers. It is 
    * @param aData The body and body length of the request.
    * @param aMethod The method to use for the request could be NW_URL_METHOD_GET or NW_URL_METHOD_POST .
    * @param aTransId The request's transaction id.
    * @param aLoadContext The load context that should be returned with the response.
    * @param aLoadCallback The callback to call when the response is ready.
    * @param aPartialLoadCallback The callback to call when a chunk is ready.
    * @param aCacheMode The cache mode. It is relevant only in case of http or https scheme.
    * @param aDefaultHeadersMask A flag indicating what default headers should be used.
    * @param aLoadType Used by accept headers
    * @param aIsTopLevel Markup page is top, images, css, etc are not top level
    * @param aIsScript Is it a WmlScript load event?
    * @return ETrue if processed the load request, EFalse otherwise
    */
    virtual TBool HandleLoadRequestL(const TUint16* aUrl,
        void* aHeader,
        TPtr8& aData,
        TUint8 aMethod,
        TUint16 aTransId,
        void* aLoadContext,
        NW_Url_RespCallback_t* aPartialLoadCallback,
        NW_Cache_Mode_t aCacheMode,
        TUint aDefaultHeadersMask,
        TUint8 aLoadType,
        TBool aIsTopLevel,
        TBool aIsScript, 
        NW_Uint8 aReason,
        void* aLoadData);
private:

    /**
    * C++ default constructor.
    */
    CKFileLoader();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

};

#endif      // CKFileLoader_H

// End of File
