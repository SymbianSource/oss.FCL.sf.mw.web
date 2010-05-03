/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
#include <uri16.h>
#include <apmrec.h>
#include <apgcli.h>
#include <escapeutils.h>

#include "fileloader_cfilehandler.h"
#include "fileloader_cfileloader.h"
//#include "urlloader_loaderutils.h"
#include "nwx_http_defs.h"
#include "fileloader_csaveddeckhandler.h"
#include "TEncodingMapping.h"

//#include "PluginHandler.h"

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
// CFileHandler::CFileHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFileHandler::CFileHandler(const TUint16 *aUrl,
                           TUint8 aMethod,
                           TUint16 aTransId,
                           TUint8 aLoadType,
                           void *aLoadContext,
                           NW_Url_RespCallback_t *aLoadCallback,
                           CKFileLoader* aFileLoader) :
        CActive(CActive::EPriorityUserInput), iData(NULL, 0), iFileName(NULL, 0),
        iSize(0)
{
    iUrl = aUrl;
    iRedirectedUrl = NULL;
    iTransId = aTransId;
    iLoadContext = aLoadContext;
    iLoadCallback = aLoadCallback;
    iFileLoader = aFileLoader;
    iRfsOpen = EFalse;
    iFileOpen = EFalse;
    iMethod = aMethod;
    iLoadType = aLoadType;
}


// -----------------------------------------------------------------------------
// CFileHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFileHandler::ConstructL()
{
    TInt ret = KErrNone;

    if (!GetFileNameL())
    {
        User::Leave(KErrBadName);
    }

    // connect to file server
    User::LeaveIfError(iRfs.Connect(1));
    iRfsOpen = ETrue;
    // Open the file
    ret = iFile.Open(iRfs, iFileName, EFileRead | EFileShareAny);
    if (ret == KErrNotFound)
    {
        // Try again with the Uri-decoded name.
		HBufC16* decodedFileName = EscapeUtils::EscapeDecodeL(iFileName);
		TPtrC decodedFileNamePtr(*decodedFileName);

		ret = iFile.Open(iRfs, decodedFileNamePtr, EFileRead | EFileShareAny);
        if (ret == KErrNotFound)
		{
            // Convert the error KErrNotFound to a file specific error
            ret = KErrPathNotFound;
		}
		else 
		{
			// Use the decoded filename
			iFileName.Copy(decodedFileNamePtr);
			iFileName.ZeroTerminate();
		}
		delete decodedFileName;
    }

    User::LeaveIfError(ret);
    iFileOpen = ETrue;

    // Get file size
    User::LeaveIfError(iFile.Size(iSize));

    CActiveScheduler::Add(this);
}


// -----------------------------------------------------------------------------
// CFileHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFileHandler* CFileHandler::NewL(const TUint16 *aUrl,
                                 TUint8	aMethod,
                                 TUint16 aTransId,
                                 TUint8 aLoadType,
                                 void *aLoadContext,
                                 NW_Url_RespCallback_t *aLoadCallback,
                                 CKFileLoader* aFileLoader)
{
    CFileHandler* self = new( ELeave ) CFileHandler(aUrl, aMethod, aTransId,
                                                    aLoadType,
                                                    aLoadContext, aLoadCallback,
                                                    aFileLoader);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}


// -----------------------------------------------------------------------------
// CFileHandler::~CFileHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CFileHandler::~CFileHandler()
{
    if ((iFileName.Length() > 0) && (iFileName.Ptr() != NULL))
    {
        delete (TUint16*) iFileName.Ptr();
    }
    if (iFileOpen)
    {
        iFile.Close();
    }
    if (iRfsOpen)
    {
        iRfs.Close();
    }
    if(iContentType)
		{
		delete iContentType;
		}
    delete (TUint8*)iData.Ptr();
    delete iRedirectedUrl;
}


