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
* Description:  Acts as an adapter between plugin and browser.
*
*/

// INCLUDE FILES
#include "../../bidi.h"

#include <e32std.h>
#include <e32def.h>
#include <eikdef.h>
#include <eikenv.h>
#include <eikappui.h>
#include <SysUtil.h>
#include <AknUtils.h>

#include "WebFrame.h"
#include "HttpDefs.h"
#include "BrCtlDefs.h"
#include "StaticObjectsContainer.h"
#include <PluginAdapterInterface.h>
#include "PluginWin.h"
#include "PluginSkin.h"
#include "PluginHandler.h"
#include "PluginStream.h"
#include "WebView.h"
#include "WebFrameView.h"
#include "WebSurface.h"
#include "BrCtl.h"
#include "Frame.h"
#include "WebCursor.h"
#include "WebPageScrollHandler.h"

using namespace WebCore;
// CONSTANTS
const TInt KPluginGranularity = 3;
_LIT(KPath,"c:\\system\\temp\\");

void panicPlugin(TInt error = KErrNone)
{
    User::Panic(_L("Plugin Handling Panic"), error);
}

// -----------------------------------------------------------------------------
// PluginWin::PluginWin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
PluginWin::PluginWin(PluginSkin* pluginskin)
    : m_pluginskin(pluginskin)
    , m_windowedPlugin(true), m_fullscreen(false), m_windowCreated(false)
{
}

// -----------------------------------------------------------------------------
// PluginWin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void PluginWin::ConstructL( const WebView& view )
{
    if (m_windowedPlugin) {
        CreateWindowL((CCoeControl*)&view);
        ActivateL();
		m_windowCreated = true;
    } else {
        SetContainerWindowL(*view.brCtl()->CCoeControlParent());
        m_bitmap = new (ELeave) CFbsBitmap();
        m_bitmap->Create(TSize(0,0),WebCore::StaticObjectsContainer::instance()->webSurface()->displayMode());
        m_mask = new (ELeave) CFbsBitmap();
        m_mask->Create(TSize(0,0),EGray2);
    }
    // Add the focus/foreground observer
    ControlEnv()->AddForegroundObserverL( *this ) ;
}

// -----------------------------------------------------------------------------
// PluginWin::~PluginWin
// Deconstructor.
// -----------------------------------------------------------------------------
//
PluginWin::~PluginWin()
{

    TRAP_IGNORE( setPluginFocusL( EFalse ) );

    // Remove the foreground observer
    ControlEnv()->RemoveForegroundObserver( *this );
    delete m_bitmap;
    delete m_mask;
    delete m_bitmapDeviceMask;
    delete m_bitmapContextMask;
}

// -----------------------------------------------------------------------------
// PluginWin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
PluginWin* PluginWin::NewL( PluginSkin* pluginskin,
                            const WebView& view  )
{
    PluginWin* self = new( ELeave ) PluginWin(pluginskin);
    return self;
}

// -----------------------------------------------------------------------------
// PluginWin::PositionChanged
// Update the size of the control.
// -----------------------------------------------------------------------------
//
void PluginWin::PositionChanged()
{
    windowChanged();
}

// -----------------------------------------------------------------------------
// PluginWin::SizeChanged
// Update the size of the control.
// -----------------------------------------------------------------------------
//
void PluginWin::SizeChanged()
{
    windowChanged();
}

void PluginWin::windowChanged()
{
    TRAPD(ret, windowChangedL());
    if (ret != KErrNone) {
        delete m_bitmap;
        m_bitmap = NULL;
        delete m_mask;
        m_mask = NULL;
        delete m_bitmapDeviceMask;
        m_bitmapDeviceMask = NULL;
        delete m_bitmapContextMask;
        m_bitmapContextMask = NULL;
    }
    if (m_forceScroll)
    	m_forceScroll = EFalse;
}

