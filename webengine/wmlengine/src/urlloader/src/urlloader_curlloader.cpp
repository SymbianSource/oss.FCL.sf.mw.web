/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <apgcli.h>
#include <coemain.h>
#include <uri16.h>
#include "urlloader_urlloaderint.h"
#include "urlloader_curlloader.h"
#include "urlloader_loaderutils.h"
#include "fileloader_cfileloader.h"
#include "BrsrStatusCodes.h"
#include <BrCtlSpecialLoadObserver.h>
#include "StringUtils.h"
#include "nwx_defs.h"
#include "httploader_mlinkresolvercallbacks.h"
//#include "LoadListeners.h"
#include "WmlControl.h"
#include "WmlInterface.h"



// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CKUrlLoader::CKUrlLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CKUrlLoader::CKUrlLoader()
{
    iTransId = 0;
	iOnEnterBackward = EFalse;
}


// -----------------------------------------------------------------------------
// CKUrlLoader::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CKUrlLoader::ConstructL(MBrCtlSpecialLoadObserver* aSpecialLoadObserver,
							 MHttpLoader_LinkResolverCallbacks* aLinkCallbacks,
							 MWmlInterface* aWKWmlInterface)
{
    iFileLoader = CKFileLoader::NewL();
    
    //Link back to BrCtl
    iLinkCallbacks = aLinkCallbacks;
    iSpecialLoadObserver = aSpecialLoadObserver;
    iWKWmlInterface = aWKWmlInterface;
}


// -----------------------------------------------------------------------------
// CKUrlLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CKUrlLoader* CKUrlLoader::NewL(MBrCtlSpecialLoadObserver* aSpecialLoadObserver,
							   MHttpLoader_LinkResolverCallbacks* aLinkCallbacks,
							   MWmlInterface* aWKWmlInterface)
{
    CKUrlLoader* self = new( ELeave ) CKUrlLoader;

    CleanupStack::PushL( self );
    self->ConstructL(aSpecialLoadObserver, aLinkCallbacks,aWKWmlInterface);
    CleanupStack::Pop();

    return self;
}


// -----------------------------------------------------------------------------
// CKUrlLoader::~CKUrlLoader
// Destructor.
// -----------------------------------------------------------------------------
//
CKUrlLoader::~CKUrlLoader()
{
    delete iFileLoader;
}


