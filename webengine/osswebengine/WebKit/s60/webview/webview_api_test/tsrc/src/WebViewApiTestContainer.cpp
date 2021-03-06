/*
* ============================================================================
*  Name     : CWebViewApiTestContainer from BrCtlBCTestContainer.h
*  Part of  : BrCtlBCTest
*  Created  : 04/17/2006 by
*  Implementation notes:
*     Initial content was generated by Series 60 AppWizard.
*  Version  :
*  Copyright:
* ============================================================================
*/

// INCLUDE FILES
#include "WebViewApiTestContainer.h"
#include <BrCtl.h>
#include "WebFrame.h"
#include "WebFrameView.h"

#include <String.h>
#include <StringLoader.h>
#include "PlatformString.h"
#include <coecntrl.h>
#include <e32std.h>
#include "NormalHardcodedAssert.h"


// CONSTANTS
using namespace WebCore;
_LIT8( KContentDataType, "text/html" );




// ================= MEMBER FUNCTIONS =======================


CWebViewApiTestContainer* CWebViewApiTestContainer::NewL()
{
    CWebViewApiTestContainer* self = new (ELeave) CWebViewApiTestContainer();
    CleanupStack::PushL( self );
    TPoint base( 0, 0 );
    TPoint extent( 352, 295 );
    TRect rect( base, extent );
    self->ConstructL( rect );
    CleanupStack::Pop(); // self

    return self;
}


// ---------------------------------------------------------
// CWebViewApiTestContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CWebViewApiTestContainer::ConstructL(const TRect& aRect)
    {
    //for BrCtl
    iBrCtlApiTestObserver = CBrCtlApiTestObserver::NewL();
    iBrCtlDialogs = CBrowserDialogsProviderProxy::NewL();
    iCommandBase = TBrCtlDefs::ECommandIdBase;
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
    CreateBrowserControlInterfaceL(aRect);
    }

// Destructor
CWebViewApiTestContainer::~CWebViewApiTestContainer()
    {
    delete iBrCtlApiTestObserver;
    delete iBrCtlInterface;
    delete iBrCtlDialogs;
    delete iView;
    }

void CWebViewApiTestContainer::CreateBrowserControlInterfaceL(const TRect& aRect)
{
    iBrCtlInterface = CreateBrowserControlL( this, aRect,
        TBrCtlDefs::ECapabilityDisplayScrollBar
            | TBrCtlDefs::ECapabilityClientResolveEmbeddedURL
            | TBrCtlDefs::ECapabilityLoadHttpFw
            | TBrCtlDefs::ECapabilitySavedPage
            | TBrCtlDefs::ECapabilityDisplayScrollBar
            | TBrCtlDefs::ECapabilityGraphicalPage,
        iCommandBase, iBrCtlApiTestObserver, iBrCtlApiTestObserver,
        iBrCtlApiTestObserver, iBrCtlApiTestObserver, iBrCtlDialogs, NULL, iBrCtlApiTestObserver );

    iBrCtlInterface->MakeVisible( ETrue );
}

TInt CWebViewApiTestContainer::CreateWebViewL()
{
	TInt err( KErrNone );
	
  	//iView = (static_cast<CBrCtl*>(iBrCtlInterface))->webView();
  	//iView = WebView::NewL(*this, static_cast<CBrCtl*>(iBrCtlInterface) );
  	TRAP( err,
        iView = WebView::NewL(*this, static_cast<CBrCtl*>(iBrCtlInterface) ));
  	if ( err ){
    	delete iView;
      	iView = 0;
    }
    return err;
}

void CWebViewApiTestContainer::MakeWebViewVisible(TBool visible)
{
	if(iView){
		iView->MakeVisible(visible);
	}
}

void CWebViewApiTestContainer::MakeViewVisible(TBool visible)
{
	if(iView){
		iView->MakeViewVisible(visible);;
	}
}

