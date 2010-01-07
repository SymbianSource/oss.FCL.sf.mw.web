/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of CWmlContentInterface 
*
*/

// INCLUDES
#include <e32base.h>
#include "BrsrStatusCodes.h"
#include "WmlContentInterface.h"
#include "nwx_http_defs.h"
#include "urlloader_urlresponse.h"
#include "urlloader_loaderutils.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// -----------------------------------------------------------------------------
// CWmlContentInterface::NewL
// Public Class Method
// Two-phased constructor.
// -----------------------------------------------------------------------------
CWmlContentInterface* CWmlContentInterface::NewL()
	{
		CWmlContentInterface* self = new (ELeave) CWmlContentInterface();
		CleanupStack::PushL( self );
		self->ConstructL(  );
		CleanupStack::Pop(); // self
		return self;
	}
// -----------------------------------------------------------------------------
// CWmlContentInterface::CWmlContentInterface
// Public Class Method
// C++ default constructor.
// -----------------------------------------------------------------------------	
CWmlContentInterface::CWmlContentInterface()
	{
	
	}

// -----------------------------------------------------------------------------
// CWmlContentInterface::ConstructL
// Public Class Method
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------	
void CWmlContentInterface::ConstructL()
	{
	
	}
// -----------------------------------------------------------------------------
// CWmlContentInterface::PartialResponse
// Public Class Method
// sends the partial response.
// -----------------------------------------------------------------------------	
TBrowserStatusCode CWmlContentInterface::WMLPartialResponse( const TUint16* aUri,
        									TPtr8& aBody,
        									void* aHeaders,
                                        	TUint8* aContentTypeString,
			                                TUint8* aContentLocationString,
			                                TBool aNoStore,
			                                TUint8* aBoundaryString,
			                                TUint16* aLastModified,
			                                TUint16 aCharset,
			                                TUint8* aCharsetString,
			                                TUint aHttpStatus,
			                                TUint8 aMethod,
			                                TUint16 aTransId,
			                                TInt aChunkIndex,
			                                void* aCertInfo,
			                                TUint aContentLength,
			                                TInt aErr,
			                                void* aLoadContext,
			                                void* aPartialCallback )
{
	TUint convertedHttpStatus = aHttpStatus;

  __ASSERT_DEBUG(aUri != NULL, LoaderUtils::PanicLoader());
  __ASSERT_DEBUG(aPartialCallback != NULL, LoaderUtils::PanicLoader());

  LoaderUtils::LogLoaderEvents(ELogTypeResponse, aTransId, aUri);

  TUint8* body = (TUint8*)aBody.Ptr();

    if (aBody.Length() && aHttpStatus > Success && aHttpStatus < MultipleChoices)
    {
        /* Http status of 2xx is success, we map to OK (200) if there is a body. */
        convertedHttpStatus = Success;
    }

    if (aBody.Length() && aHttpStatus >= MultipleChoices && aHttpStatus < BadRequest)
    {
        /* Display the content, not a generic error, if there is a body. */
        convertedHttpStatus = Success;
    }

    if (aBody.Length() && aHttpStatus >= ServerError && aHttpStatus < 600)
    {
        /* Display the content, not a generic error, if there is a body. */
        convertedHttpStatus = Success;
    }

    // when dealing with http status 300 and 500 check to see if body is present
    // if so we want to treat this as a standard content load and force the
    // http status to be 200 (success)

    if ((body) && (((aHttpStatus >= MultipleChoices) && (aHttpStatus < 400)) || ((aHttpStatus >= ServerError) && (aHttpStatus < 600))))
        {
        aHttpStatus = Success;
        convertedHttpStatus = Success;
        iIsBodyPresent = ETrue;
        }

    if ((iIsBodyPresent && (aChunkIndex == -1)) && (((aHttpStatus >= MultipleChoices) && (aHttpStatus < 400)) || ((aHttpStatus >= ServerError) && (aHttpStatus < 600))))
        {
        aHttpStatus = Success;
        convertedHttpStatus = Success;
        iIsBodyPresent = EFalse;
        }

    TBrowserStatusCode status = (aErr == KErrNone && convertedHttpStatus != Success) ?
                       KBrsrHttpStatus : LoaderUtils::MapErrors(aErr);
		
    NW_Url_Resp_t* urlResp = UrlLoader_UrlResponseNew(aUri, (TUint8*)aBody.Ptr(),
                                                   aBody.Length(), aHeaders,
                                                   aContentTypeString,
                                                   aContentLocationString,
                                                   aNoStore,
                                                   aBoundaryString, 
                                                   aLastModified,
                                                   aCharset,
                                                   aCharsetString,
                                                   aHttpStatus, 
                                                   aMethod,
                                                   aTransId,
                                                   aContentLength);
  if (aCertInfo != NULL)
    {
    urlResp->certInfo = aCertInfo;
    }
  
  CleanupStack::PushL(aContentTypeString);
  CleanupStack::PushL(aContentLocationString);
  CleanupStack::PushL(aBoundaryString);
  CleanupStack::PushL(aLastModified);
  CleanupStack::PushL(body);
  if (urlResp == NULL)
    {
    status = ((NW_Url_RespCallback_t*)aPartialCallback)(KBrsrOutOfMemory, aTransId, aChunkIndex, aLoadContext, NULL);
    CleanupStack::PopAndDestroy(5); // body, aContentTypeString, aContentLocationString
                                    // aBoundaryString, aLastModified
    }
  else
    {
    status = ((NW_Url_RespCallback_t*)aPartialCallback)(status, aTransId, aChunkIndex, aLoadContext, urlResp);
    CleanupStack::Pop(5); // body, aContentTypeString, aContentLocationString, aBoundaryString, aLastModified
    }
  
  return status;
}

void CWmlContentInterface::setUrl(const TDesC& aUrl)
{
    m_resourceUrl = HBufC::New(aUrl.Length() + 1);
    m_resourceUrl->Des().Append((const TUint16*) aUrl.Ptr(), aUrl.Length());
    m_resourceUrl->Des().ZeroTerminate();
}

// -----------------------------------------------------------------------------
// CWmlContentInterface::~CWmlContentInterface()
// Public Class Method
// Donstructor.
// -----------------------------------------------------------------------------
CWmlContentInterface::~CWmlContentInterface()
	{
	
	}
            
// End of File
