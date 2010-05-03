/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MVCSHELL_H
#define __MVCSHELL_H


#include <apmstd.h>
#include "BrsrStatusCodes.h"
#include "BrsrTypes.h"
#include <badesca.h>
#include <bldvariant.hrh>
#include <charconv.h>
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <ApEngineConsts.h>

#include "BrCtlDialogsProvider.h"

#include "HEDDocumentListener.h"
#include "httploader_mlinkresolvercallbacks.h"
#include "MSGMessageListener.h"
#include "MVCViewListener.h"
#include "nw_hed_appservices.h"
#include "nw_errnotify.h"
#include "nw_scrproxy.h"
#include "nw_wae.h"
#include "nwx_defs.h"
#include "nwx_status.h"
#include "urlloader_urlresponse.h"
#include "EmbeddedDocExitObserver.h"
//R
#include "HistoryStack.h"
//#include "webkitview.h"
#include "WmlControl.h"


#ifdef __SERIES60_WIM
#include "WimUi.h"
#endif __SERIES60_WIM


enum TUrlLoadState
  {
  // We are now loading the URL
  EUrlLoadingProgressing,

  // The loading progress has finished because of successful
  // finish, user cancel or some other reason.
  EUrlLoadingStopped,

  // Reset iDoNotRestoreContentFlag in the BrowserUI
  EResetDoNotRestoreContentFlag
  };

class CDataLoadInfo;

const TInt KPanicKmShellPanicBase = -10000;
const TInt KPanicKmShellSingletonAlreadyCreated = KPanicKmShellPanicBase - 1;
const TInt KPanicKmShellViewPtrIsNotSet = KPanicKmShellPanicBase - 2;

class CView;
class CCnvCharacterSetConverter;
class CContentLoader;
class CBrCtl;