void PluginWin::windowChangedL()
{
    if (m_fullscreen) return;
    if (m_pluginskin->getNPPluginFucs() && m_pluginskin->getNPPluginFucs()->setwindow ){
        NPWindow  window;
    TRect rect( m_pluginskin->getPluginWinRect() );
    TRect clipRect = m_pluginskin->getClipRect();
    TRect myRect = Rect();
        if (!m_windowedPlugin) {
            myRect.Move(-myRect.iTl.iX, -myRect.iTl.iY); // windowed CCoeControl uses its own coordinates
        }
    TRect passedRect = clipRect;
    passedRect.Intersection(rect);
    TPoint tl = myRect.iTl - passedRect.iTl;
        // Coordinates for the plugin always start from (0,0) and are relative
        // to the parent CCoeControl
        rect.Move(tl);
        clipRect.Move(tl);
        clipRect.Intersection(myRect);
        window.x = rect.iTl.iX;
        window.y = rect.iTl.iY;
        window.width = rect.Width();
        window.height = rect.Height();
        window.type = m_windowedPlugin ? NPWindowTypeWindow : NPWindowTypeDrawable ;
        clipRect.iTl.iX = Max(clipRect.iTl.iX, 0);
        clipRect.iTl.iY = Max(clipRect.iTl.iY, 0);
        clipRect.iBr.iX = Max(clipRect.iBr.iX, 0);
        clipRect.iBr.iY = Max(clipRect.iBr.iY, 0);
        window.clipRect.top = clipRect.iTl.iY;
        window.clipRect.left = clipRect.iTl.iX;
        window.clipRect.bottom =clipRect.iBr.iY;
        window.clipRect.right = clipRect.iBr.iX;

        if (m_windowedPlugin) {
             window.window =  (MPluginAdapter*)this;
        } else {
             //m_bitmap->Resize(myRect.Size());
             // Resize does not work!
             // Once the bug is fixed we can call Resize() instead of creating a new bitmap every time.
             if (!m_bitmap || m_bitmap->SizeInPixels() != myRect.Size()) {
                 delete m_bitmap;
                 m_bitmap = NULL;
                 m_bitmap = new (ELeave) CFbsBitmap();
                 TDisplayMode mode = m_transparentPlugin ? EColor16MA : WebCore::StaticObjectsContainer::instance()->webSurface()->displayMode();
                 m_bitmap->Create(myRect.Size(), mode);
                 delete m_mask;
                 m_mask = NULL;
                 if (m_transparentPlugin) {
                     m_mask = new (ELeave) CFbsBitmap();
                     m_mask->Create(myRect.Size(), mode);
                     m_bitmapDeviceMask = CFbsBitmapDevice::NewL( m_mask );
                     User::LeaveIfError(m_bitmapDeviceMask->CreateContext(m_bitmapContextMask));
                 }
             }
             window.window = (CFbsBitmap*)m_bitmap;
        }
		if (!m_windowedPlugin || m_windowCreated)
			m_pluginskin->getNPPluginFucs()->setwindow(m_pluginskin->getNPP(), &window );

    }
}

// -----------------------------------------------------------------------------
// PluginWin::ProcessEventL
// Procces a user interface event.
// -----------------------------------------------------------------------------
//
void PluginWin::processEventL( TPluginEventType eventType,
                                TBool& consumed )
{
	  CBrCtl*   brCtl = control(m_pluginskin->frame());
	  WebView*  view = brCtl->webView();
    consumed = EFalse;

    switch ( eventType ) {
        case EEventActivate:
                if ( m_notifier )
                {
                    TPoint pt = StaticObjectsContainer::instance()->webCursor()->position() - Position();
                    m_notifier->NotifyL( MPluginNotifier::EPluginActivated, (void*) &pt );
                }
            consumed = ETrue;
            view->setPluginActivated(true);   //Setting pluginactivated flag in webview
            setPluginFocusL( ETrue );
        break;

        case EEventDeactivate:
                if( m_notifier )
                {
                    m_notifier->NotifyL( MPluginNotifier::EPluginDeactivated, (void*) 0 );
                }
            consumed = ETrue;
            view->setPluginActivated(false);
            setPluginFocusL( EFalse );
        break;

        case EEventLoseFocus:
        if ( m_notifier ) {
            HandleLosingForeground();
            consumed = ETrue;
        }
        break;

        case EEventGainFocus:
            HandleGainingForeground();
            consumed = ETrue;
        break;

        default:
        break;
    }

}


