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

#include "utils.h"

#include <coemain.h> // for CCoeEnv
#include <eikappui.h>  // for CEikAppUi

#include "gesturedefs.h"

namespace stmUiEventEngine
{
    /** @return the longer edge of the size */
    inline TInt LongerEdge( const TSize& aSize )
        {
        return Max( aSize.iHeight, aSize.iWidth );
        }

    // for documentation, see header file
    TRect ToleranceRect( const TPoint& aCenterPoint )
        {
        long toleranceLength = Mm2Pixels(KFingerSize_mm) / 2;
        TRect toleranceRect( aCenterPoint, TSize() );
        // grow by the tolerance length, while keeping the center point
        toleranceRect.Grow( toleranceLength, toleranceLength );
        return toleranceRect;
        }

    long Twips2Pixels(long twips)
        {
        CWsScreenDevice* screen = CCoeEnv::Static()->ScreenDevice();
        TZoomFactor deviceMap(screen);
        deviceMap.SetZoomFactor(TZoomFactor::EZoomOneToOne);
        long px = deviceMap.VerticalTwipsToPixels(twips); //assuming that vertical
        return px;                                        //the same as horizontal

        }

    long Mm2Pixels(long mm)
        {
        return Twips2Pixels(mm * KTwipsInMm);
        }

    long Inches2Pixels(float inches)
        {
        return Twips2Pixels(inches * KTwipsInInch);
        }


} // namespace
