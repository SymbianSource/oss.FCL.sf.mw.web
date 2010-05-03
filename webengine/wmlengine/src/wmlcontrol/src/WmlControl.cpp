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
* Description:  
*
*/


//  INCLUDES
#include "WmlControl.h"
#include "multipartparser.h"
#include "MVCView.h"
#include "MVCShell.h"
#include "WmlInterface.h"
#include "GDIDevicecontext.h"
#include "HistoryController.h"
#include "HistoryInterface.h"

#include "WmlDispatcher.h"
#include "StringUtils.h"
#include "nwx_string.h"
#include "nwx_mem.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// -----------------------------------------------------------------------------
// CWmlControl::ConstructL
// Private Class Method
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CWmlControl::ConstructL( CBrCtl* aBrCtl, TRect& aRect, MWmlInterface* aWKWmlInterface)
{
    // Initialize messaging system
    if (NW_Msg_Initialize( "PSQ1" ) != KBrsrSuccess){
	       User::Leave(KErrNoMemory);
	   }
    iWmlContentInterface = NULL;
    iWKWmlInterface = aWKWmlInterface;
    iWmlView = CView::NewL(aBrCtl, aBrCtl, aRect, this);		
    iWmlShell = CShell::NewL(iWmlView, aBrCtl, aWKWmlInterface, this );
    iBrCtl = aBrCtl ;
}
    
// -----------------------------------------------------------------------------
// CWmlControl::PageInfoLC
// Returns to the Browser Control info about the page. The info is returned on the Cleanup Stack.
// -----------------------------------------------------------------------------
//
HBufC* CWmlControl::PageInfoLC(TBrCtlDefs::TBrCtlPageInfo aBrCtlPageInfo)
    {
    HBufC* pageInfo = NULL;
    TBool isUrl = EFalse;
    
    if ( iHistoryController && iHistoryController->historyView() )
    {
       return iHistoryController->pageInfoLC( aBrCtlPageInfo );
    }    
    else
    
    switch (aBrCtlPageInfo)
        {
        case TBrCtlDefs::EPageInfoTitle:
            pageInfo = iWmlShell->GetCurrentTitleInHBufCL(isUrl);
            if (isUrl) // should modify the shell to not lookup the URL if no title available
                {
                delete pageInfo;
                pageInfo = NULL;
                }
            break;
        case TBrCtlDefs::EPageInfoUrl:
            pageInfo = iWmlShell->GetCurrentUrlInHBufCL();
                 if ( !pageInfo->Compare(_L(" ")) )
                {
                delete pageInfo;
                pageInfo = NULL;
                }
            break;
        case TBrCtlDefs::EPageInfoContent:
             pageInfo = iWmlShell->GetPageAsTextL();
             break;
	case TBrCtlDefs::EPageInfoSavedPage:
             pageInfo = WMLGenerateSavedPageL();
             break;
        }

    CleanupStack::PushL(pageInfo);

    return pageInfo;
    }

/*
   * StartLoadingResource
   * Public Class Method
   * Purpose
   * @since 3.1
   * @param const TDesC& aURL
   * @return CWmlResourceLoadListener*
*/
CWmlResourceLoadListener* CWmlControl::LoadResourceL( const TDesC& aUrl, void* aLoadContext,
                        void* aPartialLoadCallback, TUint16 aTransId)                              
{
    if( iWmlContentInterface && (aTransId - iWmlContentInterface->getTransId()) > 1 ){
        m_contentArray.ResetAndDestroy();
        iWmlContentInterface = NULL; 
    }
    CWmlContentInterface* contentInterface = CWmlContentInterface::NewL();
    contentInterface->setUrl( aUrl );
    contentInterface->setTransId( aTransId );
    contentInterface->setLoadContext( aLoadContext );
    contentInterface->setPartialCallBack( aPartialLoadCallback );
        
    m_contentArray.AppendL( contentInterface );
                   
    HBufC8* url8 = HBufC8::New(aUrl.Length() + 1);
    if(url8) {
        TPtr8 url8Ptr(url8->Des());
        url8Ptr.Copy(aUrl);
        url8Ptr.ZeroTerminate();
        iWKWmlInterface->loadResource( url8Ptr, iWmlContentInterface);
        delete url8;
    }
       
    return NULL;
}
	