// -----------------------------------------------------------------------------
// CFileHandler::RunL
// Return the response after file loading from the File system is complete.
// -----------------------------------------------------------------------------
//
void CFileHandler::RunL()
{
    switch (iStatus.Int())
    {
        case KErrNone:
        {            
			const TText8* wmltStr = (TText8*)"0";
			TPtrC8 wmltPtr(wmltStr);
			const TText8* wmltcStr = (TText8*)"1";
			TPtrC8 wmltcPtr(wmltcStr);
			const TText8* wmlaStr = (TText8*)"2";
			TPtrC8 wmlaPtr(wmlaStr);
			const TText8* wmlacStr = (TText8*)"3";
			TPtrC8 wmlacPtr(wmlacStr);

			_LIT(KContentTypeT,"text/vnd.wap.wml");
			_LIT(KContentTypeTC,"text/vnd.wap.wmlc");
			_LIT(KContentWMLSType,"application/vnd.wap.wmlscriptc");
			_LIT(KContentWMLACType,"application/vnd.wap.wmlc");

			TInt len = KContentWMLSType().Length();
			TUint8* contentTypeString = new(ELeave) TUint8 [len + 1];
			TPtrC8 script = iData.Right(1);
			if( script.operator==(wmltPtr) )
				{
				TPtr8 contentTypeStringPtr(contentTypeString, KContentTypeT().Length() + 1); 
				contentTypeStringPtr.Copy(KContentTypeT);
				contentTypeStringPtr.ZeroTerminate();
				}
			else if(script.operator==(wmltcPtr))
				{
				TPtr8 contentTypeStringPtr(contentTypeString, KContentTypeTC().Length() + 1); 
				contentTypeStringPtr.Copy(KContentTypeTC);
				contentTypeStringPtr.ZeroTerminate();
				}
			else if(script.operator==(wmlaPtr))
				{
				TPtr8 contentTypeStringPtr(contentTypeString, KContentWMLSType().Length() + 1); 
				contentTypeStringPtr.Copy(KContentWMLSType);
				contentTypeStringPtr.ZeroTerminate();
				}
			else if(script.operator==(wmlacPtr))
				{
				TPtr8 contentTypeStringPtr(contentTypeString, KContentWMLACType().Length() + 1); 
				contentTypeStringPtr.Copy(KContentWMLACType);
				contentTypeStringPtr.ZeroTerminate();
				}

			// Get the file's content encoding.
			//If there is charset in the file, use it. Otherwise, get it basing on the content type string
			TUint16 charset = HTTP_iso_8859_1; //default to latin-1
			TBool bHasCharset = ETrue;
			TInt position = iData.LocateReverse('*');
			TInt posDollar = iData.LocateReverse('$');

			if((posDollar == KErrNotFound) || (posDollar < position)) //no charset was set
			{
				bHasCharset = EFalse;
				charset = ContentEncoding(contentTypeString);
			}
			else
			{
				TPtrC8 charsetfromfile = iData.Mid( posDollar + 1, (iData.Length() - (posDollar + 2)));
				for (int i = 0; supportedCharset[i].uid != 0; i++)
				{
					TPtrC8 charsetTemp(supportedCharset[i].charset);
					if (charsetTemp == charsetfromfile)
					{
					  charset = supportedCharset[i].ianaEncoding;
					  break;
					}
				}
			}
               
            // Check if it is a saved deck.
			// Put the "application saved page" string const and the
			// "ContentType" string in TPtrC8 to find if this is a saved page
			TPtrC8 httpSavedString(HTTP_application_saved_string);
			TPtrC8 contentString(contentTypeString);

            TUint index = contentString.FindF(httpSavedString);
			
			if ( !((0 == index) && ParseSavedDeck()) )
            {
				// Not a saved deck, send a response
                //TPtr8 data( iData ); declared up
                //
                TUint contentLength = 0;

                const TUint16* responseUrl = (iRedirectedUrl == NULL) ? iUrl : iRedirectedUrl;
            		

                if (iMethod == NW_URL_METHOD_GET)
					{
					TInt urlLen = 0;
					if(bHasCharset)
					{
						urlLen = posDollar - position;
					}
					else
					{
						urlLen = iData.Length() - position -1;
					}

					//R->fake the url
					HBufC* newUrl = HBufC::NewL( urlLen );
					newUrl->Des().Copy(iData.Mid( position + 1, urlLen-1 ));
					newUrl->Des().ZeroTerminate();				

					// Transfer ownership before calling Response
					TUint8* data = new TUint8[position];
					TPtr8 dataDes(data, position);
					dataDes.Copy(iData.Ptr(), position);
					iData.Set(NULL,0, 0);
					delete (TUint8*)iData.Ptr();		
					
					responseUrl = (TUint16*)newUrl->Ptr();

					// Send the response
					iFileLoader->PartialResponse( (TUint16*)newUrl->Ptr(), dataDes, NULL, contentTypeString, NULL, NW_FALSE, // multipart information is missing.
						NULL, NULL, charset, NULL, Success, NW_URL_METHOD_GET, iTransId, 0 /*first chunk*/, NULL, dataDes.Length(),
						KErrNone, iLoadContext, iLoadCallback);
					delete newUrl;
					}
                
                // Send the close response
                iContentType = NULL;
				TPtr8 emptyData(NULL,0,0);     
                contentTypeString = iContentType;
                
				iFileLoader->PartialResponse( responseUrl, emptyData, NULL, contentTypeString, NULL, NW_FALSE, // multipart information is missing
                  NULL, NULL, charset, NULL, Success, iMethod, iTransId, -1 /*last chunk*/, NULL, contentLength,
                  KErrNone, iLoadContext, iLoadCallback);
                }
            else
				{
                    // No response sent with this content type string,
                    // (so no subsequent delete) delete it here.
                    delete contentTypeString;
                }
			}
			
            break;

        default:
        {
            // Send an error response back
            iFileLoader->PartialResponse(iTransId, iStatus.Int(), iLoadContext, iLoadCallback);
        }
        break;
    }	// end of switch

    delete this;
}


