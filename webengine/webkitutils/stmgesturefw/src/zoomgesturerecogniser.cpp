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

#include <e32math.h>
#include <rt_uievent.h>
#include "filelogger.h"
#include "zoomgesturerecogniser.h"
#include "GenericSimpleGesture.h"

using namespace stmGesture ;

_LIT8(KZoomInName, "ZoomIn") ;
_LIT8(KZoomOutName, "ZoomOut") ;

CZoomGestureRecogniser* CZoomGestureRecogniser::NewL(MGestureListener* aListener)
{
    CZoomGestureRecogniser* self = new (ELeave) CZoomGestureRecogniser(aListener) ;
    return self;
}

CZoomGestureRecogniser::CZoomGestureRecogniser(MGestureListener* aListener) :
    CGestureRecogniser(aListener), m_area(TRect::EUninitialized)
{
    m_rangesizeInPixels = 20 ;  // by default 20 pixels from the edges is the area
    m_zooming = false ;
}

CZoomGestureRecogniser::~CZoomGestureRecogniser()
{
}

TGestureRecognitionState CZoomGestureRecogniser::recognise(int numOfActiveStreams,
        MGestureEngineIf* pge)
{
    TGestureRecognitionState state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like zoom with one pointer
    if (numOfActiveStreams == 1)
    {
        const stmUiEventEngine::MUiEvent* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents();
        stmUiEventEngine::TUiEventCode eventCode = puie->Code();

        if (m_loggingenabled)
        {
            LOGARG("CZoomGestureRecogniser: %d num %d code %d",
                    eventCode, countOfEvents, eventCode);

        }
        if (!m_zooming)
        {
            // We are not yet zoomin, check if we should start, i.e. whether we are
            // inside the zoom trigger areas near the corners when touch is done...
            if (eventCode == stmUiEventEngine::ETouch) // It must be ETouch inside the corner
            {
                if (m_loggingenabled)
                {
                    LOGARG("CZoomGestureRecogniser: 0x%x ETouch: num %d code %d, %d",
                            this, countOfEvents, puie->CurrentXY().iX, puie->CurrentXY().iY);
                    LOGARG("CZoomGestureRecogniser: area, %d,%d %d,%d, range: %d", m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY, m_rangesizeInPixels);
                }

                // the lower left rectangle is m_rangesizeIPixels from the corner
                TRect lowerleft(m_area.iTl.iX, m_area.iBr.iY-m_rangesizeInPixels,
                        m_area.iTl.iX+m_rangesizeInPixels, m_area.iBr.iY) ;
                TRect upperright(m_area.iBr.iX-m_rangesizeInPixels, m_area.iTl.iY,
                        m_area.iBr.iX, m_area.iTl.iY+m_rangesizeInPixels) ;

                m_previoustouch = m_startingtouch = puie->CurrentXY() ;
                m_delta = 0 ;
                if (lowerleft.Contains(m_startingtouch) || upperright.Contains(m_startingtouch))
                {
                    if (m_loggingenabled)
                    {
                        LOGARG("CZoomGestureRecogniser: ZOOM IN/OUT, (%d,%d) in %d,%d %d,%d, range: %d",
                            m_startingtouch.iX, m_startingtouch.iY,
                            m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY,
                            m_rangesizeInPixels);
                    }
                    m_zooming = true ;
                    if (lowerleft.Contains(m_startingtouch))
                    {
                        m_zoomtype = EZoomIn ;
                    }
                    else
                    {
                        m_zoomtype = EZoomOut ;
                    }
                    state = ELockToThisGesture ;    // keep zooming until release
                    TTwoPointGesture pgest(KUid, m_startingtouch, m_startingtouch);
                    pgest.setLogging(m_loggingenabled) ;
                    pgest.setDetails(m_delta) ;
                    pgest.setName(m_zoomtype == EZoomIn ? KZoomInName() : KZoomOutName()) ;
                    pgest.setType(m_zoomtype);

                    // Call the listener to inform that a gesture has happened
                    m_listener->gestureEnter(pgest) ;
                }
                else
                {
                    // It is not our gesture so do nothing...
                }
            }
            else
            {
                // It is not touch, so not our initiating UI event.. do nothing
            }
        }
        else
        {
            // We are already zooming, calculate the changes in zooming factor if it looks we are still zooming
            if (eventCode == stmUiEventEngine::ERelease) // ERelease stops zooming
            {
                // We were zooming, but if there are multiple touches we are not any more
                m_listener->gestureExit(KUid) ; // should we call this or not?
            }
            else    // all other UI events will keep on zooming
            {
                state = ELockToThisGesture ;    // Keep the gesture
                const TPoint& p = puie->CurrentXY() ;
                float newdist = calculateDistance(p) ;
                float olddist = calculateDistance(m_previoustouch) ;
                m_previoustouch = p ;
                m_delta = adjustZoom(olddist, newdist) ;
                state = EGestureActive ;

                // Inform listener only if there is something to say
                if (m_delta != 0)
                {
                    TTwoPointGesture pgest = TTwoPointGesture(KUid, p, m_startingtouch);
                    pgest.setLogging(m_loggingenabled) ;
                    pgest.setDetails(m_delta) ;
                    pgest.setName(m_zoomtype == EZoomIn ? KZoomInName() : KZoomOutName()) ;
                    pgest.setType(m_zoomtype);

                    // Call the listener to inform that a gesture has happened
                    m_listener->gestureEnter(pgest) ;
                }
            }
        }
    }
    else
    {
        if (m_zooming)
        {
            // We were zooming, but if there are multiple touches we are not any more
            m_listener->gestureExit(KUid) ; // should we call this or not?
        }
        m_zooming = false ;
    }
    return state;
}

