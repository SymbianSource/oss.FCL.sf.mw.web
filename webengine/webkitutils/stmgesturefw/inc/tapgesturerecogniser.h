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
* Description: Tap Gesture Recognizer
*
*/

#ifndef TAPGESTURERECOGNISER_H_
#define TAPGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

/*!
 * CTapGestureRecogniser handles both tap and double tap recognition.
 * Doubletap needs to work even if it crosses window boundaries, so
 * CTapGestureRecogniser is a "global" recogniser.  The application
 * creates only one instance of it (TODO later: how to use Singleton
 * pattern in Symbian, seems to be tricky since we would need to have
 * writable static variable to hold the one instance, so currently
 * we need well-behaving app to handle this: the application must known
 * the nature of this gesture recogniser).
 * The different windows can add their callbacks, and when recognising tap
 * the target window is stored so that proper callback can be called.
 * Doubletap is reported either to the target of second tap, or if that windows parent
 * has a doubletap listener, to that.
 * Use separate listeners for tap and doubletap.
 */
NONSHARABLE_CLASS( CTapGestureRecogniser ): public CTimer, public MGestureRecogniserIf
{
public:
    static const TGestureUid KUid = EGestureUidTap;

    /** Two-phase constructor */
    static CTapGestureRecogniser* NewL(MGestureListener* aListener) ;

    virtual ~CTapGestureRecogniser();

    /*!
     * MGestureRecogniserIf methods
     */
    virtual TGestureRecognitionState recognise(int numOfActiveStreams, MGestureEngineIf* ge) ;
    virtual void release(MGestureEngineIf* ge) ;
    virtual void enable(bool enabled) ;
    virtual bool isEnabled() ;
    virtual void setOwner(CCoeControl* owner) ;

    virtual TGestureUid gestureUid() const
        {
        return KUid;
        }

    /*!
     * Additional methods to set up tap gesture recogniser:
     * define the double tap timeout in microseconds.
     * \param timeoutInMicroseconds
     */
    void setDoubleTapTimeout(int timeoutInMicroseconds) ;

    /*!
     * Additional methods to set up tap gesture recogniser:
     * define how close the two taps need to be to be recognised
     * as a double tap.
     * \param rangeInMillimetres
     */
    void setDoubleTapRange(int rangeInMillimetres) ;
    /*!
     * Additional methods to set up tap gesture recogniser:
     * Produce two separate taps or just ignore the first one
     * if the second tap is outside range.
     */
    void ignoreFirstTap(bool ignore) ;

    /*!
     * Method to add a listener to tap gestures
     */
    void addTapListener(MGestureListener* aListener, CCoeControl* listenerOwner) ;
    /*!
     * Method to remove a listener from tap gestures
     */
    void removeTapListener(MGestureListener* aListener, CCoeControl* listenerOwner) ;
    /*!
     * Method to add a listener to doubletap gestures
     */
    void addDoubleTapListener(MGestureListener* aListener, CCoeControl* listenerOwner) ;
    /*!
     * Method to remove a listener from doubletap gestures
     */
    void removeDoubleTapListener(MGestureListener* aListener, CCoeControl* listenerOwner) ;

    /*!
     * for testing purposes we need to log the stuff somewhere...
     */
public:
    virtual void enableLogging(bool loggingOn) ;

    /*!
     * The timer function to handle timeout for tap
     */
    virtual void RunL() ;


private:
    CTapGestureRecogniser(MGestureListener* aListener) ;
    bool m_loggingenabled ;
    bool isSecondTapClose(const TPoint& secondPoint, const TPoint& firstPoint) ;
private:
    CCoeControl* m_powner ; // The owning control for this gesture
    bool m_waitingforsecondtap ;
    TPoint m_firstTapXY ;
    CCoeControl* m_firstTapTarget ;
    float m_firstTapSpeedX ;
    float m_firstTapSpeedY ;
    int m_doubleTapTimeout ;
    bool m_gestureEnabled ;
    bool m_ignorefirst ;
    int  m_rangesizeInPixels ;
    // use simple arrays to store the listeners and corresponding windows
    RPointerArray<MGestureListener> m_tapListeners ;
    RPointerArray<CCoeControl> m_tapListenerWindows ;
    RPointerArray<MGestureListener> m_doubleTapListeners ;
    RPointerArray<CCoeControl> m_doubleTapListenerWindows ;


};

} // namespace

#endif /* TAPGESTURERECOGNISER_H_ */
