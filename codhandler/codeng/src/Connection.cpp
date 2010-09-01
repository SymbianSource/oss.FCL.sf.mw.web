/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Implementation of class CConnection.   
*      
*
*/


// INCLUDE FILES

#include "Connection.h"
#include "CodPanic.h"
#include "CodUtil.h"
#include "CodError.h"
#include "CodLogger.h"
#include <CommDbConnPref.h>
#include <cdblen.h>
#include <es_enum.h>
#include <platform/mw/browser_platform_variant.hrh>
#ifdef BRDO_OCC_ENABLED_FF
#include <extendedconnpref.h>
#include <FeatMgr.h>
#include <CentralRepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <cmgenconnsettings.h>
#include <cmmanagerkeys.h>
#include <etelmm.h>
#include <rconnmon.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CConnection::NewL()
// ---------------------------------------------------------
//
CConnection* CConnection::NewL()
    {
    CConnection* conn = new (ELeave) CConnection();
    CleanupStack::PushL( conn );
    conn->ConstructL();
    CleanupStack::Pop( conn );
    return conn;
    }

// ---------------------------------------------------------
// CConnection::~CConnection()
// ---------------------------------------------------------
//
CConnection::~CConnection()
    {
    Cancel();
    DoClose();
    }

// ---------------------------------------------------------
// CConnection::ConnectL()
// ---------------------------------------------------------
//
void CConnection::ConnectL( TUint32 aIap, TRequestStatus* aStatus )
    {
    CLOG(( EConn, 2, _L("-> CConnection::ConnectL iap(%d)"), aIap ));
    // Misuse asserts.
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInvalidArguments ) );
    // Internal asserts.
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodInternal ) );

    iStatus = KErrCodCannotConnect;
    if ( iState == EConnected )
        {
        // Already connected; do nothing.
        CLOG(( EConn, 4, _L("  already connected") ));
        iStatus = KErrNone;
        }
    else if ( iState == EInit )
        {
        // Not connected. Create connection.
        CLOG(( EConn, 4, _L("  starting connection") ));
        // Make this part atomic by pushing closes on the stack.
        User::LeaveIfError( iSockServ.Connect() );
        CleanupClosePushL<RSocketServ>( iSockServ );
        User::LeaveIfError( iConn.Open( iSockServ ) );
        CleanupClosePushL<RConnection>( iConn );
        iConnPref.SetDirection( ECommDbConnectionDirectionOutgoing );
        if ( aIap )
            {
            // Use specified IAP.
            iConnPref.SetIapId( aIap );
            iConnPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
            }
        else
            {
            // aIap == 0 -> user select.
            iConnPref.SetDialogPreference( ECommDbDialogPrefPrompt );
            }
        #ifdef BRDO_OCC_ENABLED_FF
           TExtendedConnPref extPref;
           CLOG(( EConn, 4, _L("CodHalder Setting OCC parameters") ));
           
           CLOG(( EConn, 4, _L("Iap: %d"), aIap ));
           if (aIap)
           {
              CLOG(( EConn, 4, _L("Iap is found") ));
              extPref.SetSnapPurpose(CMManager::ESnapPurposeUnknown);
              extPref.SetIapId(aIap);
           }
           else
           {
              CLOG(( EConn, 4, _L("Using Internet Snap") ));
              extPref.SetSnapPurpose(CMManager::ESnapPurposeInternet);
		   }

           //Default dialog behaviour
           extPref.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourConnSilent);
                  
           if ( !IsPhoneOfflineL() )
           {
              TInt currentmode = KErrNone;
              CRepository* rep = CRepository::NewLC( KCRUidCmManager );
              rep->Get(KCurrentCellularDataUsage, currentmode );
              CleanupStack::PopAndDestroy(); //rep
              if(ECmCellularDataUsageConfirm == currentmode)
              {
                 if ( IsRoamingL() || (aIap == 0) )
                 {
                    CLOG(( EConn, 4, _L("Setting note behaviour as Default") ));
                    extPref.SetNoteBehaviour(TExtendedConnPref::ENoteBehaviourDefault);
                 }
              }
           }
           
           TConnPrefList prefList;
           prefList.AppendL(&extPref);
           iConn.Start( prefList, iStatus );
        #else
            iConn.Start( iConnPref, iStatus );
        #endif //BRDO_OCC_ENABLED_FF
        
        iState = EConnecting;
        SetActive();
        CleanupStack::Pop( 2 ); // closing iConn and iSockServ
        // End of atomic part.
        }
    else
        {
        // Not expecting this to be called in other states.
        CLOG(( EConn, 0, _L("CConnection::ConnectL: unexpected state (%d)"), \
                                                                    iState ));
        __ASSERT_ALWAYS( EFalse, CodPanic( ECodInternal ) );
        }

    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;

    if ( !IsActive() )
        {
        // Unless we have an outstanding connect request (iConn.Start),
        // we are done.
        Done();
        }

    CLOG(( EConn, 2, _L("<- CConnection::ConnectL") ));
    }

