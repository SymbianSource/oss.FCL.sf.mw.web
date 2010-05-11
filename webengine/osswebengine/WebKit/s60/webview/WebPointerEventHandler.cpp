/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <browser_platform_variant.hrh>
#include "config.h"
#include "../../bidi.h"
#include <AknUtils.h>
#ifdef BRDO_TOUCH_ENABLED_FF
#include <touchfeedback.h>
#endif // BRDO_TOUCH_ENABLED_FF
#include "brctl.h"
#include <brctldefs.h>
#include "WebPointerEventHandler.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "WebCursor.h"
#include "WebPopupDrawer.h"
#include "WebFormFillPopup.h"
#include "WebPageScrollHandler.h"
#include "WebFepTextEditor.h"
#include "PluginSkin.h"
#include "WebUtil.h"
#include "WebPageFullScreenHandler.h"
#include "PluginWin.h"
#include "WebFrameBridge.h"
#include "Page.h"
#include "Chrome.h"
#include "ChromeClient.h"
#include "FocusController.h"
#include "WebTabbedNavigation.h"
#include "SettingsContainer.h"
#include "PluginHandler.h"
#include "WebGestureInterface.h"
#include "WebPagePinchZoomHandler.h"
#include "WebScrollingDeceleratorGH.h"

#include "WebKitLogger.h"

#define IS_NAVIGATION_NONE      (m_webview->brCtl()->settings()->getNavigationType() == SettingsContainer::NavigationTypeNone)
#define IS_TABBED_NAVIGATION    (m_webview->brCtl()->settings()->getNavigationType() == SettingsContainer::NavigationTypeTabbed)

