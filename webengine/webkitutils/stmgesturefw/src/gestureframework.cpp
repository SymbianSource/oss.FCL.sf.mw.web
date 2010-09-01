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

#include "statemachine.h"
#include "gestureframework.h"
#include "gestureframeworkimpl.h"
#include "GestureEngine.h"
#include "rt_gestureengineif.h"
#include "rt_gesturelistener.h"
#include "tapgesturerecogniser.h"
#include "pangesturerecogniser.h"

using namespace stmGesture ;

CGestureFramework::CGestureFramework()
{
    // No implementation required
}

CGestureFramework::~CGestureFramework()
{
    delete m_impl ;
}

CGestureFramework* CGestureFramework::NewLC()
{
    CGestureFramework* self = new (ELeave) CGestureFramework();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CGestureFramework* CGestureFramework::NewL()
{
    CGestureFramework* self = CGestureFramework::NewLC();
    CleanupStack::Pop(self);
    return self;
}

void CGestureFramework::ConstructL()
{
    m_impl = CGestureFrameworkImpl::NewL() ;
}

TVersion CGestureFramework::Version()
{
    // Version number of example API
    const TInt KMajor = 1;
    const TInt KMinor = 0;
    const TInt KBuild = 1;
    return TVersion(KMajor, KMinor, KBuild);
}

/*!
 * Create new gesture engine if not already existing and return the interface
 */
MGestureEngineIf*  CGestureFramework::getGestureEngine()
{
    return m_impl->getGestureEngine() ;
}

stmUiEventEngine::CStateMachine* CGestureFramework::getUiStateMachine()
{
    return m_impl->getUiStateMachine() ;
}

void CGestureFramework::enableLogging(bool loggingEnabled)
{
    m_loggingenabled = loggingEnabled;
    if (m_impl) m_impl->enableLogging(loggingEnabled) ;
}

