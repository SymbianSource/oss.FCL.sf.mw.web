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
* Description:   State Engine
*
*/
#ifndef STATEENGINE_H_
#define STATEENGINE_H_

#include "rt_uievent.h"
#include "timerinterface.h"
#include "statemachine_v2.h"
#include "uieventsender.h"
#include "stateengineconfiguration.h"

namespace stmUiEventEngine
{

/*!
 * Define THwEvent since we need the point, the type and timestamp to be stored.
 * The HW event is used also to handle the timer events so that the wrapper class calling the
 * state machine will handle the starting, canceling etc. of the timers and the state machine
 * sees the timers as messages.  This hopefully makes it easier to keep the core state machine as
 * OS agnostic as possible.
 */
class THwEvent
{
public:
    THwEvent() {} ;
    THwEvent(const THwEvent& ev):
        iType(ev.iType), iPosition(ev.iPosition),
        iTime(ev.iTime), iTarget(ev.iTarget), iPointerNumber(ev.iPointerNumber) {}
    THwEvent(TStateMachineEvent code, 
             const TPoint& pos, 
             TTime time, 
             void* aTarget, 
             TInt aPointerNr):
             iType(code), iPosition(pos),
             iTime(time), iTarget(aTarget), iPointerNumber(aPointerNr) 
             {
             }

    TStateMachineEvent iType ;
    TPoint  iPosition ;
    TTime   iTime ;
    void*   iTarget ;
	TInt	iPointerNumber ;
};


NONSHARABLE_CLASS( CStateEngine ) : public CBase
{
public:
    /**
     * constructor and destructor
     */
	CStateEngine(CStateEngineConfiguration* aConfig, MTimerInterface* timerif, int index) ;

    ~CStateEngine() ;
    /**
     *  The methods needed for state machine
     */
    void ConsumeEvent() ;
    bool IsTouchTimer() ;
    bool IsHoldTimer() ;
    bool IsSuppressTimer() ;
    bool IsTouchTimeArea() ;
    bool IsTouchArea() ;
    bool IsHoldArea() ;
    bool InsideTouchTimeArea() ;
    bool InsideTouchArea() ;
    bool InsideHoldArea() ;
    bool LooksLikeHold() ;
    void ErrorEvent() ;
    void InitTouchTimer() ;
    void InitHoldTimer() ;
    void RestartHoldTimer() ;
    void InitTouchSuppressTimer() ;
    void InitMoveSuppressTimer() ;
    void ClearTouchTimer() ;
    void ClearHoldTimer() ;
    void ClearSuppressTimer() ;
    void ProduceTouch() ;
    void ProduceMove() ;
    void ProduceRelease() ;
    void ProduceHold() ;
    void RenameToDrag() ;
    void PrepareTouchTimeArea() ;
    void PrepareTouchArea() ;
    void PrepareHoldArea() ;
    void SetCurrentPos() ;
    void SetGestureStart() ;
    void AddToTouch() ;
    void AddDraggingPos() ;
    void StoreMovePos() ;

    /**
     * The event interface
     */

    THwEvent& initEvent()
    {
        m_hwe.iPointerNumber = m_index;
        return m_hwe;
    }

    // event returned by initEvent() must initialized first
	bool handleStateEvent()  ;

	TRect getTouchArea() ;
	TRect getHoldArea() ;
	bool wasLastMessageFiltered() ;

private:
    bool m_eventConsumed ;
    THwEvent m_hwe ;

    bool isNewHoldingPoint() ;
    RPointerArray<THwEvent> iDragPoints;
    MTimerInterface* m_timerif ;

    RArray<THwEvent> iTouchPoints;

    // Variables for running the state machine
    TStateMachineState m_currentState ;
    bool m_isTimerEvent ;                   // We need to separate the pointer events and the timer events
    TPoint m_currentTouchXY ;               // This is the touching point
    TPoint m_gestureStartXY ;               // gesture started at this point
    TPoint m_uiEventXY ;                    // The UI event XY point.
    TPoint m_previousPointerEventPosition ;
    TPoint m_deltaVector ;
    TRect m_touchRect ;
    TRect m_holdRect ;

    bool m_wasFiltered ;
    void CalculateDelta() ;
    void CalculateTouchAverage() ;
    void CalculateCurrentVector() ;
    TPoint calculateTouchAverageFromPoints() ;
    bool InsideArea(const TPoint& point, 
                    const TRect& rect, 
                    TAreaShape shape, 
                    const TPoint& tolerance);
    void setTolerance(long fingersize_mm, TPoint& tolerance, TAreaShape shape) ;

    void DebugPrintState(TStateMachineState anextstate) ;
    inline static TRect ToleranceRect( const TPoint& aCenterPoint, const TPoint& tolerance) ;
    void turnStateMachine() ;

    TPoint     m_touchCentre ;
    TPoint     m_holdCentre ;

    void*       m_gestureTarget ;
    TTimeIntervalMicroSeconds getInterval() ;
    TTime       m_lastMessageTime ;
    CUiEvent* createUIEventL(TUiEventCode code, const TPoint& aPos) ;
    TPoint getPreviousXY(const TPoint& aXY) ;
    TPoint m_previousXY ;
    bool isTimerMessage() ;
    int  m_index ;
    TUiEventCode m_previousUiGenerated ;
    TPoint m_lastFilteredPosition ;
    TTime m_lastFilteredMessageTime ;

	CStateEngineConfiguration* m_config ;
	bool isStatemachineBlocked;

};
}   // namespace

#endif /* STATEENGINE_H_ */
