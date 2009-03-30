/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  BrCtlApiTest
*
*/

// INCLUDE FILES
#include <apmrec.h>
#include <apgcli.h>
#include <eikappui.h>
#include <eikenv.h>
#include <eikbtgpc.h>
#include "BrCtlApiTestObserver.h"
#include <avkon.hrh>
#include <aknmessagequerydialog.h>

#ifndef RD_PF_SEC_APPARC
//#include <genericparam.h>  
#else
#include "AiwGenericParam.h"
#endif


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrCtlApiTestObserver::UpdateBrowserVScrollBarL
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::UpdateBrowserVScrollBarL(TInt /* aDocumentHeight */, 
                                      TInt /* aDisplayHeight */,
                                      TInt /* aDisplayPosY */ ) 
    {
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::UpdateBrowserHScrollBarL
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::UpdateBrowserHScrollBarL(TInt /* aDocumentWidth */, 
                                      TInt /* aDisplayWidth */,
                                      TInt /* aDisplayPosX */ ) 
    {
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::NotifyLayoutChange
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::NotifyLayoutChange( TBrCtlLayout /* aNewLayout */) 
    {
    }

// ---------------------------------------------------------
// CBrCtlSampleAppLayoutObserver::UpdateTitle
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::UpdateTitleL( const TDesC& /*aTitle*/ )
    {
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::UpdateSoftkeyL
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::UpdateSoftkeyL(TBrCtlKeySoftkey /* aKeySoftkey */,
                            const TDesC& /* aLabel */,
                            TUint32 /* aCommandId */,
                            TBrCtlSoftkeyChangeReason /* aBrCtlSoftkeyChangeReason */) 
    {
    return; 
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::NetworkConnectionNeededL
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::NetworkConnectionNeededL(TInt* /*aConnectionPtr*/,
                                      TInt* /*aSockSvrHandle*/,
                                      TBool* /*aNewConn*/,
                                      TApBearerType* /*aBearerType*/) 
    {
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::HandleRequestL
// ---------------------------------------------------------
//
TBool CBrCtlApiTestObserver::HandleRequestL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray) 
    {

    TInt i;
	
    // The 2 arrays must be in sync. Each element in iTypeArray 
    // identifies the type of the corresponding element in iDesArray.
    if( aTypeArray->Count() != aDesArray->Count() )
        {
        User::Leave( KErrArgument );
        }

	// get url
    HBufC* url = NULL;
    // Serach url in the param list
    for(i = 0; i < aTypeArray->Count(); i++)
        {
        if( (*aTypeArray)[i] == EParamRequestUrl )
            {
            // the url is found
            url = HBufC::NewLC( (*aDesArray)[i].Length() );
            url->Des().Copy( (*aDesArray)[i] );
            break;
            }
        }
    if( !url )
        {
        // The url wasn't found. 
        User::Leave( KErrArgument );
        }

	// get referrer header
    HBufC* refererHeader = NULL;
    // Serach url in the param list
    for( i = 0; i < aTypeArray->Count(); i++ )
        {
        if( (*aTypeArray)[i] == EParamRefererHeader )
            {
            // the referer Header is found
            refererHeader = HBufC::NewLC( (*aDesArray)[i].Length() );
            refererHeader->Des().Copy( (*aDesArray)[i] );
            break;
            }
        }

    if( refererHeader )
		{
			CleanupStack::PopAndDestroy( 1 ); // refererHeader
		}

    CleanupStack::PopAndDestroy( 1 ); // url

    return ETrue;
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::HandleDownloadL
// ---------------------------------------------------------
//
TBool CBrCtlApiTestObserver::HandleDownloadL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray)
    {
    TInt i = 0;
    TInt count = aTypeArray->Count();
    for (i = 0; i < count; i++)
        {
        if ((*aTypeArray)[i] == EParamLocalFileName && aDesArray[i].Length() > 0)
            {
            TInt j = 0;
            for (j = 0; j < count; j++)
                {
                if ((*aTypeArray)[j] == EParamReceivedContentType)
                    {
                    HBufC8* dataType8 = HBufC8::NewLC((*aDesArray)[j].Length());
                    dataType8->Des().Copy((*aDesArray)[j]);
                    TDataType dataType(*dataType8);
                    iHandler->OpenFileEmbeddedL((*aDesArray)[i], dataType);
                    CleanupStack::PopAndDestroy();
                    break;
                    }
                }
            break;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::HandleBrowserLoadEventL
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::HandleBrowserLoadEventL( TBrCtlDefs::TBrCtlLoadEvent /* aLoadEvent */,
                                                 TUint /* aSize */, TUint16 /* aTransactionId */ ) 
    {
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::ResolveEmbeddedLinkL
// ---------------------------------------------------------
//
TBool CBrCtlApiTestObserver::ResolveEmbeddedLinkL(const TDesC& aEmbeddedUrl,
    		                       const TDesC& /* aCurrentUrl */,
        	                       TBrCtlLoadContentType /* aLoadContentType */, 
                                   MBrCtlLinkContent& aEmbeddedLinkContent) 
    {
    if (IsFileScheme(aEmbeddedUrl))
        {
        GetFileNameL(aEmbeddedUrl);
        HBufC8* buf = ReadFileLC(*iFileName);
        HBufC* contentType = NULL;
        TPtrC p(NULL, 0);
        contentType = RecognizeLC(*iFileName, *buf);
        aEmbeddedLinkContent.HandleResolveComplete(*contentType, p, buf);        
        CleanupStack::PopAndDestroy(2); // contentType, buf
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::ResolveLinkL
// ---------------------------------------------------------
//
TBool CBrCtlApiTestObserver::ResolveLinkL(const TDesC& aUrl, const TDesC& /* aCurrentUrl */,
                   MBrCtlLinkContent& aBrCtlLinkContent) 
    {
    if (IsFileScheme(aUrl))
        {
        GetFileNameL(aUrl);
        HBufC8* buf = ReadFileLC(*iFileName);
        HBufC* contentType = NULL;
        TPtrC p(NULL, 0);
        contentType = RecognizeLC(*iFileName, *buf);
        aBrCtlLinkContent.HandleResolveComplete(*contentType, p, buf);        
        CleanupStack::PopAndDestroy(2); // contentType, buf
        return ETrue;
        }
    return EFalse;
    }


// ---------------------------------------------------------
// CBrCtlApiTestObserver::CancelAll
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::CancelAll() 
    {
    }


// ---------------------------------------------------------
// CBrCtlApiTestObserver::ConstructL
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::ConstructL()
    {
    iHandler = CDocumentHandler::NewL(CEikonEnv::Static()->Process()) ;
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::~CBrCtlApiTestObserver
// ---------------------------------------------------------
//
CBrCtlApiTestObserver::~CBrCtlApiTestObserver()
    {
    delete iFileName;
    delete iHandler;
    }


// ---------------------------------------------------------
// CBrCtlApiTestObserver::NewL
// ---------------------------------------------------------
//
CBrCtlApiTestObserver* CBrCtlApiTestObserver::NewL()
{
  CBrCtlApiTestObserver* self = new(ELeave)CBrCtlApiTestObserver;
  CleanupStack::PushL(self);
  self->ConstructL();
  CleanupStack::Pop();
  return self;
}

// ---------------------------------------------------------
// CBrCtlApiTestObserver::IsFileScheme
// ---------------------------------------------------------
//

TBool CBrCtlApiTestObserver::IsFileScheme(const TDesC& aFileName)
    {
    _LIT(KFileScheme, "file://");
    if (aFileName.Length() > 0 && aFileName.Ptr() != NULL)
        {
        if (aFileName.FindF(KFileScheme) == 0)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::ReadFile
// ---------------------------------------------------------
//
HBufC8* CBrCtlApiTestObserver::ReadFileLC(const TDesC& aFileName)
    {
    RFs rfs;
    RFile file;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    User::LeaveIfError(file.Open(rfs, aFileName, EFileRead));
    CleanupClosePushL(file);
    TInt size;
    User::LeaveIfError(file.Size(size));
    HBufC8* buf = HBufC8::NewLC(size);
    TPtr8 bufPtr(buf->Des());
    User::LeaveIfError(file.Read(bufPtr));
    CleanupStack::Pop(); // buf
    CleanupStack::PopAndDestroy(2); // file, rfs
    CleanupStack::PushL(buf);
    return buf;
    }

// ---------------------------------------------------------
// CBrCtlApiTestObserver::RecognizeL
// ---------------------------------------------------------
//
HBufC* CBrCtlApiTestObserver::RecognizeLC(const TDesC& aFileName, const TDesC8& aData)
    {
    TDataRecognitionResult dataType;
    RApaLsSession apaSession;
    TInt ret;
    HBufC* contentTypeString = NULL;

    User::LeaveIfError(apaSession.Connect());
    // Ask the application architecture to find the file type
    ret = apaSession.RecognizeData(aFileName, aData, dataType);
    apaSession.Close();

    if (ret == KErrNone &&
        (dataType.iConfidence == CApaDataRecognizerType::ECertain) ||
        (dataType.iConfidence == CApaDataRecognizerType::EProbable))
        {
        // If the file type was found, try to match it to a known file type
        TPtrC8 mimeTypePtr = dataType.iDataType.Des8();
        TInt len = mimeTypePtr.Length() + 1;
        contentTypeString = HBufC::NewL(len); 
        contentTypeString->Des().Copy(mimeTypePtr);
        contentTypeString->Des().ZeroTerminate();
        }
    CleanupStack::PushL(contentTypeString);
    return contentTypeString;
    }

// -----------------------------------------------------------------------------
// CBrCtlApiTestObserver::GetFileNameL
// Translate the file name from a URL to a valid file name in the system.
// -----------------------------------------------------------------------------
//
TBool CBrCtlApiTestObserver::GetFileNameL(const TDesC& aFileName)
    {
    // This function accepts URLs in the following format:
    // file://filename.xxx
    // file:///filename.xxx
    // file://c:/filename.xxx
    // file:///c:/filename.xxx
    //
    _LIT(KFileScheme, "file://");
    _LIT(KDefaultDrivePath, "C:\\");
    _LIT(KPathChar, "\\");
    
    TInt count;
    TInt index = 0;
    TBool drvLetter = EFalse;
    TUint16 c;
    
    // Verify the file scheme
    TPtrC urlPtr(aFileName);
    if (urlPtr.FindF(KFileScheme) != 0)
        {
        return EFalse;
        }
    urlPtr.Set(urlPtr.Mid(KFileScheme().Length()));
    
    // make sure there are enough characters in the filename before
    // trying to check them
    count = urlPtr.Length();
    if(count == 0)
        {
        return EFalse;            // no filename, so can't look at urlPtr[0]
        }
    
    // Skip the first '/' if there is one
    if (urlPtr[0] == '/')
        {
        urlPtr.Set(urlPtr.Mid(1));
        }
    count = urlPtr.Length();
    
    // Is there a drive letter?
    if(count > 1)
        {
        // can check for drive letter
        if (urlPtr[1 + index] == ':')
            {
            drvLetter = ETrue;
            }
        }
    if(drvLetter == EFalse)
        {
        // 3 additional characters for the string "c:\"
        count = urlPtr.Length() + 3;
        }
    iFileName = HBufC::NewL(count);
    if (!drvLetter)
        {
        iFileName->Des().Append(KDefaultDrivePath);
        }
    
    TBool fragment(EFalse);
    // Convert relative path containing /./ and /../ to absolute path
    for (; index < urlPtr.Length() && !fragment; index ++)
        {
        switch(urlPtr[index])
            {
            case '#':    //Check if there is a fragment '#'
                {
                fragment = ETrue;
                continue; // Just stop there
                }
                
            case '/':
                {
                iFileName->Des().Append(KPathChar);
                break;
                }
            case '.':
                {
                if (index > 1 && urlPtr[index - 1] == '/')
                    {
                    if (index < count - 1 && urlPtr[index + 1] == '/')
                        {
                        index ++; // skip  ./
                        break;
                        }
                    if (index > 2 && index < count - 3 &&
                        urlPtr[index + 1] == '.' && urlPtr[index + 2] == '/')
                        {
                        TInt i = index - 2;
                        
                        for (; i > 0 && urlPtr[i] != '/'; i--) {} // skip  /../
                        
                        iFileName->Des().SetLength(iFileName->Des().Length() - (index - i));
                        index += 2;
                        break;
                        }
                    }
                }
                // no break
                //lint -fallthrough
                
            default:
                {
                c = urlPtr[index];
                iFileName->Des().Append(&c, 1);
                break;
                }
            }	// end of switch
        }
    return ETrue;
}

// ---------------------------------------------------------
// CBrCtlApiTestObserver::NewDownloadL
// ---------------------------------------------------------
//
TBool CBrCtlApiTestObserver::NewDownloadL(TUint /* aTransactionID */,
									   const TDesC& /* aFileName */,
									   const TDesC& /* aContentType */,
									   const TDesC& /* aUrl */)
{
	return ETrue;
}

void CBrCtlApiTestObserver::ResumeDownloadL(TUint /* aTransactionID */,
                                         TUint /* aLength */,
									   const TDesC& /* aFileName */,
									   const TDesC& /* aContentType */,
									   const TDesC& /* aUrl */)
{
}

void CBrCtlApiTestObserver::HandleDownloadEventL( TUint /* aTransactionID */, 
											  TBrCtlDownloadEvent /* aDownloadEvent */,
											  TUint /* aValue*/ )
{
}

// Dummy function definitions for the functions from CBrCtlWindowObserver class - Added for Widget Extension Test

// ---------------------------------------------------------
// CBrCtlApiTestObserver::OpenWindowL
// ---------------------------------------------------------
//
CBrCtlInterface* CBrCtlApiTestObserver::OpenWindowL(TDesC& aUrl, TDesC* aTargetName, TBool aUserInitiated, 
     TAny* aReserved)
{
	
}

// ---------------------------------------------------------
// CBrCtlApiTestObserver::FindWindowL
// ---------------------------------------------------------
//
CBrCtlInterface* CBrCtlApiTestObserver::FindWindowL( const TDesC& aTargetName ) const
{
	
}            

// ---------------------------------------------------------
// CBrCtlApiTestObserver::HandleWindowCommandL
// ---------------------------------------------------------
//
void CBrCtlApiTestObserver::HandleWindowCommandL( const TDesC& aTargetName, TBrCtlWindowCommand aCommand )
{
	
}



// End of File  
