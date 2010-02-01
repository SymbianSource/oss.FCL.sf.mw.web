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
#include "releasegesturerecogniser.h"
#include "rt_uievent.h"
#include "filelogger.h"

using namespace stmGesture ;

_LIT8(KReleaseName, "Release");

CReleaseGestureRecogniser::CReleaseGestureRecogniser(MGestureListener* aListener) :
    CGestureRecogniser(aListener)
{
}

CReleaseGestureRecogniser* CReleaseGestureRecogniser::NewL(MGestureListener* aListener)
{
    CReleaseGestureRecogniser* self = new (ELeave) CReleaseGestureRecogniser(aListener) ;
    return self;
}

CReleaseGestureRecogniser::~CReleaseGestureRecogniser()
{
}

/*!
 * Release gesture recogniser.  Note that this one never owns the gesture, it just calls
 * the callback if it detects ERelease inside the area being watched.
 * There could be also check for the target window?
 */
TGestureRecognitionState CReleaseGestureRecogniser::recognise(int numOfActiveStreams,
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
        
        int countOfEvents = puie->countOfEvents();
        stmUiEventEngine::TUiEventCode eventCode = puie->Code();

        if (m_loggingenabled)
        {
            LOGARG("CReleaseGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
        if (eventCode == stmUiEventEngine::ERelease)
        {
            if (m_loggingenabled)
            {
                LOGARG("CReleaseGestureRecogniser: 0x%x ERelease: num %d code %d, %d", 
                        this, countOfEvents, puie->CurrentXY().iX, puie->CurrentXY().iY);
                LOGARG("CReleaseGestureRecogniser: area, %d,%d %d,%d", 
                        m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY);
            }
            bool produceGesture ;
            if(!m_area.IsEmpty())
            {
                produceGesture = m_area.Contains(puie->CurrentXY()) ;
                if(produceGesture && m_loggingenabled)
                {
                    LOGARG("CReleaseGestureRecogniser: HIT area (%d,%d) in %d,%d %d,%d", 
                            puie->CurrentXY().iX, puie->CurrentXY().iY, 
                            m_area.iTl.iX, m_area.iTl.iY, 
                            m_area.iBr.iX, m_area.iBr.iY);
                }
            }
            else
            {
                produceGesture = (m_powner == puie->Target()) ;  // no area defined, touch detected in the window
            }
            if (produceGesture)
            {
                state = EGestureActive ;
                // issue the release gesture using the GenericSimpleGesture
                stmGesture::TGenericSimpleGesture pgest(KUid, puie->CurrentXY());
                // Give the gesture a name
                pgest.setName(KReleaseName) ;
                // Call the listener to inform that a release has occurred...
                m_listener->gestureEnter(pgest);
            }
        }
    }
    return state;
}

void CReleaseGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    if (m_loggingenabled)
    {
        LOGARG("CReleaseGestureRecogniser: 0x%x release", this);
    }
}

void CReleaseGestureRecogniser::setArea(const TRect& theArea)
{
    m_area = theArea ;
    if (m_loggingenabled)
    {
        LOGARG("CReleaseGestureRecogniser: area, %d,%d %d,%d", 
                m_area.iTl.iX, m_area.iTl.iY, 
                m_area.iBr.iX, m_area.iBr.iY);
    }
}

