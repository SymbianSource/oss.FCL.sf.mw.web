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

#include "GenericSimpleGesture.h"
#include "filelogger.h"

using namespace stmGesture ;
using stmUiEventEngine::MUiEventSpeed;


TGenericSimpleGesture::TGenericSimpleGesture(
    TGestureUid uid, const TPoint& loc, int type, const MUiEventSpeed* speedIf) :
    m_location(loc), m_uid(uid), m_type(type), m_details(0), m_speed(speedIf)
{
}

TPoint TGenericSimpleGesture::getLocation()  const
{
    return m_location ;
}

int TGenericSimpleGesture::getType()  const
{
    return m_type ;
}

int TGenericSimpleGesture::getDetails() const
{
    return m_details ;
}

void TGenericSimpleGesture::setName(const TDesC8& aName)
{
    m_name.Set(aName) ;
}

TPtrC8 TGenericSimpleGesture::getGestureName()
{
    return m_name ;
}

TDirectionalGesture::TDirectionalGesture(
    TGestureUid uid, const TPoint& loc, const TPoint& prevLoc,
    const MUiEventSpeed* speedIf, bool logging) :
    TGenericSimpleGesture(uid, loc, 0, speedIf), m_loggingEnabled(logging)
{
    setVector(loc, prevLoc);
}

void TDirectionalGesture::setVector(const TPoint& last, const TPoint& previous)
{
    m_vector = TPoint(last.iX-previous.iX, previous.iY-last.iY) ;  // x > 0 => east, y > 0 => north
    if (m_loggingEnabled)
    {
        LOGARG("CHoveringGesture: x: %d , %d prev: %d, %d", last.iX, last.iY, previous.iX, previous.iY) ;
    }
}

TGestureDirection TDirectionalGesture::getDirection() const
{
    TGestureDirection dir = ENorth ;
    int x = Abs(m_vector.iX) ;
    int y = Abs(m_vector.iY) ;
    if (y <= x/2)
    {
        if (m_vector.iX < 0)
            dir = EWest ;
        else
            dir = EEast ;
    }
    else if (y > x/2 && y <= (x+x/2))
    {
        if (m_vector.iX < 0)
        {
            if (m_vector.iY < 0 )
                dir = ESouthWest ;
            else
                dir = ENorthWest ;
        }
        else
        {
            if (m_vector.iY < 0 )
                dir = ESouthEast ;
            else
                dir = ENorthEast ;
        }
    }
    else if (y > x+x/2)
    {
        if (m_vector.iY < 0)
            dir = ESouth ;
        else
            dir = ENorth ;
    }
    if (m_loggingEnabled)
    {
        LOGARG("CHoveringGesture: x: %d y: %d direction: %d", m_vector.iX, m_vector.iY, dir) ;
    }
    return dir ;
}

TTwoPointGesture::TTwoPointGesture(TGestureUid uid, const TPoint& loc, const TPoint& prevLoc) :
    TDirectionalGesture(uid, loc, prevLoc, NULL),
    m_location2(prevLoc)
{
}

