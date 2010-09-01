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
#include "pinchgesturerecogniser.h"
#include "GenericSimpleGesture.h"
#include <rt_uievent.h>
#include "filelogger.h"
#include "statemachine.h" // for stmUiEventEngine::Distance(dx,dy)

using namespace stmGesture ;

extern long Mm2Pixels(long mm) ;


CPinchGestureRecogniser::CPinchGestureRecogniser(MGestureListener* aListener) :
        CGestureRecogniser(aListener)
{
    m_pinching = false ;
    m_pinchingspeed = 3.5 ; // by default something suitable for capacitive
    m_holdseen = false ;
}

CPinchGestureRecogniser* CPinchGestureRecogniser::NewL(MGestureListener* aListener)
{
    CPinchGestureRecogniser* self = new (ELeave) CPinchGestureRecogniser(aListener) ;
    return self;
}

CPinchGestureRecogniser::~CPinchGestureRecogniser()
{
}

TGestureRecognitionState CPinchGestureRecogniser::recognise(int numOfActiveStreams,
        MGestureEngineIf* pge)
{
    TGestureRecognitionState state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    if (m_loggingenabled)
    {
        LOGARG("CPinchGestureRecogniser: %d %d %d ", m_pinching, m_holdseen, numOfActiveStreams) ;
    }
#if !defined(ADVANCED_POINTER_EVENTS)
    // Look at the events to see if it looks like pinch in single touch
    // WARNING: this code is a hack : in single touch capacitive touch device (like Alvin with 52.50) it works so-and-so,
    // because the pointer events were reported from the corners of the rectangle formed by two fingers pressing.
    // In resistive touch device like S60_50 or PF_52_50 the reported points are somewhere int he middle between the fingers
    // and jumping a lot, so it is very difficult to get it right.
    if (numOfActiveStreams == 1)
    {
        // Then look at the event stream, first we need to see a hold and then a fast jump
        const stmUiEventEngine::MUiEvent* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        stmUiEventEngine::TUiEventCode eventCode = puie->Code() ;

        if (countOfEvents > 0 ) // how many events
        {
            if (m_loggingenabled)
            {
                LOGARG("CPinchGestureRecogniser: %d %d %d %d %d, m: %d b: %d",
                        m_pinching, m_holdseen, numOfActiveStreams, countOfEvents, eventCode,
                        int(m_m), int(m_b)) ;
            }

            if (m_pinching)
            {
                // We have entered pinching state, lets move one of the points unless it is a release
                if (eventCode == stmUiEventEngine::ERelease)
                {
                    m_pinching = false ;
                    m_holdseen = false ;
                }
                else
                {
                    bool pointIgnored = true ;  // for logging purposes
                    int currentLength = m_loggingenabled ?
                                        stmUiEventEngine::Distance(m_pinchstart, m_pinchend) : 0;

                    TPoint oStart(m_pinchstart) ;
                    TPoint oEnd(m_pinchend) ;
                    int difference = 0 ;
                    state = ELockToThisGesture ;
                    const TPoint& tp = puie->CurrentXY();
                    // calculate the distance of the new point from the stored vector
                    int d1 =  ((m_pinchstart.iX-tp.iX)*(m_pinchstart.iX-tp.iX)) +
                                ((m_pinchstart.iY-tp.iY)*(m_pinchstart.iY-tp.iY)) ;
                    int d2 =  ((m_pinchend.iX-tp.iX)*(m_pinchend.iX-tp.iX)) +
                                ((m_pinchend.iY-tp.iY)*(m_pinchend.iY-tp.iY)) ;
                    // check also if the Y coordinate happens to be near the hold point,
                    // this seems to be the case at least with alvin, we keep getting two points,
                    // where one is near the Y coordinate of the hold point
                    int diffY = Abs(tp.iY-m_pinchstart.iY) ;

                    if (d1 < d2 || diffY < 12)
                    {
                        // the detected point is near the first point,
                        // or the detected point is about on the same horizontal line with the hold point
                        // do not do anything, but keep the gesture
                    }
                    else
                    {
                        pointIgnored = false ;
                        // the detected point is close to the other end, then adjust the stored vector
                        int xd = m_pinchend.iX-tp.iX ;
                        int yd = m_pinchend.iY-tp.iY ;
                        if (xd < 0 ) xd = - xd ;
                        if (yd < 0 ) yd = - yd ;
                        // look which coordinate is closer to the original and use that
                        if (xd < yd)
                        {
                            // calculate new point based on the X value
                            m_pinchend.iX = tp.iX ;
                            m_pinchend.iY = m_m*m_pinchend.iX + m_b ;
                            if (m_pinchend.iY < 0) m_pinchend.iY = 0 ;
                        }
                        else
                        {
                            if (m_m != 0)
                            {
                                m_pinchend.iY = tp.iY ;
                                m_pinchend.iX = (m_pinchend.iY - m_b)/m_m ;
                                if (m_pinchend.iX <0 ) m_pinchend.iX = 0 ;
                            }
                            else
                            {
                                m_pinchend.iX = tp.iX ;
                                m_pinchend.iY = m_m*m_pinchend.iX + m_b ;
                                if (m_pinchend.iY < 0) m_pinchend.iY = 0 ;
                            }
                        }
                        float newd = calculateDistance() ;
                        // check if the difference is too big and adjust accordingly
                        // the method also updates the m_ddistance
                        difference = adjustPinchMove(m_ddistance, newd) ;
                        // Now we have a pinch gesture with size as details
                        stmGesture::TTwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
                        pgest.setLogging(m_loggingenabled);
                        pgest.setDetails(difference) ;
                        // inform the listener
                        m_listener->gestureEnter(pgest);
                    }
                    if (m_loggingenabled)
                    {
                        int newLength = stmUiEventEngine::Distance(m_pinchstart, m_pinchend);
                        float speedX = puie->speedX() ;
                        float speedY = puie->speedY() ;

                        LOGARG("CPinchGestureRecogniser: %d: o: %d, n: %d, d: %d (%d,%d) " \
                                "speed %f (%d,%d : %d,%d) (from: (%d,%d : %d,%d) (m: %f b: %f)",
                                pointIgnored,
                                currentLength, newLength, difference,
                                tp.iX, tp.iY, double(speedX),
                                m_pinchstart.iX, m_pinchstart.iY, m_pinchend.iX, m_pinchend.iY,
                                oStart.iX, oStart.iY, oEnd.iX, oEnd.iY,
                                double(m_m), double(m_b)) ;

                    }

                }
            }
            else if (eventCode == stmUiEventEngine::EMove) // The last one is move and we were not pinching
            {
                if (m_loggingenabled)
                {
                    LOGARG("CPinchGestureRecogniser: %d: num %d code %d", m_pinching, countOfEvents, eventCode);
                }
                stmUiEventEngine::MUiEvent* puieFirst = puie->previousEvent();

                // check if we have seen hold
                if (m_holdseen)
                {
                    const TPoint& tp1 = puie->CurrentXY() ;
                    float speedX = puie->speedX() ;
                    float speedY = puie->speedY() ;
                    if (m_loggingenabled)
                    {
                        LOGARG("CPinchGestureRecogniser: tp1: %d %d hold %d %d, speed %f",
                                tp1.iX, tp1.iY,
                                m_holdseenAtPos.iX, m_holdseenAtPos.iY, double(speedX) );
                    }
                    // is the speed extremely high so that it looks like other finger pressing in different location?
                    if ( (speedX > m_pinchingspeed) || (speedY > m_pinchingspeed) )
                    {
                        TInt64 tstamp = puie->timestamp() ;
                        TTime now(tstamp) ;
                        TTimeIntervalMicroSeconds tim = now.MicroSecondsFrom(m_holdseenAtTime) ;
                        m_pinching = true;
                        m_pinchstart = m_holdseenAtPos;
                        m_pinchend = tp1;
                        calculateZoomingLine();
                        m_ddistance = calculateDistance();
                        state = ELockToThisGesture ;    // NOTE: once pinch is started, it will stay until release
                        // create the first pich gesture which does not yet resize anything
                        stmGesture::TTwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
                        pgest.setLogging(m_loggingenabled);
                        pgest.setDetails(0) ;
                        // inform the listener
                        m_listener->gestureEnter(pgest);
                    }
                }
            }
        }
        if (!m_pinching)
        {
            if (m_loggingenabled)
            {
                LOGARG("CPinchGestureRecogniser: not pinching %d", puie);
            }
            if (puie && puie->Code() == stmUiEventEngine::EHold) // The last one is hold and we were not pinching
            {
                m_holdseen = true;
                m_holdseenAtPos = puie->CurrentXY();
                m_holdseenAtTime = puie->timestamp() ;
                if (m_loggingenabled)
                {
                    LOGARG("CPinchGestureRecogniser: hold seen at(%d, %d) at %Ld",
                            m_holdseenAtPos.iX, m_holdseenAtPos.iY, m_holdseenAtTime.Int64());
                }
            }
        }
        if (puie && puie->Code() == stmUiEventEngine::ETouch) // The last one is touch
        {
            m_holdseen = false;
        }
        else if (puie && puie->Code() == stmUiEventEngine::ERelease) // The last one is release
        {
            m_holdseen = false;
        }
    }
#else
    // This is the multi touch case: two event streams needs to be there; this is the real pinch zoom
    if (numOfActiveStreams == 2)
    {
        const stmUiEventEngine::MUiEvent* puie1 = pge->getUiEvents(0);
        const stmUiEventEngine::MUiEvent* puie2 = pge->getUiEvents(1);
        stmUiEventEngine::TUiEventCode eventCode1 = puie1->Code() ;
        stmUiEventEngine::TUiEventCode eventCode2 = puie2->Code() ;

        if (m_loggingenabled)
        {
            TPoint p1 = puie1->CurrentXY() ;
            TPoint p2 = puie2->CurrentXY() ;
            LOGARG("CPinchGestureRecogniser: two streams: %s at [%d,%d], %s at [%d,%d]",
                    stmUiEventEngine::EventName(eventCode1), p1.iX, p1.iY,
                    stmUiEventEngine::EventName(eventCode1), p2.iX, p2.iY
                    ) ;

        }


        if (!m_pinching)
        {
            // This means we start pinching, the events can be any combination of ETouch, EMove, EHold
            if ( (  eventCode1 == stmUiEventEngine::ETouch  ||
                    eventCode1 == stmUiEventEngine::EMove   ||
                    eventCode1 == stmUiEventEngine::EHold
                  ) &&
                 (  eventCode2 == stmUiEventEngine::ETouch  ||
                    eventCode2 == stmUiEventEngine::EMove   ||
                    eventCode2 == stmUiEventEngine::EHold )
                 )
            {
                // This is valid pinching start
                m_pinching = true ;
                // get the start and end position for the picnhing vector
                m_pinchstart = puie1->CurrentXY() ;
                m_pinchend = puie2->CurrentXY() ;
                calculateZoomingLine();
                m_ddistance = calculateDistance();
                state = ELockToThisGesture ;    // NOTE: once pich is started, it will stay until release
                if (m_loggingenabled)
                {
                    LOGARG("CPinchGestureRecogniser: pinch start: [%d,%d][%d,%d]",
                            m_pinchstart.iX, m_pinchstart.iY, m_pinchend.iX, m_pinchend.iY) ;

                }
                // create the first pich gesture which does not yet resize anything
                stmGesture::TTwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
                pgest.setLogging(m_loggingenabled);
                pgest.setDetails(0) ;
                // inform the listener
                m_listener->gestureEnter(pgest);
            }
            else
            {
            	// Not a valid pinching start, do nothing (maybe it were easier to just check if one of the events is ERelease)
            }
        }
        else
        {
            // We have entered pinching state, lets move one of the points unless it is a release
            if (eventCode1 == stmUiEventEngine::ERelease || eventCode2 == stmUiEventEngine::ERelease)
            {
                release(pge);
            }
            else
            {
                state = ELockToThisGesture ;

                // get the start and end position for the picnhing vector
                m_pinchstart = puie1->CurrentXY() ;
                m_pinchend = puie2->CurrentXY() ;
                float newd = calculateDistance() ;
                // check if the difference is too big and adjust accordingly
                // the method also updates the m_ddistance
                int difference = adjustPinchMove(m_ddistance, newd) ;
                // Now we have a pinch gesture with size
                if (m_loggingenabled)
                {
                    LOGARG("CPinchGestureRecogniser: pinch: [%d,%d][%d,%d], diff %d",
                            m_pinchstart.iX, m_pinchstart.iY, m_pinchend.iX, m_pinchend.iY, difference) ;

                }

                stmGesture::TTwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
                pgest.setLogging(m_loggingenabled);
                pgest.setDetails(difference) ;
                // inform the listener
                m_listener->gestureEnter(pgest);
            }
        }

    }
#endif

    if (state == ENotMyGesture)
    {
        if (m_loggingenabled)
        {
            LOGARG("CPinchGestureRecogniser: NotMyGesture %d %d %d ",
                    m_pinching, m_holdseen, numOfActiveStreams) ;
        }
        // if it was not our gesture, then the state can not be pinching...
        m_pinching = false ;
    }
    return state;
}

void CPinchGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    m_pinching = false ;
    m_listener->gestureExit(KUid) ;
}

/*!
 * Now that we know the two points where the zooming started, we move those points only along
 * the same line y = mx + b, so lets calculate m and b.
 */
void CPinchGestureRecogniser::calculateZoomingLine()
{
    int sX = m_pinchstart.iX ;
    int sY = m_pinchstart.iY ;
    int eX = m_pinchend.iX ;
    int eY = m_pinchend.iY ;

    if (eX == sX)
    {
        m_m = 0.f ;
    }
    else
    {
        m_m = float(eY-sY)/(eX-sX) ;
    }
    m_b = sY-(m_m*sX) ;
}

/*!
 * calculate the distance, return as float
 */
float CPinchGestureRecogniser::calculateDistance()
{
    double x = ((m_pinchstart.iX-m_pinchend.iX)*(m_pinchstart.iX-m_pinchend.iX))+
               ((m_pinchstart.iY-m_pinchend.iY)*(m_pinchstart.iY-m_pinchend.iY)) ;
    double ddist ;
    Math::Sqrt(ddist, x) ;
    return float(ddist) ;
}

/*!
 * Set the pinching speed as pixels / ms (meaning that in case of singletouch device
 * the other finger looks like the EMove UI event suddenly jumps to new location;
 * in resistive the new location is somewhere in the middle of the touches, in capacitive
 * the driver seems to report three or four points:
 * original (x,y), new (a,b) and also (a,y), sometimes (x,b)
 */