// -----------------------------------------------------------------------------
// PluginWin::OfferKeyEventL
// Hands a key event to the control.
// -----------------------------------------------------------------------------
//
TKeyResponse PluginWin::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                         TEventCode aType )
{
    TKeyResponse ret = EKeyWasNotConsumed;
    if ( m_control ) {
        ret = m_control->OfferKeyEventL( aKeyEvent, aType );
    }
    else if (!m_windowedPlugin && m_pluginskin->getNPPluginFucs() && m_pluginskin->getNPPluginFucs()->event) {
        NPEvent event;
        NPEventKey eventKey;
        event.event = ENppEventKey;
        eventKey.keyEvent = &aKeyEvent;
        eventKey.type = aType;
        eventKey.reserved = NULL;
        event.param = &eventKey;
        ret = (TKeyResponse)m_pluginskin->getNPPluginFucs()->event(m_pluginskin->getNPP(), static_cast<void*>(&event));
    }
    return ret;
}


// -----------------------------------------------------------------------------
// PluginWin::ComponentControl
// Return the child control by index.
// -----------------------------------------------------------------------------
//
CCoeControl* PluginWin::ComponentControl( TInt /*aIndex*/ ) const
{
    return 0;
}
// -----------------------------------------------------------------------------
// PluginWin::CountComponentControls
// Return the number of child controls.
// -----------------------------------------------------------------------------
//
TInt PluginWin::CountComponentControls() const
{
    return 0;
}

//-------------------------------------------------------------------------------
// HandlePointerEventL
// Handles Pointer Events
//-------------------------------------------------------------------------------
void PluginWin::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (!m_windowedPlugin && m_pluginskin->getNPPluginFucs() && m_pluginskin->getNPPluginFucs()->event) {
        NPEvent event;
        NPEventPointer ep;
        event.event = ENppEventPointer;

        TPoint pt;
        if (m_pluginskin->pluginPlayer()) {
            pt = aPointerEvent.iPosition;
            pt -= Rect().iTl;
            pt = m_pluginskin->frame()->frameView()->toDocCoords(pt);
        }
        else {
            pt = m_pluginskin->frame()->frameView()->viewCoordsInFrameCoords(aPointerEvent.iPosition);
            pt -= m_pluginskin->rect().iTl;
        }
        TPointerEvent pe(aPointerEvent);
        pe.iPosition = pt;

        ep.pointerEvent = &pe;
        ep.reserved = NULL;
        event.param = &ep;
        m_pluginskin->getNPPluginFucs()->event(m_pluginskin->getNPP(), static_cast<void*>(&event));
    }
}

// -----------------------------------------------------------------------------
// PluginWin::RefreshPlugin
//
// Refresh the plugin. - Called by PluginSkin
// -----------------------------------------------------------------------------
//
TInt PluginWin::refreshPlugin(CFbsBitGc& bitmapContext)
{
    if (m_control && IsVisible() && m_control->DrawableWindow())
        m_control->DrawNow();
    if (!m_windowedPlugin && m_bitmap) {
        NPEvent event;
        event.event = ENppEventDraw;
        event.param = 0;
        if (m_pluginskin->getNPPluginFucs() && m_pluginskin->getNPPluginFucs()->event) {
            m_pluginskin->getNPPluginFucs()->event(m_pluginskin->getNPP(), static_cast<void*>(&event));
        }
        WebFrameView* fv = m_pluginskin->frame()->frameView();
        TRect rect(Rect());
        rect = TRect(fv->viewCoordsInFrameCoords(Rect().iTl), fv->viewCoordsInFrameCoords(Rect().iBr));
        rect = fv->toViewCoords(rect);
        rect.SetSize(m_bitmap->SizeInPixels()); // toViewCoords sometimes grows the rect by 1, which wil cause the bitmap to not draw
        if (m_transparentPlugin) {
            m_bitmapContextMask->SetBrushStyle( CGraphicsContext::ESolidBrush );
            m_bitmapContextMask->SetPenStyle( CGraphicsContext::ESolidPen );
            m_bitmapContextMask->SetPenColor( TRgb( 30, 30, 30 ) );
            m_bitmapContextMask->SetBrushColor( TRgb( 30, 30, 30 ) );
            m_bitmapContextMask->DrawRect(m_mask->SizeInPixels() );
            bitmapContext.DrawBitmapMasked(rect, m_bitmap, rect.Size(), m_mask, true);
        }
        else {
            bitmapContext.DrawBitmap(rect, m_bitmap, rect.Size());
        }
    }
    return KErrNone;
}

