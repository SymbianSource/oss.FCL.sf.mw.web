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

#include "hoveringgesturerecogniser.h"
#include "GenericSimpleGesture.h"
#include "rt_uievent.h"

#include "filelogger.h"

using namespace stmGesture ;

extern long Mm2Pixels(long mm) ;


CHoveringGestureRecogniser::CHoveringGestureRecogniser(MGestureListener* aListener) :
    CGestureRecogniser(aListener)
{
    m_hovering = false ;
    m_hoveringspeed = 0.9f ;
}

CHoveringGestureRecogniser* CHoveringGestureRecogniser::NewL(MGestureListener* aListener)
{
    CHoveringGestureRecogniser* self = new (ELeave) CHoveringGestureRecogniser(aListener) ;
    return self;
}

CHoveringGestureRecogniser::~CHoveringGestureRecogniser()
{
}

TGestureRecognitionState CHoveringGestureRecogniser::recognise(int numOfActiveStreams,
        MGestureEngineIf* pge)
{
    TGestureRecognitionState state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like hovering
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
            // currently we only check if the last one is for us and is EMove, then we pan...
            if (puie->Target() == m_powner &&
                    eventCode == stmUiEventEngine::EMove) // The last one is move in our window
            {
                float speedX = puie->speedX() ;
                float speedY = puie->speedY() ;
                if (m_loggingenabled)
                {
                    LOGARG("CHoveringGestureRecogniser: %d: num %d code %d, speed %f, limit %f",
                        m_hovering, countOfEvents, eventCode, 
                        double(speedX), double(m_hoveringspeed));
                }
                // It might be hovering gesture in our window, handle it
                if (!m_hovering)
                {
                    // we are not yet hovering, so lets see if it is slow movement
                    // but it must be movement; if it is 0.0 do not hover
                    if ( ((speedX > 0.01f) ||(speedY > 0.01f)) && 
                            ((speedX < m_hoveringspeed) || (speedY < m_hoveringspeed))
                        )
                    {
                        state = EGestureActive;
                        m_hovering = true;
                    }
                }
                if (m_hovering)
                {
                    // after we've started hovering, the speed could be increased a little before we loose hovering
                    // but this adjustment is not implemented now...
                    if (speedX < m_hoveringspeed || speedY < m_hoveringspeed)

                    {
                        using stmUiEventEngine::TUiEventSpeed;

                        state = EGestureActive;
                        TUiEventSpeed speedIf(speedX, speedY);
                        stmGesture::TDirectionalGesture pgest(
                                        KUid,
                                        puie->CurrentXY(),
                                        puie->PreviousXY(),
                                        &speedIf,
                                        m_loggingenabled);

                        // Call the listener to inform that a Hover has occurred...
                        m_listener->gestureEnter(pgest);
                    }
                }
            }
            else if (m_hovering)
            {
                if (eventCode == stmUiEventEngine::ERelease) // The last one is release in any window
                {
                    m_hovering = false ;
                    // release will handle informing of the listener
                }
            }
        }
        else
        {
            // count of events == 1, lets see if it is EMove, then we take it and start hovering
            if (puie->Target() == m_powner &&
                    eventCode == stmUiEventEngine::EMove) // The only one is move in our window
            {
                if (m_loggingenabled)
                {
                    LOGARG("CHoveringGestureRecogniser: move: num %d code %d", countOfEvents, eventCode);
                }
                state = EGestureActive;
                stmGesture::TDirectionalGesture pgest(
                                        KUid,
                                        puie->CurrentXY(),
                                        puie->PreviousXY(),
                                        puie,
                                        m_loggingenabled);

                // Call the listener to inform that a Hover has occurred...
                m_listener->gestureEnter(pgest);
            }
        }
    }
    if (state == ENotMyGesture)
    {
        // if it was not our gesture, then the state can not be hovering...
        m_hovering = false ;
    }
    return state;
}

void CHoveringGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    m_hovering = false ;
    m_listener->gestureExit(KUid) ;
}

void CHoveringGestureRecogniser::setHoveringSpeed(float aSpeed) __SOFTFP
{
    m_hoveringspeed = aSpeed ;
}

