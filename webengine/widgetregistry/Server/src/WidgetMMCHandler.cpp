//
// ============================================================================
//  Name     : WidgetMMCHandler.cpp
//  Part of  : SW Installer UIs / WidgetInstallerUI
//
//  Description: Handle notifications of MMC events.
//
//
//  Version     : 3.1
//
//  Copyright � 2006 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ==============================================================================
///

// INCLUDE FILES
#include "WidgetMMCHandler.h"
#include "WidgetRegistry.h"

// CONSTANTS
LOG_NAMES( "widreg", "widrmmc.txt" )

// ============================================================================
// CWidgetMMCHandler::NewL()
// two-phase constructor
//
// @since 3.1
// @param aRegistry - Widget registry for callback.
// @param aFs - file session
// @return pointer to CWidgetMMCHandler
// ============================================================================
//
CWidgetMMCHandler* CWidgetMMCHandler::NewL(
    CWidgetRegistry& aRegistry,
    RFs& aFs )
    {
    CWidgetMMCHandler* self =
        new(ELeave) CWidgetMMCHandler( aRegistry , aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ============================================================================
// CWidgetMMCHandler::CWidgetMMCHandler()
// C++ default constructor
//
// @since 3.1
// ============================================================================
//
CWidgetMMCHandler::CWidgetMMCHandler( CWidgetRegistry& aRegistry,
                                      RFs& aFs )
    : CActive( CActive::EPriorityStandard ),
      iRegistry( &aRegistry ),
      iFs( aFs ),
      iDriveFlags( 0 )
    {
    CActiveScheduler::Add( this );
    }

// ============================================================================
// CWidgetMMCHandler::ConstructL()
// Symbian default constructor
//
// @since 3.1
// ============================================================================
//
void CWidgetMMCHandler::ConstructL()
    {
    LOG_CONSTRUCTL;
    if ( KErrNone != ScanDrives( iDriveFlags ) )
        {
        iDriveFlags = 0;
        }
    iDeltaDriveFlags = 0;
    iApaAppListNotifier = CApaAppListNotifier::NewL(this,CActive::EPriorityStandard);
    }

// ============================================================================
// CWidgetMMCHandler::~CWidgetMMCHandler()
// destructor
//
// @since 3.1
// ============================================================================
CWidgetMMCHandler::~CWidgetMMCHandler()
    {
    Cancel();
    LOG_DESTRUCT;
    }

// ============================================================================
// CWidgetMMCHandler::Start()
// Start monitoring.
//
// @since 5.0
// ============================================================================
void CWidgetMMCHandler::Start()
    {
    iFs.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    }

// ============================================================================
// CWidgetMMCHandler::RunL()
// Handle notifications of MMC events.
//
// @since 3.1
// ============================================================================
void CWidgetMMCHandler::RunL()
    {
    LOG_OPEN;
    LOG1( "MMC notification status %d", iStatus.Int() );

	  TInt status = iStatus.Int();
	
	  // Request the notification before scanning
	  iFs.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    
    if ( status == KErrNone )
      {
        TInt driveFlags = 0;
        TInt deltaDriveFlags = 0;
        if ( KErrNone == ScanDrives( driveFlags ) )
          {
            LOG1( " iDriveFlags 0x%x", iDriveFlags );
            LOG1( "  driveFlags 0x%x", driveFlags );
            deltaDriveFlags = iDriveFlags ^ driveFlags;
            iDeltaDriveFlags |= deltaDriveFlags;
            LOG1( " deltaDriveFlags 0x%x", deltaDriveFlags );
            iDriveFlags = driveFlags;
          }
      }
    LOG( "MMC notification done" );
    LOG_CLOSE;
    }

// ============================================================================
// CWidgetMMCHandler::RunError()
// Ignore errors from RunL.
//
// @since 5.0
// ============================================================================
TInt CWidgetMMCHandler::RunError( TInt /* aError */ )
    {
    LOG_CLOSE;
    return KErrNone; // indicates error was handled
    }

// ============================================================================
// CWidgetMMCHandler::DoCancel()
// Cancel the MMC event handler
//
// @since 3.1
// ============================================================================
void CWidgetMMCHandler::DoCancel()
    {
    LOG_CLOSE;
    iFs.NotifyChangeCancel();
    }

/* Scans drives and records a bit flag for those that exist and are
 * suitable for installing widgets to.
 */
TInt CWidgetMMCHandler::ScanDrives( TInt& aDriveFlags )
    {
    LOG_OPEN;
    LOG( "ScanDrives" );
    // List all drives in the system
    TDriveList driveList;
    TInt error = iFs.DriveList( driveList );
    if ( KErrNone == error )
        {
        for ( TInt driveNumber = EDriveY;
              driveNumber >= EDriveA;
              driveNumber-- )
            {
            // The drives that will be filtered out are the same ones that
            // WidgetInstaller filters out in CWidgetUIHandler::SelectDriveL()
            if ( (EDriveD == driveNumber)
                 || !driveList[driveNumber] )
                {
                // EDriveD is a temporary drive usually a RAM disk
                continue;
                }

            TVolumeInfo volInfo;
            if ( iFs.Volume( volInfo, driveNumber ) != KErrNone )
                {
                // volume is not usable (e.g. no media card inserted)
                continue;
                }
            if ( (volInfo.iDrive.iType == EMediaNotPresent) ||
                 (volInfo.iDrive.iType == EMediaRom) ||
                 (volInfo.iDrive.iType == EMediaRemote) ||
                 (volInfo.iDrive.iDriveAtt & KDriveAttRom) ||
                 (volInfo.iDrive.iDriveAtt & KDriveAttSubsted) )
                {
                // not a suitable widget install drive
                continue;
                }

            // found a usable drive
            aDriveFlags |= (1 << driveNumber);
            LOG2( " drive %d, flags 0x%x", driveNumber, aDriveFlags );
            }
        }
    LOG1( "ScanDrives done, error %d", error );
    LOG_CLOSE;
    return error;
    }

void CWidgetMMCHandler::HandleAppListEvent(TInt aEvent)
    {
    TBool dirtyFlag = EFalse;
    TInt parseError = KErrNone;
    
    if ( iDeltaDriveFlags )
        {
        // Assume usual case and things are consistent
        // and the registry entry file can be parsed and used.
        TRAPD( error, iRegistry->InternalizeL( ETrue,
                                    ETrue,
                                    dirtyFlag,
                                    parseError ) );
        if ( KErrNone == error )
            {
            // internalize consistency enforcement may have altered registry
            if ( dirtyFlag )
                {
                TRAP_IGNORE( iRegistry->ExternalizeL(); );
                }
            }
        iDeltaDriveFlags = 0;
        
        CWidgetRegistry::NotifyWidgetAltered();
        }
    }
