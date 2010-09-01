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
#include "touchgesturerecogniser.h"
#include "rt_uievent.h"
#include "filelogger.h"

using namespace stmGesture ;

CTouchGestureRecogniser::CTouchGestureRecogniser(MGestureListener* aListener) :
    CGestureRecogniser(aListener)
{
}

CTouchGestureRecogniser* CTouchGestureRecogniser::NewL(MGestureListener* aListener)
{
    CTouchGestureRecogniser* self = new (ELeave) CTouchGestureRecogniser(aListener) ;
    return self;
}

CTouchGestureRecogniser::~CTouchGestureRecogniser()
{
}

/*!
 * Touch gesture recogniser.  Note that this one never owns the gesture, it just calls
 * the callback if it detects ETouch inside the area being watched.
 * There could be also check for the target window?
 */
TGestureRecognitionState CTouchGestureRecogniser::recognise(int numOfActiveStreams,
        MGestureEngineIf* pge)
{
    TGestureRecognitionState state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like edge scroll with one pointer
    if (numOfActiveStreams == 1)
    {
        // Then look at the event stream, it has to be EHold
        const stmUiEventEngine::MUiEvent* puie = pge->getUiEvents(0);
        if (!puie) return state;
        stmUiEventEngine::TUiEventCode eventCode = puie->Code();

        if (m_loggingenabled)
        {
            LOGARG("CTouchGestureRecogniser: 0x%x num %d code %d", this, puie->countOfEvents(), eventCode);

        }
        if (eventCode == stmUiEventEngine::ETouch)
        {
            const TPoint& tapPoint = puie->CurrentXY();
            if (m_loggingenabled)
            {
                LOGARG("CTouchGestureRecogniser: 0x%x ETouch: num %d at %d, %d", this, puie->countOfEvents(), tapPoint.iX, tapPoint.iY);
                LOGARG("CTouchGestureRecogniser: area, %d,%d %d,%d", m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY);
            }
            bool produceGesture ;
            if(!m_area.IsEmpty())
            {
                produceGesture = m_area.Contains(tapPoint);
                if(produceGesture && m_loggingenabled)
                {
                    LOGARG("CTouchGestureRecogniser: HIT area (%d,%d) in %d,%d %d,%d", tapPoint.iX, tapPoint.iY, m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY);
                }
            }
            else
            {
                produceGesture = (m_powner == puie->Target()); // no area defined, touch detected in the window
            }
            if (produceGesture)
            {
                // state = EGestureActive ; do not take ownership, all gestures anyway start with ETouch
                // issue the touch gesture
                stmGesture::TGenericSimpleGesture pgest(KUid, tapPoint);
                // Call the listener to inform that a touch has occurred...
                m_listener->gestureEnter(pgest);
            }
        }
    }
    return state;
}

void CTouchGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    if (m_loggingenabled)
    {
        LOGARG("CTouchGestureRecogniser: 0x%x release", this);
    }
}

void CTouchGestureRecogniser::setArea(const TRect& theArea)
{
    m_area = theArea ;
    if (m_loggingenabled)
    {
        LOGARG("CTouchGestureRecogniser: area, %d,%d %d,%d", m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY);
    }

}