// -----------------------------------------------------------------------------
// PluginWin::MakeVisible
//
// Hides or shows the PluginInst.
// -----------------------------------------------------------------------------
//
void PluginWin::makeVisible( TBool visible )
    {
    if(IsVisible() != visible) 
    {
        CCoeControl::MakeVisible(visible);
    }
    NotifyPluginVisible(visible);
    if (!m_windowedPlugin && m_pluginskin->getNPPluginFucs() && m_pluginskin->getNPPluginFucs()->event) {
        NPEvent event;
        NpEventVisibility ev;
        event.event = ENppEventVisibility;
        ev.visible = visible;
        ev.reserved = 0;
        event.param = &ev;
        m_pluginskin->getNPPluginFucs()->event(m_pluginskin->getNPP(), static_cast<void*>(&event));
    }
}

// -----------------------------------------------------------------------------
// PluginWin::HandleControlEventL
//
// Handles an event from an observed control.
// -----------------------------------------------------------------------------
//
void PluginWin::HandleControlEventL( CCoeControl* /*aControl*/,
                                      TCoeEvent /*aEventType*/ )
{
}

// -----------------------------------------------------------------------------
// PluginWin::ViewFocusChanged
//
// Invoked by CCoeControl to WebKitView to PluginWin when the Browser focus
// changes. This method notifies all plugins of thier current focus state.
// NOTES:
// iBrowserBackground means that the browser app is not top application,
// such as when bookmarks, idle screen, or another app/view is in foreground.
// aFocused is true if browser has focus and false if browser is out of focus
// and a plugin has taken focus.
// -----------------------------------------------------------------------------
void PluginWin::viewFocusChanged( TBool focused )
{
if (m_notifier)
		{
		if (focused)
			{
			// The browser brings back the focus, and so are plugins.
			    HandleGainingForeground();
			}
		else
			{
			// The browser doesn't have focus and this plugin focus state is...
			if ( m_pluginfocus )
				{
				// This plugin is selected to be activate, so the focus is on this plugin now.
				   HandleGainingForeground();
				}
			else
				{
				// The focus on the plugin is cancelled. This happens when deactivate the selected plugin
				   HandleLosingForeground();
				}
			}
		}
}

// -----------------------------------------------------------------------------
// PluginWin::SetPluginFocusL
// Give or take focus from the plugin. The calls AddToStackL or RemoveFromStackL
// will cause the CCoeControl architecture to call SetFocus, which will end up
// calling ViewFocusChanged().
// -----------------------------------------------------------------------------
//
void PluginWin::setPluginFocusL(TBool focus)
{
    if (focus) {

        if (!m_pluginfocus) {
            m_pluginfocus = ETrue;
            iEikonEnv->EikAppUi()->AddToStackL(this);
            SetFocus(ETrue);
        }
    }
    else {

        if (m_pluginfocus) {
            m_pluginfocus = EFalse;
            iEikonEnv->EikAppUi()->RemoveFromStack( this );
            SetFocus(EFalse);
        }
    }
}


// -----------------------------------------------------------------------------
// PluginWin::ResizePluginRect
//
// Resize the plugin rect. This requires that we set the iRect and re-layout
// the render object.
// -----------------------------------------------------------------------------
//
void PluginWin::resizePluginRect(TRect& rect)
{
    // Get our current rect
    TRect skinRect( m_pluginskin->rect() );

    // If the rect sizes are the same, we don't need to save and relayout
    if (skinRect.Size() == rect.Size()) {
        return;
    }

    // Set the PluginSkin rect, used by WebCore and PluginWin for layout.
    // The adjusted rect has the new size, but same starting point
    TRect adjustedRect (skinRect.iTl, rect.Size() );
    m_pluginskin->setRect(adjustedRect);

    m_pluginskin->resized();
}

