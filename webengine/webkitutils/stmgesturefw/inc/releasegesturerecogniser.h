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
* Description: release Gesture Recognizer
*
*/

#ifndef RELEASEGESTURERECOGNISER_H_
#define RELEASEGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

/*!
 * CReleaseGestureRecogniser handles release "gesture"
 * This is countarpart to touch gesture recogniser, just
 * informing the app that ERelease has been seen.
 * This should be placed towards the end of the list of gesture
 * recognisers, all other recognisers which handle ERelease with
 * some other events preceding the ERelease must be before this one.
 *
 */
NONSHARABLE_CLASS( CReleaseGestureRecogniser ): public CGestureRecogniser
{
public:
    static const TGestureUid KUid = EGestureUidRelease;

    /** Two-phase constructor */
    static CReleaseGestureRecogniser* NewL(MGestureListener* aListener);
    virtual ~CReleaseGestureRecogniser();

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
    CReleaseGestureRecogniser(MGestureListener* aListener) ;

private:
    TRect m_area ;
};

} // namespace

#endif /* RELEASEGESTURERECOGNISER_H_ */
