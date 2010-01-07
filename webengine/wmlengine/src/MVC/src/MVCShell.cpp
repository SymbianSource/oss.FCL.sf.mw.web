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
* Description:  
*
*/

// INCLUDE FILES
#include <e32def.h> // resolve NULL redefinition warning.
#include <e32base.h>
#include <e32std.h>
#include <bldvariant.hrh>
#include <CHARCONV.H>
#include <eikenv.h>
#include <f32file.h>
#include <stddef.h>
#include <stdlib.h>
#include <stringloader.h>
#include <cuseragent.h>
#include <webkit.rsg>
#include <uri16.h>
#include <apmstd.h>
#include <caf/caf.h>
#include <caf/caftypes.h>
#include <caf/supplieroutputfile.h> 
#include <Oma2Agent.h>
#include <brctldefs.h>
#include <brctldialogsprovider.h>

#include "brctl.h"

#include "BrsrStatusCodes.h"
#include "MVCShell.h"
#include "contentloader.h"
#include "GDIDeviceContext.h"
#include "kimonolocalizationstrings.h"
#include "MVCView.h"
#include "nw_adt_resizableVector.h"
#include "nw_browser_browsermimetable.h"
#include "nw_evlog_api.h"
#include "nw_hed_appservices.h"
#include "nw_hed_appservicesimpl.h"
#include "nw_hed_documentroot.h"
#include "nw_hed_historyvisitor.h"
#include "nw_hed_historyentry.h"
#include "nw_lmgr_boxvisitor.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_msg_dsp_api.h"
#include "nw_msg_messagedispatcher.h"
#include "nw_mvc_iloadlistener.h"
#include "nw_text_ucs2.h"
//#include "nw_unknown_contenthandler.h"
#include "nw_wae.h"
#include <nw_wbxml_dictionary.h>
#include "nw_wml_core.h"
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"
#include "nw_wml1x_wml1xformliaison.h"
#include "nw_wml1x_epoc32contenthandler.h"
#include "nw_xhtml_epoc32contenthandler.h"
#include "nwx_assert.h"
#include "nwx_ctx.h"
#include "nwx_datastruct.h"
#include "nwx_defs_symbian.h"
#include "nwx_http_defs.h"
#include "nwx_http_header.h"
#include "nwx_memseg.h"
#include "nwx_msg_api.h"
#include "nwx_msg_map_epoc32.h"
#include "nwx_logger.h"
#include "nwx_msg_api.h"
#include "nwx_settings.h"
#include "nwx_statuscodeconvert.h"
#include "nwx_string.h"
#include "nwx_time.h"
#include "nwx_ui_callbacks_register.h"
#include "stringutils.h"
#include "urlloader_loaderutils.h"
#include "urlloader_urlloaderint.h"
#include "urlloader_urlloaderI.h"
#include "urlloader_urlresponse.h"
#include "CSSVariableStyleSheet.h"
#include "TEncodingMapping.h"
#include "WmlInterface.h"

// EXTERNAL DATA STRUCTURES
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _NW_WaeUsrAgent_t NW_WaeUsrAgent_t;

// EXTERNAL FUNCTION PROTOTYPES  

  NW_WaeUsrAgent_t *NW_WaeUsrAgent_New();

// LOCAL CONSTANTS AND MACROS
_LIT(KHttpString, "http://");
_LIT(KFileString, "file://");
_LIT(KEmptyTitle, "");

#define SHELL_MSG_UPDATEDISPLAY 1
#define SHELL_MSG_RELAYOUT 2
#define SHELL_MSG_REDRAW 3
#define SHELL_MSG_REDRAW_NOW 4
#define SHELL_MSG_UPDATEDISPLAYPARTIAL 5
#define SHELL_MSG_RELAYOUT_CREATETAB 6

/* ------------------------------------------------------------------------- *
   known WBXML dictionaries
 * ------------------------------------------------------------------------- */
 extern NW_WBXML_Dictionary_t NW_XHTML_WBXMLDictionary;
 extern NW_WBXML_Dictionary_t NW_Wml_1_0_WBXMLDictionary;
 extern NW_WBXML_Dictionary_t NW_Wml_1_1_WBXMLDictionary;
 extern NW_WBXML_Dictionary_t NW_Wml_1_2_WBXMLDictionary;
 extern NW_WBXML_Dictionary_t NW_Wml_1_3_WBXMLDictionary;

/* ------------------------------------------------------------------------- */
const NW_WBXML_Dictionary_t* const ShellWBXMLDictionaries[] = {
  &NW_XHTML_WBXMLDictionary,
  &NW_Wml_1_0_WBXMLDictionary,
  &NW_Wml_1_1_WBXMLDictionary,
  &NW_Wml_1_2_WBXMLDictionary,
  &NW_Wml_1_3_WBXMLDictionary,
  NULL
};

// MODULE DATA STRUCTURES

// ---------------------------------------------------------------------------- 
//    Application services
// ----------------------------------------------------------------------------  
const NW_HED_AppServices_t appServices = 
{
  /* NW_Scr_DialogApi_t */
  {
    CShell::NW_UI_WmlScriptDlgAlert,
    CShell::NW_UI_WmlScriptDlgConfirm,
    CShell::NW_UI_WmlScriptDlgPrompt
  },

  /* NW_ErrorApi_t */
  {
    CShell::NW_UI_NotifyError,
    CShell::NW_UI_HttpError
  },

  /* NW_WtaiApi_t */
  {
    NULL /*NW_HED_wp_makeCall*/,
    NULL /*NW_HED_wp_sendDTMF*/,
    NULL /*NW_HED_wp_addPBEntry*/,
    NULL /*NW_HED_nokia_locationInfo*/
  },

  /* NW_GenDlgApi_t */ 
  {
    CShell::NW_UI_DialogListSelect,
    CShell::NW_UI_DialogPrompt,
    CShell::NW_UI_DialogInputPrompt,
    CShell::NW_UI_DialogSelectOption,
    NULL
  },

  /* NW_AppServices_Picture_API_t */
  {
    NULL,
    NULL
   },

  /* NW_UrlLoadProgressApi_t */
  {
    CShell::NW_UI_StartContentLoad,
    CShell::NW_UI_StopContentLoad,
    CShell::NW_UI_UrlLoadProgressOn
  },

  /* NW_EvLogApi_t event log API*/
  {
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL
  },
  /* NW_AppServices_SoftKeyAPI_t */
  {
    CShell::NW_UI_SoftKey_UpdateSoftKeys,
    CShell::NW_UI_SoftKey_UpdateSoftKeys
  },
  /* NW_UserRedirectionApi_t */
  {
    CShell::NW_UI_GetUserRedirectionData
  },
  
  /* NW_AppServices_SelectListAPI_t */
  {
	NULL
  },

  /* NW_Scr_SuspResQueryApi_t */
  {
  NULL
  },

  /* NW_AppServices_BrowserAppAPI_t */
  {
  NULL,
  CShell::NW_UI_SwitchToImgMapView,
  CShell::NW_UI_SwitchFromImgMapViewIfNeeded,
  CShell::NW_UI_IsImageMapView
  },

  /* NW_AppServices_ImageViewAPI_t */
  {
  CShell::NW_UI_ViewImage
  },
  
  /* NW_AppServices_GetLocalizedStringAPI_t */
  {
    CShell::NW_UI_GetLocalizedString
  },

  /* NW_AppServices_CharacterConversionAPI_t Character Conversion Library */
  {
      CShell::NW_UI_ConvertFromForeignCharSet,
      CShell::NW_UI_ConvertToForeignCharSet,
      CShell::NW_UI_ConvertFromForeignChunk
  },
  
  /* NW_AppServices_EcmaScriptPI_t */
  {
    CShell::WindowAlert,
    CShell::WindowConfirm,
    CShell::WindowPrompt,
    CShell::WindowOpen
  },
  
  /* NW_AppServices_FormBox_t */
  {
    CView::InputInvalid
  },
  
  /* NW_AppServices_SecurityNotes_t */
  {
	NULL
    //CShell::NW_UI_AboutToLoadPageL
  },

  /* NW_AppServices_ObjectDialog_t */
  {
    CShell::NW_UI_ShowObjectDialog
  },
};


#ifdef __cplusplus
} // extern "C"
#endif

// LOCAL FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------- 
//    Panic
// ---------------------------------------------------------------------------- 
//lint --e{512} Symbol previously used as static 
GLDEF_C void Panic(const TInt aPanicCode)
{
	_LIT(KCShell, "KM-CShell-CPP");
	User::Panic(KCShell, aPanicCode);
}

LOCAL_C inline TPtrC 
PtrCFromTText (const TText* aText)
  {
  return (aText!=NULL ? TPtrC(aText) : TPtrC());
  }

LOCAL_C inline CShell* 
Shell() 
  {
  CShell* shellInstance = REINTERPRET_CAST(CShell*, (NW_Ctx_Get(NW_CTX_BROWSER_APP, 0)));
  __ASSERT_ALWAYS(shellInstance, Panic(KErrCorrupt));
  return shellInstance;
  }

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

 
TInt CShell::RunBGTask(TAny* /* aShell*/)
    {
    User::Heap().Compress();
    return KErrNone;
    }

// --------------------------------------------------------------------------------
// CShell::CShell
// -----------------------------------------------------------------------------
CShell::CShell(CView* aEpoc32View, CBrCtl* aBrCtl) : 
               iView(aEpoc32View), iBrCtl(aBrCtl)
    {
        iIsEpoc32ShellDestroyed = EFalse;
        iContentLoader = NULL;
        iCharacterSetConverter = NULL;
        iArrayOfCharacterSetsAvailable = NULL;
        iCurrentConverter = 0;
        iSelectedCharacterSet = 0;
   			//iRefCount = 0;
        iDocExitObserver = NULL;
        iSecureItemsInNonSecurePage = EFalse;
        iNonSecureItemsInSecurePage = EFalse;
        SetView(aEpoc32View);
    }

// ----------------------------------------------------------------------------
// CShell::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
void CShell::ConstructL (TBool /*aUseWsp*/ , MWmlInterface* aWKWmlInterface, CWmlControl* aWmlControl )
    {
   // Start logging service
  NW_LOG_NEWLOGGER ("browser_core.log", NW_LOG_LEVEL4);

   // If this is the first instance of the class that we are constructing, we
   // must first initialize the class
  /*lint -e{64} Type mismatch*/
  TBrowserStatusCode status = InitializeClass (iHWnd);
  User::LeaveIfError( status );
  iWmlControl = aWmlControl;
   // Get the constructor parameters
  NW_HED_MimeTable_t* mimeTable = NW_HED_MimeTableOf (&NW_Browser_MimeTable);
  void* browserAppCtx = (void*) this;

   // Set the browserAppCtx into global context array
  status = NW_Ctx_Set (NW_CTX_BROWSER_APP, 0, browserAppCtx);
  User::LeaveIfError( status );

  // Set UserAgent string
  void* userAgent = CUserAgent::NewL();
  status = NW_Ctx_Set (NW_CTX_USER_AGENT, 0, userAgent);
  User::LeaveIfError( status );
  
  // Set Content Manager  as it is required
  // for decoding the DRM images in global array context.
  using namespace ContentAccess;
  ContentAccess::CManager* manager = CManager::NewLC();
  User::LeaveIfNull(manager);
  status = NW_Ctx_Set (NW_CTX_CONT_ACCESS_MGR, 0, (void*) manager);
  User::LeaveIfError( status );
  CleanupStack::Pop(); //Manager

   // Create the DocumentRoot */
  iDocumentRoot = NW_HED_DocumentRoot_New (mimeTable, 
                                           const_cast<NW_HED_AppServices_t*>(&appServices), 
                                           browserAppCtx);

  iDisplayDocumentInvoked= NW_FALSE;
  // Leaves with KErrNoMemory if shell could not be instantiated
  User::LeaveIfNull( iDocumentRoot );

   // Install ourselves as the listener on the DocumentRoot */
  status = NW_HED_DocumentRoot_SetDocumentListener (iDocumentRoot, this);
  NW_ASSERT (status == KBrsrSuccess);

#ifdef NEEDS_SYMBIAN_IMPLEMENTATION
  NW_Msg_MessageDispatcher_SetBrowserState(&NW_Msg_MessageDispatcher, NW_TRUE);
#endif
  
  status = InitComponents();

  User::LeaveIfError( iRfs.Connect() );
  iContentLoader = new (ELeave) CContentLoader( CEikonEnv::Static()->Process() );

  if( !iArrayOfCharacterSetsAvailable )
    {
    iArrayOfCharacterSetsAvailable = 
        CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL( iRfs );
    User::LeaveIfNull( iArrayOfCharacterSetsAvailable );
    }

  TUint uid( 0 );
   // Get the current encoding value
  NW_Uint16 encoding = NW_Settings_GetEncoding();    
  uid = GetCharsetUid(encoding);
    
   // If CharacterSetChangedL leaves then we use EISO8859_1 by default
  CharacterSetChangedL( uid );


   // Inititalize the UrlLoader (HttpLoader, FileLoader)
   UrlLoader_InitializeL(iBrCtl->brCtlSpecialLoadObserver(), this, aWKWmlInterface );
  iIdle = CIdle::NewL(CActive::EPriorityIdle);
  }

// --------------------------------------------------------------------------------
// CShell::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------------
CShell* 
CShell::NewL (CView* aEpoc32View, CBrCtl* aBrCtl, MWmlInterface* aWKWmlInterface, CWmlControl* aWmlControl)
    {
    CShell* self = new (ELeave) CShell(aEpoc32View, aBrCtl);
    CleanupStack::PushL(self);
    self->ConstructL(EFalse, aWKWmlInterface, aWmlControl );
    CleanupStack::Pop(); //self
    return self;
    }

// --------------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------------
CShell::~CShell()
  {
  iExiting = ETrue;

    iIsEpoc32ShellDestroyed = ETrue; // Set to true , any calls on this instance from 

  DestroyPreviousDocument();
  NW_Object_Delete(iDocumentRoot);
  CCSSVariableStyleSheet::DeleteSingleton();
  NW_WBXML_Dictionary_destroy();

#ifdef NEEDS_SYMBIAN_IMPLEMENTATION
  NW_Msg_MessageDispatcher_SetBrowserState(&NW_Msg_MessageDispatcher, NW_FALSE);
#endif

    delete iDocExitObserver;
    delete iCharacterSetConverter;
    delete iArrayOfCharacterSetsAvailable;
    delete iContentLoader;
    iRfs.Close();

    UrlLoader_Shutdown();
    if(iIdle)
        {
        iIdle->Cancel();
        }
    delete iIdle;
  // remove UserAgent
  CUserAgent*  userAgent = (CUserAgent*) NW_Ctx_Get(NW_CTX_USER_AGENT, 0);
  NW_ASSERT( userAgent != NULL );
  delete userAgent;
  NW_Ctx_Set(NW_CTX_USER_AGENT, 0, NULL);

  /* MSR: Added typecast to _NW_WaeUsrAgent_t*) */
  NW_WaeUsrAgent_t  *wae = (_NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);
  NW_ASSERT( wae != NULL );
  /* free useAgent */ 
  NW_WaeUsrAgent_Free(wae);
  NW_Ctx_Set(NW_CTX_WML_CORE, 0, NULL);

    NW_Ctx_Set (NW_CTX_BROWSER_APP, 0, NULL);
  /*Free Content Manager */

  using namespace ContentAccess;
  ContentAccess::CManager* manager = (ContentAccess::CManager*) NW_Ctx_Get(NW_CTX_CONT_ACCESS_MGR,0);
  delete manager;
  NW_Ctx_Set(NW_CTX_CONT_ACCESS_MGR, 0, NULL);
    }

// --------------------------------------------------------------------------------
// CShell::InitComponents
// --------------------------------------------------------------------------------
TBrowserStatusCode
CShell::InitComponents ()
    {
    NW_WaeUsrAgent_t  *wae = NULL;
    TBrowserStatusCode status = KBrsrFailure;

    wae = (NW_WaeUsrAgent_t  *)NW_Ctx_Get(NW_CTX_WML_CORE, 0);
    if (wae == NULL)
        {
        wae = NW_WaeUsrAgent_New();
        if (wae != NULL)
            {
            // Stick the context in the context manager
            status = NW_Ctx_Set(NW_CTX_WML_CORE, 0, wae);
            }
        else
            {
            return KBrsrOutOfMemory;
            }
        }

    // Added typecast to _NW_WaeUsrAgent_t*)
    wae = (_NW_WaeUsrAgent_t*) NW_Ctx_Get(NW_CTX_WML_CORE, 0);
    NW_ASSERT(wae != NULL);

    // Register error handlers
    status = NW_WaeUsrAgent_RegisterError(wae, &(iDocumentRoot->appServices->errorApi));
    if (status != KBrsrSuccess)
        {
        return status;
        }

    // Register load progress handlers
    status = NW_WaeUsrAgent_RegisterLoadProgress(wae, &(iDocumentRoot->appServices->loadProgress_api));
    if (status != KBrsrSuccess)
        {
        return status;
        }
  
    status = NW_WaeUsrAgent_RegisterGenDlg(wae, &(iDocumentRoot->appServices->genDlgApi));
        if (status != KBrsrSuccess)
        {
        return status;
        }

    // Register event log API
    status = NW_WaeUsrAgent_RegisterEvLog(wae, &(iDocumentRoot->appServices->evLogApi));
    if (status != KBrsrSuccess)
        {
        return status;
        }

    status = NW_WaeUsrAgent_RegisterUserRedirection(wae, &(iDocumentRoot->appServices->userRedirection));
    if (status != KBrsrSuccess)
        {
        return status;
        }

    // Plugin support
    if (iBrCtl->capabilities() & TBrCtlDefs::ECapabilityDisableInputAndPlugins)
        {
        // Disable the plugin support
        //R->embed
        //TRAP(status, CPluginHandler::CreateSingletonL(EFalse));
        }
    else
        {
        // Plugins are supported
        //R->embed
        //TRAP(status, CPluginHandler::CreateSingletonL(ETrue));
        }

    if (status != KBrsrSuccess)
        {
        return status;
        }

    return KBrsrSuccess;
    }

// ----------------------------------------------------------------------------
// CShell::InitComponents
// ----------------------------------------------------------------------------
TBrowserStatusCode
CShell::InitializeClass (const NW_Osu_Hwnd_t hWnd)
{
    NW_TRY (status)
        {
    NW_Uint32 numDictionaries;

    NW_REQUIRED_PARAM (hWnd);

        // Count how many WBXML dictionaries there are and install them */
    for (numDictionaries = 0;
         ShellWBXMLDictionaries[numDictionaries] != NULL;
             numDictionaries += 1)
            {
      }

        // Initialize the cXML dictionaries */
        status = StatusCodeConvert(NW_WBXML_Dictionary_initialize(numDictionaries,
                                      (NW_WBXML_Dictionary_t**) ShellWBXMLDictionaries));
    NW_THROW_ON_ERROR (status);
  }
    NW_CATCH(status)
        {
        }
    NW_FINALLY
        {
    return status;
        }
    NW_END_TRY
}

// --------------------------------------------------------------------------------
// CShell::LoseFocus
// --------------------------------------------------------------------------------
void CShell::LoseFocus ()
{
  NW_HED_DocumentNode_LoseFocus (iDocumentRoot);
}

// --------------------------------------------------------------------------------
// CShell::GainFocus
// --------------------------------------------------------------------------------
void CShell::GainFocus ()
{
  NW_HED_DocumentNode_GainFocus (iDocumentRoot);

  /* Tell the document root explicity; there is a case when
   * loading a saved page with animated images the timing was
   * such that it would not be set, and the animation wouldn't
   * happen.
   */
  if (iView != NULL && iView->GetRootBox() != NULL)
    NW_LMgr_RootBox_GainFocus(iView->GetRootBox());
}

