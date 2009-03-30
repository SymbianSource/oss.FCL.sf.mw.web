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
* Description:  
*
*/


// INCLUDE FILES
#include "wrtharvester.h"
#include "wrtharvesterregistryaccess.h"
#include "wrtharvesterpsnotifier.h"
#include "wrtharvesterpublisherobserver.h"

#include "wrtharvesterconst.h"
#include <widgetregistryconstants.h>

#include <implementationproxy.h>

#include <LiwServiceHandler.h>
#include <LiwVariant.h>
#include <LiwGenericParam.h>
#include <e32property.h>    //Publish & Subscribe
#include <AknTaskList.h>
#include <apaid.h>
#include <apacmdln.h>
#include <s32mem.h>


// TODO Check these types, should they be definedin sapidatapluginconst.h?
_LIT( KTemplatedWidget, "ai3templatedwidget");
_LIT8( KMySelected, "selected");
_LIT8( KMyActionMap, "action_map" );
_LIT8( KMyItem, "item" );
_LIT8( KMyAdd, "Add" );
_LIT8( KMyDelete, "Delete" );
_LIT8( KMyItemId, "item_id" );
_LIT8( KMyImage, "image" ); 
_LIT( KMyContType, "wideimage" );
_LIT8( KMyResultName,"mydata" );
_LIT( KMyActionName, "data" );
// ============================= LOCAL FUNCTIONS ===============================
    
// ----------------------------------------------------------------------------
// ta
// ----------------------------------------------------------------------------
//
static HBufC* GetAppNameLC( RApaLsSession& aSession, const TUid& aUid )
    {
    TApaAppInfo info;
    User::LeaveIfError( aSession.GetAppInfo( info, aUid ) );
    
    return info.iFullName.AllocLC();
    }
    
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
static void HandleWidgetCommandL( 
    RApaLsSession& aSession, 
    const TDesC& aWidget,
    const TUid& aUid,
    TUint32 aOperation )
    {
    const TInt size( 2* aWidget.Length() + 3*sizeof( TUint32 ) );
    
    // Message format is <filenameLength><unicode_filename><someintegervalue>
    CApaCommandLine* cmd( CApaCommandLine::NewLC() );
    HBufC8* opaque( HBufC8::NewLC( size ) );
    
    RDesWriteStream stream;
    TPtr8 des( opaque->Des() );
    
    stream.Open( des );
    CleanupClosePushL( stream );
    
    // Generate the command.
    stream.WriteUint32L( aUid.iUid );
    stream.WriteUint32L( aWidget.Length() );
    stream.WriteL( reinterpret_cast< const TUint8* >( aWidget.Ptr() ),
                   aWidget.Size() );
    
    stream.WriteInt32L( aOperation );
    
    CleanupStack::PopAndDestroy( &stream );
    
    // Generate command.
    cmd->SetCommandL( EApaCommandBackgroundAndWithoutViews );
    cmd->SetOpaqueDataL( *opaque );    

    CleanupStack::PopAndDestroy( opaque );
    
    cmd->SetExecutableNameL( KLauncherApp );
    
    User::LeaveIfError( aSession.StartApp( *cmd ) );
    CleanupStack::PopAndDestroy( cmd );
    }

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0xA12345FE, CWrtHarvester::NewL )
    };

