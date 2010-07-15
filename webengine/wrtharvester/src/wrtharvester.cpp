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
#include <wrtharvester.rsg>
#include <startupdomainpskeys.h>
#include <ecom/implementationproxy.h>

#include <LiwServiceHandler.h>
#include <LiwVariant.h>
#include <LiwGenericParam.h>
#include <e32property.h>    //Publish & Subscribe
#include <AknTaskList.h>
#include <apaid.h>
#include <apacmdln.h>
#include <s32mem.h>
#include <APGTASK.H>
#include <coemain.h>
#include <bautils.h>
#include <f32file.h>
#include <e32std.h>
#include <AknNotify.h>
#include <aknglobalconfirmationquery.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <oommonitorsession.h>

#ifndef BRDO_OOM_MONITOR2_COMPONENT_FF 
#include <systemwarninglevels.hrh>
#endif
#include "browser_platform_variant.hrh"

// CONSTANTS
_LIT( KResourceFileName, "\\resource\\wrtharvester.rsc" );
_LIT( KResourceDir, "Z:wrtharvester.rsc" );
_LIT( KUid, "uid");
_LIT( K0x, "0x");
_LIT( KOpenPar, "(");
_LIT( KClosePar, ")");
_LIT8( KWidgetIcon, "widget_icon");
#define KUidWidgetOOMPlugin 0x10282855
const TInt KMemoryToLaunchWidgetUi = 17*1024*1024;



/** 
* Launch or bring foreground the asked widget.
*
* Launch widget.
* @param aUid UID of the widget.
* @param aOperation Operation to perform.
*/
static void LaunchWidgetL( const TUid& aUid, TUint32 aOperation );

/** 
* In case the widget cannot be launched because of OOM
* Notify harvester and Clear event Queue
* @return void
*/
static void NotifyCommandAndCleanUp();

/** 
* Launch new widget.
*
* Launch new widget.
* @param aUid UID of the widget.
* @param aOperation Operation to perform.
*/
static void LaunchWidgetUIL( 
    const TUid& aUid, 
    const TDesC8& aMessage, 
    TUint32 aOperation );

/**
* Utility class to show the prompt for platform security access.
*
* The class exists only to provide platform security access prompt
* for the widgets which are launched in minview 
*/
class CGlobalQueryHandlerAO : public CActive
    {
public:
    /**
    * Startup.
    *
    * @param aManager Window Manager.
    * @param aWindow Window.
    * @param aMessage Message to be prompted.
    * @param aSoftkeys for prompt.
    * @param aWrtHarvester for callback
    */
    static CGlobalQueryHandlerAO* StartLD (
                            TUid& aUid,
                            const TDesC& aMessage, 
                            TInt aSoftkeys,
                            CWrtHarvester* aWrtHarvester);
    /**
    * ShowGlobalQueryDialogL.
    *
    * @param aMessage Message to be prompted.
    * @param aSoftkeys for prompt.
    */
    void ShowGlobalQueryDialogL ( 
                            const TDesC& aMessage, 
                            TInt aSoftkeys );
protected: // From CActive
    /**
    * Execute asynchronous operation.
    */
    void RunL();
    
    /**
    * Provide cancellation methods.
    */
    void DoCancel();
    
private:

    /**
    * Constructor.
    *
    * @param aManager Manager.
    * @param aWindow Window.
    * @param aMessage Message for prompt.
    * @param aSoftkeys for prompt.
    * @param aWrtHarvester for callback    
    */
    CGlobalQueryHandlerAO (
            TUid& aUid,
            const TDesC& aMessage, 
            TInt aSoftkeys,
            CWrtHarvester* aWrtHarvester);
        
    /**
    * Destructor. 
    *
    * Private on purpose.
    */
    ~CGlobalQueryHandlerAO();
    
private:

    TUid iWidgetUid;
    CAknGlobalConfirmationQuery* iGlobalConfirmationQuery ;
    HBufC* iConfirmationText;
    CWrtHarvester* iWrtHarvester;
    };

// ============================= LOCAL FUNCTIONS ===============================
    