// --------------------------------------------------------------------------------
// CShell::RestoreDocument
// --------------------------------------------------------------------------------
void
CShell::RestoreDocument ()
{
  TBrowserStatusCode status;
  NW_HED_HistoryEntry_t* entry;

  /* reset partial rendering counter */
  iNumUpdatesPartial = -1;

  /* can't restore something that doesn't exist ... */
  if (iPrevDocument != NULL) 
    {
    /* don't resume the existing document tree */
    if( iDocumentRoot->childNode == iPrevDocument ) 
      {
      return;
      }
    /* delete the existing childNode, if any */
    NW_Object_Delete( iDocumentRoot->childNode );
    iDocumentRoot->childNode = NULL;

    /* set the childNode */
    iDocumentRoot->childNode = iPrevDocument;
  
    /* scynchronize the history */ //R->history
    /* 
    ** if the entry is found in the stack set it as the current entry.  If not,
    ** then insert the entry at the beginning of the stack.  Insertion sets it
    ** as the new current entry.
    */
    entry = NW_HED_ContentHandler_GetAssociatedHistoryEntry( iDocumentRoot->childNode );
    status = NW_HED_HistoryStack_SetCurrentEntry( iDocumentRoot->historyStack, entry );
    if( status != KBrsrSuccess) 
      {
      status = NW_HED_HistoryStack_InsertEntry( iDocumentRoot->historyStack, entry,
				         NW_HED_HistoryStack_Loacation_AtBeginning );
      }

    /* resume the document root to cause it to reactivate */
    NW_HED_DocumentNode_Resume( iDocumentRoot );

    iPrevDocument = NULL;
  }
}

/* -------------------------------------------------------------------------
 * partialRender will be true for all partial render request; and false for 
 * non-partial rendering request; markupComplete is only useful for partial
 * rendering page.
 */
// this functions come from old Epoc32Shell.cpp
TBrowserStatusCode
CShell::DisplayDocument ()
    {
    NW_LMgr_Box_t* boxTree = NULL;
    NW_Ucs2* storage = NULL;
    NW_HED_HistoryEntry_t* entry;
    NW_HED_HistoryVisitor_t visitor;
	NW_HED_HistoryStack_t* history;
    const NW_Text_t* title;
    static const NW_Ucs2 empty[] = { '\0' };
    NW_GDI_Point2D_t docPosition;
    NW_LMgr_RootBox_t* rootBox;
    
    NW_TRY (status) 
        {
		history = NW_HED_DocumentRoot_GetHistoryStack (iDocumentRoot);    
		if (history != NULL) 
			{
			SetUrlRequestReason(history, NW_HED_UrlRequest_Reason_DocLoad);
            SetPossibleWmlOEB(history, NW_FALSE);
			}

        rootBox = iView->GetRootBox();
        NW_THROW_OOM_ON_NULL( rootBox, status );
        
        if(!iDisplayDocumentInvoked )
            {
            iDisplayDocumentInvoked = NW_TRUE;
            status =
                NW_HED_DocumentNode_GetBoxTree (iDocumentRoot, &boxTree);
            iDisplayDocumentInvoked = NW_FALSE;
            NW_THROW_OOM_ON_NULL (boxTree, status);
            
            /* query the document root for the box tree */
            
            /* update the view with the new box tree */
            status = SetBoxTree (boxTree);
            NW_THROW_ON_ERROR (status);
            boxTree = NULL;
            
            /* do following steps only if non-partial rendering or 
            * the first chunk of parital rendering page
            */
            if( !rootBox->partialRender || ( rootBox->partialRender && 
                rootBox->renderingState == NW_LMgr_RootBox_RenderFirstChunk ) )
                {
                /* get the title from the documentRoot */
                title = NW_HED_DocumentRoot_GetTitle (iDocumentRoot);
                if (title == NULL) 
                    {
                    storage = (NW_Ucs2*) empty;
                    } 
                else 
                    {
                    NW_Uint8 freeNeeded;
                    NW_Text_Length_t length;
                    
                    storage = (NW_Ucs2*)NW_Text_GetUCS2Buffer (title, 0, &length, &freeNeeded);
                    if (!freeNeeded) 
                        {
                        storage = NW_Str_Newcpy (storage);
                        }
                    }
                if(storage == NULL) 
                    {
                    storage = (NW_Ucs2*) empty;
                    }
                
                NW_HED_HistoryVisitor_Initialize(&visitor, iDocumentRoot->historyStack,
                    NW_HED_HistoryVisitor_Loacation_Current ) ;
                
                /* get the entry */ //R->history
                entry = (NW_HED_HistoryEntry_t*) NW_HED_HistoryVisitor_Current (&visitor);
                
                if (entry != NULL)
                    {
                    /* Store the title */
                    status = NW_HED_HistoryEntry_SetPageTitle(entry, storage);
                    NW_THROW_ON_ERROR (status);
                    
                    if (storage != empty) 
                        {
                        NW_Mem_Free (storage);
                        storage=NULL;
                        }
                    
                    /* if there is no title defined, set the storage pointer to NULL */
                    if (title == NULL) 
                        {
                        storage = NULL;
                        }
                    
                    /* update the title */
                    UpdateTitle ();
                    
                    NW_HED_HistoryEntry_GetDocPosition(entry, &docPosition);
                    
                    iView->SetInitialActiveBoxId (NW_HED_HistoryEntry_GetActiveBoxId(entry));
                    if (NW_LMgr_RootBox_IsPageRTL(rootBox))
                        {
                        iView->SetInitialDocPosition (RIGHT_ORIGIN, 0);
                        }
                    else
                        {
                        iView->SetInitialDocPosition (0, 0);
                        }
                    
                    // We read the docPosition from the history entry only if it has a 
                    // different value than the initialization value
                    if ((docPosition.x != 0 && docPosition.x != RIGHT_ORIGIN) || (docPosition.y !=0))
                        {
                        iView->SetInitialDocPosition (docPosition.x, docPosition.y);
                        }
                    } // end of if (entry != NULL)
                } // end of if ( !partialRender || (partialRender && NW_LMgr_RootBox_RenderFirstChunk ) )
            
            /* Notify the document that it has been displayed.  This is necessary to
            support features like XHTML META refresh. */
            if ( rootBox->renderingState == NW_LMgr_RootBox_RenderComplete )
                {
                status = NW_HED_DocumentRoot_DocumentDisplayed (iDocumentRoot);
                NW_THROW_ON_ERROR (status);
                }
            }
    
    }
    NW_CATCH (status) 
      {
      NW_Object_Delete (boxTree);
      RestoreDocument ();
      }
    NW_FINALLY 
      {
      return status;
      }
    NW_END_TRY
    }

/* ------------------------------------------------------------------------- */
NW_Text_t*
CShell::ConvertToValidUrl (NW_Text_t* url,
                           TBrowserStatusCode* status)
{
  NW_Text_t* textUrl = NULL;
  NW_Ucs2* tempUrl = NULL;
  NW_Ucs2* trimmedEscapedUrl = NULL;
  NW_Ucs2* storage = NULL;
  NW_Url_Schema_t scheme;
  NW_Uint32 len = 0;

  *status = KBrsrOutOfMemory;

  if(url == NULL) {
    return url;
  }

  storage = (NW_Ucs2*)NW_Text_GetUCS2Buffer (url, 0, NULL, NULL);

  if(storage == NULL) {
    return NULL;
  }

  trimmedEscapedUrl = NW_Str_Trim(storage, NW_Str_Both);
  if(trimmedEscapedUrl == NULL) {
    return NULL;
  }

  len = (NW_Uint32)NW_Url_EscapeUnwiseLen(trimmedEscapedUrl);

  tempUrl = NW_Str_New(len);
  if(tempUrl == NULL) {
    NW_Str_Delete(trimmedEscapedUrl);
    tempUrl = NULL;
    return NULL;
  }
  NW_Url_EscapeUnwise(trimmedEscapedUrl, tempUrl);

  NW_Str_Delete(trimmedEscapedUrl);
  trimmedEscapedUrl = tempUrl;

  tempUrl = NULL;

  /* check if we have a valid scheme */
  *status = NW_Url_GetScheme(trimmedEscapedUrl, &scheme);
  if ((*status == KBrsrOutOfMemory) || (*status == KBrsrMalformedUrl))
  {
    NW_Str_Delete (trimmedEscapedUrl);
    return NULL;
  }

  if (*status != KBrsrSuccess) {
      /* try to build a new url with HTTP as the default scheme */
    *status = NW_Url_BuildUrl(trimmedEscapedUrl, NW_SCHEMA_HTTP, &tempUrl);
  }

  if(*status == KBrsrSuccess) {
    textUrl = (NW_Text_t*)
      NW_Text_UCS2_New(((tempUrl != NULL) ? tempUrl : trimmedEscapedUrl), 0, NW_Text_Flags_TakeOwnership);
    if(textUrl == NULL) {
      return textUrl;
    }
    if (tempUrl != NULL){
      NW_Str_Delete (trimmedEscapedUrl);
    }
  } else {
    NW_Str_Delete (trimmedEscapedUrl);
  }

  return NW_TextOf (textUrl);
}


// aOwner and aLoadRecipient exist exclusively
/* ------------------------------------------------------------------------- */
TBrowserStatusCode CShell::StartDataLoad (TDesC& aUrl, CDataLoadInfo* aDataLoadInfo,
                                          NW_MVC_ILoadListener_t* loadListener,
                                          TUint16* aTransId,
                                          void* aOwner,
                                          void* aClientData,
                                          void* aLoadRecipient, 
                                          TUint8 aLoadType
                                          )
    {
    NW_HED_UrlRequest_t* urlRequest = NULL;
    NW_Text_t* textUrl = NULL;
    TBrowserStatusCode status = KBrsrSuccess;
    textUrl  = NW_Text_New (HTTP_iso_10646_ucs_2, (void*) aUrl.Ptr(), 
        (NW_Text_Length_t)aUrl.Length(), NW_FALSE);

    if (textUrl == NULL)
      return KErrNoMemory;

    (void) SwitchFromImgMapViewIfNeeded();
    NW_ASSERT (loadListener == NULL || NW_Object_IsInstanceOf (loadListener,
        &NW_MVC_ILoadListener_Class));
    /* if there is already a NW_MVC_ILoadListener installed, we must notify it
    that its outstanding request is being cancelled */
    if (iLoadListener != NULL) 
        {
        NW_MVC_ILoadListener_LoadComplete (iLoadListener, NW_FALSE);
        }
    iLoadListener = loadListener;

    // if multipart page sends a request with DocRoot as owner, it's a top level request
    // or multipart page css send request
    if( (aOwner && !NW_Object_IsInstanceOf( aOwner, &NW_HED_DocumentRoot_Class )) ||
        aLoadRecipient )
        {
        urlRequest = NW_HED_UrlRequest_New (textUrl, NW_URL_METHOD_DATA, NULL,
            NULL, NW_HED_UrlRequest_Reason_DocLoadChild, NW_HED_UrlRequest_LoadNormal,
            aLoadType );
        }
    else
        {
        urlRequest = NW_HED_UrlRequest_New (textUrl, NW_URL_METHOD_DATA, NULL,
            NULL, NW_HED_UrlRequest_Reason_ShellLoad, NW_HED_UrlRequest_LoadNormal,
            NW_UrlRequest_Type_Any );
        }

    if (urlRequest == NULL)
        {
        status = KBrsrOutOfMemory; 
        }
    else
        {
        NW_HED_UrlRequest_SetCacheMode (urlRequest, NW_CACHE_NORMAL);
    
        /* issue the load request */
        if( aOwner )
            {
            NW_ASSERT (NW_Object_IsInstanceOf (aOwner, &NW_HED_DocumentNode_Class));

            status = NW_HED_DocumentRoot_StartRequest_tId (iDocumentRoot,
                (NW_HED_DocumentNode_t*)aOwner, urlRequest,
                aClientData, aTransId, (void*) aDataLoadInfo );
            }
        else if( aLoadRecipient )
            {
            status = NW_HED_DocumentRoot_StartRequest_tId2 (iDocumentRoot,
                urlRequest, aLoadRecipient, 
                aClientData, aTransId, (void*) aDataLoadInfo );
            }
        else
            {
            status = NW_HED_DocumentRoot_StartRequest_tId (iDocumentRoot,
                NW_HED_DocumentNodeOf (iDocumentRoot), urlRequest,
                NULL, aTransId, (void*) aDataLoadInfo );
            }
        }

    if (status != KErrNone)
        {
        NW_Object_Delete (urlRequest);
        DisplayError (BRSR_STAT_CLASS_GENERAL, status);
        }
    NW_Object_Delete(textUrl);
    return status;
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode CShell::PostUrl(const TDesC& aUrl,
    const TDesC8& aContentType, const TDesC8& aPostData, 
    const TDesC8* aBoundary, void* /*aReserved*/)
    {
    HBufC8* typeBuf = NULL;
    HBufC8* boundaryBuf = NULL;
    typeBuf = HBufC8::New(aContentType.Length() + 1);
    if ((aBoundary != NULL) && (aBoundary->Length() > 0))
        {
        boundaryBuf = HBufC8::New(aBoundary->Length() + 1);
        }
    // Users do not type POST urls, there is no need to validate the scheme
    NW_HED_UrlRequest_t* urlRequest = NULL;
    NW_Text_t* textUrl = NULL;
    TBrowserStatusCode status = KBrsrSuccess;
    textUrl  = NW_Text_New (HTTP_iso_10646_ucs_2, (void*) aUrl.Ptr(), 
        (NW_Text_Length_t)aUrl.Length(), NW_FALSE);

    if (textUrl == NULL || typeBuf == NULL || (aBoundary != NULL && aBoundary->Length() > 0 && boundaryBuf == NULL))
        {
        delete typeBuf;
        delete boundaryBuf;
        NW_Object_Delete(textUrl);
        return KErrNoMemory;
        }
    typeBuf->Des().Copy(aContentType);
    typeBuf->Des().ZeroTerminate();
    if (boundaryBuf)
        {
        boundaryBuf->Des().Copy(*aBoundary);
        boundaryBuf->Des().ZeroTerminate();
        }

    (void) SwitchFromImgMapViewIfNeeded();
    /* if there is already a NW_MVC_ILoadListener installed, we must notify it
    that its outstanding request is being cancelled */
    if (iLoadListener != NULL) 
        {
        NW_MVC_ILoadListener_LoadComplete (iLoadListener, NW_FALSE);
        }
    iLoadListener = NULL;

    void* headers = UrlLoader_HeadersNew(NULL, typeBuf->Ptr(), NULL, boundaryBuf == NULL ? NULL : boundaryBuf->Ptr(), aPostData.Length(), NULL);
    if (headers == NULL)
        {
        status = KErrNoMemory;
        }
    if (status == KBrsrSuccess)
        {   
        NW_Buffer_t body;
        body.data = (NW_Uint8*)aPostData.Ptr();
        body.length = aPostData.Length();
        body.allocatedLength = aPostData.Length();
        
        urlRequest = NW_HED_UrlRequest_New (textUrl, NW_URL_METHOD_POST, headers,
            &body, NW_HED_UrlRequest_Reason_ShellLoad, NW_HED_UrlRequest_LoadNormal,
            NW_UrlRequest_Type_Any );
        if (urlRequest == NULL)
            {
            status = KErrNoMemory;
            }
        else
            {
            NW_HED_UrlRequest_SetCacheMode (urlRequest, NW_CACHE_NOCACHE);
            NW_Uint16 transId;
    
            status = NW_HED_DocumentRoot_StartRequest_tId (iDocumentRoot,
                NW_HED_DocumentNodeOf (iDocumentRoot), urlRequest,
                NULL, &transId, NULL);
            }
        }

    if (status != KErrNone)
        {
        NW_Object_Delete (urlRequest);
        DisplayError (BRSR_STAT_CLASS_GENERAL, status);
        }
    delete typeBuf;
    delete boundaryBuf;
    NW_Object_Delete(textUrl);
    return status;
    }


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CShell::StartLoad (const NW_Text_t* url,
                   NW_MVC_ILoadListener_t* loadListener,
						       NW_Cache_Mode_t cacheMode)
{
  TBrowserStatusCode statusCode;
  NW_Text_t* textUrl = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (url, &NW_Text_Abstract_Class));
  NW_ASSERT (loadListener == NULL ||
             NW_Object_IsInstanceOf (loadListener,
                                     &NW_MVC_ILoadListener_Class));

  NW_TRY (status) {
    /* if there is already a NW_MVC_ILoadListener installed, we must notify it
       that its outstanding request is being cancelled */
    if (iLoadListener != NULL) {
      NW_MVC_ILoadListener_LoadComplete (iLoadListener, NW_FALSE);
    }
    iLoadListener = loadListener;

    /* normalize the URL */
    textUrl = ConvertToValidUrl(const_cast<NW_Text_t*>(url), &statusCode);
    if(textUrl == NULL) {
//      status = KBrsrOutOfMemory;
      return statusCode;
    }

    /* ask for the new stuff! */
    status =
      NW_HED_DocumentRoot_StartLoad (iDocumentRoot,
                                     NW_HED_DocumentNodeOf (iDocumentRoot),
                                     textUrl,
                                     NW_HED_UrlRequest_Reason_ShellLoad, NULL,
                                     NW_UrlRequest_Type_Any, cacheMode);
    _NW_THROW_ON_ERROR (status);
  }

  NW_CATCH (status) {
    DisplayError (BRSR_STAT_CLASS_GENERAL, status);
  }

  NW_FINALLY {
    if(textUrl != url) {
      NW_Object_Delete (textUrl);
    }

    return status;
  } NW_END_TRY
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CShell::GetUseNumberContent (NW_ADT_DynamicVector_t *dynamicVector)
{
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicVector, &NW_ADT_DynamicVector_Class));

  if(dynamicVector == NULL)
  {
    return KBrsrFailure;
  }

  return NW_HED_DocumentRoot_CollectNumbers(iDocumentRoot, dynamicVector);
}

/* ------------------------------------------------------------------------- */
NW_Text_t*
CShell::GetCurrentUrl ()
    {
    NW_Text_t* text = NULL;
	int ret = KErrNone;
    TRAP(ret, text = GetCurrentUrlL());
    return text;
    }

/* ------------------------------------------------------------------------- */
NW_Text_t*
CShell::GetCurrentUrlL ()
{
  TUriParser16 uriParser;
  const TText* linkUrl = NULL;
  NW_Text_t* textUrl = NULL;

  NW_Text_t* url = (NW_Text_t*) NW_HED_DocumentRoot_GetURL (iDocumentRoot);

  if (url)
    {
    linkUrl = NW_Text_GetUCS2Buffer(url, NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned,
                                    NULL, NULL);

    TPtrC urlDes (linkUrl, User::StringLength(linkUrl));

    User::LeaveIfError(uriParser.Parse(urlDes));

    if ( uriParser.IsPresent( EUriUserinfo ) )
      {
      // Remove the username and password from the URI
      CUri16* newUri = CUri16::NewLC( uriParser );

      newUri->RemoveComponentL( EUriUserinfo );
    
      const TText* newUrl = newUri->Uri().UriDes().Ptr();

      textUrl = NW_Text_New(HTTP_iso_10646_ucs_2, (void*)newUrl, newUri->Uri().UriDes().Length(), 
                        NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy);

      CleanupStack::PopAndDestroy( newUri ); // newUri
      }
    else
      {
       textUrl = NW_Text_Copy(url, NW_TRUE);
      }
    }
  return textUrl;
}

