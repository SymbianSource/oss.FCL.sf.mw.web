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
#include "gesture.h"
#include "gestureevent.h"

using namespace RT_GestureHelper;

const TInt KEventsQueueGranularity = 4;

CGestureEventSender* CGestureEventSender::NewL( MGestureObserver& aObserver )
    {
    CGestureEventSender* self = new (ELeave) CGestureEventSender(aObserver);
    
    return self;
    }

CGestureEventSender::CGestureEventSender( MGestureObserver& aObserver ) : 
                                          CActive(EPriorityUserInput - 1),
                                          iObserver(aObserver), iEvents(KEventsQueueGranularity)
    { 
    CActiveScheduler::Add(this);
    }

CGestureEventSender::~CGestureEventSender()
    {
    iEvents.Close();
    iEvents.ResetAndDestroy();
    }

TInt CGestureEventSender::AddEvent(const CGestureEvent* aGestureEvent)
    {
    iEvents.Append(aGestureEvent);
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
                    CGestureEvent* gst = iEvents[i];
                    EmitEventL(*gst);
                    delete gst;
                    }
                }
            iEvents.Reset();
            iState = ENoEvents;
            Complete();
            break;
            }
        }
    }


void CGestureEventSender::EmitEventL( const CGestureEvent& aGesture )
    {
    iState = EBusy;
    iObserver.HandleGestureL(aGesture);
    iState = EEventsReady;
    }

TInt CGestureEventSender::RunError(TInt aError)
    {
    return aError;
    }

void CGestureEventSender::DoCancel()
    {
    iEvents.Reset();
    iState = ENoEvents;
    }
