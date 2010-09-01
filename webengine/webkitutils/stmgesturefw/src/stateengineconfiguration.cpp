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

#include "stateengineconfiguration.h"
#include "uieventsender.h"
#include "utils.h"

using namespace stmUiEventEngine ;

CStateEngineConfiguration::CStateEngineConfiguration() :
	m_touchAreaShape(ERectangle),
	m_holdAreaShape(ERectangle)
{
}

void CStateEngineConfiguration::ConstructL()
{
    m_uiEventSender = CUiEventSender::NewL() ;
}

CStateEngineConfiguration::~CStateEngineConfiguration()
{
    delete m_uiEventSender ;
}

void CStateEngineConfiguration::setTolerance(long fingersize_mm, 
                                             TPoint& tolerance, 
                                             TAreaShape shape)
{
    long s = Mm2Pixels(fingersize_mm) / 2;
    switch(shape)
    {
    case EEllipse:
        tolerance.iX = (s * 2) / 3;
        tolerance.iY = s;
        break ;
    case ERectangle:
    case ECircle:
    default:
        tolerance.iX = s;
        tolerance.iY = s;
        break ;
    }
}

/*!
 * Set the new touch limits for calculating the touch area.
 * The size is given in millimetres.  The shape of the touch area
 * defines how the parameter is used.
 * Rectangle: each side of the rectangle has length of fingersize_mm.
 * Circle:    the parameter defines the diameter of the cicle.
 * Ellipse:   the parameter defines the vertical diameter of the ellipse,
 *            horizontal diameter is half of it.
 */
void CStateEngineConfiguration::setTouchTimeArea(long fingersize_mm)
{
    setTolerance(fingersize_mm, m_touchTimeTolerance, m_touchAreaShape);

    // make sure that both touch areas are defined, so if the touch area is not yet set,
    // use the same as for touch time area.
    if (m_touchTolerance.iX == 0)
    {
        setTouchArea(fingersize_mm) ;
    }
}
/*!
 * Set the new touch limits for calculating the touch area.
 * The size is given in millimetres.  The shape of the touch area
 * defines how the parameter is used.
 * Rectangle: each side of the rectangle has length of fingersize_mm.
 * Circle:    the parameter defines the diameter of the cicle.
 * Ellipse:   the parameter defines the vertical diameter of the ellipse,
 *            horizontal diameter is half of it.
 */
void CStateEngineConfiguration::setTouchArea(long fingersize_mm)
{
    setTolerance(fingersize_mm, m_touchTolerance, m_touchAreaShape);
}
/*!
 * \return the touch are shape
 */
TAreaShape CStateEngineConfiguration::getTouchAreaShape()
{
     return m_touchAreaShape ;
}
/*!
 * \parameter the touch are shape
 */
void CStateEngineConfiguration::setTouchAreaShape(TAreaShape aShape)
{
    this->m_touchAreaShape = aShape ;
}
/*!
 * \return the touch timeout in microseconds.
 */
unsigned int CStateEngineConfiguration::getTouchTimeout()
{
    return m_touchTimerLimit ;
}
/*!
 * set the touch timeout in microseconds.
 */
void CStateEngineConfiguration::setTouchTimeout(unsigned int a)
{
    m_touchTimerLimit = a ;
}
/*!
 * Set the hold area.  Hold are shape determines how exactly the area is handled.
 * \sa setTouchArea
 */
void CStateEngineConfiguration::setHoldArea(long fingersize_mm)
{
    setTolerance(fingersize_mm, m_holdTolerance, m_holdAreaShape);
}
/*!
 * Get the shape of the hold area: Rectangle, Circle or Ellipse,
 */
TAreaShape CStateEngineConfiguration::getHoldAreaShape()
{
     return m_holdAreaShape ;
}
/*!
 * Set the shape of the hold area: Rectangle, Circle or Ellipse,
 */
void CStateEngineConfiguration::setHoldAreaShape(TAreaShape aShape)
{
    m_holdAreaShape = aShape ;
}
/*!
 * \return the hold timeout in microseconds.
 */
unsigned int CStateEngineConfiguration::getHoldTimeout()
{
    return m_holdTimerLimit ;
}
/*!
 * set the hold timeout value in microseconds.
 */
void CStateEngineConfiguration::setHoldTimeout(unsigned int a)
{
    m_holdTimerLimit = a ;
}
/*!
 * get the touch suppress timeout value in microseconds.
 */
unsigned int CStateEngineConfiguration::getTouchSuppressTimeout()
{
    return m_suppressTimerLimit ;
}
/*!
 * set the touch suppress timeout.
 * \param the timeout value in microseconds.
 */
void CStateEngineConfiguration::setTouchSuppressTimeout(unsigned int a)
{
    m_suppressTimerLimit = a ;
}
/*!
 * MStateMachine method.
 */
unsigned int CStateEngineConfiguration::getMoveSuppressTimeout()
{
    return m_moveSuppressTimerLimit ;
}
/*!
 * MStateMachine method.
 */
void CStateEngineConfiguration::setMoveSuppressTimeout(unsigned int a)
{
    m_moveSuppressTimerLimit = a ;
}
/*!
 * MStateMachine method.
 * Sets logging on or off.
 */
void CStateEngineConfiguration::enableLogging(bool a)
{
    m_enableLogging = a ;
    m_uiEventSender->setLogging(a) ;
}

/*!
 * \param a new UI event observer
 */
bool CStateEngineConfiguration::addUiEventObserver(MUiEventObserver* observer)
{
    // The event sender handles the observers
    return m_uiEventSender->addObserver(observer) ;
}
/*!
 * MStateMachine method.
 */
bool CStateEngineConfiguration::removeUiEventObserver(MUiEventObserver* observer)
{
    // The event sender handles the observers
    return m_uiEventSender->removeObserver(observer) ;
}

