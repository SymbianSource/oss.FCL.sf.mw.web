/*
* Copyright (C) 2005, 2006, 2007 Apple Inc. All rights reserved.
* Copyright (C) 2006 David Smith (catfish.man@gmail.com)
* Copyright (C) 2007 Nokia
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1.  Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
* 2.  Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
* 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
*     its contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <Browser_platform_variant.hrh>
#include "config.h"
#include "../../bidi.h"
#include "brctl.h"
#include "HistoryController.h"
#include "PageView.h"
#include "SettingsContainer.h"
#include "WebFrame.h"
#include "WebFrameBridge.h"
#include "WebFrameView.h"
#include "WebView.h"
#include "WebTabbedNavigation.h"
#include "WebChromeClient.h"
#include "WebCoreGraphicsContext.h"
#include "WebContextMenuClient.h"
#include "WebEditorClient.h"
#include "WebDragClient.h"
#include "WebFrameLoaderClient.h"
#include "WebInspectorClient.h"
#include "WebFepTextEditor.h"
#include "WebSurface.h"
#include "WebCursor.h"
#include "WebFormFill.h"
#include "WebFormFillPopup.h"
#include "WebToolBarInterface.h"
#include "WebPointerEventHandler.h"
#include "WebPageScrollHandler.h"
#include "WebPopupDrawer.h"
#include "PluginSkin.h"
#include "PluginWin.h"
#include "PluginPlayer.h"


#include "Page.h"
#include "Settings.h"
#include "Frame.h"
#include "FrameView.h"
#include "FrameTree.h"
#include "FrameLoader.h"
#include "HistoryItem.h"
#include "EventHandler.h"
#include "ResourceRequest.h"
#include "PlatformString.h"
#include "SelectionController.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformScrollbar.h"
#include "DocumentLoader.h"
#include "StaticObjectsContainer.h"
#include "ToolBar.h"
#include "Document.h"
#include "FocusController.h"
#include "WebUtil.h"
#include "WidgetExtension.h"
#include "Cache.h"
#include "RenderWidget.h"

#include <AknUtils.h>
#include <CUserAgent.h>
#include "WebPageZoomHandler.h"

#include "PlatformFontCache.h"
#include "WebPageFullScreenHandler.h"
#include "eikon.hrh"
#include "WebScrollbarDrawer.h"


using namespace WebCore;

const int KRepaintDelayLoading = 500*1000; // dont do repaints more often than this during loading (0.5s)
const int KRepaintDelayComplete = 100*1000; // faster updates after load so dynamic scripts etc have better frame rate (0.1s)
const int KNormalScrollRange = 60;

const int KPanningStartSpeed = 30;
const int KPanningStartTime = 900;
const int KPanningPageScalerStart = 1300;
const int KPanningMaxSpeed = 90;
const int KPanningMaxTime = 3000;
const int KAllowSelection = 1;

const int KScalerVisibilityTime = 1100*1000; //1.1s

const int KCursorUpdateFrquency = 20*1000; // 20ms
const int KCursorInitialDelay = 200*1000;  // 200ms
const TKeyEvent KNullKeyEvent = {0,0,0,0};
const int KSmallPageScale = 13*13;    // if the area of page is less than 169% (130%*130%) of the viewport area, it's considered to be a small page
const int KZoomLevelDelta = 10; //set 10% for each increasment

const int KZoomLevelDefaultValue = 100;
const int KZoomLevelMaxValue = 200;
const int KZoomLevelMinValue = 10;

const int KZoomBgRectColor = 209;
const int KZoomDefaultLevel = 8; //100%
const int defaultCacheCapacity = 256 * 1024;

// LOCAL FUNCTION PROTOTYPES
TInt doRepaintCb( TAny* ptr );
TInt doFepCb( TAny* ptr );

static WebFrame* incrementFrame(WebFrame* curr, bool forward, bool wrapFlag)
{
    Frame* coreFrame = core(curr);
    return kit(forward
        ? coreFrame->tree()->traverseNextWithWrap(wrapFlag)
        : coreFrame->tree()->traversePreviousWithWrap(wrapFlag));
}



// -----------------------------------------------------------------------------
// CWebKitView::NewL
// Public Class Method
// Two-phased constructor.
// -----------------------------------------------------------------------------
WebView* WebView::NewL(
                       CCoeControl& parent, CBrCtl* brctl )
{
    WebView* self = new (ELeave) WebView( brctl );

    CleanupStack::PushL( self );
    self->ConstructL( parent );
    CleanupStack::Pop(); // self

    return self;
}

// -----------------------------------------------------------------------------
// CWebKitView::CWebKitView
// Private Class Method
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
WebView::WebView( CBrCtl* brctl ) :
m_brctl(brctl)
, m_isEditable(false)
, m_currentEventKey(KNullKeyEvent)
, m_scrollingSpeed(KNormalScrollRange*KZoomLevelDefaultValue/100)
, m_focusedElementType(TBrCtlDefs::EElementNone)
, m_pageScaler(NULL)
, m_pageScalerEnabled(false)
, m_inFindState(false)
, m_pageView(NULL)
, m_savedCursorPosition(KInitialOffset, KInitialOffset)
, m_findKeyword(NULL)
, m_webFormFill(NULL)
, m_fepTimer(0)
, m_isClosing(false)
, m_widgetextension(0)
, m_userAgent(NULL)
, m_pageZoomHandler(NULL)
, m_currentZoomLevel(KZoomLevelDefaultValue)
, m_lastZoomLevel(KZoomLevelMinValue)
, m_maxZoomLevel(KZoomLevelMaxValue)
, m_minZoomLevel(KZoomLevelMinValue)
, m_defaultZoomLevel(KZoomLevelDefaultValue)
, m_pageFullScreenHandler(NULL)
, m_viewIsScrolling(false)
, m_ptrbuffer(0)
, m_pluginActivated(false)
, m_showCursor(false)
, m_allowRepaints(true)
{
}

WebView::~WebView()
{
    // the zoom handler is a client of WebView (also owned by
    // WebView--a circular dependency) so it must be deleted before
    // the WebView object is destroyed because in its destructor it
    // operates on the WebView object
    delete m_pageZoomHandler;
    m_pageZoomHandler = NULL;

    // prevent frameViews to access members when topView is
    // closing down.
    m_isClosing = true;
    m_page->setGroupName(String());

    if (m_fastScrollTimer)
        m_fastScrollTimer->Cancel();
    delete m_fastScrollTimer;

    delete [] m_ptrbuffer;
    delete m_repainttimer;
    delete m_webfeptexteditor;
    delete m_webcorecontext;
    delete m_bitmapdevice;
    delete m_page;
    delete m_pageScaler;
    delete m_pageView;
    delete m_webFormFill;
    delete m_toolbar;
    delete m_toolbarinterface;
    delete m_widgetextension;
    delete m_webpointerEventHandler;
    delete m_pageScrollHandler;
    delete m_pluginplayer;
    delete m_fepTimer;
    delete m_popupDrawer;
    delete m_tabbedNavigation;
    delete m_userAgent;
    delete m_pageFullScreenHandler;
}

// -----------------------------------------------------------------------------
// CWebKitView::ConstructL
// Private Class Method
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void WebView::ConstructL(
                         CCoeControl& parent)
{
    SetContainerWindowL(parent);

    CCoeControl::MakeVisible(ETrue);

    WebFrameView *frameView = new WebFrameView();
    m_page = new Page(new WebChromeClient(this), new WebContextMenuClient(), new WebEditorClient(this), new WebDragClient(), new WebInspectorClient());
    m_page->backForwardList()->setCapacity(0);
    m_page->setGroupName("com.s60.browser");
    //
    WebCore::Settings* settings = m_page->settings();
    settings->setLoadsImagesAutomatically(true);
    settings->setJavaScriptEnabled(true);

    settings->setMinimumFontSize(7);
    settings->setMinimumLogicalFontSize(9);
    settings->setDefaultFontSize(12);
    settings->setDefaultFixedFontSize(12);
    settings->setPluginsEnabled(true);
    settings->setJavaScriptCanOpenWindowsAutomatically(true);
    settings->setPluginsEnabled(true);

    m_tabbedNavigation = new WebTabbedNavigation(this);

    WebFrameBridge* bridge = new WebFrameBridge();
    bridge->initMainFrameWithPage(m_page, "", frameView);

    // Create and offscreen device and context
    m_bitmapdevice = CFbsBitmapDevice::NewL( StaticObjectsContainer::instance()->webSurface()->offscreenBitmap() );
    m_webcorecontext = WebCoreGraphicsContext::NewL( m_bitmapdevice, StaticObjectsContainer::instance()->webSurface()->offscreenBitmap(), mainFrame()->frameView());
    //
    m_repainttimer = CPeriodic::NewL(CActive::EPriorityHigh);

    m_webfeptexteditor = new CWebFepTextEditor(this);

    m_fastScrollTimer = CPeriodic::NewL(CActive::EPriorityHigh);

    m_dirtyZoomMode = false;
    m_currentZoomLevel = StaticObjectsContainer::instance()->fontCache()->fontZoomFactor();
    m_startZoomLevel = m_currentZoomLevel;

    m_pageZoomHandler = WebPageZoomHandler::NewL( *this );
    if ( brCtl()->capabilities()&TBrCtlDefs::ECapabilityFitToScreen ) {
        m_minZoomLevel = (KZoomLevelDefaultValue / m_pageZoomHandler->stepSize())*m_pageZoomHandler->stepSize();
        //could happen only if m_minZoomLevel < stepSize
        if (!m_minZoomLevel) m_minZoomLevel = m_pageZoomHandler->stepSize();
    }

    // construct the zoom array from the default level
    m_zoomLevelArray.Append(KZoomLevelDefaultValue);
    TInt toAddZoomLevel = KZoomLevelDefaultValue + KZoomLevelDefaultValue*KZoomLevelDelta/100;
    while (toAddZoomLevel <= m_maxZoomLevel)
        {
        // add the zoom level after default one
        m_zoomLevelArray.Append(toAddZoomLevel);
        toAddZoomLevel = toAddZoomLevel + toAddZoomLevel*KZoomLevelDelta/100;
        }

    // now go the minimum one
    toAddZoomLevel = KZoomLevelDefaultValue - KZoomLevelDefaultValue*KZoomLevelDelta/100;
    while (toAddZoomLevel >= m_minZoomLevel)
        {
        //add the zoom level after default one
        m_zoomLevelArray.Insert(toAddZoomLevel, 0);
        toAddZoomLevel = toAddZoomLevel - toAddZoomLevel*KZoomLevelDelta/100;
        }

    m_pageFullScreenHandler = WebPageFullScreenHandler::NewL( *this );

    if ( m_brctl->capabilities() & TBrCtlDefs::ECapabilityWebKitLite ) {
        m_pageScalerEnabled = false;
    }
    else  {
        initializePageScalerL();
        m_pageScalerEnabled = true;
    }
    if (m_brctl->capabilities() & TBrCtlDefs::ECapabilityAutoFormFill) {
        m_webFormFill = new WebFormFill(this);
    }

    // Create the PointerEventHandler
    m_ptrbuffer = new TPoint[256];
    m_webpointerEventHandler = WebPointerEventHandler::NewL(this);

    // Create the ScrollHandler
    m_pageScrollHandler = WebPageScrollHandler::NewL( *this );
    if (AknLayoutUtils::PenEnabled()) {
        DrawableWindow()->SetPointerGrab(ETrue);
        EnableDragEvents();
    }

    CUserAgent* usrAgnt = CUserAgent::NewL();
    CleanupStack::PushL( usrAgnt );
    HBufC8* userAgent8 = usrAgnt->UserAgentL();
    CleanupStack::PushL( userAgent8 );
    m_userAgent = HBufC::NewL(userAgent8->Length());
    m_userAgent->Des().Copy(userAgent8->Des());
    CleanupStack::PopAndDestroy(2); // userAgent8, usrAgnt

    MakeViewVisible(ETrue);
    m_isPluginsVisible=ETrue;
    CCoeControl::SetFocus(ETrue);

    cache()->setCapacities(0, 0, defaultCacheCapacity);
}

void WebView::initializePageScalerL()
{
    TBool lowQuality = !( m_brctl->capabilities() & TBrCtlDefs::ECapabilityGraphicalHistory );
    m_pageScaler = CPageScaler::NewL( *this, EColor64K, lowQuality );
    m_pageScaler->SetVisible( EFalse );
    //update the minimap support info from page scaler
    m_pageScalerEnabled = (m_pageScaler->HasOverlaySupport());
}


void WebView::fepTimerFired(Timer<WebView>*)
{
    CCoeEnv::Static()->SimulateKeyEventL(m_keyevent, m_eventcode);
}

//-------------------------------------------------------------------------------
// Draw ( from CCoeControl )
// BitBlts the offscreen bitmap to the GraphicsContext
//-------------------------------------------------------------------------------
void WebView::Draw(
                   const TRect& rect ) const
{
    if (m_pluginFullscreen) return;

    CWindowGc& gc = SystemGc();
    CEikBorderedControl::Draw( rect );

    // put offscreen bitmap onto the screen

    if ( !m_dirtyZoomMode ) {
        StaticObjectsContainer::instance()->webSurface()->flip( Rect().iTl, gc );
        if (m_widgetextension){
            m_widgetextension->DrawTransition(gc,StaticObjectsContainer::instance()->webSurface()->offscreenBitmap());
        }
    }
    else {
        gc.DrawBitmap( m_destRectForZooming, StaticObjectsContainer::instance()->webSurface()->offscreenBitmap(), m_srcRectForZooming );

        if ( m_startZoomLevel > m_currentZoomLevel) {

            TRect rectLeft( TPoint( rect.iTl.iX + m_destRectForZooming.Width() - 2, rect.iTl.iY ),
                            TPoint( rect.iBr ));

            TRect rectBottom( TPoint( rect.iTl.iX, rect.iTl.iY + m_destRectForZooming.Height() - 2 ),
                              TPoint( rect.iBr.iX + m_destRectForZooming.Width(), rect.iBr.iY ));


            const TRgb colorTest(KZoomBgRectColor,KZoomBgRectColor,KZoomBgRectColor);
            gc.SetPenColor(colorTest);
            gc.SetBrushStyle( CGraphicsContext::EForwardDiagonalHatchBrush );
            gc.SetBrushColor(colorTest);
            gc.DrawRect( rectLeft );
            gc.DrawRect( rectBottom );

        }

    }

    if (m_pageScalerEnabled && m_pageScaler->Visible()) {
        m_pageScaler->Draw( gc, rect );
    }

    if (m_toolbar) {
        m_toolbar->Draw();
    }

    if (m_popupDrawer)
    {
        m_popupDrawer->drawPopup();
    }

    if (m_widgetextension){
        m_widgetextension->DrawTransition(gc,StaticObjectsContainer::instance()->webSurface()->offscreenBitmap());
    }


}


WebCore::Page* WebView::page()
{
    return m_page;
}


WebFrame* WebView::mainFrame()
{
    if (!m_page)
        return NULL;
    return kit(m_page->mainFrame());
}

//-------------------------------------------------------------------------------
// SyncRepaint
// Repaint the given rectangle synchronously
//-------------------------------------------------------------------------------
void WebView::syncRepaint(
                          const TRect& rect)
{
    m_repaints.AddRect( rect );
    syncRepaint();
}

void WebView::MakeVisible(TBool visible)
{
    if (visible == IsVisible()) return;
    CCoeControl::MakeVisible(visible);
    MakeViewVisible(visible);
}

void WebView::MakeViewVisible(TBool visible)
{
    if ( visible ) {
        // resize the offscreen surface, this is needed
        // because the shared surface is changed.
        StaticObjectsContainer::instance()->webSurface()->setView( this );
        m_bitmapdevice->Resize( m_offscreenrect.Size() );
        m_webcorecontext->resized();
    }
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    if (cursor) {
        if (visible) {
            cursor->setCurrentView(*this);
            //Reset the iFocusedElementType to be the same as before the second window is opened.
            cursor->setPosition(m_savedCursorPosition);
            cursor->updatePositionAndElemType(m_savedCursorPosition);
        } else
            m_savedCursorPosition = cursor->position();
        cursor->cursorUpdate(visible & !AknLayoutUtils::PenEnabled());
    }
    Frame* f = m_page->mainFrame();
    while ( f ) {
        PassRefPtr<HTMLCollection> objects = f->document()->objects();
        for (Node* n = objects->firstItem(); n; n = objects->nextItem()) {
            MWebCoreObjectWidget* w = widget(n);
            if (w)
                w->makeVisible(visible);
        }
        PassRefPtr<HTMLCollection> embeds = f->document()->embeds();
        for (Node* n = embeds->firstItem(); n; n = embeds->nextItem()) {
            MWebCoreObjectWidget* w = widget(n);
            if (w)
                w->makeVisible(visible);
        }
        f = f->tree()->traverseNext();
    }
    if ( m_pageScaler )  {
        m_pageScaler->SetContainerRect(TRect(Rect().Size()));
    }
    if ( m_webFormFillPopup ){
        // don't show the popup if view's visibility changes.
        m_webFormFillPopup->MakeVisible(EFalse);
    }

    if ( visible ) {
      clearOffScreenBitmap();
      syncRepaint( mainFrame()->frameView()->visibleRect() );
    }

}



void WebView::doLayout()
{

    int zoomLevel = m_currentZoomLevel;
    if(!(   m_widgetextension && m_widgetextension->IsWidgetPublising())) {
        zoomLevelChanged( KZoomLevelDefaultValue );
    }
    Frame* f = m_page->mainFrame();

    while ( f ) {
        f->sendResizeEvent();
        f->view()->layout();
        f = f->tree()->traverseNext();
    }
        // maybe it got bigger in layout?
    TRect rect(m_repaints.BoundingRect());

    TPoint p( mainFrame()->frameView()->contentPos());

    rect.Move(-p);

      // draw to back buffer

   
    if(!(   m_widgetextension && m_widgetextension->IsWidgetPublising())) {
        mainFrame()->frameView()->draw( *m_webcorecontext, rect );
        if ( zoomLevel < m_minZoomLevel ) zoomLevel = m_minZoomLevel;
        zoomLevelChanged( zoomLevel );
    }
}
//-------------------------------------------------------------------------------
// SyncRepaint
// Repaint the current repaint region synchronously and clear it
//-------------------------------------------------------------------------------
void WebView::syncRepaint()
{
    if (m_pageScaler && m_pageScaler->FullScreenMode()) {
      ScaledPageChanged(m_pageScaler->Rect(), EFalse, EFalse);
      return;
    }

    if (!IsVisible()) {
      return;
    }

    if (isPluginFullscreen()) return;
    CFbsBitGc& gc = m_webcorecontext->gc();
    gc.Reset();

    TRect viewRect( mainFrame()->frameView()->visibleRect() );
    TPoint p( mainFrame()->frameView()->contentPos());
    Frame* f = m_page->mainFrame();
    bool layoutPending = false;
    while (f && !layoutPending) {
        layoutPending = f->view()->layoutPending();
        f = f->tree()->traverseNext();
    }
    if (!layoutPending) {
        bool needsDraw = false;
        m_repaints.Tidy();
        for (int i=0; i<m_repaints.Count(); ++i) {
            TRect r = m_repaints[i];
            if (r.Intersects(viewRect)) {
                r.Move(-p);
                mainFrame()->frameView()->draw( *m_webcorecontext, r );
                needsDraw = true;
            }
        }

        if (needsDraw){
            // blit the whole backbuffer to the screen (in Draw())
            // This could be optimized further to blit only the affected area.
            // Not a big win, scrolling is the most critical case and there you
            // need to blit the whole buffer anyway.
            m_brctl->DrawNow();
        }

        // dont do next async repaint until KRepaintDelay
        m_repaints.Clear();
    }
    m_repainttimer->Cancel();
    // tot:fixme TBool complete = iWebkitControl->IsProgressComplete(); && CImageRendererFactory::Instance()->DecodeCount()==0;

    TBool complete = ETrue;
    if ( !m_pageZoomHandler->isActive() && m_allowRepaints){
        m_repainttimer->Start(complete ? KRepaintDelayComplete : KRepaintDelayLoading,
        0, TCallBack( &doRepaintCb, this ) );
    }
    // need to draw formfill popup here.
    if (m_webFormFillPopup && m_webFormFillPopup->IsVisible()) {
        m_webFormFillPopup->reDraw();
    }
}

//-------------------------------------------------------------------------------
// DoRepaintCb
// C-style TCallback function
//-------------------------------------------------------------------------------
TInt doRepaintCb(
                 TAny* ptr )
{
    static_cast<WebView*>(ptr)->doRepaint();
    return EFalse;
}

//-------------------------------------------------------------------------------
// DoRepaint
// Perform a scheduled repaint
//-------------------------------------------------------------------------------
void WebView::doRepaint()
{
    // the timer must always be canceled here!
      m_repainttimer->Cancel();

    // only repaint when this view owns the surface
      if ( StaticObjectsContainer::instance()->webSurface()->topView() != this ) return;

    // anything to do?
      if ( !m_repaints.IsEmpty() ) {
          syncRepaint( );
      }
}

//-------------------------------------------------------------------------------
// CollectOffscreenbitmap
// Get offscreen bitmap
//-------------------------------------------------------------------------------
void WebView::collectOffscreenbitmapL(CFbsBitmap& snapshot)
{
   
    (snapshot).Create(m_brctl->Size(), StaticObjectsContainer::instance()->webSurface()->displayMode());

    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( &snapshot);
    CleanupStack::PushL(device);

    WebCoreGraphicsContext* gc = WebCoreGraphicsContext::NewL( device, &snapshot, mainFrame()->frameView());
    CleanupStack::PushL(gc);

    mainFrame()->frameView()->draw( *gc, mainFrame()->frameView()->visibleRect() );

    CleanupStack::PopAndDestroy(2);

}

//-------------------------------------------------------------------------------
// ScheduleRepaint
// Schedule an asynchronous repaint
//-------------------------------------------------------------------------------
void WebView::scheduleRepaint(
                              const TRect& rect )
{
    m_repaints.AddRect( rect );
    if(   m_widgetextension && m_widgetextension->IsWidgetPublising())
        {
#ifdef BRDO_WRT_HS_FF
        // Draw the miniview
        m_brctl->brCtlLayoutObserver()->NotifyLayoutChange(EOriginTopLeft);
#endif
        }
    // if we are active, we'll repaint when timer fires
    if( !m_repainttimer->IsActive() && !m_pageZoomHandler->isActive() && 
         m_allowRepaints) {
        // no timer yet, repaint immediatly (but asynchronously)
        m_repainttimer->Start( 0, 0, TCallBack( &doRepaintCb, this ) );
    }
}

void WebView::pageLoadFinished()
{

    if (m_brctl->settings() && m_brctl->settings()->getTabbedNavigation()) {
        m_tabbedNavigation->initializeForPage();
    }
    else {

        // Temp solution to fix a problem with scrolling large pages:
        // if user starts scrolling while loading a page - at the end of the load we should not restore
        // the content position from the history controller
        // As of now scroll events are not passed to WebCore and this will be changed in future
        // then this code should be replaces with the commented one below

        TPoint ptInit(0,0);
        TPoint ptFromHistory = m_brctl->historyHandler()->historyController()->currentEntryPosition();
        TPoint ptCurr = mainFrame()->frameView()->contentPos();

        if ( ptCurr != ptFromHistory ) {
            if ( ptInit == ptCurr ) {
                mainFrame()->frameView()->scrollTo(ptFromHistory);
            }
            else {
                m_brctl->historyHandler()->historyController()->updateCurrentEntryPositionIfNeeded();
            }
        }
        /*
        if ( !core(mainFrame())->view()->wasScrolledByUser())
            mainFrame()->frameView()->scrollTo(m_brctl->historyHandler()->historyController()->currentEntryPosition());*/

    }

    if (FrameLoadTypeStandard == core( mainFrame())->loader()->loadType()){
        if (m_brctl->capabilities()&TBrCtlDefs::ECapabilityFitToScreen){
            updateMinZoomLevel( mainFrame()->frameView()->contentSize());
        }
    }

    setHistoryLoad(false);
    setRedirectWithLockedHistory(false);

    int zoomLevel = m_brctl->historyHandler()->historyController()->currentEntryZoomLevel();
    if ( zoomLevel != m_currentZoomLevel) {
        m_brctl->historyHandler()->historyController()->updateCurrentEntryZoomLevelIfNeeded();
    }
    if ( m_pageScaler ) {
        m_pageScaler->DocumentCompleted();
        TRAP_IGNORE(m_pageScaler->DocumentChangedL());
    }

    Node* focusedNode = NULL;
    Frame* focusedFrame = page()->focusController()->focusedFrame();

    if (focusedFrame)
        {
        focusedNode = focusedFrame->document()->focusedNode();

        if (focusedNode) { // based on focused element
            m_focusedElementType = nodeTypeB(focusedNode, focusedFrame);
        }
        else {
            m_focusedElementType = TBrCtlDefs::EElementNone;
        }
    }
    else {
        m_focusedElementType = TBrCtlDefs::EElementNone;
    }
    m_brctl->updateDefaultSoftkeys();
}

