/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handle notifications of MMC events.
*
*
*
*/

#include "wrtusbhandler.h"
#include "wrtharvester.h"
#include "wrtusbhandler.h"

// CONSTANTS


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
    CWrtHarvester* aHarvester,
    RFs& aFs )
    {
    CWidgetMMCHandler* self =
        new(ELeave) CWidgetMMCHandler( aHarvester , aFs );
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
CWidgetMMCHandler::CWidgetMMCHandler( CWrtHarvester* aHarvester,
                                      RFs& aFs )
    : CActive( CActive::EPriorityUserInput ),
      iHarvester( aHarvester ),
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
    if ( KErrNone != ScanDrives( iDriveFlags ) )
        {
        iDriveFlags = 0;
        }
        RDebug::Print(_L("iDriveFlags ConstructL() : %x %d"), iDriveFlags,iDriveFlags);
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
    if ( iStatus == KErrNone )
      {
        TInt driveFlags = 0;
        TInt deltaDriveFlags = 0;
      

         User::After( 10000000 );
         
       if ( KErrNone == ScanDrives( driveFlags ) )
         {
            deltaDriveFlags = iDriveFlags ^ driveFlags;
            
            iDriveFlags = driveFlags;
         }
        
       if ( deltaDriveFlags )
          {
          //Unpluging USB from Mass storage . . . 
          if(iHarvester->IsInMSMode() == 1)
            {
             iHarvester->SetMSMode(0);
             iHarvester->ClearAllOperations();
             iHarvester->UpdateL();
             
             iFs.NotifyChange( ENotifyDisk, iStatus );
             SetActive();
             return;
            }

          TVolumeInfo volInfo;
          TInt temp = deltaDriveFlags;
          TBool massMemAltered = EFalse;
          TBool mmcAltered = EFalse;
          for(TInt DriveNo = EDriveA+1 ; DriveNo<=EDriveY; DriveNo++ )
            {
                
            temp =  temp >> 1;
            if( temp & 01)
              {
                switch (DriveNo)
                  {
                    case EDriveE :
                      {
                      massMemAltered = ETrue;   
                      break; 
                      }
                    case EDriveF:
                      {
                           
                      mmcAltered = ETrue;
                      break;
                      }
                         
                   }
              }
            
            }
                     
       if( !massMemAltered && mmcAltered)    
         {
          iHarvester->UpdateL();   
         }
       else if( massMemAltered )
         {
          iHarvester->SetMSMode(1);  
          iHarvester->UpdateL(); 
         }

        }
      }
      
      
    iFs.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    }

// ============================================================================
// CWidgetMMCHandler::RunError()
// Ignore errors from RunL.
//
// @since 5.0
// ============================================================================
TInt CWidgetMMCHandler::RunError( TInt /* aError */ )
    {

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

    iFs.NotifyChangeCancel();
    }

/* Scans drives and records a bit flag for those that exist and are
 * suitable for installing widgets to.
 */
TInt CWidgetMMCHandler::ScanDrives( TInt& aDriveFlags )
    {

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
          
            }
        }

    return error;
    }
