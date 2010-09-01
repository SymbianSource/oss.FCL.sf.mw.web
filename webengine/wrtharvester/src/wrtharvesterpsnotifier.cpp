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
 

#include "wrtharvesterpsnotifier.h"
#include "wrtharvester.h"
#include <UikonInternalPSKeys.h>//For MMC Observing
#include <startupdomainpskeys.h> // For shutdown observer
#include <UsbWatcherInternalPSKeys.h>// for Mass storage mode observing
#include <usbpersonalityids.h>

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Creates an instance of CCHPluginNotifier implementation
// ---------------------------------------------------------------------------
//
CWrtHarvesterPSNotifier* CWrtHarvesterPSNotifier::NewL( 
					CWrtHarvester* aHarvester, TPropertyKeys aKey )
    {
    CWrtHarvesterPSNotifier* self = new (ELeave) CWrtHarvesterPSNotifier( 
            aHarvester, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWrtHarvesterPSNotifier::~CWrtHarvesterPSNotifier()
    {
    Cancel();
    iProperty.Close();
    }

// -----------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::CCHPluginNotifier
// C++ default constructor
// -----------------------------------------------------------------------------
//
CWrtHarvesterPSNotifier::CWrtHarvesterPSNotifier(
		CWrtHarvester* aHarvester, TPropertyKeys aKey ) : CActive( EPriorityNormal )
    {    
    iHarvester = aHarvester;
    iKey = aKey;
    }

// -----------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CWrtHarvesterPSNotifier::ConstructL()
    {
    CActiveScheduler::Add( this );
    TInt r(KErrNone);
    if( iKey != EWidgetMassStorageMode  )
    	{	
    	// define property to be integer type
    	r = RProperty::Define( KPropertyCat, iKey, RProperty::EInt );
    	}

    if ( r != KErrAlreadyExists && r != KErrNone )
        {
        User::LeaveIfError( r );
        }
    // Attach the key
    if( iKey == EWidgetMassStorageMode )
        {
        User::LeaveIfError( iProperty.Attach( KPSUidUsbWatcher,KUsbWatcherSelectedPersonality));
        }    	
    else if( iKey == EWidgetSystemShutdown )
        {
        User::LeaveIfError( iProperty.Attach( KPSUidStartup,KPSGlobalSystemState  ));
        }    	
    else
    	{
    	User::LeaveIfError( iProperty.Attach( KPropertyCat, iKey));
   	 	}
    
    
    // Set the initial value to indicate that first widget can be launched
    if( iKey == EWidgetUIState )
        {
        User::LeaveIfError( iProperty.Set( 1 ));
        }

    // initial subscription
    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CWrtHarvesterPSNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::RunL
// ---------------------------------------------------------------------------
//
void CWrtHarvesterPSNotifier::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe( iStatus );
    SetActive();
    
    TInt value( 0 );
    TInt r (KErrNone);
    
    if( iKey != EWidgetMassStorageMode )
    	{
    		if (iKey == EWidgetSystemShutdown)
    	        GetValue( value);
    	    else
    	        iProperty.Get( KPropertyCat, iKey, value );	
    	}    
    else
        {        
        r = iProperty.Get( value );
        }

#ifdef _DEBUG    
    RDebug::Print(_L("KEY %d   VALUE %d "),iKey,value);        
#endif
        
    if( r == KErrNone )
        {
        if( iKey == EWidgetUIState && value == 1 )
            {
            iHarvester->TryLaunchNextOperationL();
            }
        else if(iKey == EWidgetUIState && value == 2)
            {
            iHarvester->ClearAllOperations();
            SetValue(1);
            }
        else if( iKey == EWidgetUIState && value == 3 )
            {            
            iHarvester->SetReinstallWidget(ETrue);
            }    
        else if( iKey == EWidgetMassStorageMode  )
            {
            if( value == KUsbPersonalityIdMS )
                {
#ifdef _DEBUG                    
                RDebug::Print(_L("WE are in mass storage mode"));
#endif                    
                iHarvester->SetMSMode(1);    
                }
                else
                {
#ifdef _DEBUG                    
                RDebug::Print(_L("NO LONGER in mass storage mode"));
#endif                    
                iHarvester->SetMSMode(0);
                }
            }
        else if( iKey == EWidgetRegAltered && value == 1 )
            {
            if(iHarvester->IsInMSMode())
                {
                iHarvester->SetRegistryAccess(EFalse);
                }
            else
            	{
            	iHarvester->SetRegistryAccess(ETrue);
                } 	
            iHarvester->UpdateL();
            }
		else if( iKey == EWidgetSystemShutdown && value == ESwStateShuttingDown  )
            {
             iHarvester->SetSystemShutdown(ETrue);
            }
        }
    }

// ---------------------------------------------------------------------------
// CCHPluginNotifier::RunError
// ---------------------------------------------------------------------------
//
TInt CWrtHarvesterPSNotifier::RunError(TInt /*aError*/)
    {
    if ( !IsActive())
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCHPluginNotifier::Value
// ---------------------------------------------------------------------------
//
TInt CWrtHarvesterPSNotifier::GetValue( TInt& aValue)
    {
    return iProperty.Get( aValue );
    }

// ---------------------------------------------------------------------------
// CWrtHarvesterPSNotifier::Value
// ---------------------------------------------------------------------------
//
TInt CWrtHarvesterPSNotifier::SetValue( TInt aValue )
    {
    return iProperty.Set( aValue );
    }

// End of File
