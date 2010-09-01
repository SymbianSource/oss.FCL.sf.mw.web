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

#ifndef GESTUREFRAMEWORK_H
#define GESTUREFRAMEWORK_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "rt_gestureengineif.h"
#include <rt_gestureif.h>
#include <rt_gesturelistener.h>

// external classes
namespace stmUiEventEngine
{
    class CStateMachine ;
}

// CLASS DECLARATION
namespace stmGesture
{
// Forward declarations
class CGestureFrameworkImpl ;

/*!
  *  Gesture framework contains the following elements
 *  \li UI State Machine.  This is the lower level driver handling the events and generating UI events.
 *  Gesture framework creates an instance of the UI state machine and adds itself to be the UI event listener.
 *  \li Gesture Engine.  The gesture engine processes the UI events received via MUiEventObserver
 *  and calls gesture recogniser interface MGestureRecogniserIf of all the gestures in the list.
  *  \li Gesture Recognisers.  Gesture recognisers scan the list of UI events and if they recognise
 *  a gesture, they call the MGestureListener.
 */
NONSHARABLE_CLASS( CGestureFramework ): public CBase
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CGestureFramework();

    /**
     * Two-phased constructor.
     */
    static CGestureFramework* NewL();

    /**
     * Two-phased constructor.
     */
    static CGestureFramework* NewLC();
public:
    // new functions,
    static TVersion Version();
    /*!
     * Get the gesture engine interface
     */
    MGestureEngineIf* getGestureEngine() ;
    /*!
     * Get the underlying low level UI event interface
     */
    stmUiEventEngine::CStateMachine* getUiStateMachine() ;
    /*!
     * logging on or off
     */
    void enableLogging(bool loggingEnabled);

private:

    /*!
     * Constructor for performing 1st stage construction
     */
    CGestureFramework();

    /*!
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

private:
    CGestureFrameworkImpl* m_impl ;

    bool m_loggingenabled ;

};

} // namespace
#endif // GESTUREFRAMEWORK_H