// -----------------------------------------------------------------------------
// 
// C++ default constructor.
// 
// -----------------------------------------------------------------------------
CWmlControl::CWmlControl()
{
}

// -----------------------------------------------------------------------------
// 
// C++ destructor.
// 
// -----------------------------------------------------------------------------
CWmlControl::~CWmlControl()
{
	   iWmlContentInterface = NULL;
	   delete iWmlShell;
	   delete iWmlView;
    m_contentArray.ResetAndDestroy();
    m_contentArray.Close();
}

// -----------------------------------------------------------------------------
// CBrCtl::SetBrowserSettingL
// From CBrCtlInterface
// Update a Browser Control setting
// -----------------------------------------------------------------------------
//
void CWmlControl::SetBrowserSettingL(TUint aSetting, TUint aValue)
    {
    iWmlView->SetBrowserSettingL( aSetting, aValue);
    }
	
// -----------------------------------------------------------------------------
// CWebKitControl::GenerateSavedPageL
// Public Class Method
// Create a formatted descriptor containing all markup and 
// resources for the active page
// -----------------------------------------------------------------------------
//
HBufC* CWmlControl::WMLGenerateSavedPageL()
    {

    RPointerArray<CBodyPart> partsArr;

    //Creat the data here

    CBodyPart* newPart = NULL;
    HBufC8* hdrData( NULL );
    TInt popDestCount( 0 );

	//The following must be same as that of KContentTypeTextWml8
	//in MultipartContentHandler.cpp otherwise Multipart parser 
	//will not process this corretcly.

	_LIT8( KWmlContentType, "text/vnd.wap.wml" );

	HBufC* wmlCurrentUrl = iWmlShell->GetCurrentUrlInHBufCL();
    User::LeaveIfNull( wmlCurrentUrl );
    CleanupStack::PushL( wmlCurrentUrl );

	HBufC8* pageWMLMarkup =  iWmlShell->GetPageAsWMLMarkUp();
	User::LeaveIfNull( pageWMLMarkup );
    CleanupStack::PushL( pageWMLMarkup );
	
	TPtrC8 charsetPtr(0,0);
	const TUint8* charset = iWmlShell->GetCharsetFromResponse();	
	
	if (charset != NULL)
		{		
		charsetPtr.Set(charset, 
					NW_Asc_strlen(reinterpret_cast<const char*>(charset)));
		}
	
	
	hdrData = HBufC8::NewLC(  KPartHeadersData().Length() + 
                                  wmlCurrentUrl->Length() + 1 +
                                  charsetPtr.Length() +
                                  KWmlContentType().Length() );

    ++popDestCount; //For 1st hdrData


    HBufC8* tempWMLUrl = HBufC8::NewLC(wmlCurrentUrl->Des().Length() + 1);        
	tempWMLUrl->Des().Copy(*wmlCurrentUrl);
	tempWMLUrl->Des().ZeroTerminate();
		
	HBufC8* contentType = 	HBufC8::NewLC(KWmlContentType().Length());
	contentType->Des().Copy(KWmlContentType);

	hdrData->Des().Format( KPartHeadersData, tempWMLUrl, contentType, &charsetPtr);
		
    //Do some clean up for first part
	CleanupStack::PopAndDestroy( contentType);
	CleanupStack::PopAndDestroy( tempWMLUrl);	
	
	
    // prepare new part and append to array
    newPart = CBodyPart::NewL();
    newPart->SetHeaders( *hdrData );
    newPart->SetBody(  pageWMLMarkup->Des() );
    partsArr.Append( newPart );
       
     
    // create CBodyPart objects from TWebCoreImageCarrier objects
	CArrayFixFlat<TBrCtlImageCarrier>* imgArr =
                             iWmlView->GetPageImagesL();
    User::LeaveIfNull( imgArr );
    CleanupStack::PushL( imgArr );
	++popDestCount; //imgArr

    TInt numImages = imgArr->Count();

    for( TInt k=0; k<numImages; k++ )
        {
        // prepare headers for part
        TInt imageUrlLen = imgArr->At(k).Url().Length()+1; //One for NULL Termination      
        TInt imageContentTypeLen = imgArr->At(k).ContentType().Length()+1; //One for NULL Termination

        hdrData = HBufC8::NewLC( KPartHeadersImg().Length() + imageUrlLen + imageContentTypeLen );
        popDestCount++; //For 2nd hdrData
        // convert url and content type to 8 bit descriptors        
        HBufC8* url8 = HBufC8::NewLC( imageUrlLen );
        url8->Des().Copy( imgArr->At(k).Url() );
		url8->Des().ZeroTerminate();    

		HBufC8* imgcontentType = 	HBufC8::NewLC(imageContentTypeLen);
		imgcontentType->Des().Copy(imgArr->At(k).ContentType());
		imgcontentType->Des().ZeroTerminate();

		// combine header values with format
        hdrData->Des().Format( KPartHeadersImg, url8, imgcontentType );
		
        CleanupStack::PopAndDestroy( imgcontentType );
		CleanupStack::PopAndDestroy( url8 );

		// prepare new part and append to array
        newPart = CBodyPart::NewL();
        newPart->SetHeaders( *hdrData );
        newPart->SetBody( imgArr->At(k).RawData() );
        partsArr.Append( newPart );	   
	    
        }

    // compose multipart/mixed document using MultipartParser        
    HBufC8* docBody8 = MultipartParser::ComposeL( partsArr, KSavePageBoundary(),
                       MultipartParser::EMultipartSubtypeMixed, 
                       MultipartParser::EMultipartTopLevelHeaderContentType |
                       MultipartParser::EMultipartTopLevelHeaderContentLength |
                       MultipartParser::EMultipartTopLevelHeaderLastModified );
    // cleanup descriptors
    partsArr.ResetAndDestroy();
    CleanupStack::PopAndDestroy( popDestCount );
    CleanupStack::PopAndDestroy(pageWMLMarkup);

    User::LeaveIfNull( docBody8 );
    CleanupStack::PushL( docBody8 );
    // convert multipart document to 16 bit descriptor
    HBufC* docBody16 = HBufC::NewL( docBody8->Length() );
    docBody16->Des().Copy( *docBody8 );
    CleanupStack::PopAndDestroy( docBody8 );
	   CleanupStack::PushL(docBody16);
    TInt contentLength = docBody16->Length();
    TInt docLen = KSavePageDocTop().Length() + wmlCurrentUrl->Des().Length() +
                  KCrLf().Length() + contentLength;
    HBufC* savedpage = HBufC::NewL( docLen );
    savedpage->Des().Copy( wmlCurrentUrl->Des() );
    savedpage->Des().Append( KCrLf );
    savedpage->Des().Insert( 0, KSavePageDocTop );
    savedpage->Des().Append( *docBody16 );
 
    CleanupStack::PopAndDestroy( docBody16 );
    CleanupStack::PopAndDestroy( wmlCurrentUrl);
    return savedpage;
    }

