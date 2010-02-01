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
* Description: Unknown gesture recognizer
*
*/

#ifndef UNKNOWNGESTURERECOGNISER_H_
#define UNKNOWNGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

/*!
 * CUnknownGestureRecogniser handles unknown gestures at ERelease
 * I.e. if no-one else has recognised a gesture when ERelease arrives,
 * This recogniser can be at the end of the list to inform the app
 * that something weird is happening...
 *
 */
NONSHARABLE_CLASS( CUnknownGestureRecogniser ): public CGestureRecogniser
{
public:
    static const TGestureUid KUid = EGestureUidUnknown;

    /** Two-phase constructor */
    static CUnknownGestureRecogniser* NewL(MGestureListener* aListener);
    virtual ~CUnknownGestureRecogniser();

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
    CUnknownGestureRecogniser(MGestureListener* aListener) ;

};

} // namespace

#endif /* UNKNOWNGESTURERECOGNISER_H_ */
