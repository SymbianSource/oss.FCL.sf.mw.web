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
#include "TapGestureRecogniser.h"
#include "rt_uievent.h"
#include "filelogger.h"

using namespace stmGesture ;

/* some utility functions, are these things not provided by the OS? */
const TInt KFingerSize_mm = 8;
const TInt KTwipsInInch = 1440;
const TReal KTwipsInMm = 56.7;

long Twips2Pixels(long twips)
{
    CWsScreenDevice* screen = CCoeEnv::Static()->ScreenDevice();
    TZoomFactor deviceMap(screen);
    deviceMap.SetZoomFactor(TZoomFactor::EZoomOneToOne);
    long px = deviceMap.VerticalTwipsToPixels(twips); //assuming that vertical
    return px; //the same as horizontal
}

long Mm2Pixels(long mm)
{
    return Twips2Pixels(mm * KTwipsInMm);
}

long Inches2Pixels(double inches)
{
    return Twips2Pixels(inches * KTwipsInInch);
}

TRect ToleranceRect(const TPoint& aCenterPoint, int size)
{
    long toleranceLength = Mm2Pixels(KFingerSize_mm) / 2;
    TRect toleranceRect(aCenterPoint, TSize());
    toleranceRect.Shrink(-size, -size);
    return toleranceRect;
}

CTapGestureRecogniser* CTapGestureRecogniser::NewL(MGestureListener* aListener)
{
    CTapGestureRecogniser* self = new (ELeave) CTapGestureRecogniser(aListener) ;
    CleanupStack::PushL(self);
    self->ConstructL(); // construct base class
    CActiveScheduler::Add(self);
    CleanupStack::Pop(self);
    return self;
}

CTapGestureRecogniser::CTapGestureRecogniser(MGestureListener* aListener) :
    CTimer(EPriorityRealTime - 1)
{
    // if a listener is given here, then it is both tap and doubletap listener
    if (aListener)
    {
        m_powner = aListener->getOwner() ;
        addTapListener(aListener, m_powner) ;
        addDoubleTapListener(aListener, m_powner) ;
    }
    m_waitingforsecondtap = false ;
    m_gestureEnabled = true ;
    m_ignorefirst = true ;  // by default ignore the first tap
}

CTapGestureRecogniser::~CTapGestureRecogniser()
{
    Cancel();
    m_tapListeners.Reset() ;
    m_tapListenerWindows.Reset() ;
    m_doubleTapListeners.Reset() ;
    m_doubleTapListenerWindows.Reset() ;

}

