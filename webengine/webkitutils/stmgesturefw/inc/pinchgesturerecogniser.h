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
* Description:Pinch Gesture Recognizer
*
*/

#ifndef PINCHGESTURERECOGNISER_H_
#define PINCHGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{
/*!
 * Recognise pinching gesture. !!! very experimental !!!
 * It seems that in capacitive touch in Symbian 9.4 touching with two
 * fingers causes fast EDrag events which are far from each other.
 * This gesture recogniser tries to use that feature
 * to determine when the gestures looks like a pinch.
 * Would requires a lot of tuning to be really useful, if at all possible.
 * With current implementation it is easy to zoom in but
 * almost impossible to zoom out.
 */
NONSHARABLE_CLASS( CPinchGestureRecogniser ): public CGestureRecogniser
{
public:
    static const TGestureUid KUid = EGestureUidPinch;

    static CPinchGestureRecogniser* NewL(MGestureListener* aListener);

    virtual ~CPinchGestureRecogniser();
    /*!
     * MGestureRecogniserIf methods
     */
    virtual TGestureRecognitionState recognise(int numOfActiveStreams, MGestureEngineIf* ge) ;
    virtual void release(MGestureEngineIf* ge) ;

    virtual TGestureUid gestureUid() const
        {
        return KUid;
        }

    /* define the pinch triggering as speed value */
    /* after EHold has been seen, then any move which is faster than this will enter pinch gesture */
    /* note that the value varies depending on techology, speed is measured as pixels / ms */
    void setPinchingSpeed(float aSpeed) __SOFTFP;

private:
    CPinchGestureRecogniser(MGestureListener* aListener) ;

    void calculateZoomingLine() ;
    float calculateDistance() ;
    int adjustPinchMove(float& aPreviousDistance, float aNewDistance) ;

private:
    bool m_pinching ;
    float m_pinchingspeed ;
    TPoint m_pinchstart ;
    TPoint m_pinchend ;

    float m_m ;
    float m_b ;

    float m_ddistance ;
    bool m_holdseen ;
    TPoint m_holdseenAtPos ;
    TTime m_holdseenAtTime ;
};

}

#endif /* PINCHGESTURERECOGNISER_H_ */
