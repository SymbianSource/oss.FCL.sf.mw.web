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

#ifndef GESTUREENGINE_H_
#define GESTUREENGINE_H_

#include <coemain.h>
#include <e32property.h>
#include <w32std.h>

#include "rt_gestureengineif.h"
#include "rt_uievent.h"

namespace stmGesture
{

/*!
 * CGEstureEngine handles the processing of the UI events.
 * It acts as a stmUiEventEngine::MUiEventObserver to receive the UI events.
 * The gesture recognisers are stored in a list, and at each HandleUiEventL the
 * list of gesture recognisers is processed.  The first one to return EGestureActive
 * from the recognise method "owns" the gesture.  If some other recogniser owned it
 * previously, its release method is called. Gesture recogniser can also lock the gesture
 * by returning ELockToThisGesture. Then only that gesture recogniser will be called
 * until release is detected or the recogniser returns something else than ELockToThisGesture.
 */
NONSHARABLE_CLASS( CGestureEngine ): public CBase, 
                                     public MGestureEngineIf, 
                                     public stmUiEventEngine::MUiEventObserver
{
public:
    CGestureEngine();
    virtual ~CGestureEngine();

    /*!
     *  add gesture to the end of the list of gestures
     */
    virtual bool addGesture(const MGestureRecogniserIf* aNewGesture) ;
    /*!
     * inset a gesture to the beginning of the list of gestures
     */
    virtual bool insertGesture(const MGestureRecogniserIf* aNewGesture) ;
    /*!
     * inset a gesture to the specific position
     */
    virtual bool insertGestureAt(const MGestureRecogniserIf* aNewGesture, int position) ;
    /*!
     * remove a gesture from the list
     */
    virtual bool removeGesture(const MGestureRecogniserIf* aOldGesture) ;
    /*!
     * get the number of non-empty event streams.
     * Event streams correspond UI events generated by one touch pointer (=finger).
     * The low level state machine handles the necessary filtering etc.
     * so that it is safe to remove the event stream after UI Release event has been processed.
     */
    virtual int activeStreamCount() const ;
    /*!
     * get the UI events of stream X
     * \param indexOfActiveStream defines which active stream is used.
     * Note that MUiEvent contains all the events from down up to the current event.
     * Assumption: the UI events contain the target "window handle", i.e.
     * the gesture recognition needs to be aware of all possible windows of the application.
     */
    virtual const stmUiEventEngine::MUiEvent* getUiEvents(int indexOfActiveStream) const ;
	/*!
	 * enable/disable logging
	 */
	virtual void enableLogging(bool aLoggingEnabled) {m_loggingEnabled = aLoggingEnabled;} ;
	/*!
     * The stmUiEventEngine::MUiEventObserver interface
     */
    virtual void HandleUiEventL( const stmUiEventEngine::MUiEvent& aEvent ) ;
private:
    /*!
     * The list of available gesture recognisers
     */
    RPointerArray< MGestureRecogniserIf >  m_gestures;
    /*!
     * Is 5 UI event streams enough?  Jos tulee Torvisen voittaja?
     * We need to store only the latest event since the interface has
     * methods to walk trough the events. The current (sept 2009) 9.2 seems to have dual-touch support  
     */
    const stmUiEventEngine::MUiEvent* m_uiEventStream[stmUiEventEngine::KMaxNumberOfPointers] ;

    void storeUiEvent(const stmUiEventEngine::MUiEvent& aEvent) ;
    void walkTroughGestures() ;
    void updateUiEvents() ;
    int m_numOfActiveStreams ;
    int m_currentGestureOwner ;
    int m_currentLockedGesture ;
	bool m_loggingEnabled ;
};

}

#endif /* GESTUREENGINE_H_ */