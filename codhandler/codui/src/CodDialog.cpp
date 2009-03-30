/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of class CodDialog.
*
*/


#include <coemain.h>
#include <AknNoteWrappers.h>
#include <AknQueryDialog.h>
#include <CAknMemorySelectionDialog.h>
#include <CodUi.rsg>
#include "CodDialog.h"
#include "CodLogger.h"
#include <pathinfo.h>
#ifndef RD_MULTIPLE_DRIVE
_LIT(KBackSlash,"\\");
#endif

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif

// CONSTANTS
#ifdef RD_MULTIPLE_DRIVE
const TInt KMaxDriveListStrLen = KMaxDrives << 1;
_LIT( KDefaultDriveListSep, ";" );
_LIT( KDefaultDriveList, "c;e" );
#endif

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CodDialog::GetRootPathL()
// ----------------------------------------------------------
//

void CodDialog::GetRootPathL( TDes& aRootPath )
    {
    CLOG(( 2, _L("-> CodDialog::SelectDownloadLocationL") ));
    CRepository *repository = CRepository::NewLC(KCRUidBrowser);
	aRootPath=KNullDesC;
	RFs fs;
 	User::LeaveIfError(fs.Connect());

#ifdef RD_MULTIPLE_DRIVE 
    TDriveList driveList;
    TInt driveCount( 0 );
    TChar driveLetter;
    TBuf8<KMaxDriveListStrLen> driveLettersDyn;
    TBuf<KMaxDrives> driveListCenRep;

    // Checking validity of drives in Cenrep List
    // drive letters are separated by semicolons
    // Destination is FFS in default
    TInt drive;
    User::LeaveIfError(
        DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive ) );

	if( repository->Get(KBrowserDrivePrefListForDownloadedContent, driveListCenRep) == KErrNone )
        {
        driveListCenRep.Copy( KDefaultDriveList );
        }

    // get the list of drives available in real time
    if ( DriveInfo::GetUserVisibleDrives( fs, driveList, driveCount ) == KErrNone )
        {
        if ( driveListCenRep.Length() > 0 )
            {
                driveLettersDyn.Append( driveListCenRep );
                if ( driveLettersDyn[driveLettersDyn.Length() - 1] != ';' )
                    {
                    driveLettersDyn.Append( KDefaultDriveListSep );
                    }
            }
        TInt count( driveList.Length() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            if ( driveList[ i ] )
                {
                User::LeaveIfError( fs.DriveToChar( i, driveLetter) );
                TInt drivePos = driveLettersDyn.LocateF( driveLetter );
                if ( drivePos == KErrNotFound )
                    {
                    driveLettersDyn.Append( driveLetter ); 
                    driveLettersDyn.Append( KDefaultDriveListSep ); 
                    }
                }
            }
        }
	    TPtrC8 drives(driveLettersDyn);
	    TInt err(KErrNone);
	    for( TInt i = 0; i < drives.Length() && err; i = i + 2 )
	        {
	            err = fs.CharToDrive( drives[i], drive ); 
	        }
        User::LeaveIfError( PathInfo::GetRootPath( aRootPath, drive ) );
#else
	TBuf<KMaxDrives> driveList;
	TInt drive( EDriveC );

	if( repository->Get(KBrowserDrivePrefListForDownloadedContent, driveList) == KErrNone )
        {
	    TPtrC drives(driveList);
	    TInt err(KErrNone);
	    for( TInt i = 0; i < drives.Length(); i = i + 2 )
	        {
	            err = fs.CharToDrive( drives[i], drive );
	            if (err == KErrNone)
	            		break;	            
	        }
	    TDriveUnit driveUnit(drive);
	    aRootPath = driveUnit.Name();
	    aRootPath.Append(KBackSlash);
        }
#endif
    fs.Close();
	CleanupStack::PopAndDestroy(repository);   
    CLOG(( 2, _L("<- CodDialog::GetRootPathL <%S>"), &aRootPath ));
    }

// ----------------------------------------------------------
// CodDialog::ConfirmConnectL()
// ----------------------------------------------------------
//
TBool CodDialog::ConfirmConnectL()
    {
    CLOG(( 2, _L("-> CodDialog::ConfirmConnectL") ));
    TBool ret( ETrue );
    // TODO
    CLOG(( 2, _L("<- CodDialog::ConfirmConnectL(%d)"), ret ));
    return ret;
    }

// ----------------------------------------------------------
// CodDialog::InfoNoteL()
// ----------------------------------------------------------
//
void CodDialog::InfoNoteL( TInt aPromptResourceId, CCoeEnv& aCoeEnv )
    {
    HBufC* prompt = aCoeEnv.AllocReadResourceLC( aPromptResourceId );
    CAknInformationNote* note = new (ELeave) CAknInformationNote();
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

// ----------------------------------------------------------
// CodDialog::InfoNoteL()
// ----------------------------------------------------------
//
void CodDialog::InfoNoteL( const TDesC& aPrompt )
    {
    CAknInformationNote* note = new (ELeave) CAknInformationNote( ETrue );
    note->ExecuteLD( aPrompt );
    }

// ----------------------------------------------------------
// CodDialog::ConfNoteL()
// ----------------------------------------------------------
//
void CodDialog::ConfNoteL( TInt aPromptResourceId, CCoeEnv& aCoeEnv )
    {
    HBufC* prompt = aCoeEnv.AllocReadResourceLC( aPromptResourceId );
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

// ----------------------------------------------------------
// CodDialog::ConfNoteL()
// ----------------------------------------------------------
//
void CodDialog::ConfNoteL( const TDesC& aPrompt )
    {
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
    note->ExecuteLD( aPrompt );
    }
