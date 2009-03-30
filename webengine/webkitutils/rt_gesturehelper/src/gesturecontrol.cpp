/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Gesture control
*
*/


// class include
#include "rt_gesturecontrol.h"

// system includes
#include <alf/alfevent.h>

// local includes
#include "rt_gesturehelper.h"

using namespace RT_GestureHelper;

// ----------------------------------------------------------------------------
// Two-phase constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CGestureControl* CGestureControl::NewLC( MGestureObserver& aObserver,
        CAlfEnv& aEnv, CAlfDisplay& aDisplay, TInt aFreeControlGroupId )
    {
    CGestureControl* self = new ( ELeave ) CGestureControl;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aEnv, aDisplay, aFreeControlGroupId );
    // create a full screen visual to capture pointer events anywhere on screen
    self->AppendVisualL( EAlfVisualTypeVisual );
    return self;
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CGestureControl::CGestureControl()
    {
    // do nothing
    }

// ----------------------------------------------------------------------------
// Second-phase constructor
// ----------------------------------------------------------------------------
//
EXPORT_C void CGestureControl::ConstructL( MGestureObserver& aObserver,
        CAlfEnv& aEnv, CAlfDisplay& aDisplay, TInt aFreeControlGroupId )
    {
    CAlfControl::ConstructL( aEnv );
    iHelper = CGestureHelper::NewL( aObserver );
    iHelper->InitAlfredPointerCaptureL( aEnv, aDisplay, aFreeControlGroupId );
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
EXPORT_C CGestureControl::~CGestureControl()
    {
    delete iHelper;
    }

// ----------------------------------------------------------------------------
// OfferEventL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CGestureControl::OfferEventL( const TAlfEvent& aEvent )
    {
    return iHelper->OfferEventL( aEvent );
    }

// ----------------------------------------------------------------------------
// SetHoldingEnabled
// ----------------------------------------------------------------------------
//
EXPORT_C void CGestureControl::SetHoldingEnabled( TBool aEnabled )
    {
    iHelper->SetHoldingEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsHoldingEnabled
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CGestureControl::IsHoldingEnabled() const
    {
    return iHelper->IsHoldingEnabled();
    }

// ----------------------------------------------------------------------------
// SetDoubleTapEnabled
// ----------------------------------------------------------------------------
//
EXPORT_C void CGestureControl::SetDoubleTapEnabled( TBool aEnabled )
    {
    iHelper->SetDoubleTapEnabled( aEnabled );
    }

// ----------------------------------------------------------------------------
// IsDoubleTapEnabled
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CGestureControl::IsDoubleTapEnabled() const
    {
    return iHelper->IsDoubleTapEnabled();
    }