void WebView::updatePageScaler()
{
    if (m_brctl->capabilities() & TBrCtlDefs::ECapabilityGraphicalPage) {
        TRAP_IGNORE(if (m_pageScaler) m_pageScaler->DocumentChangedL());
    }
}

void WebView::openUrl(const TDesC& url)
{
    mainFrame()->loadRequest(WebCore::ResourceRequest(url, true));
}

WebCore::DOMDocument* WebView::mainFrameDocument()
{
    return mainFrame()->DOMDocument();
}

TBool WebView::shouldClose()
{
    Frame* coreFrame = core(mainFrame());
    if (!coreFrame)
        return ETrue;
    return coreFrame->shouldClose();
}

TBool WebView::isLoading()
{
    return m_page->mainFrame()->loader()->activeDocumentLoader()->isLoading();
}

TInt WebView::CountComponentControls() const
{
    int count = (m_webFormFillPopup && m_webFormFillPopup->IsVisible()) ? 1 : 0;
    return count;
}

CCoeControl* WebView::ComponentControl(TInt aIndex) const
{
    CCoeControl* rv = NULL;

    if ( aIndex == 0)
        if ( m_webFormFillPopup && m_webFormFillPopup->IsVisible()) rv = m_webFormFillPopup;

    return rv;
}

