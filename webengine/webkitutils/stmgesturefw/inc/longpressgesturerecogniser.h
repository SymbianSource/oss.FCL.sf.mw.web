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
* Description:LongPress Gesture Recognizer
*
*/

#ifndef LONGPRESSGESTURERECOGNISER_H_
#define LONGPRESSGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

/*!
 * CLongPressGestureRecogniser handles long press gesture (generated from Hold UI event)
 *
 */
NONSHARABLE_CLASS( CLongPressGestureRecogniser ): public CGestureRecogniser
{
public:
    static const TGestureUid KUid = EGestureUidLongPress;

    /** Two-phase constructor */
    static CLongPressGestureRecogniser* NewL(MGestureListener* aListener);
    virtual ~CLongPressGestureRecogniser();

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
     * define the rectangle
     * \param theArea
     */
    void setArea(const TRect& theArea) ;

private:
    CLongPressGestureRecogniser(MGestureListener* aListener) ;

private:
    bool m_waitingforsecondtap ;
    TRect m_area ;
};

} // namespace

#endif /* LONGPRESSGESTURERECOGNISER_H_ */
