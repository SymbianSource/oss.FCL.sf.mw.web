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
* Description:  File-name related utility methods.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <CDirectoryLocalizer.h>
#include <f32file.h>
#include "BrsrStatusCodes.h"
#include "FileNameUtils.h"
#include "nwx_assert.h"

// CONSTANTS

// A localized file name can be larger than a standard file name. A size of 50%
// more should provide adequate space.
const TInt KMaxLocalizedFileName = (KMaxFileName + (KMaxFileName >> 1));

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// FileNameUtils::LocalizePath
// Gets the localized file name, in full, which includes the path but
// not the drive. This is intended for use as a displayable path and
// as such the drive is not included since drive letters are never shown
// to the user.
//
// Note, returns newly allocated memory in aLocalizedFullFileName that the
// caller is responsible for deleting.
//
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
FileNameUtils::GetLocalizedFullFileName( TPtrC& aFullFileName,
    HBufC** aLocalizedFullFileName )
    {
    TInt err;
    TInt len;
    TBrowserStatusCode status = KBrsrSuccess;
    CDirectoryLocalizer* localizer = NULL;
    HBufC* buf1 = NULL;
    HBufC* buf2 = NULL;

    NW_ASSERT( aLocalizedFullFileName );

    // Instantiate a directory localizer.
    TRAP( err, localizer = CDirectoryLocalizer::NewL() );
    if ( err )
        {
        // Map the system error to browser error: out-of-memory or failure.
        status = (err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure;
        goto _Exit;
        }

    // Allocate two buffers. Both are used as work areas. In the end, buf1 will
    // contain the localized file name, which will be returned to the caller,
    // and buf2 will be deleted.
    buf1 = HBufC::New( KMaxLocalizedFileName );
    buf2 = HBufC::New( KMaxLocalizedFileName );
    if ( !buf1 || !buf2 )
        {
        status = KBrsrOutOfMemory;
        goto _Exit;
        }

    // The following "if" statement is used more as a way to enclose the next
    // section of code in braces, creating it's own block - necessary to keep
    // compiler from generating warnings about uninitialized variables due to
    // the goto-exit-code-pathways above.
    if ( status == KBrsrSuccess )
        {
        // Pointers to the two newly allocated work buffers.
        TPtr ptr1( buf1->Des() );
        TPtr ptr2( buf2->Des() );

        // Create a file parser for the original full-file name.
        TParsePtrC fileParserOrigFileName( aFullFileName );

        // Loop through the sub-directories, localizing each one in the path.
        // The localized sub-directory is appended into buf2. Buf2 ends up
        // with all of the localized sub-directories but in reverse order. The
        // contents of buf2 will then be reversed again into buf1 to get the
        // correct order.
        TInt loc;
        TPtrC localName;
        // To start with, buf1 (ptr1) gets a copy of the orginal drive and path.
        // This copy will gradually be taken apart as each sub-directory gets
        // popped in the loop below.
        ptr1.Copy( fileParserOrigFileName.DriveAndPath() );
        TParsePtr fileParser1( ptr1 );
        TPtrC driveAndPath( fileParser1.DriveAndPath() );
        while ( (loc = driveAndPath.LocateReverse( '\\' )) != KErrNotFound )
            {
            localizer->SetFullPath( driveAndPath );

            // Check to see if current sub-directory is localized.
            if ( localizer->IsLocalized() )
                {
                // If localized, use localized sub-directory.
                localName.Set( localizer->LocalizedName() );
                ptr2.Append( '\\' );
                }
            else
                {
                // If not localized, use the sub-directory name as it is,
                // extracting it from the end of the path.

                // Remove trailing backslash.
                driveAndPath.Set( driveAndPath.Left( loc ) );
                // Get length of sub-directory.
                if ( (loc = driveAndPath.LocateReverse( '\\' )) == KErrNotFound )
                    {
                    len = 0;
                    }
                else
                    {
                    len = driveAndPath.Length() - loc;
                    }
                // Extract the sub-directory, including its leading backslash.
                localName.Set( driveAndPath.Right( len ) );
                }
            ptr2.Append( localName );

            // Prepare for next iteration through loop by popping the
            // sub-directory just processed and setting driveAndPath to the new
            // value, which has the sub-directory removed.
            err = fileParser1.PopDir();
            if ( err != KErrNone )
                {
                break;
                }
            driveAndPath.Set( fileParser1.DriveAndPath() );
            }

        // Work buf2 contains the localized path in reverse order. Reverse it
        // back to the correct order, putting the corrected path in buf1.
        ptr1.Zero();
        while ( (loc = ptr2.LocateReverse( '\\' )) != KErrNotFound  )
            {
            ptr1.Append( ptr2.Right( ptr2.Length() - loc ) );
            ptr2.SetLength( loc );
            }

        // Append the file name and extension.
        TPtrC nameAndExt( fileParserOrigFileName.NameAndExt() );
        len = ptr1.Length();
        if ( !len || (ptr1[len - 1] != '\\') )
            {
            ptr1.Append( '\\' );
            }
        ptr1.Append( nameAndExt );

        // Return the allocated buffer with the localized path.
        *aLocalizedFullFileName = buf1;
        buf1 = NULL; // set to null so it doesn't get deallocated below.

        }

_Exit:
    delete localizer;
    delete buf1;
    delete buf2;
    return status;
    }

//  End of File
