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
#include "unknowngesturerecogniser.h"
#include "rt_uievent.h"
#include "filelogger.h"

using namespace stmGesture ;

CUnknownGestureRecogniser::CUnknownGestureRecogniser(MGestureListener* aListener) :
    CGestureRecogniser(aListener)
{
}

CUnknownGestureRecogniser* CUnknownGestureRecogniser::NewL(MGestureListener* aListener)
{
    CUnknownGestureRecogniser* self = new (ELeave) CUnknownGestureRecogniser(aListener) ;
    return self;
}

CUnknownGestureRecogniser::~CUnknownGestureRecogniser()
{
}

/*!
 * recognise the long press; basically it is just the EHold UI event
 */
TGestureRecognitionState CUnknownGestureRecogniser::recognise(int numOfActiveStreams,
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
        int countOfEvents = puie->countOfEvents();
        stmUiEventEngine::TUiEventCode eventCode = puie->Code();

        if (m_loggingenabled)
        {
            LOGARG("CUnknownGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
        if (puie->Target() == m_powner && eventCode == stmUiEventEngine::ERelease) // The last one is ERelease
        {
            const TPoint& currentXY = puie->CurrentXY() ;
            if (m_loggingenabled)
            {
                LOGARG("CUnknownGestureRecogniser: (%d, %d) ", currentXY.iX, currentXY.iY) ;
            }
            state = EGestureActive ;
            // issue the gesture
            stmGesture::TGenericSimpleGesture pgest(KUid, currentXY, 0, puie) ;
            // Call the listener to inform that a gesture has occurred...
            m_listener->gestureEnter(pgest) ;
        }
    }
    return state;
}

void CUnknownGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    if (m_loggingenabled)
    {
        LOGARG("CUnknownGestureRecogniser: 0x%x release", this);
    }
}

