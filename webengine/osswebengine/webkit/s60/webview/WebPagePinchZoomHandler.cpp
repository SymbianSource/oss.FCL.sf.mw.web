/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <../bidi.h>
#include "WebPagePinchZoomHandler.h"
#include "WebView.h"
#include "PluginSkin.h"

const int KBitmapUpdateTimeout  = 100*1000;
const int KPinchExitWaitTimeout = 300*1000;

const int KPinchZoomStepSizeSmall  = 3;
const int KPinchZoomStepSizeMedium = 4;
const int KPinchZoomStepSizeLarge  = 5;
const int KMinPinchFactor = 1;

int pinchBitmapUpdateTimerCb( void* ptr );
int pinchExitWaitTimerCb( void* ptr );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WebPagePinchZoomHandler::NewL
// The two-phase Symbian constructor
// -----------------------------------------------------------------------------
//
WebPagePinchZoomHandler* WebPagePinchZoomHandler::NewL(WebView* webView)
{
    WebPagePinchZoomHandler* self = new (ELeave) WebPagePinchZoomHandler(webView);
    CleanupStack::PushL(self);
    self->constructL();
    CleanupStack::Pop(); //self
    return self;
}

// -----------------------------------------------------------------------------
// WebPagePinchZoomHandler::WebPointerEventHandler
// C++ default constructor
//
// -----------------------------------------------------------------------------
//
WebPagePinchZoomHandler::WebPagePinchZoomHandler(WebView* webView)
: m_webView(webView)
, m_zoomOutBaseLevel(0)
, m_pinchCenterSet(false)
, m_zoomStepSize(0)
, m_pinchActive(false)
, m_pinchCenter(0,0)
{
}

// -----------------------------------------------------------------------------
// WebPagePinchZoomHandler::constructL
// The constructor that can contain code that might leave.
// -----------------------------------------------------------------------------
//
void WebPagePinchZoomHandler::constructL()
{
    m_bitmapUpdateTimer = CPeriodic::NewL(CActive::EPriorityHigh);
    m_pinchExitWaitTimer = CPeriodic::NewL(CActive::EPriorityHigh);
}

// -----------------------------------------------------------------------------
// WebPagePinchZoomHandler::~WebPagePinchZoomHandler
// -----------------------------------------------------------------------------
WebPagePinchZoomHandler::~WebPagePinchZoomHandler()
{
    if (m_bitmapUpdateTimer->IsActive())
        m_bitmapUpdateTimer->Cancel();
    delete m_bitmapUpdateTimer;
    
    if (m_pinchExitWaitTimer->IsActive()) 
        m_pinchExitWaitTimer->Cancel();
    delete m_pinchExitWaitTimer;

}

// -----------------------------------------------------------------------------
// getPinchZoomStepSize
// -----------------------------------------------------------------------------
TInt WebPagePinchZoomHandler::getPinchZoomStepSize()
{
    TInt zoomStepSize = 0;

    TInt absPinchFactor = Abs (m_pinchFactor);
    if (absPinchFactor == 0)
        return zoomStepSize;
    
    if ( absPinchFactor >= KMinPinchFactor  &&  absPinchFactor <= 10 ) {
        zoomStepSize = KPinchZoomStepSizeSmall;
    }    
    else if ( absPinchFactor > 10 && absPinchFactor <= 20 ) {
        zoomStepSize = KPinchZoomStepSizeMedium;
    }  
    else if ( absPinchFactor > 20 ) {
        zoomStepSize = KPinchZoomStepSizeLarge;
    }

    return (m_pinchFactor/absPinchFactor) * zoomStepSize; 
}

// -----------------------------------------------------------------------------
// handlePinchGestureEventL
// -----------------------------------------------------------------------------
void WebPagePinchZoomHandler::handlePinchGestureEventL(const TStmGestureEvent& aGesture)
{
    if(aGesture.GestureState() == EGestureEnter) {
        m_pinchActive = true;
        handlePinchGestureL(aGesture);
    }
    else {
        m_pinchActive = false;
        handlePinchGestureExitL(aGesture);
    }

}

