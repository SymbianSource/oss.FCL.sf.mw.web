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
* Description:  
*
*/
 

#include "WidgetInstallerUiObserver.h"
#include "WidgetMMCHandler.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Creates an instance of CWidgetInstallerUiObserver implementation
// ---------------------------------------------------------------------------
//
CWidgetInstallerUiObserver* CWidgetInstallerUiObserver::NewL( 
                    CWidgetRegistry* aRegistry)
    {
    CWidgetInstallerUiObserver* self = new (ELeave) CWidgetInstallerUiObserver( 
            aRegistry );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWidgetInstallerUiObserver::~CWidgetInstallerUiObserver()
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CWidgetInstallerUiObserver::CWidgetInstallerUiObserver
// C++ default constructor
// -----------------------------------------------------------------------------
//
CWidgetInstallerUiObserver::CWidgetInstallerUiObserver(
        CWidgetRegistry* aRegistry ) : CActive( EPriorityNormal )
    {    
    iRegistry = aRegistry;    
    }

// -----------------------------------------------------------------------------
// CWidgetInstallerUiObserver::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CWidgetInstallerUiObserver::ConstructL()
    {
    CActiveScheduler::Add( this );
    TInt r(KErrNone);
    
    // define property to be integer type
    r = RProperty::Define( KPropertyCat, EWidgetInstallUninstallStart, RProperty::EInt );
    

    if ( r != KErrAlreadyExists && r != KErrNone )
        {
        User::LeaveIfError( r );
        }
    
    // Attach the key
    User::LeaveIfError( iProperty.Attach( KPropertyCat, EWidgetInstallUninstallStart));

    // initial subscription
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CWidgetInstallerUiObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CWidgetInstallerUiObserver::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CWidgetInstallerUiObserver::RunL
// ---------------------------------------------------------------------------
//
void CWidgetInstallerUiObserver::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();
    
    TInt value( 0 );
    TInt r (KErrNone);
    
    r = iProperty.Get( KPropertyCat, EWidgetInstallUninstallStart, value );
    
    if( r == KErrNone )
        {
        if( value == 1 )
            iRegistry->MMCHandler()->NeedToNotify(EFalse);
        }    
    }

// ---------------------------------------------------------------------------
// CWidgetInstallerUiObserver::RunError
// ---------------------------------------------------------------------------
//
TInt CWidgetInstallerUiObserver::RunError(TInt /*aError*/)
    {
    if ( !IsActive())
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    return KErrNone;
    }

// End of File
