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
* Description:  Utilities used by url loader
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include "urlloader_loaderutils.h"
#include <httperr.h>
#include <in_sock.h>
#include <flogger.h>
#include <sslerr.h>
#include "BrsrStatusCodes.h"
#include <inet6err.h>
#include "nwx_logger.h"

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
// LoaderUtils::PanicLoader
// General purpose panic function for url loader related errors.
// -----------------------------------------------------------------------------
//
void LoaderUtils::PanicLoader(TInt aError)
{
    User::Panic(_L("Loader Panic"), aError);
}


// -----------------------------------------------------------------------------
// LoaderUtils::MapErrors
// Map the Symbian errors to rainbow errors.
// -----------------------------------------------------------------------------
//
TBrowserStatusCode LoaderUtils::MapErrors(TInt aError)
    {
    if (aError <= KBrsrFailure && aError > KBrsrLastErrorCode)
        {
        return aError;
        }

    if (aError == KErrSSLAlertUserCanceled)
        {
        return KBrsrCancelled;
        }
    // All SSL errors are mapped into this one, which gets displayed to the user
    if (aError <= SSL_ERROR_BASE && aError > SSL_ERROR_BASE - 200 || 
        aError == KErrHttpCannotEstablishTunnel)
        {
        return KErrSSLAlertHandshakeFailure; 
        }
    
    // Deal with DNS lookup errors
    if ((aError <= KErrInet6NoDestination) && (aError > (KErrInet6NoDestination - 200)))
        {
        return KBrsrHTTP502;
        }
    
    // Deal with unsupported feature error
    if (aError == KErrNotSupported)
        {
        return KErrNotSupported;
        }
    
    // Deal with HTTP errors
    if (aError <= KHttpErrorBase && aError > KHttpErrorBase - 200)
        {
        // Encode errors
        if (aError <= KErrHttpEncodeDefault && aError >= KErrHttpEncodeCookie2)
            {
            return KBrsrFailure;
            }
        // Auth errors
        if (aError == KErrHttpDecodeWWWAuthenticate ||
            aError == KErrHttpDecodeUnknownAuthScheme ||
            aError == KErrHttpDecodeBasicAuth || 
            aError == KErrHttpDecodeDigestAuth)
            {
            return KBrsrMissingAuthHeader;
            }
        
        // Decode errors
        if (aError <= KErrHttpDecodeMalformedDate && aError >= KErrHttpDecodeCookie)
            {
            return KBrsrWmlbrowserBadContent;
            }
        switch (aError)
            {
            case KErrHttpRedirectExceededLimit:
                return KBrsrTooManyRedirects;
            case KErrHttpRedirectNoLocationField:
            case KErrHttpRedirectUseProxy:
                return KBrsrBadRedirect;
            case KErrHttpInvalidUri:
                return KBrsrMalformedUrl;
            default:
                return KBrsrFailure;
            }
        }
    
    
    switch (aError)
        {
        case KErrHostUnreach:
            return KBrsrTcpHostUnreachable;
        case KErrNone:
            return KBrsrSuccess;
        case KErrCancel:
            return KBrsrCancelled;
        case KErrNoMemory:
            return KBrsrOutOfMemory;
        case KErrBadName:
        case KErrPathNotFound:
            return KBrsrFileNotFound;
            
        case KErrAbort:
        case KErrCommsLineFail:
        case KErrNotReady:
            return KBrsrConnFailed;
        case KErrDiskFull:
            return KBrsrFileDiskFullError;
        case KErrTimedOut:
            return KBrsrWaitTimeout;
        case KErrCouldNotConnect:
            return KBrsrMalformedUrl;
        case KErrDisconnected:
            return KBrsrHTTP504;
        default:
            return KBrsrFailure;
        }	// end of switch
    }


