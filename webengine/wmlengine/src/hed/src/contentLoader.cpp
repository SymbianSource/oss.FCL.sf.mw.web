/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


// This module forwards content that is not handled by the browser to the OS
// document handler.

#include <e32base.h>
#include <AiwVariant.h>    
#include <apmstd.h>             // for TDataType
#include <ErrorUI.h>            // for Series60 error notes
#include <eikenv.h>
#include "nwx_http_defs.h"      
#include "contentLoader.h"
#include "MVCShell.h"
#include "BrsrStatusCodes.h"
#include <BrCtlSpecialLoadObserver.h>
#include "StringUtils.h"

// CONSTANTS

CContentLoader::CContentLoader( CEikProcess* aProcess )
    {
    iFileName = NULL;
    iProcess = aProcess;        // For embedding.
    }

CContentLoader::~CContentLoader()
    {    
    if (iFileName != NULL)
        {
        iRfs.Delete(*iFileName);
        delete iFileName;
        iFileName = NULL;
        }
    }

void CContentLoader::SetMimeTypeL( TUint8* aContentType, TDes8& aMimetype )
    {
    
    __ASSERT_DEBUG(aContentType != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );


    //lint -e{774} Boolean within 'if' always evaluates to True
        if ((aContentType != NULL) && 
            (NW_Asc_strlen((char*)aContentType) <= (NW_Uint32)aMimetype.MaxLength()))
        {
            aMimetype.Copy(aContentType);
        }
        else
        {
            User::Leave(KBrsrWmlbrowserBadContentType);
        }
    }


void CContentLoader::HandleL( HBufC* aUrl, HBufC8* aCharset, HBufC8* aContentType,
                              struct NW_HED_DocumentRoot_s* aDocRoot,
                              const TDesC& aFileName, TBool* aNeedDelete )
    {    
    __ASSERT_DEBUG(aContentType != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );
    __ASSERT_DEBUG(aNeedDelete != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );
    *aNeedDelete = NW_TRUE;

    //lint -e{550} Symbol not accessed
    NW_REQUIRED_PARAM(aUrl);

	/* if embedded browser, let the embedding app deal with the unsupported content */
    CShell* shell = (CShell*)aDocRoot->browserApp_Ctx; 
    /* Add the AP Id to the parameter list */
    __ASSERT_DEBUG(shell != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );
 
	// these arrays are pushed into CleanupStack in case leave
	// if no leave, they will be freed below
    RArray<TUint>* aTypeArray = new (ELeave) RArray<TUint>(4);
    CleanupStack::PushL(aTypeArray);

	CDesCArrayFlat* aDesArray = new (ELeave) CDesCArrayFlat(4);
    CleanupStack::PushL(aDesArray);

	TPtrC url16 = aUrl->Des(); 
    User::LeaveIfError(aTypeArray->Append(EParamRequestUrl));
    aDesArray->AppendL(url16);

	TDesC16* stringDes;
    if (aCharset != NULL)
        {
        stringDes = StringUtils::CopyTDesC8ToTDesC16(aCharset->Des());
        if (stringDes)
            {
            CleanupStack::PushL(stringDes);
            User::LeaveIfError(aTypeArray->Append(EParamCharset));
            aDesArray->AppendL(*stringDes);
            CleanupStack::Pop(stringDes);
            }
        }

    if (aContentType != NULL)
        {
        stringDes = StringUtils::CopyTDesC8ToTDesC16(aContentType->Des());
        if (stringDes)
            {
            CleanupStack::PushL(stringDes);
            User::LeaveIfError(aTypeArray->Append(EParamReceivedContentType));
            aDesArray->AppendL(*stringDes);
            CleanupStack::Pop(stringDes);
            }
        }

    User::LeaveIfError(aTypeArray->Append(EParamLocalFileName));
    aDesArray->AppendL(aFileName);
    
    shell->HandleDownloadL(aTypeArray, aDesArray);
    iFileName = aFileName.AllocL();
    *aNeedDelete = NW_FALSE;

	// No leave, so pop here and clean up
    CleanupStack::Pop(aDesArray);
    CleanupStack::Pop(aTypeArray);

    // cleanup arrays
   if (aTypeArray)
	 {
	 // Closes the array and frees all memory allocated to the array
     aTypeArray->Close();
     delete aTypeArray;
     }
   
   if (aDesArray)
     {
	 // Deletes all descriptors from the array and frees the memory allocated to the array buffer
     aDesArray->Reset(); 
     delete aDesArray;
     }
}


/*
 * HandleContent
 *
 * External C function used to forward content not handled by the
 * browser to the OS document handler
 *
 * Parameters:
 *  aResponse - Response data to handle
 *
 * Returns:
 *  Status code
 */
TBrowserStatusCode HandleContent(HBufC* aUrl, HBufC8* aCharset, HBufC8* aContentType,
                                 struct NW_HED_DocumentRoot_s* aDocRoot,
                                 const TDesC& aFileName, TBool* aNeedDelete)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    TInt error = KErrNone;
    
    __ASSERT_DEBUG(aDocRoot != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );
    CShell* shell = (CShell*)aDocRoot->browserApp_Ctx; 
    __ASSERT_DEBUG(shell != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );
    CContentLoader *contentLoader = shell->iContentLoader;
    __ASSERT_DEBUG(contentLoader != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );

    // Send to the browser's CContentLoader object.
    TRAP(error, contentLoader->HandleL(aUrl, aCharset, aContentType, aDocRoot, aFileName,  aNeedDelete));
    if ( error != KErrNone && error != KLeaveWithoutAlert )
      {
      switch(error )
        {
        case KErrNoMemory:
            status = KBrsrOutOfMemory;
            break;
        case KBrsrWmlbrowserBadContentType:
            status = KBrsrWmlbrowserBadContentType;
            break;
        default:
            // All other errors get passed along to the error handler as is.
            // There are many possible errors such as corrupt DRM files, etc.
            status = KBrsrFailure;
            break;
        }
      NW_ASSERT(aDocRoot->appServices != NULL);
      (void) aDocRoot->appServices->errorApi.notifyError (NULL, error, NULL, NULL);
      }

    return status;
    }

void DeletePrevContentL(struct NW_HED_DocumentRoot_s* aDocRoot)
    {
    __ASSERT_DEBUG(aDocRoot != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );
    CShell* shell = (CShell*)aDocRoot->browserApp_Ctx; 
    __ASSERT_DEBUG(shell != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );
    CContentLoader *contentLoader = shell->iContentLoader;
    __ASSERT_DEBUG(contentLoader != NULL, User::Panic( _L("ContentLoader"), KErrArgument) );

    contentLoader->iRfs = shell->iRfs; // Pass reference to RFs, needed in the deconstructor
    if (contentLoader->iFileName != NULL)
        {
        contentLoader->iRfs.Delete(*contentLoader->iFileName);
        delete contentLoader->iFileName;
        contentLoader->iFileName = NULL;
        }
    }
