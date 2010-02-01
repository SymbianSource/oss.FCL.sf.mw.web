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
* Description: LeftRight Gesture Recognizer
*
*/

#ifndef LEFTRIGHTGESTURERECOGNISER_H_
#define LEFTRIGHTGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

NONSHARABLE_CLASS( CLeftrightGestureRecogniser ): public CGestureRecogniser
{
public:
    static const TGestureUid KUid = EGestureUidLeftRight;

    static CLeftrightGestureRecogniser* NewL(MGestureListener* aListener);

    virtual ~CLeftrightGestureRecogniser();
    /*!
     * MGestureRecogniserIf methods
     */
    virtual TGestureRecognitionState recognise(int numOfActiveStreams, MGestureEngineIf* ge) ;
    virtual void release(MGestureEngineIf* ge) ;

    virtual TGestureUid gestureUid() const
        {
        return KUid;
        }

private:
    CLeftrightGestureRecogniser(MGestureListener* aListener) ;
};

}

#endif /* LEFTRIGHTGESTURERECOGNISER_H_ */
