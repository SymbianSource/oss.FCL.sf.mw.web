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
#include "edgescrollgesturerecogniser.h"
#include "rt_uievent.h"
#include "filelogger.h"

using namespace stmGesture ;

CEdgeScrollGestureRecogniser::CEdgeScrollGestureRecogniser(MGestureListener* aListener) :
    CPeriodic(0), m_listener(aListener) , m_area()
{
    m_powner = aListener->getOwner() ;
    m_gestureEnabled = true ;
    m_rangesizeInPixels = 10 ;  // by default 10 pixels from the edges is the area
}

CEdgeScrollGestureRecogniser* CEdgeScrollGestureRecogniser::NewL(MGestureListener* aListener)
{
    CEdgeScrollGestureRecogniser* self = new (ELeave) CEdgeScrollGestureRecogniser(aListener) ;
    CleanupStack::PushL(self);
    self->ConstructL(); // construct base class
    CActiveScheduler::Add(self);
    CleanupStack::Pop(self);
    return self;
}

CEdgeScrollGestureRecogniser::~CEdgeScrollGestureRecogniser()
{
    Cancel();
}

TGestureRecognitionState CEdgeScrollGestureRecogniser::recognise(int numOfActiveStreams,
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
        int countOfEvents = puie->countOfEvents();
        stmUiEventEngine::TUiEventCode eventCode = puie->Code();

        if (m_loggingenabled)
        {
            LOGARG("CEdgeScrollGestureRecogniser: %d num %d code %d", 
                    eventCode, countOfEvents, eventCode);
        }

        if (eventCode == stmUiEventEngine::EHold) // The last one is EHold, look if it is near the area defined
        {
            const TPoint& p = puie->CurrentXY();
            if (m_loggingenabled)
            {
                LOGARG("CEdgeScrollGestureRecogniser: 0x%x EHold: num %d code %d, %d", 
                        this, countOfEvents, p.iX, p.iY);
                LOGARG("CEdgeScrollGestureRecogniser: area, %d,%d %d,%d, range: %d", 
                        m_area.iTl.iX, 
                        m_area.iTl.iY, 
                        m_area.iBr.iX, 
                        m_area.iBr.iY, 
                        m_rangesizeInPixels);
            }
            // check where the point is inside the area defined but outside of the area shrinked by m_rangesizeInPixels.
            TRect rcInner = m_area ;
            rcInner.Shrink(m_rangesizeInPixels, m_rangesizeInPixels) ;
            if (m_area.Contains(p) && !rcInner.Contains(p))
            {
                if (m_loggingenabled)
                {
                    LOGARG("CEdgeScrollGestureRecogniser: HIT, (%d,%d) in %d,%d %d,%d, range: %d", 
                            p.iX, p.iY, m_area.iTl.iX, m_area.iTl.iY, 
                            m_area.iBr.iX, m_area.iBr.iY, 
                            m_rangesizeInPixels);
                }

                state = EGestureActive ;
                // this is edge scroll, check where it is about...
                TEdgeScroll scrolltype = EEdgeScrollUnknown ;
                if (p.iY < m_area.iTl.iY + m_rangesizeInPixels)
                    scrolltype = EEdgeScrollUp ;   // if Y is small enough, it is always up
                else if (p.iY > m_area.iBr.iY - m_rangesizeInPixels)
                    scrolltype = EEdgeScrollDown ; // if Y is big enough, it is always down
                else if (p.iX < m_area.iTl.iX + m_rangesizeInPixels)
                    scrolltype = EEdgeScrollLeft ; // if X is small enough, it is always left
                else  if (p.iX > m_area.iBr.iX - m_rangesizeInPixels)
                    scrolltype = EEdgeScrollRight ;   // if X is big enough, it is always right
                // issue the edge scroll gesture
                stmGesture::TGenericSimpleGesture pgest(KUid, p, scrolltype, puie) ;
                // Call the listener to inform that a Tap has occurred...
                m_listener->gestureEnter(pgest) ;
            }
        }
    }
    return state;
}

void CEdgeScrollGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    Cancel() ;  // some other gesture took hold of the thing, do not send gesture
    if (m_loggingenabled)
    {
        LOGARG("CEdgeScrollGestureRecogniser: 0x%x release", this);
    }
}

void CEdgeScrollGestureRecogniser::RunL()
{
    if (m_loggingenabled)
    {
        LOGARG("CEdgeScrollGestureRecogniser: 0x%x timer", this);
    }
}

void CEdgeScrollGestureRecogniser::enableLogging(bool loggingOn)
{
    m_loggingenabled = loggingOn;
}

void CEdgeScrollGestureRecogniser::enable(bool enabled)
{
    m_gestureEnabled = enabled ;
}

bool CEdgeScrollGestureRecogniser::isEnabled()
{
    return m_gestureEnabled ;
}

void CEdgeScrollGestureRecogniser::setOwner(CCoeControl* owner)
{
    m_powner = owner;
}

void CEdgeScrollGestureRecogniser::setScrollRange(int rangeInPixels)
{
    m_rangesizeInPixels = rangeInPixels ;
}

void CEdgeScrollGestureRecogniser::setArea(const TRect& theArea)
{
    m_area = theArea ;
    if (m_loggingenabled)
    {
        LOGARG("CEdgeScrollGestureRecogniser: set area, %d,%d %d,%d", m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY);
    }
}
