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
 *
 *  Created on: Oct 19, 2010

 */

// INCLUDE FILES
//#include "config.h"
#include "../../bidi.h"
#include <coemain.h>
#include "WMLWebGestureInterface.h"
#include "MVCView.h"


const TInt TOUCH_AREA_TIMEOUT = 200;
const TInt TOUCH_TIME_AREA_TIMEOUT = 0;
const TInt HOLD_AREA_TIMEOUT = 1500;
const TInt SUPPRESS_TIMEOUT = 0;
const TInt MOVE_SUPPRESS_TIMEOUT = 0;
const TInt DOUBLE_TAP_TIMEOUT = 400;
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
WMLWebGestureInterface* WMLWebGestureInterface::NewL(CView* view)
{
    WMLWebGestureInterface* self = WMLWebGestureInterface::NewLC(view);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CgesturetestAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
WMLWebGestureInterface* WMLWebGestureInterface::NewLC(CView* view)
{
    WMLWebGestureInterface* self = new (ELeave) WMLWebGestureInterface(view);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// WMLWebGestureInterface::WMLWebGestureInterface
// C++ default constructor
//
// -----------------------------------------------------------------------------
//
WMLWebGestureInterface::WMLWebGestureInterface(CView* view)
: m_WMLview(view)
{
}

// -----------------------------------------------------------------------------
// WMLWebGestureInterface::~WMLWebGestureInterface
// -----------------------------------------------------------------------------
WMLWebGestureInterface::~WMLWebGestureInterface()
{
    iGestureContext->Deactivate();
    iGestureContext->RemoveListener(this);
    delete iGestureEngine;
}
// -----------------------------------------------------------------------------
// WMLWebGestureInterface::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void WMLWebGestureInterface::ConstructL()
{
    iGestureEngine = CStmGestureEngine::NewL();
    iGestureContext = iGestureEngine->CreateContextL(TInt(this));
    iGestureContext->SetContext(m_WMLview);
    iGestureContext->AddListenerL(this);

    CStmGestureParameters& gestureParams = iGestureContext->Config();
    //Enable the Gestures needed
    gestureParams.SetEnabled(stmGesture::EGestureUidTouch, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidTap, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidDoubleTap, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidRelease, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidPan, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidFlick, ETrue);
    gestureParams.SetEnabled(stmGesture::EGestureUidLongPress, EFalse);
    gestureParams.SetEnabled(stmGesture::EGestureUidPinch, EFalse);

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
    
    //Double tap functionality is enabled based on ECapabilityFitToScreen capability.
    //setting the doubletap timeout to ZERO if this cap is not defined
    gestureParams[stmGesture::EDoubleTapTimeout   ] = DOUBLE_TAP_TIMEOUT;    

    gestureParams[stmGesture::ESuppressTimeout    ] = SUPPRESS_TIMEOUT;
    gestureParams[stmGesture::EMoveSuppressTimeout] = MOVE_SUPPRESS_TIMEOUT;
    gestureParams[stmGesture::EPanSpeedLow        ] = PAN_SPEED_LOW;
    gestureParams[stmGesture::EPanSpeedHigh       ] = PAN_SPEED_HIGH;

    gestureParams[stmGesture::EEnableFiltering    ] = ETrue;
    //gestureParams[stmGesture::ECapacitiveUpUsed   ] = ETrue;
    iGestureContext->ActivateL();

}

// -----------------------------------------------------------------------------
// HandlePointerEventL
// -----------------------------------------------------------------------------
void WMLWebGestureInterface::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    iGestureEngine->HandlePointerEventL(aPointerEvent, m_WMLview);
}

// -----------------------------------------------------------------------------
// HandleGestureEventL
// -----------------------------------------------------------------------------
void  WMLWebGestureInterface::HandleGestureEventL(const TStmGestureEvent& aGesture)
{
     m_WMLview->HandleGestureEventL(aGesture);
}