//-----------------------------------------------------------------------------
// WebPointerEventHandler::NewL
//-----------------------------------------------------------------------------
WebPointerEventHandler* WebPointerEventHandler::NewL(WebView* view)
{
    WebPointerEventHandler* self = new (ELeave) WebPointerEventHandler(view);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

//-----------------------------------------------------------------------------
// WebPointerEventHandler::WebPointerEventHandler
//-----------------------------------------------------------------------------
WebPointerEventHandler::WebPointerEventHandler(WebView* view)
    : m_webview(view),
      m_isHighlighted(false),
      m_highlightedNode(NULL),
      m_ignoreTap(false),
      m_gestureInterface(NULL)
{
}

//-----------------------------------------------------------------------------
// WebPointerEventHandler::~WebPointerEventHandler
//-----------------------------------------------------------------------------
WebPointerEventHandler::~WebPointerEventHandler()
{
   delete m_gestureInterface;
}

//-----------------------------------------------------------------------------
// WebPointerEventHandler::ConstructL
//-----------------------------------------------------------------------------
void WebPointerEventHandler::ConstructL()
{
#ifdef BRDO_USE_GESTURE_HELPER
  m_gestureInterface = WebGestureInterface::NewL(m_webview);
#else
  m_gestureInterface = NULL;
#endif
}


// ======================================================================
// WebPointerEventHandler::HandleGestureEventL
// ======================================================================
void  WebPointerEventHandler::HandleGestureEventL(const TStmGestureEvent& aGesture)
{
    TStmGestureUid uid = aGesture.Code();

    if (m_webview->pinchZoomHandler()->isPinchActive() && uid != stmGesture::EGestureUidPinch)
        return;

    TBrCtlDefs::TBrCtlElementType elType = m_webview->focusedElementType();

    PluginSkin* plugin = m_webview->mainFrame()->focusedPlugin();
    if (plugin && plugin->pluginWin()) {
        if (plugin->pluginWin()->HandleGesture(aGesture)) {
         if(!plugin->isActive()) {
            plugin->activate();
         }
         else {
            m_webview->mainFrame()->frameView()->topView()->setFocusedElementType(TBrCtlDefs::EElementActivatedObjectBox);
            m_webview->page()->focusController()->setFocusedNode(plugin->getElement(), m_webview->page()->focusController()->focusedOrMainFrame());                        
            m_webview->brCtl()->updateDefaultSoftkeys();
         }
         return;
        }
    }

    updateCursor(aGesture.CurrentPos());
    if (IS_TABBED_NAVIGATION) {
        m_webview->tabbedNavigation()->updateCursorPosition(aGesture.CurrentPos());
    }


    switch(uid) {
        case stmGesture::EGestureUidTouch:
        {
            if (m_webview->viewIsScrolling()) {
                m_ignoreTap = true;
                m_webview->pageScrollHandler()->handleTouchDownGH(aGesture);
            }
            else {
                handleTouchDownL(aGesture);
            }
            break;
        }


        case stmGesture::EGestureUidTap:
        {
            if (aGesture.Type() == stmGesture::ETapTypeSingle) {
                if (!m_ignoreTap) {
                    handleTapL(aGesture);
                }
            }
            else {
                handleDoubleTap(aGesture);
            }

                break;
        }

        case stmGesture::EGestureUidRelease:
        {
            m_ignoreTap = false;
            handleTouchUp(aGesture);
			m_webview->resumeJsTimers(); // resume js timers
            break;
        }

        case stmGesture::EGestureUidPan:
        {
            // pause js timers (if not already done)
            if(!m_webview->jsTimersPaused()) 
                m_webview->pauseJsTimers();

            if(!m_webview->isScrolling())
                m_webview->setScrolling(true);
            handleMove(aGesture);
            break;
        }
        case stmGesture::EGestureUidFlick:
        {
            m_ignoreTap = false;
            if (!IS_NAVIGATION_NONE) {
                handleTouchUp(aGesture);
            }
            else {
                Frame* frm = m_webview->page()->focusController()->focusedOrMainFrame();
                m_webview->sendMouseEventToEngine(TPointerEvent::EButton1Up, m_highlightPos, frm);
            }
            break;
        }

        case stmGesture::EGestureUidPinch:
        {
        if(!m_webview->inPageViewMode())
            {
            handlePinchZoomL(aGesture);
            if(aGesture.GestureState() != EGestureEnter) {
                m_webview->resumeJsTimers(); // resume js timers
                }
            }
            break;
        }
        default:
            break;

    }
    return;
}


// ======================================================================
// WebPointerEventHandler::handleTap
// ======================================================================
void WebPointerEventHandler::handleTapL(const TStmGestureEvent& aGesture)
{
    m_lastTapEvent = m_currentEvent;
    if(!m_webview->inPageViewMode()){
    	doTapL();
    }
}
// ======================================================================
//  WebPointerEventHandler::handleDoubleTap
//======================================================================
void WebPointerEventHandler::handleDoubleTap(const TStmGestureEvent& aGesture)
{
    if ( !m_webview->viewIsScrolling() &&
         (m_webview->brCtl()->capabilities() & TBrCtlDefs::ECapabilityFitToScreen)) {
        if (m_isHighlighted){
            dehighlight();
        }
        m_webview->setZoomLevelAdaptively();
    }
    else {
        m_webview->resetLastZoomLevelIfNeeded();
    }
}

// ======================================================================
// WebPointerEventHandler::handleTouchDownL
//======================================================================
void WebPointerEventHandler::handleTouchDownL(const TStmGestureEvent& aGesture)
{
    TBrCtlDefs::TBrCtlElementType elType = m_webview->focusedElementType();
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    PluginSkin* pluginToActivate = pluginHandler->pluginToActivate();
    m_buttonDownEvent = m_currentEvent;
    m_highlightPos = aGesture.CurrentPos();


    if (!IS_NAVIGATION_NONE) {
        m_webview->pageScrollHandler()->handleTouchDownGH(aGesture);
    }

    if ( TBrCtlDefs::EElementActivatedObjectBox == elType) {
        PluginSkin* plugin = m_webview->mainFrame()->focusedPlugin();
        if (plugin && plugin->pluginWin()) {
            if (!plugin->getClipRect().Contains(m_buttonDownEvent.iPosition)) {
                plugin->deActivate();
            }
            else {
                plugin->pluginWin()->HandlePointerEventL(m_buttonDownEvent);
            }
        }
    }
    
    doTouchDownL();
}

// ======================================================================
// WebPointerEventHandler::handleTouchUp
// ======================================================================
void WebPointerEventHandler::handleTouchUp(const TStmGestureEvent& aGesture)
{
    m_highlightPos = TPoint(-1,-1);
    m_highlightedNode = NULL;
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    pluginHandler->setPluginToActivate(NULL);
    if (!IS_NAVIGATION_NONE) {
        m_webview->pageScrollHandler()->handleTouchUpGH(aGesture);
    }
}

// ======================================================================
// WebPointerEventHandler::handleMoveL
// ======================================================================
void WebPointerEventHandler::handleMove(const TStmGestureEvent& aGesture)
{
    TBrCtlDefs::TBrCtlElementType elType = m_webview->focusedElementType();
    TPoint curPos =  aGesture.CurrentPos();
    PluginHandler* pluginHandler = WebCore::StaticObjectsContainer::instance()->pluginHandler();
    pluginHandler->setPluginToActivate(NULL);

    if (IS_NAVIGATION_NONE) {
        Frame* frm = m_webview->page()->focusController()->focusedOrMainFrame();
        m_webview->sendMouseEventToEngine(TPointerEvent::EMove, curPos, frm);
    }
    else {
    HandleHighlightChange(curPos);

    m_webview->pageScrollHandler()->handleScrollingGH(aGesture);
    }
}


// ======================================================================
// WebPointerEventHandler::HandlePointerEventL
// ======================================================================
void WebPointerEventHandler::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    m_currentEvent = aPointerEvent;

    if (m_webview->isSynchRequestPending()) {
       return;
    }

    // Handle graphical history - should never happen, as HistoryView handles this event by itself
    if ( m_webview->brCtl()->historyHandler()->historyController()->historyView()) {
        return;
    }

    // Handle FormFill popup
    if (m_webview->formFillPopup() && m_webview->formFillPopup()->IsVisible()) {
        m_webview->formFillPopup()->HandlePointerEventL(aPointerEvent);
        return;
    }
    
    //if scrolling is going on, it need to be stopped immediately when user touches down    
    if (aPointerEvent.iType == TPointerEvent::EButton1Down && m_webview->viewIsScrolling()) {
        WebScrollingDeceleratorGH* scrollDecelGH = m_webview->pageScrollHandler()->ScrollingDeceleratorGH();
        if(scrollDecelGH) {
            scrollDecelGH->cancelDecel();            
        }
    }
    
    // pause JS timers for better ui response    
    if (aPointerEvent.iType == TPointerEvent::EButton1Down) {
        m_webview->pauseJsTimers();    
    }
#ifdef BRDO_USE_GESTURE_HELPER
      m_gestureInterface->HandlePointerEventL(aPointerEvent);
#endif
}

