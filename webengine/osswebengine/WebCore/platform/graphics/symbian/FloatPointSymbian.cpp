/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "config.h"
#include "FloatPoint.h"
#include <e32cmn.h>

namespace WebCore {

FloatPoint::FloatPoint(const TPoint& p)
    : m_x(p.iX)
    , m_y(p.iY)
{
}

FloatPoint::operator TPoint() const
{
    return TPoint(m_x, m_y);
}

}
