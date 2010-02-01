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
* Description:   
*
*/

#ifndef STATEMACHINE_V2_H_
#define STATEMACHINE_V2_H_

namespace stmUiEventEngine
{
/**
 * the next templates define the static callbacks required to pass the this object to the member function
 * There are two definitions, one for the TBool is*() method checking the message,
 * the other one returning void for entry, event and action methods.
 */
template<class T, bool (T::*F)()>
bool isF(void *p)
{
    return ((reinterpret_cast<T*>(p))->*F)();
}

template<class T, void (T::*F)()>
void aF(void *p)
{
    ((reinterpret_cast<T*>(p))->*F)();
}

/////////////////////////////////////////////////////////////////////////////////////
// We define the state machine in C fashion so that we get the initialized state table
// already at compilation phase.  The message checking methods, event methods  and action methods are
// then passed to the C++ object to be processed.
// The currently defined states are the following, at the same time they are used as the index to the array of states.
enum TStateMachineState {
            Eignore, 
            EInit, 
            EDispatch, 
            EInTouchTime, 
            EInHoldTime_U, 
            EInHoldTime_D, 
            EInTouchArea, 
            ESuppress_D
        } ;
// Using these kind of functions the state machine is of course single threaded
// the necessary parameters need to be passed in the member variables
typedef bool  (*condition_t)(void* ) ;
typedef void  (*action_t)(void*) ;

/*
 * The possible events to the state machine.  The pointer and timer events are possible.
 */
enum TStateMachineEvent {
            EDown, 
            EDrag, 
            ECapacitiveUP, 
            EResistiveUP, 
            ETouchTimer, 
            EHoldTimer, 
            ESuppressTimer
        } ;
/*!
 * STATE_ELEMENT defines one line in the state/event instance.
 * It contains three fields: ConditionFunction, ActionFunction and NextState.
 * The generic state machine will call the ConditionFunction (if it is != NULL)
 * and if the result is true, it will call ActionFunction (if it is != NULL).
 * Then it will continue to NextState.
 * If NextState is Eignore, it will try the next line of state/event.
 */
typedef struct _STATE_ELEMENT {
    /*!
     * Condition function contains the pointer to the method used to check some condition.
     * If the pointer is non-NULL the state machine will call the function and based on the result
     * (if true) calls the ActionFunction.
     */
    const condition_t conditionFunction ;
    /*!
     * ActionFunction contains a pointer to a method performing some action.  The state machine
     * will call the method if the pointer is non-NULL.
     */
    const action_t    actionFunction ;
    /*!
     * NextState contains either the next state or Eignore.  The state machine will process state elements
     * until the NextState != Eignroe is found.
     */
    const TStateMachineState  nextState ;
} STATE_ELEMENT ;

/*!
 * Each state contains an array defining the possible event and the state elements
 * that are processed if the event happens.
 */
typedef struct _STATE {
    /*!
     * The event defines the pointer event or timer event being processed
     */
    const TStateMachineEvent    theEvent ;        //
    /*!
     * StateElements points to the array of STATE_ELEMENT entries which define the
     * condition and action functions to be processed and the resulting nesxt state.
     */
    const STATE_ELEMENT* const   stateElements ;
} STATE ;

} // namespace

#endif /* STATEMACHINE_V2_H_ */
