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
* Description:  Gesture helper implementation
*
*/

#include "GestureEngine.h"

#include "filelogger.h"


using namespace stmGesture ;

CGestureEngine::CGestureEngine()
{
    m_numOfActiveStreams = 0 ;
    m_currentGestureOwner = -1 ;
    m_currentLockedGesture = -1 ;
    for (int i = 0; i < stmUiEventEngine::KMaxNumberOfPointers; i++)
    {
        m_uiEventStream[i] = NULL ;
    }
}

CGestureEngine::~CGestureEngine()
{
    m_gestures.Reset() ;
}

bool CGestureEngine::addGesture(const MGestureRecogniserIf* aNewGesture)
{
    // Add the new gesture recogniser to our list of recognisers
    return m_gestures.Append(aNewGesture) == 0;
}

bool CGestureEngine::insertGesture(const MGestureRecogniserIf* aNewGesture)
{
    return insertGestureAt(aNewGesture, 0) == 0;
}

bool CGestureEngine::insertGestureAt(const MGestureRecogniserIf* aNewGesture, int position)
{
    return m_gestures.Insert(aNewGesture, position) == 0 ;
}

bool CGestureEngine::removeGesture(const MGestureRecogniserIf* aOldGesture)
{
    // If gestures are removed, there cannot be current gesture owner...
    if (m_currentGestureOwner != -1)
    {
        MGestureRecogniserIf* pgrif = m_gestures[m_currentGestureOwner] ;
        pgrif->release(this) ;
        m_currentGestureOwner = -1 ;    // no more gesture owners...
    }
    TInt ix = m_gestures.Find(aOldGesture) ;
    bool found = (ix != -1);
    if (found)
    {
        m_gestures.Remove(ix) ;
    }
    return found ;
}

int CGestureEngine::activeStreamCount() const
{
    return m_numOfActiveStreams ;
}

const stmUiEventEngine::MUiEvent* CGestureEngine::getUiEvents(int indexOfActiveStream) const
{
#if defined(ADVANCED_POINTER_EVENTS)
	// create temporary array of active event streams and initialize with zero
	const stmUiEventEngine::MUiEvent* activeEventPointers[stmUiEventEngine::KMaxNumberOfPointers] ;
	for (int x = 0; x < stmUiEventEngine::KMaxNumberOfPointers; x++) activeEventPointers[x] = 0 ;
	// then fill from currently active event streams
	int indextoactiveEventPointers = 0 ;
    for (int i = 0; i < stmUiEventEngine::KMaxNumberOfPointers; i++)
    {
    	if (m_uiEventStream[i])
    	    activeEventPointers[indextoactiveEventPointers++] = m_uiEventStream[i] ;
    }
	// then return the active event stream asked
    return activeEventPointers[indexOfActiveStream] ;
#else
    // in single touch it is enough to return the only possible pointer
    return m_uiEventStream[indexOfActiveStream] ;
#endif
}

/*!
 * Process the UI events
 */
void CGestureEngine::HandleUiEventL(const stmUiEventEngine::MUiEvent& aEvent )
{
    // process one incoming UI event
    storeUiEvent(aEvent) ;  // store the event to the "stream" based on the index of pointer
    walkTroughGestures() ;  // and walk trough the gestures to process the UI event
    updateUiEvents() ;
    // If it was last release event, make sure no-one has the gestures locked
    m_numOfActiveStreams = 0 ;
    for (int i = 0; i < stmUiEventEngine::KMaxNumberOfPointers; i++)
    {
        if (m_uiEventStream[i]) m_numOfActiveStreams++ ;
    }
    if (m_numOfActiveStreams == 0)
    {
        if (m_currentLockedGesture != -1)
        {
            MGestureRecogniserIf* pgrif = m_gestures[m_currentLockedGesture] ;
            pgrif->release(this) ;
        }
        m_currentLockedGesture = -1 ;
    }
}

/*!
 * Store the UI event.  There are max X "streams" of events, one for each
 * pointer.  The streams are actually just pointers to the latest event, since the
 * MUiEvent interface has methods to walk trough the chain of events.
 */