// ----------------------------------------------------------------------------
// Returns the app full name
// ----------------------------------------------------------------------------
//
/*
static HBufC* GetAppNameLC( RApaLsSession& aSession, const TUid& aUid )
    {
    TApaAppInfo info;
    User::LeaveIfError( aSession.GetAppInfo( info, aUid ) );
    
    return info.iFullName.AllocLC();
    }
*/
    
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
    iCPSInterface( aCPSInterface ),
    iHSCount(0)
	{
	}

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CWrtHarvester::ConstructL()
    {
    User::LeaveIfError( iApaSession.Connect() );
    	
    iSystemShutdown = EFalse;
    iWidgetUIListener = CWrtHarvesterPSNotifier::NewL( this, EWidgetUIState );
    iWidgetRegListener = CWrtHarvesterPSNotifier::NewL( this, EWidgetRegAltered );
    iWidgetSystemShutdownListener = CWrtHarvesterPSNotifier::NewL( this, EWidgetSystemShutdown );
    iMsModeListener = CWrtHarvesterPSNotifier::NewL( this, EWidgetMassStorageMode );
    
    User::LeaveIfError( iFs.Connect() );
    
    SetMSMode(0);
    
    iCanAccessRegistry = ETrue;    
	iReinstallingWidget = EFalse;
	
	    
    TFileName resourceFileName;  
    TParse parse;    
    Dll::FileName (resourceFileName);           
    parse.Set(KResourceFileName, &resourceFileName, NULL); 
    resourceFileName = parse.FullName(); 
          
    CCoeEnv* coeEnv = CCoeEnv::Static(); 
    BaflUtils::NearestLanguageFile(coeEnv->FsSession(), resourceFileName); 
       
    if ( !BaflUtils::FileExists( coeEnv->FsSession(), resourceFileName ) )
        { 
        // Use resource file on the Z drive instead  
        parse.Set( KResourceDir, &KDC_RESOURCE_FILES_DIR, NULL ); 
        resourceFileName = parse.FullName();  
        BaflUtils::NearestLanguageFile( coeEnv->FsSession(),resourceFileName );            
        } 
    iResourceFileOffset = coeEnv->AddResourceFileL(resourceFileName);
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
    if ( iResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }
    iObservers.ResetAll();
    iWidgetInfo.ResetAll();
    iWidgetStateArray.ResetAll();
    
    delete iWidgetUIListener;
    delete iWidgetRegListener;    
    delete iMsModeListener;    
    delete iWidgetSystemShutdownListener;
    
    if(iAsyncCallBack)
        {
        iAsyncCallBack->Cancel();       
        }
    delete iAsyncCallBack;
    iAsyncCallBack = NULL;
    iUid.Close();
    iWidgetOperations.Close();
    iHSWidgets.ResetAndDestroy();
    iApaSession.Close();
    }
    
// ----------------------------------------------------------------------------
// Is called by Content Harvester server
// ----------------------------------------------------------------------------
//	
void CWrtHarvester::UpdateL() 
	{
	UpdatePublishersL();
	} 