// -----------------------------------------------------------------------------
// CFileHandler::DoCancel
// Noop. File loading cannot be cancelled.
// -----------------------------------------------------------------------------
//
void CFileHandler::DoCancel()
{
}


// -----------------------------------------------------------------------------
// CFileHandler::LoadFileL
// Create a CFileHandler object and initiate the load.
// -----------------------------------------------------------------------------
//
void CFileHandler::LoadFileL(const TUint16 *aUrl,
                             TUint16 aTransId,
                             TUint8 aLoadType,
                             void *aLoadContext,
                             NW_Url_RespCallback_t *aLoadCallback,
                             CKFileLoader* aFileLoader)
{
    __ASSERT_DEBUG(aUrl != NULL, LoaderUtils::PanicLoader());
    __ASSERT_DEBUG(aLoadCallback != NULL, LoaderUtils::PanicLoader());

    // Create a file handler object
    CFileHandler* handler = CFileHandler::NewL(aUrl, NW_URL_METHOD_GET,
            aTransId, aLoadType, aLoadContext, aLoadCallback, aFileLoader);

    // Start reading the file
    if (handler->iSize > 0)
    {
        TUint8* data = NULL;
        data = new TUint8 [handler->iSize];
        User::LeaveIfNull(data);
        handler->iData.Set(data, 0, handler->iSize);
        handler->iFile.Read(handler->iData, handler->iStatus);
    }

    handler->SetActive();
}


// -----------------------------------------------------------------------------
// CFileHandler::LoadFileL
// Create a CFileHandler object and initiate the load.
// -----------------------------------------------------------------------------
//
void CFileHandler::GetFileInfoL(const TUint16 *aUrl,
                                TUint16 aTransId,
                                TUint8 aLoadType,
                                void *aLoadContext,
                                NW_Url_RespCallback_t *aLoadCallback,
                                CKFileLoader* aFileLoader)
{
    __ASSERT_DEBUG(aUrl != NULL, LoaderUtils::PanicLoader());
    __ASSERT_DEBUG(aLoadCallback != NULL, LoaderUtils::PanicLoader());

    // Create a file handler object
    CFileHandler* handler = CFileHandler::NewL(aUrl, NW_URL_METHOD_HEAD, 
            aTransId, aLoadType, aLoadContext, aLoadCallback, aFileLoader);

    // Start reading the file
    if (handler->iSize > 0)
    {
        TUint8* data = NULL;
        data = new TUint8 [handler->iSize];
        User::LeaveIfNull(data);
        handler->iData.Set(data, 0, handler->iSize);
        handler->iFile.Read(handler->iData, handler->iStatus);
    }

    handler->SetActive();
}