TGestureRecognitionState CTapGestureRecogniser::recognise(int numOfActiveStreams,
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

        if (m_loggingenabled)
        {
            LOGARG("CTapGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
        if (countOfEvents == 2) // Do we have touch and release in the stream, check if there are two events
        {
            // Then look at the events to see if they are suitable for us
            if (eventCode == stmUiEventEngine::ERelease) // The last one is release
            {
                stmUiEventEngine::MUiEvent* puieFirst = puie->previousEvent();
                
                if(puieFirst)
                    eventCode = puieFirst->Code();
                else 
                    return state; 
                
                if (eventCode == stmUiEventEngine::ETouch)   // is the first one ETouch
                {
                    if (m_loggingenabled)
                    {
                        LOGARG("CTapGestureRecogniser: 0x%x TAP: num %d code %d", 
                                this, countOfEvents, eventCode);
                    }
                    // It is tap gesture in our window, handle it
                    state = EGestureActive;

                    CCoeControl* target = (CCoeControl*)puie->Target();

                    if (m_waitingforsecondtap)
                    {
                        m_waitingforsecondtap = false ;
                        if (m_loggingenabled)
                        {
                            LOGARG("CTapGestureRecogniser: 0x%x second tap: num %d code %d", 
                                    this, countOfEvents, eventCode);
                        }

                        Cancel() ;  // The timer

                        const TPoint& secondPoint = puieFirst->CurrentXY() ;
                        if (isSecondTapClose(secondPoint, m_firstTapXY))
                        {
                            // Taps were close enough together, so issue a doubletap

                            // Call the listener of the current window to inform that a doubletap has occurred...
                            TInt inx = m_doubleTapListenerWindows.Find(target) ;
                            if (inx == KErrNotFound)
                            {
                                // the second tap hit a window with no listener,
                                // check if the first one has a listener
                                inx = m_doubleTapListenerWindows.Find(m_firstTapTarget) ;
                            }
                            // not found, check if the parent is in the listener list
                            if (inx == KErrNotFound)
                            {
                                CCoeControl* pc = target ;
                                while (pc)
                                {
                                    pc = pc->Parent() ;
                                    inx = m_doubleTapListenerWindows.Find(pc) ;
                                    if (inx != KErrNotFound) break ;
                                }
                            }
                            if (inx != KErrNotFound)
                            {
                                // Tap gesture
                                stmGesture::TGenericSimpleGesture pgest(
                                    stmGesture::EGestureUidDoubleTap, 
                                    secondPoint, stmGesture::ETapTypeDouble, puie) ;
                                MGestureListener* plistener = m_doubleTapListeners[inx] ;
                                plistener->gestureEnter(pgest) ;
                            }
                        }
                        else
                        {
                            // Second tap is too far away, generate just tap
                            // and if configured, also the fist tap is generated
                            if (!m_ignorefirst)
                            {
                                // do not ignore the first tap, so issue it now using the stored location
                                // Call the listener to inform that a Tap has occurred, if there was a listener in that window
                                TInt inx = m_tapListenerWindows.Find(m_firstTapTarget) ;
                                if (inx != KErrNotFound)    // check if the listener exists
                                {
                                    stmGesture::TGenericSimpleGesture pgest(
                                        stmGesture::EGestureUidTap, puieFirst->CurrentXY(), 
                                        stmGesture::ETapTypeSingle, puieFirst) ; // TODO: speed is 0?
                                    MGestureListener* plistener = m_tapListeners[inx] ;
                                    plistener->gestureEnter(pgest) ;
                                }
                            }
                            // generate a tap at the current location, if there is a listener for it
                            TInt inx = m_tapListenerWindows.Find(target) ;
                            if (inx != KErrNotFound)
                            {
                                stmGesture::TGenericSimpleGesture pgest(
                                    stmGesture::EGestureUidTap, puie->CurrentXY(), 
                                    stmGesture::ETapTypeSingle, puie) ; // TODO: speed is 0?
                                MGestureListener* plistener = m_tapListeners[inx] ;
                                plistener->gestureEnter(pgest) ;
                            }
                        }
                    }
                    else
                    {
                        m_firstTapXY = puieFirst->CurrentXY() ;
                        m_firstTapTarget = target ;
                        m_firstTapSpeedX = puie->speedX() ;
                        m_firstTapSpeedY = puie->speedY() ;
                        // This was the first tap, start the timer...
                        m_waitingforsecondtap = true ;
                        if (m_loggingenabled)
                        {
                            LOGARG("CTapGestureRecogniser: 0x%x first tap: num %d code %d", 
                                    this, countOfEvents, eventCode);
                        }
                        Cancel() ;  // Just to be sure...
                        After(m_doubleTapTimeout) ;
                    }

                }
            }
        }
    }
    return state;
}

void CTapGestureRecogniser::release(MGestureEngineIf* /*ge*/)
{
    Cancel() ;  // some other gesture took hold of the thing, do not send tap gesture
    m_waitingforsecondtap = false ;
    if (m_loggingenabled)
    {
        LOGARG("CTapGestureRecogniser: 0x%x release, %d %d", 
                this, m_firstTapXY.iX, m_firstTapXY.iY);
    }
}

void CTapGestureRecogniser::RunL()
{
    m_waitingforsecondtap = false ;
    if (m_loggingenabled)
    {
        LOGARG("CTapGestureRecogniser: 0x%x timer, %d %d", this, m_firstTapXY.iX, m_firstTapXY.iY);
    }
    // Double tap timer has been elapsed without new Touch/Release, generate the tap if there is a listener
    TInt inx = m_tapListenerWindows.Find(m_firstTapTarget) ;
    if (inx != KErrNotFound)
    {
        using stmUiEventEngine::TUiEventSpeed;

        TUiEventSpeed speedIf(m_firstTapSpeedX,m_firstTapSpeedY);

        stmGesture::TGenericSimpleGesture pgest(
                stmGesture::EGestureUidTap,
                m_firstTapXY,
                stmGesture::ETapTypeSingle,
                &speedIf) ;

        MGestureListener* plistener = m_tapListeners[inx] ;
        plistener->gestureEnter(pgest) ;
    }
}

void CTapGestureRecogniser::enableLogging(bool loggingOn)
{
    m_loggingenabled = loggingOn;
}

void CTapGestureRecogniser::setOwner(CCoeControl* owner)
{
    m_powner = owner;
}

void CTapGestureRecogniser::setDoubleTapTimeout(int newtimeout)
{
    m_doubleTapTimeout  = newtimeout;
}

void CTapGestureRecogniser::enable(bool enabled)
{
    m_gestureEnabled = enabled ;
}

bool CTapGestureRecogniser::isEnabled()
{
    return m_gestureEnabled ;
}

void CTapGestureRecogniser::setDoubleTapRange(int rangeInMillimetres)
{
    m_rangesizeInPixels = Mm2Pixels(rangeInMillimetres) ;
}

void CTapGestureRecogniser::ignoreFirstTap(bool ignore)
{
    m_ignorefirst = ignore ;
}

/*!
 * Check whether the two taps are close enough to each other
 */
bool CTapGestureRecogniser::isSecondTapClose(const TPoint& secondPoint, const TPoint& firstTapXY)
{
    TRect tolerance = ToleranceRect(secondPoint, m_rangesizeInPixels) ;
    bool aretheyclose = tolerance.Contains(firstTapXY);
    return aretheyclose ;
}

void CTapGestureRecogniser::addTapListener(MGestureListener* aListener, CCoeControl* listenerOwner)
{
    m_tapListeners.Append(aListener) ;
    m_tapListenerWindows.Append(listenerOwner) ;
}

void CTapGestureRecogniser::removeTapListener(MGestureListener* aListener, 
                                              CCoeControl* /*listenerOwner*/)
{
    TInt inx = m_tapListeners.Find(aListener) ;
    if(inx != KErrNotFound)
    {
        m_tapListeners.Remove(inx) ;
        m_tapListenerWindows.Remove(inx) ;
    }
}

void CTapGestureRecogniser::addDoubleTapListener(MGestureListener* aListener, 
                                                 CCoeControl* listenerOwner)
{
    m_doubleTapListeners.Append(aListener) ;
    m_doubleTapListenerWindows.Append(listenerOwner) ;
}

void CTapGestureRecogniser::removeDoubleTapListener(MGestureListener* aListener, 
                                                    CCoeControl* /*listenerOwner*/)
{
    TInt inx = m_doubleTapListeners.Find(aListener) ;
    if(inx != KErrNotFound)
    {
        m_doubleTapListeners.Remove(inx) ;
        m_doubleTapListenerWindows.Remove(inx) ;
    }
}