// ----------------------------------------------------------------------------
// Is called when Homescreen sends events to publisher
// ----------------------------------------------------------------------------
//  
void CWrtHarvester::HandlePublisherNotificationL( const TDesC& aContentId, const TDesC8& aTrigger )
    {
    
    //Do not send the Operations to the Widgets when in Mass storage mode.. . .  
    TUid uid( WidgetUid( aContentId ) );
    
    
    if(iSystemShutdown && aTrigger == KDeActive )
    	{
    	return ;
    	}
    if( IsInMSMode() == 1 )
      {
      if( aTrigger == KDeActive && !iSystemShutdown )
          {
          RemovePublisherAndObserverL(aContentId);
          RWidgetRegistryClientSession session;
          CleanupClosePushL( session );
          User::LeaveIfError( session.Connect() );
          session.SetBlanketPermissionL( uid, EBlanketUnknown );
          CleanupStack::PopAndDestroy( &session );
          }
      return; 
      }
             
    TWidgetOperations operation( Uninitialized );
    if( aTrigger == KActive )
        {
        HBufC* temp = aContentId.Alloc();
        iHSWidgets.Append( temp );
        iHSCount++;	
        // queue the activated state event only for network accessing widgets
        if ( CheckNetworkAccessL( uid) )
            {
            TWrtState* widgetState = NULL;
            widgetState = new TWrtState( uid, EActivatedState );
            if ( widgetState )
                {
                iWidgetStateArray.AppendL( widgetState );
                }
            }
        
        operation = LaunchMiniview;
        }
    else if ( aTrigger == KDeActive )
        {
        iHSCount--;
        operation = Deactivate;
        HBufC *temp = aContentId.Alloc();
        TPtr ptr (temp->Des());
        for( TInt i=0; i<iHSWidgets.Count(); i++ )
          {
          if( ! ptr.Compare(iHSWidgets[i]->Des()))
            {
            iHSWidgets.Remove(i);
            break;
            }
          }
        delete temp;
        
        // Removing . Miniview, shall remove full view as well. For blanket permissions
        // will be revoked for miniview
         
      	if(!iSystemShutdown)
      	    {        
            RWidgetRegistryClientSession session;
            CleanupClosePushL( session );
            User::LeaveIfError( session.Connect() );
            session.SetBlanketPermissionL( uid, EBlanketUnknown );
            CleanupStack::PopAndDestroy( &session );
            }
      }
    else if( aTrigger == KSuspend )
        {
        operation = WidgetSuspend;
        }
    else if( aTrigger == KResume )
        {
        TInt idx = FindWidget( uid );
        
        // if unable to find the widget, queue the resume as usual
        if ( idx == -1 )
            {
            QueueResumeL( uid );
            }
        else
            {
            iWidgetStateArray[idx]->iState = EResumeState;
            }
        return;
        }
    else if( aTrigger == KSelected )
        {
        operation = WidgetSelect;
        }
    else if( aTrigger == KHSOnline )
        {
        ProcessNetworkModeL( uid, WidgetOnline );
        return;
        }
    else if( aTrigger == KHSOffline )
        {
        ProcessNetworkModeL( uid, WidgetOffline );
        return;
        }
    QueueOperationL( operation, uid );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::UpdatePublishersL() 
    {
    if(iReinstallingWidget)
        {        
        iReinstallingWidget = EFalse;
        return;
        }
    iRegistryAccess.WidgetInfosL( iWidgetInfo );
    RemoveObsoletePublishersL();
    
    for( TInt i = iWidgetInfo.Count() - 1; i >= 0; --i )
        {
        // Register the observer first as notification may be
        // received immediately after registration
        RequestForNotificationL( *iWidgetInfo[i] );
        TInt id = RegisterPublisherL( *iWidgetInfo[i] );
        if( id == KErrNotFound )
            {
            // remove the observer as the publisher registration failed
            RemoveObserver( *iWidgetInfo[i]->iBundleId);
            }
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RemoveObsoletePublishersL() 
    {
    RWrtArray<HBufC> publishers;
    publishers.PushL();
    RegisteredPublishersL( publishers );
    
    // Remove observer and CPS registry entry of uninstalled widget
    TInt count = publishers.Count();
    for( TInt i = count - 1; i >= 0; --i )
        {
        TBool found( EFalse );
        for( TInt j = 0; j < iWidgetInfo.Count(); j++ )
            {
            if( *publishers[i] == iWidgetInfo[j]->iBundleId )
                {
                found = ETrue;
                break;
                }
            }
        if( found == EFalse )
            {
            RemovePublisherAndObserverL( *publishers[i] );
            }
        }
    CleanupStack::PopAndDestroy( &publishers );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RegisteredPublishersL( RPointerArray<HBufC>& publishers )
    {
    publishers.ResetAndDestroy();

    CLiwGenericParamList* publisherList( CLiwGenericParamList::NewLC() );
    CLiwGenericParamList* inParamList( CLiwGenericParamList::NewLC() );
    
    TLiwGenericParam type( KType, TLiwVariant( KPubData ));
    inParamList->AppendL( type );
    
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    
    filter->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ) );
    filter->InsertL( KContentType, TLiwVariant( KTemplatedWidget ) );
     
    //append filter to input param
    TLiwGenericParam item( KFilter, TLiwVariant( filter ));
    inParamList->AppendL( item );
       
    // Get all publishers from CPS
    iCPSInterface->ExecuteCmdL( KGetList, *inParamList, *publisherList ); 
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( inParamList );
    
    TInt pos = 0;
    publisherList->FindFirst( pos, KResults );
    if( pos != KErrNotFound )
        {
        //extract iterator on results list
        TLiwVariant variant = (*publisherList)[pos].Value();
        CLiwIterable* iterable = variant.AsIterable();
        iterable->Reset();
        
        //get all the templated widget publishers
        while( iterable->NextL( variant ) )
            {
            //extract content map
            CLiwDefaultMap* publisherMap = CLiwDefaultMap::NewLC();         
            variant.Get( *publisherMap );
            variant.Reset();
             
            if( publisherMap && publisherMap->FindL( KContentId , variant ))
                {
                 HBufC* bundleId = variant.AsDes().AllocLC();
                 variant.Reset();
                 TBool isNokiaWidget = EFalse;
                 if ( publisherMap->FindL( KDataMap, variant) )
                     {
                     CLiwDefaultMap* dataMap = CLiwDefaultMap::NewLC();
                     variant.Get( *dataMap );
                     variant.Reset();
                     if ( dataMap->FindL( KWidgetInfo, variant ) )
                         {
                         CLiwDefaultMap* widgetInfoMap = CLiwDefaultMap::NewLC();
                         variant.Get( *widgetInfoMap );
                         if ( widgetInfoMap->FindL( KWidgetType, variant ) )
                             {
                             if ( KS60Widget == variant.AsTInt32())
                                 {
                                 isNokiaWidget = ETrue;
                                 }
                             }
                         CleanupStack::PopAndDestroy( widgetInfoMap );
                         }
                     CleanupStack::PopAndDestroy( dataMap );
                     }
                 
                 if (isNokiaWidget )
                     {
                     publishers.AppendL( bundleId );
                     CleanupStack::Pop( bundleId );
                     }
                 else
                     {
                     CleanupStack::PopAndDestroy( bundleId );
                     }
                 }
             CleanupStack::PopAndDestroy( publisherMap );     
             }
            
        variant.Reset();
        }
    CleanupStack::PopAndDestroy( publisherList );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::RegisterPublisherL( CWrtInfo& wrtInfo )
    {
    TBool networkAccess = CheckNetworkAccessL( wrtInfo.iUid );
    
    __UHEAP_MARK;
    TInt id( KErrNotFound );
    if( iCPSInterface )
        {
        TBuf<10> uid(wrtInfo.iUid.Name());  // [12345678]
        uid.Delete(0,1);                    // 12345678]
        uid.Delete( uid.Length()-1, 1);     // 12345678
        TBuf<50> wrtuid;
        wrtuid.Append(KUid);                // uid
        wrtuid.Append(KOpenPar);            // uid(
        wrtuid.Append(K0x);                 // uid(0x
        wrtuid.Append(uid );			  // uid(0x12345678
        wrtuid.Append(KClosePar);           // uid(0x12345678)
   
        CLiwGenericParamList* inparam( CLiwGenericParamList::NewLC() );
        CLiwGenericParamList* outparam( CLiwGenericParamList::NewLC() );

        TLiwGenericParam type( KType, TLiwVariant( KPubData ));
        inparam->AppendL( type );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* actionmap( NULL );

        // Create the data map for publisher registry
        cpdatamap->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
        cpdatamap->InsertL( KContentType, TLiwVariant( KTemplatedWidget ));
        cpdatamap->InsertL( KContentId, TLiwVariant( wrtInfo.iBundleId ));
        
        // Widget info map
    	  CLiwDefaultMap* widgetInfo = CLiwDefaultMap::NewLC();
		    widgetInfo->InsertL( KTemplateType, TLiwVariant( KTemplateName ));
		    widgetInfo->InsertL( KWidgetName, TLiwVariant( wrtInfo.iDisplayName ));
		    widgetInfo->InsertL( KWidgetIcon, TLiwVariant( wrtuid));  // uid(0x12345678) This is the expected format 
            widgetInfo->InsertL( KWidgetType, TLiwVariant( TInt32 (wrtInfo.iType ) ));

		    datamap->InsertL( KWidgetInfo , TLiwVariant( widgetInfo ));
		    CleanupStack::PopAndDestroy( widgetInfo );
        
		    // Take dynamic menu items into use
		    if (networkAccess)
		        {
		        CLiwDefaultMap* mapMenu = CLiwDefaultMap::NewLC();
		        mapMenu->InsertL( KItemOnlineOffline, TLiwVariant( KMyActionName ));
		        datamap->InsertL( KMenuItems, TLiwVariant( mapMenu ));
		        CleanupStack::PopAndDestroy(mapMenu);
		        }

        cpdatamap->InsertL( KDataMap, TLiwVariant(datamap) );
        
        // Create the action map for publisher registry
        actionmap = CLiwDefaultMap::NewLC();
        actionmap->InsertL( KActive, TLiwVariant( KMyActionName ));
        actionmap->InsertL( KDeActive, TLiwVariant( KMyActionName ));
        actionmap->InsertL( KSuspend, TLiwVariant( KMyActionName ));
        actionmap->InsertL( KResume, TLiwVariant( KMyActionName ));
        actionmap->InsertL( KSelected, TLiwVariant( KMyActionName ));
        if (networkAccess)
            {
            actionmap->InsertL( KHSOnline, TLiwVariant( KMyActionName ));
            actionmap->InsertL( KHSOffline, TLiwVariant( KMyActionName ));
            }
 
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
    __UHEAP_MARKEND;
    return id;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RemoveObserver(const TDesC& aBundleId)
	{
	TInt count = iObservers.Count();
	for( TInt i = 0; i < count; i++ )
		{
		if( iObservers[i]->BundleId() == aBundleId )
			{
			delete iObservers[i];
			iObservers.Remove( i );
			break;
			}
		}
	}
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RemovePublisherAndObserverL( const TDesC& aBundleId )
    {
    
    if( IsInMSMode() == 1 )
      {
      HBufC *temp = aBundleId.Alloc();
      TPtr ptr (temp->Des());
      for( TInt i=0; i<iHSWidgets.Count(); i++ )
        {

        if( ptr.Compare(iHSWidgets[i]->Des()) == 0)
          {
           return;
          }
        }

      }   
              
    RemoveObserver( aBundleId );
    
    __UHEAP_MARK;
    if( iCPSInterface )
        {
        CLiwGenericParamList* inparam = CLiwGenericParamList::NewLC();
        CLiwGenericParamList* outparam = CLiwGenericParamList::NewLC();
        TLiwGenericParam type( KType, TLiwVariant( KPubData ));
        inparam->AppendL( type );
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
        CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();

        // Create data map
        cpdatamap->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
        cpdatamap->InsertL( KContentType, TLiwVariant( KTemplatedWidget ));
        cpdatamap->InsertL( KContentId, TLiwVariant( aBundleId ));
        
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
    __UHEAP_MARKEND;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::RequestForNotificationL( CWrtInfo& wrtInfo )
    {
    // Preventing duplicate entries.
    for( TInt i = 0; i < iObservers.Count(); i++ )
        {
        if( iObservers[i]->BundleId() == wrtInfo.iBundleId )
            {
            return;
            }
        }
    
    CWrtHarvesterPublisherObserver* observer( NULL );
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    
    filter->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ) );
    filter->InsertL( KContentType, TLiwVariant( KTemplatedWidget ) );
    filter->InsertL( KContentId, TLiwVariant( wrtInfo.iBundleId ) );
    filter->InsertL( KOperation, TLiwVariant( KExecute ));
    
    observer = CWrtHarvesterPublisherObserver::NewLC( *wrtInfo.iBundleId, this );
    observer->RegisterL(filter);
    
    iObservers.AppendL( observer );
    
    CleanupStack::Pop( observer );
    
    CleanupStack::PopAndDestroy(filter);
    }
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::QueueOperationL( TWidgetOperations aOperation, TUid aUid )
    {
    //Hack to find out if WidgetUi exist as Queue keeps filling up
    SWidgetOperation op = { aOperation, aUid };
    
    if((iHSCount*3 <= iWidgetOperations.Count() && !CheckTaskExistsL() ) || (aOperation == WidgetSelect))
        {        
        ClearAllOperations();
        iWidgetUIListener->SetValue(1);
        } 
    
    TBool queued (EFalse);
    for(TInt i =0; i < iWidgetOperations.Count() && (aOperation == WidgetResume || aOperation == WidgetSuspend ) ; i++)
        {
        SWidgetOperation opInQueue = iWidgetOperations[i];
        //we get resume but we already have suspend in Queue
        //make Resume as Suspend    
        if(op.iOperation == WidgetResume && opInQueue.iUid == op.iUid && opInQueue.iOperation == WidgetSuspend )
            {            
            iWidgetOperations[i].iOperation = WidgetResume;
            queued = ETrue;
            break;
            }            
        //we get suspend but we already have resume in Queue
        //make Suspend as Resume    
        if(op.iOperation == WidgetSuspend  && opInQueue.iUid == op.iUid && opInQueue.iOperation == WidgetResume )
            {            
            iWidgetOperations[i].iOperation = WidgetSuspend;
            queued = ETrue;
            break;
            }    
        }
            
    if(!queued)
        iWidgetOperations.Append( op );
            
    TryLaunchNextOperationL();
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TBool CWrtHarvester::CheckTaskExistsL()
    {
    TUid widgetAppUid( TUid::Uid( KWidgetAppUid ) );
    RWsSession wsSession;    
        
    // Create Window server session
    User::LeaveIfError( wsSession.Connect() );
    
    TApaTaskList taskList( wsSession );
    TApaTask task = taskList.FindApp( widgetAppUid );
    return task.Exists()?ETrue:EFalse;
    }
    

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::TryLaunchNextOperationL()
    {
    TInt value = KErrNone;
    TInt error = iWidgetUIListener->GetValue(value);
    if( error == KErrNone && value == 1 && iWidgetOperations.Count() != 0 )
        {
        // Set value to 0 so that next widget is not launched before Widget App sets value to 1.
        iWidgetUIListener->SetValue( 0 );
        //Always launch the first in operation
        LaunchWidgetOperationL( iWidgetOperations[0] );
        iWidgetOperations.Remove( 0 );
        }
    }

void CWrtHarvester::ClearAllOperations()
    {
    TInt value = KErrNone;
    TInt error = iWidgetUIListener->GetValue(value);
    if( error == KErrNone )
        {
        iWidgetOperations.Reset();
        }
    }
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TUid CWrtHarvester::WidgetUid( TPtrC aBundleId )
    {
    TInt count( iWidgetInfo.Count());
    TUid uid = {0};
    for( TInt i = 0; i < count; i++ )
        {
        if( iWidgetInfo[i]->iBundleId == aBundleId )
            {
            uid = iWidgetInfo[i]->iUid;
            break;
            }
        }
    return uid;
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
void CWrtHarvester::LaunchWidgetOperationL( SWidgetOperation aOperation )
    {
    LaunchWidgetL (aOperation.iUid, aOperation.iOperation );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::QueueResumeL( TUid& aUid )
    {
    RWidgetRegistryClientSession session;
    CleanupClosePushL( session );
    User::LeaveIfError( session.Connect() );

    CWidgetPropertyValue* property = session.GetWidgetPropertyValueL( aUid, EPreInstalled );
    TBool preInstalled = property && *(property);
    delete property;

    // Set blanket permission to true for pre-installed widgets
    if ( preInstalled )
        {
        session.SetBlanketPermissionL( aUid, EBlanketTrue );
        }
    
    if ( session.IsBlanketPermGranted ( aUid ) == EBlanketUnknown && !iDialogShown 
         &&  iCanAccessRegistry  )
        {
        iDialogShown = ETrue;            
        AllowPlatformAccessL( aUid );
        }
    else if(session.IsBlanketPermGranted ( aUid ) == EBlanketUnknown)
        {
        iUid.Append(aUid);
        }
    else
        {
        QueueOperationL( WidgetResume, aUid );
        }        
    CleanupStack::PopAndDestroy( &session );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::ProcessNetworkModeL( TUid& aUid, TWidgetOperations aOperation )
    {
    // first queue the online/offline event and then the resume event
    QueueOperationL( aOperation, aUid );
    
    // check if there is a resume event to queue
    TInt idx = FindWidget(aUid, EResumeState);
    if ( idx != -1 )
        {
        QueueResumeL( aUid );          
        }
    idx = (idx == -1) ? FindWidget(aUid): idx;
    if(idx != -1 )
        {
        // remove it from the array, no longer needed
        delete iWidgetStateArray[idx];
        iWidgetStateArray.Remove(idx);
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::FindWidget( TUid& aUid, TWidgetState aState )
    {
    TInt idx = -1;
    for( TInt i( iWidgetStateArray.Count() - 1 ); i >= 0; --i )
        {
        if ( ( iWidgetStateArray[i]->iUid == aUid ) && ( iWidgetStateArray[i]->iState == aState ) )
            {
            idx = i;
            }
        }
    return idx;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::FindWidget( TUid& aUid )
    {
    TInt idx = -1;
    for( TInt i( iWidgetStateArray.Count() - 1 ); i >= 0; --i )
        {
        if ( iWidgetStateArray[i]->iUid == aUid )
            {
            idx = i;
            }
        }
    return idx;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TBool CWrtHarvester::CheckNetworkAccessL( TUid& aUid )
    {
    RWidgetRegistryClientSession session;
    CleanupClosePushL( session );
    User::LeaveIfError( session.Connect() );

    CWidgetPropertyValue* value = session.GetWidgetPropertyValueL( aUid, EAllowNetworkAccess );
    TBool networkAccess = value && *(value);
    delete value;

    CleanupStack::PopAndDestroy( &session );
    
    return networkAccess;
    }
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
TInt CWrtHarvester::DeleteCallback(TAny* aPtr)
    {
    CWrtHarvester* self = (CWrtHarvester*)aPtr;    
    self->QueueResumeL(self->iUid[0]);
    self->iUid.Remove(0);    
    delete self->iAsyncCallBack;
    self->iAsyncCallBack = NULL;
    return 0;
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::DialogShown()
    {
    iDialogShown = EFalse;
    if(iUid.Count())
        {
        iAsyncCallBack = new (ELeave) CAsyncCallBack(TCallBack(DeleteCallback,this),CActive::EPriorityUserInput);
        iAsyncCallBack->CallBack(); 
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
void CWrtHarvester::AllowPlatformAccessL( TUid& aUid )
    {
    HBufC* confirmationText = StringLoader::LoadLC(R_WRTHRV_PLATFORM_ACCESS);
    TInt softKey = R_AVKON_SOFTKEYS_OK_CANCEL;
    CGlobalQueryHandlerAO* tmp = CGlobalQueryHandlerAO::StartLD( aUid, confirmationText->Des(), softKey, this );
    CleanupStack::PopAndDestroy(confirmationText);
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::StartLD
//
// Initialize AO.
// ------------------------------------------------------------------------
CGlobalQueryHandlerAO* CGlobalQueryHandlerAO::StartLD(
    TUid& aUid,
    const TDesC& aMessage, 
    TInt aSoftkeys,
    CWrtHarvester* aWrtHarvester)
    {
    CGlobalQueryHandlerAO* self( new( ELeave ) CGlobalQueryHandlerAO( aUid, aMessage, aSoftkeys, aWrtHarvester) );
    TRAPD(error, self->ShowGlobalQueryDialogL ( aMessage, aSoftkeys ));
    if ( error )
        {
        delete self;
        User::Leave(error);
        }
    self->SetActive();
    return self;
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::CGlobalQueryHandlerAO
//
// Constructor.
// ------------------------------------------------------------------------
CGlobalQueryHandlerAO::CGlobalQueryHandlerAO(
    TUid& aUid,
    const TDesC& aMessage, 
    TInt /*aSoftkeys*/,
    CWrtHarvester* aWrtHarvester):CActive( EPriorityHigh ),
    iWidgetUid(aUid),
    iConfirmationText(aMessage.AllocL()),
    iWrtHarvester(aWrtHarvester)
    {
    CActiveScheduler::Add( this );
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::CGlobalQueryHandlerAO
//
// ISet network and platform access permission based on user response.
// ------------------------------------------------------------------------
void CGlobalQueryHandlerAO::RunL()
    {    
    RWidgetRegistryClientSession session;
    CleanupClosePushL( session );
    User::LeaveIfError( session.Connect() );  
    if (iStatus == EAknSoftkeyOk)
        {
        session.SetBlanketPermissionL( iWidgetUid, EBlanketTrue );
        }
    else if ( iStatus == EAknSoftkeyCancel)
        {
        session.SetBlanketPermissionL( iWidgetUid, EBlanketFalse );
        }
    iWrtHarvester->QueueOperationL( WidgetResume, iWidgetUid );
    iWrtHarvester->DialogShown();
    CleanupStack::PopAndDestroy( &session );
    
    delete this;
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::DoCancel
//
// Do nothing.
// ------------------------------------------------------------------------
void CGlobalQueryHandlerAO::DoCancel()
    {
    if ( iGlobalConfirmationQuery )
        {
        iGlobalConfirmationQuery->CancelConfirmationQuery();
        }
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::~CGlobalQueryHandlerAO
//
// Destructor.
// ------------------------------------------------------------------------
CGlobalQueryHandlerAO::~CGlobalQueryHandlerAO()
    {
    Cancel();
    delete iGlobalConfirmationQuery;
    delete iConfirmationText;
    }

// ---------------------------------------------------------
// CGlobalQueryHandlerAO::ShowGlobalQueryDialogL()
// ---------------------------------------------------------
// 
void CGlobalQueryHandlerAO::ShowGlobalQueryDialogL(const TDesC& aMessage, TInt aSoftkeys)
    {
    iGlobalConfirmationQuery = CAknGlobalConfirmationQuery::NewL();
    iGlobalConfirmationQuery->ShowConfirmationQueryL
                                (iStatus,
                                aMessage,
                                aSoftkeys);
    }
//======================================================================
// Launch widget.
//===========================================================================
//
void LaunchWidgetL( const TUid& aUid, TUint32 aOperation )
    {
    __UHEAP_MARK;
    
    TUid widgetAppUid( TUid::Uid( KWidgetAppUid ) );
    
    RWsSession wsSession;
    ROomMonitorSession monitorSession;
    TApaTaskList taskList( wsSession );
    HBufC8* message( HBufC8::NewLC( KWidgetUiMaxMessageLength ) );
    TPtr8 des( message->Des() );
    TInt err(KErrNone);
    RDesWriteStream stream( des );
    
    CleanupClosePushL( stream );
    
    // Make the message to be sent.
    stream.WriteUint32L( 1 );
    stream.WriteUint32L( aUid.iUid );
    stream.WriteInt32L( aOperation );
        
    CleanupStack::PopAndDestroy( &stream );
    
    // Create Window server session
    User::LeaveIfError( wsSession.Connect() );
    User::LeaveIfError( monitorSession.Connect() );
    CleanupClosePushL( wsSession );

    // Get the task list
    // Try to find out if stub ui is already running
    TApaTask task = taskList.FindApp( widgetAppUid );

    if ( task.Exists() )
        {
        // TODO make something here, or not...
        widgetAppUid = TUid::Uid( 1 );
        if ( aOperation == WidgetSelect )
            {
            task.BringToForeground();
            }
        task.SendMessage( widgetAppUid, des );
        }
    else
        {
        // TODO CONST
        if ( aOperation == LaunchMiniview ||
             aOperation == WidgetSelect ||
             aOperation == WidgetResume ||
             aOperation == WidgetRestart ) //WidgetUI has died -> re-launch
            {
            TInt bytesAvailaible(0);
            if (aOperation != WidgetSelect )
                {
#ifdef BRDO_OOM_MONITOR2_COMPONENT_FF
                err = monitorSession.RequestOptionalRam(KMemoryToLaunchWidgetUi, KMemoryToLaunchWidgetUi,KUidWidgetOOMPlugin, bytesAvailaible);
#else
                   TMemoryInfoV1Buf info;
                   UserHal::MemoryInfo(info);
                   err = info().iFreeRamInBytes > KMemoryToLaunchWidgetUi +  KRAMGOODTHRESHOLD ? KErrNone : KErrNoMemory;
#endif
                if( err == KErrNone)
                    {
                    LaunchWidgetUIL( widgetAppUid, *message, aOperation );
                    }
                }
            else
                {
                //The modification is related to the manual starting of WRT widgets from HS. After noticing an issue when
                //the user taps manually a WRT widget from the HS. 
                //If RAM is not available with RequestOptionalRam() API, the manual tapping does nothing
                //and that is incorrect behaviour. Therefore if widgetSelect -event is sent to the launcher we are using RequestFreeMemory() instead of using RequestOptionalRam() API. 
                //This means that we apply mandatory RAM allocation when a widget is started manually from HS in order to make sure that RAM is released properly
                err = monitorSession.RequestFreeMemory( KMemoryToLaunchWidgetUi );
                if( err == KErrNone)
                    {
                    LaunchWidgetUIL( widgetAppUid, *message, aOperation );
                    }
                }
            if(err != KErrNone)
                NotifyCommandAndCleanUp();
            }
        else
            {
            NotifyCommandAndCleanUp();
            }
            
        }
        
    CleanupStack::PopAndDestroy( 2, message );
    monitorSession.Close();
    __UHEAP_MARKEND;
    }

//===========================================================================
// Launch Widget UI.
//===========================================================================
void LaunchWidgetUIL( 
    const TUid& aUid, 
    const TDesC8& aMessage, 
    TUint32 aOperation )
    {
    HBufC* document( NULL );
    CApaCommandLine* line( CApaCommandLine::NewLC() );
    TApaAppInfo info;
    RApaLsSession session;
    
    User::LeaveIfError( session.Connect() );
    CleanupClosePushL( session );
    
    User::LeaveIfError( session.GetAppInfo( info, aUid ) );
        
    document = HBufC::NewMaxLC( TReal( TReal( aMessage.Length() )  / 2.0 ) + 0.5 );

    Mem::Copy( 
        reinterpret_cast< TUint8* >( const_cast< TUint16* >( document->Ptr() ) ),
        aMessage.Ptr(),
        KWidgetUiMaxMessageLength );
        
    line->SetDocumentNameL( *document );
    line->SetExecutableNameL( info.iFullName );
        
    // TODO make const definitions.
    if ( aOperation == 1 || aOperation == 3 )
        {
        line->SetCommandL( EApaCommandBackground );
        }
        
    session.StartApp( *line );

    CleanupStack::PopAndDestroy( 3, line );
    }

void NotifyCommandAndCleanUp()
    {
    const TUid KMyPropertyCat = { 0x10282E5A };
    enum TMyPropertyKeys { EWidgetUIState = 109 };
    TInt state( 2 );
    RProperty::Set( KMyPropertyCat, EWidgetUIState , state );    
    }
 //  End of File
