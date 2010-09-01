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

#include <e32math.h>
#include "UiEvent.h"
#include "filelogger.h"
#include "statemachine.h" // for stmUiEventEngine::Distance(dx,dy)
//#include "flogger.h"

using namespace stmUiEventEngine ;

static const char* const eventNames[] = {  // for debugging purposes define the names of the UI events
        "noEvent",
        "ETouch",
        "EHold",
        "EDrag",
        "ERelease",
        "ENull"
} ;

const char* stmUiEventEngine::EventName(stmUiEventEngine::TUiEventCode aCode)
{
    return eventNames[aCode];
}

CUiEvent* CUiEvent::NewL(
    TUiEventCode aCode,
    const TPoint& aStart, const TPoint& aXY, const TPoint& aPreviousXY,
    bool aTimerExpired, void* aTarget, TTimeIntervalMicroSeconds aInterval,
    int aIndex, TInt64 aTimestamp)
{
    CUiEvent* self = new (ELeave) CUiEvent(aCode, aStart, aXY, aPreviousXY,
        aTimerExpired, aTarget, aInterval, aIndex, aTimestamp) ;
    return self;
}

CUiEvent::CUiEvent(
    TUiEventCode aCode,
    const TPoint& aStart, const TPoint& aXY, const TPoint& aPreviousXY,
    bool aTimerExpired, void* aTarget, TTimeIntervalMicroSeconds aInterval,
    int aIndex, TInt64 aTimestamp) :
    m_code(aCode), m_start(aStart), m_XY(aXY), m_previousXY(aPreviousXY),
    m_statetransition(aInterval), m_target(aTarget), m_timerExpired(aTimerExpired),
    m_index(aIndex), m_previousEvent(NULL), m_timestamp(aTimestamp)
{
}

CUiEvent::~CUiEvent()
{
    delete m_previousEvent ;
    m_previousEvent = NULL ;
}
const TPoint& CUiEvent::StartPos() const
{
    return m_start ;
}
const TPoint& CUiEvent::CurrentXY() const
{
    return m_XY ;
}
TUiEventCode CUiEvent::Code()const
{
    return m_code ;
}
const TPoint& CUiEvent::PreviousXY() const
{
    return m_previousXY ;
}
TTimeIntervalMicroSeconds CUiEvent::StateTransition() const
{
    return m_statetransition ;
}
bool CUiEvent::TimerExpired() const
{
    return m_timerExpired ;
}
void * CUiEvent::Target() const
{
    return m_target ;
}
int CUiEvent::Index() const
{
    return m_index ;
}
void CUiEvent::setPrevious(CUiEvent* aEvent)
{
    m_previousEvent = aEvent ;
}
MUiEvent* CUiEvent::previousEvent() const
{
    return m_previousEvent ;
}
int CUiEvent::countOfEvents() const
{
    int count = 1 ;
    for(CUiEvent* prev = m_previousEvent; prev; prev = prev->m_previousEvent)
        {
        ++count;
        }
    return count ;
}
TInt64 CUiEvent::timestamp() const
{
    return m_timestamp ;
}

float CUiEvent::speedX() const __SOFTFP
{
    int distX = m_XY.iX - m_previousXY.iX;
    // now calculate speed
    float elapsed = float (m_statetransition.Int64()/1000) ; // use ms as time unit

    float speed = 0;
    if (elapsed == 0) {
        elapsed = 5.0;    // if time is 0, take 5ms
    }
    speed = float(distX)/elapsed ;
    return speed ;
}

float CUiEvent::speedY() const __SOFTFP
{
    int distY = m_XY.iY - m_previousXY.iY;
    // now calculate speed
    float elapsed = float (m_statetransition.Int64()/1000) ; // use ms as time unit
    float speed = 0;
    if (elapsed == 0) {
        elapsed = 5.0;    // if time is 0, take 5ms
    }
    speed = float(distY)/elapsed ;
    return speed ;
}

