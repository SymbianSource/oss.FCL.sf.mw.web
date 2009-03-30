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



// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include "nwx_http_defs.h"
#include "fileloader_CFileLoader.h"
#include "urlloader_loaderutils.h"
#include "fileloader_cfilehandler.h"


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
// CKFileLoader::CKFileLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CKFileLoader::CKFileLoader()
{
}


// -----------------------------------------------------------------------------
// CKFileLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CKFileLoader* CKFileLoader::NewL()
{
    CKFileLoader* self = new( ELeave ) CKFileLoader();

    return self;
}


// Destructor
CKFileLoader::~CKFileLoader()
{
}


// -----------------------------------------------------------------------------
// CKFileLoader::HandleLoadRequestL
// Handles requests for file scheme loading.
// -----------------------------------------------------------------------------
//
TBool CKFileLoader::HandleLoadRequestL(const TUint16          *aUrl,
                                      void                   *aHeader,
                                      TPtr8&                 aData,
                                      TUint8				  aMethod,
                                      TUint16                aTransId,
                                      void                   *aLoadContext,
                                      NW_Url_RespCallback_t* aPartialLoadCallback,
                                      NW_Cache_Mode_t,
                                      TUint,
                                      TUint8                  aLoadType,
                                      TBool,
                                      TBool,
                                      NW_Uint8 ,
                                      void* )
{
    __ASSERT_DEBUG(aUrl != NULL, LoaderUtils::PanicLoader());
    __ASSERT_DEBUG(aPartialLoadCallback != NULL, LoaderUtils::PanicLoader());
    // Check if the URL scheme is file://
    _LIT(KFileScheme, "file://");
    TPtrC urlPtr(aUrl);
    if (urlPtr.FindF(KFileScheme) != 0)
        {
        return EFalse;
        }
   
	// Handle the GET method
	if (aMethod == NW_URL_METHOD_GET)
	{
		__ASSERT_DEBUG(aData.Length() == 0 && aData.Ptr() == NULL,
					   LoaderUtils::PanicLoader());

                //lint -e{774} always eval to false
		if (aUrl == NULL || aPartialLoadCallback == NULL || aHeader != NULL ||
			aData.Length() != 0 || aData.Ptr() != NULL)
		{
			User::Leave(KErrArgument);
		}

        // The browser expects the response to return asynchronously.
        // So we have to use the asynchronous API of RFile
        CFileHandler::LoadFileL(aUrl, aTransId, aLoadType, aLoadContext, aPartialLoadCallback, this);
	}
    else if (aMethod == NW_URL_METHOD_HEAD)
    {
        CFileHandler::GetFileInfoL(aUrl, aTransId, aLoadType, aLoadContext, aPartialLoadCallback, this);
	}
	else
	{
		// We only handle GET or HEAD
		User::Leave(KErrArgument);
	}
    return ETrue;
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