// ---------------------------------------------------------
// CConnection::AttachL()
// ---------------------------------------------------------
//
void CConnection::AttachL( TUint32 aIap )
    {
    CLOG(( EConn, 2, _L("-> CConnection::AttachL iap(%d)"), aIap ));

    iStatus = KErrCodCannotConnect;
    if ( iState == EConnected )
        {
        // Already connected; do nothing.
        CLOG(( EConn, 4, _L("  already connected") ));
        iStatus = KErrNone;
        }
    else if ( iState == EInit )
        {
        // Not connected. Create connection, or attach to existing.
        CLOG(( EConn, 4, _L("  not connected") ));
        if ( aIap )
            {
            // Make this part atomic by pushing closes on the stack.
            User::LeaveIfError( iSockServ.Connect() );
            CleanupClosePushL<RSocketServ>( iSockServ );
            User::LeaveIfError( iConn.Open( iSockServ ) );
            CleanupClosePushL<RConnection>( iConn );
            TConnectionInfoBuf connInfo;
            TUint count;
            User::LeaveIfError( iConn.EnumerateConnections( count ) );
            CLOG(( EConn, 4, _L("  %d existing connections"), count ));
            TUint i;
            for ( i = count; i; i-- )
                {
                // Note: GetConnectionInfo expects 1-based index.
                User::LeaveIfError( iConn.GetConnectionInfo( i, connInfo ) );
                CLOG(( EConn, 4, _L("  conn(%d) iap(%d)"), \
                                                i, connInfo().iIapId ));
                if ( connInfo().iIapId == aIap )
                    {
                    CLOG(( EConn, 4, _L("  attach"), i, count ));
                    User::LeaveIfError( iConn.Attach
                        ( connInfo, RConnection::EAttachTypeNormal ) );
                    iState = EConnected;
                    iStatus = KErrNone;
                    break;
                    }
                }
            CleanupStack::Pop( 2 ); // closing iConn and iSockServ
            // End of atomic part.
            }
        if ( iState == EInit )
            {
            CLOG(( EConn, 4, _L("  ended unconnected") ));
            iConn.Close();
            iSockServ.Close();
            }
        }
    else
        {
        // Not expecting this to be called in other states.
        CLOG(( EConn, 0, _L("CConnection::AttachL: unexpected state (%d)"), \
                                                                    iState ));
        __ASSERT_ALWAYS( EFalse, CodPanic( ECodInternal ) );
        }
    User::LeaveIfError( iStatus.Int() );

    CLOG(( EConn, 2, _L("<- CConnection::AttachL") ));
    }

// ---------------------------------------------------------
// CConnection::Close()
// ---------------------------------------------------------
//
void CConnection::Close()
    {
    CLOG(( EConn, 2, _L("-> CConnection::Close") ));
    Cancel();
    DoClose();
    CLOG(( EConn, 2, _L("<- CConnection::Close") ));
    }

// ---------------------------------------------------------
// CConnection::IsConnected()
// ---------------------------------------------------------
//
TBool CConnection::IsConnected( TUint32& aIap )
    {
    TBool connected( EFalse );
    if( iState == EConnected )
        {
        TBuf<KCommsDbSvrMaxColumnNameLength * 2 + 1> iapId;
        _LIT( KFormatIapId, "%S\\%S" );
        TPtrC iap( IAP );
        TPtrC id( COMMDB_ID );
        iapId.Format( KFormatIapId, &iap, &id );
        TInt err = iConn.GetIntSetting( iapId, aIap );
        connected = err ? EFalse : ETrue;
        }
    return connected;
    }

// ---------------------------------------------------------
// CConnection::CConnection()
// ---------------------------------------------------------
//
CConnection::CConnection()
: CActive( CActive::EPriorityStandard ),
  iState( EInit )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CConnection::ConstructL()
// ---------------------------------------------------------
//
void CConnection::ConstructL()
    {
    if ( !CodUtil::ApCountL() )
        {
        // It's futile to try making a connection without an AP, so leave if
        // we don't have any.
        User::Leave( KErrCodNoAccessPoint );
        }
    }

// ---------------------------------------------------------
// CConnection::DoCancel()
// ---------------------------------------------------------
//
void CConnection::DoCancel()
    {
    CLOG(( EConn, 2, _L("-> CConnection::DoCancel") ));
    __ASSERT_DEBUG( iState == EConnecting, CodPanic( ECodInternal ) );
    // RConnection has no cancel method matching the async RConnection::Start.
    // The only way to cancel is to close the RConnection.
    // (Our request will complete with KErrCancel, as expected.)
    DoClose();
    Done();
    CLOG(( EConn, 2, _L("<- CConnection::DoCancel") ));
    }

