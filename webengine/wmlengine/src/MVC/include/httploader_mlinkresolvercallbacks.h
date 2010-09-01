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
* Description:  Callback classes from Link resolver to the UI and back
*
*/

#ifndef MLINKRESOLVERCALLBACKS_H
#define MLINKRESOLVERCALLBACKS_H

#include <brctldefs.h>

#include "BrsrStatusCodes.h"
#include "urlloader_urlresponse.h"

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MBrCtlLinkResolver;
class CWmlResourceLoadListener;

/**
*  Callback class for requests to the UI from Link Resolver.
*
*  @lib httploader.lib
*  @since 2.0
*/
class MHttpLoader_LinkResolverCallbacks
    {

public: // New functions

    /**
    * Called to obtain the current URL.
    * @since 2.8
	* @param 
    * @return The current URL
    */
    virtual HBufC* CurrentUrlLC() = 0;

    /**
    * Called to obtain a pointer to the Link Resolver
    * @since 2.8
    * @return The Browser Control's Link Resolver
    */
    virtual MBrCtlLinkResolver* LinkResolver() = 0;
    

    /**
    * Called to check if the Host Application should be called to resolve the URL
    * @since 2.8
    * @return ETrue if the Host Application will resolve the URL, EFalse otherwise
    */
    virtual TBool ShouldResolveUrl(TBool aIsTopLevel, NW_Uint8 aReason) = 0;
    
    /**
    * Called to Load Url to resolve the URL
    * @since 3.1
    * @return 
    */
    virtual void LoadLinkUrl(const TDesC& aUrl, void* aHeaders ) = 0;
    
    /**
    * Called to Load Url to resolve the URL
    * @since 3.1
    * @return 
    */
    virtual void PostLinkUrl(TDesC& aUrl, TDesC8& aPostData, TDesC& aContentType) = 0;
    	
    /**
    * Called to Load Ebbeded content
    * @since 3.1
    * @return 
    */
    virtual CWmlResourceLoadListener* LoadEmbededUrl( const TDesC& aUrl, void *aLoadContext,
                        void* aPartialLoadCallback, TUint16 aTransId  ) = 0;

	/**
    * Called to set the ONENTERBACKWORD event
    * @since 3.1
    * @return void
    */
    virtual void SetOnEnterBackwordReason() = 0;
	
    /**
    * Convert a charset from string to IANA encoding
    * @since 2.8
    * @return IANA encoding of the charset
    */
    virtual TUint16 ConvertCharset(const TDesC8& aCharset) = 0;

    };

#endif      // MLINKRESOLVERCALLBACKS_H   

// End of File