// -----------------------------------------------------------------------------
// CFileHandler::ContentTypeL
// Determine the content type of the file.
// -----------------------------------------------------------------------------
void CFileHandler::ContentTypeL(TUint8** aContentTypeString)
    {
    // part of string defined in "Exported mimetypes" section in BrowserRec.h
    __ASSERT_DEBUG(aContentTypeString != NULL, LoaderUtils::PanicLoader());
    
    TDataRecognitionResult dataType;
    RApaLsSession apaSession;
    TInt ret;
    
    *aContentTypeString = NULL;
    User::LeaveIfError(apaSession.Connect());
    // Ask the application architecture to find the file type
    ret = apaSession.RecognizeData(iFileName, iData, dataType);
    apaSession.Close();
    
    if (ret == KErrNone &&
        (dataType.iConfidence == CApaDataRecognizerType::ECertain) ||
        (dataType.iConfidence == CApaDataRecognizerType::EProbable))
        {
        // If the file type was found, try to match it to a known file type
        TPtrC8 mimeTypePtr = dataType.iDataType.Des8();
        TInt len = mimeTypePtr.Length() + 1;
        *aContentTypeString = new(ELeave) TUint8 [len]; 
        
        TPtr8 contentTypeStringPtr(*aContentTypeString, len);
        contentTypeStringPtr.Copy(mimeTypePtr);
        contentTypeStringPtr.ZeroTerminate();
        return;
        }
    
    // Check if it is css
    _LIT(KCssExt, ".css");
    TPtrC extPtr (iFileName.Right(iFileName.Length() -  iFileName.LocateReverse('.')));
    if (!extPtr.CompareF(KCssExt()))
        {
        TPtrC8 mimeTypePtr(HTTP_text_css_string);
        TInt len = mimeTypePtr.Length() + 1;
        *aContentTypeString = new(ELeave) TUint8 [len]; 
        
        TPtr8 contentTypeStringPtr(*aContentTypeString, len);
        contentTypeStringPtr.Copy(mimeTypePtr);
        contentTypeStringPtr.ZeroTerminate();
        return;
        }
    
    // Check if it is wbmp
    _LIT(KWbmpExt, ".wbmp");
    extPtr.Set(iFileName.Right(iFileName.Length() -  iFileName.LocateReverse('.')));
    if (!extPtr.CompareF(KWbmpExt()))
        {
        TPtrC8 mimeTypePtr(HTTP_image_vnd_wap_wbmp_string);
        TInt len = mimeTypePtr.Length() + 1;
        *aContentTypeString = new(ELeave) TUint8 [len]; 
        
        TPtr8 contentTypeStringPtr(*aContentTypeString, len);
        contentTypeStringPtr.Copy(mimeTypePtr);
        contentTypeStringPtr.ZeroTerminate();
        return;
        }
    //R->ecma
    // Check if it is ecma
    /*
    _LIT(KEcmaScriptExt, ".es");
    _LIT(KJavaScriptExt, ".js");
    extPtr.Set(iFileName.Right(iFileName.Length() -  iFileName.LocateReverse('.')));
    if (!extPtr.CompareF(KEcmaScriptExt()) || !extPtr.CompareF(KJavaScriptExt()))
        {
        TPtrC8 mimeTypePtr(HTTP_text_ecmascript_string);
        TInt len = mimeTypePtr.Length() + 1;
        *aContentTypeString = new(ELeave) TUint8 [len]; 
        
        TPtr8 contentTypeStringPtr(*aContentTypeString, len);
        contentTypeStringPtr.Copy(mimeTypePtr);
        contentTypeStringPtr.ZeroTerminate();
        return;
        }
    */
    // Check if it is a supported plugin
    //R->embed
    //CPluginHandler* pluginHandler = CPluginHandler::GetSingleton();
    //NW_ASSERT (pluginHandler != NULL); // we should always have a plugin handler
    /* //R
    TUint16* mimeType16 = pluginHandler->GetPluginMimeTypeL(iFileName);
    if (mimeType16 != NULL)
        {
        // This file's extension is supported
        CleanupStack::PushL(mimeType16);
        TPtrC16 mimeTypePtr16(mimeType16);
        TUint8* mimeType8 = new(ELeave) TUint8 [mimeTypePtr16.Length() + 1];
        TPtr8 mimeTypePtr8(mimeType8, 0, mimeTypePtr16.Length() + 1);
        mimeTypePtr8.Copy(mimeTypePtr16);
        mimeTypePtr8.ZeroTerminate();
        CleanupStack::PopAndDestroy(); // mimeType16
        *aContentTypeString = mimeType8;
        return;
        }
	*/
    User::Leave(KBrsrWmlbrowserBadContentType);
    }


// -----------------------------------------------------------------------------
// CFileHandler::ContentEncoding
// Determine the content encoding of the file.
// -----------------------------------------------------------------------------
//
TUint16 CFileHandler::ContentEncoding(TUint8* aContentTypeString)
{
    // Assume Latin-1 for xhtml and wml. ucs2 for any other
    TUint16 charset = HTTP_iso_8859_1;

	TPtrC8 httpAppString(HTTP_application_vnd_wap_wmlc_string);
	TPtrC8 contentString(aContentTypeString);

	// Is the contentType a HTTP_application_vnd_wap_wmlc_string
	TUint index = contentString.FindF(httpAppString);
	if (index == 0)
    {
		// This is a HTTP_application_vnd_wap_wmlc_string
		charset = HTTP_iso_10646_ucs_2;
    }

    return charset;
}


