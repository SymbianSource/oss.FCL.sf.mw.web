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
* Description:  Gesture event sender implementation
*
*/

#include "gesturehelpereventsender.h"

CGestureEventSender* CGestureEventSender::NewL( MStmGestureListener& aObserver )
    {
    CGestureEventSender* self = new (ELeave) CGestureEventSender(aObserver);
    
    return self;
    }

CGestureEventSender::CGestureEventSender( MStmGestureListener& aObserver ) 
    :iObserver(aObserver)
    { 
    }

CGestureEventSender::~CGestureEventSender()
    {
    }

void CGestureEventSender::AddEvent(TStmGestureUid aUid, const MStmGesture* aGestureEvent)
{
    TStmGestureEvent event;
    if (aGestureEvent) {
        event.SetCode(aGestureEvent->gestureUid());
        event.SetCurrentPos(aGestureEvent->getLocation());
        event.SetSpeed(TRealPoint(aGestureEvent->getSpeedX()*1000,aGestureEvent->getSpeedY()*1000));//App needs the speed in pixels/sec
        event.SetType(TStmTapType(aGestureEvent->getType()));
        event.SetDetails(aGestureEvent->getDetails());
        event.SetGestureDirection(aGestureEvent->getDirection());
        event.SetPinchEndPos(aGestureEvent->getPinchEndPos());
        event.SetGestureState(EGestureEnter);
    }
    else {
        if (aUid != stmGesture::EGestureUidPinch) {
            return;
        }    
        event.SetCode(aUid);
        event.SetGestureState(EGestureExit);
    }
        
    EmitEventL(event);
    if (event.Code() == stmGesture::EGestureUidTap) {
        event.SetCode(stmGesture::EGestureUidRelease);
        EmitEventL(event);
    }
}

void CGestureEventSender::EmitEventL( const TStmGestureEvent& aGesture )
{
    iObserver.HandleGestureEventL(aGesture);
}
