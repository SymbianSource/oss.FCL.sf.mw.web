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
* Description: Touch Gesture Recogniser
*
*/

#ifndef TOUCHGESTURERECOGNISER_H_
#define TOUCHGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

/*!
 * CTouchGestureRecogniser handles touch "gesture".  The purpose is to inform the app
 * that a touch has happened.  The gesture is never owned, since most of the gestures start with touch.
 * The app may define the area where to watch the gestures, or it can define empty area:
 * then the touch is recognised inside the window.
 *
 */
NONSHARABLE_CLASS( CTouchGestureRecogniser ): public CGestureRecogniser
{
public:
    static const TGestureUid KUid = EGestureUidTouch;

    /** Two-phase constructor */
    static CTouchGestureRecogniser* NewL(MGestureListener* aListener);
    virtual ~CTouchGestureRecogniser();

    /*!
     * MGestureRecogniserIf methods
     */
    virtual TGestureRecognitionState recognise(int numOfActiveStreams, MGestureEngineIf* ge) ;
    virtual void release(MGestureEngineIf* ge) ;

    virtual TGestureUid gestureUid() const
        {
        return KUid;
        }

    /*!
     * Additional methods to set up edge scroll gesture recogniser:
     * define the rectangle where the touch is watched
     * \param theArea
     */
    void setArea(const TRect& theArea) ;

private:
    CTouchGestureRecogniser(MGestureListener* aListener) ;

private:
    TRect m_area ;
};

} // namespace

#endif /* TOUCHGESTURERECOGNISER_H_ */
