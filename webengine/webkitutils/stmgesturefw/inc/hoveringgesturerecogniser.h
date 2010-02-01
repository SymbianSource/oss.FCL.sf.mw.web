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
* Description:Hover Gesture Recognizer
*
*/

#ifndef HOVERINGGESTURERECOGNISER_H_
#define HOVERINGGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{
/*!
 * Recognise hovering gesture.
 * If the EMove distance from previous is short enough, then
 * we assume hovering gesture.  If the previous UI event is ETouch, EHold or EMove
 * and the distance is short enough the gesture takes control.
 */
NONSHARABLE_CLASS( CHoveringGestureRecogniser ): public CGestureRecogniser
{
public:
    static const TGestureUid KUid = EGestureUidHover;

    static CHoveringGestureRecogniser* NewL(MGestureListener* aListener);

    virtual ~CHoveringGestureRecogniser();
    /*!
     * MGestureRecogniserIf methods
     */
    virtual TGestureRecognitionState recognise(int numOfActiveStreams, MGestureEngineIf* ge) ;
    virtual void release(MGestureEngineIf* ge) ;

    virtual TGestureUid gestureUid() const
        {
        return KUid;
        }

    void setHoveringSpeed(float aSpeed) __SOFTFP ;

private:
    CHoveringGestureRecogniser(MGestureListener* aListener) ;
    bool m_hovering ;
    float m_hoveringspeed ;
};

}

#endif /* HOVERINGGESTURERECOGNISER_H_ */