void CGestureEngine::storeUiEvent(const stmUiEventEngine::MUiEvent& aEvent)
{
    m_uiEventStream[aEvent.Index()] = &aEvent ;
    m_numOfActiveStreams = 0 ;
    for (int i = 0; i < stmUiEventEngine::KMaxNumberOfPointers; i++)
    {
        if (m_uiEventStream[i]) m_numOfActiveStreams++ ;
    }
}

/*!
 *  Call each gesture handler in turn until one claims to be in control of the gesture.
 */
void CGestureEngine::walkTroughGestures()
{
    int newowner = -1 ;
    int newlocker = m_currentLockedGesture ;
    // check if someone has locked the gesture
    TGestureRecognitionState thestate = ENotMyGesture ;
    if (m_currentLockedGesture != -1)
    {
        MGestureRecogniserIf* pgrif = m_gestures[m_currentLockedGesture] ;
        if (pgrif)
        {
            if (m_loggingEnabled)
            {
                // log entry about locked gesture (hmm.. should have added names to the MGestureRecogniserIf
                LOGARG("locked gesture recognizer %d (addr %d), active streams %d",
                        m_currentLockedGesture, pgrif, m_numOfActiveStreams);
            }

            thestate = pgrif->recognise(m_numOfActiveStreams, this) ;
            switch (thestate)
            {
            case EGestureActive:
            {
                // This gesture recogniser owns the gesture, so release the lock
                newlocker = -1 ;
                newowner = m_currentLockedGesture ;
                if (m_loggingEnabled)
                {
                    LOGARG("new owner %d lock release", m_currentLockedGesture);
                }
                break;
            }
            case ELockToThisGesture:
            {
                // this gesture recogniser wants to keep the lock
                newowner = m_currentLockedGesture ;
                newlocker = m_currentLockedGesture ;
                if (m_loggingEnabled)
                {
                    LOGARG("new owner %d keep lock", m_currentLockedGesture);
                }
                break;

            }
            case ENotMyGesture:
            {
                break;
            }
            }
        }
        else
        {
            if (m_loggingEnabled)
            {
                LOGARG("NULL recogniser for %d", m_currentLockedGesture);
            }
        }
    }

    if (thestate == ENotMyGesture)
    {
        if (m_loggingEnabled)
        {
            LOGARG("walk trough recognizers active streams %d", m_numOfActiveStreams);
        }
        // No locking gesture, walk trough the list until someone handles this
        for (int i = 0; i < m_gestures.Count(); i++)
        {
            bool controlObtained = false;
            MGestureRecogniserIf* pgrif = m_gestures[i];
            if (pgrif)
            {
                switch (pgrif->recognise(m_numOfActiveStreams, this))
                {
                case EGestureActive:
                {
                    // This gesture recogniser owns the gesture, stop looping...
                    controlObtained = true;
                    newowner = i;
                    break;
                }
                case ELockToThisGesture:
                {
                    // this gesture recogniser wants to take ownership
                    controlObtained = true;
                    newowner = i;
                    newlocker = i;
                    break;

                }
                case ENotMyGesture:
                {
                    break;
                }
                }
            }
            if (controlObtained)
            {
                break; // do not process rest of the gestures
            }

        }
    }
    if (newowner != -1 && newowner != m_currentGestureOwner)
    {
        if (m_currentGestureOwner != -1)
        {
            MGestureRecogniserIf* pgrif = m_gestures[m_currentGestureOwner] ;
            pgrif->release(this) ;
        }
        m_currentGestureOwner = newowner ;
    }
    m_currentLockedGesture = newlocker ;    // if someone locked it or released the lock
}

void CGestureEngine::updateUiEvents()
{
    for (int i = 0; i < stmUiEventEngine::KMaxNumberOfPointers; i++)
    {
        if (m_uiEventStream[i])
        {
            if (m_uiEventStream[i]->Code() == stmUiEventEngine::ERelease)
            {
                // Event can be removed since Release is the last event
                // note that it is the lower layer event engine
                // which actually deletes the object
                m_uiEventStream[i] = NULL ;
            }
        }
    }
}