NONSHARABLE_CLASS(CShell): public CBase, public MHttpLoader_LinkResolverCallbacks,
               public MViewListener, public MHEDDocumentListener, public MMessageListener
{
  public: // Epoc constructor and destructor

    static CShell* NewL (CView* aEpoc32View, CBrCtl* aBrCtl, MWmlInterface* aWKWmlInterface, CWmlControl* aWmlControl);

    ~CShell();


  public: // Public exported methods

    /**
     *
     * Starts loading of content from given URL.
     *
     * @param URL to be loaded
     * @return KErrNone if successfull, KErrGeneral on error
     */
    TInt Load(const TDesC& aUrl, TInt aApId, NW_Cache_Mode_t cacheMode=NW_CACHE_NORMAL);

    TInt CancelAll();

    TInt FreeConnection();
    TInt HistoryLoad(THistoryStackDirection aDirection);

    /**
     *
     * Process the Back event
     * Now that we have multiple views (normal view and image map view), the processing
     * of the back event may be different depending on which view we are in.  If we
     * are in the image map view, we will want to switch back to the normal view if the
     * back button is selected.  If we are in normal view, we will want to call HistoryLoad
     * and load the previous page.
     *
     * @param None
     * @return KErrNone if successfull, KErrUnknown on error
     */
    TInt ProcessBack();

     /**
     *
     * Switch to image map view
     * Now that we have multiple views (normal view and image map view).If we select
     * an image map we want to switch to image map view
     *
     * @param None
     * @return KErrNone if successfull, KErrUnknown on error
     */
    TInt SwitchToImgMapView();

     /**
     *
     * Switch from image map view
     * Now that we have multiple views (normal view and image map view).If we
     * are in the image map view, we will want to switch back to the normal view.
     *
     * @param None
     * @return KErrNone if successfull, KErrUnknown on error
     */
    TInt SwitchFromImgMapViewIfNeeded();

    void SetView(CView* aEpocView); // Does not take the ownership!

    TInt HandleDo( TUint32 aElemId );

    /**
     *
     * Get a history list
     *
     * @param aUrls Descriptor array to append
     * @return Error code
     */
    TInt GetHistoryList (CArrayFix<TBrCtlSelectOptionData>& aHistoryList);

    /**
     *
     * Gives current position in the history stack
     *
     * @return THistoryStackPosition enumeration 
     */
    TWmlEngineDefs::THistoryStackPosition HistoryStackPosition();

    /**
     * Loads page from history by given index
     *
     * @param history list
     * @return KErrNone if found, otherwise KErrNotFound
     */
    TInt LoadHistoryEntry( CArrayFix<TBrCtlSelectOptionData>& aHistoryList );

    /**
    *
    * Gets the current URL as HBufC*, Leaves on error
    * Returns at least zero-length HBufC on success
    *
    * @return HBufC* Current URL
    */

    HBufC* GetCurrentUrlInHBufCL() const;

    /**
    *
    * Gets the current title of the content as HBufC*, leaves on error
    * Returns at least zero-length HBufC on success
    *
    * @return HBufC* Current WML deck / XHTML Page <title> element contents
    */
    HBufC* GetCurrentTitleInHBufCL(TBool& aStripUrl) const;

    TUint16 SaveToFileL(TDesC& aFileName) const;

	/**
    *
    * Gets the WML of the domtree
    * Returns pointer to HBufC
    * 
    * @return HBufC* extracted WML MarkUp
    */

	HBufC8* GetPageAsWMLMarkUp();


    /**
    *
    * Gets the text of the domtree
    * Returns pointer to HBufC
    * 
    * @return HBufC* extracted text	
    */
    HBufC* GetPageAsTextL();


    /**
    *
    * Gets the charset from the domtree
    * Returns pointer to the charset
    * 
    * @return charset
    */
	const NW_Uint8* GetCharsetFromResponse();

    /**
    *
    *
    */
    void ShowImagesL() const;
    
    /**
    *it must be called when the browser view loses focus
    */
    void LoseFocus();
    
    /**
    *it must be called when the browser view gains focus
    */
    void GainFocus(); 

    /**
    *provide for browser to reset the history list, when close softkey is selected.
    */
    void ClearHistoryList();

    // Called when selected character set changes
    void CharacterSetChangedL(TUint aSelectedCharacterSet);
    
    void* GetCertInfo();

  public: // public not exported methods


    /**
    * From MHttpLoader_LinkResolverCallbacks Called to obtain the current URL.
    * @since 2.8
	* @param 
    * @return The current URL
    */
    HBufC* CurrentUrlLC();

    /**
    * From MHttpLoader_LinkResolverCallbacks Called to obtain a pointer to the Link Resolver
    * @since 2.8
    * @return The Browser Control's Link Resolver
    */
    MBrCtlLinkResolver* LinkResolver();
    
    /**
    * From MHttpLoader_LinkResolverCallbacks Called to check if the Host Application should be called to resolve the URL
    * @since 2.8
    * @return ETrue if the Host Application will resolve the URL, EFalse otherwise
    */
    TBool ShouldResolveUrl(TBool aIsTopLevel, NW_Uint8 aReason);
    
    /**
    * From MHttpLoader_LinkResolverCallbacks Called to call LoadUrlL
    * @since 3.0
    * @return void
    */
    void LoadLinkUrl(const TDesC& aUrl, void* aHeaders );
	
	/**
    * From MHttpLoader_LinkResolverCallbacks Called to call PostUrlL
    * @since 3.0
    * @return void
    */
	void PostLinkUrl(TDesC& aUrl, TDesC8& aPostData, TDesC& aContentType);
	
	/**
    * From MHttpLoader_LinkResolverCallbacks Called to load Embeded content
    * @since 3.1
    * @return void
    */
	CWmlResourceLoadListener* LoadEmbededUrl( const TDesC& aUrl, void *aLoadContext,
                        void* aPartialLoadCallback, TUint16 aTransId  );

	/**
    * From MHttpLoader_LinkResolverCallbacks Called to set the OEB event
    * @since 3.1
    * @return void
    */
	void SetOnEnterBackwordReason();
	
    /**
    * From MHttpLoader_LinkResolverCallbacks Convert a charset from string to IANA encoding
    * @since 2.8
    * @return IANA encoding of the charset
    */
    TUint16 ConvertCharset(const TDesC8& aCharset);

    TUint ConvertFromForeignCharSetL(void* response);
    TUint ConvertFromForeignChunkL(  NW_Uint32 foreignEncoding, void* inBuf, TInt * numUnconvertible, TInt* indexFirstUnconvertible, void** outBuf);
    static TBrowserStatusCode ConvertToForeignCharSet(void* ucsText);  
    TUint ConvertToForeignCharSetL(void* text);
    NW_Text_t* ConvertToValidUrl (NW_Text_t* url, TBrowserStatusCode* status);
		/**
		* From MHttpLoader_ToUiCallbacks Called when the user need to be notified with an error
		* @since 2.0
		* @return void
		*/
    void CoverageEvent (TInt aError);
    CBrCtl* BrCtl() {return iBrCtl;}
		/**
		* From MHttpLoader_ToUiCallbacks Called after disconnect is complete.
		* @since 2.0
		* @param 
		* @return void
		*/
		void Disconnect_Resp() {};
    
    TBrowserStatusCode DocumentChanged (NW_HED_DocumentRoot_t* documentRoot, 
      CActive::TPriority aMessagePriority );

    TBrowserStatusCode DocumentChangedPartial (NW_HED_DocumentRoot_t* documentRoot, 
      CActive::TPriority aMessagePriority );

    NW_Uint32 GetCharsetUid(NW_Uint32 internalEncoding);
    NW_Uint32 GetCharsetUid2(NW_Uint32 ianaEncoding);
    NW_Uint32 GetUidFromCharset( NW_Uint8* charset, NW_Uint32 charsetLength );
    NW_Text_t* GetCurrentUrl ();
    NW_Text_t* GetCurrentUrlL ();
    const NW_Text_t* GetCurrentTitle ();
    inline NW_HED_DocumentRoot_t* GetDocumentRoot() { return iDocumentRoot; };
    NW_LMgr_RootBox_t* GetRootBox ();
    TBrowserStatusCode GetUseNumberBuf (NW_ADT_DynamicVector_t* dynamicVector, void** ptr );
    TBrowserStatusCode GetUseNumberContent (NW_ADT_DynamicVector_t *dynamicVector);
    inline CView* GetView() { return iView; };
	TBool HandleDownloadL(RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray);
    void HandleError (TBrowserStatusCode status);
    void FormattingComplete ();
    void HandleMessage (NW_Msg_Message_t* message);
    TBrowserStatusCode IsActive (NW_HED_DocumentRoot_t* documentRoot, NW_Bool state);
    NW_Bool LoadEnd (NW_HED_DocumentRoot_t* documentRoot, NW_Int16 errorClass, NW_WaeError_t error);
    TBrowserStatusCode LoadProgressOn (NW_Uint16 transactionId);
    TBrowserStatusCode LoadProgressOff (NW_Uint16 transactionId);
    void NewDocument (NW_HED_DocumentRoot_t* documentRoot, NW_HED_UrlRequest_t* urlRequest, NW_Bool aDestroyBoxTree);
    void DestroyPreviousDocument();
    void RestoreDocument ();
    void NotifyProgress (TBrCtlDefs::TBrCtlLoadEvent aEvent, TUint32 aSize, TUint16 aTransactionId);
    MBrCtlSpecialLoadObserver* GetLoadObserver ();
    static TBrowserStatusCode NW_Http_ConnNeeded (TInt* aConnectionPtr, TInt* aSockSvrHandle, TBool* aNewConn, TApBearerType* aBearerType);
//    static TBrowserStatusCode NW_UI_AboutToLoadPageL( NW_Uint32 aNoteType );
    static TBrowserStatusCode NW_UI_ConvertFromForeignCharSet (void* ctx, NW_Url_Resp_t* response);
    static TBrowserStatusCode NW_UI_ConvertToForeignCharSet (void* ctx, NW_String_t* text);
    static TBrowserStatusCode NW_UI_ConvertFromForeignChunk ( void* ctx, NW_Uint32 foreignEncoding, void* inBuf, TInt * numUnconvertible, TInt* indexFirstUnconvertible, void** outBuf);
    static void NW_UI_DialogListSelect (NW_Dlg_ListSelect_t* data, void* callbackCtx, NW_Dlg_ListSelectCB_t *callback );
    static void NW_UI_DialogPrompt (NW_Dlg_Prompt_t* data, void* callbackCtx, NW_Dlg_PromptCB_t *callback);
    static void NW_UI_DialogInputPrompt (NW_Dlg_InputPrompt_t *data, void* callbackCtx, NW_Dlg_InputPromptCB_t *callback );
    static void NW_UI_DialogSelectOption (NW_Dlg_SelectData_t *data, void* callbackCtx, NW_Dlg_SelectCB_t* callback );
    /* Return localized string for 'back', 'reset', 'select', 'submit' */
    static NW_Ucs2* NW_UI_GetLocalizedString (NW_Uint32 type);
    void NW_UI_GetUserAuthenticationDataL(const NW_Ucs2* aUri, const NW_Ucs2* aUsername, const NW_Ucs2* aRealm, 
                                          NW_Ucs2** aRetUsername, NW_Ucs2** aRetPassword,
                                          TBool aBasicAuthentication = EFalse);
    static void NW_UI_GetUserRedirectionData (void* wae, const NW_Ucs2* originalUrl, const NW_Ucs2* redirectedUrl,
												                      void  *callbackCtx, NW_Wae_UserRedirectionCB_t *callback);
    static TBrowserStatusCode NW_UI_HttpError (void *userAgent, const NW_WaeHttpStatus_t httpError, 
                                               const NW_Ucs2 *uri, void *callback_ctx, NW_ErrorCB_t *callback);
     /**
     *
     * Query if we are in image map view
     * We have multiple views (image map view or normal view)  
     *
     * @param None
     * @return KErrNone if successfull, KErrUnknown on error
     */
    static TBool NW_UI_IsImageMapView();
    static void NW_UI_NotifyCoverageEvent (TInt aError);
    static TBrowserStatusCode NW_UI_NotifyError (void *userAgent, const TBrowserStatusCode status, void *callback_ctx, NW_ErrorCB_t *callback);
    // Display the object info dialog box
    static NW_Bool NW_UI_ShowObjectDialog();
    static void NW_UI_SoftKey_UpdateSoftKeys (NW_SoftKeyText_t softKeyText);
    static TBrowserStatusCode NW_UI_StartContentLoad (void *userAgent, const NW_Ucs2* urlName);
    static TBrowserStatusCode NW_UI_StopContentLoad (void *userAgent, const NW_Ucs2* urlName);
    static TInt NW_UI_SwitchFromImgMapViewIfNeeded();
    static TInt NW_UI_SwitchToImgMapView();
    static TBrowserStatusCode NW_UI_UrlLoadProgressOn (void *wae, NW_Uint16 transactionId);
    static void NW_UI_ViewImage (void* rawData, NW_Int32 length, NW_Bool isWbmp);
    /* display a alert dialog to the user */
    static void NW_UI_WmlScriptDlgAlert (void* wae, const NW_Ucs2* message, void* callback_ctx, NW_ScrProxy_DlgAlertCB_t* callback );
    /* display a confirm dialog to the user */
    static void NW_UI_WmlScriptDlgConfirm (void *wae, const NW_Ucs2 *message, const NW_Ucs2 *yesMessage,
                                           const NW_Ucs2 *noMessage, void  *callback_ctx, NW_Dlg_ConfirmCB_t *callback );
    /* display a prompt dialog to the user */
    static void NW_UI_WmlScriptDlgPrompt (void *wae, const NW_Ucs2 *message, const NW_Ucs2 *defaultInput,
                                          void  *callback_ctx, NW_ScrProxy_DlgPromptCB_t *callback );
    void OptionListInit(); // just forwards call to CEpoc32View
    void OptionListAddItem( const TText* aText, const TUint32 aElemID, const TInt aElType ); // just forwards call to CEpoc32View
    NW_Bool ReportError (NW_HED_DocumentRoot_t* documentRoot, NW_Int16 errorClass, NW_WaeError_t error,
                         ReportErrorCallback* callback);
    TBrowserStatusCode SendRedrawMsg (NW_Bool drawNow);
    TBrowserStatusCode SendRelayoutMsg (NW_Bool createTabList);
	  TUint SelectedCharacterSet() const {return iSelectedCharacterSet;}
    TBrowserStatusCode SetAnchorName (NW_Text_t* anchorName);
    void SetFileOpenedByDocHandler( TBool aFileOpenedByDocHandler );
    TBrowserStatusCode ShowNamedBox (NW_LMgr_Box_t* boxTree, const NW_Text_t* targetName);
	/**
	* From MHttpLoader_ToUiCallbacks Called after shutdown is complete.
	* @since 2.0
	* @param 
	* @return void
	*/
	void Shutdown_Resp() {};
    TBrowserStatusCode SwitchLayout (NW_Bool aVerticalLayout, NW_Bool aReformatBoxTree);
    void UpdateSoftKeys();
    void UpdateTitle();
    void UrlLoadStateChanged (TBrCtlDefs::TBrCtlLoadEvent aEvent);

    // Following functions are for ECMAScript support 
    static void WindowAlert (void* browserApp_Ctx, NW_Ucs2* msg);
    static NW_Bool WindowConfirm (void* browserApp_Ctx, NW_Ucs2* msg, NW_Ucs2* yesMsg, NW_Ucs2* noMsg);
    static NW_Ucs2* WindowPrompt (void* browserApp_Ctx, NW_Ucs2* msg, NW_Ucs2* defaultVal); 

    static NW_Ucs2* WindowPromptL (void* browserApp_Ctx, NW_Ucs2* msg, NW_Ucs2* defaultVal); 
    static void WindowOpen(void* browserApp_Ctx, NW_Ucs2* url, NW_Ucs2* target, NW_Uint32 reason);

     /**
    * Request the host applicaion to open the URL in a new window
    * @since 3.0
    * @param aUrl The Url of the request to be done in the new window
    * @param aTargetName The name of the new window
    * @param aReason The reason for opening the new window
    * @param aMethod The method to be used for fetching the supplied url
    * @param aContentType If the method is POST, this is the content type of the data to be posted 
    * @param aPostData If the method is POST, this is the data to be posted 
    * @param aBoundary The boundary used if the content is multipart/form-data
    * @param aReserved For future use
    * @return Return Value is for future reference and is currently ignored
    */
    void WindowOpenAndPostL(TUint16* url, TUint16* target, TUint32 reason, 
        TUint16 method, TDesC8* contentType, TDesC8* postData,
        TDesC8* aBoundary, void* reserved);

     /**
    * 
    * @since 3.0
    * @param url
    * @param target
    * @param reason
    */
    void WindowOpenL(TUint16* url, TUint16* target, TUint32 reason);

    TBool Exiting() {return iExiting;};

    // Support for AboutToLoadPage and the UI
    TBool SecureItemsInNonSecurePage() { return iSecureItemsInNonSecurePage; }
    TBool NonSecureItemsInSecurePage() { return iNonSecureItemsInSecurePage; }

    TInt GetCharsetAndCharsetStringFromUid(TUint aUid, TUint16* aCharset,
                                           const TUint8** aCharsetString);
    TBrowserStatusCode StartDataLoad (TDesC& aUrl, CDataLoadInfo* aDataLoadInfo,
        NW_MVC_ILoadListener_t* loadListener, TUint16* aTransId, 
        void* aOwner, void* aClientData, void* aLoadRecipient, TUint8 aLoadType);    

    TBrowserStatusCode PostUrl(const TDesC& aUrl, 
        const TDesC8& aContentType, const TDesC8& aPostData, 
        const TDesC8* aBoundary, void* aReserved);

    static TInt RunBGTask(TAny* aShell);
    void StartIdle() {if (!iIdle->IsActive()) iIdle->Start(TCallBack(RunBGTask, this));}

    /**
    * Create observer to delete temp files when the embedded application exists
    * @param TFilename Name and path for the temporary file
    * @return NULL if creation failed;
    */
    MApaEmbeddedDocObserver* CreateEmbeddedDocObserver( TFileName aFilename, RFs& aRfs);
    
  void WmlShellLoadUrlL( const TDesC& aUrl, TInt aAppId );
  public: // Public member variables
    TBool                  iEmbedded;
    TInt                   iApId; 
    CContentLoader*        iContentLoader;
    RFs                    iRfs;
  
  private: // Private methods

    CShell( CView* aEpoc32View, CBrCtl* aBrCtl );
    void ConstructL (TBool aUseWsp, MWmlInterface* aWmlInterface, CWmlControl* aWmlControl );
    TBrowserStatusCode DisplayDocument ();
    void DisplayError (NW_Int16 errorClass, NW_WaeError_t error);
    TBrowserStatusCode InitComponents ();
    TBrowserStatusCode InitializeClass (const NW_Osu_Hwnd_t hWnd);
    // check whether the charset is supported and whether its char converter is in the ROM
    NW_Bool IsConversionSupported(NW_Uint32 uid, NW_Bool * ConverterAvailable);
    static void NW_UI_DoDialogSelectOptionL( NW_Dlg_SelectData_t *data, void* callbackCtx, 
                                             NW_Dlg_SelectCB_t* callback );
    /* display a confirm dialog to the user */
    static void NW_UI_WmlScriptDlgConfirmL (void *wae, const NW_Ucs2 *message, const NW_Ucs2 *yesMessage,
                                            const NW_Ucs2 *noMessage, void  *callback_ctx, NW_Dlg_ConfirmCB_t *callback );
    static void NW_UI_WmlScriptDlgPromptL( void* wae, const NW_Ucs2 *message, const NW_Ucs2 *defaultInput,
                                           HBufC*& aReturnedInput );
    TInt SetBoxTree( NW_LMgr_Box_t* aBoxTree ); // just forwards call to CEpoc32View
    TBrowserStatusCode StartLoad (const NW_Text_t* url, NW_MVC_ILoadListener_t* loadListener, NW_Cache_Mode_t cacheMode);

  private: // Private member variables

    CView* iView;
    CBrCtl*                iBrCtl;
    TBool                  iIsEpoc32ShellDestroyed;// The instance is set to true in the  
                                                   // destructor, and is used not to delegate 
    CCnvCharacterSetConverter* iCharacterSetConverter;
    TUint iSelectedCharacterSet;
    TUint iCurrentConverter; 
    CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* iArrayOfCharacterSetsAvailable;

    NW_MVC_ILoadListener_t* iLoadListener;
    NW_Osu_Hwnd_t iHWnd;
    NW_HED_DocumentRoot_t* iDocumentRoot;
    NW_HED_DocumentNode_t* iPrevDocument;
    NW_Uint16 iNumUpdates;
    NW_Int16 iNumUpdatesPartial;
    NW_Uint16 iNumRelayouts;
    NW_Uint16 iNumRedraws;
    NW_Bool iDisplayDocumentInvoked ;
    TBool iSecureItemsInNonSecurePage;
    TBool iNonSecureItemsInSecurePage;

    NW_LMgr_Box_t* iBoxTree;

    /* Do background tasks */
    CIdle* iIdle;

    CEmbeddedDocExitObserver* iDocExitObserver;
    TBool iExiting;

    // flag to indicate the file processed by document handler
    // e.g. unknowCH invoke docHanlder to open a .txt file
    TBool iFileOpenedByDocHandler;
    CWmlControl* iWmlControl;
};

#endif
