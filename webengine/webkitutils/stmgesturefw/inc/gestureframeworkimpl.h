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

#ifndef GESTUREENGINEIMPL_H
#define GESTUREENGINEIMPL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// forward declarations
namespace stmUiEventEngine
{
    class CStateMachine ;
    class MStateMachine ;
}

// CLASS DECLARATION
namespace stmGesture
{
// Forward declarations
class CGestureEngine ;
class MGestureEngineIf ;

/**
 *  CGestureFrameworkImpl
 *
 */
NONSHARABLE_CLASS( CGestureFrameworkImpl ): public CBase
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CGestureFrameworkImpl();

    /**
     * Two-phased constructor.
     */
    static CGestureFrameworkImpl* NewL();

    /**
     * Two-phased constructor.
     */
    static CGestureFrameworkImpl* NewLC();
    /*!
     *
     */
    MGestureEngineIf* getGestureEngine() ;
    /*!
     *
     */
    stmUiEventEngine::CStateMachine* getUiStateMachine() {return m_statemachine; }

	void enableLogging(bool loggingEnabled) ;
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CGestureFrameworkImpl();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    CGestureEngine* m_gestureEngine ;
    stmUiEventEngine::CStateMachine*  m_statemachine ;
	bool m_loggingenabled ;

};
} // namespace
#endif // GESTUREENGINEIMPL_H