// ----------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                                TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CWrtHarvester::CWrtHarvester( MLiwInterface* aCPSInterface ):
    iCPSInterface( aCPSInterface )
	{
	}

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CWrtHarvester::ConstructL()
    {
    User::LeaveIfError( iApaSession.Connect() );
    iWidgetUIListener = CWrtHarvesterPSNotifier::NewL( this, EWidgetUIState );
    iWidgetRegListener = CWrtHarvesterPSNotifier::NewL( this, EWidgetRegAltered );
    iWidgetMMCListener =  CWrtHarvesterPSNotifier::NewL( this, EWidgetMMCAltered );
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CWrtHarvester* CWrtHarvester::NewL(  MLiwInterface* aCPSInterface )
    {
    CWrtHarvester* self = new ( ELeave ) CWrtHarvester( aCPSInterface );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWrtHarvester::~CWrtHarvester()
    {
    iObservers.ResetAll();
    delete iWidgetUIListener;
    delete iWidgetRegListener;
    delete iWidgetMMCListener;
    iWidgetUids.Close();
    iApaSession.Close();
    }
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//	
void CWrtHarvester::UpdateL() 
	{
	UpdatePublishersL();
	} 

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//  
void CWrtHarvester::HandlePublisherNotificationL( const TDesC& aContentId, const TDesC8& aTrigger )
    {
    TUid uid( iRegistryAccess.WidgetUid( aContentId ) );
    if( aTrigger == KActive() )
        {
        StartWidgetL( uid );
        }
    else
        {
        TWidgetOperations operation( Uninitialized );
        if ( aTrigger == KDeActive )
            {
            operation = Deactivate;
            }
        else if( aTrigger == KSuspend )
            {
            operation = WidgetSuspend;
            }
        else if( aTrigger == KResume )
            {
            operation = WidgetResume;
            }
        else if( aTrigger == KMySelected )
            {
            operation = WidgetSelect;
            }
        LaunchWidgetL( operation, uid );
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::UpdatePublishersL() 
    {
    RPointerArray< HBufC > bundleNames;
    
    // The array contain references.
    CleanupClosePushL( bundleNames );
    
    iRegistryAccess.WidgetBundleNamesL( bundleNames );
    
    // Remove observer and CPS registry entry of uninstalled widget
    for( TInt i = 0; i < iObservers.Count(); i++ )
        {
        TBool found( EFalse );
        for( TInt j = 0; j < bundleNames.Count(); j++ )
            {
            if( iObservers[i]->Name() == bundleNames[j] )
                {
                found = ETrue;
                break;
                }
            }
        if( found == EFalse )
            {
            RemovePublisherL( KTemplatedWidget, iObservers[i]->Name(), KMyContType );
            delete iObservers[i];
            iObservers.Remove( i );
            }
        }
    
    for( TInt i = bundleNames.Count() - 1; i >= 0; --i )
        {
        TInt id = RegisterPublisherL( 
            KTemplatedWidget,
            *( bundleNames[ i ] ), 
            KMyContType, 
            KMyImage );
            
        if( id != KErrNotFound )
            {
            RequestForNotificationL( KTemplatedWidget, *( bundleNames[ i ] ), KMyContType );
            }
        }
        
    CleanupStack::PopAndDestroy( &bundleNames ); 
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::RegisterPublisherL( 
    const TDesC& aPublisherId, 
    const TDesC& aContentId,
    const TDesC& aContentType, 
    const TDesC8& aResultType )
    {
    TInt id( KErrNotFound );
    if( iCPSInterface )
        {   
        CLiwGenericParamList* inparam( CLiwGenericParamList::NewLC() );
        CLiwGenericParamList* outparam( CLiwGenericParamList::NewLC() );

        TLiwGenericParam type( KType, TLiwVariant( KPubData ));
        inparam->AppendL( type );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* actionmap( NULL );

        // Create the data map for publisher registry
        cpdatamap->InsertL( KContentType, TLiwVariant( aContentType ));
        cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
        cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
        datamap->InsertL( KMyResultName, TLiwVariant( aResultType ));

        cpdatamap->InsertL( KDataMap, TLiwVariant(datamap) );
        
        // Create the action map for publisher registry
        actionmap = CLiwDefaultMap::NewLC();
        actionmap->InsertL(KActive, TLiwVariant( KMyActionName ));
        actionmap->InsertL(KDeActive, TLiwVariant( KMyActionName ));
        actionmap->InsertL(KSuspend, TLiwVariant( KMyActionName ));
        actionmap->InsertL(KResume, TLiwVariant( KMyActionName ));
        actionmap->InsertL(KMySelected, TLiwVariant( KMyActionName ));
 
        cpdatamap->InsertL( KMyActionMap, TLiwVariant(actionmap));
        CleanupStack::PopAndDestroy( actionmap );
        
        TLiwGenericParam item( KMyItem, TLiwVariant( cpdatamap ));        
        inparam->AppendL( item );
        
        iCPSInterface->ExecuteCmdL( KMyAdd , *inparam, *outparam);
        id = ExtractItemId(*outparam);
        
        CleanupStack::PopAndDestroy( datamap );
        CleanupStack::PopAndDestroy( cpdatamap );
        item.Reset();
        type.Reset();   
        CleanupStack::PopAndDestroy(outparam);
        CleanupStack::PopAndDestroy(inparam);
        }   
    return id;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RemovePublisherL( const TDesC& aPublisherId, const TDesC& aContentId,
        const TDesC& aContentType )
    {
    if( iCPSInterface )
        {
        CLiwGenericParamList* inparam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList* outparam = CLiwGenericParamList::NewLC();
        TLiwGenericParam type( KType, TLiwVariant( KPubData ));
        inparam->AppendL( type );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();

        // Create data map
        cpdatamap->InsertL( KContentType, TLiwVariant( aContentType ));
        cpdatamap->InsertL( KContentId, TLiwVariant( aContentId ));
        cpdatamap->InsertL( KPublisherId, TLiwVariant( aPublisherId ));
        
        cpdatamap->InsertL( KDataMap, TLiwVariant(datamap) );
        
        TLiwGenericParam item( KMyItem, TLiwVariant( cpdatamap ));        
        inparam->AppendL( item );
        iCPSInterface->ExecuteCmdL( KMyDelete , *inparam, *outparam);
        
        CleanupStack::PopAndDestroy( datamap );
        CleanupStack::PopAndDestroy( cpdatamap );
        item.Reset();
        type.Reset();
        CleanupStack::PopAndDestroy(outparam);
        CleanupStack::PopAndDestroy(inparam);
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RequestForNotificationL( const TDesC& aPublisherId, const TDesC& aContentId,
        const TDesC& aContentType )
    {
    // Preventing duplicate entries.
    for( TInt i = 0; i < iObservers.Count(); i++ )
        {
        if( iObservers[i]->Name() == aContentId )
            {
            return;
            }
        }
    
    CWrtHarvesterPublisherObserver* observer( NULL );
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    
    filter->InsertL( KPublisherId, TLiwVariant( aPublisherId ) );
    filter->InsertL( KContentId, TLiwVariant( aContentId ) );
    filter->InsertL( KContentType, TLiwVariant( aContentType ) );
    filter->InsertL( KOperation, TLiwVariant( KExecute ));
    
    observer = CWrtHarvesterPublisherObserver::NewLC( aContentId, this );
    observer->RegisterL(filter);
    
    iObservers.AppendL( observer );
    
    CleanupStack::Pop( observer );
    
    CleanupStack::PopAndDestroy(filter);
    }
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::StartWidgetL( TUid aUid )
    {
    iWidgetUids.Append( aUid );
    TryLaunchNextWidgetL();
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::TryLaunchNextWidgetL()
    {
    TInt value = KErrNone;
    TInt error = iWidgetUIListener->GetValue(value);
    if( error == KErrNone && value == 1 && iWidgetUids.Count() != 0 )
        {
        // Set value to 0 so that next widget is not launched before Widget App sets value to 1.
        iWidgetUIListener->SetValue( 0 );
        //Always launch the first widget
        LaunchWidgetL( LaunchMiniview, iWidgetUids[0] );
        iWidgetUids.Remove( 0 );
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::ExtractItemId( const CLiwGenericParamList& aInParamList )
    {
    TInt result ( KErrNotFound );
    TInt pos( 0 );
    aInParamList.FindFirst( pos, KMyItemId );
    if( pos != KErrNotFound )
        {
        // item id present - extract and return
        TUint uResult = 0;
        if ( aInParamList[pos].Value().Get( uResult ) )
            {
            result = uResult;
            }
        }
    return result;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::LaunchWidgetL( TWidgetOperations aOperation, TUid aUid )
    {
    HBufC* widgetName( GetAppNameLC( iApaSession, aUid) );
    HandleWidgetCommandL( iApaSession, *widgetName, aUid, aOperation );
    CleanupStack::PopAndDestroy( widgetName );
    }

 //  End of File

