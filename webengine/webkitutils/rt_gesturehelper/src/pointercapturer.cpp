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
* Description:  Alf control group and control for capturing pointer events
*
*/


#include "pointercapturer.h"

#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/alfroster.h>

#include "gesturehelperimpl.h"

namespace RT_GestureHelper
{

/**
 * CPointerCaptureControl
 * Gesture control own a gesture helper, and captures any pointer events that 
 * no other control captures. After it captures pointer down event, it will 
 * capture all pointer events until pointer up. It does this by bringing its
 * control group to the top upon pointer down, and lowering the control group
 * to the botton upon pointer up.
 */
NONSHARABLE_CLASS( CPointerCaptureControl ) : public CAlfControl
    {
public: 
    /** Two-phased constructor */
    static CPointerCaptureControl* NewLC( CGestureHelperImpl& aHelper, 
            CAlfEnv& aEnv, CAlfDisplay& aDisplay )
        {
        CPointerCaptureControl* self = new ( ELeave ) CPointerCaptureControl( aHelper );
        CleanupStack::PushL( self );
        self->ConstructL( aEnv ); // construct base class
        self->BindDisplay( aDisplay );
        self->AppendVisualL( EAlfVisualTypeVisual );       
        return self;
        }
    
    void StartCaptureL()
        {
        User::LeaveIfError( Display()->Roster().SetPointerEventObservers( 
            EAlfPointerEventReportDrag | EAlfPointerEventReportUnhandled, 
                *this ) );
        Display()->Roster().ShowL( *ControlGroup(), KAlfRosterShowAtTop );
        }
    
    void StopCapture()
        {
        ( void ) Display()->Roster().SetPointerEventObservers( 0, *this );
        TRAP_IGNORE( Display()->Roster().ShowL( *ControlGroup(), KAlfRosterShowAtBottom ) );
        }
    
private:
    /** Constructor */
    CPointerCaptureControl( CGestureHelperImpl& aHelper )
            : iHelper( aHelper )
        {
        }
    
    // From CAlfControl
    TBool OfferEventL( const TAlfEvent& aEvent )
        {
        TBool consumed = EFalse;
        if ( aEvent.IsPointerEvent() )
            {
            // pointer down should never come here if everything is working properly. 
            // if pointer event comes here, stop capturing and allow the event to 
            // fall to the controls below
            if ( aEvent.PointerDown() )
                {
                StopCapture();
                }
            else 
                {
                consumed = iHelper.HandlePointerEventL( aEvent.PointerEvent() );
                }
            }
        return consumed;
        }
    
private:    
    /// gesture helper that analyses pointer sequences. own.
    CGestureHelperImpl& iHelper;
    };
    
} // namespace RT_GestureHelper

using namespace RT_GestureHelper;
    
// ----------------------------------------------------------------------------
// Two phase constructor
// ----------------------------------------------------------------------------
//
CPointerCapturer* CPointerCapturer::NewL()
    {
    return new ( ELeave ) CPointerCapturer();
    }

// ----------------------------------------------------------------------------
// Two phase constructor
// ----------------------------------------------------------------------------
//
CPointerCapturer::~CPointerCapturer()
    {
    if ( iCaptureControl )
        {
        // deleting the control group will also delete the control
        iCaptureControl->Env().DeleteControlGroup( iControlGroupId );
        }
    }

// ----------------------------------------------------------------------------
// InitForAlfredL
// ----------------------------------------------------------------------------
//
void CPointerCapturer::InitForAlfredL( CGestureHelperImpl& aHelper, 
        CAlfEnv& aEnv, CAlfDisplay& aDisplay, TInt aControlGroupId )
    {
    CAlfControlGroup& group = aEnv.NewControlGroupL( aControlGroupId );
    CPointerCaptureControl* control = CPointerCaptureControl::NewLC( aHelper, 
        aEnv, aDisplay );
    group.AppendL( control );
    CleanupStack::Pop( control );    
   
    // store control only after it has been successfully added to group
    iCaptureControl = control;
    iControlGroupId = aControlGroupId;
    }

// ----------------------------------------------------------------------------
// StartCaptureL
// ----------------------------------------------------------------------------
//
void CPointerCapturer::StartL()
    {
    if ( iCaptureControl )
        {
        iCaptureControl->StartCaptureL();
        }
    }

// ----------------------------------------------------------------------------
// StopCapture
// ----------------------------------------------------------------------------
//
void CPointerCapturer::Stop()
    {
    if ( iCaptureControl )
        {
        iCaptureControl->StopCapture();
        }
    }