TKeyResponse WebView::OfferKeyEventL(const TKeyEvent& keyevent, TEventCode eventcode )
{
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    //hijack
    if (isSynchRequestPending()) {
        return EKeyWasNotConsumed;
    }
    if (m_toolbar)
        return m_toolbar->HandleOfferKeyEventL(keyevent, eventcode);

    if (m_popupDrawer)
        return m_popupDrawer->handleOfferKeyEventL(keyevent, eventcode );

    if ( m_webFormFillPopup && m_webFormFillPopup->IsVisible() && AknLayoutUtils::PenEnabled() ) {
	    if (EKeyWasConsumed == m_webFormFillPopup->HandleKeyEventL(keyevent, eventcode)) {
            return EKeyWasConsumed;
	        }
    }

    bool consumed = false;
    Frame* coreFrame = core(mainFrame());
    if (!coreFrame)
        return EKeyWasNotConsumed;

    coreFrame = page()->focusController()->focusedOrMainFrame();

    // edit events
    if (m_isEditable) {
        consumed = page()->focusController()->focusedOrMainFrame()->eventHandler()->keyEvent(PlatformKeyboardEvent(keyevent,eventcode));
        // exit input on up/down key
        // EXCEPT on touch-enabled devices. We'll just consume in that case
            if ( !consumed
                 && (    keyevent.iCode == EKeyUpArrow         // North
                      || keyevent.iCode == EKeyRightUpArrow    // Northeast
                      || keyevent.iCode == EStdKeyDevice11     //   : Extra KeyEvent supports diagonal event simulator wedge
                      || keyevent.iCode == EKeyRightArrow      // East
                      || keyevent.iCode == EKeyRightDownArrow  // Southeast
                      || keyevent.iCode == EStdKeyDevice12     //   : Extra KeyEvent supports diagonal event simulator wedge
                      || keyevent.iCode == EKeyDownArrow       // South
                      || keyevent.iCode == EKeyLeftDownArrow   // Southwest
                      || keyevent.iCode == EStdKeyDevice13     //   : Extra KeyEvent supports diagonal event simulator wedge
                      || keyevent.iCode == EKeyLeftArrow       // West
                      || keyevent.iCode == EKeyLeftUpArrow     // Northwest
                      || keyevent.iCode == EStdKeyDevice10 ) ) //   : Extra KeyEvent supports diagonal event simulator wedge
                {
                if (m_webfeptexteditor->validateTextFormat() ) {
                    setFocusNone();
                } else {
                    consumed = true;
                }
            }
    }

    // scroll events
    if (!consumed) {
        switch( eventcode ) {
        case EEventKeyDown:
            m_currentEventKey = keyevent;
            //page()->mainFrame()->setFocusedNodeIfNeeded();
            //coreFrame->eventHandler()->mousePressNode()->focus();

            break;
        case EEventKey:
            if (keyevent.iScanCode != m_currentEventKey.iScanCode ) return EKeyWasNotConsumed;
            //
            m_currentEventKey = keyevent;
            if (keyevent.iCode == EKeyDevice3) {
                // pass it to webcore
                TPointerEvent event;
                event.iPosition = StaticObjectsContainer::instance()->webCursor()->position();
                event.iModifiers = 0;
                event.iType = TPointerEvent::EButton1Down ;
                coreFrame->eventHandler()->handleMousePressEvent(PlatformMouseEvent(event));

                // mimic ccb's behavior of onFocus
                Node* node = page()->focusController()->focusedOrMainFrame()->eventHandler()->mousePressNode();
                for (Node* n = node; n; n = n->parentNode()) {
                    if ( n->isFocusable() ) {
                        page()->focusController()->setFocusedNode(n, page()->focusController()->focusedOrMainFrame());
                    }
                }

                // Toolbar is activated on long key press only if the element
                // type is EElementNone during EEventKeyDown and EEventKey.
                // This prevents toolbar from popping up in DHTML pages. Also,
                // toolbar is activated when the user is not in fast scroll
                // mode, or in page overview mode, or on wml page.
                if ( ( m_focusedElementType == TBrCtlDefs::EElementNone ||
                       m_focusedElementType == TBrCtlDefs::EElementBrokenImage ) &&
                       keyevent.iRepeats && !m_brctl->wmlMode() )
                    {
                    launchToolBarL();
                    }
                consumed = true;
            } else if (    keyevent.iCode == EKeyUpArrow              // North
                        || keyevent.iCode == EKeyRightUpArrow         // Northeast
                        || keyevent.iCode == EStdKeyDevice11          //   : Extra KeyEvent supports diagonal event simulator wedge
                        || keyevent.iCode == EKeyRightArrow           // East
                        || keyevent.iCode == EKeyRightDownArrow       // Southeast
                        || keyevent.iCode == EStdKeyDevice12          //   : Extra KeyEvent supports diagonal event simulator wedge
                        || keyevent.iCode == EKeyDownArrow            // South
                        || keyevent.iCode == EKeyLeftDownArrow        // Southwest
                        || keyevent.iCode == EStdKeyDevice13          //   : Extra KeyEvent supports diagonal event simulator wedge
                        || keyevent.iCode == EKeyLeftArrow            // West
                        || keyevent.iCode == EKeyLeftUpArrow          // Northwest
                        || keyevent.iCode == EStdKeyDevice10 )        //   : Extra KeyEvent supports diagonal event simulator wedge
            {
                if (m_brctl->settings()->getTabbedNavigation()) {
                    int horizontal = 0;
                    int vertical = 0;
                    switch(keyevent.iCode)
                    {
                        case EKeyUpArrow:             // North
                            vertical = -1;
                            break;

                        case EKeyRightUpArrow:        // Northeast
                        case EStdKeyDevice11:         //   : Extra KeyEvent supports diagonal event simulator wedge
                            vertical   = -1;
                            horizontal = +1;
                            break;

                        case EKeyRightArrow:          // East
                            horizontal = +1;
                            break;

                        case EKeyRightDownArrow:      // Southeast
                        case EStdKeyDevice12:         //   : Extra KeyEvent supports diagonal event simulator wedge
                            vertical   = +1;
                            horizontal = +1;
                            break;

                        case EKeyDownArrow:           // South
                            vertical   = +1;
                            break;

                        case EKeyLeftDownArrow:       // Southwest
                        case EStdKeyDevice13:         //   : Extra KeyEvent supports diagonal event simulator wedge
                            vertical   = +1;
                            horizontal = -1;
                            break;

                        case EKeyLeftArrow:           // West
                            horizontal = -1;
                            break;

                        case EKeyLeftUpArrow:         // Northwest
                        case EStdKeyDevice10:         //   : Extra KeyEvent supports diagonal event simulator wedge
                            vertical   = -1;
                            horizontal = -1;
                            break;

                        default:                      // (Should never get here)
                            break;

                    }
                    
                   	consumed = m_tabbedNavigation->navigate(horizontal, vertical);
                }
                else {
                    // start fast scrolling
					m_showCursor = true;
                    if (!cursor->isVisible()) {
                        cursor->cursorUpdate(true);
                    }
                    bool fastscroll(m_fastScrollTimer->IsActive());
                    m_savedPosition = mainFrame()->frameView()->contentPos();
                    cursor->scrollAndMoveCursor(keyevent.iCode, m_scrollingSpeed, fastscroll);
                    if (!fastscroll) {
                        m_fastScrollTimer->Start(KCursorInitialDelay,KCursorUpdateFrquency,TCallBack(&scrollTimerCb,this));
                        m_scrollingStartTime.HomeTime();
                    }
                    // and minimap comes on
                    int scrollingTime = millisecondsScrolled();
                    if (!AknLayoutUtils::PenEnabled() && m_pageScalerEnabled && m_pageScaler && !isSmallPage() &&
                        m_brctl->settings()->brctlSetting(TBrCtlDefs::ESettingsPageOverview) &&
                        (scrollingTime > KPanningPageScalerStart || m_pageScaler->Visible())) {
                        m_pageScaler->SetVisibleUntil(KScalerVisibilityTime);
					}
                    //
                    if (!fastscroll) {
                      TPointerEvent event;
                      event.iPosition = cursor->position();
                      event.iModifiers = 0;
                      event.iType = TPointerEvent::EMove;
                      coreFrame->eventHandler()->handleMouseMoveEvent(PlatformMouseEvent(event));
                    }
                    consumed = true;
                } // if (m_brctl->settings()->getTabbedNavigation()
            } else {
                // activate hovered input element by just start typing
                if (m_focusedElementType == TBrCtlDefs::EElementInputBox || m_focusedElementType == TBrCtlDefs::EElementSelectBox
                    || m_focusedElementType == TBrCtlDefs::EElementSelectMultiBox || m_focusedElementType == TBrCtlDefs::EElementTextAreaBox) {
                    TPointerEvent event;
                    event.iModifiers = 0;
                    event.iPosition = cursor->position();
                    event.iType = TPointerEvent::EButton1Down ;
                    coreFrame->eventHandler()->handleMousePressEvent(PlatformMouseEvent(event));
                    event.iType = TPointerEvent::EButton1Up;
                    coreFrame->eventHandler()->handleMouseReleaseEvent(PlatformMouseEvent(event));


                    if (m_focusedElementType == TBrCtlDefs::EElementInputBox || m_focusedElementType == TBrCtlDefs::EElementTextAreaBox) {
                        if (!m_fepTimer)
                            m_fepTimer = new WebCore::Timer<WebView>(this, &WebView::fepTimerFired);

                        m_fepTimer->startOneShot(0.2f);
                        setEditable(true);
                    }
                    m_keyevent = keyevent;
                    m_eventcode = eventcode;

                    consumed = true;
            }
            }
            break;
        case EEventKeyUp:
            m_fastScrollTimer->Cancel();
            m_scrollingSpeed = KNormalScrollRange*100/scalingFactor();

            if (viewIsFastScrolling()) {
	      setViewIsFastScrolling(false);
              toggleRepaintTimer(true);
              if (AknLayoutUtils::PenEnabled() && !inPageViewMode()) {
                  m_pageScrollHandler->scrollbarDrawer()->fadeScrollbar();
              }
            }
            m_currentEventKey = KNullKeyEvent;
            if ( (keyevent.iScanCode == EStdKeyDevice3) || (keyevent.iScanCode == EStdKeyEnter) ){
                // pass it to webcore
                TPointerEvent event;
                if (m_focusedElementType == TBrCtlDefs::EElementInputBox || 
                        m_focusedElementType == TBrCtlDefs::EElementTextAreaBox)
                    {
                    setEditable(true);
                    }
                event.iPosition = cursor->position();
                event.iType = TPointerEvent::EButton1Up;
                coreFrame->eventHandler()->handleMouseReleaseEvent(PlatformMouseEvent(event));
                consumed = true;
            }
            break;
        }
    }

    // keyevents
    if (!consumed) {
        consumed = coreFrame->eventHandler()->keyEvent(PlatformKeyboardEvent(keyevent,eventcode));
        if (!consumed && eventcode == EEventKey && (m_brctl->capabilities() & TBrCtlDefs::ECapabilityAccessKeys)) {
            TKeyEvent ke = keyevent;
            TChar c(ke.iCode);
            // Not consumed by WebCore, is alphanumeric and does not have any modifier
            if (c.IsAlphaDigit() && !(ke.iModifiers & (EModifierCtrl | EModifierAlt | EModifierShift))) {
                ke.iModifiers = EModifierCtrl;
                coreFrame->eventHandler()->keyEvent(PlatformKeyboardEvent(ke,EEventKeyDown));
                consumed = true;
            }
        }
    }

    //setEditable();

    return (consumed)?EKeyWasConsumed:EKeyWasNotConsumed;
}


