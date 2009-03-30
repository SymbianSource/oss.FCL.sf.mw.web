/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture helper helper functions
*
*/


#include "rt_gestureobserver.h"

// system includes
#include <alf/alfcontrol.h>
#include <alf/alflayout.h>

using namespace RT_GestureHelper;

namespace 
    {
    /** @return the leaf-most visual that is under aPos */
    const CAlfVisual* VisualByCoordinates( const CAlfVisual& aRoot, const TPoint& aPos )
        {
        // If it has children, chose any of them is hit first
        const CAlfLayout* layout = dynamic_cast< const CAlfLayout* >( &aRoot );
        if ( layout)
            {
            TInt i = layout->Count();
            while( --i >= 0 )
                {
                if( VisualByCoordinates( layout->Visual( i ), aPos ) )
                    {
                    return &layout->Visual( i );
                    }
                }
            }
            
        // children were not hit. is this visual hit?
        if( TRect( aRoot.DisplayRect() ).Contains( aPos ) )
            {
            return &aRoot;
            }
            
        // visual was not found
        return NULL;
        }
        
    TBool IsRootVisual( const CAlfVisual& aVisual )
        {
        return !aVisual.Layout();
        }
    } // unnamed namespace 
    
// ----------------------------------------------------------------------------
// VisualByCoordinates
// ----------------------------------------------------------------------------
//
EXPORT_C CAlfVisual* HitTest::VisualByCoordinates( const CAlfControl& aControl, 
        const TPoint& aPos )
    {
    TInt i = aControl.VisualCount();
    while( --i >= 0 )
        {
        // Ask only root visuals, since all children are within the root visuals
        if ( IsRootVisual( aControl.Visual( i ) ) )
            {
            const CAlfVisual* hitVisual = ::VisualByCoordinates( 
                aControl.Visual( i ), aPos );
            if ( hitVisual )
                {
                return const_cast< CAlfVisual* >( hitVisual );
                }
            }
        }
    return NULL;
    }
