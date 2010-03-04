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

#ifndef WMLCONTROL_H
#define WMLCONTROL_H

//  INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <brctldefs.h>

#include "BrCtl.h"
#include "WmlEngineInterface.h"
#include "WmlContentInterface.h"

// CONSTANTS
_LIT ( KCrLf, "\r\n");
_LIT ( KHeaderBoundary, "\r\n\r\n" );
_LIT8( KHeaderBoundary8, "\r\n\r\n" );
_LIT ( KHeaderContentLocation, "Content-Location: " );
_LIT8( KHeaderSavePageProto, "SAVED-PAGE/3.1" );
_LIT8( KMultipartBoundaryMarker, "multipart/mixed; boundary=" );
_LIT8( KPartHeadersData, "Content-Location: %S\r\nContent-Type: %S;charset=%S\r\n\r\n" );
_LIT8( KPartHeadersImg, "Content-Location: %S\r\nContent-Type: %S\r\n\r\n" );
_LIT ( KSavePageDocTop, "SAVED-PAGE/3.1\r\nContent-Location: " );
_LIT8( KSavePageBoundary, "S60BrowserSavedPage" );
_LIT8( KSavedPageContentType, "multipart/mixed" );

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CView;
class CShell;

// CLASS DECLARATION
/**
*
*  @lib resLoader.lib
*  @since 3.0
*/
 NONSHARABLE_CLASS(CWmlControl): public CBase, public MWmlEngineInterface 
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CWmlControl();

    public: // new functions

        /**
        * StartLoadingResource
        * Public Class Method
        * Purpose
        * @since 3.0
        * @param MWebCoreResourceLoader& aWebCoreLoader
        * @param const TDesC& aURL
        * @param CArrayFix<TWebCoreKeyValuePair>* aCustomHeaders
        * @return MWebCoreResourceHandle*
        */
        CWmlResourceLoadListener* LoadResourceL( const TDesC& aURL, void* aLoadContext,
                        void* aPartialLoadCallback, TUint16 aTransId );

        // Returns a WmlView reference
        CView* WmlView() {return iWmlView;}
        // Returns a WmlShell reference
        CShell* WmlShell() {return iWmlShell;}
        // Returns a WmlMode to set the right control
        TBool WmlMode() { return iWmlMode; }
		// Returns a WmlLink
        TBool WmlLink() { return iWmlLink; }
        // Returns the Application Id
        TInt AppId()  { return iApId; }
        // Sets the AppId
        void SetAppId(TInt aApId){ iApId = aApId; }
        // Sets a WmlMode
        void SetWmlMode(TBool aMode) { iWmlMode = aMode; }
		// Sets the WmlLink
        void SetWmlLink(TBool aLink) { iWmlLink = aLink; }
        // Returns to the Browser Control info about the wml page
        HBufC* PageInfoLC(TBrCtlDefs::TBrCtlPageInfo aBrCtlPageInfo);

        /**
        * Update a Browser setting
        * @since 3.1
        * @param aSetting The setting to update
        * @param aValue The new value of the setting
        * @return void
        */
        void SetBrowserSettingL(TUint aSetting, TUint aValue);

        /**
        * Sets a pointer to the active history controller
        * @since 3.1
        * @return void
        */
        void SetHistoryController( HistoryControllerInterface* aHistoryController ) { iHistoryController = aHistoryController; }

		/**
		* return history controller
		* @return history controller
		*/
        HistoryControllerInterface& HistoryController() const { return *iHistoryController; }


      /**
        * Accessor function to the MWKWmlInterface.
        * @since 3.1
        */

		

		CBrCtl*	WmlControlGetBrCtl() const { return	iBrCtl; }

		MWmlInterface* WKWmlInterface() {return iWKWmlInterface; }

		void InputElementEditComplete( TBool aInputConfirmed );
		void GainFocus();

		void LoseFocus();

		TInt HandleDo( TUint32 aElemId );

		TInt ProcessBack();

		CArrayFixFlat<TBrCtlImageCarrier>* GetPageImagesL();

		TBrCtlDefs::TBrCtlElementType GetFocusedElementType();

		TWmlEngineDefs::THistoryStackPosition HistoryStackPosition();

		void MakeVisible(TBool aVisible);

		void SetFocus( TBool aFocus, TDrawNow aDrawNow );

		void SetRect( const TRect &aRect );


		//TBrowserStatusCode

		TInt WMLPartialResponse( CWmlContentInterface* aWmlContentInterface,
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
			                                void* aPartialCallback );

        CCoeControl* coeControl();
		void WmlLoadUrlL( const TDesC& aUrl);
        void LoadImages();
		TRect WmlViewRectangle();
        void ScrollTo(TPoint aPoint);
		CWmlControl();
  void SetCertInfo( const TCertInfo*  aCertInfo );
  const TCertInfo* CertInfo() const { return iCertInfo; }
      void* loadContext() {return iWmlContentInterface->getLoadContext();}
      void* partialLoadCallback() {return iWmlContentInterface->getPartialCallBack();}
      TUint16 transId() {return iWmlContentInterface->getTransId();}

      void WmlParameters( const char* data, int length, TDesC& contentType, TDesC& charset, TInt httpStatus, TInt chunkIndex, TDesC& url );
      virtual TSize WMLDocumentSize();
      virtual TPoint WMLDocumentPosition();
		
    private:

        /**
        * C++ default constructor.
        */
        //CWmlControl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        virtual void ConstructL( CBrCtl* aBrCtl, TRect& aRect, MWmlInterface* aWKWmlInterface);

        /**
        * For saving is WML page. The function is same as that of
        * CWebKitControl::GenerateSavedPageL().
        */

        HBufC* WMLGenerateSavedPageL();
    private:
		//
        CShell*     	iWmlShell;    // owned
        //
        CView*      	iWmlView;     // owned
        //
        CBrCtl*			iBrCtl;
        //
        TInt 			iApId;
        //
        TBool			iWmlMode;
        //
	    TBool			iWmlLink;
        //
        const TCertInfo*  iCertInfo;         // owned
        HistoryControllerInterface* iHistoryController;
        MWmlInterface* iWKWmlInterface;
	       CWmlContentInterface* iWmlContentInterface;
        RPointerArray<CWmlContentInterface> m_contentArray;
    };

#endif      // WMLCONTROL_H

// End of File
