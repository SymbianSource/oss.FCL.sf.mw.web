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

#include "GenericSimpleGesture.h"
#include "flickgesturerecogniser.h"
#include "rt_uievent.h"
#include "filelogger.h"
//#include "flogger.h"

using namespace stmGesture ;

CFlickGestureRecogniser::CFlickGestureRecogniser(MGestureListener* aListener) :
    CGestureRecogniser(aListener)
{
}

CFlickGestureRecogniser* CFlickGestureRecogniser::NewL(MGestureListener* aListener)
{
    CFlickGestureRecogniser* self = new (ELeave) CFlickGestureRecogniser(aListener) ;
    return self;
}

CFlickGestureRecogniser::~CFlickGestureRecogniser()
{
}
/*!
 * Release gesture recogniser.  Note that this one never owns the gesture, it just calls
 * the callback if it detects ERelease inside the area being watched.
 * There could be also check for the target window?
 */
TGestureRecognitionState CFlickGestureRecogniser::recognise(int numOfActiveStreams,
        MGestureEngineIf* pge)
{
    TGestureRecognitionState state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like flick with one pointer
    if (numOfActiveStreams == 1)
    {
        const stmUiEventEngine::MUiEvent* puie = pge->getUiEvents(0);
        if (!puie) return state;
        int countOfEvents = puie->countOfEvents();
        stmUiEventEngine::TUiEventCode eventCode = puie->Code();
        if (m_loggingenabled)
        {
            LOGARG("CFlickGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);

        }
        if (m_powner == puie->Target() && eventCode == stmUiEventEngine::ERelease)
        {
            if (m_loggingenabled)
            {
                LOGARG("CFlickGestureRecogniser: 0x%x ERelease: num %d code %d, %d", this, countOfEvents, puie->CurrentXY().iX, puie->CurrentXY().iY);
            }
            // Check if the speed before release was fast enough for flick
            const stmUiEventEngine::MUiEvent* puieprev = puie->previousEvent() ;
            if (puieprev && puieprev->Code() == stmUiEventEngine::EMove)
            {
                using stmUiEventEngine::TUiEventSpeed;

                float thespeedX = puieprev->speedX() ;
                float thespeedY = puieprev->speedY() ;
                if (m_loggingenabled)
                {
                    LOGARG("CFlickGestureRecogniser: prev speed: %f (limit: %f)", double(thespeedX), double(m_speed)) ;
                }
                if (Abs( thespeedX) >= m_speed || Abs( thespeedY) >= m_speed )
                {
                    state = EGestureActive ;

                    TUiEventSpeed speedIf(thespeedX, thespeedY);

                    // issue the flick gesture using the TDirectionalGesture (it has the speed and direction)
                    stmGesture::TDirectionalGesture pgest(
                            KUid,
                            puieprev->CurrentXY(),
                            puieprev->PreviousXY(),
                            &speedIf,
                            m_loggingenabled);

                    // Call the listener to inform that a flick has occurred...
                    m_listener->gestureEnter(pgest);
                }
            }
        }
    }
    return state;
}
void CFlickGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    if (m_loggingenabled)
    {
        LOGARG("CFlickGestureRecogniser: 0x%x flick", this);
    }
}
void CFlickGestureRecogniser::setFlickingSpeed(float aSpeed) __SOFTFP
{
    m_speed = aSpeed ;
}