// -----------------------------------------------------------------------------
// PluginWin::HandleGainingForeground
// -----------------------------------------------------------------------------
//
void PluginWin::HandleGainingForeground()
{
    if (m_notifier) {
        TRAP_IGNORE(m_notifier->NotifyL(MPluginNotifier::EApplicationFocusChanged, (void*)1));
    }
}

// -----------------------------------------------------------------------------
// PluginWin::HandleLosingForeground
// -----------------------------------------------------------------------------
//
void PluginWin::HandleLosingForeground()
{
    if (m_notifier) {
        TRAP_IGNORE(m_notifier->NotifyL(MPluginNotifier::EApplicationFocusChanged, (void*)0));
    }
}


// Functions from MPluginAdapter
HBufC8* PluginWin::PluginDataUrl()
{
    if (m_pluginskin)
        return m_pluginskin->url();
    return NULL;
}

// -----------------------------------------------------------------------------
// PluginWin::addPluginOptionsL
//
// Add the Option Menu items to the menuPane, these will be handled by the
// plugin. The menu items are added in this method and then handled in the
// HandlePluginCommandL method. If the Option Menu Handler is not defined, then
// the plugin doesn't handle custom menu commands.
// -----------------------------------------------------------------------------
//
void PluginWin::addPluginOptionsL(CEikMenuPane& menuPane, TInt commandBase, TInt after)
    {
    if (m_optionmenuhandler)
        {
        m_optionmenuhandler->AddPluginOptionMenuItemsL(menuPane, commandBase, after);
        }
    }

// -----------------------------------------------------------------------------
// PluginWin::handlePluginCommandL
//
// Add the Option Menu items to the menuPane, these will be handled by the
// plugin. The menu items are added in this method and then handled in the
// HandlePluginCommandL method. If the Option Menu Handler is not defined, then
// the plugin doesn't handle custom menu commands.
// -----------------------------------------------------------------------------
//
void PluginWin::handlePluginCommandL(TInt id)
    {
    if (m_optionmenuhandler)
        {
        m_optionmenuhandler->HandlePluginCommandL(id);
        }
    }

// -----------------------------------------------------------------------------
// PluginWin::FocusChanged
//
// virtual function from CCoeControl for plugin win
// -----------------------------------------------------------------------------
//
void PluginWin::FocusChanged(TDrawNow aDrawNow)
{
    if(m_pluginfocus) {
        m_pluginskin->pluginFocusChanged(IsFocused());
    }
}

// -----------------------------------------------------------------------------
//
NPObject* PluginWin::windowScriptNPObject()
    {
    Frame* f = core(m_pluginskin->frame());
    if (f)
        return f->windowScriptNPObject();
    return 0;
    }


// -----------------------------------------------------------------------------
// PluginWin::ForceRedraw
// -----------------------------------------------------------------------------
//
void PluginWin::forceRedraw(bool drawNow)
    {
    if (m_pluginskin->pluginPlayer()) {
        DrawNow();
    }
    else {
        WebFrame* mf = (m_pluginskin->frame());
        WebFrameView* fv = mf->frameView();
        TRect rect(Rect());
        rect = TRect(fv->viewCoordsInFrameCoords(Rect().iTl), fv->viewCoordsInFrameCoords(Rect().iBr));
    if (mf && mf->frameView())
            mf->frameView()->invalidateRect(rect, drawNow);
    }
}

// -----------------------------------------------------------------------------
// CPluginWin::MoveWindow
// Move/scroll window by aOffset and also sets the current cursor position
// -----------------------------------------------------------------------------
//
void PluginWin::moveWindow(const TPoint& aOffset)
    {
    // set force scroll to true, when plugin gives move command, 
    // browser needs to move window according to offset value
    m_forceScroll = ETrue;
    // This plug-in has focus, so hide the Browser's cursor. The Plugin
    // can display its own cursor, if it needs one.

    WebFrame* mf = mainFrame(m_pluginskin->frame());
    WebCursor* c = StaticObjectsContainer::instance()->webCursor();
    if (c && mf && mf->frameView() && !m_pluginskin->pluginPlayer())
        {
            // Offset sent by plugins doesn't take care of zoom level. Calculate currect offset value
            WebView *view = control(m_pluginskin->frame())->webView();
            TInt zoomlevel = view->zoomLevel();
		    TPoint oldPos(mf->frameView()->contentPos());
		    TPoint newPos ((aOffset.iX  * 100)/zoomlevel, (aOffset.iY  * 100)/zoomlevel);
	        c->offsetCursor( aOffset );
            mf->frameView()->scrollTo(oldPos + newPos);
            c->cursorUpdate(EFalse);        
        }

    }