TInt CWebViewApiTestContainer::mainFrame()
{
	WebFrame* frame;
	if(iView){
		frame = iView->mainFrame();
	}
  return KErrNone;
}

TInt CWebViewApiTestContainer::mainFrameDocument()
{
	WebCore::DOMDocument* document;
	if(iView){
		document = iView->mainFrameDocument();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::page()
{
	Page* page;
	if(iView){
		page = iView->page();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::isMIMETypeRegisteredAsPlugin()
{
	//fix me empty & non empty string to be tested
	TBool mimeType;

	if(iView){
		mimeType = iView->isMIMETypeRegisteredAsPlugin(String());
	}
	if ( !mimeType ){
  		return KErrNone;
    }
    else{
  		return KErrGeneral;
  	}
}

TInt CWebViewApiTestContainer::pluginForExtension()
{
	//fix me empty & non empty string to be tested
	CPluginHandler* plugin = NULL;
	if(iView){
		plugin = iView->pluginForExtension(String());
	}
	if ( plugin == NULL ){
  		return KErrNone;
    }
    else{
  		return KErrGeneral;
  	}
}

TInt CWebViewApiTestContainer::loadBackForwardListFromOtherView()
{
/*	TInt err( KErrNone );
	CBrCtlApiTestContainer* brCtlTestContainer;
	WebView* otherView;
	TPoint base( 1, 1 );
    TPoint extent( 250, 190 );
    TRect rect( base, extent );
    TRAP( err,
      brCtlTestContainer = new (ELeave) CBrCtlApiTestContainer;
      brCtlTestContainer->ConstructL( rect );
      brCtlTestContainer->CreateBrowserControlInterfaceL();
      );
    if ( err ){
        delete brCtlTestContainer;
    }

	TRAP( err,
		otherView = WebView::NewL(*brCtlTestContainer, static_cast<CBrCtl*>(brCtlTestContainer->BrCtlInterface()));
		);
	if ( err ){
        delete otherView;
    }
*/
	//not implemented in webview
	/*if(iView){
		iView->loadBackForwardListFromOtherView(otherView);
	}*/

	return KErrNone;
}

TInt CWebViewApiTestContainer::shouldClose()
{
	TBool result;
	if(iView){
		result = iView->shouldClose();
	}
	//STIF_ASSERT_TRUE(result);
	return KErrNone;
}

TInt CWebViewApiTestContainer::isLoading()
{
	if(iView){
		iView->isLoading();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::syncRepaint()
{
	TPoint base( 0, 0 );
    TPoint extent( 250, 190 );
    const TRect rect( base, extent );
	if(iView){
		iView->syncRepaint(rect);
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::scheduleRepaint()
{
	TPoint base( 0, 0 );
    TPoint extent( 250, 190 );
    const TRect rect( base, extent );
	if(iView){
		iView->scheduleRepaint(rect);//async
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::doRepaint()
{
	if(iView){
		iView->doRepaint();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::pageLoadFinished()
{
	if(iView){
		iView->pageLoadFinished();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::updatePageScaler()
{
	if(iView){
		iView->updatePageScaler();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::doLayout()
{
	if(iView){
		//iView->doLayout();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::maxBidiSize()
{
	TSize size;
	TSize zerosize = TSize();
	TInt err(KErrNone);
	if(iView){
		size = iView->maxBidiSize();
	}
	if(size == zerosize){
		err = KErrGeneral;
	}
	return err;
}

TInt CWebViewApiTestContainer::clearOffScreenBitmap()
{
	if(iView){
		iView->clearOffScreenBitmap();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::scrollBuffer()
{
	TPoint aFrom(1,1);
	TPoint aTo(100,100);

	if(iView){
		iView->scrollBuffer(aTo,aFrom, ETrue);
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::offscreenRect()
{
	TRect rect;
	TInt err(KErrNone);
	if(iView){
		rect = iView->offscreenRect();
	}

	//if(rect.IsEmpty()){
	//	err = KErrGeneral;
	//}
	return err;
}


TInt CWebViewApiTestContainer::pageScaler()
{
	TInt err(KErrNone);
	CPageScaler* pagescaler;

	if(iView){
		pagescaler = iView->pageScaler();
		if(pagescaler == NULL){
			err = KErrGeneral;
		}
	}
	else{
		err = KErrGeneral;
	}

	return err;
}


TInt CWebViewApiTestContainer::scalingFactor()
{
	TInt err(KErrNone);

	if(iView){
		if(iView->scalingFactor() == 0){
			err = KErrGeneral;
		}
	}
	else{
		err = KErrGeneral;
	}

	return err;
}

TInt CWebViewApiTestContainer::openUrl()
{
	if(iView){
		//iView->openUrl( (TDesC16 &)KUrlLoadTestHtml() );
	}

	return KErrNone;
}

TInt CWebViewApiTestContainer::brCtl()
{
	TInt err(KErrNone);
	
	if(iView){
		CBrCtl* brctl = iView->brCtl();
		if(!brctl){
			err = KErrGeneral;
		}	
	}
	
	return err;
}

TInt CWebViewApiTestContainer::fepTextEditor()
{
	TInt err(KErrNone);
	
	if(iView){
		CWebFepTextEditor* editor = iView->fepTextEditor();
		if(!editor){
			err = KErrGeneral;
		}	
	}
	
	return err;
}

TInt CWebViewApiTestContainer::formFillPopup()
{	
	if(iView){
		iView->formFillPopup();	
	}
	
	return KErrNone;
}
 
TInt CWebViewApiTestContainer::setFormFillPopup()
{	
	if(iView){
		iView->setFormFillPopup(NULL);
	}		
	return KErrNone;
}

TInt CWebViewApiTestContainer::setEditable()
{	
	TInt err(KErrNone);
	if(iView){
		iView->setEditable(ETrue);
		if(!iView->isEditable()){
			err = KErrGeneral;
		}
	}		
	return err;
}

TInt CWebViewApiTestContainer::isEditable()
{	
	if(iView){
		iView->isEditable();
	}		
	return KErrNone;
}

TInt CWebViewApiTestContainer::OfferKeyEvent()
{
    TKeyEvent key;
    key.iCode = 0;
    key.iScanCode = 0;
    key.iModifiers = 0;
    key.iRepeats = 0;
    
    TRAPD( err, iView->OfferKeyEventL( key, EEventNull ) );
    
    return err;
}

TInt CWebViewApiTestContainer::setFocusedElementType()
{	
	TInt err(KErrNone);
	if(iView){
		iView->setFocusedElementType(TBrCtlDefs::EElementNone);
		if(iView->focusedElementType() != TBrCtlDefs::EElementNone){
			err = KErrGeneral;
		}
	}		
	return err;
}

TInt CWebViewApiTestContainer::focusedElementType()
{	
	if(iView){
		iView->focusedElementType();
	}		
	return KErrNone;
}

TInt CWebViewApiTestContainer::pageScrollHandler()
{	
	TInt err(KErrNone);
	if(iView){
		WebPageScrollHandler* handler = iView->pageScrollHandler();
		if(!handler){
			err = KErrGeneral;
		}
	}		
	return err;
}

 
TInt CWebViewApiTestContainer::pageFullScreenHandler()
{	
	TInt err(KErrNone);
	if(iView){
		WebPageFullScreenHandler* handler = iView->pageFullScreenHandler();
		if(!handler){
			err = KErrGeneral;
		}
	}		
	return err;
}

TInt CWebViewApiTestContainer::pointerEventHandler()
{	
	TInt err(KErrNone);
	if(iView){
		WebPointerEventHandler* handler = iView->pointerEventHandler();
		if(!handler){
			err = KErrGeneral;
		}
	}		
	return err;
}

TInt CWebViewApiTestContainer::updateScrollbars()
{	
	if(iView){
		iView->updateScrollbars(iView->Rect().Height(),
                				iView->mainFrame()->frameView()->contentPos().iY, 
                				iView->Rect().Width(),
                				iView->mainFrame()->frameView()->contentPos().iX);
	}		
	return KErrNone;
}

TInt CWebViewApiTestContainer::openPageView()
{	
	TInt err(KErrNone);
	if(iView){
		TRAP( err,iView->openPageViewL() );
	}		
	return err;
}

TInt CWebViewApiTestContainer::closePageView()
{	
	if(iView){
		iView->closePageView();
	}		
	return KErrNone;
}

TInt CWebViewApiTestContainer::cancelPageView()
{	
	if(iView){
		iView->cancelPageView();
	}		
	return KErrNone;
}


TInt CWebViewApiTestContainer::pageView()
{	
	if(iView){
		iView->pageView();
	}		
	return KErrNone;
}

TInt CWebViewApiTestContainer::inPageViewMode()
{	
	if(iView){
		iView->inPageViewMode();
	}		
	return KErrNone;
}

TInt CWebViewApiTestContainer::searchFor()
{	
	_LIT8( KKeyWord, "test" );
	TInt err(KErrNone);
	if(iView){
		openUrl();
		if(!TBrCtlDefs::EFindNoMatches == iView->searchFor(NULL)){
//			err = KErrGeneral;
		}
		if(TBrCtlDefs::EFindNoMatches == iView->searchFor((TDesC16 &)KKeyWord())){
//			err = KErrGeneral;
		}
//		searchAgain();
		iView->exitFindState();
	}		
	return err;
}

TInt CWebViewApiTestContainer::searchAgain()
{
	if(iView){
		iView->searchAgain(ETrue);
	}		
	return KErrNone;
}

TInt CWebViewApiTestContainer::exitFindState()
{
	_LIT8( KKeyWord, "test" );
	TInt err(KErrNone);
	if(iView){
		iView->searchFor((TDesC16 &)KKeyWord());
		iView->exitFindState();
		if(iView->inFindState()){
			err = KErrGeneral;
		}
	}		
	return err;;
}

TInt CWebViewApiTestContainer::inFindState()
{
	if(iView){
		iView->inFindState();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::pageScalerEnabled()
{
	TInt err(KErrNone);
	if(iView){
		if(!iView->pageScalerEnabled())
			err = KErrGeneral;
	}
	return err;
}

TInt CWebViewApiTestContainer::isSmallPage()
{
	if(iView){
		iView->isSmallPage();
	}
	return KErrNone;
}

TInt CWebViewApiTestContainer::willSubmitForm()
{
	if(iView){
//		iView->willSubmitForm();
	}
	return KErrNone;
}































/*************************************************************************/
//BrCtrl methods

void CWebViewApiTestContainer::CancelLoad()
{}

void CWebViewApiTestContainer::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
{}

void CWebViewApiTestContainer::Draw(const TRect& /*aRect*/) const
{}

void CWebViewApiTestContainer::HandleCommandL(
    TBrCtlDefs::TBrCtlClientCommands /* aCommand */,
    const CArrayFix<TPtrC>& /* aAttributesNames */,
    const CArrayFix<TPtrC>& /* aAttributeValues */ )
{}

TKeyResponse CWebViewApiTestContainer::OfferKeyEventL(const TKeyEvent& /* aKeyEvent */,TEventCode /* aType */ )
{
	return EKeyWasConsumed;
}

CCoeControl* CWebViewApiTestContainer::ComponentControl(TInt aIndex) const
{
    switch ( aIndex )
        {
        case 0:
            return iBrCtlInterface; // Could be NULL
        default:
            return NULL;
        }
}

TInt CWebViewApiTestContainer::CountComponentControls() const
{
    if (iBrCtlInterface)
        return 1;
    return 0;
}

void CWebViewApiTestContainer::SizeChanged()
{
	if (iBrCtlInterface)
        {
        iBrCtlInterface->SetRect(Rect());
        }
}

// End of File
