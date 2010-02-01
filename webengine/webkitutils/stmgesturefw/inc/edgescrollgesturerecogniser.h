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
* Description:  Edge Scroll Recognizer
*
*/

#ifndef EDGESCROLLGESTURERECOGNISER_H_
#define EDGESCROLLGESTURERECOGNISER_H_

#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

class CCoeControl ;

namespace stmGesture
{

/*!
 * CEdgeScrollGestureRecogniser handles edge scroll gesture: Hold event near the edges of the window
 *
 */
NONSHARABLE_CLASS( CEdgeScrollGestureRecogniser ): public CPeriodic, 
                                                   public MGestureRecogniserIf
{
public:
    static const TGestureUid KUid = EGestureUidEdgeScroll;

    /** Two-phase constructor */
    static CEdgeScrollGestureRecogniser* NewL(MGestureListener* aListener);
    virtual ~CEdgeScrollGestureRecogniser();

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
     * Additional methods to set up edge scroll gesture recogniser:
     * define the rectangle where the edges would be watched.
     * \param theArea
     */
    void setArea(const TRect& theArea)  ;

    /*!
     * Additional methods to set up edge scroll gesture recogniser:
     * Define how close to the border the Hold UI event must be
     * \param rangeInPixels
     */
    void setScrollRange(int rangeInPixels) ;
    /*!
     * for testing purposes we need to log the stuff somewhere...
     */
public:
    virtual void enableLogging(bool loggingOn) ;
    /*!
     * The timer function to handle timeout for generating scroll events
     */
    virtual void RunL() ;


private:
    CEdgeScrollGestureRecogniser(MGestureListener* aListener) ;
    bool m_loggingenabled ;
private:
    CCoeControl* m_powner ; // The owning control for this gesture
    MGestureListener* m_listener ;
    bool m_waitingforsecondtap ;
    TRect m_area ;
    bool m_gestureEnabled ;
    int  m_rangesizeInPixels ;
};

} // namespace

#endif /* EDGESCROLLGESTURERECOGNISER_H_ */