void CZoomGestureRecogniser::release(MGestureEngineIf*)
{
    if (m_zooming)
    {
        m_zooming = false ;
        m_listener->gestureExit(KUid) ; // should we call this or not?
    }
    if (m_loggingenabled)
    {
        LOGARG("CZoomGestureRecogniser: 0x%x release", this);
    }
}

void CZoomGestureRecogniser::setRange(int rangeInPixels)
{
    m_rangesizeInPixels = rangeInPixels ;
}

void CZoomGestureRecogniser::setArea(const TRect& theArea)
{
    m_area = theArea ;
    if (m_loggingenabled)
    {
        LOGARG("CZoomGestureRecogniser: set area, %d,%d %d,%d",
                m_area.iTl.iX, m_area.iTl.iY, m_area.iBr.iX, m_area.iBr.iY);
    }
}

/*!
 * calculate the distance, return as as float
 */
float CZoomGestureRecogniser::calculateDistance(const TPoint& tp)
{
    double x = ((m_startingtouch.iX-tp.iX)*(m_startingtouch.iX-tp.iX)) +
                ((m_startingtouch.iY-tp.iY)*(m_startingtouch.iY-tp.iY)) ;
    double ddist ;
    Math::Sqrt(ddist, x) ;
    return ddist ;
}

int CZoomGestureRecogniser::adjustZoom(float& aPreviousDistance, float aNewDistance)
{
    float diff = aNewDistance - aPreviousDistance ;
    float logdiff = diff ;
    if (diff < 0) diff = -diff ;
    float changePercentage = (diff/aPreviousDistance)*100.f ;
    if (changePercentage > 10.f)
    {
        // change more than 10%, make at most 10%
        float newdiff = aPreviousDistance*0.1f;
        if (aPreviousDistance > aNewDistance) newdiff = -newdiff ;
        if (m_loggingenabled)
        {
            LOGARG("CZoomGestureRecogniser: adjust zoom from %f to %f : was, now %f %f",
                double(logdiff), double(newdiff), double(aPreviousDistance), double(aNewDistance));
        }

        aPreviousDistance = aPreviousDistance + newdiff ;
        diff = newdiff ;

    }
    else
    {
        if (m_loggingenabled)
        {
            LOGARG("CZoomGestureRecogniser: adjust zoom from %f to %f : was, now %f %f",
                double(logdiff), double(diff), double(aPreviousDistance), double(aNewDistance));
        }
        aPreviousDistance = aNewDistance ;  // accept the new value and update the new length
        diff = logdiff ;    // put the original back
    }
    return (int)diff ;
}
