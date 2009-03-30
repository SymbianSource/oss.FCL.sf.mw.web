/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handle communications to Widget Registry.
*
*/


// INCLUDE FILES

#include "wrtharvesterregistryaccess.h"

_LIT( KSeparator, ":" );


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
WrtHarvesterRegistryAccess::WrtHarvesterRegistryAccess()
	{
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
WrtHarvesterRegistryAccess::~WrtHarvesterRegistryAccess()
    {
    iWidgetInfoArray.ResetAll();
    }

// ---------------------------------------------------------------------------
// Collect bundle names of widgets supporting miniviews.
// ---------------------------------------------------------------------------
//
void WrtHarvesterRegistryAccess::WidgetBundleNamesL( 
    RPointerArray< HBufC >& aArray )
    {
    RWrtArray< CWidgetInfo > widgetInfoArr;
    RWidgetRegistryClientSession session;
    
    CleanupClosePushL( session );
    widgetInfoArr.PushL();
    
    User::LeaveIfError( session.Connect() );
    
    aArray.Reset();
    // Reset previously appended widget infos
    iWidgetInfoArray.ResetAll();
    
    TInt err = session.InstalledWidgetsL( widgetInfoArr );
    
    for( TInt i( widgetInfoArr.Count() - 1 ); i >= 0; --i )
        {
        CWidgetInfo* widgetInfo( widgetInfoArr[i] );
        
        if ( SupportsMiniviewL( session, widgetInfo->iUid ) )
            {
            aArray.AppendL( ConstructWidgetNameL( session, *widgetInfo ) );
            iWidgetInfoArray.AppendL( widgetInfo );
            }
        else
            {
            delete widgetInfo;
            }
            
        widgetInfoArr.Remove( i );
        }
        
    CleanupStack::PopAndDestroy( 2, &session ); 
    }

// ---------------------------------------------------------------------------
// Find widget by UID.
// ---------------------------------------------------------------------------
//
TUid WrtHarvesterRegistryAccess::WidgetUid( TPtrC aBundleName )
    {
    TUid uid={0};
    TInt pos = aBundleName.Find( KSeparator );
    if( pos != KErrNotFound )
        {
        aBundleName.Set( aBundleName.Right( aBundleName.Length()-pos-1 ));
        }
                   
    for( TInt i = 0; i < iWidgetInfoArray.Count(); i++ )
        {
        if( aBundleName == *iWidgetInfoArray[i]->iBundleName )
            {
            uid = iWidgetInfoArray[i]->iUid;
            break;
            }
        }
    return uid;
    }

// ---------------------------------------------------------------------------
// Check if widget supports miniview.
// ---------------------------------------------------------------------------
//
TBool WrtHarvesterRegistryAccess::SupportsMiniviewL( 
    RWidgetRegistryClientSession& aSession,
    const TUid& aUid )
    {
    TBool res( EFalse );
    
    CWidgetPropertyValue* value( NULL );
    value = aSession.GetWidgetPropertyValueL( aUid, EMiniViewEnable );
    
    res = value && *value;
    
    delete value;
    return res;
    }
    
// ---------------------------------------------------------------------------
// Get the Bundle identifier.
// ---------------------------------------------------------------------------
//
HBufC* WrtHarvesterRegistryAccess::ConstructWidgetNameL( 
    RWidgetRegistryClientSession& aSession,
    CWidgetInfo& aInfo )
    {   
    CWidgetPropertyValue* value( NULL );
    value = aSession.GetWidgetPropertyValueL( aInfo.iUid, EBundleIdentifier );
    CleanupStack::PushL( value );
    
    const TDesC& identifier = *value;    
    HBufC* bundle = aInfo.iBundleName;

    HBufC* name = HBufC::NewL( identifier.Length() + KSeparator().Length() + bundle->Length());
    TPtr ptr = name->Des();
    ptr.Append( identifier );
    ptr.Append( KSeparator );
    ptr.Append( *bundle);
    CleanupStack::PopAndDestroy( value );
    return name;
    }
    
 //  End of File

