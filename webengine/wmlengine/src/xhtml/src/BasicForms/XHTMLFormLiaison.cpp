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


#include <e32def.h>
#include <calslbs.h>
#include <StringLoader.h>
#include <webkit.rsg>
#include <commoncontentpolicy.h>

#include "BrCtl.h"
#include "BrCtlDialogsProvider.h"

#include "BrsrStatusCodes.h"
#include "MVCShell.h"
#include "Epoc32DialogProvider.h"
#include "nw_adt_mapiterator.h"
#include "nw_basicforms_xhtmlformliaisoni.h"
//#include "nw_ecma_contenthandler.h"
#include "nw_dom_document.h"
#include "nw_dom_domvisitor.h"
#include "nw_fbox_formbox.h"
#include "nw_fbox_optgrpbox.h"
#include "nw_fbox_validator.h"
#include "nw_hed_documentroot.h"
#include "nw_loadreq.h"
#include "nw_nvpair.h"
#include "nw_object_exceptions.h"
#include "nwx_http_defs.h"
#include "nwx_settings.h"
#include "nwx_string.h"
//R->ul #include "urlloader_urlloaderint.h"
#include "TEncodingMapping.h"


// LOCAL CONSTANTS
_LIT(KEmptyTitle, "");

#if 0
#if (defined(_DEBUG)) || (defined(_ENABLE_LOGS))
#define XFL_LOGGING 1
#include <FLogger.h>

    _LIT(KLogFileName, "XhtmlFormLiaison.log");
    _LIT(KLogDirName, "Browser");
#endif
#endif
static NW_Bool NW_XHTML_FormLiaison_IsSupportedEncoding(NW_Uint32 encoding)
{
    switch (encoding)
    {
      case HTTP_gb2312:
      case HTTP_big5:
      case HTTP_iso_8859_8:
      case HTTP_iso_8859_6:
      case HTTP_iso_8859_2:
      case HTTP_iso_8859_4:
      case HTTP_iso_8859_5:
      case HTTP_iso_8859_7:
      case HTTP_iso_8859_9:
      case HTTP_windows_1255:
      case HTTP_windows_1256:
      case HTTP_windows_1250:
      case HTTP_windows_1251:
      case HTTP_windows_1253:
      case HTTP_windows_1254:
      case HTTP_windows_1257:
      case HTTP_tis_620:  // not flagged because we will not get here unless tis_620 is enabled
      case HTTP_shift_JIS:
      case HTTP_jis_x0201_1997:
      case HTTP_jis_x0208_1997:
      case HTTP_euc_jp:
      case HTTP_iso_2022_jp:
      case HTTP_windows_874:
      case HTTP_Koi8_r:   //Russian
      case HTTP_Koi8_u:   //Ukrainian
        return NW_TRUE;
      default:
          return NW_FALSE;
    }
}


/* ------------------------------------------------------------------------- *
   private types
 * ------------------------------------------------------------------------- */
typedef enum NW_XHTML_FormControlType_e {
  NW_XHTML_FormControlType_None,
  NW_XHTML_FormControlType_InputCheckbox, /* <input type=checkbox> */
  NW_XHTML_FormControlType_InputFile,     /* <input type=file> */
  NW_XHTML_FormControlType_InputHidden,   /* <input type=hidden> */
  NW_XHTML_FormControlType_InputPassword, /* <input type=password> */
  NW_XHTML_FormControlType_InputRadio,    /* <input type=radio> */
  NW_XHTML_FormControlType_InputReset,    /* <input type=reset> */
  NW_XHTML_FormControlType_InputSubmit,   /* <input type=submit> */
  NW_XHTML_FormControlType_InputText,     /* <input type=text> */
  NW_XHTML_FormControlType_InputImage,     // <input type=image>
  NW_XHTML_FormControlType_Option,        /* <option> */
  NW_XHTML_FormControlType_Select,        /* <select> */
  NW_XHTML_FormControlType_Textarea      /* <textarea> */
} NW_XHTML_FormControlType_t;

static const
NW_Object_Core_Class_t NW_XHTML_FormLiaisonTrue_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Core_Class,
    /* queryInterface          */ _NW_Object_Core_QueryInterface
  }
};

static const
NW_Object_Core_t NW_XHTML_FormLiaisonTrue = {
  (NW_Object_t*) &NW_XHTML_FormLiaisonTrue_Class,
};

static const
NW_Object_Core_Class_t NW_XHTML_FormLiaisonFalse_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Core_Class,
    /* queryInterface          */ _NW_Object_Core_QueryInterface
  }
};

static const
NW_Object_Core_t NW_XHTML_FormLiaisonFalse = {
  (NW_Object_t*) &NW_XHTML_FormLiaisonFalse_Class,
};


// LOCAL FUNCTION PROTOTYPES

static
TBrowserStatusCode
NW_XHTML_FormLiaison_CheckFiles( NW_XHTML_FormLiaison_t* aThisObj,
    NW_NVPair_t* aNameValuePairs, TBool* aMissingFiles, TBool* aRestrictedFiles );

static
TBrowserStatusCode
NW_XHTML_FormLiaison_DisplayFileMissingConfirmationPrompt( TInt aMissingFiles,
    TDesC& aMissingFileName, TBool& aDialogOk );

static
TBrowserStatusCode
NW_XHTML_FormLiaison_DisplayFileRestrictedConfirmationPrompt( TInt aRestrictedFiles,
    TDesC& aRestrictedFileName, TBool& aDialogOk );

static
TBrowserStatusCode
NW_XHTML_FormLiaison_GetNameValuePairs( NW_XHTML_FormLiaison_t* aThisObj,
    void* aSubmitButtonNode, NW_NVPair_t** aNameValuePairs );

static
TBrowserStatusCode
NW_XHTML_FormLiaison_IterateSuccessfulControls( NW_XHTML_FormLiaison_t* formLiaison,
    NW_DOM_ElementNode_t* submitButtonNode, NW_NVPair_NVFunction_t* controlCallback,
    NW_NVPair_t* controlCallbackData );

static
TBrowserStatusCode
NW_XHTML_FormLiaison_ResetFieldForName( NW_XHTML_FormLiaison_t* aThisObj,
    NW_Ucs2* aName );


/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_XHTML_FormLiaison::DisplayFileMissingConfirmationPrompt
// Displays a confirmation prompt, notifying the user that 1 or more files
// were missing and giving the user the choice to continue or cancel.
// @param aMissingFiles Number of files missing.
// @param aMissingFileName Name of the missing file if there is only one
//      missing. If more than one missing, this argument is ignored and can be
//      NULL.
// @param aDialogOk Returned value EFalse if the user canceled, ETrue otherwise.
// @returns Browser status code:
//      KBrsrSuccess if no errors
//      KBrsrOutOfMemory if not enough memory
//      KBrsrFailure for all other failure conditions
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_XHTML_FormLiaison_DisplayFileMissingConfirmationPrompt(TInt aMissingFiles,
                                                          TDesC& aMissingFileName,
                                                          TBool& aDialogOk)
    {
    TInt err = KErrNone;
    HBufC* message = NULL;
    HBufC* okMsg = NULL;
    HBufC* cancelMsg = NULL;
    
    // Parameter assertion block.
    NW_ASSERT(aMissingFiles >= 1);
    
    // In the event an error occurs, default is dialog canceled.
    aDialogOk = EFalse;
    
    NW_TRY(status)
        {
        // Choose which prompt to use. If only one file, then its name is
        // displayed. If more than one, then the number of missing files
        // is displayed.
        if (aMissingFiles == 1)
            {
            // Extact name.ext from fully-qualified path.
            TPtrC ptr(aMissingFileName);
            TParsePtrC fileNameParser(ptr);
            TPtrC nameAndExt(fileNameParser.NameAndExt());
            
            // Load the resource string, inserting the name&ext, add txt to cleanupstack.
            /*TRAP(err, message =
                 StringLoader::LoadLC(R_QTN_XHTML_QUERY_FILE_MISSING, nameAndExt);
                 CleanupStack::Pop(message););*/
            }
        else
            {
            // Load the resource string, inserting # of missing files.
            /*TRAP(err, message = 
                 StringLoader::LoadLC(R_QTN_XHTML_QUERY_FILES_MISSING, aMissingFiles);
                 CleanupStack::Pop(message););*/
            }
        if (err)
            {
            NW_THROW_STATUS(status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure));
            }
        
        // Set up the OK button text.
        TRAP(err, okMsg = StringLoader::LoadLC(R_WML_OK_BUTTON); CleanupStack::Pop(okMsg););
        if (err)
            {
            NW_THROW_STATUS(status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure));
            }

        // Set up the CANCEL button text.
        TRAP(err, cancelMsg = StringLoader::LoadLC(R_WML_CANCEL_BUTTON); CleanupStack::Pop(cancelMsg););
        if (err)
            {
            NW_THROW_STATUS(status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure));
            }
//R->ul
        CShell* shell = NULL;//REINTERPRET_CAST(CShell*, NW_Ctx_Get(NW_CTX_BROWSER_APP, 0));  
        NW_ASSERT(shell);
        MBrCtlDialogsProvider* brCtlDialogsProvider = shell->BrCtl()->brCtlDialogsProvider();
        
        // Display a confirmation dialog.
        TRAP(err, aDialogOk = brCtlDialogsProvider->DialogConfirmL(
            KEmptyTitle(), *message, *okMsg, *cancelMsg));
        if (err)
            {
            NW_THROW_STATUS(status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure));
            }
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH(status)
        {
        }
    NW_FINALLY
        {
        // Delete the memory we allocated
        delete message;
        delete okMsg;
        delete cancelMsg;

        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// NW_XHTML_FormLiaison::DisplayFileRestrictedConfirmationPrompt
// Displays a confirmation prompt, notifying the user that 1 or more files
// were restricted (closed-content) and giving the user the choice to continue
// or cancel.
// @param aRestrictedFiles Number of files restricted.
// @param aRestrictedFileName Name of the restricted file if there is only one
//      restricted. If more than one restricted, this argument is ignored and
//      can be NULL.
// @param aDialogOk Returned value EFalse if the user canceled, ETrue otherwise.
// @returns Browser status code:
//      KBrsrSuccess if no errors
//      KBrsrOutOfMemory if not enough memory
//      KBrsrFailure for all other failure conditions
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_XHTML_FormLiaison_DisplayFileRestrictedConfirmationPrompt(TInt aRestrictedFiles,
                                                          TDesC& aRestrictedFileName,
                                                          TBool& aDialogOk)
    {
    TInt err = KErrNone;
    HBufC* message = NULL;
    HBufC* okMsg = NULL;
    HBufC* cancelMsg = NULL;
    
    // Parameter assertion block.
    NW_ASSERT(aRestrictedFiles >= 1);
    
    // In the event an error occurs, default is dialog canceled.
    aDialogOk = EFalse;
    
    NW_TRY(status)
        {
        // Choose which prompt to use. If only one file, then its name is
        // displayed. If more than one, then the number of restricted files
        // is displayed.
        if (aRestrictedFiles == 1)
            {
            // Extact name.ext from fully-qualified path.
            TPtrC ptr(aRestrictedFileName);
            TParsePtrC fileNameParser(ptr);
            TPtrC nameAndExt(fileNameParser.NameAndExt());
            
            // Load the resource string, inserting the name&ext, add txt to cleanupstack.
            /*TRAP(err, message =
                 StringLoader::LoadLC(R_QTN_XHTML_QUERY_FILE_RESTRICTED, nameAndExt);
                 CleanupStack::Pop(message););*/
            }
        else
            {
            // Load the resource string, inserting # of restricted files.
            /*TRAP(err, message = 
                 StringLoader::LoadLC(R_QTN_XHTML_QUERY_FILES_RESTRICTED, aRestrictedFiles);
                 CleanupStack::Pop(message););*/
            }
        if (err)
            {
            NW_THROW_STATUS(status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure));
            }
        
        // Set up the OK button text.
        TRAP(err, okMsg = StringLoader::LoadLC(R_WML_OK_BUTTON); CleanupStack::Pop(okMsg););
        if (err)
            {
            NW_THROW_STATUS(status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure));
            }

        // Set up the CANCEL button text.
        TRAP(err, cancelMsg = StringLoader::LoadLC(R_WML_CANCEL_BUTTON); CleanupStack::Pop(cancelMsg););
        if (err)
            {
            NW_THROW_STATUS(status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure));
            }