// -----------------------------------------------------------------------------
// CWmlControl::InputElementEditComplete( TBool aInputConfirmed )
// 
// -----------------------------------------------------------------------------
//
void CWmlControl::InputElementEditComplete( TBool aInputConfirmed )
{
 iWmlView->InputElementEditComplete( aInputConfirmed );
}//end CBrCtl::InputElementEditComplete()


// -----------------------------------------------------------------------------
// CWmlControl::coeControl()
// 
// -----------------------------------------------------------------------------
//
CCoeControl* CWmlControl::coeControl()
{
 return (CCoeControl*) iWmlView;

}

// -----------------------------------------------------------------------------
// CWmlControl::GainFocus()
// 
// -----------------------------------------------------------------------------
void CWmlControl::GainFocus()
{
 iWmlShell->GainFocus();
}

// -----------------------------------------------------------------------------
// CWmlControl::GainFocus()
// 
// -----------------------------------------------------------------------------
CArrayFixFlat<TBrCtlImageCarrier>* CWmlControl::GetPageImagesL()
{
 return ( iWmlView->GetPageImagesL() );
}

// -----------------------------------------------------------------------------
// CWmlControl::GetFocusedElementType()
// 
// -----------------------------------------------------------------------------
TBrCtlDefs::TBrCtlElementType CWmlControl::GetFocusedElementType()
{
 return ( iWmlView->GetFocusedElementType() );
}

