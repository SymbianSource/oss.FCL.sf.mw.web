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
* Description:  Handling of general url loading
*
*/



#ifndef CKUrlLoader_H
#define CKUrlLoader_H

//  INCLUDES
#include "urlloader_murlschemehandler.h"
#include "BrsrStatusCodes.h"
#include "BrCtlDefs.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MBrCtlSpecialLoadObserver;
class MHttpLoader_LinkResolverCallbacks;
class CWmlResourceLoadListener;
class MWmlInterface;

// CLASS DECLARATION
/**
*  Url loader implementation class.
*
*  @lib urlloader.lib
*  @since 2.0
*/
class CKUrlLoader : public MUrlSchemeHandler  
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CKUrlLoader* NewL(MBrCtlSpecialLoadObserver* aSpecialLoadObserver,
    						 MHttpLoader_LinkResolverCallbacks* aLinkCallbacks,
							 MWmlInterface* aWKWmlInterface);

    /**
    * Destructor.
    */
    virtual ~CKUrlLoader();

public: // New functions


    /**
    * Make a generic load request
    * @since 2.0
    * @param aUrl The request URL.
    * @param aHeader The request headers or NULL if no request specific headers
    * @param aData The body and body length of the request
    * @param aMethod The method to use for the request could be
	*				 NW_URL_METHOD_GET or NW_URL_METHOD_POST
    * @param aTransId The request's transaction id
    * @param aLoadContext Load context that should be returned with the response
    * @param aLoadCallback The callback to call when the response is ready
    * @param aPartialLoadCallback The callback to call when a chunk is ready
    * @param aCacheMode The cache mode is relevant only in case of http or https
    * @param aDefaultHeadersMask Flag indicating what default headers should be used
	* @param aLoadType The load type
	* @param aIsTopLevel Boolean denoting if this url is top level (not images, css)
    * @return TBrowserStatusCode
    */
    TBrowserStatusCode LoadRequest(const TUint16* aUrl, void* aHeader, TUint8* aData,
                                   TUint aDataLen, TUint8 aMethod, TUint16* aTransId,
                                   void* aLoadContext,
                                   NW_Url_RespCallback_t* aPartialLoadCallback,
                                   NW_Cache_Mode_t aCacheMode, TUint aDefaultHeadersMask,
                                   TUint8 aLoadType, TBool aIsTopLevel, TBool aIsScript,
                                   NW_Uint8 aReason, void* aLoadData);
    


public: // Functions from base classes

    /**
    * From MUrlSchemeHandler If the url scheme is not http, https or file,
	*	send the request to the application architecture.
    * @since 2.0
    * @param aUrl The request URL.
    * @param aHeader The request headers or NULL if no request specific headers
    * @param aData The body and body length of the request
    * @param aMethod The method to use for the request could be
	*				 NW_URL_METHOD_GET or NW_URL_METHOD_POST
    * @param aTransId The request's transaction id
    * @param aLoadContext Load context that should be returned with the response
    * @param aLoadCallback The callback to call when the response is ready
    * @param aPartialLoadCallback The callback to call when a chunk is ready
    * @param aCacheMode The cache mode is relevant only in case of http or https
    * @param aDefaultHeadersMask Flag indicating what default headers should be used
	* @param aLoadType The load type
	* @param aIsTopLevel Boolean denoting if this url is top level (not images, css)
    * @param aIsScript Is it a WmlScript load event?
    * @return ETrue if processed the load request, EFalse otherwise
    */
    virtual TBool HandleLoadRequestL(const TUint16* aUrl, void* aHeader,
                                     TPtr8& aData, TUint8 aMethod, TUint16 aTransId,
                                     void* aLoadContext,
                                     NW_Url_RespCallback_t* aPartialLoadCallback,
                                     NW_Cache_Mode_t aCacheMode,
                                     TUint aDefaultHeadersMask,
                                     TUint8 aLoadType, TBool aIsTopLevel,
                                     TBool aIsScript,  NW_Uint8 aReason,
                                     void* aLoadData);

	/**
    * Checks if the specified url is in the cache
    * @since 3.1
    * @param aUrl The url of the page
    * @return TBool ETrue, if url is in the cache, otherwise EFalse
    */
	TBool IsUrlInCache(const TUint16* aUrl);

private:

    /**
    * C++ default constructor.
    */
    CKUrlLoader();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(MBrCtlSpecialLoadObserver* aSpecialLoadObserver, 
		       MHttpLoader_LinkResolverCallbacks* aLinkCallbacks, MWmlInterface* aWKWmlInterface);

	/**
    * SetOnEnterBackward
    * @since 3.1
    * @param aOnEnterBackward The OnEnterBackward event value
	*/
    void SetOnEnterBackward(TBool aOnEnterBackward){iOnEnterBackward = aOnEnterBackward;}

    void SetOnEnterForward(TBool aOnEnterForward){iOnEnterForward = aOnEnterForward;}
	/**
    * OnEnterBackward
    * @since 3.1
	* @return ETrue if processed the OnEnterBackward event, EFalse otherwise
	*/
    TBool OnEnterBackward(){return iOnEnterBackward;}
    TBool OnEnterForward() {return iOnEnterForward;}

public:	// Data


    // The capabilities that are supported by the "Browser As Plugin".
    // This is passed in as a TUint, but maps to TBrCtlDefs::TBrCtlCapabilities.
    TUint iCapabilities;

private:    // Data

    // A pointer to File loader
    MUrlSchemeHandler* iFileLoader;
    // A popinter to SpecialLoadObserver
    MBrCtlSpecialLoadObserver* iSpecialLoadObserver;
    // The next transaction id to use
    TUint16 iTransId;  
    //The back link to BrCtl
    MHttpLoader_LinkResolverCallbacks* iLinkCallbacks;
    //
    CWmlResourceLoadListener* iResHandle;
	//
	TBool iOnEnterBackward;
 TBool iOnEnterForward;
	MWmlInterface* iWKWmlInterface; //Does not own

};

#endif      // CKUrlLoader_H

// End of File