/* ------------------------------------------------------------------------- */
const NW_Text_t*
CShell::GetCurrentTitle ()
{
  return (NW_Text_t*) NW_HED_DocumentRoot_GetTitle (iDocumentRoot);
}

/* ------------------------------------------------------------------------- */
void
CShell::DisplayError (NW_Int16 errorClass,
                      NW_WaeError_t error)
{
  TInt err;  
  TRAP(err,  iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventLoadError, 0, 0));
  TRAP(err,  iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventContentFinished, 0, 0));

  switch (errorClass) {
    case BRSR_STAT_CLASS_HTTP: {
      NW_HED_HistoryStack_t* history; //R->history
      const NW_Ucs2* url = NULL;

      history = NW_HED_DocumentRoot_GetHistoryStack (iDocumentRoot);
      if (history != NULL) {
        NW_HED_HistoryEntry_t* entry;

        entry = NW_HED_HistoryStack_GetEntry (history, NW_HED_HistoryStack_Direction_Current);
        if (entry != NULL) {
          const NW_HED_UrlRequest_t* urlRequest;

          urlRequest = NW_HED_HistoryEntry_GetUrlRequest (entry);
          if (urlRequest != NULL) {
            url = NW_HED_UrlRequest_GetRawUrl (urlRequest);
          }
        }
      }

      (void) iDocumentRoot->appServices->errorApi.
        httpError (iDocumentRoot, (NW_WaeHttpStatus_t) error, url, NULL, NULL);
      break;
    }

    case BRSR_STAT_CLASS_GENERAL:
      if (error != KBrsrCancelled &&
          error != KBrsrHedContentDispatched) {
        (void) iDocumentRoot->appServices->errorApi.
          notifyError (iDocumentRoot, (TBrowserStatusCode) error, NULL, NULL);
      }
      break;

    default:
      NW_ASSERT (0);
  }
}

// ---------------------------------------------------------------------------- 
// MHEDDocumentListener method implementations
// ---------------------------------------------------------------------------- 
void
CShell::NewDocument (NW_HED_DocumentRoot_t* documentRoot,
                     NW_HED_UrlRequest_t* urlRequest,
                     NW_Bool aDeleteBoxTree)
{
  NW_HED_HistoryStack_t* history;

  /* avoid 'unreferenced formal parameter' warnings */
  (void) documentRoot;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentRoot,
				     &NW_HED_DocumentRoot_Class));

  /* reset partial rendering counter */
  iNumUpdatesPartial = 0;

  // reset the flag
  iFileOpenedByDocHandler = EFalse;

  // Delete the existing previous document (content handler), unless it is the
  // same as the current child node (content handler).  This happens when
  // multipart/mixed is dispatched (see NW_HED_DocumentRoot_CreateContentHandler).
  // We need the previous document (content handler) to go back and forth from
  // the multipart calling page and the dispatched content.
  if ((iPrevDocument != NULL) &&
	  (iPrevDocument != iDocumentRoot->childNode)) 
    {
    DestroyPreviousDocument();
  }
  //R->history
  history = NW_HED_DocumentRoot_GetHistoryStack (iDocumentRoot);
  if (history != NULL) 
  {
    NW_HED_HistoryEntry_t* entry;

    entry = NW_HED_HistoryStack_GetEntry (history, NW_HED_HistoryStack_Direction_Current);
    if (entry != NULL) 
    {
      NW_GDI_Point2D_t docPosition;
      NW_ADT_Vector_Metric_t activeBoxId;
      NW_Uint8 reason;

      /*lint -e{666} Expression with side effects passed to repeated parameter in macro*/
      docPosition = *(iView->GetOrigin());
      activeBoxId = iView->GetActiveBoxId();
      reason = NW_HED_UrlRequest_GetReason(urlRequest);

      if ((reason == NW_HED_UrlRequest_Reason_DocLoad) || 
					(reason == NW_HED_UrlRequest_Reason_ShellLoad) || 
					(reason == NW_HED_UrlRequest_Reason_ShellReload))
      {
        NW_HED_HistoryEntry_SetDocPosition(entry, &docPosition);
        NW_HED_HistoryEntry_SetActiveBoxId(entry, activeBoxId);
      }
    }
  }
  // save current top level content handler unless it is an 
  // unknown content handler. Unknown content handler does not
  // carry any useful information so it isn't worth saving.
  if( iDocumentRoot->childNode  )
    {
    iPrevDocument = iDocumentRoot->childNode;
    }

  // and suspend it.
  NW_HED_DocumentNode_Suspend(iDocumentRoot, aDeleteBoxTree);
}

/* ------------------------------------------------------------------------- */
void CShell::DestroyPreviousDocument()
  {
  //
  if( iPrevDocument )
    {
    // If the previous document was a result of data load, remove it from History.
    // We no-longer have the data and cannot go back to that page
    NW_HED_HistoryEntry_t* entry; //R->history
    entry = NW_HED_ContentHandler_GetAssociatedHistoryEntry( iPrevDocument );
    if (entry != NULL && !NW_HED_HistoryStack_ValidateEntry(entry))
        {
        NW_Int16 i = NW_HED_HistoryStack_GetIndexByEntry (iDocumentRoot->historyStack, entry);
        // NW_HED_HistoryStack_GetIndex returns -1 if it does not find the entry
        if (i != -1)
            {
            NW_HED_HistoryStack_DeleteEntry(iDocumentRoot->historyStack, (NW_Uint8)i);
            }
        }
    NW_Object_Delete (iPrevDocument);
    iPrevDocument = NULL;
    }
  }

/* ------------------------------------------------------------------------- */
NW_Bool
CShell::LoadEnd (NW_HED_DocumentRoot_t* documentRoot,
                                         NW_Int16 errorClass,
                                         NW_WaeError_t error)
{
  NW_LMgr_RootBox_t* rootBox;
  NW_Bool loadSuccessful;

  /* avoid 'unreferenced formal parameter' warnings */
  (void) documentRoot;

  NW_ASSERT (NW_Object_IsInstanceOf (documentRoot,
				     &NW_HED_DocumentRoot_Class));

  /* was the load successful */
  loadSuccessful = (NW_Bool)
    ((errorClass == BRSR_STAT_CLASS_GENERAL && (error == KBrsrSuccess || error == KBrsrHedContentDispatched)) ?
     NW_TRUE : NW_FALSE);

  /* if we have been supplied with a NW_MVC_ILoadListener, we must notify that
     of the load end */
  if (iLoadListener != NULL) 
    {
    NW_MVC_ILoadListener_LoadComplete (iLoadListener, loadSuccessful);
    iLoadListener = NULL;
  }

  if ((errorClass == BRSR_STAT_CLASS_GENERAL) && 
      ((error == KBrsrCancelled) || /* The top level request was cancelled */
       // (error == KBrsrHedContentDispatched) || /* content was dispatched */
       // (error == KBrsrWmlbrowserBadContentType) || /* contenthandler was NULL */
       (error == KBrsrConnCancelled) || /* User pressed NO to ConnNeeded */
       (error == KBrsrConnFailed))) /* Connection failed */

  {
    if (!iDocumentRoot->childNode )
      {
      return NW_FALSE;
      }
  }

  /* if the load was unsuccessful or if the contents were dispatched to 
     through content dispatch server we must restore the old document */
  /*lint -e{774} Boolean within 'right side of || within if' always evaluates to False */
  if( !(errorClass == BRSR_STAT_CLASS_GENERAL && error == KBrsrSuccess) || 
        errorClass == BRSR_STAT_CLASS_HTTP ) 
    {
    // if we failed on the first chunk than restore the previous 
    // document, otherwise let the user check what we have 
    rootBox = iView->GetRootBox();
    NW_ASSERT( rootBox != NULL );
    // NW_LMgr_RootBox_RenderUninitialized means no chunk has been displayed.
    // or in case of non-partial rendering. the pages has not been fully loaded
    if( rootBox->renderingState == NW_LMgr_RootBox_RenderUninitialized )
      {
      RestoreDocument ();
      }
    }
  return NW_FALSE;
}

/* -------------------------------------------------------------------------
 * will be called by non-partial rendering page;
 * for partial rendering page, this will be called when the markup completes 
 * and any content after that
 */
TBrowserStatusCode
CShell::DocumentChanged( NW_HED_DocumentRoot_t* documentRoot,
                         CActive::TPriority aMessagePriority )
{
  NW_Msg_Message_t* message = NULL;

  /* avoid 'unreferenced formal parameter' warnings */
  (void) documentRoot;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentRoot, &NW_HED_DocumentRoot_Class));

  NW_LOG0( NW_LOG_LEVEL2, "CShell::DocumentChanged START" );

  NW_TRY (status) {
    /* not partial rendering (e.g. wml): invalidate the current box tree (if any) */
    if (iNumUpdatesPartial == 0 ||
        iNumUpdatesPartial == -1 )
      {
      status = SetBoxTree (NULL);
      NW_THROW_ON_ERROR (status);
    }

    /* create the update display message */
    message = NW_Msg_Message_New (SHELL_MSG_UPDATEDISPLAY, NULL, NW_FALSE);
    NW_THROW_OOM_ON_NULL (message, status);
    message->data = this;
    NW_Msg_Message_SetPriority( message, aMessagePriority );

    /* dispatch it */
    status = NW_Msg_MessageDispatcher_DispatchMessage (&NW_Msg_MessageDispatcher, message, this);
    NW_THROW_ON_ERROR (status);

    message = NULL;

    /* message successfully sent, increment the counter */
    iNumUpdates += 1;
  }

  NW_CATCH (status) {
    NW_Object_Delete (message);
  }

  NW_FINALLY {
    /* successful completion */
    NW_LOG0( NW_LOG_LEVEL2, "CShell::DocumentChanged END" );
    return status;
  } NW_END_TRY
}

/* -------------------------------------------------------------------------
 * will be called only by partial rendering page before the last chunk of 
 * markup content
 */