// -----------------------------------------------------------------------------
// CWmlControl::LoseFocus()
// 
// -----------------------------------------------------------------------------
void CWmlControl::LoseFocus()
{
 iWmlShell->LoseFocus();
}

// -----------------------------------------------------------------------------
// CWmlControl::ProcessBack()
// 
// -----------------------------------------------------------------------------
TInt CWmlControl::ProcessBack()
{
 return ( iWmlShell->ProcessBack() );
}

// -----------------------------------------------------------------------------
// CWmlControl::HandleDo()
// 
// -----------------------------------------------------------------------------
TInt CWmlControl::HandleDo( TUint32 aElemId )
{
 // try to commit editing before HandleDo
 InputElementEditComplete(ETrue);
 return ( iWmlShell->HandleDo( aElemId ) );
}

// -----------------------------------------------------------------------------
// CWmlControl::HistoryStackPosition()
// 
// -----------------------------------------------------------------------------
TWmlEngineDefs::THistoryStackPosition CWmlControl::HistoryStackPosition()
{
  return ( iWmlShell->HistoryStackPosition() );
}

// -----------------------------------------------------------------------------
// CWmlControl::MakeVisible()
// 
// -----------------------------------------------------------------------------
void CWmlControl::MakeVisible( TBool aVisible )
{
  if (!aVisible)
    {
    // cancel editing if we go elsewhere
    InputElementEditComplete(EFalse);
    }
  return ( iWmlView->MakeVisible(aVisible) );
}

// -----------------------------------------------------------------------------
// CWmlControl::SetFocus()
// 
// -----------------------------------------------------------------------------
void CWmlControl::SetFocus(TBool aFocus, TDrawNow aDrawNow )
{
  return ( iWmlView->SetFocus( aFocus, aDrawNow ) );
}


// -----------------------------------------------------------------------------
// CWmlControl::SetRect()
// 
// -----------------------------------------------------------------------------
void  CWmlControl::SetRect( const TRect &aRect )
{
 return ( iWmlView->SetRect( aRect ) );
}

// -----------------------------------------------------------------------------
// CWmlControl::LoadImages()
// 
// -----------------------------------------------------------------------------
void  CWmlControl::LoadImages()
{
 iWmlShell->ShowImagesL();
}

// -----------------------------------------------------------------------------
// CWmlControl::WmlViewRectangle()
// 
// -----------------------------------------------------------------------------
TRect CWmlControl::WmlViewRectangle() 
{
	return TRect( iWmlView->Position(), iWmlView->Size() );
}

// -----------------------------------------------------------------------------
// CWmlControl::ScrollTo()
// 
// -----------------------------------------------------------------------------
void CWmlControl::ScrollTo(TPoint aPoint)
    {
    iWmlView->ScrollTo(aPoint);
    }

// -----------------------------------------------------------------------------
// CWmlControl::WMLPartialResponse()
// 
// -----------------------------------------------------------------------------
TInt CWmlControl::WMLPartialResponse( CWmlContentInterface* aWmlContentInterface,
											                           const TUint16* aUri,
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
    TBrowserStatusCode retVal = aWmlContentInterface->WMLPartialResponse(aUri,
    									                         aBody,
    									                         aHeaders,
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
			                                   aChunkIndex,
			                                   aCertInfo,
			                                   aContentLength,
			                                   aErr,
			                                   aLoadContext,
			                                   aPartialCallback);
    return (TInt) retVal;
}

