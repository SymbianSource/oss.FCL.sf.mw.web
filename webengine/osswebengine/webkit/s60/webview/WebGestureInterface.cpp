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
#include <Browser_platform_variant.hrh>
#include "config.h"
#include "../../bidi.h"
#include <coemain.h>
#include "brctl.h"
#include <brctldefs.h>
#include "WebGestureInterface.h"
#include "WebView.h"
#include "WebPointerEventHandler.h"

const TInt TOUCH_AREA_TIMEOUT = 200;
const TInt TOUCH_TIME_AREA_TIMEOUT = 0;
const TInt HOLD_AREA_TIMEOUT = 1500;
const TInt DOUBLE_TAP_TIMEOUT = 400;
const TInt SUPPRESS_TIMEOUT = 0;
const TInt MOVE_SUPPRESS_TIMEOUT = 0;

const TInt TOUCH_TIME_AREA_WIDTH = 4;
const TInt TOUCH_AREA_WIDTH = 4;
const TInt HOLD_AREA_WIDTH = 4;

const TInt PAN_SPEED_LOW = 0;
const TInt PAN_SPEED_HIGH = 400;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CgesturetestAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
WebGestureInterface* WebGestureInterface::NewL(WebView* view)
{
    WebGestureInterface* self = WebGestureInterface::NewLC(view);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CgesturetestAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
WebGestureInterface* WebGestureInterface::NewLC(WebView* view)
{
    WebGestureInterface* self = new (ELeave) WebGestureInterface(view);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// WebGestureInterface::WebGestureInterface
// C++ default constructor
//
// -----------------------------------------------------------------------------
//
WebGestureInterface::WebGestureInterface(WebView* view)
: m_webview(view)
{
}

// -----------------------------------------------------------------------------
// WebGestureInterface::~WebGestureInterface
// -----------------------------------------------------------------------------
WebGestureInterface::~WebGestureInterface()
{
    iGestureContext->Deactivate();
    iGestureContext->RemoveListener(this);
    delete iGestureEngine;
}
// -----------------------------------------------------------------------------
// WebGestureInterface::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void WebGestureInterface::ConstructL()
{
    iGestureEngine = CStmGestureEngine::NewL();
    iGestureContext = iGestureEngine->CreateContextL(TInt(this));
    iGestureContext->SetContext(m_webview);
    iGestureContext->AddListenerL(this);

    CStmGestureParameters& gestureParams = iGestureContext->Config();
    //Enable the Gestures needed
    gestureParams.SetEnabled(stmGesture::EGestureUidTouch, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidTap, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidRelease, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidPan, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidFlick, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidLongPress, ETrue);
#ifdef BRDO_MULTITOUCH_ENABLED_FF
    if (m_webview->brCtl()->capabilities() & TBrCtlDefs::ECapabilityPinchZoom) {
        gestureParams.SetEnabled(stmGesture::EGestureUidPinch, ETrue);
    }
    else {
        gestureParams.SetEnabled(stmGesture::EGestureUidPinch, EFalse);
    }
#else
    gestureParams.SetEnabled(stmGesture::EGestureUidPinch, EFalse);
#endif

    //Set other parameters

    TStmGestureArea& touchTimeArea = *gestureParams.Area(stmGesture::ETouchTimeArea);
    TStmGestureArea& touchArea = *gestureParams.Area(stmGesture::ETouchArea);
    TStmGestureArea& holdArea  = *gestureParams.Area(stmGesture::EHoldArea);

    touchTimeArea.iShape = TStmGestureArea::ERectangle;
    touchTimeArea.iTimeout =  TOUCH_TIME_AREA_TIMEOUT;
    touchTimeArea.iSize.iWidth = TOUCH_TIME_AREA_WIDTH;

    touchArea.iShape = TStmGestureArea::ERectangle;
    touchArea.iTimeout =  TOUCH_AREA_TIMEOUT;
    touchArea.iSize.iWidth = TOUCH_AREA_WIDTH;

    holdArea.iShape = TStmGestureArea::ERectangle;
    holdArea.iTimeout =  HOLD_AREA_TIMEOUT;
    holdArea.iSize.iWidth = HOLD_AREA_WIDTH;

    gestureParams[stmGesture::EDoubleTapTimeout   ] = DOUBLE_TAP_TIMEOUT;
    gestureParams[stmGesture::ESuppressTimeout    ] = SUPPRESS_TIMEOUT;
    gestureParams[stmGesture::EMoveSuppressTimeout] = MOVE_SUPPRESS_TIMEOUT;
    gestureParams[stmGesture::EPanSpeedLow        ] = PAN_SPEED_LOW;
    gestureParams[stmGesture::EPanSpeedHigh       ] = PAN_SPEED_HIGH;

    gestureParams[stmGesture::EEnableFiltering    ] = ETrue;
#ifdef BRDO_MULTITOUCH_ENABLED_FF
    gestureParams[stmGesture::ECapacitiveUpUsed   ] = ETrue;
#ifndef __WINSCW__    
    gestureParams[stmGesture::EAdjustYPos         ] = ETrue;
#endif    
#else
    gestureParams[stmGesture::ECapacitiveUpUsed   ] = EFalse;
    gestureParams[stmGesture::EAdjustYPos         ] = EFalse;
#endif
    iGestureContext->ActivateL();

}

// -----------------------------------------------------------------------------
// HandlePointerEventL
// -----------------------------------------------------------------------------
void WebGestureInterface::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    iGestureEngine->HandlePointerEventL(aPointerEvent, m_webview);
}

// -----------------------------------------------------------------------------
// HandleGestureEventL
// -----------------------------------------------------------------------------
void  WebGestureInterface::HandleGestureEventL(const TStmGestureEvent& aGesture)
{
     m_webview->pointerEventHandler()->HandleGestureEventL(aGesture);
}