//-----------------------------------------------------------------------------
// WebPointerEventHandler::HandleHighlightChange
//-----------------------------------------------------------------------------
void WebPointerEventHandler::HandleHighlightChange(const TPoint &aPoint)
{
    if (!m_highlightedNode || !m_isHighlighted)    return;

    if (!canDehighlight(aPoint)) return;

    dehighlight();
}

//-----------------------------------------------------------------------------
// WebPointerEventHandler::highlitableElement
//-----------------------------------------------------------------------------
TBrCtlDefs::TBrCtlElementType WebPointerEventHandler::highlitableElement()
{
    TRect elRect;
    TBrCtlDefs::TBrCtlElementType elType = TBrCtlDefs::EElementNone;
    Frame* coreFrame = core(m_webview->mainFrame());
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();

    TPointerEvent event;
    TPoint pos = cursor->position();
    WebFrame* wfrm = cursor->getFrameAtPoint(pos);
    Frame* frm = core(wfrm);
    TPoint pt(wfrm->frameView()->viewCoordsInFrameCoords(pos));
    TPoint nodePoint;

    Element* eventNode = frm->document()->elementFromPoint(pt.iX, pt.iY);

    m_highlightedNode = NULL;

    Node* retNode = 0;
    frm->bridge()->getTypeFromElement(eventNode, elType, elRect, retNode);

    if (elType == TBrCtlDefs::EElementNone) {
        Node* n = wfrm->getClosestAnchorElement(cursor->position(), pos);
        if (n) {
            wfrm = cursor->getFrameAtPoint(pos);
            frm = core(wfrm);
            //eventNode = frm->document()->elementFromPoint(pos.iX, pos.iY);
            TPoint newPos(wfrm->frameView()->viewCoordsInFrameCoords(pos));
            eventNode = frm->document()->elementFromPoint(newPos.iX, newPos.iY);
            frm->bridge()->getTypeFromElement(eventNode, elType, elRect, retNode);
            TPoint nodePoint = n->getRect().Rect().Center();
            m_offset = (pt.iX- nodePoint.iX)*(pt.iX- nodePoint.iX) +
                       (pt.iY- nodePoint.iY)*(pt.iY- nodePoint.iY);
        }
    }
    m_highlightedNode = eventNode;
    if (m_highlightedNode) {
        m_highlightPos = pos;
        m_buttonDownEvent.iPosition = m_highlightPos;
    }
    m_webview->setFocusedElementType(elType);
    return elType;
}

