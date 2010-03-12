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
* Description:  Gesture helper implementation
*
*/

#include "gesturehelpereventsender.h"

const TInt KEventsQueueGranularity = 4;

CGestureEventSender* CGestureEventSender::NewL( MStmGestureListener& aObserver )
    {
    CGestureEventSender* self = new (ELeave) CGestureEventSender(aObserver);
    
    return self;
    }

CGestureEventSender::CGestureEventSender( MStmGestureListener& aObserver ) : 
                                          CActive(EPriorityUserInput - 1),
                                          iObserver(aObserver), iEvents(KEventsQueueGranularity)
    { 
    CActiveScheduler::Add(this);
    }

CGestureEventSender::~CGestureEventSender()
    {
    if (IsActive()) 
        {
        Cancel();
        }

    iEvents.Close();
    }

TInt CGestureEventSender::AddEvent(TStmGestureUid aUid, const MStmGesture* aGestureEvent)
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
            return iEvents.Count();
        }    
        event.SetCode(aUid);
        event.SetGestureState(EGestureExit);
    }
        
    iEvents.Append(event);
    
    if (event.Code() == stmGesture::EGestureUidTap) {
        event.SetCode(stmGesture::EGestureUidRelease);
        iEvents.Append(event);
    }

    if (iState != EBusy)
        {
        iState = EEventsReady;
        }
    
    if (!IsActive()) 
        {
        Complete();
        }
    return iEvents.Count();
    }

void CGestureEventSender::Complete()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CGestureEventSender::RunL()
    {
    switch (iState)
        {
        case EEventsReady:
            {
            if (iEvents.Count() > 0)
                {
                TInt count = iEvents.Count();
                for (int i = 0;  i < count; i++)
                    {
                    TStmGestureEvent& gst = iEvents[i];
                    EmitEventL(gst);
                    }
                }
            iEvents.Reset();
            iState = ENoEvents;
            break;
            }
        }
    }

void CGestureEventSender::EmitEventL( const TStmGestureEvent& aGesture )
    {
    iState = EBusy;
    iObserver.HandleGestureEventL(aGesture);
    iState = EEventsReady;
    }

TInt CGestureEventSender::RunError(TInt aError)
    {
    iEvents.Reset();
    return aError;
    }

void CGestureEventSender::DoCancel()
    {
    iEvents.Reset();
    iState = ENoEvents;
    }
