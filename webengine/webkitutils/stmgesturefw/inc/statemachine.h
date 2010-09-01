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
* Description: State Machine implementation
*
*/

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

// INCLUDES
#include <e32base.h>
#include <aknwseventobserver.h>

#include <rt_uievent.h>
#include "timerinterface.h"
#include "statemachine_v2.h"

class CCoeEnv;

namespace stmUiEventEngine
{

class CCallbackTimer;
class CStateEngine;
class THwEvent;
class CStateEngineConfiguration;


int Distance(int dx, int dy);

inline int Distance(const TPoint& aP1, const TPoint& aP2)
{
    return Distance(aP1.iX - aP2.iX, aP1.iY - aP2.iY);
}

// CLASS DECLARATION
/**
 *  CStateMachine
 *
 */
NONSHARABLE_CLASS( CStateMachine ): public CBase, 
                                    public MTimerInterface, 
                                    public MAknWsEventObserver, 
                                    public MStateMachine
{
public:
    /**
     * The MStateMachine interface methods
     */
    virtual TRect getTouchArea(TInt aPointerNumber) ;
    virtual void setTouchTimeArea(long fingersize_mm) ;
    virtual void setTouchArea(long fingersize_mm) ;
    virtual TAreaShape getTouchAreaShape() ;
    virtual void setTouchAreaShape(const TAreaShape shape) ;
    virtual unsigned int getTouchTimeout() ;
    virtual void setTouchTimeout(unsigned int) ;
    virtual TRect getHoldArea(TInt aPointerNumber) ;
    virtual void setHoldArea(long fingersize_mm) ;
    virtual TAreaShape getHoldAreaShape() ;
    virtual void setHoldAreaShape(const TAreaShape shape) ;
    virtual unsigned int getHoldTimeout();
    virtual void setHoldTimeout(unsigned int a);
    virtual unsigned int getTouchSuppressTimeout() ;
    virtual void setTouchSuppressTimeout(unsigned int a) ;
    virtual unsigned int getMoveSuppressTimeout() ;
    virtual void setMoveSuppressTimeout(unsigned int a) ;
    virtual bool addUiEventObserver(MUiEventObserver* observer) ;
    virtual bool removeUiEventObserver(MUiEventObserver* observer) ;
    virtual bool wasLastMessageFiltered(TInt aPointerNumber) ;
    virtual void enableCapacitiveUp(bool enable) ;
    virtual void enableLogging(bool aEnable) ;
    virtual void enableYadjustment(bool aEnable) ;
    virtual int getNumberOfPointers() ;

    /**
     * MAknWsEventObserver methods
     */
    virtual void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination) ;

    /* other methods */
    /**
     * Pointer event handling in case the messaging should be handled that way, note that the target CCoeControl needs to be givem
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent, void* target) ;
    /**
     * Simple toggle changes from handling the messages using ...WsEventL or ...PointerEventL
     */
    void EnableWsEventMonitoring(bool aEnable) ;
    /**
     * Method to handle the TPointerEvent
     */
    bool HandleStateEvent(const TPointerEvent& aPointerEvent, void* aTarget, const TTime& aTime) ;
    /*!
     * Setting the Y adjustment useful in capacitive touch
     * Note that there are problems with the adjustment if done at this level,
     * the most proper place would be the window server.
     */

    /**
     * MTimerInterface methods, these are internal and do not need to be exported
     */
    virtual void startTouchTimer(TInt aDelay, TInt aPointerNumber) ;
    virtual void cancelTouchTimer(TInt aPointerNumber)  ;
    virtual void startHoldTimer(TInt aDelay, TInt aPointerNumber)  ;
    virtual void cancelHoldTimer(TInt aPointerNumber)  ;
    virtual void startSuppressTimer(TInt aDelay, TInt aPointerNumber) ;
    virtual void cancelSuppressTimer(TInt aPointerNumber)  ;

public:
    ~CStateMachine();
    static CStateMachine* NewL();
    static CStateMachine* NewLC();

private:
    CStateMachine();
    void ConstructL();

    inline static TInt PointerIndex(const TPointerEvent& aPointerEvent);

    /**
     * Convert pointer event into our own event format (including timestamp)
     */
    void CreateHwEvent(THwEvent& aEvent, 
                       const TPointerEvent& aPointerEvent, 
                       void* aTarget, 
                       const TTime& aTime) ;
    
    void CreateTimerEvent(THwEvent& aEvent, TStateMachineEvent aEventCode) ;
    
    /**
     * the actual state machines implementing the transitions etc.
     */
    CStateEngine* m_impl[KMaxNumberOfPointers] ;
    /*
     * The configuration for state machines
     */
    CStateEngineConfiguration* m_config ;
    /*!
     * convert window coordinates to screen coordinates.
     */
    TPoint screenCoordinates(const TPoint& aPos, void* aGestureTarget) ;

    CCoeEnv* m_coeEnv;

    bool m_WasMessageFiltered ;
    bool m_wseventmonitoringenabled ;
    bool m_loggingenabled ;
    bool m_capacitiveup ;

    int m_3mminpixels ;
    bool m_adjustYposition ;
    // Use same naming scheme with the timers, and variables and methods
    // using macro expansion tricks (with multitouch support starts to look quite ugly):
#define DECLARE_TIMER(x) \
            void start##x##Timer(TInt aPointerNumber) ;\
            CCallbackTimer* m_##x##Timer[KMaxNumberOfPointers] ;\
            void handle##x##Timer(TInt aPointerNumber) ;\
            void cancel##x##Timer(TInt aPointerNumber)

    DECLARE_TIMER(touch) ;
    DECLARE_TIMER(hold) ;
    DECLARE_TIMER(suppress) ;

};
}   // namespace
#endif // STATEMACHINE_H