// -----------------------------------------------------------------------------
// CFileHandler::GetFileNameL
// Translate the file name from a URL to a valid file name in the system.
// -----------------------------------------------------------------------------
//
TBool CFileHandler::GetFileNameL()
{
    // This function accepts URLs in the following format:
    // file://filename.xxx
    // file:///filename.xxx
    // file://c:/filename.xxx
    // file:///c:/filename.xxx
    //
    _LIT(KFileScheme, "file://");
    _LIT(KDefaultDrivePath, "C:\\");
    _LIT(KDefaultDriveInUrl, "C:/");
    _LIT(KPathChar, "\\");
    
	TUint16* name = NULL;
    TInt count;
    TInt index = 0;
    TBool drvLetter = EFalse;
    TUint16 c;

    // Verify the file scheme
    TPtrC urlPtr(iUrl);
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
    name = new(ELeave) TUint16 [count];
    CleanupStack::PushL(name);
    iFileName.Set(name, 0, count);
    if (!drvLetter)
    {
        // If there is no drive letter, add "C:\"
        iRedirectedUrl = new(ELeave) TUint16[count + 1 + KFileScheme().Length()];
        TPtr redirectedUrlPtr(iRedirectedUrl, count + 1 + KFileScheme().Length());
        redirectedUrlPtr.Copy(KFileScheme());
        redirectedUrlPtr.Append(KDefaultDriveInUrl);
        redirectedUrlPtr.Append(urlPtr);
        redirectedUrlPtr.ZeroTerminate();
        iFileName.Append(KDefaultDrivePath);
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
                iFileName.Append(KPathChar);
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
                        
                        iFileName.SetLength(iFileName.Length() - (index - i));
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
                iFileName.Append(&c, 1);
                break;
            }
        }	// end of switch
    }
    CleanupStack::Pop(); // name
    return ETrue;
}


// -----------------------------------------------------------------------------
// CFileHandler::ParseSavedDeck
// Determine if the file is a saved deck or not. If it is saved deck, parse the file.
// -----------------------------------------------------------------------------
//
TBool CFileHandler::ParseSavedDeck()
{
    TPtr8 responseData(NULL,0,0);
    TPtr responseUrl(NULL, 0, 0);
    TPtr8 contentTypeString(NULL, 0, 0);
    TUint charset;
    TBool isSavedDeck;
    TPtrC8 dataPtr(iData.Ptr(), iData.Length());
    TPtrC16 urlPtr(iUrl, User::StringLength(iUrl));

    // Create a CSavedDeckHandler object to parse the file
    TInt ret = CSavedDeckHandler::ParseSavedDeck(dataPtr, urlPtr, responseData,
               responseUrl, contentTypeString, charset, isSavedDeck);
  
    if (isSavedDeck)
      {
      // It is a saved deck
      if (ret == KErrNone)
        {
        TUint8* data = (TUint8*)iData.Ptr();
        delete data;
        // Transfer ownership before calling Response
        iData.Set(NULL, 0, 0);
        // copy contentTypeString for the second (closing) response
        TUint8* tempContentTypeString = NULL;
        tempContentTypeString = (TUint8*)NW_Mem_Malloc( contentTypeString.Length() + 1 );
        if( tempContentTypeString )
          {
           NW_Mem_memcpy( tempContentTypeString, contentTypeString.Ptr(), contentTypeString.Length() );
           tempContentTypeString[ contentTypeString.Length() ] = 0;
           // Send the first part of the file as the response
           iFileLoader->PartialResponse((TUint16*)responseUrl.Ptr(), responseData, NULL,
             (TUint8*)contentTypeString.Ptr(),  NULL, NW_FALSE, // multipart information is missing.
             NULL, NULL, (TUint16)charset, NULL, Success, 0, iTransId, 0 /*first chunk*/, NULL, responseData.Length(),
             KErrNone, iLoadContext, iLoadCallback);
           
           // Send closing response. pass tempContentTypeString ownership
           TPtr8 emptyResponseData(NULL,0,0);        
           iFileLoader->PartialResponse((TUint16*)responseUrl.Ptr(), emptyResponseData, NULL,
             tempContentTypeString, NULL, NW_FALSE, // multipart information is missing.
             NULL, NULL, (TUint16)charset, NULL, Success, 0, iTransId, -1/*last chunk*/, NULL, 0,
             KErrNone, iLoadContext, iLoadCallback);
          }
        delete (TUint16*)responseUrl.Ptr();
        }
      else
        {
        // Send an error response
        iFileLoader->PartialResponse(iTransId, ret, iLoadContext, iLoadCallback);
        }
      }
    return isSavedDeck;
  }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