TBrowserStatusCode
CShell::DocumentChangedPartial( NW_HED_DocumentRoot_t* documentRoot, 
                                CActive::TPriority aMessagePriority )
{
  NW_Msg_Message_t* message = NULL;

  /* avoid 'unreferenced formal parameter' warnings */
  NW_REQUIRED_PARAM (documentRoot);

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentRoot, &NW_HED_DocumentRoot_Class));

  NW_TRY (status) {
    if (iNumUpdatesPartial == 0) 
    /* the first chunk of partial rendering, invalidate the current box tree (if any); 
	 */
      {
      status = SetBoxTree (NULL);
      // box tree can be invalidated by NW_LMgr_RootBox_InvalidateCache function
      //NW_LMgr_RootBox_InvalidateCache(iView->GetRootBox());
      NW_THROW_ON_ERROR (status);
      }

    /* create the update display partial message */
    message = NW_Msg_Message_New (SHELL_MSG_UPDATEDISPLAYPARTIAL, NULL, NW_FALSE);
    NW_THROW_OOM_ON_NULL (message, status);
    message->data = this;
    NW_Msg_Message_SetPriority( message, aMessagePriority );

    /* dispatch it */
    status = NW_Msg_MessageDispatcher_DispatchMessage (&NW_Msg_MessageDispatcher, message, this);
    NW_THROW_ON_ERROR (status);

    message = NULL;

    /* message successfully sent, increment the counter */
    iNumUpdatesPartial += 1;
  }

  NW_CATCH (status) {
    NW_Object_Delete (message);
  }

  NW_FINALLY {
    /* successful completion */
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
/* the function switches the layout and reformats the boxtree if aReformatBoxTree is NW_TRUE
aVerticalLayout - is NW_TRUE to switch to vertical layout
aVerticalLayout - is NW_FALSE to switch to normal layout
*/
TBrowserStatusCode
CShell::SwitchLayout (NW_Bool aVerticalLayout, NW_Bool aReformatBoxTree)
  {
  TBrowserStatusCode status = KBrsrSuccess;
  
  NW_REQUIRED_PARAM( aVerticalLayout );

  // SwitchVerticalLayout , switches the boxes and formats the box tree
  status = iView->SwitchVerticalLayout(aReformatBoxTree, NW_TRUE);
  return status;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CShell::IsActive (NW_HED_DocumentRoot_t* documentRoot,
                  NW_Bool state)
{
  NW_Text_t* url = NULL;
  const NW_Ucs2* urlName = NULL;
  NW_Text_Length_t length;
  void *browserApp = NULL;

  /* avoid 'unreferenced formal parameter' warnings */
  (void) documentRoot;
  
  /* get the current url */
  url = GetCurrentUrl ();
  if(url != NULL) 
    {
    urlName = NW_Text_GetUCS2Buffer (url, 0, &length, NULL);
  }
  
  /* start/stop the load animation */
  browserApp = NW_Ctx_Get(NW_CTX_BROWSER_APP, 0);
  if (state) 
    {
    (void) CShell::NW_UI_StartContentLoad(browserApp, urlName);
    } 
  else 
    {
    (void) CShell::NW_UI_StopContentLoad(browserApp, urlName);
  }

  // delete the url (memory allocated in GetCurrentUrl)
  NW_Object_Delete(url);

  /* successfull completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CShell::LoadProgressOn (NW_Uint16 transactionId)
  {
  void *browserApp = NULL;
  
  browserApp = NW_Ctx_Get(NW_CTX_BROWSER_APP, 0);

  (void) CShell::NW_UI_UrlLoadProgressOn(browserApp, transactionId);

  return KBrsrSuccess;
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CShell::LoadProgressOff(NW_Uint16 transactionId)
    {
	TInt err;
    TRAP(err, iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventUrlLoadingFinished,
                                         0, (TUint16)(transactionId)));
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
NW_Bool
CShell::ReportError (NW_HED_DocumentRoot_t* documentRoot,
                     NW_Int16 errorClass,
                     NW_WaeError_t error,
                     ReportErrorCallback* callback)
{
  /* display the error */
  DisplayError (errorClass, error);

  /* 
  ** call the callback to complete the process.
  ** 
  ** Note: this default implementation uses the synchronous "Error API" to display
  ** the error.  One could override this method for a particular platform 
  ** to use an asynchronous error reporting mechanism (calling the callback after
  ** the user acknowledges it).
  */
  if (callback != NULL) {
     return (callback) (documentRoot, errorClass, error);
  /* Be careful adding code after this line, since we may already exitted the browser */
  }

  return NW_FALSE; /* we are not exitting browser */
}

/* ------------------------------------------------------------------------- */
NW_LMgr_RootBox_t*
CShell::GetRootBox ()
  {
  NW_LMgr_RootBox_t* rootBox=NULL;
  
    if (iView)
      {
      rootBox = iView->GetRootBox();
      }

  return rootBox;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
CShell::ShowNamedBox (NW_LMgr_Box_t* boxTree,
                      const NW_Text_t* name)
{
  TBrowserStatusCode status;
  NW_LMgr_Box_t* targetBox = NULL;
  NW_LMgr_Box_t* box = NULL;
  NW_LMgr_BoxVisitor_t boxVisitor;
  NW_LMgr_Property_t  prop;

  /* find the named box */
  NW_LMgr_BoxVisitor_Initialize (&boxVisitor, boxTree);
  (void) NW_LMgr_BoxVisitor_SetCurrentBox (&boxVisitor, boxTree);

  while((box = NW_LMgr_BoxVisitor_NextBox (&boxVisitor, NULL)) != NULL) {
    status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_boxName, &prop); 

    if ((status == KBrsrSuccess) && (prop.type == NW_CSS_ValueType_Text)) {
      if (!NW_Text_Compare (name, prop.value.object)) {
        targetBox = box;
        break;
      }
    }
  }
  
  /* show the targetBox */
  if (targetBox != NULL) {
    return SetAnchorName (const_cast<NW_Text_t*>(name));
  }
  else {
    return KBrsrNotFound;
  }
}

// ----------------------------------------------------------------------------
TBrowserStatusCode
CShell::SetAnchorName (NW_Text_t *anchorName)
{
  NW_TRY(status)
  {
	status = iView->SetAnchorName(anchorName);
    _NW_THROW_ON_ERROR(status);
	status = iView->Draw (NW_TRUE);
	_NW_THROW_ON_ERROR(status);
  }
  NW_CATCH (status){
  }
  NW_FINALLY{
    return status;
  }
  NW_END_TRY
}

// ----------------------------------------------------------------------------
void
CShell::SetFileOpenedByDocHandler( TBool aFileOpenedByDocHandler )
    {
    iFileOpenedByDocHandler = aFileOpenedByDocHandler;
    }

/* ------------------------------------------------------------------------- */
/* CShell::SendRedrawMsg
 *
 * Sends a redraw request message to itself.
 *
 * Parameters:
 *  image:     this object
 */
TBrowserStatusCode
CShell::SendRedrawMsg (NW_Bool drawNow)
{
  NW_Msg_Message_t* message = NULL;
  
  NW_TRY (status) {
    /* create the redraw message */
    
    if(drawNow)
      message = NW_Msg_Message_New (SHELL_MSG_REDRAW_NOW, NULL, NW_FALSE);
    else
      message = NW_Msg_Message_New (SHELL_MSG_REDRAW, NULL, NW_FALSE);
    NW_THROW_OOM_ON_NULL (message, status);
    message->data = this;

    /* dispatch it */
    status = NW_Msg_MessageDispatcher_DispatchMessage (&NW_Msg_MessageDispatcher, message, this);
    NW_THROW_ON_ERROR (status);

    message = NULL;

    /* message successfully sent, increment the counter */
    iNumRedraws += 1;
  }

  NW_CATCH (status) {
    NW_Object_Delete (message);
  }

  NW_FINALLY {
    /* successful completion */
    return KBrsrSuccess;
  } NW_END_TRY
}


/* ------------------------------------------------------------------------- */
/* CShell::SendRelayoutMsg
 *
 * Sends a relayout request message to itself.
 *
 * Parameters:
 *  image:     this object
 */
TBrowserStatusCode
CShell::SendRelayoutMsg (NW_Bool createTabList)
{
  NW_Msg_Message_t* message = NULL;

  NW_TRY (status) {
    /* create the redraw message */
    if (createTabList)
      {
      message = NW_Msg_Message_New (SHELL_MSG_RELAYOUT_CREATETAB, NULL, NW_FALSE);
      }
    else
      {
      message = NW_Msg_Message_New (SHELL_MSG_RELAYOUT, NULL, NW_FALSE);
      }
    NW_THROW_OOM_ON_NULL (message, status);
    message->data = this;

    /* dispatch it */
    status = NW_Msg_MessageDispatcher_DispatchMessage (&NW_Msg_MessageDispatcher, message, this);
    NW_THROW_ON_ERROR (status);

    message = NULL;

    /* message successfully sent, increment the counter */
    iNumRelayouts += 1;
  }

  NW_CATCH (status) {
    NW_Object_Delete (message);
  }

  NW_FINALLY {
    /* successful completion */
    return KBrsrSuccess;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
   MSGMessageListener method implementations
 * ------------------------------------------------------------------------- */
void
CShell::HandleMessage (NW_Msg_Message_t* message)
{  
  NW_LMgr_RootBox_t* rootBox;
  NW_Bool createTab = NW_FALSE;

  NW_ASSERT (message != NULL);
  
NW_LOG0(NW_LOG_LEVEL3, "CShell::HandleMessage");

  rootBox = iView->GetRootBox();
  NW_ASSERT( rootBox != NULL );

  switch (NW_Msg_Message_GetMessageId (message)) 
    {
    case SHELL_MSG_UPDATEDISPLAY:
      {
      // The "UPDATEDISPLAY" messages are queued in the message queue.
      // If there is more than one, then we ignore all but the last.
      NW_LOG0(NW_LOG_LEVEL3, "CShell::HandleMessage SHELL_MSG_UPDATEDISPLAY");
      if (--iNumUpdates == 0) 
        {
        // if SHELL_MSG_UPDATEDISPLAY is called while numUpdatesPartial equals to 0 
        // means that the top level content handler does not support partial rendering
        if( iNumUpdatesPartial == 0 ) 
          {
          // reset rendering flags
	        rootBox->partialRender = NW_FALSE;
          rootBox->renderingState = NW_LMgr_RootBox_RenderComplete;
          } 
        // iNumUpdatesPartial == -1 means that the previous document has been restored
        // no need to cary out any partial rendering
        else if( iNumUpdatesPartial == -1 )
          {
          rootBox->partialRender = NW_FALSE;
          rootBox->renderingState = NW_LMgr_RootBox_RenderComplete;
          iNumUpdatesPartial = 0;
          }
        // partial rendering just finished, adjust flag related to partial rendering 
        else //if( iNumUpdatesPartial != 0 )
          {
          rootBox->partialRender = NW_TRUE;
  	      rootBox->renderingState = NW_LMgr_RootBox_RenderComplete;
          iNumUpdatesPartial = 0;
          }

        // set priority
        if( iView )
          {
          //
          iView->SetFormattingPriority( NW_Msg_Message_GetPriority( message ) );
          }
        (void) DisplayDocument ();
      }
      break;
      }
    case SHELL_MSG_UPDATEDISPLAYPARTIAL:
      {
      // The "UPDATEDISPLAYPARTIAL" messages are queued in the message queue.
      NW_LOG0(NW_LOG_LEVEL3, "CShell::HandleMessage SHELL_MSG_UPDATEDISPLAY");

      if( iNumUpdatesPartial == 1 ) 
      // the first chunk of partial rendering, adjust partial rendering related flags
        {
         rootBox->partialRender = NW_TRUE;
         rootBox->renderingState = NW_LMgr_RootBox_RenderFirstChunk;
        } 
      else if( rootBox->renderingState == NW_LMgr_RootBox_RenderFirstChunk )
      // the middle chunks of partial rendering, adjust partial rendering related flags */
        {
        rootBox->renderingState = NW_LMgr_RootBox_RenderFurtherChunks;
        }
      // set priority
      if( iView )
        {
        //
        iView->SetFormattingPriority( NW_Msg_Message_GetPriority( message ) );
        }
      (void) DisplayDocument ();
      break;
      }
    case SHELL_MSG_RELAYOUT_CREATETAB:
      createTab = NW_TRUE;
    //lint -fallthrough
    case SHELL_MSG_RELAYOUT:
      // The "RELAYOUT" messages are queued in the message queue.
      // If there is more than one, then we ignore all but the last.
      // Also, no relayout is done if there is a pending "UPDATEDISPLAY".
      if (--iNumRelayouts == 0 && iNumUpdates == 0) 
        {
        TBrowserStatusCode status;

        if (iView != NULL) 
          {
          status = iView->Relayout(createTab);
          if (status == KBrsrSuccess){
            SendRedrawMsg(NW_FALSE);
          }
        }
      }
      break;
    case SHELL_MSG_REDRAW_NOW:
      // The "DRAW" messages are queued in the message queue.
      // If there is more than one, then we ignore all but the last.
      // Also, no relayout is done if there is a pending "UPDATEDISPLAY"
      // or "RELAYOUT".
      if (--iNumRedraws == 0 && iNumUpdates == 0 && iNumRelayouts == 0) 
        {
        if (iView != NULL) 
          {
          if (iView->GetRootBox() != NULL)
            {
            NW_LMgr_RootBox_InvalidateCache(iView->GetRootBox());
            }
          (void) iView->Draw(NW_TRUE);
        }
      }
      break;

    case SHELL_MSG_REDRAW:
      // The "REDRAW" messages are queued in the message queue.
      // If there is more than one, then we ignore all but the last.
      // Also, no relayout is done if there is a pending "UPDATEDISPLAY"
      // or "RELAYOUT".
      if (--iNumRedraws == 0 && iNumUpdates == 0 && iNumRelayouts == 0) 
        {
        if (iView != NULL) 
          {
          if (iView->GetRootBox() != NULL)
            {
            NW_LMgr_RootBox_InvalidateCache(iView->GetRootBox());
            }
          (void) iView->Draw(NW_FALSE);
        }
      }
      break;

    default:
      break;
  }

  // delete the message
  NW_Object_Delete (message);
}

/* ------------------------------------------------------------------------- *
   NW_MVC_IViewListener methods
 * ------------------------------------------------------------------------- */
void
CShell::HandleError (TBrowserStatusCode status)
{
  // display the error and reset the previous document if necessary
  DisplayError (BRSR_STAT_CLASS_GENERAL, status);
  RestoreDocument ();
}

/* ------------------------------------------------------------------------- */
void
CShell::FormattingComplete()
  {
  TBrowserStatusCode status = KBrsrSuccess; //R->ecma NW_Ecma_FormattingComplete(iDocumentRoot);
  if(status != KBrsrSuccess)
    {
    HandleError (status );
    }
  if (!iDocumentRoot->isLoading)
      {
      UrlLoadStateChanged (TBrCtlDefs::EEventContentFinished);
      }
  }

// following functions come from old Epoc32Shell.cpp
/* ------------------------------------------------------------------------- */
TInt CShell::HandleDo( TUint32 aElemId )
    {
  TInt retVal=KErrNone;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_HED_DocumentNode_t* rootNode;

  /* get the document root node (not the DocumentRoot) */
  rootNode = NW_HED_DocumentRoot_GetChildNode (iDocumentRoot);
  if (NW_Object_IsInstanceOf (rootNode, &NW_Wml1x_ContentHandler_Class)) {    
    NW_Wml1x_ContentHandler_t* wml1xCH;
    NW_Wml_t* wml;

    wml1xCH = NW_Wml1x_ContentHandlerOf (rootNode);
    wml = &wml1xCH->wmlInterpreter;
    status = NW_Wml_GUIEvent (wml, (NW_Uint16)aElemId, NULL);
    if (status != KBrsrSuccess) {
      NW_Wml_HandleError(wml, BRSR_STAT_CLASS_GENERAL, (NW_Int16) status);
    }
  }

    if ( status!=KBrsrSuccess && status!= KBrsrWmlbrowserBadContent )
        {
        retVal=KErrUnknown;
        }

    return retVal;
}

// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::GetUseNumberBuf (NW_ADT_DynamicVector_t* dynamicVector, 
                         void** ptr )
    {
    NW_Ucs2                *pBuffer  = NULL;
    NW_Uint32              charCount;
    NW_Ucs2                *pToWrite;
    NW_ADT_Vector_Metric_t j;
    NW_ADT_Vector_Metric_t size;
    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (dynamicVector,
        &NW_ADT_DynamicVector_Class));

  NW_TRY (status) 
  {
    size = NW_ADT_Vector_GetSize(dynamicVector);

    for (charCount = j = 0; j < size; j++) 
        {
        NW_Text_t *text;
        text = *((NW_Text_t**) NW_ADT_Vector_ElementAt(dynamicVector, j));
        if (text)
            {
            charCount += NW_Text_GetCharCount(text); 
            }
        }
    /* size to allocate */
    charCount = (charCount+size+1)*sizeof(NW_Ucs2);
    
    *ptr = pBuffer = pToWrite =(NW_Ucs2*) NW_Mem_Malloc(charCount);

    if (pBuffer) 
        {
        NW_Mem_memset(pBuffer, 0, charCount);
        }
 
    for ( j = 0; j < size; j++) 
        {
        NW_Text_t *text;
        NW_Ucs2   *ucs2Str;

        text    = *((NW_Text_t **) NW_ADT_Vector_ElementAt(dynamicVector, j));
        if (text)
            {
            ucs2Str = NW_Text_GetUCS2Buffer (text, NW_Text_Flags_Copy, NULL, NULL);

            if (ucs2Str && pBuffer) 
                {
                (void)NW_Str_Strcpy (pToWrite, ucs2Str);
                pToWrite += (NW_Text_GetCharCount(text) );
                /* Add LF after each element */
                *pToWrite = NW_TEXT_UCS2_LF; /* 0x0A; */
                pToWrite++;
                }
    
            NW_Str_Delete(ucs2Str);
            NW_Object_Delete(text);
            }
        }

    NW_THROW_OOM_ON_NULL (pBuffer, status);

    /* We had pBuffer allocated but could not allocate temporary memory 
       for each of the collected strings. 
    */
    if (size > 0 && *pBuffer == 0) 
        {
        NW_THROW_STATUS(status, KBrsrOutOfMemory);
        }
    }  
    NW_CATCH (status) 
  {}
    NW_FINALLY 
        {
        NW_Object_Delete(dynamicVector);
        return status;
        } 
    NW_END_TRY
    }

// ---------------------------------------------------------------------------- 
void 
CShell::ClearHistoryList()
    {//R->history
  NW_HED_HistoryStack_t *history;

  history = NW_HED_DocumentRoot_GetHistoryStack (iDocumentRoot);
  if (history == NULL) {
    return;
  }

  /* delete all of the remaining entries */
  NW_HED_HistoryStack_DeleteAll (history);
}

// following functions come from old CEpoc32Shell.cpp
// ---------------------------------------------------------------------------- 
NW_Bool 
CShell::IsConversionSupported(NW_Uint32 uid, NW_Bool * ConverterAvailable)
{
  NW_Uint32 i = 0;
//lint --e{578} (Warning -- Declaration of symbol 'index' hides symbol 'index(const char *, int)' (line 75, file Q:\EPOC32\INCLUDE\LIBC\string.h)) 
  TInt index, count;

  NW_ASSERT(iArrayOfCharacterSetsAvailable);

  *ConverterAvailable = NW_FALSE;

  for (i = 0; supportedCharset[i].uid != 0; i++)
  {
    if (supportedCharset[i].uid == uid)
    {
      
      
      // now go through the iArrayOfCharacterSetsAvailable, check whether the 
      // charset is in the list, if not, i.e. converter is not available during
      // the runtime, set ConverterAvailable false to avoid future panic in 
      // PrepareToConvertToOrFromL functions.
      count = iArrayOfCharacterSetsAvailable->Count();
      
      for (index = 0; index < count; index++)
      {
        if (uid == ((CCnvCharacterSetConverter::SCharacterSet)(iArrayOfCharacterSetsAvailable->At(index))).Identifier())
        {
            *ConverterAvailable = NW_TRUE;
          break;
        }
      }
      return supportedCharset[i].needsConvert;
    }
  }
  return NW_FALSE;
}

// ---------------------------------------------------------------------------- 
NW_Uint32 
CShell::GetCharsetUid(NW_Uint32 internalEncoding)
{
    NW_Uint32 i = 0;
    for (i = 0; supportedCharset[i].uid != 0; i++)
    {
        if (supportedCharset[i].internalEncoding == internalEncoding)
        {
            return supportedCharset[i].uid;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------- 
NW_Uint32 
CShell::GetCharsetUid2(NW_Uint32 ianaEncoding)
{
    NW_Uint32 i = 0;
    for (i = 0; supportedCharset[i].uid != 0; i++)
    {
        if (supportedCharset[i].ianaEncoding == ianaEncoding)
        {
            return supportedCharset[i].uid;
        }
    }
    return KCharacterSetIdentifierIso88591;
}

NW_Uint32
CShell::GetUidFromCharset(NW_Uint8* charset, NW_Uint32 charsetLength)
{
  NW_Uint32 i = 0;
  NW_Ucs2* supportedCharStr = NULL;

  for (; supportedCharset[i].charset != NULL; i++)
  {
    supportedCharStr = NW_Asc_toUcs2((char*)supportedCharset[i].charset);

    if (charsetLength == NW_Str_Strlen(supportedCharStr)*sizeof(NW_Ucs2) &&
      NW_Byte_Strnicmp((const NW_Byte*)supportedCharStr, (const NW_Byte*)charset, charsetLength) == 0)
    {
      NW_Str_Delete(supportedCharStr);
      return supportedCharset[i].uid;
    }

    NW_Str_Delete(supportedCharStr);
  }

  return KCharacterSetIdentifierIso88591;
}

// ---------------------------------------------------------------------------- 
TInt CShell::GetCharsetAndCharsetStringFromUid(TUint aUid, TUint16* aCharset,
                                               	const TUint8** aCharsetString)
    {
    NW_Uint32 i = 0;
    for (i = 0; supportedCharset[i].uid != 0; i++)
        {
        if (supportedCharset[i].uid == aUid)
            {
            *aCharset = supportedCharset[i].ianaEncoding;
            return NW_HED_CompositeContentHandler_GetCharsetString(*aCharset, aCharsetString);
            }
        }
    return KErrArgument;
    }

// ---------------------------------------------------------------------------- 
//    CShell::CancelAll
// ---------------------------------------------------------------------------- 
TInt 
CShell::CancelAll()
  {
  /* invoke the cancel method on the document root */
  (void) NW_HED_DocumentNode_Cancel (iDocumentRoot, NW_HED_CancelType_User);
  //    UrlLoadStateChanged(EUrlLoadingStopped);
  return KErrNone;
  }

// ---------------------------------------------------------------------------- 
TInt 
CShell::FreeConnection()
    {
    //UrlLoader_Disconnect();
    return KErrNone;
    }

// ---------------------------------------------------------------------------- 
//    CShell::HistoryLoad
// ---------------------------------------------------------------------------- 
TInt 
CShell::HistoryLoad(THistoryStackDirection aDirection)
    {//R->history
    TInt retVal=KErrNone;
    NW_HED_HistoryStack_Direction_e kmDirection;

    switch ( aDirection )
        {
        case EHistoryStackDirectionPrevious:
            kmDirection=NW_HED_HistoryStack_Direction_Previous;
            break;

        case EHistoryStackDirectionNext:
            kmDirection=NW_HED_HistoryStack_Direction_Next;
            break;

        /*case ERestorePrevious:
            kmDirection=NW_HED_HistoryStack_Direction_RestorePrevious;
            break;

        case ERestoreNext:
            kmDirection=NW_HED_HistoryStack_Direction_RestoreNext;
            break;*/

        case EHistoryStackDirectionCurrent:
        default:
            kmDirection=NW_HED_HistoryStack_Direction_Current;
            break;
        }

    TBrowserStatusCode status = NW_HED_DocumentRoot_HistoryLoad (iDocumentRoot, kmDirection);  

    if ( status==KBrsrHedNoCurrentHistEntry || 
         status==KBrsrHedNoPreviousHistEntry|| 
         status==KBrsrHedNoNextHistEntry)
        {
        retVal=KErrNotFound;
        }
    else if ( status != KBrsrSuccess && status != KBrsrCancelled )
        {
        retVal=KErrUnknown;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------- 
//    CShell::ProcessBack
// ---------------------------------------------------------------------------- 
TInt CShell::ProcessBack()
  {
  TInt err = KErrNone;

  if (iView->IsImageMapView())
    {
    TBrowserStatusCode status = iView->SwitchFromImageMapView();

    if (status == KBrsrSuccess)
      {
      err = KErrNone;
      }
    else
      {
      err = KErrUnknown;
      }
    }
  else
    {
    err = HistoryLoad( EHistoryStackDirectionPrevious );
    }

  return err;
  }

// ---------------------------------------------------------------------------- 
//    CShell::SwitchToImgMapView
// ---------------------------------------------------------------------------- 
TInt 
CShell::SwitchToImgMapView()
  {
  TInt err = KErrNone;

  if (!iView->IsImageMapView())
    {
    TBrowserStatusCode status = iView->SwitchToImageMapView();

    if (status == KBrsrSuccess)
      {
      err = KErrNone;
      }
    else
      {
      err = KErrUnknown;
      }
    }
  return err;
  }

// ---------------------------------------------------------------------------- 
//    CShell::SwitchFromImgMapViewIfNeeded
// ---------------------------------------------------------------------------- 
TInt 
CShell::SwitchFromImgMapViewIfNeeded()
  {
  TInt err = KErrNone;

  if (iView->IsImageMapView())
    {
    TBrowserStatusCode status = iView->SwitchFromImageMapView();

    if (status == KBrsrSuccess)
      {
      err = KErrNone;
      }
    else
      {
      err = KErrUnknown;
      }
    }
  return err;
  }

// ---------------------------------------------------------------------------- 
//    CShell::Load
// ---------------------------------------------------------------------------- 
TInt 
CShell::Load (const TDesC& aUrl, TInt aApId, NW_Cache_Mode_t cacheMode)
    {
    // UriParser assumes the scheme is any characters prior to the first
    // colon (:)  This is an issue if we've been passed a url that has
    // a port number or is an IPv6 address, and there is not a scheme
    // in the url.  If this is the case, default to using http:// scheme.
    _LIT16(KPortSepStr,":");
    _LIT16(KSchemeEndStr,"://");
    _LIT16(KHTTPSchemeStr,"http://");
    TInt tempBufLength = aUrl.Length() + 8;
    HBufC* tempUrl = HBufC::New(tempBufLength);
    if(tempUrl == NULL)
        {
        return KErrNoMemory;
        }
    TPtr tempUrlPtr(tempUrl->Des());
    tempUrlPtr.FillZ();

    if ((aUrl.Find(KPortSepStr) != KErrNotFound) && (aUrl.Find(KSchemeEndStr) == KErrNotFound))
        {
        tempUrlPtr.Append(KHTTPSchemeStr);
        }
    tempUrlPtr.Append(aUrl);
    tempUrlPtr.ZeroTerminate();

    iApId = aApId;
    NW_Text_t* textUrl = NW_Text_New (HTTP_iso_10646_ucs_2, 
                                      (void*) tempUrl->Des().Ptr(), 
                                      (NW_Text_Length_t)tempUrl->Des().Length(), 
                                      NW_FALSE);

    if (textUrl == NULL)
        {
      return KErrNoMemory;
        }

    (void) SwitchFromImgMapViewIfNeeded();
	
    NW_HED_DocumentRoot_SetHistoryControl(iDocumentRoot, iWmlControl );
    
    TBrowserStatusCode status = StartLoad (textUrl, NULL, cacheMode);

    NW_Object_Delete(textUrl);
    delete tempUrl;

    if(status == KBrsrSuccess) 
        {
        /*
        storage = (NW_Ucs2*)NW_Text_GetStorage (textUrl, 0, NULL, NULL);
        NW_Str_Strcpy(g_Location, storage);
        */
        return KErrNone;
        }
    if(status == KBrsrOutOfMemory)
        {
      return KErrNoMemory;
        }

    if(status == KBrsrMalformedUrl)
      return KErrNotFound;

    return KErrGeneral;
    }

// ---------------------------------------------------------------------------- 
void 
CShell::SetView (CView* aEpocView)
{
  NW_ASSERT(aEpocView!=NULL);
  iView = aEpocView; // Does not take the ownership!
  iView->SetShell(this);
  iView->SetListener(this);
  iView->SetAppServices((void *)(&appServices));
}

// ---------------------------------------------------------------------------- 
// CShell::GetHistoryList
// ---------------------------------------------------------------------------- 
TInt 
CShell::GetHistoryList (CArrayFix<TBrCtlSelectOptionData>& aHistoryList)
    {//R->history
    NW_HED_HistoryStack_t* history = NULL;
    NW_HED_HistoryVisitor_t* visitor;
    const NW_HED_HistoryEntry_t* entry;
    const NW_HED_HistoryEntry_t* currentEntry;

    history = NW_HED_DocumentRoot_GetHistoryStack(iDocumentRoot);
    if ( history == NULL )
        {
        return KErrNoMemory;
        }

  // get current history position
    visitor = NW_HED_HistoryVisitor_New( history, 
                                         NW_HED_HistoryVisitor_Loacation_Current );
    if ( visitor == NULL )
        {
        return KErrNoMemory;
        }
    currentEntry = NW_HED_HistoryVisitor_Current( visitor );

  // set inital entry at the end of docRoot's history list
    if ( NW_HED_HistoryVisitor_Initialize( 
            visitor, history, 
            NW_HED_HistoryVisitor_Loacation_AtEnd ) != KBrsrSuccess )
        {
        return KErrNoMemory;
        }

    entry = NW_HED_HistoryVisitor_Current( visitor );
    while ( entry != NULL )
        {
        TPtrC ptr;

    // use page title
        if(entry->pageTitle)
          {
          ptr.Set(entry->pageTitle);
          }
    // if there's no title - use URL
        if(!ptr.Length() && entry->urlResponse->rawUrl)
          {
          ptr.Set(entry->urlResponse->rawUrl);
          }
        if(!ptr.Length() && entry->urlRequest->rawUrl)
          {
          ptr.Set(entry->urlRequest->rawUrl);
          }

    // Copy the result into a TDes.
		TInt len = ptr.Length();
		if ( (ptr.Left( KHttpString().Length() ).Compare( KHttpString ) == 0 ) || 
			   (ptr.Left( KFileString().Length() ).Compare( KFileString ) == 0 ) )
			{
			len -= KHttpString().Length();								
			}
		ptr.Set (ptr.Right( len ));

    // set iIsSelected to true to inform host apps which one is the current history entry
    TBool isSelected = (currentEntry == entry) ? ETrue : EFalse;
    TBrCtlSelectOptionData historyListEntry(ptr, isSelected, EFalse, ETrue);

    // add history list entry
	TInt err;  
    TRAP(err, aHistoryList.AppendL (historyListEntry));
    if (err != KErrNone)
            {
      return err;
            }

        /* advance to the next entry */
        entry = NW_HED_HistoryVisitor_Prev( visitor );
        }

    NW_Object_Delete( visitor );

    return KErrNone;
    }

// ---------------------------------------------------------------------------- 
//    CShell::HistoryStackPosition()
// ---------------------------------------------------------------------------- 
TWmlEngineDefs::THistoryStackPosition 
CShell::HistoryStackPosition()
    {//R->history
	TWmlEngineDefs::THistoryStackPosition pos = TWmlEngineDefs::EHistoryInTheMiddle;
    NW_HED_HistoryStack_t* history;
    NW_HED_HistoryVisitor_t* visitor;
    
    if ( ( history = NW_HED_DocumentRoot_GetHistoryStack ( iDocumentRoot ) ) != NULL ) 
        {
        if ( ( visitor = NW_HED_HistoryVisitor_New( history, 
            NW_HED_HistoryVisitor_Loacation_Current ) ) != NULL ) 
            {
            /* if we can not go forward we are at the end of the stack */
            if ( NW_HED_HistoryVisitor_Next( visitor )  == NULL ) 
                {
                pos = TWmlEngineDefs::EHistoryAtTheEnd;
                }
            else
                {
                /* we made one step forward, so step back to current entry */
                NW_HED_HistoryVisitor_Prev( visitor );
                }

            /* if we can not go back we are at the beginning of the stack */
            if ( NW_HED_HistoryVisitor_Prev( visitor ) == NULL ) 
                {
                pos = TWmlEngineDefs::EHistoryAtTheBeginning;
                }
            
            NW_Object_Delete( visitor );
            }
        }
    
    return pos;    
    }

// ------------------------------------------------------------------------- 
//    CShell::LoadHistoryEntry(CArrayFix<TBrCtlSelectOptionData>& aHistoryList)
// ------------------------------------------------------------------------- 
TInt 
CShell::LoadHistoryEntry (CArrayFix<TBrCtlSelectOptionData>& aHistoryList)
  {//R->history
  TInt size = aHistoryList.Count();
//lint --e{578} (Warning -- Declaration of symbol 'index' hides symbol 'index(const char *, int)' (line 75, file Q:\EPOC32\INCLUDE\LIBC\string.h)) 
  TInt index;
  for (index = 0 ; index < size ; index++)
    {
    TBrCtlSelectOptionData& entry = aHistoryList[index];
    if (entry.IsSelected())
      {
      break;
      }
    }

  /* get docRoot's history stack */
  NW_HED_HistoryStack_t* docHistory = NW_HED_DocumentRoot_GetHistoryStack( iDocumentRoot );
  if ( docHistory == NULL )
        {
        return KErrNoMemory;
        }

  // docRoot's history stack has a reversed order of entries compared to CArrayFix<TBrCtlSelectOptionData>
  // this is because in GetHistoryList():
  // entry taken from docRoot's history stack at NW_HED_HistoryVisitor_Loacation_AtEnd
  // and then appended to CArrayFix<TBrCtlSelectOptionData>
  index = size - index - 1;
  NW_HED_HistoryEntry_t* entry = NW_HED_HistoryStack_GetEntryByIndex (docHistory, 
                                             (NW_HED_HistoryStack_Metric_t)index);

  if ( NW_HED_HistoryStack_SetCurrentEntry( docHistory, entry ) != KBrsrSuccess ) 
        {
        return KErrNotFound;
        }

    (void) SwitchFromImgMapViewIfNeeded();

    if (NW_HED_DocumentRoot_HistoryLoad (iDocumentRoot, NW_HED_HistoryStack_Direction_Current) != KBrsrSuccess)
        {
        return KErrGeneral;
        }

    return KErrNone;    
    }


// ---------------------------------------------------------------------------- 
//    CShell::SetBoxTree(NW_LMgr_Box_t* aBoxTree)
// ---------------------------------------------------------------------------- 
TInt 
CShell::SetBoxTree( NW_LMgr_Box_t* aBoxTree )
    {
	__ASSERT_DEBUG(iView, Panic(KPanicKmShellViewPtrIsNotSet));

    TInt status;

    NW_LMgr_RootBox_t* rootBox = iView->GetRootBox ();

    status = iView->SetBoxTree (aBoxTree);
    
    // update CBA depends on if browser or external apps opened the file
    if( !iFileOpenedByDocHandler )
        {
    iView->UpdateCBAs ();
        }
    // else, e.g. NotePad.exe opened the file, we can't update CBAs

    (void) iView->Draw (NW_TRUE /*DrawNow*/);

    if ( status== KBrsrSuccess && aBoxTree)
        {
      if (!rootBox->partialRender ||
            rootBox->partialRender &&
            rootBox->renderingState == NW_LMgr_RootBox_RenderFirstChunk)
      {
          TInt err;  
		  TRAP(err, iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventNewContentDisplayed, 0, 0));
      }
        else if (rootBox->partialRender &&
                 rootBox->renderingState == NW_LMgr_RootBox_RenderFurtherChunks)
      {
            TInt err;  
			TRAP(err, iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventMoreContentDisplayed, 0, 0));
      }
        }

  return KBrsrSuccess;
    }

// ----------------------------------------------------------------------------
void 
CShell::OptionListInit()
    {
	iWmlControl->WKWmlInterface()->resetWmlServiceOptions(); 
    }

// ----------------------------------------------------------------------------
void 
CShell::OptionListAddItem( const TText* aText, TUint32 aElemID, TInt aElType )
    {
    	TInt err;  
	TRAP(err, iWmlControl->WKWmlInterface()->setWmlServiceOptionsAddItemL(aText, aElemID, aElType));
    }

// ----------------------------------------------------------------------------
void 
CShell::UrlLoadStateChanged(TBrCtlDefs::TBrCtlLoadEvent /*aEvent*/)
  {
  }

// ----------------------------------------------------------------------------
void 
CShell::UpdateTitle()
{
    TInt err;  
	TRAP(err, iBrCtl->HandleBrowserLoadEventL (TBrCtlDefs::EEventTitleAvailable, 0, 0));
}

// ---------------------------------------------------------------------------- 
//    CShell::DialogApi
// ---------------------------------------------------------------------------- 

HBufC* 
CShell::GetCurrentUrlInHBufCL() const
{
	NW_Text_t* currentUrl = (const_cast<CShell*>(this))->GetCurrentUrl ();

	HBufC* ret = NULL;
	if (currentUrl && NW_Text_GetCharCount(currentUrl) > 0)
	{
		NW_Ucs2* currentUrlUcs2 = NW_Text_GetUCS2Buffer(currentUrl, NW_Text_Flags_NullTerminated, NULL, NULL);
		ret = TPtrC(currentUrlUcs2).AllocL();
	}
	else
	{
        _LIT(KEmptyString, " ");
        ret = KEmptyString().AllocL();
	}

    NW_Object_Delete(currentUrl);

	return ret;
}

// ---------------------------------------------------------------------------- 
HBufC* 
CShell::GetCurrentTitleInHBufCL( TBool& aStripUrl) const
{
    NW_REQUIRED_PARAM( aStripUrl );

	const NW_Text_t* currentTitleTxt = (const_cast<CShell*>(this))->GetCurrentTitle ();

	HBufC* ret = NULL;

	if (currentTitleTxt && NW_Text_GetCharCount(currentTitleTxt) > 0)
	{
		NW_Ucs2* currentTitleUcs2 = NW_Text_GetUCS2Buffer(currentTitleTxt, NW_Text_Flags_NullTerminated, NULL, NULL);
		ret = TPtrC(currentTitleUcs2).AllocL();
	}
	else
	{
        ret = GetCurrentUrlInHBufCL();
        aStripUrl = ETrue;
	}

	return ret;
}

// ---------------------------------------------------------------------------- 
TUint16 
CShell::SaveToFileL(TDesC& aFileName) const
{
  HBufC* hbuf = HBufC::New(aFileName.Length()+1);
  TUint16 err = 1; /* default to sucess */
  CleanupStack::PushL(hbuf);
  hbuf->Des().Append(aFileName);

  /* get the document root node (not the DocumentRoot) */
  NW_ASSERT (iDocumentRoot != NULL);
  NW_HED_DocumentNode_t* rootNode = NW_HED_DocumentRoot_GetChildNode (iDocumentRoot);

  TBrowserStatusCode status;
  if (NW_Object_IsClass(rootNode, &NW_Wml1x_Epoc32ContentHandler_Class)) 
    {     
    status = NW_Wml1x_Epoc32ContentHandler_SaveToFile(NW_Wml1x_Epoc32ContentHandlerOf(rootNode), CONST_CAST(TUint16*, hbuf->Des().PtrZ()));
    }
  else if (NW_Object_IsClass(rootNode, &NW_XHTML_Epoc32ContentHandler_Class)) 
    {    
    status = NW_XHTML_Epoc32ContentHandler_SaveToFile(NW_XHTML_Epoc32ContentHandlerOf(rootNode), CONST_CAST(TUint16*, hbuf->Des().PtrZ()));
    }
  else
    {
    status = KBrsrFailure;
    }

  if( status != KBrsrSuccess )
    {
    Shell()->iBrCtl->brCtlDialogsProvider()->DialogNotifyErrorL(status);
    err = 0; /* failure */
    }
  CleanupStack::PopAndDestroy();

  /* in this particular case, the UI is expecting 0 on failure and 1 on success */
  return err;
}
// -------------------------------------------------------------------------------
// CShell::GetPageAsTextL()
// -------------------------------------------------------------------------------
// 
HBufC* 
CShell::GetPageAsTextL()
    {    
    NW_ADT_DynamicVector_t* dynamicVector;
    

    TText* ptr = NULL;
    dynamicVector = (NW_ADT_DynamicVector_t*) 
    NW_ADT_ResizableVector_New ( sizeof (NW_Text_t*), 5, 5 );
    
    TBrowserStatusCode status1 = KBrsrFailure;
    TBrowserStatusCode status = KBrsrFailure;
    
    if( dynamicVector )
        {
        status1 = GetUseNumberContent (dynamicVector);
        status = GetUseNumberBuf( dynamicVector, (void**) &ptr );
        }
    if( status == KBrsrSuccess )
        {
        status = status1;
        }
    if( status != KBrsrSuccess )
        {
        return HBufC::NewL(0);
        }
    else
    {        
        HBufC* pageText = NULL;
        TPtrC bufPointer( ptr );
        pageText = bufPointer.AllocL();
        
        NW_Mem_Free( ptr );
        return pageText;
    }
    
}

// -------------------------------------------------------------------------------
// CShell::GetPageAsWMLMarkUp()
// Get the page as WML markup And contents as in the DOM.
// -------------------------------------------------------------------------------
// 
HBufC8* 
CShell::GetPageAsWMLMarkUp()
{  
 NW_HED_ContentHandler_t* contentHandler; 
 NW_Wml1x_Epoc32ContentHandler_t *wmlContentHandler;
 NW_Wml1x_ContentHandler_t *wml1xch;
 HBufC8* pageWML = NULL;
  
 /* get the document root node (not the DocumentRoot) */
 NW_ASSERT (iDocumentRoot != NULL);
 NW_HED_DocumentNode_t* rootNode = NW_HED_DocumentRoot_GetChildNode (iDocumentRoot);

 
 
 if (NW_Object_IsClass(rootNode, &NW_Wml1x_Epoc32ContentHandler_Class)) 
    { 
	 wmlContentHandler = NW_Wml1x_Epoc32ContentHandlerOf(rootNode);
	 wml1xch = NW_Wml1x_ContentHandlerOf(wmlContentHandler);
	 contentHandler = NW_HED_ContentHandlerOf( wml1xch );

	 pageWML = HBufC8::NewL(contentHandler->response->body->length);
     pageWML->Des().Copy(contentHandler->response->body->data,contentHandler->response->body->length);
	 
	}
  return pageWML;
}//end CShell::GetPageAsWMLMarkUp()


// -------------------------------------------------------------------------------
// CShell::GetCharsetFromResponse()
// Get the charset from the header.
// -------------------------------------------------------------------------------
// 


const NW_Uint8* CShell::GetCharsetFromResponse()
{  
 
 NW_HED_ContentHandler_t* contentHandler; 
 NW_Wml1x_Epoc32ContentHandler_t *wmlContentHandler;
 NW_Wml1x_ContentHandler_t *wml1xch;
   
 /* get the document root node (not the DocumentRoot) */
 NW_ASSERT (iDocumentRoot != NULL);
 NW_HED_DocumentNode_t* rootNode = NW_HED_DocumentRoot_GetChildNode (iDocumentRoot);
  
 if (NW_Object_IsClass(rootNode, &NW_Wml1x_Epoc32ContentHandler_Class)) 
    { 
		wmlContentHandler = NW_Wml1x_Epoc32ContentHandlerOf(rootNode);
		wml1xch = NW_Wml1x_ContentHandlerOf(wmlContentHandler);
		contentHandler = NW_HED_ContentHandlerOf( wml1xch );		
				    
		NW_Uint32 charsetUid = GetCharsetUid2(contentHandler->response->charset);

	    for (NW_Uint32 i = 0; supportedCharset[i].uid != 0; i++)
	        {
	        if (supportedCharset[i].uid == charsetUid)
	            {
				return supportedCharset[i].charset;	            	            
	            }
	        }
                        
	}
	
	return NULL;
	    
}//end CShell::GetCharsetFromResponse()


// ---------------------------------------------------------------------------- 
//    CShell::ShowImagesL()
// ---------------------------------------------------------------------------- 
//
void 
CShell::ShowImagesL() const
  {
  TBrowserStatusCode status =  NW_HED_DocumentRoot_ShowImages(iDocumentRoot);
  if( status != KBrsrSuccess )
		{
		if(status == KBrsrOutOfMemory)
			User::LeaveNoMemory();
		else
			User::Leave(status);
		}
	}

// ---------------------------------------------------------------------------- 
void 
CShell::CharacterSetChangedL( TUint aSelectedCharacterSet ) 
  {
    NW_Bool available = NW_TRUE; 
    // if called here, char converter should always be available 
    // therefore, no need to check the return value of available
    if (IsConversionSupported(aSelectedCharacterSet, &available))
    {
    // Create the character Converter object if not created already
    if( !iCharacterSetConverter )
      {
		  iCharacterSetConverter = CCnvCharacterSetConverter::NewL();			
      }
    if( !iArrayOfCharacterSetsAvailable )
      {
	    iArrayOfCharacterSetsAvailable = 
            CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL( iRfs );
      User::LeaveIfNull( iArrayOfCharacterSetsAvailable );
      }
    // Specifies the character set to convert to/from using a search array.
    iCharacterSetConverter->PrepareToConvertToOrFromL( aSelectedCharacterSet, 
          *iArrayOfCharacterSetsAvailable, iRfs);
	  iCharacterSetConverter->SetDefaultEndiannessOfForeignCharacters( CCnvCharacterSetConverter::ELittleEndian );
        // set the charset flags
    iSelectedCharacterSet = aSelectedCharacterSet;
        iCurrentConverter = aSelectedCharacterSet;
    }
  else
  {
    iSelectedCharacterSet = aSelectedCharacterSet;
  }
  }

/* Convert a foreign charset encoding into a UCS2 encoding stream,
 * such as GB2312 or Big5 using Symbian Character set converter.
 * return values:
 * KErrNone (0) : if no conversion is necessary or conversion succeeded
 * EErrorIllFormedInput: if there is some parsing error in conversion.
 *                       same value as KErrCorrupt=(-20);
 */
TUint 
CShell::ConvertFromForeignCharSetL(void* resp)
{
  NW_Uint32 uid;
  NW_Url_Resp_t* response = (NW_Url_Resp_t*)resp;
  NW_Bool available = NW_FALSE;
  NW_ASSERT(resp != NULL);
  uid = GetCharsetUid2(response->charset);
  /* response->charset must be the same as the current settings, or the current setting must be automatic */
  
  NW_ASSERT((uid == iSelectedCharacterSet) || iSelectedCharacterSet == 0);
  if (IsConversionSupported(uid, &available))
  {
    if (!available)
    {
      NW_LOG1(NW_LOG_LEVEL2, "ConvertFromForeignCharSetL returns KBrsrMvcConverterNotFound for encoding: %x.", response->charset);
      User::Leave(KBrsrMvcConverterNotFound);
    }
    if (!(uid == iCurrentConverter))
    {
      // Create the character Converter object if not created already
      if( !iCharacterSetConverter )
      {
        iCharacterSetConverter = CCnvCharacterSetConverter::NewL();
      }
      if( !iArrayOfCharacterSetsAvailable )
      {
        iArrayOfCharacterSetsAvailable = 
          CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL( iRfs );
        User::LeaveIfNull( iArrayOfCharacterSetsAvailable );
      }
      // Specifies the character set to convert to/from using a search array.
      iCharacterSetConverter->PrepareToConvertToOrFromL( uid, 
        *iArrayOfCharacterSetsAvailable, iRfs);
      iCharacterSetConverter->SetDefaultEndiannessOfForeignCharacters( CCnvCharacterSetConverter::ELittleEndian );
      // set the charset flags
      iCurrentConverter = uid;
    }
    TPtrC8 originalForeign(response->body->data,response->body->length);
    TBuf16<20> outputBuffer;
    HBufC16* generatedUnicode = HBufC16::NewL(20);
    CleanupStack::PushL(generatedUnicode); /*when use realloc later, the pointer is destroyed and realloc. cannot use NewLC here*/
    TInt state = CCnvCharacterSetConverter::KStateDefault;
    TInt returnValue = KErrNone;
    FOREVER
    {
      returnValue = iCharacterSetConverter->ConvertToUnicode(outputBuffer, originalForeign, state);
      generatedUnicode->Des().Append(outputBuffer);
      CleanupStack::Pop();
      generatedUnicode = generatedUnicode->ReAllocL(generatedUnicode->Des().Length()+20);
      CleanupStack::PushL(generatedUnicode);
      if (returnValue <= 0)
      {
        break;
      }
      originalForeign.Set(originalForeign.Right(returnValue));
    }
    if (returnValue >= 0) /*no error at all when converting*/
    {
      NW_Byte* generateData = (NW_Byte*)NW_Mem_Malloc(generatedUnicode->Des().Length()*2); /*16 bit to 8 bit*/
      if (generateData == NULL)
      {
        User::Leave(KErrNoMemory);
      }
      NW_Mem_Free(response->body->data); /*free the orginial data*/
      TPtr16 TempBodyData((unsigned short*)generateData,generatedUnicode->Des().Length()); /*this casting ok, this is home we can use the Des16 to copy the data to the buffer*/
      TempBodyData.Copy(generatedUnicode->Des());
      response->body->data = generateData;
      response->body->length = generatedUnicode->Des().Length()*2;
      response->body->allocatedLength = response->body->length;
      response->charset = HTTP_iso_10646_ucs_2;
    }
    else
    {
      CleanupStack::PopAndDestroy();//generatedUnicode
      User::Leave(returnValue);
    }
    
    // change the byte order to big endian becasue tempest expects big endian coming from network.
    // code copy from NW_HTMLP_WbxmlEncoder_Ucs2NetworkToNativeByteOrder function.
    NW_Uint16 c_ucs2;
    NW_Uint32 i = 0;
    for (i = 0; i < response->body->length; i += 2) 
    {
    /* this is a clever trick: pick up bytes in big endian order,
    force the result to memory via taking address of result which
    will put the 16-bits into native byte order, then copy the
      result over the original bytes */
      c_ucs2 = (NW_Uint16)((response->body->data[i] << 8) | response->body->data[i+1]);
      (void)NW_Mem_memcpy(response->body->data + i, &c_ucs2, sizeof(NW_Uint16));
    }
    
    CleanupStack::Pop(); /*generatedUnicode*/
    delete generatedUnicode;
    //CleanupStack::PopAndDestroy(2);
  }
  return KErrNone;
}


/* Convert a foreign charset encoding into a UCS2 encoding stream,
 * such as GB2312 or Big5 using Symbian Character set converter.
 * return values:
 * KErrNone (0) : if no conversion is necessary or conversion succeeded
 * EErrorIllFormedInput: if there is some parsing error in conversion.
 *                       same value as KErrCorrupt=(-20);
 */
TUint 
CShell::ConvertFromForeignChunkL( NW_Uint32 foreignEncoding, void* inBuf, TInt * numUnconvertible, TInt* indexFirstUnconvertible, void** outBuf)
{
  NW_Uint32 uid;
  TInt unconvertible = -1;
  TInt firstUnconvertible = -1;
  NW_Buffer_t * buf = NULL;
  NW_Bool available;

  NW_ASSERT(inBuf != NULL);
  NW_ASSERT(outBuf != NULL);

  uid = GetCharsetUid2(foreignEncoding);
  // response->charset must be the same as the current settings, or the current setting must be automatic 
  
  NW_ASSERT((uid == iSelectedCharacterSet) || iSelectedCharacterSet == 0);
  if (IsConversionSupported(uid, &available))
  {
    if (!available)
    {
      NW_LOG1(NW_LOG_LEVEL2, "ConvertFromForeignChunkL returns KBrsrMvcConverterNotFound for encoding: %x.", foreignEncoding);
      User::Leave(KBrsrMvcConverterNotFound);
    }

    if (uid != iCurrentConverter)
    {
      // Create the character Converter object if not created already
      if( !iCharacterSetConverter )
      {
        iCharacterSetConverter = CCnvCharacterSetConverter::NewL();
      }
      if( !iArrayOfCharacterSetsAvailable )
      {
        iArrayOfCharacterSetsAvailable = 
          CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL( iRfs );
        User::LeaveIfNull( iArrayOfCharacterSetsAvailable );
      }
      // Specifies the character set to convert to/from using a search array.
      iCharacterSetConverter->PrepareToConvertToOrFromL( uid, 
        *iArrayOfCharacterSetsAvailable, iRfs);

      iCharacterSetConverter->SetDefaultEndiannessOfForeignCharacters( CCnvCharacterSetConverter::ELittleEndian );
      // set the charset flags
      iCurrentConverter = uid;
    }
    TPtrC8 originalForeign(((NW_Buffer_t*)inBuf)->data,((NW_Buffer_t*)inBuf)->length);
    TBuf16<20> outputBuffer;
    HBufC16* generatedUnicode = HBufC16::NewL(20);
    CleanupStack::PushL(generatedUnicode); //when use realloc later, the pointer is destroyed and realloc. cannot use NewLC here
    TInt state = CCnvCharacterSetConverter::KStateDefault;
    TInt returnValue = KErrNone;
    FOREVER
    {
      returnValue = iCharacterSetConverter->ConvertToUnicode(outputBuffer, originalForeign, state, unconvertible, firstUnconvertible);
      *numUnconvertible = unconvertible;
      if (firstUnconvertible != -1)
      {
        *indexFirstUnconvertible = firstUnconvertible;
      }
      // keep track on how many successful converted chars already 
      else if (returnValue > 0)
      {
        *indexFirstUnconvertible = ((NW_Buffer_t*)inBuf)->length - returnValue;
      }
      generatedUnicode->Des().Append(outputBuffer);
      CleanupStack::Pop();
      generatedUnicode = generatedUnicode->ReAllocL(generatedUnicode->Des().Length()+20);
      CleanupStack::PushL(generatedUnicode);
      if (returnValue == 0 && *numUnconvertible > 2)
      {
            returnValue = KErrCorrupt;
      }
      if (returnValue <= 0)
      {
        break;
      }
      originalForeign.Set(originalForeign.Right(returnValue));
    }
    // some error tolerance due to incomplete character
    if (returnValue < 0 && *indexFirstUnconvertible != -1)
    {      *numUnconvertible = ((NW_Buffer_t*)inBuf)->length - *indexFirstUnconvertible;
      if (*numUnconvertible <= 2)
        returnValue = KErrNone;
    }
    if (returnValue >= 0 )
    {
      buf = (NW_Buffer_t*)NW_Buffer_New(0);     
      User::LeaveIfNull(buf);
      *outBuf = buf;
      NW_Byte* generateData = (NW_Byte*)NW_Mem_Malloc(generatedUnicode->Des().Length()*2); //16 bit to 8 bit
      User::LeaveIfNull(generateData);
      buf->data = generateData;
      TPtr16 TempBodyData((unsigned short*)generateData,generatedUnicode->Des().Length()); /// this casting ok, this is home we can use the Des16 to copy the data to the buffer
      TempBodyData.Copy(generatedUnicode->Des());
      buf->length = generatedUnicode->Des().Length()*2;
    }
    else
    {
      CleanupStack::PopAndDestroy();//generatedUnicode
      User::Leave(returnValue);
    } 
    
    // change the byte order to big endian becasue tempest expects big endian coming from network.
    // code copy from NW_HTMLP_WbxmlEncoder_Ucs2NetworkToNativeByteOrder function.
    NW_Uint16 c_ucs2;
    NW_Uint32 i = 0;
    for (i = 0; i < buf->length; i += 2) 
    {
    // this is a clever trick: pick up bytes in big endian order,
    //force the result to memory via taking address of result which
    //will put the 16-bits into native byte order, then copy the
    //  result over the original bytes 
      c_ucs2 = (NW_Uint16)((buf->data[i] << 8) | buf->data[i+1]);
      (void)NW_Mem_memcpy(buf->data + i, &c_ucs2, sizeof(NW_Uint16));
    }
    
      CleanupStack::Pop(); //generatedUnicode
    delete generatedUnicode;
    //CleanupStack::PopAndDestroy(2);
  }

  return KErrNone;
}

/* Convert UCS2 encoding stream into a foreign charset encoding,
 * such as GB2312 or Big5 using Symbian Character set converter.
 * return values:
 * KErrNone (0) : if no conversion is necessary or conversion succeeded
 * EErrorIllFormedInput: if there is some parsing error in conversion.
 *                       same value as KErrCorrupt=(-20);
 */
TUint 
CShell::ConvertToForeignCharSetL(void* ucsText)
{
  if (!ucsText)
    return KErrNone;
  TUint settingEncoding = NW_Settings_GetOriginalEncoding();
  TUint uid = GetCharsetUid2(settingEncoding);
  NW_Bool available =NW_FALSE;
  if (!IsConversionSupported(uid, &available))
    User::Leave(KBrsrMvcConverterNotFound);
  else if (!available)
    {
    NW_LOG1(NW_LOG_LEVEL2, "ConvertToForeignCharSetL returns KBrsrMvcConverterNotFound for encoding: %x.", settingEncoding);
    User::Leave(KBrsrMvcConverterNotFound);
    }

  NW_String_t* text = (NW_String_t*)ucsText;
  if (NW_String_getStorage(text) == 0)
    return KErrNone;

  if (!(uid == iCurrentConverter))
  {
      // Create the character Converter object if not created already
      if( !iCharacterSetConverter )
      {
	 	  iCharacterSetConverter = CCnvCharacterSetConverter::NewL();
      }
      if( !iArrayOfCharacterSetsAvailable )
      {
	    iArrayOfCharacterSetsAvailable = 
        CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL( iRfs );
        User::LeaveIfNull( iArrayOfCharacterSetsAvailable );
      }
      // Specifies the character set to convert to/from using a search array.
      iCharacterSetConverter->PrepareToConvertToOrFromL( uid, 
        *iArrayOfCharacterSetsAvailable, iRfs);
      iCharacterSetConverter->SetDefaultEndiannessOfForeignCharacters( CCnvCharacterSetConverter::ELittleEndian );
      // set the charset flags
      iCurrentConverter = uid;
  }


	TPtrC16 originalUcs((const unsigned short *)NW_String_getStorage(text), NW_String_getByteCount (text)/2);
	TBuf8<20> outputBuffer;
	HBufC8* generatedBuffer= HBufC8::NewL(20);
	CleanupStack::PushL(generatedBuffer); 
	TInt returnValue = KErrGeneral;
	FOREVER
	{
		returnValue = iCharacterSetConverter->ConvertFromUnicode(outputBuffer, originalUcs);
		generatedBuffer->Des().Append(outputBuffer);
		CleanupStack::Pop();
		generatedBuffer = generatedBuffer->ReAllocL(generatedBuffer->Des().Length()+20);
		CleanupStack::PushL(generatedBuffer);
		if (returnValue <= 0)
			break;
		originalUcs.Set(originalUcs.Right(returnValue));
	}
	if (returnValue >= 0) 	{
		NW_Byte* generateData = (NW_Byte*)NW_Mem_Malloc(generatedBuffer->Des().Length() + 2); 
    if (generateData == NULL)
      User::Leave(KErrNoMemory);
    NW_String_deleteStorage(text); 
 	  TPtr8 TempBodyData((unsigned char*)generateData, generatedBuffer->Des().Length()); 
    TempBodyData.Copy(generatedBuffer->Des());
    /* make sure there are enough null bytes at the end of string, for future Ucs2 manipulation*/
    *(generateData + generatedBuffer->Des().Length()) = 0;
    *(generateData + generatedBuffer->Des().Length() + 1) = 0; 
		text->storage = generateData;
		text->length = generatedBuffer->Des().Length() + 2;
	}

	CleanupStack::Pop(); /*generatedBuffer*/
	delete generatedBuffer;
	//CleanupStack::PopAndDestroy(2);
  return returnValue;

}

// ---------------------------------------------------------------------------- 
//    CShell::GetCertInfo()
// ---------------------------------------------------------------------------- 
//
void* 
CShell::GetCertInfo()
{
  return NW_HED_DocumentRoot_GetCertInfo (iDocumentRoot);
}

//--------------------------------------------------------------------------
//    CShell::NotifyProgress()
//--------------------------------------------------------------------------
void 
CShell::NotifyProgress(TBrCtlDefs::TBrCtlLoadEvent aEvent,
                       TUint32 aSize,
                       TUint16 aTransactionId)
    {
    TInt err;  
	TRAP(err, iBrCtl->HandleBrowserLoadEventL(aEvent, aSize, aTransactionId));
    }

//--------------------------------------------------------------------------
//    CShell::NotifyProgress()
//--------------------------------------------------------------------------
MBrCtlSpecialLoadObserver* 
CShell::GetLoadObserver()
    {
    return iBrCtl->brCtlSpecialLoadObserver();
    }

//--------------------------------------------------------------------------
//    CShell::HandleDownloadL()
//--------------------------------------------------------------------------
//
TBool 
CShell::HandleDownloadL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray)
    {
    return iBrCtl->brCtlSpecialLoadObserver()->HandleDownloadL(aTypeArray, aDesArray);
    }

// functions from old Epoc32ShellCallbacks.cpp
// ---------------------------------------------------------------------------- 
//    CShell::GetLocalizedString
// ---------------------------------------------------------------------------- 
TUint16* 
CShell::NW_UI_GetLocalizedString( NW_Uint32 type )
{
    TInt resourceId = 0;
	TUint16* pLocal = NULL;  
    switch ((TKmLocStringId)type)
    {
        case EKmLocSubmitButton:
            resourceId = R_KIMONO_FORM_BUTTON_SUBMIT;
            break;
        case EKmLocResetButton:
            resourceId = R_KIMONO_FORM_BUTTON_RESET;
            break;
        case EKmLocSelectLabel:
            resourceId = R_KIMONO_FORM_OPTION_SELECT;
            break;
        case EKmLocBackSoftkeyLabel:
            resourceId = R_KIMONO_SOFTKEY_BACK;
            break;
    }

    HBufC* returnedString = NULL;
    TInt err;  
    TRAP(err, returnedString = StringLoader::LoadLC(resourceId);CleanupStack::Pop());
    if (err == KErrNone)
        {
        err = StringUtils::ConvertPtrUcs2ToUcs2(*returnedString, &pLocal);
        delete returnedString;
        }
    return (TUint16*)pLocal;
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_StartContentLoad
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::NW_UI_StartContentLoad(void *userAgent, const NW_Ucs2* urlName)
{
  CShell* shell = (CShell*)userAgent;
  NW_REQUIRED_PARAM(urlName);
  // reset the  iDoNotRestoreContentFlag 
  shell->UrlLoadStateChanged (TBrCtlDefs::EEventNewContentStart);

  return KBrsrSuccess;
}


// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_StopContentLoad
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::NW_UI_StopContentLoad(void *userAgent, const NW_Ucs2* urlName)
{
  CShell* shell = (CShell*)userAgent;
  NW_REQUIRED_PARAM(urlName);

  // Stop the progressbar
  shell->UrlLoadStateChanged (TBrCtlDefs::EEventLoadFinished);
  // if iNumUpdates == 0 here, probably formatting completed before load finshied
  // EEventContentFinished was not sent ( see CShell::FormattingComplete() )
  // send EEventContentFinished here
  if (shell->iNumRelayouts == 0)
      {
      shell->UrlLoadStateChanged(TBrCtlDefs::EEventContentFinished);
      }
  shell->StartIdle();
  
  return KBrsrSuccess;
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_UrlLoadProgressOn
// ----------------------------------------------------------------------------
TBrowserStatusCode 
CShell::NW_UI_UrlLoadProgressOn(void* wae, NW_Uint16 transactionId)
{
  CShell* shell = (CShell*)wae;
  TInt err;  
  TRAP(err, shell->iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventUrlLoadingStart,
                                                0, (TUint16)(transactionId)));
  return KBrsrSuccess;
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_HttpError
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::NW_UI_HttpError (void *userAgent, 
                         const NW_WaeHttpStatus_t httpError,
                         const NW_Ucs2 *uri,
                         void *callback_ctx,
                         NW_ErrorCB_t *callback)
    {
    NW_REQUIRED_PARAM(userAgent);
    NW_REQUIRED_PARAM(callback_ctx);
    NW_REQUIRED_PARAM(callback);

	TInt err;  
    TRAP(err, Shell()->iBrCtl->brCtlDialogsProvider()->DialogNotifyHttpErrorL(httpError, PtrCFromTText(uri)));
	__ASSERT_DEBUG(err == KErrNone, Panic(err));

	TInt err1;  
    TRAP(err1, Shell()->iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventLoadError, 0, 0));

	if (callback)
	{
		(void) (callback)(callback_ctx, (err == KErrNone ? KBrsrSuccess : KBrsrFailure));
	}

    return (err == KErrNone ? KBrsrSuccess : KBrsrFailure);
    }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_NotifyError
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::NW_UI_NotifyError (void *userAgent, 
                           const TBrowserStatusCode status,
                           void *callback_ctx,
                           NW_ErrorCB_t *callback)
    {
    NW_REQUIRED_PARAM(userAgent);
    NW_REQUIRED_PARAM(callback_ctx);
    NW_REQUIRED_PARAM(callback);

	TInt err;  
    TRAP(err, Shell()->iBrCtl->brCtlDialogsProvider()->DialogNotifyErrorL(status));
    
    TInt err1;  
    TRAP(err1, Shell()->iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventLoadError, 0, 0));

	if (callback)
	{
		(void) (callback)(callback_ctx, (err == KErrNone ? KBrsrSuccess : KBrsrFailure));
	}
  if (err == KErrNoMemory)
    return KBrsrOutOfMemory;

    return (err == KErrNone ? KBrsrSuccess : KBrsrFailure);
    }

// ---------------------------------------------------------------------------- 
// NW_UI_GetUserAuthenticationData
// ---------------------------------------------------------------------------- 
void 
CShell::NW_UI_GetUserAuthenticationDataL(const NW_Ucs2* aUri,
                                         const NW_Ucs2* aUsername,
                                         const NW_Ucs2* aRealm, 
                                         NW_Ucs2** aRetUsername,
                                         NW_Ucs2** aRetPassword,
                                         TBool aBasicAuthentication)
    {
    __ASSERT_DEBUG(aUri != NULL && aUsername != NULL && aRealm != NULL, Panic(KErrArgument));
    __ASSERT_DEBUG(aRetUsername != NULL && aRetPassword != NULL, Panic(KErrArgument));
    
    TBool bRet = EFalse;
    HBufC* returnedUserName = NULL;
    HBufC* returnedPasswd = NULL;
    
    // Initialize our return values
    *aRetUsername = NULL;
    *aRetPassword = NULL;
    TInt needPop = 0;
    
    MBrCtlDialogsProvider* dialogsProvider = iBrCtl->brCtlDialogsProvider();
    
    // Display the User Authentication Dialog
    bRet = dialogsProvider->DialogUserAuthenticationLC(PtrCFromTText(aUri),
        PtrCFromTText(aRealm),
        PtrCFromTText(aUsername),
        returnedUserName,
        returnedPasswd,
        aBasicAuthentication);
    
    // Display the Basic Authentication dialog, if needed.
    if (bRet && aBasicAuthentication && NW_Settings_GetHttpSecurityWarnings())
        {
        // Our memory pointers for the basic authentication dialog 
        HBufC* message = NULL;
        HBufC* okMsg = NULL;
        HBufC* cancelMsg = NULL;
        
        // Set up the message text
        message = StringLoader::LoadLC(R_BROWSER_QUERY_BASIC_AUTHENTICATION);
        
        // Set up the OK button text
        okMsg = StringLoader::LoadLC(R_WML_OK_BUTTON);
        
        // Set up the CANCEL button text
        cancelMsg = StringLoader::LoadLC(R_WML_CANCEL_BUTTON);
        
        // Display the basic authentication dialog
        if (message && okMsg && cancelMsg)
            {
            bRet = dialogsProvider->DialogConfirmL(KEmptyTitle, *message, *okMsg, *cancelMsg);
            }
        
        // Clean up the basic authentication dialog memory
        CleanupStack::PopAndDestroy(3); // message, okMsg, cancelMsg
        
        }   // end of if (bRet && basic authentication && display security warnings)
    
    // The user selected "yes" or "continue", from the dialog(s).
    // Return the username and password.
    if (bRet)
        {
        
        // Set up the returned user name
        if (returnedUserName == NULL)
            {
            *aRetUsername = NULL;
            }
        else
            {
            //lint -e{119} Error -- Too many arguments (2) for prototype 'operator new[]'
            *aRetUsername = new(ELeave) TUint16[returnedUserName->Length() + 1];
            CleanupStack::PushL(*aRetUsername);
            needPop ++;

            TPtr usernamePtr(*aRetUsername, returnedUserName->Length() + 1);
            usernamePtr.Copy(*returnedUserName);
            usernamePtr.ZeroTerminate();
            }
        
        // Set up the returned password
        if (returnedPasswd == NULL)
            {
            *aRetPassword = NULL;
            }
        else
            {
            //lint -e{119} Error -- Too many arguments (2) for prototype 'operator new[]'
            *aRetPassword = new(ELeave) TUint16[returnedPasswd->Length() + 1];
            TPtr passwordPtr(*aRetPassword, returnedPasswd->Length() + 1);
            passwordPtr.Copy(*returnedPasswd);
            passwordPtr.ZeroTerminate();
            }
        }
    else
        {
		TInt err;  
        TRAP(err, iBrCtl->HandleBrowserLoadEventL(TBrCtlDefs::EEventAuthenticationFailed, 0, 0));
        }

    CleanupStack::Pop(needPop); // *aRetUsername
    
    // Clean up the memory from User Authentication dialog
    if (returnedUserName)
        {
        CleanupStack::PopAndDestroy(); // returnedUserName
        }
    if (returnedPasswd)
        {
        CleanupStack::PopAndDestroy(); // returnedPasswd
        }
    if (!bRet)
        {
        User::Leave(KErrCancel);
        }
    }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_GetUserRedirectionData
// ---------------------------------------------------------------------------- 
void 
CShell::NW_UI_GetUserRedirectionData(void*, // wae
								     const NW_Ucs2* /*originalUrl*/, 
								     const NW_Ucs2* /*redirectedUrl*/,
					                 void  *callbackCtx,
						             NW_Wae_UserRedirectionCB_t *callback)
{
	TBool redirect = ETrue;
    TInt err = KErrNone;

	if (callback)
	{
		(void) (callback)(callbackCtx, 
                   (err == KErrNone) ? KBrsrSuccess : KBrsrOutOfMemory, 
                        redirect ? DIALOG_CONFIRM_YES : DIALOG_CONFIRM_NO);
	}
}


// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_DialogListSelect
// ---------------------------------------------------------------------------- 
void 
CShell::NW_UI_DialogListSelect (NW_Dlg_ListSelect_t* data,
                                void* callbackCtx,
                                NW_Dlg_ListSelectCB_t *callback )
  {
#ifndef __SERIES60WIM
  NW_REQUIRED_PARAM(data);
  NW_REQUIRED_PARAM(callbackCtx);
  NW_REQUIRED_PARAM(callback);
#endif

	// See NW_Dlg_ListSelectInit

#ifdef __SERIES60_WIM
	if (data->uiOptions == UI_OPTION_CERT_LIST_SIGN_TEXT)
	{
		TInt err;  
		TRAP(err, Shell()->WimUi()->DoShowSelectCertificateDlgL((void*)data, callbackCtx,(void*)callback));
	}
#endif 
  }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_DialogPrompt
// ---------------------------------------------------------------------------- 
void 
CShell::NW_UI_DialogPrompt (NW_Dlg_Prompt_t* data,
                            void* callbackCtx, 
                            NW_Dlg_PromptCB_t *callback )
  {
	// See NW_Dlg_PromptInit definition to find out the NW_Dlg_Prompt_t definition

	// data->context might be of several types & classes!!!!

#ifdef __SERIES60_WIM
	if (data->uiOptions == UI_OPTION_TEXT_TO_SIGN)
	{
		TInt err;  
		TRAP(err, Shell()->WimUi()->DoShowSignTextDialogL((void*)data, callbackCtx, (void*)callback));
	}
	else
	{ 
#endif 

		TInt err;  
		TRAP(err, Shell()->iBrCtl->brCtlDialogsProvider()->DialogNotifyErrorL(KBrsrWimiNoCard));
    
    if ( callback )
      {
        (void) (callback)( data, callbackCtx );
      }

#ifdef __SERIES60_WIM
	}
#endif 
  }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_DialogInputPrompt
// ---------------------------------------------------------------------------- 
void 
CShell::NW_UI_DialogInputPrompt (NW_Dlg_InputPrompt_t* data,
                                 void* callbackCtx, 
                                 NW_Dlg_InputPromptCB_t *callback )
{
#ifndef __SERIES60WIM
  NW_REQUIRED_PARAM(data);
  NW_REQUIRED_PARAM(callbackCtx);
  NW_REQUIRED_PARAM(callback);
#endif

	// See implementation of NW_Dlg_InputPromptInit to find out the NW_Dlg_InputPrompt_t declaration

#ifdef __SERIES60_WIM
	if (data->uiOptions == UI_OPTION_ENTER_PIN_SIGN_TEXT)
	{
		TInt err;  
		TRAP(err, Shell()->WimUi()->DoShowPinCodeQueryDialogL((void*)data, callbackCtx, (void*)callback));
	}
#endif 
}
  
// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_WmlScriptDlgAlert
// ---------------------------------------------------------------------------- 
/* display a alert dialog to the user */
void 
CShell::NW_UI_WmlScriptDlgAlert (void* wae, 
                                 const NW_Ucs2* message,
                                 void* callback_ctx,
                                 NW_ScrProxy_DlgAlertCB_t* callback )
    {
	TInt status = KErrNone;

    if ( wae )
        {
    NW_WmlScript_ContentHandler_t* thisCH = (NW_WmlScript_ContentHandler_t*)wae;
    NW_HED_DocumentRoot_t* docRoot = 
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisCH);

    CShell* shell = (CShell*) docRoot->browserApp_Ctx;
    NW_ASSERT(shell != NULL);

        TRAP(status, shell->iBrCtl->brCtlDialogsProvider()->DialogNoteL(PtrCFromTText(message)));
		__ASSERT_DEBUG(status == KErrNone, Panic(status));
        }

    if ( callback )
        {
			
//#pragma message(__FILE__" TODO: handle NW_Dlg_Confirm_Result_t arg to NW_ScrProxy_DlgAlertCB_t")		
        (void) (callback)( callback_ctx, (status == KErrNone) ? KBrsrSuccess : KBrsrFailure,
					  DIALOG_CONFIRM_YES /*NW_Dlg_Confirm_Result_t*/);
        }
    }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_WmlScriptDlgConfirm
// ---------------------------------------------------------------------------- 
/* display a confirm dialog to the user */
void 
CShell::NW_UI_WmlScriptDlgConfirm (void *wae,
                                   const NW_Ucs2 *message, 
                                   const NW_Ucs2 *yesMessage,
                                   const NW_Ucs2 *noMessage,
                                   void  *callback_ctx,
                                   NW_Dlg_ConfirmCB_t *callback )
    {
	TInt status;  
    TRAP( status, CShell::NW_UI_WmlScriptDlgConfirmL( wae, message, yesMessage, noMessage,
                                                       callback_ctx, callback) );
    __ASSERT_DEBUG(status == KErrNone, Panic(status));
    }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_WmlScriptDlgConfirmL
// ---------------------------------------------------------------------------- 
/* display a confirm dialog to the user */
void 
CShell::NW_UI_WmlScriptDlgConfirmL( void *wae,
                                    const NW_Ucs2 *message, 
                                    const NW_Ucs2 *yesMessage,
                                    const NW_Ucs2 *noMessage,
                                    void  *callback_ctx,
                                    NW_Dlg_ConfirmCB_t *callback )
    {
    TInt status=KErrNone;
    TBool isConfirmed=EFalse;

    if ( wae )
        {
        HBufC* title = StringLoader::LoadLC( R_QTN_BROWSER_QUERY_SCRIPT_CONFIRM );
        TRAP(status, isConfirmed = 
            Shell()->iBrCtl->brCtlDialogsProvider()->DialogConfirmL(*title,
                                                                    PtrCFromTText(message), 
                                                       PtrCFromTText(yesMessage), 
                                                       PtrCFromTText(noMessage)));
         CleanupStack::PopAndDestroy(); // title
		__ASSERT_DEBUG(status == KErrNone, Panic(status));
        }

//#pragma message(__FILE__" TODO: Handle DIALOG_CONFIRM_END (see rb_wae/src/utils/include/nwx_generic_dlg.h)")		
    if ( callback )
        {
        (void) (callback)(callback_ctx, (status == KErrNone) ? KBrsrSuccess : KBrsrFailure, 
                    isConfirmed ? DIALOG_CONFIRM_YES : DIALOG_CONFIRM_NO );
        }    
    }

/* display a confirm dialog to the user */
// ------------------------------------------------------------------------------- 
//    CShell::NW_UI_DialogSelectOption
// ------------------------------------------------------------------------------- 
void 
CShell::NW_UI_DialogSelectOption (NW_Dlg_SelectData_t* data, 
                                  void* callbackCtx, 
                                  NW_Dlg_SelectCB_t* callback)
{
	TInt err;  
	TRAP(err, NW_UI_DoDialogSelectOptionL(data, callbackCtx, callback));
}

// ------------------------------------------------------------------------------- 
void 
CShell::NW_UI_DoDialogSelectOptionL (NW_Dlg_SelectData_t *data, 
                                     void* callbackCtx, 
                                     NW_Dlg_SelectCB_t* callback )
    {
    /* convert data to EPOC format */
    CArrayFixFlat<TBrCtlSelectOptionData>* options = new (ELeave) CArrayFixFlat<TBrCtlSelectOptionData>(10);
	  CleanupStack::PushL(options);
    TBool selectionChanged= EFalse;

    TInt count = NW_ADT_Vector_GetSize(data->options);

    for ( TInt ii = 0; ii < count; ++ii )
        {
        NW_Dlg_SelectItemData_t* selectItemData;
        
        selectItemData = 
            (NW_Dlg_SelectItemData_t*)NW_ADT_Vector_ElementAt( data->options, 
                                                               (NW_ADT_Vector_Metric_t)ii );
        
        TPtrC selectItemDataText = TPtrC(selectItemData->text);
        TBrCtlSelectOptionData selectOptionData = TBrCtlSelectOptionData(selectItemDataText,
                                                                         selectItemData->isSelected,
                                                                         selectItemData->isOptGroup,
                                                                         selectItemData->hasOnPick);

        options->AppendL( selectOptionData );
        }

    data->isCanceled = NW_FALSE;
    TBrCtlSelectOptionType optionType = data->multiple ? ESelectTypeMultiple : ESelectTypeSingle;

    /* call dialog */
    if (Shell()->iBrCtl->brCtlDialogsProvider()->DialogSelectOptionL(PtrCFromTText(data->title), 
                                                                     optionType, 
                                                  *options ) )
        {
        /* set new selection to KM format */
        for ( TInt ii = 0; ii < count; ++ii )
            {
            NW_Dlg_SelectItemData_t* selectItemData;
        
            selectItemData = 
                (NW_Dlg_SelectItemData_t*)NW_ADT_Vector_ElementAt( data->options, 
                                                                   (NW_ADT_Vector_Metric_t)ii );
            if (selectItemData->isSelected != (NW_Bool)options->At(ii).IsSelected())
              {
              selectionChanged = ETrue;              
              }
            selectItemData->isSelected = (NW_Bool)options->At(ii).IsSelected();
            }
        }
    else 
    {   /* the dialog was cancelled */
        data->isCanceled = NW_TRUE;
    }

    /* we have to call callback even if user cancelled the dialog */    
    if ( callback )
        {
        (void) (callback)( data, callbackCtx , (NW_Bool)selectionChanged);
        }

	CleanupStack::PopAndDestroy(); // options
    }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_WmlScriptDlgPrompt
// ---------------------------------------------------------------------------- 
/* display a prompt dialog to the user */
void 
CShell::NW_UI_WmlScriptDlgPrompt( void* wae,
                                  const NW_Ucs2 *message,
                                  const NW_Ucs2 *defaultInput,
                                  void  *callback_ctx,
                                  NW_ScrProxy_DlgPromptCB_t *callback )
{
  HBufC* returnedInput = NULL;

  TInt err;  
  TRAP( err, CShell::NW_UI_WmlScriptDlgPromptL( wae, message, defaultInput, returnedInput ) );

  __ASSERT_DEBUG( err == KErrNone, Panic(err) );

  // zero-terminate returned value
  HBufC* input = HBufC::New( returnedInput->Length() + 1 );  // +1 is for zero-termination

  const TUint16* pReturnedInput = NULL;

  if (input)
  {
    TPtr pInput(input->Des());
    pInput.Copy( *returnedInput );
    pReturnedInput = pInput.PtrZ();
  }
  delete returnedInput; 

  if (callback)
  {
  //#pragma message(__FILE__" TODO: handle endPressed arg to NW_ScrProxy_DlgPromptCB_t")		
  (void) (callback)( callback_ctx, (err == KErrNone) ? KBrsrSuccess : KBrsrFailure, 
                     pReturnedInput, NW_FALSE );
  }

  delete input;
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_WmlScriptDlgPromptL
// ---------------------------------------------------------------------------- 
/* display a prompt dialog to the user */
void 
CShell::NW_UI_WmlScriptDlgPromptL( void* wae,
                                   const NW_Ucs2 *message,
                                   const NW_Ucs2 *defaultInput,
                                   HBufC*& aReturnedInput )
{
  if (!wae)
    return;

  NW_WmlScript_ContentHandler_t* thisCH = (NW_WmlScript_ContentHandler_t*)wae;
  NW_HED_DocumentRoot_t* docRoot = 
    (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisCH);

  CShell* shell = (CShell*)docRoot->browserApp_Ctx;
  NW_ASSERT(shell != NULL);

  shell->iBrCtl->brCtlDialogsProvider()->DialogPromptLC( KEmptyTitle(),
                                                         PtrCFromTText(message), 
                                                         PtrCFromTText(defaultInput), 
                                                         aReturnedInput );
  CleanupStack::Pop();  //returnedInput 

  if (!aReturnedInput)
  {
    aReturnedInput = KNullDesC().Alloc();
  }

  __ASSERT_DEBUG(aReturnedInput, Panic(KErrNoMemory));
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_Http_ConnNeeded
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::NW_Http_ConnNeeded (TInt* aConnectionPtr,
                            TInt* aSockSvrHandle,
                            TBool* aNewConn,
                            TApBearerType* aBearerType)
	{
	TInt err;  
	TRAP(err,
          Shell()->iBrCtl->brCtlSpecialLoadObserver()->NetworkConnectionNeededL(aConnectionPtr,
                                                                                aSockSvrHandle,
                                                                                aNewConn,
                                                                                aBearerType));

	switch(err)
		{
		case KErrNone:
			return KBrsrSuccess;
		case KErrCancel:
			return KBrsrConnCancelled;
		default:
			return KBrsrConnFailed;
		}
    }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_NotifyCoverageEvent
// ---------------------------------------------------------------------------- 
void 
CShell::NW_UI_NotifyCoverageEvent(TInt aError)
{

  Shell()->CancelAll();
  TInt err;  
  TRAP(err, Shell()->iBrCtl->brCtlDialogsProvider()->DialogNotifyErrorL(aError));
}

// ------------------------------------------------------------------------- 
//    CShell::CurrentUrlLC
// ------------------------------------------------------------------------- 
HBufC* CShell::CurrentUrlLC()
    {
    HBufC* url = NULL;
    url = GetCurrentUrlInHBufCL();
    CleanupStack::PushL(url);
    return url;
    }

// ------------------------------------------------------------------------- 
//    CShell::LinkResolver
// ------------------------------------------------------------------------- 
MBrCtlLinkResolver* CShell::LinkResolver()
    {
    return iBrCtl->brCtlLinkResolver();
    }

// ------------------------------------------------------------------------- 
//    CShell::LoadEmbededUrl
// ------------------------------------------------------------------------- 
CWmlResourceLoadListener* CShell::LoadEmbededUrl( const TDesC& aUrl, void *aLoadContext,
                        void* /*NW_Url_RespCallback_t*/ aPartialLoadCallback, TUint16 aTransId)
	{	
 	CWmlResourceLoadListener* handle = iWmlControl->LoadResourceL(aUrl, aLoadContext, aPartialLoadCallback, aTransId);
	return handle;
	}

// ------------------------------------------------------------------------- 
//    CShell::LoadLinkUrl
// ------------------------------------------------------------------------- 
void CShell::LoadLinkUrl( const TDesC& aUrl, void* aHeaders )
	{
    iWmlControl->SetWmlLink(ETrue);
    HttpRequestHeaders* reqestHeaders = (HttpRequestHeaders*) aHeaders ;
    if( reqestHeaders && reqestHeaders->referer &&
        ( User::StringLength(reqestHeaders->referer) > 0) )
    {

     NW_Ucs2*  buf16 = NULL;
     buf16 = StringUtils::CopyAsciiToUsc2(reqestHeaders->referer);
     if( buf16 )
     {
     CleanupStack::PushL(  buf16 );
     TPtrC16 referer16( buf16 ) ;
     iWmlControl->WKWmlInterface()->wmlLoadUrlL(aUrl, referer16, iWmlControl->AppId(), TBrCtlDefs::ECacheModeNormal);
     CleanupStack::PopAndDestroy( buf16 );
     }
    }
    else
    {
     iBrCtl->LoadUrlL(aUrl, iWmlControl->AppId(), TBrCtlDefs::ECacheModeNormal);
     
    }
    iWmlControl->SetWmlLink(EFalse);
    
	int status = NW_HED_Loader_CancelAllLoads(GetDocumentRoot()->loader);
	// remove the entry from the load queue
    NW_ADT_DynamicVector_t* loadQueue = GetNW_HED_Loader_LoadQueue();
    if(loadQueue)
		{
		int queueSize  = NW_ADT_Vector_GetSize (loadQueue);
		for(int i = 0; i < queueSize; i++)
			{
			status = NW_ADT_DynamicVector_RemoveAt(loadQueue, i);
			}

		//remove the transaction id entry from the loader
		int numRequests = NW_HED_Loader_GetRequestListSize( GetDocumentRoot()->loader );
		for(int i = 0; i < numRequests; i++)
			{
			status = NW_ADT_DynamicVector_RemoveAt( GetDocumentRoot()->loader->requestList, i );
			}
		}
    
 }

// ------------------------------------------------------------------------- 
//    CShell::PostLinkUrl
// ------------------------------------------------------------------------- 
void CShell::PostLinkUrl(TDesC& aUrl, TDesC8& aPostData, TDesC& aContentType)
	{
	iWmlControl->WKWmlInterface()->setPostUrlL(aUrl, aPostData, aContentType );
	}

// ------------------------------------------------------------------------- 
//    CShell::SetOnEnterBackwordReason
// ------------------------------------------------------------------------- 
void CShell::SetOnEnterBackwordReason()
	{
	NW_HED_HistoryStack_t* history = NW_HED_DocumentRoot_GetHistoryStack (iDocumentRoot);    
    if (history != NULL) 
		{
		SetUrlRequestReason(history, NW_HED_UrlRequest_Reason_DocPrev);
		}
	}

// ------------------------------------------------------------------------- 
//    CShell::ShouldResolveUrl
// ------------------------------------------------------------------------- 
TBool CShell::ShouldResolveUrl(TBool aIsTopLevel, NW_Uint8 aReason)
    {
    TInt capabilities = iBrCtl->capabilities();
    // Check the reason, because we do not want the Host Application to resolve a URL for
    // a data buffer that it just passed to us.
    if (aIsTopLevel && aReason != NW_HED_UrlRequest_Reason_ShellLoad)
        {
        if (capabilities & TBrCtlDefs::ECapabilityClientNotifyURL)
            return ETrue;
        }
    else
        {
        if (capabilities & TBrCtlDefs::ECapabilityClientResolveEmbeddedURL)
            return ETrue;
        }
    return EFalse;
    }


// ------------------------------------------------------------------------- 
//    CShell::ConvertCharset
// ------------------------------------------------------------------------- 
TUint16 CShell::ConvertCharset(const TDesC8& aCharset)
    {
	__ASSERT_DEBUG(aCharset.Ptr() != NULL, Panic(KErrArgument));

    TInt16 charset = NW_HED_CompositeContentHandler_Encoding((TUint8*)aCharset.Ptr(), aCharset.Length());
    return charset;

}

// ------------------------------------------------------------------------- 
//    CShell::NW_UI_SoftKey_UpdateSoftKeys
// ---------------------------------------------------------------------------- 
void 
CShell::NW_UI_SoftKey_UpdateSoftKeys(NW_SoftKeyText_t /*softKeyText*/)
    {
    Shell()->iWmlControl->WKWmlInterface()->setUpdateSoftkeys();
    }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_ViewImage
// ---------------------------------------------------------------------------- 
/* view an image in image viewer application */
void 
CShell::NW_UI_ViewImage(void* rawData, NW_Int32 length, NW_Bool isWbmp)
{
  NW_REQUIRED_PARAM(rawData);
  NW_REQUIRED_PARAM(length);
  NW_REQUIRED_PARAM(isWbmp);
}


// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_ConvertFromForeignCharSet
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::NW_UI_ConvertFromForeignCharSet(void* ctx, NW_Url_Resp_t* response)
{
  CShell* shell = (CShell*)(ctx);
  TInt err;  
  TRAP(err, shell->ConvertFromForeignCharSetL(response));
  if(err == KErrNoMemory)
    return KBrsrOutOfMemory;
  if(err == KErrNone)
    return KBrsrSuccess;
  else
    return KBrsrFailure;
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_ConvertFromForeignChunk
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::NW_UI_ConvertFromForeignChunk(void* ctx,
                                      NW_Uint32 foreignEncoding, 
                                      void* inBuf, 
                                      TInt * numUnconvertible, 
                                      TInt* indexFirstUnconvertible, 
                                      void** outBuf)
{
  NW_Uint8 aTryCount = 0;
  CShell* shell = (CShell*)(ctx);
  TInt err;  
  TRAP(err, shell->ConvertFromForeignChunkL(foreignEncoding, inBuf, numUnconvertible, indexFirstUnconvertible, outBuf));
  if(err == KErrNoMemory)
    return KBrsrOutOfMemory;
  if(err == KErrNone)
    return KBrsrSuccess;
  if (err == KBrsrMvcConverterNotFound)
    return KBrsrMvcConverterNotFound;
  else
  {
    // for failed conversion, we try conversion 2 more times
    while (aTryCount++ < 2 && err)
    {
      if (*outBuf && *outBuf != inBuf)
      {
        NW_Buffer_Free((NW_Buffer_t*)(*outBuf));
      }
      TRAP(err, shell->ConvertFromForeignChunkL(foreignEncoding, inBuf, numUnconvertible, indexFirstUnconvertible, outBuf));
    }
    return err; 
  }
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_ConvertFromForeignCharSet
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::NW_UI_ConvertToForeignCharSet(void* ctx, NW_String_t* text)
{
  CShell* shell = (CShell*)(ctx);
  TInt err;  
  TRAP(err, shell->ConvertToForeignCharSetL(text));
  if(err == KErrNoMemory)
    return KBrsrOutOfMemory;
  if(err == KErrNone)
    return KBrsrSuccess;
  else
    return KBrsrFailure;
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_AboutToLoadPageL
// ---------------------------------------------------------------------------- 
/*
TBrowserStatusCode 
CShell::NW_UI_AboutToLoadPageL(NW_Uint32 aNoteType)
    {
    TBool result = EFalse;
    TEnterStatus enterStatus = (TEnterStatus)aNoteType;

    // resId = 0 means accept, don't display a dialog, return KBrsrSuccess
    TInt resId = 0;

    // event = 0 means, don't send an event. NOTE: TBrCtlDefs::EEventNone = 0
    TBrCtlDefs::TBrCtlLoadEvent event = TBrCtlDefs::EEventNone;
    
    // Map TEnterStatus to a corresponding dialog string resource IDs and
    // browser load event
    switch (enterStatus)
        {
        case EExitingSecurePage:
            resId =  R_HTTPSEC_LEAVE_SECURE_SITE;
            event = TBrCtlDefs::EEventExitingSecurePage;
            break;
            
        case EEnteringSecurePage:
            resId = R_HTTPSEC_ENTER_SECURE_SITE;
            event = TBrCtlDefs::EEventEnteringSecurePage;

            // for now, we do not wish to display the confirmation dialog
            // that goes with this event so just call BrowserLoadEvent and return
            Shell()->iBrCtl->HandleBrowserLoadEventL(event, 0, 0);
            return KBrsrSuccess;
            
        case ESomeItemsNotSecure:
            resId = R_HTTPSEC_SOME_ITEMS_NOT_SECURE;
            event = TBrCtlDefs::EEventSomeItemsNotSecure;
            break;
            
        case ESubmittingToNonSecurePage:
            resId = R_HTTPSEC_SUBMITTING_TO_NON_SECURE_PAGE;
            event = TBrCtlDefs::EEventSubmittingToNonSecurePage;
            break;
            
        case ERedirectConfirmation:
            resId = R_HTTP_REDIRECT_POST_CONFIRMATION;
            event = TBrCtlDefs::EEventRedirectConfirmation;
            break;                    
            
        case ERepostConfirmation:
            resId = R_HTTP_REPOST_CONFIRMATION;
            event = TBrCtlDefs::EEventRepostConfirmation;
            break;
            
        case ESecureItemInNonSecurePage:
            resId = 0;
            event = TBrCtlDefs::EEventSecureItemInNonSecurePage;
            break;
            
        default:
            resId = 0;
            event = TBrCtlDefs::EEventNone;
            break;
        }   // end of switch
    
    // Send the BrowserLoadEvent to the registered listeners
    if (event != TBrCtlDefs::EEventNone)
        {
        Shell()->iBrCtl->HandleBrowserLoadEventL(event, 0, 0);
        }
    
    // Show dialogs only if warning dialogs are enabled in the preferences, and
    // if we what to show a dialog (resId != 0).
    // If dialogs are disabled, accept by default
    if (NW_Settings_GetHttpSecurityWarnings() && resId)
      {
        HBufC* message = StringLoader::LoadLC(resId);

        HBufC* leftMessage = NULL;
        HBufC* rightMessage = NULL;
        if( resId == R_HTTP_REDIRECT_POST_CONFIRMATION)
          {
          leftMessage = StringLoader::LoadLC(R_TEXT_SOFTKEY_CONTINUE);
          rightMessage = StringLoader::LoadLC(R_WML_CANCEL_BUTTON);
          }
        else
          {
          leftMessage = StringLoader::LoadLC(R_TEXT_SOFTKEY_YES);
          rightMessage = StringLoader::LoadLC(R_TEXT_SOFTKEY_NO);
          }
        
        MBrCtlDialogsProvider* dialogsProvider = Shell()->iBrCtl->BrCtlDialogsProvider();
		TInt err;  
        TRAP(err, result = dialogsProvider->DialogConfirmL(KEmptyTitle,
                                                            *message,
                                                            *leftMessage,
                                                            *rightMessage));
        // Clean up the memory we allocated, before returning
        CleanupStack::PopAndDestroy(3); // message, leftMessage, rightMessage

        // Check to see if DialogConfirmL leaves
        if (err != KErrNone)
          {
          return KBrsrOutOfMemory;
          }

        // If the user selected "No" or "Cancel", we are cancelling the load
        if (result == EFalse)
          {
            return KBrsrCancelled;
          }

        }   // end of if (NW_Settings_GetHttpSecurityWarnings() )

    // The user selected "Yes" or "Continue" or we aren't displaying a dialog
    return KBrsrSuccess;
  }
*/
// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_SwitchToImgMapView
// ---------------------------------------------------------------------------- 
TInt 
CShell::NW_UI_SwitchToImgMapView()
{
  TInt err = KErrNone;

  err = Shell()->SwitchToImgMapView();
  return err;
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_SwitchFromImgMapViewIfNeeded
// ---------------------------------------------------------------------------- 
TInt 
CShell::NW_UI_SwitchFromImgMapViewIfNeeded()
{
  TInt err = KErrNone;

  err = Shell()->SwitchFromImgMapViewIfNeeded();
  return err;
}

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_IsImageMapView
// ---------------------------------------------------------------------------- 
TBool 
CShell::NW_UI_IsImageMapView()
  {
  return Shell()->iView->IsImageMapView();
  }

// ---------------------------------------------------------------------------- 
//    CShell::NW_UI_ShowObjectDialog
// ---------------------------------------------------------------------------- 
NW_Bool 
CShell::NW_UI_ShowObjectDialog()
{
    TBool ret = EFalse;

    // Create a CBrCtlObjectInfo object and populate it with the current node
    // information.  Used by the embed (plugin) code.
    CBrCtlObjectInfo* objectInfo = Shell()->iWmlControl->WKWmlInterface()->objectInfoCreateL();
    
	

	TInt err;  
    TRAP(err, ret = Shell()->iBrCtl->brCtlDialogsProvider()->DialogDownloadObjectL(objectInfo));
    
    __ASSERT_DEBUG(err == KErrNone, Panic(err));
    delete objectInfo;
    return (NW_Bool)ret;
}


// Following functions are for ECMAScript support
// ---------------------------------------------------------------------------- 
//    CShell::WindowAlert
// ---------------------------------------------------------------------------- 
void 
CShell::WindowAlert(void* browserApp_Ctx, NW_Ucs2* /*msg*/)
  {
  CShell* shell = (CShell*)browserApp_Ctx;

  if (!shell->iExiting)
      {
	  /*HBufC* returnedString = NULL;
	  returnedString = StringLoader::LoadLC(R_QTN_BROWSER_QUERY_SCRIPT_ALERT);

	  TInt status;  
	  TRAP(status, shell->iBrCtl->BrCtlDialogsProvider()->DialogAlertL(*returnedString,
																	  PtrCFromTText(msg)));
 	  __ASSERT_DEBUG(status == KErrNone, Panic(status));
	  
	  CleanupStack::PopAndDestroy();*/
      }
  }

// ---------------------------------------------------------------------------- 
//    CShell::WindowConfirm
// ---------------------------------------------------------------------------- 
NW_Bool 
CShell::WindowConfirm(void* browserApp_Ctx, NW_Ucs2* /*msg*/, NW_Ucs2* /*yesMsg*/, NW_Ucs2* /*noMsg*/)
  {
  TBool isConfirmed=EFalse;
  CShell* shell = (CShell*)browserApp_Ctx;
  if (!shell->iExiting)
      {
  /*HBufC* returnedString = NULL;
  returnedString = StringLoader::LoadLC(R_QTN_BROWSER_QUERY_SCRIPT_CONFIRM);
  TInt status;  
  TRAP(status, isConfirmed = shell->iBrCtl->BrCtlDialogsProvider()->DialogConfirmL(*returnedString,
                                                     PtrCFromTText(msg),
                                                     PtrCFromTText(yesMsg), 
                                                     PtrCFromTText(noMsg)));
  __ASSERT_DEBUG(status == KErrNone, Panic(status));  
  CleanupStack::PopAndDestroy();*/
    
      }
    
  return (NW_Bool)isConfirmed;
  }

// ---------------------------------------------------------------------------- 
//    CShell::WindowPrompt
// ---------------------------------------------------------------------------- 
NW_Ucs2* 
CShell::WindowPrompt(void* browserApp_Ctx, NW_Ucs2* msg, NW_Ucs2* defaultVal)
    {
    NW_Ucs2* returnVal = NULL;

	TInt err;  
    TRAP(err, returnVal = WindowPromptL(browserApp_Ctx, msg, defaultVal));

    return returnVal;
    }

// ---------------------------------------------------------------------------- 
//    CShell::WindowPromptL
// ---------------------------------------------------------------------------- 
NW_Ucs2* 
CShell::WindowPromptL(void* browserApp_Ctx, NW_Ucs2* /*msg*/, NW_Ucs2* /*defaultVal*/)
    {
    HBufC* returnedInput = NULL;
    CShell* shell = (CShell*)browserApp_Ctx;
    NW_ASSERT(shell != NULL);
    if (!shell->iExiting)
        {
    /*HBufC* returnedString = NULL;
	returnedString = StringLoader::LoadLC(R_QTN_BROWSER_QUERY_SCRIPT_PROMPT);
	shell->iBrCtl->BrCtlDialogsProvider()->DialogPromptLC(*returnedString,
                                                          PtrCFromTText(msg), 
                                                          PtrCFromTText(defaultVal), 
                                                          returnedInput);
    CleanupStack::Pop(); //returnedInput
    CleanupStack::PopAndDestroy(); //returnedString*/
        }


    if (!returnedInput)
        {
        returnedInput = KNullDesC().Alloc();
        }

    __ASSERT_DEBUG(returnedInput, Panic(KErrNoMemory));

    // zero-terminate returned value

    TText* input = new TText [returnedInput->Length()*sizeof(TText) + 2];
    const TUint16* pReturnedInput = NULL;

    if (input)
        {
        TPtr pInput(input, returnedInput->Length()*sizeof(TText) + 2);
        pInput.Copy( *returnedInput );
        pReturnedInput = pInput.PtrZ();
        }

    delete returnedInput;
    return (NW_Ucs2*)pReturnedInput;
    }

// ---------------------------------------------------------------------------- 
//    CShell::WindowOpen
// ---------------------------------------------------------------------------- 
void CShell::WindowOpen(void* browserApp_Ctx, NW_Ucs2* url, NW_Ucs2* target, NW_Uint32 reason)
    {
    CShell* shell = (CShell*)browserApp_Ctx;
    NW_ASSERT(shell != NULL);
	TInt ret;
    TRAP(ret, shell->WindowOpenL(url, target, reason));
    }


// ---------------------------------------------------------------------------- 
//    CShell::WindowOpenAndPostL
// ---------------------------------------------------------------------------- 
void CShell::WindowOpenAndPostL(TUint16* url, TUint16* target, TUint32 reason,
                         NW_Uint16 method, TDesC8* contentType, TDesC8* postData,
                         TDesC8* boundary, void* reserved)
    {
    TPtrC urlPtr(url);
    TPtrC targetPtr(NULL);
    if (target)
        {
        targetPtr.Set(target);
        }

    NW_REQUIRED_PARAM(urlPtr.Length());
    NW_REQUIRED_PARAM(targetPtr.Length());
    NW_REQUIRED_PARAM(reason);
    NW_REQUIRED_PARAM(method);
    NW_REQUIRED_PARAM(contentType);
    NW_REQUIRED_PARAM(postData);
    NW_REQUIRED_PARAM(boundary);
    NW_REQUIRED_PARAM(reserved);

    }

// ---------------------------------------------------------------------------- 
//    CShell::WindowOpenL
// ---------------------------------------------------------------------------- 
void CShell::WindowOpenL(TUint16* url, TUint16* target, TUint32 reason)
    {
    WindowOpenAndPostL(url, target, reason, TBrCtlDefs::EMethodGet, NULL, NULL, NULL, NULL);
    }


// ---------------------------------------------------------------------------- 
//    CShell::ConvertToForeignCharSet
// ---------------------------------------------------------------------------- 
TBrowserStatusCode 
CShell::ConvertToForeignCharSet(void* aUcsText)
    {   
	TInt err;  
    TRAP( err, Shell()->ConvertToForeignCharSetL( aUcsText ) );
    if ( err != KErrNone )
        {
        return ( err == KErrNoMemory ) ? KBrsrOutOfMemory : KBrsrFailure;
        }
    return KBrsrSuccess;
    }


// --------------------------------------------------------------------------------
// CShell::CoverageEvent
// --------------------------------------------------------------------------------
void 
CShell::CoverageEvent(TInt aError)
{
     CShell::NW_UI_NotifyCoverageEvent(aError);
}


// -----------------------------------------------------------------------------
// CShell::CreateEmbeddedDocObserver
// -----------------------------------------------------------------------------
MApaEmbeddedDocObserver*
CShell::CreateEmbeddedDocObserver( TFileName aFileName, RFs& aRfs)
    {
    if (!iDocExitObserver)
        {
		TInt err;  
        TRAP( err, iDocExitObserver = CEmbeddedDocExitObserver::NewL(aRfs));
        if (err != KErrNone)
            {
            return NULL;
            }
        }
    iDocExitObserver->SetTempFile(aFileName);
    return (MApaEmbeddedDocObserver*) iDocExitObserver;
    }


void CShell::WmlShellLoadUrlL( const TDesC& aUrl, TInt aAppId )
{
 	if (aUrl.Ptr() == NULL || aUrl.Length() == 0)
        {
        User::Leave(KErrArgument);
        }

    TUint16* url = NULL;
    User::LeaveIfError(StringUtils::ConvertPtrUcs2ToUcs2(aUrl, &url));
    CleanupStack::PushL(url);
    TPtrC urlPtr(url);

	// cancell all loads
	int status = NW_HED_Loader_CancelAllLoads(GetDocumentRoot()->loader);
	// remove the entry from the load queue
    NW_ADT_DynamicVector_t* loadQueue = GetNW_HED_Loader_LoadQueue();
    if(loadQueue)
		{
		int queueSize  = NW_ADT_Vector_GetSize (loadQueue);
		for(int i = 0; i < queueSize; i++)
			{
			status = NW_ADT_DynamicVector_RemoveAt(loadQueue, i);
			}

		//remove the transaction id entry from the loader
		int numRequests = NW_HED_Loader_GetRequestListSize( GetDocumentRoot()->loader );
		for(int i = 0; i < numRequests; i++)
			{
			status = NW_ADT_DynamicVector_RemoveAt( GetDocumentRoot()->loader->requestList, i );
			}
		}
	
	User::LeaveIfError( Load(urlPtr, aAppId , NW_CACHE_NORMAL));
    CleanupStack::PopAndDestroy(); // url
	return;
}//end WmlShellLoadUrlL()

/*lint -restore*/

// --------------------------------------------------------------------------------
// Loaders_InitializeL
// --------------------------------------------------------------------------------
static void Loaders_InitializeL ()
{
    if (NW_Msg_Initialize( "PSQ1" ) != KBrsrSuccess)
    {
        User::Leave(KErrNoMemory);
    }

	// Start up wae
    NW_WaeUsrAgent_t *wae = NULL;
  	if ((wae = NW_WaeUsrAgent_New()) == NULL)
    {
        User::Leave(KErrNoMemory);   
    }
	
	  NW_Ctx_Set(NW_CTX_WML_CORE, 0, wae);
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// --------------------------------------------------------------------------------
// NW_Initialize_Loadercli
// --------------------------------------------------------------------------------
TBrowserStatusCode NW_Initialize_Loadercli ()
{
    TInt ret = KErrNone;
    TRAP(ret, Loaders_InitializeL());
    if (ret != KErrNone)
    {
        Dll::SetTls(NULL); 
    }
    return LoaderUtils::MapErrors(ret);
}
//  End of File