void WebView::updateScrollbars(int documentHeight, int displayPosY, int documentWidth, int displayPosX)
{
    m_brctl->updateScrollbars((documentHeight * scalingFactor()) / 100, Rect().Height(),
        (displayPosY * scalingFactor()) / 100, (documentWidth * scalingFactor()) / 100, Rect().Width(),
        (displayPosX * scalingFactor()) / 100);
}

void WebView::openPageViewL()
{
    // don't show pageview if we are in lite mode
    if ( m_brctl->capabilities() & TBrCtlDefs::ECapabilityWebKitLite )
        return;
    if (!m_pageView) {
        m_pageView = CPageView::NewL(*this);
        MakeVisible(EFalse);
        m_savedPosition = mainFrame()->frameView()->contentPos();
        m_brctl->reportStateChanged(TBrCtlDefs::EStateThumbnailView, ETrue);
    }
}

void WebView::closePageView()
{
    if ( m_brctl->capabilities() & TBrCtlDefs::ECapabilityWebKitLite )
        return;
    if (m_pageView) {
        delete m_pageView;
        m_pageView = 0;
        MakeVisible(ETrue);
        m_brctl->DrawNow();
        m_brctl->reportStateChanged(TBrCtlDefs::EStateThumbnailView, EFalse);
    }
}

void WebView::cancelPageView()
{
    if ( m_brctl->capabilities() & TBrCtlDefs::ECapabilityWebKitLite )
        return;
    if (m_pageView) {
        mainFrame()->frameView()->scrollTo(m_savedPosition);
        closePageView();
    }
}

//-------------------------------------------------------------------------------
// launchToolBarL
//-------------------------------------------------------------------------------
void WebView::launchToolBarL()
{
    if ( AknLayoutUtils::PenEnabled() || m_brctl->wmlMode() ) {
        return;
    }

    if ( !(m_brctl->capabilities() & TBrCtlDefs::ECapabilityToolBar) ) {
        return;
    }

    if ( !m_brctl->settings()->brctlSetting(TBrCtlDefs::ESettingsToolbarOnOff) ) {
        return;
    }

    delete m_toolbar;
    m_toolbar = 0;

    delete m_toolbarinterface;
    m_toolbarinterface = 0;

    m_toolbarinterface = new WebToolBarInterface(this);
    TRAPD(leave, m_toolbar = CToolBar::NewL(*m_toolbarinterface));
    if (leave != KErrNone)
        return;

    /*EnableDragEvents();*/

    m_brctl->DrawNow();
    m_brctl->reportStateChanged(TBrCtlDefs::EStateToolBarMode, ETrue);

}