//R->ul
        CShell* shell = NULL;//REINTERPRET_CAST(CShell*, NW_Ctx_Get(NW_CTX_BROWSER_APP, 0));  
        NW_ASSERT(shell);
        MBrCtlDialogsProvider* brCtlDialogsProvider = shell->BrCtl()->brCtlDialogsProvider();
        
        // Display a confirmation dialog.
        TRAP(err, aDialogOk = brCtlDialogsProvider->DialogConfirmL(
            KEmptyTitle(), *message, *okMsg, *cancelMsg));
        if (err)
            {
            NW_THROW_STATUS(status, ((err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure));
            }
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH(status)
        {
        }
    NW_FINALLY
        {
        // Delete the memory we allocated
        delete message;
        delete okMsg;
        delete cancelMsg;

        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// NW_XHTML_FormLiaison::IsRestrictedFileL
// Checks a file to see if it is restricted (closed-content of any kind).
// @param aFileName Ptr to the name of the file being checked.
// @returns ETrue if file is closed; else, EFalse.
// -----------------------------------------------------------------------------
//
static
TBool
IsRestrictedFileL(const TDesC& aFileName)
    {
	CCommonContentPolicy* contentPolicy = CCommonContentPolicy::NewLC();

    TBool banned(ETrue);
	TRAPD(err, banned = contentPolicy->IsClosedFileL( aFileName ));
    if (err == KErrNoMemory) // ignore other errors
        {
        User::Leave(err);
        }
    
	CleanupStack::PopAndDestroy(); //contentPolicy
	
    return banned;
    }

//STREAMING: Obtain a lock on all files and keep them until transmitted. This
//will be dealt with when streaming is implemented.
// -----------------------------------------------------------------------------
// NW_XHTML_FormLiaison::CheckFiles
// Checks all of the values for any files to upload.  If any are files, two
// checks are made on the files:
// 1. Checks the existance of the files.  If any missing, a confirmation prompt
// will be displayed, giving the user the option to continue or cancel.
// 2. Checks the files for closed-content (files that are restricted from being
// transferred.)
// @param aThisObj Ptr to the OOC class structure for this class.
// @param aNameValuePairs Ptr to array of names and values to check.
// @param aMissingFiles Ptr to destination for missing-file(s) indicator.
// @param aRestrictedFiles Ptr to destination for restricted-file(s) indicator.
// @returns Browser status code:
//      KBrsrSuccess if no errors
//      KBrsrCancelled if the user cancels
//      KBrsrOutOfMemory if not enough memory
//      KBrsrFailure for all other failure conditions
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_XHTML_FormLiaison_CheckFiles(NW_XHTML_FormLiaison_t* aThisObj,
                                NW_NVPair_t* aNameValuePairs,
                                TBool* aMissingFiles,
                                TBool* aRestrictedFiles )
    {
    NW_Ucs2* name;
    NW_Ucs2* value;
    TBool missing;
    TBool restricted;
    TInt missingFiles = 0;
    TInt restrictedFiles = 0;
    NW_Ucs2* firstMissingFileName = NULL;
    NW_Ucs2* firstRestrictedFileName = NULL;
    TInt err = KErrNone;
    RFs rfs;
    TBool rfsConnected = EFalse;
    RFile file;
    TPtrC fileNamePtr;
    
    // Parameter assertion block.
    NW_ASSERT(aNameValuePairs != NULL);
    
    NW_TRY(status)
        {
        // Iterate through all of the values.
        (void)NW_NVPair_ResetIter(aNameValuePairs);
        while ((status = NW_NVPair_GetNext(aNameValuePairs, &name, &value)) == KBrsrSuccess)
            {
            // Check to see if it is a file for uploading.
            if (NW_LoadReq_IsUploadFile(value))
                {
                // If no RFs connection yet, create one.
                if (!rfsConnected)
                    {
                        err = rfs.Connect();
                        if ( err )
                            {
                            NW_THROW_STATUS( status, ((err == KErrNoMemory)
                                ? KBrsrOutOfMemory : err ) );
                            }
                        rfsConnected = ETrue;
                    }

                // Get the file name.
                fileNamePtr.Set( (TUint16*)value, NW_Str_Strlen( value ) );

                // Check to see if an exclusive lock on the file can be
                // obtained. If not, then the file is either missing, locked
                // by another application, or some unexpected error occurred.
                // All cases are treated the same -- as if the file is missing,
                // with the one exception of out-of-memory, which is reported
                // as an error.
                missing = EFalse;
                err = file.Open( rfs, fileNamePtr, EFileRead | EFileShareExclusive );
                if ( err )
                    {
                    if ( err == KErrNoMemory )
                        {
                        NW_THROW_STATUS( status, KBrsrOutOfMemory );
                        }
                    missing = ETrue;
                    // If the file is missing, then reset the field.
                    status = NW_XHTML_FormLiaison_ResetFieldForName(
                        aThisObj, name );
                    NW_THROW_ON_ERROR( status );
                    // Count number of missing files and remember name of first.
                    if ( missingFiles++ == 0 )
                        {
                        firstMissingFileName = value;
                        }
                    }
                file.Close();

                // Check to see if the file is restricted.
                restricted = EFalse;
                if ( !missing )
                    {
                    TRAP( err, restricted = IsRestrictedFileL( fileNamePtr ) );
                    if ( err )
                        {
                        NW_THROW_STATUS( status, ((err == KErrNoMemory)
                            ? KBrsrOutOfMemory : err ) );
                        }
                    if ( restricted )
                        {
                        // If the file is restricted, then reset the field.
                        status = NW_XHTML_FormLiaison_ResetFieldForName(
                            aThisObj, name );
                        NW_THROW_ON_ERROR( status );

                        // Count number of restricted files and remember name of first.
                        if ( restrictedFiles++ == 0 )
                            {
                            firstRestrictedFileName = value;
                            }
                        }
                    }
                }
            }
        status = KBrsrSuccess;

        // If any files missing, prompt the user for confirmation.
        if ( missingFiles )
            {
            TBool dialogOk;
            fileNamePtr.Set((TUint16*)firstMissingFileName,
                            NW_Str_Strlen(firstMissingFileName));
            status = NW_XHTML_FormLiaison_DisplayFileMissingConfirmationPrompt(
                missingFiles, fileNamePtr, dialogOk);
            _NW_THROW_ON_ERROR( status );
            // If the user did not select OK, then cancel.
            if (!dialogOk)
                {
                NW_THROW_STATUS( status, KBrsrCancelled );
                }
            }

        // If any files restricted, prompt the user for confirmation.
        if ( restrictedFiles )
            {
            TBool dialogOk;
            fileNamePtr.Set((TUint16*)firstRestrictedFileName, 
                NW_Str_Strlen(firstRestrictedFileName));
            status = NW_XHTML_FormLiaison_DisplayFileRestrictedConfirmationPrompt(
                restrictedFiles, fileNamePtr, dialogOk);
            _NW_THROW_ON_ERROR( status );
            // If the user did not select OK, then cancel.
            if (!dialogOk)
                {
                NW_THROW_STATUS(status, KBrsrCancelled);
                }
            }
        }
    // status = KBrsrSuccess; // set by NW_CATCH
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        if ( rfsConnected )
            {
            rfs.Close();
            }
        *aMissingFiles = ( missingFiles ) ? ETrue : EFalse;
        *aRestrictedFiles = ( restrictedFiles ) ? ETrue : EFalse;
        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// NW_XHTML_FormLiaison::GetNameValuePairs
// Iterates through the successful controls and builds a set of name-value pairs.
// @param aThisObj Ptr to the OOC class structure for this class.
// @param aSubmitButtonNode Ptr to the submit-button node.
// @param aNameValuePairs Ptr to destination into which the allocated name-value
//  pair set is returned. The caller takes ownership of the memory and is
//  responsible for deallocating it.
// @returns Browser status code:
//      KBrsrSuccess if no errors
//      KBrsrOutOfMemory if not enough memory
//      etc.
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_XHTML_FormLiaison_GetNameValuePairs( NW_XHTML_FormLiaison_t* aThisObj,
    void* aSubmitButtonNode, NW_NVPair_t** aNameValuePairs )
    {
    // Parameter assertion block.
    NW_ASSERT( aThisObj );
    NW_ASSERT( aSubmitButtonNode );
    NW_ASSERT( aNameValuePairs );

    NW_TRY( status )
        {
        *aNameValuePairs = NW_NVPair_New();
        NW_THROW_OOM_ON_NULL( *aNameValuePairs, status );

        // First pass: calculate the size needed for name-value pairs.
        status = NW_XHTML_FormLiaison_IterateSuccessfulControls( aThisObj,
            (NW_DOM_ElementNode_t*)aSubmitButtonNode, NW_NVPair_CalSize,
            *aNameValuePairs );
        NW_THROW_ON_ERROR( status );

        // Allocate memory for name-value pairs.
        status = NW_NVPair_AllocStorage( *aNameValuePairs );
        NW_THROW_ON_ERROR (status);

        // Second pass: copy name-value pairs to the newly allocated memory.
        status = NW_XHTML_FormLiaison_IterateSuccessfulControls( aThisObj,
            (NW_DOM_ElementNode_t*)aSubmitButtonNode, NW_NVPair_Add,
            *aNameValuePairs );
        NW_THROW_ON_ERROR( status );
        }
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        return status;
        }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
// NW_XHTML_FormLiaison::ResetFieldForName
// Resets the field for the form node with the given name.
// @param aThisObj Ptr to the OOC class structure for this class.
// @param aName Ptr to name of the form variable to reset.
// @returns Browser status code:
//      KBrsrSuccess if no errors
//      KBrsrOutOfMemory if not enough memory
//      KBrsrFailure for all other failure conditions
// -----------------------------------------------------------------------------
//
static
TBrowserStatusCode
NW_XHTML_FormLiaison_ResetFieldForName( NW_XHTML_FormLiaison_t* aThisObj,
    NW_Ucs2* aName )
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_ADT_Vector_Metric_t i;
    NW_DOM_ElementNode_t* node;
    NW_Text_t* name = NULL;

    // Walk through nodes of the form.
    for ( i = aThisObj->firstControl;
        i < aThisObj->firstControl + aThisObj->numControls; i++ )
        {
        // Get node.
        node = NW_XHTML_ControlSet_GetControl( aThisObj->controlSet, i );

        // Get name from DOM tree.
        NW_XHTML_GetDOMAttribute( aThisObj->contentHandler, node, 
            NW_XHTML_AttributeToken_name, &name );
        if ( name != NULL )
            {
            NW_Int32 cmp = NW_Str_Strcmp( (NW_Ucs2*)name->storage, aName );
            NW_Object_Delete( name );
            // If this node matches the specified name then reset it.
            if (  cmp == 0 )
                {
                NW_LMgr_Box_t* box;
                box = *(NW_LMgr_Box_t**)NW_ADT_Vector_ElementAt(
                    aThisObj->controlSet->boxVector, i );
                status = NW_FBox_FormBox_Reset( (NW_FBox_FormBox_t*) box );
                (void)NW_LMgr_Box_Refresh( box );
                break;
                }
            }
        }
    return status;
    }


/* ------------------------------------------------------------------------- */
static
NW_XHTML_FormControlType_t
NW_XHTML_FormLiaison_GetInputType(NW_XHTML_FormLiaison_t* thisObj,
                                   NW_DOM_ElementNode_t* node)
{
  /* default to <input type=text> */
  NW_XHTML_FormControlType_t type = NW_XHTML_FormControlType_InputText;
  NW_Text_t* typeValue = NULL;
  const NW_Ucs2* typeValueStorage;
  NW_Bool typeFreeNeeded = NW_FALSE;

  NW_XHTML_GetDOMAttribute (thisObj->contentHandler, node,
                                        NW_XHTML_AttributeToken_type, &typeValue);

  if (typeValue != NULL) {
    /* storage used for compare; force alignment and null-termination */
    typeValueStorage = NW_Text_GetUCS2Buffer (typeValue,
                                          (NW_Text_Flags_NullTerminated |
                                            NW_Text_Flags_Aligned),
                                          NULL,
                                          &typeFreeNeeded);

    if (NW_Str_StricmpConst (typeValueStorage, "checkbox") == 0) {
      type = NW_XHTML_FormControlType_InputCheckbox;
    } else if (NW_Str_StricmpConst (typeValueStorage, "file") == 0) {
      type = NW_XHTML_FormControlType_InputFile;
    } else if (NW_Str_StricmpConst (typeValueStorage, "hidden") == 0) {
      type = NW_XHTML_FormControlType_InputHidden;
    } else if (NW_Str_StricmpConst (typeValueStorage, "password") == 0) {
      type = NW_XHTML_FormControlType_InputPassword;
    } else if (NW_Str_StricmpConst (typeValueStorage, "radio") == 0) {
      type = NW_XHTML_FormControlType_InputRadio;
    } else if (NW_Str_StricmpConst (typeValueStorage, "reset") == 0) {
      type = NW_XHTML_FormControlType_InputReset;
    } else if (NW_Str_StricmpConst (typeValueStorage, "submit") == 0) {
      type = NW_XHTML_FormControlType_InputSubmit;
    } else if (NW_Str_StricmpConst (typeValueStorage, "text") == 0) {
      type = NW_XHTML_FormControlType_InputText;
    } else if (NW_Str_StricmpConst (typeValueStorage, "image") == 0) {
      type = NW_XHTML_FormControlType_InputImage;
    }

    NW_Object_Delete (typeValue);
    if (typeFreeNeeded) {
      NW_Str_Delete ((NW_Ucs2*) typeValueStorage);
      typeValueStorage = NULL;
    }
  }

  return type;
}

/* ------------------------------------------------------------------------- */
static
NW_XHTML_FormControlType_t
NW_XHTML_FormLiaison_GetControlType(NW_XHTML_FormLiaison_t* thisObj,
                                    NW_DOM_ElementNode_t* node)
{
  NW_XHTML_FormControlType_t type = NW_XHTML_FormControlType_None;
  NW_Uint16 token;

  token = NW_HED_DomHelper_GetElementToken (node);

  switch (token) {
  case NW_XHTML_ElementToken_input:
    type = NW_XHTML_FormLiaison_GetInputType(thisObj, node);
    break;
  case NW_XHTML_ElementToken_option:
    type = NW_XHTML_FormControlType_Option;
    break;
  case NW_XHTML_ElementToken_select:
    type = NW_XHTML_FormControlType_Select;
    break;
  case NW_XHTML_ElementToken_textarea:
    type = NW_XHTML_FormControlType_Textarea;
    break;
  default:
    type = NW_XHTML_FormControlType_None;
    break;
  }

  return type;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_FormLiaison_CallControlCallback (const NW_Text_t* controlName,
                                          const NW_Text_t* controlValue,
                                          NW_NVPair_NVFunction_t* controlCallback,
                                          NW_NVPair_t* controlCallbackData)
{
  TBrowserStatusCode status = KBrsrSuccess;
  const NW_Ucs2* name = NULL;
  NW_Bool nameFreeNeeded = NW_FALSE;
  const NW_Ucs2* value = NULL;
  NW_Bool valueFreeNeeded = NW_FALSE;

  /* only process controls that have a name */
  if (controlName != NULL) {
    name = NW_Text_GetUCS2Buffer (controlName,
                                          ( NW_Text_Flags_NullTerminated |
                                            NW_Text_Flags_Aligned ),
                                          NULL,
                              &nameFreeNeeded);
    if (controlValue != NULL) {
      value = NW_Text_GetUCS2Buffer (controlValue,
                                 (NW_Text_Flags_NullTerminated |
                                  NW_Text_Flags_Aligned),
                                 NULL,
                                 &valueFreeNeeded);
    }

    status = controlCallback (controlCallbackData, name, value);

    if (nameFreeNeeded) {
      NW_Mem_Free ((void*)name); /* const cast-away */
    }
    if (valueFreeNeeded) {
      NW_Mem_Free ((void*)value); /* const cast-away */
    }
  }

  return status;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_FormLiaison_IterateSuccessfulControls (NW_XHTML_FormLiaison_t* formLiaison,
                                                NW_DOM_ElementNode_t* submitButtonNode,
                                                NW_NVPair_NVFunction_t* controlCallback,
                                                NW_NVPair_t* controlCallbackData)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_ADT_Vector_Metric_t i;
  NW_DOM_ElementNode_t* node;
  NW_Text_t* value = NULL;
  NW_Text_t* name  = NULL;
  NW_Bool isOn;
  NW_Bool dummy;  /* used by NW_FBox_FormLiaison_GetStringValue to indicate if a value attribute was specified */

  /* walk through the form collecting the submit string */
  for (i = formLiaison->firstControl;
       (status == KBrsrSuccess &&
        i < formLiaison->firstControl + formLiaison->numControls);
       i++) {
    /* get node */
    node = NW_XHTML_ControlSet_GetControl (formLiaison->controlSet, i);

    /* get name and value */
    switch (NW_XHTML_FormLiaison_GetControlType (formLiaison, node)) {
    case NW_XHTML_FormControlType_InputCheckbox:
	case NW_XHTML_FormControlType_InputRadio:
      status = NW_FBox_FormLiaison_GetBoolValue (
                 (NW_FBox_FormLiaison_t*) formLiaison, node, &isOn);

      /* only submit checked checkboxes and radiobuttons */
      if (status == KBrsrSuccess && isOn) {
        /* get the name from the DOM tree */
        NW_XHTML_GetDOMAttribute (formLiaison->contentHandler, node, 
                                         NW_XHTML_AttributeToken_name, &name);
        if (name != NULL) {
          /* 
           * Bug - Fix MK02052137535: HTML 4.01 spec says, value attribute
           * is mandatory for checkbox and radiobutton, in the abscense of
           * value attribute for checkbox or radiobutton, browser post "on"
           * value, to be consistant with the other popular browsers.
           */
          NW_Ucs2 on[] = {'o','n', 0};
          /* get the value from the DOM tree */
          NW_XHTML_GetDOMAttribute (formLiaison->contentHandler, node,
                                            NW_XHTML_AttributeToken_value, &value);
          if(!value)
            {
	        value = NW_Text_New (HTTP_iso_10646_ucs_2, on, NW_Str_Strlen(on), NW_FALSE);
            }

          status = NW_XHTML_FormLiaison_CallControlCallback (
                     name,
                     value,
                     controlCallback,
                     controlCallbackData);

          NW_Object_Delete (value);
          NW_Object_Delete (name);
        }
      }
    break;

    case NW_XHTML_FormControlType_InputImage:
      {
      // do not submit input image other than the one that was pressed
      // pressed button has 'on' string value

      // get the name from the DOM tree 
      NW_XHTML_GetDOMAttribute( formLiaison->contentHandler, node, 
        NW_XHTML_AttributeToken_name, &name );

      if( name ) 
        {
        // check if this button was pressed
        status = NW_FBox_FormLiaison_GetStringValue( (NW_FBox_FormLiaison_t*) formLiaison,
          node, &value, &dummy );
        if( status == KBrsrSuccess && value )
          {
          // 'on' keyword
          NW_Ucs2 onStr[] = { 'o', 'n', 0 };
          NW_Text_t* onText = NW_Text_New( HTTP_iso_10646_ucs_2, onStr, 
            NW_Str_Strlen( onStr ), NW_Text_Flags_Copy );
          
          if( onText && NW_Text_Compare( value, onText ) == 0 ) 
            {
            // simulate pointer event by adding .x and .y to the POST
            // it is required by some servers.
            NW_Text_t* xName = NULL;
            NW_Text_t* yName = NULL;
            NW_Text_t* xPostString = NULL;
            NW_Text_t* yPostString = NULL;
            NW_Text_t* xValue = NULL;
            NW_Text_t* yValue = NULL;
            //
            NW_TRY( status )
              {
              // post should look like this: buttonName.x=1 buttonName.y=1
              NW_Ucs2 x[] = {'.','x', 0};
              NW_Ucs2 y[] = {'.','y', 0};
              NW_Ucs2 xyValue[] = {'1', 0};

              // create .x and.y strings
              xName = NW_Text_New( HTTP_iso_10646_ucs_2, x, NW_Str_Strlen( x ), NW_Text_Flags_Copy );
              NW_THROW_OOM_ON_NULL( xName, status );

              yName = NW_Text_New( HTTP_iso_10646_ucs_2, y, NW_Str_Strlen( y ), NW_Text_Flags_Copy );
              NW_THROW_OOM_ON_NULL( yName, status );

              // add .x and .y to buttonName
              xPostString = NW_Text_Concatenate( name, xName );
              NW_THROW_OOM_ON_NULL( xPostString, status );

              yPostString = NW_Text_Concatenate( name, yName );
              NW_THROW_OOM_ON_NULL( yPostString, status );

              // create values x and y '1'
              xValue = NW_Text_New( HTTP_iso_10646_ucs_2, xyValue, NW_Str_Strlen( xyValue ), NW_Text_Flags_Copy );
              NW_THROW_OOM_ON_NULL( xValue, status );

              yValue = NW_Text_New( HTTP_iso_10646_ucs_2, xyValue, NW_Str_Strlen( xyValue ), NW_Text_Flags_Copy );
              NW_THROW_OOM_ON_NULL( yValue, status );

              // send buttonName.x=1
              status = NW_XHTML_FormLiaison_CallControlCallback( xPostString, xValue, 
                controlCallback, controlCallbackData);
              // send buttonName.y=1
              status = NW_XHTML_FormLiaison_CallControlCallback( yPostString, yValue,
                controlCallback, controlCallbackData);
              } 
            NW_CATCH( status ) 
              {
              } 
            NW_FINALLY 
              {
              NW_Object_Delete( xName );
              NW_Object_Delete( yName );
              NW_Object_Delete( xPostString );
              NW_Object_Delete( yPostString );
              NW_Object_Delete( xValue );
              NW_Object_Delete( yValue );
              } 
            NW_END_TRY
            }
          NW_Object_Delete( onText );
          }
        NW_Object_Delete( name );
        }
      break;
      }
    case NW_XHTML_FormControlType_InputSubmit:
      if (node != submitButtonNode) {
        /* do not submit buttons other than the one that was pressed */
        break;
      }
      /* FALL THROUGH */
      /*lint -fallthrough*/

      // FALL THROUGH

    case NW_XHTML_FormControlType_InputFile:     /* FALL THROUGH */
    case NW_XHTML_FormControlType_InputHidden:   /* FALL THROUGH */
    case NW_XHTML_FormControlType_InputPassword: /* FALL THROUGH */
    case NW_XHTML_FormControlType_InputReset:    /* FALL THROUGH */
    case NW_XHTML_FormControlType_InputText:     /* FALL THROUGH */
    case NW_XHTML_FormControlType_Textarea:
      /* get the name from the DOM tree */
      NW_XHTML_GetDOMAttribute (formLiaison->contentHandler, node, 
                                       NW_XHTML_AttributeToken_name, &name);

      if (name != NULL) {
        /* use the current value in the control set */
        status = NW_FBox_FormLiaison_GetStringValue (
                   (NW_FBox_FormLiaison_t*) formLiaison,
                   node, 
                   &value,
                   &dummy);
        if (status == KBrsrSuccess) {
          status = NW_XHTML_FormLiaison_CallControlCallback (
                     name,
                     value,
                     controlCallback,
                     controlCallbackData);
          /* must not delete the value as it is owned by the control set */
        }
        NW_Object_Delete (name);
      }
    break;

    case NW_XHTML_FormControlType_Option:
      status = NW_FBox_FormLiaison_GetBoolValue ((NW_FBox_FormLiaison_t*) formLiaison,
                                                node,
                                                &isOn);

      /* only submit selected options */
      if (status == KBrsrSuccess && isOn) {
        NW_DOM_ElementNode_t* selectNode;

        /* find the enclosing <select> element */
        selectNode = node;
        while ((selectNode = NW_DOM_Node_getParentNode (selectNode)) != NULL) {
          if (NW_XHTML_FormLiaison_GetControlType (formLiaison, selectNode) ==
              NW_XHTML_FormControlType_Select) {
            /* found the <select> node */
    break;
          }
        }

        /* if we found the <select> node, get its name from the DOM tree */
        if (selectNode != NULL) {
            NW_XHTML_GetDOMAttribute (formLiaison->contentHandler, selectNode,
                                      NW_XHTML_AttributeToken_name, &name);
        }

        if (name != NULL) {
          /* get the value initial string value */
          status = NW_FBox_FormLiaison_GetStringValue (
                     (NW_FBox_FormLiaison_t*) formLiaison,
                     node,
                     &value,
                     &dummy);

          if (status == KBrsrSuccess) {
            status = NW_XHTML_FormLiaison_CallControlCallback (
                       name,
                       value,
                       controlCallback,
                       controlCallbackData);
            /* must delete value as it is *not* owned by the control set */
            NW_Object_Delete (value);
          }
          NW_Object_Delete (name);
        }
      }
      break;

    case NW_XHTML_FormControlType_Select:
      /*
       * TODO: an optimization would be to save the name of the <select>
       * element to the form liaison so that every <option> inside the
       * <select> element could pick it up from there. However, doing
       * so would necessarily spread out the code somewhat, so for now
       * let's just go through the process of finding the name of the
       * <select> element in the NW_XHTML_FormControlType_Option case.
       */
      /* FALL THROUGH */
  default:
      /* do nothing */
    break;
  }
  }

  return status;
}

/* ------------------------------------------------------------------------- */
static
NW_Object_t*
NW_XHTML_FormLiaison_GetBoolAttribute(NW_XHTML_FormLiaison_t* thisObj,
                                      NW_DOM_ElementNode_t* node,
                                      const NW_Uint16 nameToken,
                                      const NW_Ucs2* nameTag)
{
  NW_Object_t* value;
  NW_Ucs2* nameAttr = NULL;
  TBrowserStatusCode status;

  status = NW_XHTML_ElementHandler_GetAttribute(thisObj->contentHandler, node, 
                                                nameToken, &nameAttr);

  if (status == KBrsrSuccess) {
    if ( NW_Str_Stricmp( nameAttr, nameTag ) == 0 ) {
      /* only attribute="attribute" maps to NW_TRUE */

      /*  See task / bug references at implementation.
      if(IsPrevSiblingNodeChecked((*thisObj).contentHandler, node))
        value = (NW_Object_t*) &NW_XHTML_FormLiaisonFalse;
      else 
      */
        value = (NW_Object_t*) &NW_XHTML_FormLiaisonTrue;
    } else {
      /* everything else maps to NW_FALSE */
      value = (NW_Object_t*) &NW_XHTML_FormLiaisonFalse;
    }

    NW_Str_Delete((NW_Ucs2*) nameAttr);
    nameAttr = NULL;
  } else {
    /* map attribute not found to NW_FALSE */
    value = (NW_Object_t*) &NW_XHTML_FormLiaisonFalse;
  }

  return value;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_FormLiaison_SetCheckboxValue(NW_XHTML_FormLiaison_t* thisObj,
                                      NW_DOM_ElementNode_t* node,
                                      NW_Bool value)
{
  NW_Object_t* valueObj;

  valueObj = (NW_Object_t*)
    (value ? &NW_XHTML_FormLiaisonTrue : &NW_XHTML_FormLiaisonFalse);

  return NW_XHTML_ControlSet_SetValue(thisObj->controlSet, node, valueObj);
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_FormLiaison_SetOptionValue(NW_XHTML_FormLiaison_t* thisObj,
                                    NW_DOM_ElementNode_t* node,
                                    NW_Bool value)
{
  static const NW_Ucs2 multipleTag[] = {'m','u','l','t','i','p','l','e','\0'};
  NW_DOM_ElementNode_t* selectNode;

  /* first find the enclosing <select> element */
  selectNode = node;
  while ((selectNode = NW_DOM_Node_getParentNode(selectNode)) != NULL) {
    if (NW_XHTML_FormLiaison_GetControlType(thisObj, selectNode) ==
        NW_XHTML_FormControlType_Select) {
      /* found the <select> node */
      break;
    }
  }

  /* if we found the <select> node, see if it has a multiple attribute */
  if (selectNode != NULL) {
    if (NW_XHTML_FormLiaison_GetBoolAttribute(thisObj,
                                              selectNode,
                                              NW_XHTML_AttributeToken_multiple,
                                              multipleTag) ==
        (NW_Object_t*) &NW_XHTML_FormLiaisonTrue) {
      NW_Object_t* valueObj;

      /* found multiple="multiple"; just set the value of this option */
      valueObj = (NW_Object_t*)
        (value ? &NW_XHTML_FormLiaisonTrue : &NW_XHTML_FormLiaisonFalse);

      return NW_XHTML_ControlSet_SetValue(thisObj->controlSet, node, valueObj);
    } else {

      NW_cXML_DOM_DOMVisitor_t* domVisitor;
      NW_DOM_Node_t* domNode;

      /* no multiple attribute; select this option and deselect others */

	  if(!value) return KBrsrFailure;

      /* first select this option */
      if (NW_XHTML_ControlSet_SetValue(thisObj->controlSet,
                                       node,
                                       (NW_Object_t*) &NW_XHTML_FormLiaisonTrue) !=
          KBrsrSuccess) {
        return KBrsrFailure;
      }

      /* then deselect others */
      domVisitor = NW_cXML_DOM_DOMVisitor_New(selectNode);
      if (domVisitor != NULL) {
        while ((domNode = NW_cXML_DOM_DOMVisitor_Next(domVisitor,
                                                      NW_DOM_ELEMENT_NODE)) !=
               NULL) {
          if (domNode != node &&
              NW_XHTML_FormLiaison_GetControlType(thisObj, domNode) ==
              NW_XHTML_FormControlType_Option) {
            /* found another <option> node; deselect it */
            if (NW_XHTML_ControlSet_SetValue(thisObj->controlSet,
                                             domNode,
                                             (NW_Object_t*) &NW_XHTML_FormLiaisonFalse) !=
                KBrsrSuccess) {
              NW_Object_Delete (domVisitor);
              return KBrsrFailure;
            }
          }
        }
        
        NW_Object_Delete (domVisitor);
      }
    }
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_ResetRadioValue (NW_FBox_FormLiaison_t* formLiaison,
                                       void* node,
                                       NW_Bool *value)
  {
  NW_XHTML_FormLiaison_t* thisObj = NW_XHTML_FormLiaisonOf (formLiaison);
  NW_Object_t* valueObj = (NW_Object_t*) (*value ? &NW_XHTML_FormLiaisonTrue : &NW_XHTML_FormLiaisonFalse);

  NW_XHTML_ControlSet_SetValue( thisObj->controlSet, (NW_DOM_ElementNode_t*) node, valueObj ); 

  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_FormLiaison_SetRadioValue(NW_XHTML_FormLiaison_t* thisObj,
                                   NW_DOM_ElementNode_t* node,
                                   NW_Bool value)
{
  NW_Text_t* aNameValue = NULL;
  NW_Text_t* bNameValue = NULL;
  const NW_Ucs2* aNameValueStorage;
  const NW_Ucs2* bNameValueStorage;
  NW_Text_Length_t aLength;
  NW_Text_Length_t bLength;
  NW_DOM_ElementNode_t* bNode;
  NW_ADT_Vector_Metric_t i;

  /* find the control name of the radio button */
  NW_XHTML_GetDOMAttribute (thisObj->contentHandler, node,
                                                       NW_XHTML_AttributeToken_name, &aNameValue);
  if (aNameValue == 0) {
    return KBrsrSuccess;
  }

  /* radiobutton can only be switched on */
  if (value == NW_FALSE) {
    NW_Object_Delete(aNameValue);
    return KBrsrFailure;
  }

  /* set the radiobutton itself on */
  if (NW_XHTML_ControlSet_SetValue(thisObj->controlSet,
                                   node,
                                   (NW_Object_t*) &NW_XHTML_FormLiaisonTrue) !=
      KBrsrSuccess) {
    NW_Object_Delete(aNameValue);
    return KBrsrFailure;
  }

  /* if the radiobutton has a name, look for others with the same name */
  /* storage only used for memcmp; no need to force alignment */
  aNameValueStorage = NW_Text_GetUCS2Buffer(aNameValue,
                                         0,
                                         &aLength,
                                         NULL);

  /* iterate through all controls of the form */
  for (i = thisObj->firstControl;
       i < thisObj->firstControl + thisObj->numControls;
       i++) {
    bNode = NW_XHTML_ControlSet_GetControl (thisObj->controlSet, i);

    if (NW_XHTML_FormLiaison_GetControlType(thisObj, bNode) ==
        NW_XHTML_FormControlType_InputRadio &&
        bNode != node) {
      /* set other radiobuttons with the same name off */

      NW_XHTML_GetDOMAttribute (thisObj->contentHandler, bNode,
                                                           NW_XHTML_AttributeToken_name, &bNameValue);

      if (bNameValue != NULL) {
        /* storage only used for memcmp; no need to force alignment */
        bNameValueStorage = NW_Text_GetUCS2Buffer(bNameValue, 0,
                                               &bLength, NULL);

        if (aLength == bLength && NW_Mem_memcmp(aNameValueStorage,
                                                bNameValueStorage,
                                                aLength*sizeof(NW_Ucs2)) == 0) {
          /* found another radiobutton of the same name; force it off */
          if (NW_XHTML_ControlSet_SetValue(thisObj->controlSet,
                                           bNode,
                                           (NW_Object_t*) &NW_XHTML_FormLiaisonFalse) !=
              KBrsrSuccess) {
            NW_Object_Delete(bNameValue);
            NW_Object_Delete(aNameValue);
            return KBrsrFailure;
          }
        }

        NW_Object_Delete(bNameValue);
      }
    }
  }

  NW_Object_Delete(aNameValue);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static
NW_Object_t*
NW_XHTML_FormLiaison_GetRadioValue(NW_XHTML_FormLiaison_t* thisObj,
                                   NW_DOM_ElementNode_t* node)
{
  NW_Text_t* aNameValue;
  NW_Text_t* bNameValue;
  const NW_Ucs2* aNameValueStorage;
  const NW_Ucs2* bNameValueStorage;
  NW_Text_Length_t aLength;
  NW_Text_Length_t bLength;
  NW_DOM_ElementNode_t* bNode;
  NW_ADT_Vector_Metric_t i;
  NW_Object_t* value;
  NW_Object_t* checked;
  static const NW_Ucs2 checkedTag[]  = {'c','h','e','c','k','e','d','\0'};

  /* find the control name of the radio button */
  NW_XHTML_GetDOMAttribute (thisObj->contentHandler, node,
                            NW_XHTML_AttributeToken_name, &aNameValue);

  /* if the radiobutton has a name, look for others with the same name */
  if (aNameValue != NULL) {
    /* storage only used for memcmp; no need to force alignment */
    aNameValueStorage = NW_Text_GetUCS2Buffer(aNameValue,
                                           0,
                                           &aLength,
                                           NULL);

    /* iterate through all controls of the form */
    for (i = thisObj->firstControl;
         i < thisObj->firstControl + thisObj->numControls;
         i++) {
      bNode = NW_XHTML_ControlSet_GetControl (thisObj->controlSet, i);

      if (NW_XHTML_FormLiaison_GetControlType(thisObj, bNode) ==
          NW_XHTML_FormControlType_InputRadio) {

        NW_XHTML_GetDOMAttribute (thisObj->contentHandler, bNode,
                                  NW_XHTML_AttributeToken_name,
                                  &bNameValue);

        if (bNameValue != NULL) {
          /* storage only used for memcmp; no need to force alignment */
          bNameValueStorage = NW_Text_GetUCS2Buffer(bNameValue, 0,
                                                 &bLength, NULL);

          if (aLength == bLength && NW_Mem_memcmp(aNameValueStorage,
                                                  bNameValueStorage,
                                                  aLength*sizeof(NW_Ucs2)) == 0) {
            /* Found another radiobutton of the same name. 
             */
            checked = NW_XHTML_FormLiaison_GetBoolAttribute (thisObj,
                                                  bNode,
                                                  NW_XHTML_AttributeToken_checked,
                                                  checkedTag);

            /* If multiple radiobuttons with the same name are checked,
             * ignore all but the first.
             */
            if (checked == (NW_Object_t*) &NW_XHTML_FormLiaisonTrue) {
              if (bNode == node) {
                value = (NW_Object_t*) &NW_XHTML_FormLiaisonTrue;
              } else {
                value = (NW_Object_t*) &NW_XHTML_FormLiaisonFalse;
              }
              NW_Object_Delete(bNameValue);
              NW_Object_Delete(aNameValue);
              return value;
            }
          }         
          NW_Object_Delete(bNameValue);
        }
      }
    } 
    NW_Object_Delete(aNameValue);

    return (NW_Object_t*) &NW_XHTML_FormLiaisonFalse;
  } 

  /* RadioButton didn't have a name. */
  return NW_XHTML_FormLiaison_GetBoolAttribute (thisObj,
    node,
    NW_XHTML_AttributeToken_checked,
    checkedTag);
}

/* ------------------------------------------------------------------------- */
static
NW_Object_t*
NW_XHTML_FormLiaison_GetChildText (NW_XHTML_ContentHandler_t* contentHandler,
                                   NW_DOM_ElementNode_t* elementNode)
{
  TBrowserStatusCode status;
  NW_String_t childText;
  NW_Text_t* text = NULL;

  status = NW_HED_DomHelper_GetChildText (&contentHandler->domHelper, 
                                          elementNode, 
                                          &childText);
  if (status == KBrsrSuccess) {
    text = (NW_Text_t*) NW_Text_UCS2_New (childText.storage,
                                          NW_String_getCharCount (&childText, HTTP_iso_10646_ucs_2),
                                          NW_Text_Flags_TakeOwnership);

  }

  /* Ideally this method should return TBrowserStatusCode but text = NULL reflects OOM - Vishy 7/8/2002 */
  return (NW_Object_t*) text;
}

/* ------------------------------------------------------------------------- */
NW_Object_t*
NW_XHTML_FormLiaison_GetInitialValue (NW_XHTML_FormLiaison_t* thisObj,
                                      NW_DOM_ElementNode_t* node)
{
  NW_Object_t* value = NULL;
  static const NW_Ucs2 checkedTag[]  = {'c','h','e','c','k','e','d','\0'};
  static const NW_Ucs2 selectedTag[] = {'s','e','l','e','c','t','e','d','\0'};

  switch (NW_XHTML_FormLiaison_GetControlType (thisObj, node)) {
  case NW_XHTML_FormControlType_InputCheckbox: 
    value = NW_XHTML_FormLiaison_GetBoolAttribute (thisObj,
                                                  node,
                                                  NW_XHTML_AttributeToken_checked,
                                                  checkedTag);
    break; 

  case NW_XHTML_FormControlType_InputRadio:
    value = NW_XHTML_FormLiaison_GetRadioValue (thisObj, node);
    break;

  case NW_XHTML_FormControlType_Option:
    value = NW_XHTML_FormLiaison_GetBoolAttribute (thisObj,
                                                  node,
                                                  NW_XHTML_AttributeToken_selected,
                                                  selectedTag);
    break;

  case NW_XHTML_FormControlType_InputHidden:   /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputPassword: /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputReset:    /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputSubmit:   /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputText:     /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputImage:    // FALL THROUGH
    NW_XHTML_GetDOMAttribute (thisObj->contentHandler, node,
                                      NW_XHTML_AttributeToken_value, (NW_Text_t**)&value);
    break;

  case NW_XHTML_FormControlType_Textarea:
    /* get the child text node */
    value = NW_XHTML_FormLiaison_GetChildText (thisObj->contentHandler, node);
    break;

  case NW_XHTML_FormControlType_InputFile:  /* FALL THROUGH */
  case NW_XHTML_FormControlType_Select:     /* FALL THROUGH */
  default:
    value = NULL;
    break;
  }

  return value;
}


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_FormLiaison_Class_t NW_XHTML_FormLiaison_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_FBox_FormLiaison_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_XHTML_FormLiaison_t),
    /* construct               */ _NW_XHTML_FormLiaison_Construct,
    /* destruct                */ _NW_XHTML_FormLiaison_Destruct
  },
  { /* NW_FBox_FormLiaison     */
    /* getWBXMLVersion         */ _NW_XHTML_FormLiaison_GetWBXMLVersion,
    /* addControl              */ _NW_XHTML_FormLiaison_AddControl,
    /* setInitialStringValue   */ _NW_XHTML_FormLiaison_SetInitialStringValue,
    /* setStringValue          */ _NW_XHTML_FormLiaison_SetStringValue,
    /* validateStringValue     */ _NW_XHTML_FormLiaison_ValidateStringValue,
    /* getStringValue          */ _NW_XHTML_FormLiaison_GetStringValue,
    /* getStringName           */ _NW_XHTML_FormLiaison_GetStringName,
    /* getStringTitle          */ _NW_XHTML_FormLiaison_GetStringTitle,
    /* setBoolValue            */ _NW_XHTML_FormLiaison_SetBoolValue,
    /* resetRadioValue         */ _NW_XHTML_FormLiaison_ResetRadioValue,
    /* toggleBoolValue         */ _NW_XHTML_FormLiaison_ToggleBoolValue,
    /* getBoolValue            */ _NW_XHTML_FormLiaison_GetBoolValue,
    /* reset                   */ _NW_XHTML_FormLiaison_Reset,
    /* submit                  */ _NW_XHTML_FormLiaison_Submit,
    /* focus                   */ _NW_XHTML_FormLiaison_Focus,
    /* isOptionMultiple        */ _NW_XHTML_FormLiaison_IsOptionMultiple,
    /* isOptionSelected        */ _NW_XHTML_FormLiaison_IsOptionSelected,
    /* optionHasOnPick         */ _NW_XHTML_FormLiaison_OptionHasOnPick,
    /* isLocalNavOnPick        */ _NW_XHTML_FormLiaison_IsLocalNavOnPick,
    /* getInitialValue         */ _NW_XHTML_FormLiaison_GetInitialValue,
    /* getDocRoot              */ _NW_XHTML_FormLiaison_GetDocRoot,
    /* delegateEcmaEvent       */ _NW_XHTML_FormLiaison_DelegateEcmaEvent
  },
  { /* NW_XHTML_FormLiaison    */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
NW_Bool
_NW_XHTML_FormLiaison_GetInitialValue (NW_FBox_FormLiaison_t* formLiaison,
									   void* controlId)
{
  static const NW_Ucs2 checkedTag[]  = {'c','h','e','c','k','e','d','\0'};
  static const NW_Ucs2 selectedTag[] = {'s','e','l','e','c','t','e','d','\0'};

  NW_Bool retvalue = NW_FALSE;
  NW_Object_t* value = NULL;
  NW_DOM_ElementNode_t* node = (NW_DOM_ElementNode_t*) controlId;
  NW_XHTML_FormLiaison_t* thisObj;

  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  switch (NW_XHTML_FormLiaison_GetControlType (thisObj, node)) {
  case NW_XHTML_FormControlType_InputCheckbox: /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputRadio:
    value = NW_XHTML_FormLiaison_GetBoolAttribute (thisObj,
                                                  node,
                                                  NW_XHTML_AttributeToken_checked,
                                                  checkedTag);
    break;

  case NW_XHTML_FormControlType_Option:
    value = NW_XHTML_FormLiaison_GetBoolAttribute (thisObj,
                                                  node,
                                                  NW_XHTML_AttributeToken_selected,
                                                  selectedTag);
    break;

  case NW_XHTML_FormControlType_InputHidden:   /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputPassword: /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputText:     /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputImage:     // FALL THROUGH
  case NW_XHTML_FormControlType_InputSubmit:
    NW_XHTML_GetDOMAttribute (thisObj->contentHandler, node,
                                      NW_XHTML_AttributeToken_value, (NW_Text_t**)&value);
    break;

  case NW_XHTML_FormControlType_Textarea:
    /* get the child text node */
    value = NW_XHTML_FormLiaison_GetChildText (thisObj->contentHandler, node);
    break;


  case NW_XHTML_FormControlType_Select:     /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputFile:  /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputReset: /* FALL THROUGH */
  default:
    value = NULL;
    break;
  }

  if (value == (NW_Object_t*) &NW_XHTML_FormLiaisonTrue) 
	{
		retvalue = NW_TRUE;
	}
  return retvalue;
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp)
{
  NW_XHTML_FormLiaison_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_XHTML_FormLiaison_Class));
  NW_ASSERT (argp != NULL);

  /* for convenience */
  thisObj = NW_XHTML_FormLiaisonOf (dynamicObject);

  /* initialize our member variables */
  thisObj->formNode = va_arg (*argp, NW_DOM_ElementNode_t*);
  thisObj->contentHandler = va_arg (*argp, NW_XHTML_ContentHandler_t*);

  thisObj->controlSet = va_arg (*argp, NW_XHTML_ControlSet_t*);

  (void)NW_XHTML_GetDOMAttribute (thisObj->contentHandler, thisObj->formNode,
                              NW_XHTML_AttributeToken_name, &thisObj->formName );

  (void)NW_XHTML_GetDOMAttribute (thisObj->contentHandler, thisObj->formNode,
                              NW_XHTML_AttributeToken_action, &thisObj->formAction );

  (void)NW_XHTML_GetDOMAttribute (thisObj->contentHandler, thisObj->formNode,
                            NW_XHTML_AttributeToken_method, &thisObj->formMethod );

  (void)NW_XHTML_GetDOMAttribute (thisObj->contentHandler, thisObj->formNode,
                            NW_XHTML_AttributeToken_enctype, &thisObj->formEnctype );

  (void)NW_XHTML_GetDOMAttribute (thisObj->contentHandler, thisObj->formNode,
                            NW_XHTML_AttributeToken_id, &thisObj->formId );

  if(!thisObj->formAction)
    {
    thisObj->formAction = (NW_Text_t*)NW_Text_UCS2_New((void * )"",0,  NW_Text_Flags_Aligned | NW_Text_Flags_Copy);
    if (!thisObj->formAction)
      {
      status = KBrsrOutOfMemory;
      }
    }

  if(!thisObj->formMethod)
    {
    thisObj->formMethod = (NW_Text_t*)NW_Text_UCS2_New((void * )"",0,  NW_Text_Flags_Aligned | NW_Text_Flags_Copy);
    if (!thisObj->formMethod)
      {
      status = KBrsrOutOfMemory;
      }
    }
  /*
   * initialize numControls and firstControl for now; they will get their
   * proper values when controls get added to the form liaison in _AddControl
   */
  thisObj->numControls = 0;
  thisObj->firstControl = NW_ADT_Vector_AtEnd;

  /* successful completion */
  return status;
}

/* ------------------------------------------------------------------------- */
void
_NW_XHTML_FormLiaison_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_XHTML_FormLiaison_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_XHTML_FormLiaison_Class));

  /* for convenience */
  thisObj = NW_XHTML_FormLiaisonOf (dynamicObject);

  NW_Object_Delete (thisObj->formName);
  NW_Object_Delete (thisObj->formAction);
  NW_Object_Delete (thisObj->formMethod);
  NW_Object_Delete (thisObj->formEnctype);
  NW_Object_Delete (thisObj->formId);

  /* delete the validatorMap and its contents (if it exists) */
  if (thisObj->validatorMap != NULL) {
    NW_ADT_Iterator_t* iterator;
    void** entry;

    iterator = (NW_ADT_Iterator_t*)
      NW_ADT_MapIterator_New (thisObj->validatorMap);
    while ((entry = NW_ADT_Iterator_NextElement(iterator)) 
           != NULL) {
      NW_FBox_Validator_t* validator;

      TBrowserStatusCode status = NW_ADT_Map_Get( thisObj->validatorMap, entry, &validator );
      NW_ASSERT (status == KBrsrSuccess);
      NW_Object_Delete (validator);
    }
    NW_Object_Delete (iterator);
    NW_Object_Delete (thisObj->validatorMap);
  }
}

/*****************************************************************

  Name:         _NW_XHTML_FormLiaison_GetWBXMLVersion()

  Description:  The WBXML version of the XHTML document is retrieved and translated
                to an enum as defined by TWapDocType in "rb_tempest\fbox\include\
                CoreWrapper.h"

  Parameters:   aWapDocType is an "out" parameter, representative of spec level of
                conformance with W3C XHTML the gateway encoder used to
                encode the content from the origin server.

  Algorithm:    Using NW_FBox_FormLiaison_t, look up the version field
                maintained under tinyparser/domtree/doc/version.

  Return Value: KBrsrSuccess, if pointers valid and a value exists
                otherwise, return KBrsrFailure

*****************************************************************/
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetWBXMLVersion(NW_FBox_FormLiaison_t*  thisObj,
                                      NW_Uint32*               wmlVersion)
{
  TBrowserStatusCode             status = KBrsrSuccess;
  NW_REQUIRED_PARAM(thisObj);
  *wmlVersion = NW_xhtml_1_0_PublicId; 
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_AddControl (NW_FBox_FormLiaison_t* formLiaison,
                                 void* node)
{
  NW_XHTML_FormLiaison_t* thisObj;
  TBrowserStatusCode status;
  NW_ADT_Vector_Metric_t index;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (formLiaison, &NW_XHTML_FormLiaison_Class));
  NW_ASSERT (node != NULL);

  /* for convenience */
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  status = NW_XHTML_ControlSet_AddNode (thisObj->controlSet, 
      (NW_DOM_ElementNode_t*) node, &index);
  if (status != KBrsrSuccess) {
    return status;
    }

  /* update the indices */
  /* TODO: fix this for embedded forms (LT) */
  if (thisObj->numControls == 0) {
    /* we are dealing with the first control of the form; save its index */
    thisObj->firstControl = index;
  }
  thisObj->numControls++;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_SetInitialStringValue (NW_FBox_FormLiaison_t* formLiaison,
                                             void* node,
											 NW_Text_Length_t maxChars)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_XHTML_FormLiaison_t* thisObj;
  NW_Object_t* valueObj;
  NW_Text_t* valueText;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (formLiaison, &NW_XHTML_FormLiaison_Class));
  NW_ASSERT (node != NULL);

  /* for convenience */
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  valueText = (NW_Text_t*) NW_XHTML_ControlSet_GetValue(thisObj->controlSet,
                                           (NW_DOM_ElementNode_t*) node);

  if ( valueText != NULL ) {
    /* The control already has a value.  Don't change it. */
    return KBrsrSuccess;
  }

  valueObj = NW_XHTML_FormLiaison_GetInitialValue ((NW_XHTML_FormLiaison_t*) thisObj, 
      (NW_DOM_ElementNode_t*) node);
	/* Make sure initial string is less then or equal to maximum characters that are allowed */
  if ((valueObj != NULL) && (NW_TextOf(valueObj)->characterCount <= maxChars)) {
    NW_ASSERT (NW_Object_IsInstanceOf (valueObj, &NW_Text_Abstract_Class));
    status = NW_FBox_FormLiaison_SetStringValue((NW_FBox_FormLiaison_t*) thisObj,
                                                node, 
                                                NW_TextOf(valueObj));
    if (status != KBrsrSuccess) {
      /* Failed so did not take ownership of valueObj.  Free it up. */
      NW_Object_Delete( valueObj );
    }
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_SetStringValue (NW_FBox_FormLiaison_t* formLiaison,
                                     void* node,
                                     NW_Text_t* value)
{
  TBrowserStatusCode status;
  NW_XHTML_FormLiaison_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (formLiaison, &NW_XHTML_FormLiaison_Class));
  NW_ASSERT (node != NULL);

  /* for convenience */
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  switch (NW_XHTML_FormLiaison_GetControlType (thisObj, (NW_DOM_ElementNode_t*) node)) {
  case NW_XHTML_FormControlType_InputPassword: /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputText:     /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputImage:    // FALL THROUGH
  case NW_XHTML_FormControlType_Textarea:      /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputHidden:
      status = NW_FBox_FormLiaison_ValidateStringValue (formLiaison, node, value);
      if (status != KBrsrSuccess) {
        return status;
      }
      status = NW_XHTML_ControlSet_SetValue (thisObj->controlSet,
                                          (NW_DOM_ElementNode_t*)node,
                                          value);
    break;

  // No validation for file input.  Users cannot enter file name directly.
  case NW_XHTML_FormControlType_InputFile:
      status = NW_XHTML_ControlSet_SetValue (thisObj->controlSet,
                                          (NW_DOM_ElementNode_t*)node,
                                          value);
    break;


  default:
    /*
     * cannot set value; the input element is of an unknown type,
     *  reset, submit or one of the elements with boolean value
     */
    status = KBrsrFailure;
    break;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_ValidateStringValue (NW_FBox_FormLiaison_t* formLiaison,
                                           void* node,
                                           const NW_Text_t* value)
{
  TBrowserStatusCode status;
  NW_XHTML_FormLiaison_t* thisObj;

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (formLiaison, &NW_XHTML_FormLiaison_Class));
  NW_ASSERT (node != NULL);

  /* for convenience */
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  /* do nothing if there are no validators at all */
  status = KBrsrSuccess;
  if (thisObj->validatorMap != NULL) {
    NW_FBox_Validator_t* validator;

    /* get the validator */
    status = NW_ADT_Map_Get (thisObj->validatorMap, &node, &validator);
    if (status == KBrsrSuccess && validator != NULL) {
      /* the validator makes the ultimate decision about the validity of
         the string */
      return NW_FBox_Validator_Validate (validator, value);
    }
  }
  if(status == KBrsrNotFound)
    {
    status = KBrsrSuccess ;
    }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetStringValue (NW_FBox_FormLiaison_t* formLiaison,
                                     void* node,
                                     NW_Text_t** valueOut,
                                     NW_Bool*   initialValueUsed)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Text_t* value = NULL;
  NW_XHTML_FormLiaison_t* thisObj;

  NW_REQUIRED_PARAM(initialValueUsed);

  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  /* if a set value exists, return it; otherwise return reset value */

  switch (NW_XHTML_FormLiaison_GetControlType (thisObj, (NW_DOM_ElementNode_t*) node)) {
  case NW_XHTML_FormControlType_InputFile:     /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputPassword: /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputText:     /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputImage:    // FALL THROUGH
  case NW_XHTML_FormControlType_Textarea:      /* FALL THROUGH */
    value = (NW_Text_t*) NW_XHTML_ControlSet_GetValue (thisObj->controlSet, 
        (NW_DOM_ElementNode_t*) node);
  
    /* Only inputText, inputPassword, and textarea have input format properties.
       Move the decision to set to initial value in element handler, so can
       check for a valid input values. */
    break;

  case NW_XHTML_FormControlType_InputHidden:   /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputReset:    /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputSubmit:   /* FALL THROUGH */
    value = (NW_Text_t*) NW_XHTML_ControlSet_GetValue (thisObj->controlSet, 
        (NW_DOM_ElementNode_t*) node);

    /* Put decision to set to initial value in element handler, so can
       check for a valid value. */
    /* if value has not been set yet, use the initial (reset) value */
    if (value == NULL) {
        value = (NW_Text_t*) NW_XHTML_FormLiaison_GetInitialValue (thisObj, 
            (NW_DOM_ElementNode_t*) node);

      /* set value so that it will be released by the control set */
        status = NW_XHTML_ControlSet_SetValue (thisObj->controlSet,
            (NW_DOM_ElementNode_t*) node, value);
    }
    break;

  case NW_XHTML_FormControlType_Option:
    /*
     * <option> is a special case since it has both the editable boolean
     * value (selected="selected") and the initial string value (either
     * value attribute or enclosed text).
     * This function deals with the initial string value only.
     * The initial boolean value can be obtained with _GetInitialValue().
     * The main difference compared to other elements handled by this
     * function is that the obtained value is *not* saved to the control set
     * because the control set deals with the <option> boolean value only.
     * The caller must thus deallocate the value.
     *
     * NOTE: The order of precedence for rendering (in Select element handler) 
     *       is different from the code here for sending to server.  Also, we 
     *       support attributes from both WML and Xhtml/Html.
     */

     /* first try to get the value from the value attribute */
     NW_XHTML_GetDOMAttribute (thisObj->contentHandler, 
          (NW_DOM_ElementNode_t*) node, NW_XHTML_AttributeToken_value, &value);

      if (value == NULL) {
          /* no value attribute; try the enclosed text */
          value = (NW_Text_t*) NW_XHTML_FormLiaison_GetChildText (thisObj->contentHandler, 
                (NW_DOM_ElementNode_t*) node);
          
          /*NW_Ucs2* temp = NW_Text_GetUCS2Buffer (value, NW_Text_Flags_NullTerminated |
          NW_Text_Flags_Aligned, NULL, NULL);*/

          NW_Bool freeNeeded;
          NW_Ucs2* tempBuf = NW_Text_GetUCS2Buffer (value, NW_Text_Flags_NullTerminated |
          NW_Text_Flags_Aligned, NULL, &freeNeeded);
          if(tempBuf == NULL)
          {
            NW_Object_Delete (value); 
            value = NULL;
            status = KBrsrOutOfMemory;
            break;
          }
          NW_Ucs2* tempStr = NW_Str_Trim(tempBuf, NW_Str_End);
          if(freeNeeded)
            {
            NW_Mem_Free((NW_Ucs2*)tempBuf);
            } 
          if(tempStr == NULL)
          {
            NW_Object_Delete (value); 
            value = NULL;
            status = KBrsrOutOfMemory;
            break;
          }

          status = NW_Text_SetStorage (value, tempStr, NW_Str_Strlen(tempStr), NW_Text_Flags_NullTerminated |
          NW_Text_Flags_Aligned | NW_Text_Flags_Copy);
          NW_Str_Delete(tempStr);
          if(status != KBrsrSuccess)
          {
            NW_Object_Delete (value); 
            value = NULL;
            break;
          }

          // We don't want an empty value
          if (NW_Text_GetCharCount(value) == 0) {
              NW_Object_Delete (value); 
              value = NULL;
          }
      }
      
      if (value == NULL) {
          /* no enclosed text, try the label */
          NW_XHTML_GetDOMAttribute(thisObj->contentHandler, (NW_DOM_ElementNode_t*) node, 
              NW_XHTML_AttributeToken_label, &value);
      }

      if (value == NULL) {
          // Try the title attribute
          NW_XHTML_GetDOMAttribute(thisObj->contentHandler, (NW_DOM_ElementNode_t*) node, 
              NW_XHTML_AttributeToken_title, &value);
      }
    break;

  case NW_XHTML_FormControlType_InputRadio:
  case NW_XHTML_FormControlType_InputCheckbox:
    NW_XHTML_GetDOMAttribute (thisObj->contentHandler, 
          (NW_DOM_ElementNode_t*) node, NW_XHTML_AttributeToken_value, &value);
    break;

  /* no NW_XHTML_FormControlType_Select as they don't have a value */

  default:
    status = KBrsrFailure;
    value = NULL;
    break;
  }

    *valueOut = value;
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetStringName (NW_FBox_FormLiaison_t* formLiaison,
                                     void* node,
                                     NW_Ucs2** nameOut)
{
  NW_Text_t* name = NULL;
  NW_XHTML_FormLiaison_t* thisObj;
  NW_Ucs2* storage;

  NW_TRY (status) {  
    thisObj = NW_XHTML_FormLiaisonOf (formLiaison);
    
    *nameOut = NULL;
    NW_XHTML_GetDOMAttribute (thisObj->contentHandler, (NW_DOM_ElementNode_t*) node,
      NW_XHTML_AttributeToken_name, &name);
    
    if (name == NULL) {
      NW_THROW_STATUS(status, KBrsrFailure);
    }
    
    /* convert the supplied name to Unicode and get its storage */
    storage = NW_Text_GetUCS2Buffer (name,
      NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
      NULL, NULL);
    
    if (storage == NULL) {
      NW_THROW_STATUS(status, KBrsrFailure);
    }
    
    *nameOut = storage;

  } NW_CATCH (status) {  /* empty */
  } 
  NW_FINALLY {
    NW_Object_Delete (name);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetStringTitle (NW_FBox_FormLiaison_t* formLiaison,
                                     void* node,
                                     NW_Ucs2** titleOut)
{
  NW_Text_t* title = NULL;
  NW_XHTML_FormLiaison_t* thisObj;
  NW_Ucs2* storage;
  
  NW_TRY (status) {  
    thisObj = NW_XHTML_FormLiaisonOf (formLiaison);
    
    *titleOut = NULL;
    NW_XHTML_GetDOMAttribute (thisObj->contentHandler, (NW_DOM_ElementNode_t*) node,
      NW_XHTML_AttributeToken_title, &title);
    
    if (title == NULL) {
      NW_THROW_STATUS(status, KBrsrFailure);
    }
    
    /* convert the supplied name to Unicode and get its storage */
    storage = NW_Text_GetUCS2Buffer (title,
      NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
      NULL, NULL);
    
    if (storage == NULL) {
      NW_THROW_STATUS(status, KBrsrFailure);
    }
    
    *titleOut = storage;
  } NW_CATCH (status) {  /* empty */
  } 
  NW_FINALLY {
    NW_Object_Delete (title); 
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_SetBoolValue (NW_FBox_FormLiaison_t* formLiaison,
                                   void* node,
                                   NW_Bool *value)
{
  TBrowserStatusCode status = KBrsrFailure;
  NW_XHTML_FormLiaison_t* thisObj;

  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  if (value == NULL) {
    status = NW_XHTML_ControlSet_SetValue (thisObj->controlSet,
                                          (NW_DOM_ElementNode_t*)node,
                                          NULL);
    return status;
  }

  switch (NW_XHTML_FormLiaison_GetControlType (thisObj, (NW_DOM_ElementNode_t*) node)) {
  case NW_XHTML_FormControlType_InputCheckbox:
    status = NW_XHTML_FormLiaison_SetCheckboxValue (thisObj, 
        (NW_DOM_ElementNode_t*) node, *value);
    break;

  case NW_XHTML_FormControlType_InputRadio:
    status = NW_XHTML_FormLiaison_SetRadioValue (thisObj, 
        (NW_DOM_ElementNode_t*) node, *value);
    break;

  case NW_XHTML_FormControlType_Option:
		status = NW_XHTML_FormLiaison_SetOptionValue (thisObj, 
        (NW_DOM_ElementNode_t*) node, *value);
    break;

  default:
    /*
     * cannot set value; the input element is of an unknown type,
     * hidden, reset, submit or one of the elements with text value
     */
    status = KBrsrFailure;
    break;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_ToggleBoolValue (NW_FBox_FormLiaison_t* formLiaison,
                                      void* node)
{
  NW_Bool value;
  
  if (NW_FBox_FormLiaison_GetBoolValue (formLiaison, node, &value) !=
      KBrsrSuccess) {
    return KBrsrFailure;
  }

  value = (NW_Bool) !value;
  if (NW_FBox_FormLiaison_SetBoolValue (formLiaison, node, &value) !=
      KBrsrSuccess) {
    return KBrsrFailure;
  }

  (void) NW_FBox_FormLiaison_DelegateEcmaEvent(formLiaison, 
      (NW_DOM_ElementNode_t*) node, NW_Ecma_Evt_OnChange);
  /*generate OnChange ecma event , for checkBox/selectBox/options*/
  (void) NW_FBox_FormLiaison_DelegateEcmaEvent(formLiaison, 
      (NW_DOM_ElementNode_t*) node, NW_Ecma_Evt_OnClick);
  
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetBoolValue (NW_FBox_FormLiaison_t* formLiaison,
                                   void* node,
                                   NW_Bool* valueOut)
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Object_t* value;
  NW_XHTML_FormLiaison_t* thisObj;

  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  switch (NW_XHTML_FormLiaison_GetControlType (thisObj, (NW_DOM_ElementNode_t*) node)) {
  case NW_XHTML_FormControlType_InputCheckbox: /* FALL THROUGH */
  case NW_XHTML_FormControlType_InputRadio:    /* FALL THROUGH */
  case NW_XHTML_FormControlType_Option:
    value = NW_XHTML_ControlSet_GetValue (thisObj->controlSet, (NW_DOM_ElementNode_t*) node);

    /* if value has not been set yet, use the initial (reset) value */
    if (value == NULL) {
      value = NW_XHTML_FormLiaison_GetInitialValue (thisObj, (NW_DOM_ElementNode_t*) node);
    }

    /* map FormLiaisonTrue and FormLiaisonFalse to NW_TRUE and NW_FALSE */
    if (value == (NW_Object_t*) &NW_XHTML_FormLiaisonTrue) {
      if (valueOut != NULL) {
        *valueOut = NW_TRUE;
      }
    } else if (value == (NW_Object_t*) &NW_XHTML_FormLiaisonFalse) {
      if (valueOut != NULL) {
        *valueOut = NW_FALSE;
      }
    } else {
      /* either NULL or not boolean value */
      status = KBrsrFailure;
    }
    break;

  default:
    status = KBrsrFailure;
    break;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FormLiaison_Reset (NW_FBox_FormLiaison_t* formLiaison, 
                             void* resetButtonNode)
{
 TBrowserStatusCode status = NW_FBox_FormLiaison_DelegateEcmaEvent(formLiaison, resetButtonNode, 
                                                           NW_Ecma_Evt_OnReset) ;
  /*generate OnReset ecma event. If the return is TRUE then submit the form */
  if(status == KBrsrNotFound)
  {
    /* handle the form reset */  
    status = NW_XHTML_FormLiaison_Reset(formLiaison,resetButtonNode);
  }
  return status;
}


/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_XHTML_FormLiaison_Submit (NW_FBox_FormLiaison_t* formLiaison,
                              void* submitButtonNode)
{

  TBrowserStatusCode status = NW_FBox_FormLiaison_DelegateEcmaEvent(formLiaison, submitButtonNode, 
                                                           NW_Ecma_Evt_OnSubmit) ;
  /*generate OnSubmit ecma event. If the return is TRUE then submit the form */
  if(status == KBrsrNotFound)
  {
    /* handle the form submit */  
    status = NW_XHTML_FormLiaison_Submit (formLiaison,submitButtonNode);
  }
  return status;
}


/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_XHTML_FormLiaison_Focus (NW_FBox_FormLiaison_t* formLiaison,
                              void* node)
{
  TBrowserStatusCode status = KBrsrSuccess;
  status = NW_FBox_FormLiaison_DelegateEcmaEvent(formLiaison, node, NW_Ecma_Evt_OnFocus) ;
  return status;
}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_XHTML_FormLiaison_IsOptionMultiple (NW_FBox_FormLiaison_t* formLiaison,
                                       void* controlId)
{
  NW_DOM_ElementNode_t* optionNode;
  NW_DOM_ElementNode_t* parentNode;
  NW_Object_t* value;
  NW_XHTML_FormLiaison_t* thisObj;
  static const NW_Ucs2 multipleTag[] = {'m','u','l','t','i','p','l','e','\0'};
  
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  optionNode = (NW_DOM_ElementNode_t*) controlId;
  parentNode = NW_DOM_Node_getParentNode(optionNode);
  while (parentNode != NULL) 
  {
    if (NW_HED_DomHelper_GetElementToken (parentNode) == NW_XHTML_ElementToken_select) {
      /* found the <select> node */
      break;
    }
    parentNode = NW_DOM_Node_getParentNode(parentNode);
  }
  if (parentNode == NULL){
    return NW_FALSE;
  }
  value = NW_XHTML_FormLiaison_GetBoolAttribute (thisObj,
                                                parentNode,
                                                NW_XHTML_AttributeToken_multiple,
                                                multipleTag);

  if (value == NULL){
    return NW_FALSE;
  }
  if (value == (NW_Object_t*) &NW_XHTML_FormLiaisonTrue) {
    return NW_TRUE;
  }
  return NW_FALSE;  
}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_XHTML_FormLiaison_IsOptionSelected(NW_FBox_FormLiaison_t* formLiaison,
                                       void* controlId)
{
  NW_DOM_ElementNode_t* optionNode;
  NW_XHTML_FormLiaison_t* thisObj;
  static const NW_Ucs2 selectedTag[] = {'s','e','l','e','c','t','e','d','\0'};
  
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  optionNode = (NW_DOM_ElementNode_t*) controlId;
  if( NW_XHTML_FormLiaison_GetBoolAttribute (thisObj,
                                             optionNode,
                                             NW_XHTML_AttributeToken_selected,
                                             selectedTag) 
                                             == (NW_Object_t*) &NW_XHTML_FormLiaisonTrue) {
    return NW_TRUE;
  }
  return NW_FALSE;  
}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_XHTML_FormLiaison_OptionHasOnPick(NW_FBox_FormLiaison_t* formLiaison,
                                      void* controlId)
{
  NW_REQUIRED_PARAM (formLiaison);
  NW_REQUIRED_PARAM (controlId);

  /* XHTML doesn't support OnPick attribute */
  return NW_FALSE; 
}

NW_Bool
_NW_XHTML_FormLiaison_IsLocalNavOnPick(NW_FBox_FormLiaison_t* formLiaison,
                                      void* controlId)
{
  NW_REQUIRED_PARAM (formLiaison);
  NW_REQUIRED_PARAM (controlId);

  /* XHTML doesn't support OnPick attribute - so we don't care about local navs either */
  return NW_FALSE; 
}

TBrowserStatusCode
_NW_XHTML_FormLiaison_GetDocRoot(NW_FBox_FormLiaison_t *thisObj,
                                    NW_HED_DocumentRoot_t **docRoot)
{
  NW_XHTML_FormLiaison_t  *xhtmlFormLiaison;
  NW_ASSERT (docRoot != NULL);
  xhtmlFormLiaison = NW_XHTML_FormLiaisonOf(thisObj);
  *docRoot = (NW_HED_DocumentRoot_t*)
    NW_HED_DocumentNode_GetRootNode (xhtmlFormLiaison->contentHandler);
  return KBrsrSuccess;
}


TBrowserStatusCode
_NW_XHTML_FormLiaison_DelegateEcmaEvent(NW_FBox_FormLiaison_t* /*thisObj*/,
                                        void* /*eventNode*/,
                                        NW_ECMA_Evt_Type_t /*ecmaEvent*/)
{
  // Calls the ecma handle event on EcmaContentHandler
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_FormLiaison_RegisterValidator (NW_XHTML_FormLiaison_t* thisObj,
                                         void* node,
                                         NW_FBox_Validator_t* validator)
{
  TBrowserStatusCode status;
  NW_FBox_Validator_t* oldValidator;

  if (thisObj->validatorMap == NULL) {
    thisObj->validatorMap =
      NW_ADT_ResizableMap_New (sizeof (void*), sizeof (NW_FBox_Validator_t*),
                               1, 1);
    if (thisObj->validatorMap == NULL) {
      return KBrsrOutOfMemory;
    }
  } else {
    status = NW_ADT_Map_Get (thisObj->validatorMap, &node, &oldValidator);
    if (status == KBrsrSuccess) {
      NW_Object_Delete (oldValidator);
  }
  }
  return NW_ADT_Map_Set (thisObj->validatorMap, &node, &validator);
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_FormLiaison_AddBox (NW_FBox_FormLiaison_t* formLiaison,
                             NW_LMgr_Box_t* box , NW_DOM_ElementNode_t* searchElementNode)
{
  NW_XHTML_FormLiaison_t* thisObj;
  NW_DOM_ElementNode_t*  elementNode;
  NW_ADT_Vector_Metric_t elementSize;
  NW_ADT_Vector_Metric_t index = 0;

#if defined(XFL_LOGGING)
  NW_ADT_Vector_Metric_t boxIndex = 0;
#endif

  /* parameter assertions */
  NW_ASSERT (NW_Object_IsInstanceOf (formLiaison, &NW_XHTML_FormLiaison_Class));

  /* for convenience */
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);


  /*find the index of the  node vector*/
   elementSize = NW_ADT_Vector_GetSize(thisObj->controlSet->nodeVector);
   
   while (index < elementSize ) 
   {
    elementNode = *(NW_DOM_ElementNode_t**)NW_ADT_Vector_ElementAt (thisObj->controlSet->nodeVector, index);
    if(elementNode == searchElementNode)
    {

#if defined(XFL_LOGGING)
      boxIndex = index;
#endif     
      if(index  < NW_ADT_Vector_GetSize(thisObj->controlSet->boxVector))
        NW_ADT_DynamicVector_ReplaceAt (thisObj->controlSet->boxVector, &box, index );
      else
          {
          NW_ADT_DynamicVector_InsertAt (thisObj->controlSet->boxVector, &box, NW_ADT_Vector_AtEnd );

#if defined(XFL_LOGGING)
          boxIndex = (NW_ADT_Vector_Metric_t) (NW_ADT_Vector_GetSize(thisObj->controlSet->boxVector) - 1) ;
#endif
          }

#if defined(XFL_LOGGING)
      {
	  TBuf16<300> buf;
      buf.Format( _L("XhtmlFormLiaison::AddBox elementNode=%x, box=%x index=%d, boxIndex=%d\n"), elementNode, box, index, boxIndex);
      RFileLogger::Write( KLogDirName, KLogFileName, EFileLoggingModeAppend, buf );
      NW_LMgr_Box_DumpBoxTree(box);
      }
#endif

      break;
    }
    index++;
   }
  /* successful completion */
  return KBrsrSuccess;
}


TBrowserStatusCode
NW_XHTML_FormLiaison_GetAction(NW_FBox_FormLiaison_t* formLiaison,
                              NW_Ucs2** formAction)
{
  NW_Ucs2*    tFormAction = NULL;
  NW_XHTML_FormLiaison_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  tFormAction= NW_Text_GetUCS2Buffer (thisObj->formAction,NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
							                  NULL, NULL);
  if(tFormAction)
  {
    *formAction = tFormAction;
  }
  else
  {
    status = KBrsrOutOfMemory;
  }

  
  return status;
}

TBrowserStatusCode
NW_XHTML_FormLiaison_SetAction (NW_XHTML_FormLiaison_t* thisObj,
                                const NW_Ucs2* formAction)
{
  NW_Text_t* formActionText = NULL;
  NW_TRY (status) 
  {  
    formActionText = NW_Text_New (HTTP_iso_10646_ucs_2, (NW_Ucs2* )formAction, 
                      NW_Str_Strlen(formAction), NW_Text_Flags_Copy);
    NW_THROW_OOM_ON_NULL(formActionText , status);
    /*delete prev formAction string */ 
    NW_Object_Delete (thisObj->formAction);
    thisObj->formAction = formActionText;
  } 
  NW_CATCH (status) 
  {  /* empty */
  } 
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY

}

TBrowserStatusCode
NW_XHTML_FormLiaison_SetMethod (NW_XHTML_FormLiaison_t* thisObj,
                                const NW_Ucs2* formMethod)
{
  NW_Text_t* formMethodText = NULL;
  NW_TRY (status) 
  {  
    formMethodText = NW_Text_New (HTTP_iso_10646_ucs_2, (NW_Ucs2* )formMethod, 
                      NW_Str_Strlen(formMethod), NW_Text_Flags_Copy);
    NW_THROW_OOM_ON_NULL(formMethodText, status);
    /*delete prev formMethod string */ 
    NW_Object_Delete (thisObj->formMethod);
    thisObj->formMethod = formMethodText;
  } 
  NW_CATCH (status) 
  {  /* empty */
  } 
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY

}

TBrowserStatusCode
NW_XHTML_FormLiaison_GetEnctype(NW_FBox_FormLiaison_t* formLiaison,
                              NW_Ucs2** formEnctype)
{
  NW_Ucs2*    tFormEnctype = NULL;
  NW_XHTML_FormLiaison_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  tFormEnctype= NW_Text_GetUCS2Buffer (thisObj->formEnctype,NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
							                  NULL, NULL);
  if(tFormEnctype)
  {
    *formEnctype = tFormEnctype;
  }
  else
  {
    status = KBrsrOutOfMemory;
  }
  
  return status;
}

TBrowserStatusCode
NW_XHTML_FormLiaison_SetEnctype (NW_XHTML_FormLiaison_t* thisObj,
                                const NW_Ucs2* formEnctype)
{
  NW_Text_t* formEnctypeText = NULL;
  NW_TRY (status) 
  {  
    formEnctypeText = NW_Text_New (HTTP_iso_10646_ucs_2, (NW_Ucs2* )formEnctype, 
                      NW_Str_Strlen(formEnctype), NW_Text_Flags_Copy);
    NW_THROW_OOM_ON_NULL(formEnctypeText, status);
    /*delete prev formEnctype string */ 
    NW_Object_Delete (thisObj->formEnctype);
    thisObj->formEnctype = formEnctypeText;
  } 
  NW_CATCH (status) 
  {  /* empty */
  } 
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY

}


TBrowserStatusCode
NW_XHTML_FormLiaison_GetId(NW_FBox_FormLiaison_t* formLiaison,
                              NW_Ucs2** formId)
{
  NW_Ucs2*    tFormId = NULL;
  NW_XHTML_FormLiaison_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;

  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  tFormId= NW_Text_GetUCS2Buffer (thisObj->formId,NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
							                  NULL, NULL);
  if(tFormId)
  {
    *formId = tFormId;
  }
  else
  {
    status = KBrsrOutOfMemory;
  }
  
  return status;
}


TBrowserStatusCode
NW_XHTML_FormLiaison_SetId (NW_XHTML_FormLiaison_t* thisObj,
                                const NW_Ucs2* formId)
{
  NW_Text_t* formIdText = NULL;
  NW_TRY (status) 
  {  
    formIdText = NW_Text_New (HTTP_iso_10646_ucs_2, (NW_Ucs2* )formId, 
                      NW_Str_Strlen(formId), NW_Text_Flags_Copy);
    NW_THROW_OOM_ON_NULL(formIdText, status);
    /*delete prev formMethod string */ 
    NW_Object_Delete (thisObj->formId);
    thisObj->formId = formIdText;
  } 
  NW_CATCH (status) 
  {  /* empty */
  } 
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY

}




TBrowserStatusCode
NW_XHTML_FormLiaison_SetFormName (NW_XHTML_FormLiaison_t* thisObj,
                                const NW_Ucs2* formName)
{
  NW_Text_t* formNameText = NULL;
  NW_TRY (status) 
  {  
    formNameText = NW_Text_New (HTTP_iso_10646_ucs_2, (NW_Ucs2* )formName, 
                      NW_Str_Strlen(formName), NW_Text_Flags_Copy);
    NW_THROW_OOM_ON_NULL(formNameText, status);
    /*delete prev formMethod string */ 
    NW_Object_Delete (thisObj->formName);
    thisObj->formName = formNameText;
  } 
  NW_CATCH (status) 
  {  /* empty */
  } 
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY

}

TBrowserStatusCode
NW_XHTML_FormLiaison_Submit (NW_FBox_FormLiaison_t* formLiaison,
                              void* submitButtonNode)
{
  NW_XHTML_FormLiaison_t* thisObj;
  NW_NVPair_t*            nameValuePairs = NULL;
  NW_Text_t*              urlText = NULL;
  NW_Text_t*              retUrl = NULL;
  const NW_Ucs2*          url = NULL;
  NW_Bool                 urlFreeNeeded = NW_FALSE;
  const NW_Ucs2*          method = NULL;
  NW_Bool                 methodFreeNeeded = NW_FALSE;
  NW_Bool                 isPost = NW_FALSE; /* assume method="get" */
  NW_Text_t*              acceptCharsetText = NULL;
  const NW_Ucs2*          acceptCharset = NULL;
  NW_Bool                 acceptCharsetFreeNeeded = NW_FALSE;
  const NW_Ucs2*          enctype = NULL;
  NW_Bool                 enctypeFreeNeeded = NW_FALSE;
  NW_Http_CharSet_t       documentCharset = HTTP_unknown;
  NW_Http_Header_t*       requestHeader = NULL;
  NW_Ucs2*                requestUrl = NULL;
  NW_Text_UCS2_t*         requestUrlText = NULL;
  NW_Uint8                requestMethod = NW_URL_METHOD_GET;
  NW_Buffer_t*            requestBody = NULL;
  NW_HED_UrlRequest_t*    urlRequest = NULL;
  NW_Cache_Mode_t         cacheMode = NW_CACHE_NORMAL;
  NW_HED_DocumentRoot_t*  docRoot;
  NW_HED_AppServices_t*   appServices = NULL;
  void*                   appCtx = NULL;

  NW_TRY (status) {
    
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);
  // we wont be able to submit if there is no formAction in the html page
  if(thisObj->formAction)
  {
// TODO CHARSET: This comment doesn't match the code. Fix one or the other.
    /* if the encoding setting is either gb2312 or big5, set app services and app context
       so that later they could be used to make charactset converter calls.
       Otherwise, they would remain NULL, and no charset conversion would occur later
    */
    if (NW_XHTML_FormLiaison_IsSupportedEncoding(NW_Settings_GetOriginalEncoding()))
    {
      /* get appServices and Browser app context */
      docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(thisObj->contentHandler);
      appServices = (NW_HED_AppServices_t*) NW_HED_DocumentRoot_GetAppServices(docRoot);
      appCtx = NW_HED_DocumentRoot_GetBrowserAppCtx(docRoot);
    }

    // Iterate through successful controls and build the set of name-value pairs.
    status = NW_XHTML_FormLiaison_GetNameValuePairs( thisObj, submitButtonNode, &nameValuePairs );
    NW_THROW_ON_ERROR( status );

    urlText = NW_Text_Copy(thisObj->formAction, NW_TRUE);  /* deep copy */

    /* get and resolve the original url */
    status = NW_HED_ContentHandler_ResolveURL (
        thisObj->contentHandler, urlText, &retUrl);
    _NW_THROW_ON_ERROR( status );

    /* transfer ownership to urlText */
    urlText = retUrl;
    retUrl = NULL;

    url = NW_Text_GetUCS2Buffer (urlText, NW_Text_Flags_NullTerminated |
        NW_Text_Flags_Aligned, NULL, &urlFreeNeeded);
    NW_THROW_OOM_ON_NULL (url, status);

    /* are we posting */
    if (thisObj->formMethod != NULL) {
      method = NW_Text_GetUCS2Buffer(thisObj->formMethod,
          NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned,
          NULL, &methodFreeNeeded);
      NW_THROW_OOM_ON_NULL (method, status);

      if (NW_Str_StricmpConst(method, "post") == 0) {
        isPost = NW_TRUE;
        cacheMode = NW_CACHE_NOCACHE;
      }
    }

    /* get the enctype attribute */
    if (thisObj->formEnctype != NULL) {
      enctype = NW_Text_GetUCS2Buffer (thisObj->formEnctype, NW_Text_Flags_NullTerminated | 
                                       NW_Text_Flags_Aligned, NULL, &enctypeFreeNeeded);
      NW_THROW_OOM_ON_NULL (enctype, status);
    }

    /* get the accept-charset attribute */
    NW_XHTML_GetDOMAttribute (thisObj->contentHandler, thisObj->formNode,
                                                  NW_XHTML_AttributeToken_accept_charset, &acceptCharsetText);
    if (acceptCharsetText != NULL) {
      acceptCharset = NW_Text_GetUCS2Buffer(acceptCharsetText, NW_Text_Flags_NullTerminated | 
                                            NW_Text_Flags_Aligned, NULL, &acceptCharsetFreeNeeded);
      NW_THROW_OOM_ON_NULL (acceptCharset, status);
    }

    /* get document charset */
    documentCharset = (NW_Http_CharSet_t) NW_HED_DomHelper_GetEncoding(thisObj->formNode);

    // If this is a multipart post, then the nameValuePairs need to be checked.
    // If any are for files to be uploaded, then the existance of the files
    // has to be confirmed.
    if ( isPost && NW_LoadReq_IsMultipart( enctype ) ) {
        TBool missingFiles;
        TBool restrictedFiles;
        status = NW_XHTML_FormLiaison_CheckFiles( thisObj, nameValuePairs,
            &missingFiles, &restrictedFiles );
        if ( status != KBrsrSuccess ) {
            // If the user cancels, then exit without posting.
            if ( status == KBrsrCancelled ) {
                NW_THROW_STATUS( status, KBrsrSuccess );
            } else {
                NW_THROW( status );
            }
        }
        if ( missingFiles || restrictedFiles ) {
            // If there were missing or restricted files, then those controls
            // with missing or restricted files were reset and the set of
            // name-value pairs is no longer accurate and needs to be re-built.
            NW_NVPair_Delete(nameValuePairs);
            nameValuePairs = NULL;
            status = NW_XHTML_FormLiaison_GetNameValuePairs( thisObj, submitButtonNode,
                &nameValuePairs );
            NW_THROW_ON_ERROR( status );
        }
    }

    /* create the header and body for the request */
    /* This following is a fairly completed sequence of events.  The reason is that
       we want NW_LoadReq_Create to do the checking for enctype and acceptCharset.
       If they are invalid in HTML, we want to accept the default by passing in NULL for
       these parameters. */
    status = NW_LoadReq_Create (url, isPost, NULL, acceptCharset, nameValuePairs, enctype,
                                documentCharset, &requestHeader, &requestUrl, &requestMethod, 
                                &requestBody, appServices, appCtx);
    if (status == KBrsrWmlbrowserBadContent ) {
      /* The enctype is invalid.  Force to NULL so will take the default
         value (urlencoded). */
      if ( enctypeFreeNeeded ) {
        NW_Str_Delete( (NW_Ucs2*) enctype );
      }
      enctype = NULL;
      status = NW_LoadReq_Create (url, isPost, NULL, acceptCharset, nameValuePairs, enctype,
                                  documentCharset, &requestHeader, &requestUrl, &requestMethod, 
                                  &requestBody, appServices, appCtx);
    }
    if (status == KBrsrUnsupportedFormCharset ) {
      /* The accept-charset is invalid.  Force to NULL so will take the default
         value provided by the document charset. */
      if ( acceptCharsetFreeNeeded ) {
        NW_Str_Delete( (NW_Ucs2*) acceptCharset );
      }
      acceptCharset = NULL;
      status = NW_LoadReq_Create (url, isPost, NULL, acceptCharset, nameValuePairs, enctype,
                                  documentCharset, &requestHeader, &requestUrl, &requestMethod, 
                                  &requestBody, appServices, appCtx);
    }
    NW_THROW_ON_ERROR (status);

    /* make a text object for the url */
    requestUrlText = NW_Text_UCS2_New (requestUrl, 0, NW_Text_Flags_TakeOwnership);
    requestUrl = NULL;
    NW_THROW_OOM_ON_NULL (requestUrlText, status);

    /* create a top level load request from the url, header and body */
    urlRequest = NW_HED_UrlRequest_New (NW_TextOf(requestUrlText), requestMethod,
        requestHeader, requestBody, NW_HED_UrlRequest_Reason_DocLoad, 
        NW_HED_UrlRequest_LoadNormal, NW_UrlRequest_Type_Any);
    NW_THROW_OOM_ON_NULL (urlRequest, status);

    NW_HED_UrlRequest_SetCacheMode ((NW_HED_UrlRequest_t*) urlRequest, cacheMode);
    requestHeader = NULL;
    requestBody = NULL;

    //Set the referrer header also.

    NW_XHTML_ContentHandler_SetReferrerUrl(thisObj->contentHandler);

#if !defined(__SERIES60_28__) && !defined(__SERIES60_27__)
    NW_Text_t* target = NULL;
    NW_DOM_ElementNode_t* formNode = (NW_DOM_ElementNode_t*)submitButtonNode;
    // Find the form node and look for a target attribute on it
    while (formNode != NULL)
    	{
    	formNode = NW_DOM_Node_getParentNode(formNode);
    	NW_Uint16 token = NW_HED_DomHelper_GetElementToken (formNode);
    	if (token == NW_XHTML_ElementToken_form)
    		{
    		break;
    		}  	
    	}
    if (formNode != NULL)
    	{
    	NW_XHTML_GetDOMAttribute (thisObj->contentHandler, formNode, NW_XHTML_AttributeToken_target, &target);
    	}
    TBool newWindow = EFalse;
	if (target != NULL && target->characterCount == 0)
       	{
       	NW_Object_Delete(target);
       	target = NULL;
       	}
    if (target == NULL)
    	{
    	NW_XHTML_ContentHandler_GetBaseTagTarget (thisObj->contentHandler, &target);
    	}
    if (target != NULL)
        {
        // If the target attribute exists, and it is not an empty string, and 
        // it is not "_self", open the url in a new window
        NW_Bool targetTextFreeNeeded = NW_FALSE;;
        NW_Ucs2* targetStorage;
        HBufC8* enctypeBuf = NULL;
        NW_Text_Length_t targetTextLength;
        _LIT(KSelf, "_self");
        targetStorage = NW_Text_GetUCS2Buffer (target, NW_Text_Flags_NullTerminated,
            &targetTextLength, &targetTextFreeNeeded);
        // Check that the target attribute is not empty
        TPtrC targetPtr(targetStorage, targetTextLength);
        if (targetTextLength > 0 && targetPtr.CompareF(KSelf) != 0)
            {
            if (requestMethod == NW_URL_METHOD_GET)
                {
                status = NW_XHTML_ContentHandler_WindowOpen(thisObj->contentHandler, (NW_Text_t*)requestUrlText, target);
                }
            else
                {
                // get the content-type (enctype) of the request body.
                // use what is in the form, or url encoding by default
                _LIT8(KTypePtr, "application/x-www-form-urlencoded");
                TPtrC8 enctypePtr(NULL, 0);
                if (enctype != NULL)
                    {
					TInt ret;
                    TRAP(ret, enctypeBuf = HBufC8::NewL(User::StringLength(enctype) + 1));
                    if (enctypeBuf != NULL)
                        {
                        TPtrC encPtr(enctype);
                        enctypeBuf->Des().Copy(encPtr);
                        enctypeBuf->Des().ZeroTerminate();
                        }
                    }
                else
                    {
						TInt ret;
                        TRAP(ret, enctypeBuf = HBufC8::NewL(KTypePtr().Length()+ 1));
                    if (enctypeBuf != NULL)
                        {
                        enctypeBuf->Des().Copy(KTypePtr);
                        enctypeBuf->Des().ZeroTerminate();
                        }
                    }
                if (enctypeBuf == NULL)
                    {
                    status = KBrsrOutOfMemory;
                    }
                else
                    {
                    status = NW_XHTML_ContentHandler_WindowOpenAndPost(thisObj->contentHandler,
                        (NW_Text_t*)requestUrlText, target,(NW_Uint8*)enctypeBuf->Ptr(), (NW_Uint32)(enctypeBuf->Length()),
                        NW_HED_UrlRequest_GetBody(urlRequest)->data, 
                        NW_HED_UrlRequest_GetBody(urlRequest)->length,
                        NULL);//R->ul ((HttpRequestHeaders*)NW_HED_UrlRequest_GetHeader(urlRequest))->boundary);
                    }
                delete enctypeBuf;
                }
            newWindow = ETrue;
            }
        if (targetTextFreeNeeded)
            {
            NW_Mem_Free(targetStorage);
            }
        NW_Object_Delete(target);
        }
    if (!newWindow)
#endif
		{
	    /* submit the request */
	    status = NW_XHTML_ContentHandler_StartRequest( thisObj->contentHandler, urlRequest, NULL );
	    NW_THROW_ON_ERROR (status);

	    urlRequest = NULL;
		}
  	}
  else
  {
    status = KBrsrSuccess;
  }
 } 
  
  NW_CATCH (status) {
  } 
  
  NW_FINALLY {
    if (nameValuePairs)
    {
        NW_NVPair_Delete(nameValuePairs);
    }
    NW_Object_Delete (urlText);
    if (urlFreeNeeded) 
    {
      NW_Str_Delete((NW_Ucs2*) url);
    }
    if ( methodFreeNeeded ) 
    {
      NW_Str_Delete( (NW_Ucs2*) method );
    }
    if (enctypeFreeNeeded) 
    {
      NW_Str_Delete((NW_Ucs2*) enctype);
    }
    NW_Object_Delete(acceptCharsetText);
    if (acceptCharsetFreeNeeded) 
    {
      NW_Str_Delete((NW_Ucs2*) acceptCharset);
    }

//R->ul    UrlLoader_HeadersFree (requestHeader);
    requestHeader = NULL;
    NW_Str_Delete (requestUrl);
    requestUrl = NULL;
    NW_Object_Delete(requestUrlText);
    requestUrlText = NULL;
    NW_Buffer_Free (requestBody);
    requestBody = NULL;
    NW_Object_Delete (urlRequest);
    urlRequest = NULL;

    return status;
  } NW_END_TRY
}


TBrowserStatusCode
NW_XHTML_FormLiaison_Reset (NW_FBox_FormLiaison_t* formLiaison, 
                             void* resetButtonNode)
{
  NW_XHTML_FormLiaison_t* thisObj;
  NW_LMgr_Box_t* box;
  NW_ADT_Vector_Metric_t index =  0;

  NW_REQUIRED_PARAM(resetButtonNode);
    
  thisObj = NW_XHTML_FormLiaisonOf (formLiaison);

  index = thisObj->firstControl;

  while (index < (thisObj->firstControl + thisObj->numControls - 1))
  {
    box = *(NW_LMgr_Box_t**)NW_ADT_Vector_ElementAt (thisObj->controlSet->boxVector, index);
    if(box && !NW_Object_IsInstanceOf (box, &NW_FBox_OptGrpBox_Class )&&
        !NW_Object_IsInstanceOf (box, &NW_FBox_OptionBox_Class))
    {
      if (NW_Object_IsInstanceOf (box, &NW_FBox_FormBox_Class) == NW_TRUE) 
      {
        (void) NW_FBox_FormBox_Reset ((NW_FBox_FormBox_t*) box);
      }
    }
    index++;
  }

  // radiobox's on or off depends on default radiobox; 
  // refresh box only when all box are reset
  index = thisObj->firstControl;
  while (index < (thisObj->firstControl + thisObj->numControls - 1))
  {
    box = *(NW_LMgr_Box_t**)NW_ADT_Vector_ElementAt (thisObj->controlSet->boxVector, index);
    if(box && !NW_Object_IsInstanceOf (box, &NW_FBox_OptGrpBox_Class ) &&
          !NW_Object_IsInstanceOf (box, &NW_FBox_OptionBox_Class))
    {
      NW_LMgr_Box_Refresh(box);
    }
    index++;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_XHTML_FormLiaison_t*
NW_XHTML_FormLiaison_New (NW_DOM_ElementNode_t* formNode,
                          NW_XHTML_ContentHandler_t* contentHandler,
                          NW_XHTML_ControlSet_t* controlSet)
{
  return (NW_XHTML_FormLiaison_t*)NW_Object_New (&NW_XHTML_FormLiaison_Class,
                                                 formNode,
                                                 contentHandler,
                                                 controlSet);
}