// -----------------------------------------------------------------------------
// handlePinchGestureL
// -----------------------------------------------------------------------------
void WebPagePinchZoomHandler::handlePinchGestureL(const TStmGestureEvent& aGesture)
{

    m_pinchFactor = aGesture.Details();
    TInt zoomValue = 0;
    TInt currentZoom = m_webView->zoomLevel();
    TInt zoomStepSize = getPinchZoomStepSize();

    if (zoomStepSize == 0)
        return;

    zoomValue = currentZoom + zoomStepSize;
    if (m_zoomStepSize > 0 && zoomStepSize < 0)  m_zoomOutBaseLevel = currentZoom / 2;
    
    m_zoomStepSize = zoomStepSize;

    if(zoomValue >= m_webView->minZoomLevel() && zoomValue <= m_webView->maxZoomLevel()) {
        if (!m_pinchCenterSet) {
            TPoint pinchCenter = aGesture.PinchEndPos() + aGesture.CurrentPos(); 
            m_pinchCenter.iX = pinchCenter.iX / 2;
            m_pinchCenter.iY = pinchCenter.iY / 2;
            m_pinchCenterSet = true;
            if (zoomStepSize < 0)   m_zoomOutBaseLevel = currentZoom / 2;  
        }
        if (!(zoomStepSize < 0 && zoomValue < m_zoomOutBaseLevel))
            setZoomLevelL(zoomValue);

    }
}

// -----------------------------------------------------------------------------
// handlePinchGestureExitL
// -----------------------------------------------------------------------------
void WebPagePinchZoomHandler::handlePinchGestureExitL(const TStmGestureEvent& aGesture)
{
    m_zoomOutBaseLevel  = 0;
    m_zoomStepSize = 0;
    m_pinchCenterSet = false;
    if (!m_bitmapUpdateTimer->IsActive()) 
        m_bitmapUpdateTimer->Start( KBitmapUpdateTimeout,KBitmapUpdateTimeout,TCallBack(&pinchBitmapUpdateTimerCb,this));
    
    if (!m_pinchExitWaitTimer->IsActive()) 
        m_pinchExitWaitTimer->Start( KPinchExitWaitTimeout,0,TCallBack(&pinchExitWaitTimerCb,this));
    
}

// -----------------------------------------------------------------------------
// setZoomLevelL
// -----------------------------------------------------------------------------
void WebPagePinchZoomHandler::setZoomLevelL(int zoomLevel)
{
    m_webView->setPinchBitmapZoomLevelL(zoomLevel);
}

// -----------------------------------------------------------------------------
// isPinchActive
// -----------------------------------------------------------------------------
TBool WebPagePinchZoomHandler::isPinchActive()
{
  bool pinchActive = false;
  if (m_pinchActive == true || m_pinchExitWaitTimer->IsActive())
      pinchActive = true;
  
  return pinchActive;

}

// -----------------------------------------------------------------------------
// updateBitmap
// -----------------------------------------------------------------------------
void WebPagePinchZoomHandler::updateBitmap(void)
{
    m_bitmapUpdateTimer->Cancel();
    m_webView->restoreZoomLevel(m_webView->scalingFactor());
    //update the plugin rect after pinch zoom exit
    m_webView->mainFrame()->notifyPluginsOfPositionChange();
    m_webView->startCheckerBoardDestroyTimer();
}

// -----------------------------------------------------------------------------
// handlepinchExitWaitTimer
// -----------------------------------------------------------------------------
void WebPagePinchZoomHandler::handlepinchExitWaitTimer(void)
{
    m_pinchExitWaitTimer->Cancel();
}

// -----------------------------------------------------------------------------
// pinchBitmapUpdateTimerCb
// -----------------------------------------------------------------------------
int pinchBitmapUpdateTimerCb(void* ptr)
{
    ((WebPagePinchZoomHandler*)ptr)->updateBitmap();
    return 0;
}

// -----------------------------------------------------------------------------
// pinchExitWaitTimerCb
// -----------------------------------------------------------------------------
int pinchExitWaitTimerCb(void* ptr)
{
    ((WebPagePinchZoomHandler*)ptr)->handlepinchExitWaitTimer();
    return 0;
}

//  End of File