// -----------------------------------------------------------------------------
// CWmlControl::WmlLoadUrlL(..)
// 
// -----------------------------------------------------------------------------
void CWmlControl::WmlLoadUrlL( const TDesC& aUrl )
{
    return (iWmlShell->WmlShellLoadUrlL( aUrl, AppId() ) );
}

// -----------------------------------------------------------------------------
// CWmlControl::SetCertInfo( const TCertInfo*  aCertInfo )
// 
// -----------------------------------------------------------------------------
void CWmlControl::SetCertInfo( const TCertInfo*  aCertInfo )
{
    // set cert info
    iCertInfo = aCertInfo;
}

void CWmlControl::WmlParameters( const char* data, TInt length, TDesC& contentType, TDesC& charsetType, TInt httpStatus, TInt chunkIndex, TDesC& url  )
{
    TUint8  method = 1;
    TUint16 charset = 64;
    TInt err = 0;

    TInt contTypeLen = contentType.Length() + 1;
    TUint8 *contType = new(ELeave)TUint8[contTypeLen];
    TPtr8 contType8(contType, contTypeLen );
    contType8.Copy( contentType );
    contType8.ZeroTerminate();


    TInt charSetLen = charsetType.Length() + 1;
    TUint8 *charSet = new(ELeave)TUint8[charSetLen];
    TPtr8 charSet8(charSet, charSetLen );
    charSet8.Copy( charsetType );
    charSet8.ZeroTerminate();

    TPtr8 myFake (0, 0);
    
    for (TInt i=0; i< m_contentArray.Count(); i++){
        if((m_contentArray[i]->getUrl()->Des()).operator==(url) ){
            iWmlContentInterface = m_contentArray[i];
        }
    }
    if (iWmlContentInterface){
		if (chunkIndex >= 0) {
			TUint8 *body = new(ELeave)TUint8[length];
			NW_Mem_memcpy(body, data, length);
			TPtr8 bodyPtr((unsigned char*) body, length);
			bodyPtr.SetLength(length);
			WMLPartialResponse( iWmlContentInterface,
												  (const TUint16*)(iWmlContentInterface->getUrl()->Des()).Ptr(),
        									bodyPtr,                        
        									NULL,                           
								(TUint8 *)contType8.Ptr(),                      
										 (TUint8 *)myFake.Ptr(),                           
										 EFalse,                         
										 NULL,                           
										 NULL,                           
										 charset,                       
										 (TUint8 *)charSet8.Ptr(),      
										 httpStatus,                   
										 method,                       
										 iWmlContentInterface->getTransId(),  
										 chunkIndex,                     
										 NULL,                           
										 length,                         
										 err,                            
										 iWmlContentInterface->getLoadContext(), 
										 iWmlContentInterface->getPartialCallBack() 
							  );
		  }
		  else {
			TPtr8 myData8 ((unsigned char*)data, length);
			WMLPartialResponse( iWmlContentInterface,
												  (const TUint16*)(iWmlContentInterface->getUrl()->Des()).Ptr(),
        									myData8,                        
        									NULL,                           
								NULL,                      
										 NULL,                      
										 EFalse,                    
										 NULL,                      
										 NULL,                      
										 charset,                   
										 NULL,                      
										 httpStatus,                
										 method,                    
										 iWmlContentInterface->getTransId(),  
										 chunkIndex,                     
										 NULL,                           
										 length,                         
										 err,                            
										 iWmlContentInterface->getLoadContext(), 
										 iWmlContentInterface->getPartialCallBack() 
							  );
		  }
		  }
}



TSize CWmlControl::WMLDocumentSize()
{
    TSize size(0, 0);
    if (iWmlView) {
        iWmlView->ContentSize(size);
    }
    return size;
}

TPoint CWmlControl::WMLDocumentPosition()
{
    if (iWmlView) {
        return TPoint(iWmlView->GetDeviceContext()->Origin()->x, iWmlView->GetDeviceContext()->Origin()->y);
    }
    return TPoint(0, 0);
}




/**
* Only following function is exported from the Dynamic DLL.
*/

EXPORT_C MWmlEngineInterface* NewWmlEngineL()
{
 return new (ELeave) CWmlControl;
}

// End of File
