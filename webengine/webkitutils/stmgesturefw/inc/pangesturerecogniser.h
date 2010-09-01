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
* Description:Pan Gesture Recognizer
*
*/

#ifndef PANGESTURERECOGNISER_H_
#define PANGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

NONSHARABLE_CLASS( CPanGestureRecogniser ): public CGestureRecogniser
{
public:
    static const TGestureUid KUid = EGestureUidPan;

    static CPanGestureRecogniser* NewL(MGestureListener* aListener);

    virtual ~CPanGestureRecogniser();
    /*!
     * MGestureRecogniserIf methods
     */
    virtual TGestureRecognitionState recognise(int numOfActiveStreams, MGestureEngineIf* ge) ;
    virtual void release(MGestureEngineIf* ge) ;

    virtual TGestureUid gestureUid() const
        {
        return KUid;
        }

    virtual void setPanningSpeedLow(float aSpeed) __SOFTFP ;
    virtual void setPanningSpeedHigh(float aSpeed) __SOFTFP ;   // speeds above this are flicks

private:
    CPanGestureRecogniser(MGestureListener* aListener) ;

private:
    float m_panningspeedlow ;
    float m_panningspeedhigh ;
};

}

#endif /* PANGESTURERECOGNISER_H_ */