//-------------------------------------------------------------------------------
// closeToolBarL
//-------------------------------------------------------------------------------
void WebView::closeToolBarL()
{
    delete m_toolbar;
    m_toolbar = 0;

    delete m_toolbarinterface;
    m_toolbarinterface = 0;

    if (IsVisible())
    {
        StaticObjectsContainer::instance()->webCursor()->cursorUpdate(ETrue);
        m_brctl->DrawNow();
    }

    m_brctl->reportStateChanged(TBrCtlDefs::EStateToolBarMode, EFalse);
}

//-------------------------------------------------------------------------------
// DrawDocumentPart ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
void WebView::DrawDocumentPart(
                               CFbsBitGc& aGc,
                               CFbsBitmap* aBitmap,
                               const TRect& aDocumentAreaToDraw )
{
    TRect r( mainFrame()->frameView()->toDocCoords(aDocumentAreaToDraw) ) ;
    WebCoreGraphicsContext* wcgc = 0;
    TRAPD( err, wcgc = WebCoreGraphicsContext::NewL( aGc, aBitmap, mainFrame()->frameView()));
    if ( err == KErrNone )
    {
        // ensure all documents have valid layout
        // tot:fixme
        //mainFrame->LayoutRecursive();

        TWebCoreSavedContext saved( wcgc->save() );

        wcgc->setOrigin( -aDocumentAreaToDraw.iTl );
        wcgc->setClippingRect( aDocumentAreaToDraw );

        // draw using bridge directly as frame::draw clips drawing to view size
        mainFrame()->paintRect( *wcgc, r );
        wcgc->restore(saved);
        delete wcgc;
    }
}

//-------------------------------------------------------------------------------
// DocumentViewport ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
TRect WebView::DocumentViewport()
{
    return mainFrame()->frameView()->toViewCoords(mainFrame()->frameView()->visibleRect());
}

//-------------------------------------------------------------------------------
// ScaledPageChanged ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
void WebView::ScaledPageChanged(
                                const TRect& aRect,
                                TBool aFullScreen,
                                TBool aScroll )
{
    if (!aScroll) {
        Window().Invalidate( aRect );
    }

    if ( !aScroll && aFullScreen )
    {
        // update the history with new bitmap
        CFbsBitmap* scaledPage = m_pageScaler->ScaledPage();
        if (scaledPage) {
            // Get the browser control rect
            TRAP_IGNORE( m_brctl->historyHandler()->historyController()->updateHistoryEntryThumbnailL(scaledPage));

            m_brctl->HandleBrowserLoadEventL( TBrCtlDefs::EEventThumbnailAvailable, 0, 0 );
        }
        // ignore err since we will use the default image
    }
}

//-------------------------------------------------------------------------------
// DocumentSize ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
TSize WebView::DocumentSize()
{
    return mainFrame()->frameView()->toViewCoords(mainFrame()->frameView()->contentSize());
}

//-------------------------------------------------------------------------------
// TouchScrolling ( from MPageScalerCallback )
//
//
//-------------------------------------------------------------------------------
TBool WebView::TouchScrolling()
{
    return m_pageScrollHandler->touchScrolling();
}

CCoeControl& WebView::PageControlView()
{
    return *(m_brctl->CCoeControlParent());
}

void WebView::setEditable(TBool editable)
{
    Frame* frame = core(mainFrame());
    if (!frame || m_isEditable == editable)
        return;

    m_isEditable = editable;

    Node* focusedNode = NULL;
    Frame* focusedFrame = page()->focusController()->focusedFrame();

    if (editable && focusedFrame) {
        m_webfeptexteditor->UpdateEditingMode();
        focusedNode = focusedFrame->document()->focusedNode();
    }
    else {
        m_webfeptexteditor->CancelEditingMode();
    }

    WebCursor* c = StaticObjectsContainer::instance()->webCursor();
    //
    if (focusedNode)
        // based on focused element
        m_focusedElementType = nodeTypeB(focusedNode, focusedFrame);
    else {
        m_focusedElementType = TBrCtlDefs::EElementNone;

        // based on cursor pos
        c->updatePositionAndElemType(c->position());
    }
    c->cursorUpdate(!m_isEditable);
    m_brctl->updateDefaultSoftkeys();
    iCoeEnv->InputCapabilitiesChanged();
}

TCoeInputCapabilities WebView::InputCapabilities() const
{
    return m_webfeptexteditor->InputCapabilities();
}

void WebView::FocusChanged(TDrawNow aDrawNow)
{
if (m_isPluginsVisible && !m_pluginActivated) {
        Frame* coreFrame =core(mainFrame());
        MWebCoreObjectWidget* view = NULL;
        TBool focus(IsFocused());
        for (Frame* frame = coreFrame; frame; frame = frame->tree()->traverseNext(coreFrame)) {
            PassRefPtr<HTMLCollection> objects = frame->document()->objects();
            for (Node* n = objects->firstItem(); n; n = objects->nextItem()) {
                view = widget(n);
                if (view) {
                    static_cast<PluginSkin*>(view)->viewFocusChanged(focus);
                }
            }

            PassRefPtr<HTMLCollection> embeds = frame->document()->embeds();
            for (Node* n = embeds->firstItem(); n; n = embeds->nextItem()) {
                view = widget(n);
                if (view) {
                    static_cast<PluginSkin*>(view)->viewFocusChanged(focus);
                }
            }
        }
    }
    if (m_pageFullScreenHandler && m_pageFullScreenHandler->isFullScreenMode()) {
        if (IsFocused()) m_pageFullScreenHandler->showEscBtnL();
        else m_pageFullScreenHandler->hideEscBtnL();
    }
}

void WebView::SizeChanged()
{
    adjustOffscreenRect();
    //
    if (mainFrame()) {
        mainFrame()->frameView()->setRect(mainFrame()->frameView()->toDocCoords(Rect()));
    }
    if ( m_pageScaler )
        m_pageScaler->SetContainerRect(TRect(Rect().Size()));
    if (m_webFormFillPopup && m_webFormFillPopup->IsVisible()) {
        m_webFormFill->updatePopupView();
    }
    if (m_pageFullScreenHandler) {
        m_pageFullScreenHandler->SizeChanged();
    }
    if (m_viewIsScrolling) {
        m_pageScrollHandler->scrollbarDrawer()->redrawScrollbar();
    }
}

TSize WebView::maxBidiSize() const
{
    return TSize((Rect().Size().iWidth-20),(Rect().Size().iHeight-20));
}

void WebView::clearOffScreenBitmap()
{
    m_webcorecontext->gc().Reset();
    m_webcorecontext->gc().Clear();
}

void WebView::scrollBuffer(TPoint to, TPoint from, TBool usecopyscroll)
{
    TRect rect(m_offscreenrect);

    TSize bufSize( rect.Size() );

    CFbsBitGc& gc = m_webcorecontext->gc();
    gc.Reset();

    TRect paintArea;

    int dx = from.iX - to.iX;
    int dy = from.iY - to.iY;

    if (usecopyscroll && abs(dx) < bufSize.iWidth && abs(dy) < bufSize.iHeight) {
        TPoint fromPt(dx<0?-dx:0, dy<0?-dy:0);
        TRect fromRt(fromPt, TSize(bufSize.iWidth-abs(dx), bufSize.iHeight-abs(dy)));
        TRect toRt(TPoint(fromPt.iX+dx, fromPt.iY+dy), fromRt.Size());

        gc.CopyRect(TPoint(dx, dy), fromRt);

        RRegion region;
        region.AddRect(TRect(TPoint(0, 0), bufSize));
        region.AddRect(toRt);
        region.SubRect(toRt, 0);

        for (int i=0; i<region.Count(); ++i) {
            TRect r = region[i];
            r.Move(to);
            r = mainFrame()->frameView()->toDocCoords(r);
            if (mainFrame()->frameView()->isScaled())
                r.Grow(1, 1);
            m_repaints.AddRect(r);
        }
    }
    else {
        TRect r(to, bufSize);
        r = mainFrame()->frameView()->toDocCoords(r);
        if (mainFrame()->frameView()->isScaled())
            r.Grow(1, 1);
        m_repaints.AddRect(r);
    }

}

int WebView::scalingFactor() const
{
    return m_currentZoomLevel;
}

CPluginHandler* WebView::pluginForExtension(const WebCore::String& pluginExtension)
{
    return NULL;
}

bool WebView::isMIMETypeRegisteredAsPlugin(const WebCore::String& MIMEType)
{
    return false;
}

void WebView::adjustOffscreenRect()
{
    TRect rect(Rect());

    if ( m_offscreenrect != rect ) {
        m_offscreenrect = rect;
        TSize bmSize = StaticObjectsContainer::instance()->webSurface()->offscreenBitmap()->SizeInPixels();
        if (bmSize.iWidth != rect.Width() || bmSize.iHeight != rect.Height()) {
            m_bitmapdevice->Resize(rect.Size());
            m_webcorecontext->resized();
        }
    }
}

void WebView::autoScroll()
{
    // update scrolling speed
    int milli = millisecondsScrolled();
    setViewIsFastScrolling(true);
    toggleRepaintTimer(false);
    m_scrollingSpeed = milli>=KPanningMaxTime ? KPanningMaxSpeed : KPanningStartSpeed+(KPanningMaxSpeed-KPanningStartSpeed)*Max(milli-KPanningStartTime,0)/KPanningMaxTime;
    m_scrollingSpeed = m_scrollingSpeed*100/scalingFactor();
    TPoint scrollDelta = mainFrame()->frameView()->contentPos() - m_savedPosition;
    scrollDelta.iX *= 100;
    scrollDelta.iY *= 100;
    if (AknLayoutUtils::PenEnabled() && !inPageViewMode()) {
        m_pageScrollHandler->scrollbarDrawer()->drawScrollbar(this, scrollDelta);
    }
    OfferKeyEventL(m_currentEventKey, EEventKey);
}

int WebView::millisecondsScrolled() const
{
    TTime timeNow;
    timeNow.HomeTime();
    return I64INT(timeNow.MicroSecondsFrom(m_scrollingStartTime).Int64()/1000);
}

TInt WebView::scrollTimerCb(TAny* aAny)
{
    WebView* v = static_cast<WebView*>(aAny);
    v->autoScroll();
    return ETrue;
}