// ---------------------------------------------------------
// CConnection::RunL()
// ---------------------------------------------------------
//
void CConnection::RunL()
    {
    CLOG(( EConn, 2, _L("-> CConnection::RunL iStatus(%d)"), \
        iStatus.Int() ));
    __ASSERT_DEBUG( iState == EConnecting, CodPanic( ECodInternal ) );
    User::LeaveIfError( iStatus.Int() );    // Handle errors in RunError().
    iState = EConnected;
    Done();
    CLOG(( EConn, 2, _L("<- CConnection::RunL") ));
    }

// ---------------------------------------------------------
// CConnection::RunError()
// ---------------------------------------------------------
//
TInt CConnection::RunError( TInt LOG_ONLY( aError ) )
    {
    CLOG(( EConn, 2, _L("-> CConnection::RunError aError(%d)"), aError ));
    __ASSERT_DEBUG( iState == EConnecting, CodPanic( ECodInternal ) );
    DoClose();
    Done();
    CLOG(( EConn, 2, _L("<- CConnection::RunError") ));
    return KErrNone;
    }

// ---------------------------------------------------------
// CConnection::DoClose()
// ---------------------------------------------------------
//
void CConnection::DoClose()
    {
    CLOG(( EConn, 2, _L("CConnection::DoClose") ));
    iConn.Close();
    iSockServ.Close();
    iState = EInit;
    }

// ---------------------------------------------------------
// CConnection::Done()
// ---------------------------------------------------------
//
void CConnection::Done()
    {
    CLOG(( EConn, 2, _L("CConnection::Done iStatus(%d)"), iStatus.Int() ));
    __ASSERT_DEBUG( iParentStatus, CodPanic( ECodInternal ) );
    User::RequestComplete( iParentStatus, iStatus.Int() );
    iParentStatus = NULL;
    }

#ifdef BRDO_OCC_ENABLED_FF
// ---------------------------------------------------------
// CConnection::IsPhoneOfflineL
//
// Checks if phone is in offline mode or not.
// Return ETrue if phone is in offline mode.
// Return EFalse if phone is not in offline mode.
// ---------------------------------------------------------
//
TBool CConnection::IsPhoneOfflineL() const
     {
      if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
         {
         CRepository* repository = CRepository::NewLC( KCRUidCoreApplicationUIs );
         TInt connAllowed = 1;
         repository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );
         CleanupStack::PopAndDestroy();  // repository
         if ( !connAllowed )
             {
             CLOG(( EConn, 2, _L("Yes, Phone is in Offline mode") ));
             return ETrue;
             }
         }
     
     CLOG(( EConn, 2, _L("Phone is in Online mode") ));
     return EFalse;
     }

// ---------------------------------------------------------
// CConnection::IsRoamingL
//
// Checks if phone is in home network or in roam network.
// Return ETrue if phone is in foriegn network.
// Return EFalse if phone is in home network.
// ---------------------------------------------------------
//
TBool CConnection::IsRoamingL()
    {
        RTelServer telServer;
        User::LeaveIfError( telServer.Connect());
        
        RTelServer::TPhoneInfo teleinfo;
        User::LeaveIfError( telServer.GetPhoneInfo( 0, teleinfo ) );
        
        RMobilePhone phone;
        User::LeaveIfError( phone.Open( telServer, teleinfo.iName ) );
        User::LeaveIfError(phone.Initialise()); 
        
        RMobilePhone::TMobilePhoneNetworkMode mode;                     
        TInt err = phone.GetCurrentMode( mode );
        phone.Close();
        telServer.Close();
        TInt Bearer = EBearerIdGSM ;
        if( KErrNone == err )
        {
           switch(mode)
           {
                case RMobilePhone::ENetworkModeGsm:     
                {           
                Bearer = EBearerIdGSM ;
                    break;      
                }
                case RMobilePhone::ENetworkModeWcdma:
                {                                   
                    Bearer = EBearerIdWCDMA  ;
                    break;  
                }   
                default: 
                {                   
                
                }                       
            }
        }   
        RConnectionMonitor monitor;
        TRequestStatus status;
        // open RConnectionMonitor object
        monitor.ConnectL();
        CleanupClosePushL( monitor );
        TInt netwStatus ;
        monitor.GetIntAttribute( Bearer, 0, KNetworkRegistration, netwStatus, status );
        User::WaitForRequest( status );
        CleanupStack::PopAndDestroy(); // Destroying monitor
        if ( status.Int() == KErrNone && netwStatus == ENetworkRegistrationRoaming )
        {
            CLOG(( EConn, 2, _L("Yes, Phone is in Forign network") ));
            return ETrue;
        }
        else //home n/w or some other state in n/w
        {
            CLOG(( EConn, 2, _L("Phone is in Home network") ));
            return EFalse;
        }
   }
#endif