// ======================================================================
// WebPointerEventHandler::isHighlitableElement
// ======================================================================
bool WebPointerEventHandler::isHighlitableElement(TBrCtlDefs::TBrCtlElementType& elType)
{
    return (elType == TBrCtlDefs::EElementAnchor
                    || elType == TBrCtlDefs::EElementInputBox
                    || elType == TBrCtlDefs::EElementActivatedInputBox
                    || elType == TBrCtlDefs::EElementButton
                    || elType == TBrCtlDefs::EElementCheckBoxChecked
                    || elType == TBrCtlDefs::EElementCheckBoxUnChecked
                    || elType == TBrCtlDefs::EElementTextAreaBox
                    || elType == TBrCtlDefs::EElementRadioButtonSelected
                    || elType == TBrCtlDefs::EElementRadioButtonUnSelected
                    || elType == TBrCtlDefs::EElementTelAnchor
                    || elType == TBrCtlDefs::EElementMailtoAnchor
                    || elType == TBrCtlDefs::EElementSmartLinkTel
                    || elType == TBrCtlDefs::EElementSmartLinkEmail
                    || elType == TBrCtlDefs::EElementSmartLinkVoip);
}


//-----------------------------------------------------------------------------
// WebPointerEventHandler::doTapL
//-----------------------------------------------------------------------------
void WebPointerEventHandler::doTapL()
{
    TBrCtlDefs::TBrCtlElementType elType = m_webview->focusedElementType();
    Frame* coreFrame = core(m_webview->mainFrame());

#ifdef BRDO_TOUCH_ENABLED_FF
    if (m_isHighlighted)
    {
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if (feedback)
           {
           feedback->InstantFeedback(ETouchFeedbackBasic);
           }
    }
#endif // BRDO_TOUCH_ENABLED_FF
    
     /*
      * We assume that if element visibility has been changed
      * between "up" and "down" that means that some node event
      * listener (onMouseOver etc) handling happened and we don't
      * want to send a click (mouse press + mouse release) event.
      * The exception is editable element, since we want VKB anyway
      */
     if (!IS_NAVIGATION_NONE &&
         elType != TBrCtlDefs::EElementActivatedInputBox &&
         elType != TBrCtlDefs::EElementTextAreaBox &&
         m_webview->page()->chrome()->client()->elementVisibilityChangedByMouse()) {
         return;
     }

     m_lastTapEvent.iPosition = m_buttonDownEvent.iPosition;
     m_lastTapEvent.iType = TPointerEvent::EButton1Up;
     m_lastTapEvent.iModifiers = 0;

    // don't pass the event if the text input is not in valid format
    if (m_webview->fepTextEditor()->validateTextFormat()) {
        if (!IS_NAVIGATION_NONE) {
            // in case of navigation none button down was sent in buttonDownTimerCB()
            m_webview->sendMouseEventToEngine(TPointerEvent::EButton1Down, m_highlightPos, coreFrame);
        }
        m_webview->sendMouseEventToEngine(TPointerEvent::EButton1Up,  m_highlightPos, coreFrame);
    }

    // special handling for broken image (why is this here??)
    if (elType == TBrCtlDefs::EElementBrokenImage) {
        loadFocusedImage(m_webview);
    }
    else if (elType == TBrCtlDefs::EElementActivatedObjectBox) {
        PluginSkin* plugin = m_webview->mainFrame()->focusedPlugin();
        if(plugin && plugin->pluginWin() && plugin->getClipRect().Contains(m_lastTapEvent.iPosition)){
            plugin->pluginWin()->HandlePointerEventL(m_lastTapEvent);
        }
    }
    else {
        m_webview->activateVirtualKeyboard();
    }
}


