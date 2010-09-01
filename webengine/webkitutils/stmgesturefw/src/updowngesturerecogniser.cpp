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

#include "updowngesturerecogniser.h"
#include "GenericSimpleGesture.h"
#include "rt_uievent.h"

#include "filelogger.h"

using namespace stmGesture ;

_LIT8(KUpdownName, "Updown") ;

CUpdownGestureRecogniser::CUpdownGestureRecogniser(MGestureListener* aListener) : 
        CGestureRecogniser(aListener)
{
}

CUpdownGestureRecogniser* CUpdownGestureRecogniser::NewL(MGestureListener* aListener)
{
    CUpdownGestureRecogniser* self = new (ELeave) CUpdownGestureRecogniser(aListener) ;
    return self;
}

CUpdownGestureRecogniser::~CUpdownGestureRecogniser()
{
}

TGestureRecognitionState CUpdownGestureRecogniser::recognise(int numOfActiveStreams,
        MGestureEngineIf* pge)
{
    TGestureRecognitionState state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like a tap or double tap
    if (numOfActiveStreams == 1)
    {
        // Then look at the event stream, it has to be tap and release
        const stmUiEventEngine::MUiEvent* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        stmUiEventEngine::TUiEventCode eventCode = puie->Code() ;
        if (countOfEvents > 1) // do we have more than one event in the stream?
        {
            // Then look at the events to see if they are suitable for us
            // should we check that all of the events are targeted to our window?
            // currently we only check if the last one is for us and is EMove, then check if |x| > |y|
            if (puie->Target() == m_powner &&
                    eventCode == stmUiEventEngine::EMove) // The last one is move in our window

            {
                if (m_loggingenabled)
                {
                    LOGARG("CUpdownGestureRecogniser: UpDown: num %d code %d", 
                            countOfEvents, eventCode);
                }
                // Is it leftright gesture in our window?
                const TPoint& p = puie->CurrentXY() ;
                TPoint dp = p - puie->PreviousXY() ;
                // check that the Y movement is bigger
                if (Abs(dp.iX) < Abs(dp.iY))
                {
                    state = EGestureActive;
                    stmGesture::TGenericSimpleGesture pgest(KUid, p, dp.iY, puie) ;
                    // Give the gesture a name
                    pgest.setName(KUpdownName) ;
                    // Call the listener to inform that a UpDown has occurred...
                    m_listener->gestureEnter(pgest) ;
                }
            }
        }
    }
    return state;
}

void CUpdownGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    m_listener->gestureExit(KUid) ;
}