// -----------------------------------------------------------------------------
// LoaderUtils::LogLoaderEvents
// Log the request and the response. The url can be long (test case and search
// return case), so we truncate it to 200 chars to avoid a panic.
//
// NOTE: NW_Debug_Log() uses variable argument list (VA_LIST) to handle logging,
// but it is not enabled in the release version.  When debugging you will see a
// panic if you provide a long argument (~1K bytes) to print to the log. 
// -----------------------------------------------------------------------------
//
void LoaderUtils::LogLoaderEvents(TUrlLoaderLogType aUrlLoaderLogType, ...)
{
#if (defined (_DEBUG)) || (defined(_ENABLE_LOGS))
    _LIT(KLogFileName, "default.log");
    _LIT(KLogDirName, "Loaders");
    const TUint kMaxUrlLength = 200;
	const TUint kMaxUrlBuffer = kMaxUrlLength + 1;

    VA_LIST ap;
	TBuf16<300> buf;
	TBuf16<kMaxUrlBuffer> urlBuf;

    switch (aUrlLoaderLogType)
    {
    case ELogTypeRequest:
        {
            _LIT(KFormatRequest, "Request: Tid: %d  Url: %s");
            VA_START(ap, aUrlLoaderLogType);
			TUint tid = VA_ARG(ap, TUint);
			TUint16* urlPtr = VA_ARG(ap, TUint16*);
            VA_END(ap);
			TPtrC fullUrl(urlPtr);			
			TUint urlLength = fullUrl.Length();
			if (urlLength > kMaxUrlLength)
			{
				urlLength = kMaxUrlLength;
			}
			TPtr url((TUint16*)urlBuf.Ptr(), 0, kMaxUrlBuffer);
			url.Copy(fullUrl.Ptr(), urlLength);
			url.ZeroTerminate();
            buf.Format(KFormatRequest(), tid, url.Ptr());
            buf.Append(0);
            RFileLogger::Write(KLogDirName, KLogFileName, EFileLoggingModeAppend, buf);
            break;
        }
    case ELogTypeResponse:
        {
            _LIT(KFormatResponse, "Response: Tid: %d  Url: %s");
            VA_START(ap, aUrlLoaderLogType);
			TUint tid = VA_ARG(ap, TUint);
			TUint16* urlPtr = VA_ARG(ap, TUint16*);
            VA_END(ap);
			TPtrC fullUrl(urlPtr);			
			TUint urlLength = fullUrl.Length();
			if (urlLength > kMaxUrlLength)
			{
				urlLength = kMaxUrlLength;
			}
			TPtr url((TUint16*)urlBuf.Ptr(), 0, kMaxUrlBuffer);
			url.Copy(fullUrl.Ptr(), urlLength);
			url.ZeroTerminate();
            buf.Format(KFormatResponse(), tid, url.Ptr());
            buf.Append(0);
            RFileLogger::Write(KLogDirName, KLogFileName, EFileLoggingModeAppend, buf);
            break;
        }
    case ELogTypeFailedResponse:
        {
            _LIT(KFormatFailedResponse, "Response Failed: Tid: %d  Status code: %d");
            VA_START(ap, aUrlLoaderLogType);
            buf.FormatList(KFormatFailedResponse(), ap);
            VA_END(ap);
            buf.Append(0);
            RFileLogger::Write(KLogDirName, KLogFileName, EFileLoggingModeAppend, buf);
            break;
        }
    case ELogTypeSavedDeck:
        {
            _LIT(KFormatSavedDeck, "ParseSavedDeck complete with return code: %d");
            VA_START(ap, aUrlLoaderLogType);
            buf.FormatList(KFormatSavedDeck(), ap);
            VA_END(ap);
            buf.Append(0);
            RFileLogger::Write(KLogDirName, KLogFileName, EFileLoggingModeAppend, buf);
            break;
        }
    default:
        break;
    }	// end of switch
#else //(defined (_DEBUG)) || (defined(_ENABLE_LOGS))
    NW_REQUIRED_PARAM( aUrlLoaderLogType );
#endif //(defined (_DEBUG)) || (defined(_ENABLE_LOGS))
}


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