int WebView::searchFor(const TPtrC& keyword)
{
    m_inFindState = true;
    delete m_findKeyword;
    m_findKeyword = NULL;
    m_findKeyword = keyword.Alloc();
    if (!m_findKeyword || keyword.Length() == 0) {
        WebFrame* frame = mainFrame()->findFrameWithSelection();
        if(frame)
            frame->clearSelection();
        return TBrCtlDefs::EFindNoMatches;
    }
    m_findKeyword->Des().LowerCase();
    return search(keyword, true, true);
}

int WebView::searchAgain(bool forward)
{
    if (!m_findKeyword) {
        WebFrame* frame = mainFrame()->findFrameWithSelection();
        if(frame) {
			frame->clearSelection();
        }
        return TBrCtlDefs::EFindNoMatches;
    }
    return search(*m_findKeyword, forward, true);
}

int WebView::search(TPtrC keyword, bool forward, bool wrapFlag)
{
    // Get the frame holding the selection, or start with the main frame
    WebFrame* startFrame = mainFrame()->findFrameWithSelection();
    WebFrame* lastFrame = NULL;
    if (!startFrame)  {
        if (StaticObjectsContainer::instance()->webCursor()) {
            startFrame = StaticObjectsContainer::instance()->webCursor()->getFrameUnderCursor();
        }
        else {
            startFrame = mainFrame();
        }
    }
    WebFrame* frame = startFrame;
    do {
        WebFrame* nextFrame = incrementFrame(frame, forward, wrapFlag);
        bool onlyOneFrame = (frame == nextFrame);
        if (frame == startFrame)
            lastFrame = frame;

        bool foundString = false;
        // In some cases we have to search some content twice; see comment later in this method.
        // We can avoid ever doing this in the common one-frame case by passing YES for wrapFlag
        // here, and then bailing out before we get to the code that would search again in the
        // same content.
        bool wrapOnThisPass = wrapFlag && onlyOneFrame;
        if (!core(frame)->isFrameSet() ) {
            foundString = frame->bridge()->searchFor(keyword, forward, false, wrapOnThisPass, true);
        }
        if (foundString)  {
            if (frame != startFrame)
                startFrame->clearSelection();
            return TBrCtlDefs::EFindMatch;
        }
        if (onlyOneFrame) {
			startFrame->clearSelection();
            return TBrCtlDefs::EFindNoMatches;
        }
        frame = nextFrame;
    } while (frame && frame != startFrame);

    // If there are multiple frames and wrapFlag is true and we've visited each one without finding a result, we still need to search in the
    // first-searched frame up to the selection. However, the API doesn't provide a way to search only up to a particular point. The only
    // way to make sure the entire frame is searched is to pass YES for the wrapFlag. When there are no matches, this will search again
    // some content that we already searched on the first pass. In the worst case, we could search the entire contents of this frame twice.
    // To fix this, we'd need to add a mechanism to specify a range in which to search.
    if (wrapFlag && lastFrame) {
        if (frame->bridge()->searchFor(keyword, forward, false, true, false))
            return TBrCtlDefs::EFindMatch;
    }

	if(frame) {
		frame->clearSelection();
	}		
    return TBrCtlDefs::EFindNoMatches;
}

void WebView::exitFindState()
{
    m_inFindState = false;
    delete m_findKeyword;
    m_findKeyword = NULL;
    WebFrame* selectedFrame = mainFrame()->findFrameWithSelection();
    selectedFrame->clearSelection();
}

bool WebView::isSmallPage()
{
    TSize docSize = DocumentSize();
    TSize viewSize = DocumentViewport().Size();
    return ((docSize.iWidth * docSize.iHeight*100)/(viewSize.iWidth*viewSize.iHeight) < KSmallPageScale);
}

void WebView::willSubmitForm(FormState* formState)
{
    if (m_webFormFill) {
        m_webFormFill->willSubmitForm(formState);
    }
}


//-------------------------------------------------------------------------------
// HandlePointerBufferReadyL
// Handles Pointer Move Events
//-------------------------------------------------------------------------------
void WebView::HandlePointerBufferReadyL()
{
    memset(m_ptrbuffer,0,256*sizeof(TPoint));
    TPtr8 ptr((TUint8 *)m_ptrbuffer,256*sizeof(TPoint));

    TInt numPnts = Window().RetrievePointerMoveBuffer(ptr);

    for (int i = 0; i < numPnts; i++) {
        TPointerEvent pe;
        pe.iType = TPointerEvent::EDrag;
        pe.iPosition = m_ptrbuffer[i];
        m_webpointerEventHandler->HandlePointerEventL(pe);
    }
}

//-------------------------------------------------------------------------------
// HandlePointerEventL
// Handles Pointer Events
//-------------------------------------------------------------------------------
void WebView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    m_webpointerEventHandler->HandlePointerEventL(aPointerEvent);
}

void WebView::notifyMetaData(String& name, String& value)
{
    if (name == "navigation") {
        if (value == "tabbed") {
            m_brctl->settings()->setTabbedNavigation(true);
            StaticObjectsContainer::instance()->webCursor()->cursorUpdate(true);
        }
    }
}

//-------------------------------------------------------------------------------
// HandleShowAnchorHref()
// Display a popup with the url of an anchor
//-------------------------------------------------------------------------------
void WebView::handleShowAnchorHrefL()
{
    HBufC* url = NULL;
    IntPoint p;
    WebFrame* f = frameAndPointUnderCursor(p, *this);
    String urlStr = getNodeUrlAtPointInFrame(*f, p);

    if (urlStr.length() > 0)
        url = urlStr.des().AllocL();
    else
        User::Leave(KErrNotSupported);

    CleanupStack::PushL(url);


    TRect elRect;
    TBrCtlDefs::TBrCtlElementType elType = TBrCtlDefs::EElementNone;
    WebFrame* frm = StaticObjectsContainer::instance()->webCursor()->getFrameUnderCursor();
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    TPoint pt(frm->frameView()->viewCoordsInFrameCoords(cursor->position()));
    StaticObjectsContainer::instance()->webCursor()->navigableNodeUnderCursor(*(frm), pt, elType, elRect);


    TRect rect(elRect);
    rect.SetRect(f->frameView()->frameCoordsInViewCoords(rect.iTl), f->frameView()->frameCoordsInViewCoords(rect.iBr));

    TPtrC urlPtr(url->Des());
    m_popupDrawer = WebPopupDrawer::NewL(*this, urlPtr, rect);
    DrawNow();

    CleanupStack::PopAndDestroy(); // url
}

//-------------------------------------------------------------------------------
// FocusedImageLC()
// Return the image that is under the cursor
//-------------------------------------------------------------------------------
TBrCtlImageCarrier* WebView::focusedImageLC()
{
    if (m_focusedElementType != TBrCtlDefs::EElementImageBox && m_focusedElementType != TBrCtlDefs::EElementAreaBox) {
        User::Leave(KErrNotSupported);
    }
    TBrCtlImageCarrier* imageCarrier = NULL;
    User::LeaveIfError(focusedImage(this, imageCarrier));
    CleanupStack::PushL(imageCarrier);
    return imageCarrier;
}

//-------------------------------------------------------------------------------
// LoadFocusedImageL()
// Load the image that is under the cursor
//-------------------------------------------------------------------------------
void WebView::loadFocusedImageL()
{
    // Ensure cursor is on a broken image and image loading is disabled
    if (m_focusedElementType != TBrCtlDefs::EElementBrokenImage || m_brctl->settings()->brctlSetting(TBrCtlDefs::ESettingsAutoLoadImages)) {
        User::Leave(KErrNotSupported);
    }
    loadFocusedImage(this);
}