// -----------------------------------------------------------------------------
// CKUrlLoader::HandleLoadRequestL
// If the url scheme is not http, https or file, send the request to the
// application architecture.
// -----------------------------------------------------------------------------
//
TBool CKUrlLoader::HandleLoadRequestL(const TUint16         *aUrl,
                                     void                   *aHeader,
                                     TPtr8&                 aData,
                                     TUint8                 aMethod,
                                     TUint16                aTransId,
                                     void                   *aLoadContext, 
                                     NW_Url_RespCallback_t* aPartialLoadCallback,
                                     NW_Cache_Mode_t        aCacheMode,
                                     TUint                  aDefaultHeadersMask,
                                     TUint8                 aLoadType,
                                     TBool                  aIsTopLevel,
                                     TBool                  aIsScript,
                                     NW_Uint8               aReason,
                                     void*                  aLoadData)
    {
    // Go through the loaders until we find one that can handle the request

	if ( aReason == NW_HED_UrlRequest_Reason_DocPrev || aReason ==  NW_HED_UrlRequest_Reason_ShellPrev )
		{
    		    SetOnEnterBackward(ETrue);                
                }                        
	
	// LinkResolver
    if ( aReason == NW_HED_UrlRequest_Reason_DocLoad || 
         (aReason == NW_HED_UrlRequest_Reason_DocPrev || aReason ==  NW_HED_UrlRequest_Reason_ShellPrev) && OnEnterBackward())
        {
        TPtrC desUrl(aUrl);
        if( aMethod == NW_URL_METHOD_POST)
        	{	
        	HttpRequestHeaders* header = (HttpRequestHeaders*)aHeader;
        	TInt headerLen = User::StringLength(header->contentType);
        	HBufC8* contType8 = HBufC8::NewL( headerLen );
        	CleanupStack::PushL( contType8 );
        	contType8->operator=(header->contentType);
        	HBufC* contType = HBufC::NewL( headerLen + 1);
        	CleanupStack::PushL( contType );
        	contType->Des().Copy( contType8->Des() );
        	contType->Des().ZeroTerminate();
        	iLinkCallbacks->PostLinkUrl( desUrl, aData, *contType );
        	CleanupStack::PopAndDestroy(2);
        	SetOnEnterBackward(EFalse);
        	}
        else
        	{
        	iLinkCallbacks->LoadLinkUrl( desUrl, aHeader );	
        	}	
        return ETrue;
        }
    // ImageLoader
    else if ( aReason == NW_HED_UrlRequest_Reason_DocLoadChild && aLoadType == NW_UrlRequest_Type_Image)//aLoadType == 2
    	{
        TPtrC desUrl(aUrl);
        iResHandle = iLinkCallbacks->LoadEmbededUrl(desUrl, aLoadContext, (void*)aPartialLoadCallback, aTransId);
			return ETrue;
			}
	
	if ( aReason == NW_HED_UrlRequest_Reason_ShellLoad && OnEnterBackward() )
		{
		aReason = NW_HED_UrlRequest_Reason_DocPrev;
		iLinkCallbacks->SetOnEnterBackwordReason();
		SetOnEnterBackward(EFalse);
		}
        
    // FileLoader
    if ( iFileLoader->HandleLoadRequestL(aUrl, aHeader, aData, aMethod,
        aTransId, aLoadContext, aPartialLoadCallback, aCacheMode,
        aDefaultHeadersMask, aLoadType, aIsTopLevel, aIsScript,
        aReason, aLoadData))
        {
        return ETrue;
        }
        
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CKUrlLoader::LoadRequest
// Make a generic load request.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode CKUrlLoader::LoadRequest(const TUint16* aUrl, void* aHeader,
									       TUint8* aData, TUint aDataLen,
									       TUint8 aMethod, TUint16* aTransId,
									       void* aLoadContext,
                                           NW_Url_RespCallback_t* aPartialLoadCallback,
                                           NW_Cache_Mode_t aCacheMode,
                                           TUint aDefaultHeadersMask,
                                           TUint8 aLoadType,
                                           TBool aIsTopLevel, 
                                           TBool aIsScript,
                                           NW_Uint8 aReason,
                                           void* aLoadData)
{
    if (aUrl == NULL || (aData == NULL && aDataLen > 0) ||
        aTransId == NULL || aPartialLoadCallback == NULL)
    {
        return KBrsrBadInputParam;
    }

    *aTransId = ++iTransId;
    LoaderUtils::LogLoaderEvents(ELogTypeRequest, *aTransId, aUrl);

    TPtr8 data(aData, aDataLen, aDataLen);

    TRAPD(err, HandleLoadRequestL(aUrl, aHeader, data, aMethod, iTransId,
                                  aLoadContext, aPartialLoadCallback,
                                  aCacheMode, aDefaultHeadersMask, aLoadType,
                                  aIsTopLevel, aIsScript, aReason, aLoadData));

    return LoaderUtils::MapErrors(err);
}


// -----------------------------------------------------------------------------
// CKUrlLoader::IsUrlInCache
//
// -----------------------------------------------------------------------------
//
TBool CKUrlLoader::IsUrlInCache(const TUint16* aUrl)
{
	if (aUrl == NULL)
	{
		return EFalse;
	}

	TBool ret = EFalse;
	TPtrC urlDes(aUrl);
	if( iWKWmlInterface )
		{
		ret = iWKWmlInterface->wmlIsUrlInCache(urlDes);
		}
	return ret;
}


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
