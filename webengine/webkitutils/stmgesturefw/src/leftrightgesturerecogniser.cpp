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

#include "leftrightgesturerecogniser.h"
#include "GenericSimpleGesture.h"
#include "rt_uievent.h"

#include "filelogger.h"

using namespace stmGesture ;

_LIT8(KLeftrightName, "Leftlight");

CLeftrightGestureRecogniser::CLeftrightGestureRecogniser(MGestureListener* aListener) : 
        CGestureRecogniser(aListener)
{
}

CLeftrightGestureRecogniser* CLeftrightGestureRecogniser::NewL(MGestureListener* aListener)
{
    CLeftrightGestureRecogniser* self = new (ELeave) CLeftrightGestureRecogniser(aListener) ;
    return self;
}

CLeftrightGestureRecogniser::~CLeftrightGestureRecogniser()
{
}

TGestureRecognitionState CLeftrightGestureRecogniser::recognise(int numOfActiveStreams,
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
                    LOGARG("CLeftrightGestureRecogniser: Leftright: num %d code %d", 
                            countOfEvents, eventCode);
                }
                // Is it leftright gesture in our window?
                const TPoint& p = puie->CurrentXY();
                TPoint dp = p - puie->PreviousXY();
                if (Abs(dp.iX) > Abs(dp.iY))
                {
                    state = EGestureActive;
                    stmGesture::TGenericSimpleGesture pgest(KUid, p, dp.iX, puie) ;
                    pgest.setName(KLeftrightName) ;
                    // Call the listener to inform that a Leftright has occurred...
                    m_listener->gestureEnter(pgest) ;
                }
            }
        }
    }
    return state;
}

void CLeftrightGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    m_listener->gestureExit(KUid) ;
}