//-----------------------------------------------------------------------------
// WebPointerEventHandler::deHighlight
//-----------------------------------------------------------------------------
void  WebPointerEventHandler::dehighlight()
{
    // send dehighlight event to engine by passing -1, -1
    // sending any other pointer value may result in highligh of other links
    m_highlightPos = TPoint(-1, -1);
    m_isHighlighted = EFalse;

    Frame* frm = m_webview->page()->focusController()->focusedOrMainFrame();
    m_webview->sendMouseEventToEngine(TPointerEvent::EMove, m_highlightPos, frm);

    m_highlightedNode = NULL;

    m_webview->syncRepaint();
}


//-----------------------------------------------------------------------------
// WebPointerEventHandler::canDehighlight
//-----------------------------------------------------------------------------
 bool WebPointerEventHandler::canDehighlight(const TPoint &aPoint)
 {
    TBool checkDehighlight = true;

    if (!m_webview->viewIsScrolling())
    {
        WebFrame* frm = StaticObjectsContainer::instance()->webCursor()->getFrameUnderCursor();
        IntPoint framePoint = frm->frameView()->viewCoordsInFrameCoords(aPoint);
        if (m_highlightedNode->getRect().contains(framePoint)) {
            checkDehighlight = false;
        } // check if new points are coming towards closest element
        else if(m_offset) {
            //Calculate the new offset
           IntPoint nodePoint = m_highlightedNode->getRect().Rect().Center();
           TInt newoffset = (framePoint.x()- nodePoint.x())*(framePoint.x()- nodePoint.x()) + (framePoint.y()- nodePoint.y())*(framePoint.y()- nodePoint.y());

           if(newoffset <= m_offset ) {
                m_offset = newoffset;
                checkDehighlight = false;
           }
           else {
               m_offset =0;
               checkDehighlight = true;
           }
        }
    }

    return checkDehighlight;
}


//-----------------------------------------------------------------------------
// WebPointerEventHandler::buttonDownTimerCallback
//-----------------------------------------------------------------------------
void WebPointerEventHandler::doTouchDownL()
{
    Frame* coreFrame = core(m_webview->mainFrame());
    TPointerEvent event;
    
    TBrCtlDefs::TBrCtlElementType elType = highlitableElement();

    if (!isHighlitableElement(elType)) {
        elType = TBrCtlDefs::EElementNone;
    }
    m_isHighlighted = (m_highlightedNode != NULL) && (elType != TBrCtlDefs::EElementNone) ;
    m_webview->page()->chrome()->client()->setElementVisibilityChanged(false);
    
    /*
     * Tabbed navigation might already set the focused node.
     * If it's the same as m_highlightedNode FocuseController::setFocusedNode()
     * wouldn't do anything and setEditable won't be called.
     * So we are setting focused node to NULL here and let mouse event handler
     * set it through FocuseController::setFocusedNode()
     */
    if (IS_TABBED_NAVIGATION &&
       (elType == TBrCtlDefs::EElementInputBox || elType == TBrCtlDefs::EElementTextAreaBox)) {
           coreFrame->document()->setFocusedNode(NULL);
    }

    if (!IS_NAVIGATION_NONE) {
		m_webview->sendMouseEventToEngine(TPointerEvent::EMove, m_highlightPos, coreFrame);
    } 
    else {
        m_webview->sendMouseEventToEngine(TPointerEvent::EButton1Down, m_highlightPos, coreFrame);
    }

}


//-----------------------------------------------------------------------------
// WebPointerEventHandler::updateCursor
//----------------------------------------------------------------------------
void WebPointerEventHandler::updateCursor(const TPoint& pos)
{
    WebCursor* cursor = StaticObjectsContainer::instance()->webCursor();
    if (cursor) {
        if (pos != TPoint(0, 0)) {
            cursor->setPosition(pos);
        }
        if (m_webview->showCursor()) {
            cursor->resetTransparency();
            m_webview->setShowCursor(false);
            cursor->cursorUpdate(false);
        }
    }
}


//-----------------------------------------------------------------------------
// WebPointerEventHandler::handlePinchZoom
//----------------------------------------------------------------------------
void  WebPointerEventHandler::handlePinchZoomL(const TStmGestureEvent& aGesture)
{
 //dehighlight anything which is highlighted already
    if (m_isHighlighted){
        dehighlight();
    }
    m_webview->pinchZoomHandler()->handlePinchGestureEventL(aGesture);
}
