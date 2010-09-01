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

#include "pangesturerecogniser.h"
#include "GenericSimpleGesture.h"
#include "rt_uievent.h"

#include "filelogger.h"

using namespace stmGesture ;

CPanGestureRecogniser::CPanGestureRecogniser(MGestureListener* aListener) : 
        CGestureRecogniser(aListener)
{
}

CPanGestureRecogniser* CPanGestureRecogniser::NewL(MGestureListener* aListener)
{
    CPanGestureRecogniser* self = new (ELeave) CPanGestureRecogniser(aListener) ;
    return self;
}

CPanGestureRecogniser::~CPanGestureRecogniser()
{
}

TGestureRecognitionState CPanGestureRecogniser::recognise(int numOfActiveStreams,
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
        if (!puie) return state;
        int countOfEvents = puie->countOfEvents() ;
        stmUiEventEngine::TUiEventCode eventCode = puie->Code() ;
        if (countOfEvents > 1) // do we have more than one event in the stream?
        {
            // Then look at the events to see if they are suitable for us
            // should we check that all of the events are targeted to our window?
            // currently we only check if the last one is for us and is EMove, then we pan if the speed is OK
            if (puie->Target() == m_powner &&
                eventCode == stmUiEventEngine::EMove) // The last one is move in our window

            {
                if (m_loggingenabled)
                {
                    LOGARG("CPanGestureRecogniser: Pan: num %d code %d", countOfEvents, eventCode);
                }
                float speedX = puie->speedX();
                float speedY = puie->speedY();

                using stmUiEventEngine::TUiEventSpeed;

                state = EGestureActive;
                TUiEventSpeed speedIf(speedX, speedY);
                // Panning gesture
                stmGesture::TDirectionalGesture pgest(
                                 KUid,
                                 puie->CurrentXY(),
                                 puie->PreviousXY(),
                                 &speedIf,
                                 m_loggingenabled);

               // Call the listener to inform that a Pan has occurred...
               m_listener->gestureEnter(pgest);
            }
        }
    }
    return state;
}

void CPanGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    m_listener->gestureExit(KUid) ;
}

void CPanGestureRecogniser::setPanningSpeedLow(float aSpeed) __SOFTFP
{
    m_panningspeedlow = aSpeed ;
}

void CPanGestureRecogniser::setPanningSpeedHigh(float aSpeed) __SOFTFP
{
    m_panningspeedhigh = aSpeed ;
}