// -----------------------------------------------------------------------------
// CPluginWin::PluginDeactivate
// Deactivate the plugin
// -----------------------------------------------------------------------------
//
void PluginWin::pluginDeactivate()
    {
    m_pluginskin->deActivate();
    }

// -----------------------------------------------------------------------------
// CPluginWin::HitRegionContains
// -----------------------------------------------------------------------------
//
TBool PluginWin::HitRegionContains(const TPoint &aPoint, const CCoeControl &/*aControl*/) const
    {

    if (m_pluginskin->frame()->frameView()->topView()->focusedElementType() != TBrCtlDefs::EElementActivatedObjectBox)
        {

        if ( !m_pluginskin->CanInteract() && !m_pluginskin->pluginPlayer() )
        {
            m_pluginskin->activate();
        }
        else {
        TPoint point = m_pluginskin->getPluginWinRect().iTl + aPoint;

        // Plugin gets pointer event. Activate plugin
	      control(m_pluginskin->frame())->webView()->setFocusNone();

        WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
        cursor->setPosition(point);
        TKeyEvent keyEvent;
        keyEvent.iModifiers = 0;
        keyEvent.iRepeats = 0;
        keyEvent.iCode = EKeyDevice3;
        keyEvent.iScanCode = EStdKeyDevice3;
        CCoeEnv::Static()->SimulateKeyEventL(keyEvent, EEventKeyDown);
        CCoeEnv::Static()->SimulateKeyEventL(keyEvent, EEventKey);
        CCoeEnv::Static()->SimulateKeyEventL(keyEvent, EEventKeyUp);
        PluginWin* win = const_cast<PluginWin*>(this);
        }
        }
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CPluginWin::PluginConstructedL
//
// Notify the associated PluginInst that it's control has been created.
// -----------------------------------------------------------------------------
//
void PluginWin::PluginConstructedL( CCoeControl *aControl )
    {
    m_control = aControl;
    aControl->SetHitTest(this);
    }

// -----------------------------------------------------------------------------
// CPluginWin::Draw
//
// Draw the plugin, called only when in the PluginPlayer.
// -----------------------------------------------------------------------------
//
void PluginWin::Draw( const TRect& /*rect*/ ) const
{
    if (m_pluginskin->pluginPlayer() && !m_windowedPlugin && m_bitmap) {
        NPEvent event;
        event.event = ENppEventDraw;
        event.param = 0;
        if (m_pluginskin->getNPPluginFucs() && m_pluginskin->getNPPluginFucs()->event) {
            m_pluginskin->getNPPluginFucs()->event(m_pluginskin->getNPP(), static_cast<void*>(&event));
        }
        CWindowGc& gc = SystemGc();
        gc.BitBlt(Rect().iTl, m_bitmap);
    }
}

void PluginWin::TogleScreenMode(bool aFullScreen)
{
  m_fullscreen = aFullScreen;
  WebFrame* mf = mainFrame(m_pluginskin->frame());
  CBrCtl*   brCtl = control(m_pluginskin->frame());
  WebView*  view = brCtl->webView();

  view->setPluginFullscreen(aFullScreen);
  brCtl->reportStateChanged(TBrCtlDefs::EStatePluginFullScreen, m_fullscreen);
}

void PluginWin::NotifyPluginVisible (TBool visible)

{
  if(m_notifier)
    {
      if(visible && IsVisible())
      {
        TRAP_IGNORE(m_notifier->NotifyL(MPluginNotifier::EPluginInvisible, (void*)1));
      }
      else
      {
        TRAP_IGNORE(m_notifier->NotifyL(MPluginNotifier::EPluginInvisible, (void*)0));
      }	
    }
}
