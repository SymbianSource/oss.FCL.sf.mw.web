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
#include "longpressgesturerecogniser.h"
#include "rt_uievent.h"
#include "filelogger.h"

using namespace stmGesture ;

CLongPressGestureRecogniser::CLongPressGestureRecogniser(MGestureListener* aListener) :
    CGestureRecogniser(aListener)
{
}

CLongPressGestureRecogniser* CLongPressGestureRecogniser::NewL(MGestureListener* aListener)
{
    CLongPressGestureRecogniser* self = new (ELeave) CLongPressGestureRecogniser(aListener) ;
    return self;
}

CLongPressGestureRecogniser::~CLongPressGestureRecogniser()
{
}

/*!
 * recognise the long press; basically it is just the EHold UI event
 */
TGestureRecognitionState CLongPressGestureRecogniser::recognise(int numOfActiveStreams,
        MGestureEngineIf* pge)
{
    TGestureRecognitionState state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;


    // Look at the events to see if it looks like long press with one pointer
    if (numOfActiveStreams == 1)
    {
        // Then look at the event stream, it has to be EHold
        const stmUiEventEngine::MUiEvent* puie = pge->getUiEvents(0);
        if (!puie) return state;
        int countOfEvents = puie->countOfEvents();
        stmUiEventEngine::TUiEventCode eventCode = puie->Code();

        if (m_loggingenabled)
        {
            LOGARG("CLongPressGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
        if (puie->Target() == m_powner && eventCode == stmUiEventEngine::EHold) // The last one is EHold, look if it is near our borders
        {
            const TPoint& p = puie->CurrentXY() ;
            if (m_loggingenabled)
            {
                LOGARG("CLongPressGestureRecogniser: (%d, %d) in (%d,%d)(%d,%d)", p.iX, p.iY,
                        m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY);
            }
             state = EGestureActive ;
             // issue the long press gesture
             stmGesture::TGenericSimpleGesture pgest(KUid, p, 0, puie) ; // TODO: speed is 0?
             // Call the listener to inform that the gesture has occurred...
             m_listener->gestureEnter(pgest) ;
        }
    }
    return state;
}

void CLongPressGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    if (m_loggingenabled)
    {
        LOGARG("CLongPressGestureRecogniser: 0x%x release", this);
    }
}

void CLongPressGestureRecogniser::setArea(const TRect& theArea)
{
    m_area = theArea ;
}