void CPinchGestureRecogniser::setPinchingSpeed(float aSpeed) __SOFTFP
{
    m_pinchingspeed = aSpeed ;
}

/*!
 * Adjust the pinch move so that it will not be too jumpy
 */
int CPinchGestureRecogniser::adjustPinchMove(float& aPreviousDistance, float aNewDistance)
{
    float diff = aNewDistance - aPreviousDistance ;
    float logdiff = diff ;
    if (diff < 0) diff = -diff ;	// Note that the next calculations need the positive diff value, but keep the original in logdiff
    float changePercentage = (diff/aPreviousDistance)*100.f ;
    if (changePercentage > 10.f)
    {
        // change more than 10%, make at most 10%
        float newdiff = aPreviousDistance*0.1f ;
        if (aPreviousDistance > aNewDistance) newdiff = -newdiff ;
        if (m_loggingenabled)
        {
            LOGARG("CPinchGestureRecogniser: adjustPinchMove from %f to %f : was, now %f %f",
                double(logdiff), double(newdiff), double(aPreviousDistance), double(aNewDistance));
        }

        aPreviousDistance = aPreviousDistance + newdiff ;
        diff = newdiff ;
    }
    else
    {
        if (m_loggingenabled)
        {
            LOGARG("CPinchGestureRecogniser: adjustPinchMove from %f to %f : was, now %f %f",
                double(logdiff), double(diff), double(aPreviousDistance), double(aNewDistance));
        }
        aPreviousDistance = aNewDistance ;  // accept the new value and update the new length
        diff = logdiff ;    // put the original back (this is why the logdiff can not be Abs(diff)!
    }
    return (int)diff ;
}
