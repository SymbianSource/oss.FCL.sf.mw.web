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
* Description:  
*
*/

#ifndef WMLENGINEINTERFACE_H
#define WMLENGINEINTERFACE_H

//  INCLUDES

#include <e32base.h>
#include <coedef.h>
#include "BrCtlDefs.h"
#include "WmlEngineDefs.h"


// CONSTANTS


//Forword declrartion;
class CBrCtl;
class MWmlInterface;
class CView;
class CShell;
class HistoryController;
class CWmlContentInterface;
class CWmlDispatcher;
class CWmlResourceLoadListener;
class TBrCtlImageCarrier;
class CCoeControl;
//class THistoryStackPosition;
class TCertInfo;


// CLASS DECLARATION
/**
*
*  @lib resLoader.lib
*  @since 3.0
*/

class MWmlEngineInterface 
    {
    public: 
	  //Only the seconds phase contruction is done here.
      virtual void ConstructL( CBrCtl* aBrCtl, TRect& aRect, MWmlInterface* aWmlInterface ) = 0;

	  inline virtual ~MWmlEngineInterface() {};
	  
	  virtual CWmlResourceLoadListener* LoadResourceL(  const TDesC& aUrl, void *aLoadContext,
                        void * /*NW_Url_RespCallback_t*/ aPartialLoadCallback, TUint16 aTransId) = 0;

	  virtual CCoeControl* coeControl() = 0;

	  virtual TBool WmlLink() = 0;

	  virtual TInt AppId() = 0;

	  virtual void SetAppId( TInt aApId ) = 0;

	  virtual void SetWmlLink( TBool aLink ) = 0 ;

	  virtual HBufC* PageInfoLC( TBrCtlDefs::TBrCtlPageInfo aBrCtlPageInfo ) = 0;

      virtual void SetBrowserSettingL( TUint aSetting, TUint aValue ) = 0;

	  virtual void SetHistoryController( HistoryControllerInterface* aHistoryController ) = 0;

	  virtual HistoryControllerInterface& HistoryController() const = 0;

	  virtual void InputElementEditComplete( TBool aInputConfirmed ) = 0;

	  virtual void GainFocus() = 0;

	  virtual void LoseFocus() = 0;

	  virtual TInt ProcessBack() = 0;

	  virtual TInt HandleDo( TUint32 aElemId ) = 0;

	  virtual CArrayFixFlat<TBrCtlImageCarrier>* GetPageImagesL() = 0;

	  virtual TBrCtlDefs::TBrCtlElementType GetFocusedElementType() = 0;

	  virtual TWmlEngineDefs::THistoryStackPosition HistoryStackPosition() = 0;

	  virtual void MakeVisible(TBool aVisible) = 0;

	  virtual void SetFocus( TBool aFocus, TDrawNow aDrawNow ) = 0;

	  virtual void SetRect( const TRect &aRect ) = 0;

	  //TBrowserStatusCode

	  virtual 	TInt WMLPartialResponse( CWmlContentInterface* aWmlContentInterface,
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
			                                void* aPartialCallback ) = 0;

	  virtual void WmlLoadUrlL( const TDesC& aUrl ) = 0;
	  virtual void LoadImages() = 0;
		
	  virtual TRect WmlViewRectangle() = 0;

      virtual void ScrollTo(TPoint aPoint) = 0;
   virtual void SetCertInfo( const TCertInfo*  aCertInfo ) = 0;
   virtual const TCertInfo* CertInfo() const = 0;
	  
      virtual void* loadContext() = 0;
      virtual void* partialLoadCallback() = 0;
      virtual TUint16 transId() = 0;

      virtual void WmlParameters( const char* data, int length, TDesC& contentType, TDesC& charset, TInt httpStatus, TInt chunkIndex, TDesC& url  ) = 0;	
      virtual TSize WMLDocumentSize() = 0;
      virtual TPoint WMLDocumentPosition() = 0;

    private:       
       

    };

#endif      // WMLDISPATCHER_H            
// End of File