//-------------------------------------------------------------------------------
// RemovePopup()
// Delete the popupDrawer
//-------------------------------------------------------------------------------
void WebView::removePopup()
{
    if (m_popupDrawer) {
        delete m_popupDrawer;
        m_popupDrawer = NULL;
        DrawNow();
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
RArray<TUint>* WebView::zoomLevels()
{
    return &m_zoomLevelArray;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::setZoomLevel(int zoomLevel)
{
    m_dirtyZoomMode = false;
    if (zoomLevel < KZoomLevelMinValue ||
        zoomLevel > m_maxZoomLevel ||
        zoomLevel == m_currentZoomLevel)
        return;
    zoomLevelChanged(zoomLevel);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::setBitmapZoomLevel(int zoomLevel)
{
    m_zoomLevelChangedByUser = true;
    WebFrameView* view = mainFrame()->frameView();
    if (!view) return;

    m_dirtyZoomMode = true;
    m_isPluginsVisible = false;
    mainFrame()->makeVisiblePlugins(false);

    if (zoomLevel > m_startZoomLevel) {

        // cut m_srcRectForZooming from m_offscreenrect and enlarge it to fit the view rect

        float newWidth =   m_offscreenrect.Width() *  zoomLevel / m_startZoomLevel;
        float newHeight =  m_offscreenrect.Height() * zoomLevel / m_startZoomLevel;

        newWidth =  (float)(m_offscreenrect.Width() * (float)m_offscreenrect.Width()) / newWidth;
        newHeight = (float)m_offscreenrect.Height() * (float)m_offscreenrect.Height() / newHeight;

        TSize docSize = DocumentSize();
        TSize viewSize = DocumentViewport().Size();
        TSize contentSize = view->contentSize();


        m_srcRectForZooming.iTl.iX = 0;
        m_srcRectForZooming.iTl.iY = 0;

        m_srcRectForZooming.iBr.iX = newWidth;
        m_srcRectForZooming.iBr.iY = newHeight;

        m_destRectForZooming = Rect();

    }
    else {
       // take the whole rect and calculate new rect to fit it the rest of view rect paint gray colour

        TInt newWidth  = m_offscreenrect.Width() * zoomLevel / m_startZoomLevel;
        TInt newHeight = m_offscreenrect.Height() * zoomLevel / m_startZoomLevel;

        m_srcRectForZooming = m_offscreenrect;

        m_destRectForZooming.iTl.iX = 0;
        m_destRectForZooming.iTl.iY = 0;

        m_destRectForZooming.iBr.iX = newWidth;
        m_destRectForZooming.iBr.iY = newHeight;
    }

    m_currentZoomLevel = zoomLevel;

    DrawNow();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::restoreZoomLevel(int zoomLevel)
{
    m_dirtyZoomMode = false;
    clearOffScreenBitmap();
    zoomLevelChanged(zoomLevel);
    mainFrame()->notifyPluginsOfScrolling();
    m_isPluginsVisible = false;
    mainFrame()->makeVisiblePlugins(true);
    m_isPluginsVisible = true;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::resetZoomLevel(void)
{
    if(   m_widgetextension && m_widgetextension->IsWidgetPublising()) {
        return ;
        }
    if (m_historyLoad) {
        int zoomLevel = m_brctl->historyHandler()->historyController()->currentEntryZoomLevel();
        if (!zoomLevel) zoomLevel = KZoomLevelDefaultValue;
        if (m_currentZoomLevel != zoomLevel) {
            m_currentZoomLevel = zoomLevel;
        }
        int minZoomLevel = m_brctl->historyHandler()->historyController()->currentEntryMinZoomLevel();
        if (!minZoomLevel)
            minZoomLevel = (KZoomLevelDefaultValue/ m_pageZoomHandler->stepSize())*m_pageZoomHandler->stepSize();
        else
            minZoomLevel = (minZoomLevel / m_pageZoomHandler->stepSize())*m_pageZoomHandler->stepSize();
        //could happen only if m_minZoomLevel < stepSize
        if (!m_minZoomLevel) m_minZoomLevel = m_pageZoomHandler->stepSize();

        if (m_minZoomLevel != minZoomLevel) {
            m_minZoomLevel = minZoomLevel;
        }
        m_currentZoomLevel = zoomLevel;
    //set to settings
    m_brctl->settings()->setBrctlSetting(TBrCtlDefs::ESettingsCurrentZoomLevelIndex, m_currentZoomLevel);
    }
    else {
    if (m_currentZoomLevel != KZoomLevelDefaultValue)
    //set default current zoom index
    {
          m_currentZoomLevel = KZoomLevelDefaultValue;
      //set to settings
          m_brctl->settings()->setBrctlSetting(TBrCtlDefs::ESettingsCurrentZoomLevelIndex, m_currentZoomLevel);
    }
    }
    clearOffScreenBitmap();
    m_zoomLevelChangedByUser = false;
    mainFrame()->scalingFactorChanged(m_currentZoomLevel);
    mainFrame()->frameView()->setRect(mainFrame()->frameView()->toDocCoords(Rect()));
    m_startZoomLevel = m_currentZoomLevel;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::updateMinZoomLevel(TSize size)
{
    int newMinZoomLevel;
    if (size.iWidth && size.iHeight) {
        TRect screenSize(Rect().Size());

        float minWidth = ((float)screenSize.Width()) * 100 /(float)( size.iWidth);
        float minHeight = ((float)screenSize.Height()) * 100 /(float)( size.iHeight);

        newMinZoomLevel = ( minWidth > minHeight )?minWidth:minHeight;

        if ( newMinZoomLevel < KZoomLevelMinValue ) newMinZoomLevel = KZoomLevelMinValue;
        if ( newMinZoomLevel > KZoomLevelDefaultValue ) newMinZoomLevel = KZoomLevelDefaultValue;
        if ( newMinZoomLevel > m_currentZoomLevel ) newMinZoomLevel = m_currentZoomLevel;
    }
    else {
        newMinZoomLevel = KZoomLevelDefaultValue;
    }

    newMinZoomLevel = (newMinZoomLevel/m_pageZoomHandler->stepSize())*m_pageZoomHandler->stepSize();

  TBool needsUpdateArray = EFalse;
  //Update the new array
  if ( m_minZoomLevel!= newMinZoomLevel)
  {
    needsUpdateArray = ETrue;
  }

     //could happen only if m_minZoomLevel < stepSize
    if (!newMinZoomLevel) newMinZoomLevel = m_pageZoomHandler->stepSize();

    if (m_minZoomLevel != newMinZoomLevel)
    {
       m_minZoomLevel = newMinZoomLevel;
      if (m_pageZoomHandler->isActive()) {
          m_pageZoomHandler->hideZoomSliderL();
          m_pageZoomHandler->showZoomSliderL();
      }
      else {
        UpdateZoomArray(); //for non-touch
      }
  } //endof if (m_minZoomLevel != newMinZoomLevel)
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::UpdateZoomArray()
{
    //only for non-touch device
    if (m_pageZoomHandler->isActive())
    {
    return;
    }

    m_zoomLevelArray.Reset();

    m_minZoomLevel = TInt(m_minZoomLevel/10) * 10;
    m_zoomLevelArray.Append(KZoomLevelDefaultValue);

    //construct the zoom array from the default level
    TInt toAddZoomLevel = KZoomLevelDefaultValue + KZoomLevelDefaultValue*KZoomLevelDelta/100;
    while (toAddZoomLevel <= m_maxZoomLevel)
    {
        //add the zoom level after default one
        m_zoomLevelArray.Append(toAddZoomLevel);
        toAddZoomLevel = toAddZoomLevel + toAddZoomLevel*KZoomLevelDelta/100;
    }

    //now goes the minimum one
    toAddZoomLevel = KZoomLevelDefaultValue - KZoomLevelDefaultValue*KZoomLevelDelta/100;
    while (toAddZoomLevel >= m_minZoomLevel)
    {
        //add the zoom level after default one
        m_zoomLevelArray.Insert(toAddZoomLevel, 0);
        toAddZoomLevel = toAddZoomLevel - toAddZoomLevel*KZoomLevelDelta/100;
    }

}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::zoomLevelChanged(int newZoomLevel)
{
    int currZoomLevel = m_startZoomLevel;
    m_dirtyZoomMode = false;
    // instead of clearOffScreen bitmap, just reset.
    m_webcorecontext->gc().Reset();
    m_currentZoomLevel = newZoomLevel;
    TSize sz( m_offscreenrect.Size() );
    WebFrameView* view = mainFrame()->frameView();
    if (!view) return;

    TPoint pt( view->contentPos() );
        TSize contentSize( view->contentSize() );
        int z = m_currentZoomLevel;
        if ( pt.iX * z > contentSize.iWidth * z - sz.iWidth * 100 )
        pt.iX = ( contentSize.iWidth - sz.iWidth * 100 / z );

        if ( pt.iY * z > contentSize.iHeight * z - sz.iHeight * 100 )
        pt.iY = ( contentSize.iHeight - sz.iHeight * 100 / z );

        if ( pt.iY < 0 ) pt.iY = 0;
        if ( pt.iX < 0 ) pt.iX = 0;


    view->setContentPos( pt );
    mainFrame()->scalingFactorChanged(z);
    view->checkScrollbarVisibility();

    TRect rect = view->rect();

    TInt tlx = (rect.iTl.iX * currZoomLevel) / m_currentZoomLevel;
    TInt tly = (rect.iTl.iY * currZoomLevel) / m_currentZoomLevel;
    TInt brx = (rect.iBr.iX * currZoomLevel) / m_currentZoomLevel;
    TInt bry = (rect.iBr.iY * currZoomLevel) / m_currentZoomLevel;

    // rounding

    if (( rect.iTl.iX * currZoomLevel) % m_currentZoomLevel ){
        tlx -= 1;
    }
    if (( rect.iTl.iY * currZoomLevel) % m_currentZoomLevel ){
        tly -= 1;
    }
    if ((rect.iBr.iX * currZoomLevel) % m_currentZoomLevel ){
        brx += 1;
    }
    if ((rect.iBr.iY * currZoomLevel) % m_currentZoomLevel ){
        bry += 1;
    }

    view->setRect(TRect(tlx, tly, brx, bry));

    // now just do a repaint, should be very fast
    if ( currZoomLevel > newZoomLevel ) {
        clearOffScreenBitmap();
    }


    m_startZoomLevel = m_currentZoomLevel;

    updateScrollbars(mainFrame()->frameView()->contentSize().iHeight, mainFrame()->frameView()->contentPos().iY,
        mainFrame()->frameView()->contentSize().iWidth, mainFrame()->frameView()->contentPos().iX);

    syncRepaint(view->visibleRect());

    int zoomLevel = m_brctl->historyHandler()->historyController()->currentEntryZoomLevel();
    if (zoomLevel != m_currentZoomLevel) m_brctl->historyHandler()->historyController()->updateCurrentEntryZoomLevelIfNeeded();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::resetLastZoomLevelIfNeeded()
{
    if (m_lastZoomLevel == m_maxZoomLevel)
        m_lastZoomLevel = 100;
}

//-----------------------------------------------------------------------------
// setZoomLevelAdaptively()
//-----------------------------------------------------------------------------
void WebView::setZoomLevelAdaptively()
{
    int zoomLevel = KZoomLevelDefaultValue;

  // Double Tap Zooming: it toggles between default, maxiZoomLevel.
  // Depending on the current zoom level:
  //   A. If the current is already the max, it zooms to the max 
  //   B. If the current is bigger than/equal to the default zoom level zooms to the default, it zooms to the max
  //   C. Otherwise it zooms to the default level first. 
  // For the mobile pages, such as google.com and cnn.com, minimum zoom level equals
  // to the default zoom level. Zooming is only possible between default and maximum
  // zoom level, double tap only won't reach logic C
  //
  // For both mobile and non-mobile pages, it creates the same end user double tap 
  // experiences 
  
    if (m_currentZoomLevel == m_maxZoomLevel ) {
        zoomLevel = KZoomLevelDefaultValue;    	
    }
    else if (m_currentZoomLevel >= KZoomLevelDefaultValue ) {
        zoomLevel = m_maxZoomLevel;
    }	
    else {
        zoomLevel = KZoomLevelDefaultValue;    	
    } 

    // move the content
    WebFrameView* view = mainFrame()->frameView();
    if (!view) return;
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    if (cursor) {
        TPoint pt = cursor->position();
        int z = zoomLevel * 100 / m_currentZoomLevel;
        int xOff = (pt.iX * z / 100 - pt.iX)*100/zoomLevel;
        int yOff = (pt.iY * z / 100 - pt.iY)*100/zoomLevel;
        TPoint cp = view->contentPos();
        cp += TPoint(xOff, yOff);
        view->setContentPos(cp);
    }

    // zoom it
    m_lastZoomLevel = m_currentZoomLevel;

    setZoomLevel(zoomLevel);
    mainFrame()->notifyPluginsOfScrolling();
}

//-------------------------------------------------------------------------------
// Called when user clicks a plugin which is able to accept user input,
// this feature is only used in US build
//-------------------------------------------------------------------------------
void WebView::openPluginPlayer(PluginWin* plugin)
{
    if (!m_pluginplayer) {
        PluginSkin* pluginskin = mainFrame()->focusedPlugin();

        if ( !pluginskin || !pluginskin->isInteractive() ) return;

        // we only create the player when the plugin content is ready and
        // the plugin is not allowed to interact inside html page.
        if ( pluginskin->CanInteract() ) return;

        m_pluginplayer = PluginPlayer::NewL( *m_brctl, plugin );
        MakeVisible(EFalse);
        m_brctl->reportStateChanged(TBrCtlDefs::EStatePluginPlayer, ETrue);
        // now start playing the content
        m_pluginplayer->start();
    }
}

void WebView::setFocusNone()
{
    page()->focusController()->setFocusedNode(NULL, page()->focusController()->focusedOrMainFrame());
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::closePluginPlayer()
{
    // stop playing
    m_pluginplayer->stop();
    MakeVisible( ETrue );

    delete m_pluginplayer;
    m_pluginplayer = 0;
    m_brctl->reportStateChanged(TBrCtlDefs::EStatePluginPlayer, EFalse);
    // redraw the control
    m_brctl->DrawNow();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
CWidgetExtension* WebView::createWidgetExtension(MWidgetCallback &aWidgetCallback)
{
    if (!m_widgetextension) {
        m_widgetextension = CWidgetExtension::NewL(*this, aWidgetCallback);
#if USE(LOW_BANDWIDTH_DISPLAY)
        m_page->mainFrame()->loader()->setUseLowBandwidthDisplay(false);
#endif
    }

    return m_widgetextension;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::forceLayoutAndResize(WebFrame* frame)
{
    if (frame) {
        Frame* f = core(frame);
        FrameView* v = f->view();
        Document* doc = f->document();
        if (v && doc) {
            Node* node = doc;
            while (node) {
                if (node->renderer()) {
                    node->renderer()->setNeedsLayoutAndPrefWidthsRecalc();
                    RenderStyle* style = node->renderer()->style();
                    if (style) {
                        RenderStyle* newstyle = new (node->renderer()->renderArena()) RenderStyle(*style);
                        node->renderer()->setStyle(newstyle);
                    }
                }
                node = node->traverseNextNode();
            }
            f->document()->recalcStyle(Node::Force);
            v->setNeedsLayout();
            v->layout();
        }
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::showZoomSliderL()
{
    if ( !inPageViewMode()) {
        m_pageZoomHandler->showZoomSliderL();
        if ( m_repainttimer->IsActive())
        {
            m_repainttimer->Cancel();
        }
    }
    m_startZoomLevel = m_currentZoomLevel;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::hideZoomSliderL()
{
    if ( !inPageViewMode()) {
        m_pageZoomHandler->hideZoomSliderL();
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::notifyZoomSliderModeChangeL( bool mode )
{
    m_dirtyZoomMode = false;
    clearOffScreenBitmap();
    zoomLevelChanged( m_currentZoomLevel );
    m_brctl->reportStateChanged(TBrCtlDefs::EStateZoomSliderMode, mode );
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
int WebView::maxZoomLevel()
{
    return m_maxZoomLevel;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
int WebView::minZoomLevel()
{
    return m_minZoomLevel;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::updateZoomLevel( TBrCtlDefs::TBrCtlSettings setting, unsigned int value)
{
   switch( setting ) {
       case TBrCtlDefs::ESettingsZoomLevelMax:
       {
        if (value != m_maxZoomLevel)
          {
      //maxzoomlevel is different, needs to reset the zoom
          m_maxZoomLevel = value;
          UpdateZoomArray();
          }
       }

            break;
       case TBrCtlDefs::ESettingsZoomLevelDefault:
            m_scrollingSpeed = KNormalScrollRange*100/scalingFactor();
            break;

        default:
            break;   // should not occur
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::EnterFullscreenBrowsingL()
{
    if (m_pageFullScreenHandler) {
         m_pageFullScreenHandler->showEscBtnL();
         StaticObjectsContainer::instance()->setFullScreenMode(true);
     }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::LeaveFullscreenBrowsingL()
{
    if (m_pageFullScreenHandler) {
        m_pageFullScreenHandler->hideEscBtnL();
        StaticObjectsContainer::instance()->setFullScreenMode(false);
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::notifyFullscreenModeChangeL(bool mode)
{
    if (m_pageFullScreenHandler )
         m_brctl->reportStateChanged(TBrCtlDefs::EStateFullscreenBrowsing, mode );
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void WebView::checkForZoomChange()
{
     zoomLevelChanged(m_currentZoomLevel);
}


void WebView::activateVirtualKeyboard()
{
    if (isEditable() && iCoeEnv->Fep()) {
        fepTextEditor()->CancelEditingMode();
        fepTextEditor()->UpdateEditingMode();
        iCoeEnv->Fep()->HandleChangeInFocus();
        fepTextEditor()->ActivatePenInputRequest();
    }
}

void WebView::Stop()
{
    mainFrame()->stopLoading();
}
void WebView::synchRequestPending(bool flag)
{
    m_synchRequestPending = flag;

    // TODO - nl - Disabled for now - there's no cursor in S60 5.0 release
    //WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();

    if (flag) {
        m_brctl->reportStateChanged(TBrCtlDefs::EStateSynchRequestMode, ETrue);
        //cursor->setWaitCursor(ETrue);
        if (AknLayoutUtils::PenEnabled()) {
            // Disable (block) pointer events
            DrawableWindow()->SetPointerGrab(EFalse);
        }
    }
    else {
        if (AknLayoutUtils::PenEnabled()) {
            // Reenable pointer events
            DrawableWindow()->SetPointerGrab(ETrue);
        }
        //cursor->setWaitCursor(EFalse);
        m_brctl->reportStateChanged(TBrCtlDefs::EStateSynchRequestMode, EFalse);
    }
}

void WebView::setHistoryLoad(bool value) {
     m_historyLoad = value;
}
void WebView::setRedirectWithLockedHistory(bool value) {
     m_redirectWithLockedHistory = value;
}
//-------------------------------------------------------------------------------
// Webview notifies plugins to handle play/pause of .swf files when user switches to menu/another application
//-------------------------------------------------------------------------------
void WebView::notifyPlugins(TBool focus)
{
    Frame* coreFrame =core(mainFrame());
    MWebCoreObjectWidget* view = NULL;
    for (Frame* frame = coreFrame; frame; frame = frame->tree()->traverseNext(coreFrame)) {
        PassRefPtr<HTMLCollection> objects = frame->document()->objects();
        for (Node* n = objects->firstItem(); n; n = objects->nextItem()) {
            view = widget(n);
            if (view) {
                static_cast<PluginSkin*>(view)->handlePluginForeground(focus);
            }
        }

        PassRefPtr<HTMLCollection> embeds = frame->document()->embeds();
        for (Node* n = embeds->firstItem(); n; n = embeds->nextItem()) {
            view = widget(n);
            if (view) {
                static_cast<PluginSkin*>(view)->handlePluginForeground(focus);
            }
        }
    }
}


void WebView::setFastScrollingMode(bool fastScrolling)
{
  if (fastScrolling != m_viewIsFastScrolling) {
  setViewIsFastScrolling (fastScrolling);
  m_isPluginsVisible = false;
  mainFrame()->makeVisiblePlugins(!m_viewIsFastScrolling);
  m_isPluginsVisible = !m_viewIsFastScrolling;

  if (!m_viewIsFastScrolling) {
    mainFrame()->notifyPluginsOfScrolling();
  }
  toggleRepaintTimer(!m_viewIsFastScrolling);
}
}

void WebView::toggleRepaintTimer(bool on)
{
  if ( m_repainttimer->IsActive()) {
      m_repainttimer->Cancel();
  }

  if (on) {
      m_repainttimer->Start(0, 0, TCallBack( &doRepaintCb, this ) );
  }
  m_allowRepaints = on;
}


//-----------------------------------------------------------------------------
// setZoomCursorPosition(TBool)
// Set the cursor position while zooming, this is only for non-touch device
//-----------------------------------------------------------------------------
void WebView::setZoomCursorPosition(TBool isZoomIn)
{
    int zoomLevel = m_currentZoomLevel;

    TInt index = FindCurrentZoomIndex(m_currentZoomLevel);
    if (index == -1)
    {
        //Shouldn't happen -- do nothing
        return;
    }

    if (isZoomIn && index <=m_zoomLevelArray.Count()-2)
    {
        index++;
        zoomLevel = m_zoomLevelArray[index];
    }
    else if (!isZoomIn && index > 0 )
    {
        index--;
        zoomLevel = m_zoomLevelArray[index];
    }

    // move the content
    WebFrameView* view = mainFrame()->frameView();
    if (!view) return;
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    if (cursor) {

        TPoint pt = cursor->position();
        int z = zoomLevel * 100 / m_currentZoomLevel;
        int xOff = (pt.iX * z / 100 - pt.iX)*100/zoomLevel;
        int yOff = (pt.iY * z / 100 - pt.iY)*100/zoomLevel;
        TPoint cp = view->contentPos();
        cp += TPoint(xOff, yOff);
        view->setContentPos(cp);
    }
    //setZoomLevel(zoomLevel);
    mainFrame()->notifyPluginsOfScrolling();
}


// ---------------------------------------------------------------------------
// FindCurrentZoomIndex
// ---------------------------------------------------------------------------
TInt WebView::FindCurrentZoomIndex(TInt aCurrentZoomLevel)
{
    TInt aIndex = -1;

    for (aIndex=0; aIndex<m_zoomLevelArray.Count()-1; aIndex++)
    {
        TInt tmpZoomLevel = m_zoomLevelArray[aIndex];
        if (aCurrentZoomLevel == m_zoomLevelArray[aIndex] )
        {
            break;
        }
    }
    return aIndex;
}

TInt WebView::getWidgetId()
{
    return m_widgetextension ? m_widgetextension->GetWidgetId():0;
}

void WebView::setShowCursor(TBool showCursor) 
{
    m_showCursor = showCursor;
    StaticObjectsContainer::instance()->webCursor()->setCursorVisible(showCursor);
}

// END OF FILE