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
* Description:   State Engine Configuration
*
*/

#ifndef STATEENGINECONFIGURATION_H_
#define STATEENGINECONFIGURATION_H_

#include "rt_uievent.h"
#include "timerinterface.h"
#include "statemachine_v2.h"


namespace stmUiEventEngine
{
class CUiEventSender;


NONSHARABLE_CLASS( CStateEngineConfiguration ): public CBase
{
public:
    CStateEngineConfiguration() ;
    void ConstructL();
    ~CStateEngineConfiguration() ;

    void setTouchTimeArea(const long fingersize_mm) ;
    void setTouchArea(const long fingersize_mm) ;
    TAreaShape getTouchAreaShape() ;
    void setTouchAreaShape(const TAreaShape shape) ;
    unsigned int getTouchTimeout() ;
    void setTouchTimeout(unsigned int) ;
    void setHoldArea(const long fingersize_mm) ;
    TAreaShape getHoldAreaShape() ;
    void setHoldAreaShape(const TAreaShape shape) ;
    unsigned int getHoldTimeout() ;
    void setHoldTimeout(unsigned int a) ;
    unsigned int getTouchSuppressTimeout() ;
    void setTouchSuppressTimeout(unsigned int a) ;
    unsigned int getMoveSuppressTimeout() ;
    void setMoveSuppressTimeout(unsigned int a) ;
    bool addUiEventObserver(MUiEventObserver* observer) ;
    bool removeUiEventObserver(MUiEventObserver* observer) ;
    void enableLogging(bool a) ;

private:
    void setTolerance(long fingersize_mm, TPoint& tolerance, TAreaShape shape) ;

public:
    CUiEventSender* m_uiEventSender ;

    TAreaShape m_touchAreaShape ;
    TAreaShape m_holdAreaShape ;
    bool       m_enableLogging ;
    TPoint m_touchTimeTolerance ;
    TPoint m_touchTolerance ;
    TPoint m_holdTolerance ;
    int  m_touchTimerLimit ;
    int  m_holdTimerLimit ;
    int  m_suppressTimerLimit ;
    int  m_moveSuppressTimerLimit ;


};

} // namespace stmUiEventEngine

#endif /* STATEENGINECONFIGURATION_H_ */
