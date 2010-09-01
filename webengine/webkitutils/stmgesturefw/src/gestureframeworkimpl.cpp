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


#include "gestureframeworkimpl.h"
#include "GestureEngine.h"
#include "statemachine.h"

using namespace stmGesture ;

CGestureFrameworkImpl::CGestureFrameworkImpl()
{
    // No implementation required
}

CGestureFrameworkImpl::~CGestureFrameworkImpl()
{
    m_statemachine->removeUiEventObserver(m_gestureEngine) ;
    delete m_gestureEngine ;
    delete m_statemachine ;
}

CGestureFrameworkImpl* CGestureFrameworkImpl::NewLC()
{
    CGestureFrameworkImpl* self = new (ELeave) CGestureFrameworkImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CGestureFrameworkImpl* CGestureFrameworkImpl::NewL()
{
    CGestureFrameworkImpl* self = CGestureFrameworkImpl::NewLC();
    CleanupStack::Pop(self);
    return self;
}

void CGestureFrameworkImpl::ConstructL()
{
    /*!
     * create the gesture engine and the low level UI event engine
     */
    m_gestureEngine = new (ELeave)CGestureEngine() ;
    m_statemachine = stmUiEventEngine::CStateMachine::NewL() ;
    // TODO: all kinds of default settings for the lower layer, but in the test app
    // they are set from the settings menu so no need now...
    // Add the gesture engine to be the observer of UI events...
    m_statemachine->addUiEventObserver(m_gestureEngine) ;
}
MGestureEngineIf*  CGestureFrameworkImpl::getGestureEngine()
{
    return m_gestureEngine ;
}

void CGestureFrameworkImpl::enableLogging(bool loggingEnabled) 
{
	m_loggingenabled = loggingEnabled;
	if (m_gestureEngine)
	{
		m_gestureEngine->enableLogging(loggingEnabled) ;
	}
}

