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
* Description:   UI Event
*
*/

#ifndef UIEVENT_H_
#define UIEVENT_H_
#include "rt_uievent.h"

namespace stmUiEventEngine
{

NONSHARABLE_CLASS( CUiEvent ): public MUiEvent
{
public:
    virtual const TPoint& StartPos() const ;
    virtual const TPoint& CurrentXY() const ; // current yx-coordinate
    virtual const TPoint& PreviousXY() const ;    // Past yx coordinate
    virtual TTimeIntervalMicroSeconds StateTransition() const ;    // Time taken for state transition
    virtual bool TimerExpired() const;   // if timer expired
    virtual TUiEventCode Code()const ;
    virtual void* Target() const ;
    virtual int Index() const ;
    MUiEvent* previousEvent() const ;
    virtual int countOfEvents() const  ;
    virtual TInt64 timestamp() const ;
    virtual float speedX() const __SOFTFP; // pixels / millisecond
    virtual float speedY() const __SOFTFP; // pixels / millisecond

    static CUiEvent* NewL(TUiEventCode aCode, const TPoint& aStart, const TPoint& aXY,
        const TPoint& aPreviousXY, bool aTimerExpired, void* aTarget,
        TTimeIntervalMicroSeconds aInterval, int aIndex, TInt64 aTimestamp) ;
    ~CUiEvent() ;
    /*!
     * Chain the UI events ; the whole chain is deleted after release has been handled
     * in UI sender
     */
    virtual void setPrevious(CUiEvent* aEvent) ;
private:
    CUiEvent(
        TUiEventCode aCode,
        const TPoint& aStart, const TPoint& aXY, const TPoint& aPreviousXY,
        bool aTimerExpired, void* aTarget, TTimeIntervalMicroSeconds aInterval,
        int aIndex, TInt64 aTimestamp) ;

    TUiEventCode m_code ;
    TPoint m_start ;
    TPoint m_XY ;
    TPoint m_previousXY ;
    TTimeIntervalMicroSeconds m_statetransition ;
    void* m_target ;
    bool m_timerExpired ;
    int  m_index ;
    CUiEvent* m_previousEvent ;
    TInt64 m_timestamp ;
};
}
#endif /* UIEVENT_H_ */
