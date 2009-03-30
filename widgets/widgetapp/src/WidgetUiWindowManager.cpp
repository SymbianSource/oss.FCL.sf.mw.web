/*
* Copyright (c) 2006, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manages widget windows, each window display one widget
*
*/


#include "WidgetUiWindowManager.h"
#include "WidgetUiWindowView.h"
#include "WidgetUiWindowContainer.h"
#include "WidgetUiWindow.h"
#include "WidgetUiAppUi.h"
#include "WidgetInstallerInternalCRKeys.h"
#include "SWInstWidgetUid.h"
#include "widgetappdefs.rh"
#include "Browser_platform_variant.hrh"
#ifdef BRDO_WRT_HS_FF
#include "cpspublisher.h"
#endif

#include <WidgetUi.rsg>
#include <BrCtlInterface.h>
#include <WidgetRegistryConstants.h>
#include <centralrepository.h>
#include <StringLoader.h>
#include <AknNoteDialog.h>
#include <BrowserDialogsProvider.h>
#include <S32FILE.H>
#include <aknnotewrappers.h>
#include "cpglobals.h" // CPS string definitions.

#include <InternetConnectionManager.h>
#include <ActiveApDb.h>


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
    */
    static CGlobalQueryHandlerAO* StartLD (
                            CWidgetUiWindowManager& aManager,
                            CWidgetUiWindow& aWindow,
                            const TDesC& aMessage, 
                            TInt aSoftkeys);
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
    */
    CGlobalQueryHandlerAO (
            CWidgetUiWindowManager& aManager,
            CWidgetUiWindow& aWindow,
            const TDesC& aMessage, 
            TInt aSoftkeys);
        
    /**
    * Destructor. 
    *
    * Private on purpose.
    */
    ~CGlobalQueryHandlerAO();
    
private:

    CWidgetUiWindowManager& iManager;
    CWidgetUiWindow& iWindow;
    CAknGlobalConfirmationQuery* iGlobalConfirmationQuery ;
    CActiveSchedulerWait iScheduler;
    HBufC* iConfirmationText;

    };

// =============================================================================

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::CWidgetUiWindowManager()
// Default constructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowManager::CWidgetUiWindowManager(CWidgetUiAppUi& aAppUi):
    iAppUi(aAppUi),
    iStrictMode(ETrue)
    {
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::ConstructL()
// Symbian constructor
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::ConstructL()
    {
    CRepository* cenRep(NULL);
    
    CCoeEnv::Static()->FsSession().CreatePrivatePath( EDriveC );
    // Create the client-side session
    User::LeaveIfError( iClientSession.Connect() );
    iServerConnected = ETrue;


    TRAPD(error,cenRep = CRepository::NewL( TUid::Uid( KSWInstWidgetUIUid ) ));
    if (!error)
        {
        TInt strictMode;
        if (cenRep->Get( KWidgetInstallerStrictMode, strictMode ) == KErrNone)
            {
            iStrictMode = strictMode;
            }
        delete cenRep;
        }
        
    iDialogsProvider = CBrowserDialogsProvider::NewL( NULL );
    iHandler = CDocumentHandler::NewL(CEikonEnv::Static()->Process());

    iDb = CActiveApDb::NewL( EDatabaseTypeIAP );
    iConnection = CInternetConnectionManager::NewL( iDb->Database(), EFalse );
#ifdef BRDO_WRT_HS_FF    
    iCpsPublisher = CCpsPublisher::NewL();
#endif
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::NewL
// Two-phased constructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowManager* CWidgetUiWindowManager::NewL( CWidgetUiAppUi& aAppUi )
    {
    CWidgetUiWindowManager* self = new ( ELeave ) CWidgetUiWindowManager( aAppUi );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::~CWidgetUiWindowManager()
// Destructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowManager::~CWidgetUiWindowManager()
    {
    iWindowList.ResetAndDestroy();

    // TODO Why there is a "Disconnect" method in the first place...
    // RHandleBase::Close() should be enough?
    if ( iServerConnected )
        {
        iClientSession.Disconnect();
        }
        
    iClientSession.Close();
    
    delete iHandler;
    delete iDialogsProvider;

    if ( iConnection )
        {
        TRAP_IGNORE( iConnection->StopConnectionL() );
        }

    delete iConnection;
#ifdef BRDO_WRT_HS_FF  
    delete iCpsPublisher;
#endif
    delete iDb;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::DeactivateMiniViewL()
// Stop MiniView. Stops publishing and exits widget
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::DeactivateMiniViewL( const TUid& aUid )
    {
    CWidgetUiWindow* wdgt_window = GetWindow(aUid);
    
    if(!wdgt_window)
        return ;
    wdgt_window->SetWindowStateMiniViewL( EMiniViewEnabled );
    SuspendWidget( aUid );

    // TODO also other states are possible when we should react?

    // Removing . Miniview, shall remove full view as well. For blanket permissions
    // will be revoked for miniview

    iClientSession.SetBlanketPermissionL( aUid, EFalse );
    iClientSession.SetMiniViewL( aUid, EFalse );
    CloseWindow( wdgt_window );
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::ActivateMiniView()
// Start publishing offscreenbitmap  to HS.
//
// -----------------------------------------------------------------------------
//
TBool CWidgetUiWindowManager::ActivateMiniViewL( 
    const TUid& aUid, 
    const TRect& aRect )
    {
    TBool res( EFalse );
    CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
    
    __ASSERT_DEBUG( wdgt_window, User::Invariant() );
    
    if ( wdgt_window->WidgetMiniViewState() != EMiniViewNotEnabled )
        {
        // We could throw User::Invariant() if state == EPublishStart, but that would be nasty...
        if ( wdgt_window->WidgetMiniViewState() != EPublishStart )
            {            
            iClientSession.SetMiniViewL( aUid, ETrue );

            HideWindow( iActiveFsWindow );
            //This is done to prevent offscreen bit map overlap 
            //when widget selected from FSW
            wdgt_window->Engine()->MakeVisible( EFalse );       
                     
            if ( !iClientSession.IsBlanketPermGranted ( aUid) )
                {
                AllowPlatformAccessL(aUid);
                }
            else 
                { 
                wdgt_window->SetBlanketPromptDisplayed(ETrue); 
                ResumeWidgetL(aUid); 
 		        }                 
            }
        
        res = ETrue;
        }
        
    return res;
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::HandleWidgetCommandL()
// Open or close widget window
//
// ------------------------------------------------------------------------
//
void CWidgetUiWindowManager::HandleWidgetCommandL( 
    const TUid& aUid,
    TUint32 aOperation )
    {
    switch ( aOperation )
        {
        case LaunchFullscreen:
        case LaunchMiniview:
            {
            OpenOrCreateWindowL( aUid, aOperation );
            }
            break;
        case Deactivate:
            {
            DeactivateMiniViewL( aUid );
            }
            break;
        case WidgetResume:
            {
            ResumeWidgetL( aUid );
            }
            break;
        case WidgetSuspend:
            {
            SuspendWidget( aUid );
            }
            break;
        case WidgetSelect:
            {
            // If we don't have window we know that WidgetUI has died
            // We must enable miniview state
            if( !GetWindow( aUid ))
                {
                OpenOrCreateWindowL( aUid, LaunchMiniview );
                }
            //WidgetLauncher modified to bring app to foreground
            OpenOrCreateWindowL( aUid, LaunchFullscreen );
            }
            break;
        }
    }
    
void CWidgetUiWindowManager::OpenOrCreateWindowL( 
    const TUid& aUid,
    TUint32 aOperation )
    {
    CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
    TBool setFullViewState(EFalse);

#ifdef BRDO_WRT_HS_FF
    TSize miniviewSize( iCpsPublisher->BitmapSize());
#else
    TSize miniviewSize( 0,0 );
#endif
         
    if( wdgt_window ) 
        {
        // This might come from app shell as well where appshell window needs to be resized
        // then active window should be set
        //  if active window is there hide it
        // set the new window as current and active. Chcek if window needs resizing and set the registry as fullView and active
        // can be checked , we can launch in full view if Rect size is same as view size


        if ( aOperation == LaunchMiniview )
            {
            if ( !ActivateMiniViewL( aUid, miniviewSize ) )
                {
                wdgt_window = NULL;
                }
            }

        // If the existing widget is in miniview state, we need to make it full-screen.
        // Sanity check that we don't have to do anything extra.
        else 
            {
#ifdef BRDO_WRT_HS_FF
            //Make sure that all the mini widgets are already suspended
            SuspendAllWidget();
#endif
            // TODO Combine these methods somewhere in WidgetUiWindow, so it knows
            // how to tamper its own internal state. For example MakeFullScreen().
           
            setFullViewState = ETrue;
            iClientSession.SetFullViewL( aUid, ETrue );
            }
        }
    else
        {
        // Create a new window.
        if ( !IsWidgetAllowedL( aUid ) )
            {
            ShowNoteDialogL( R_WIDGETUI_STRICT_MODE_ACCESS );
            
            iAppUi.SendAppToBackground();
            }
        else 
            {
            // Does the widget support miniviews, in case that is desired?
            if ( aOperation == LaunchFullscreen || DoesWidgetSupportMiniviewL( aUid ) )
                {
                if ( aOperation == LaunchFullscreen )
                    iClientSession.SetFullViewL( aUid, ETrue );
                // Create window.
                wdgt_window = CreateNewWidgetWindowL( aUid );
            
                if ( aOperation )
                    {
                    // Don't care if this returns, we know miniviews are supported.
                    TRAPD( err, ActivateMiniViewL( aUid, miniviewSize ) );
                    if ( err )
                        {
                        CloseWindow( wdgt_window );
                        wdgt_window = NULL;
                        }
                    }
                else
                    {
#ifdef BRDO_WRT_HS_FF
                    SuspendAllWidget();
#endif
                    setFullViewState = ETrue;
                   
                    }
                }
            }
        }
        
    // Windows are up & running. Hopefully.
    if ( wdgt_window  )
        {
        if ( iActiveFsWindow != wdgt_window )
            {
            HideWindow( iActiveFsWindow );
            }
        
        if( setFullViewState )
            {
            iActiveFsWindow = wdgt_window;
            iActiveFsWindow->SetWindowStateFullView(ETrue);
            iActiveFsWindow->SetCurrentWindow( ETrue );
            if ( iActiveFsWindow->Engine()->Rect() != View()->ClientRect())
                {
                iActiveFsWindow->Engine()->SetRect( View()->ClientRect() );
                }
            //iActiveFsWindow->Engine()->MakeVisible( ETrue );
            
            }
        }
    iClientSession.SetActive( aUid, ETrue );
    }

// =============================================================================
// CWidgetUiWindowManager::AllowPlatformAccessL()
// Prompt for network access allow
//
// =============================================================================
//
void CWidgetUiWindowManager::AllowPlatformAccessL( const TUid& aUid )
    {
    CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
    if( !wdgt_window)
        return ;
     
    HBufC* confirmationText = StringLoader::LoadLC(R_WIDGETUI_PLEASE_WORK);
    TInt softKey = R_AVKON_SOFTKEYS_OK_CANCEL;
    CGlobalQueryHandlerAO* tmp = NULL;
    TRAP_IGNORE(  tmp = CGlobalQueryHandlerAO::StartLD( *this, *wdgt_window , confirmationText->Des(), softKey ) );
    CleanupStack::PopAndDestroy(confirmationText);  
    }

// =============================================================================
// CWidgetUiWindowManager::GetWindow()
// return the window of a widget with a particular url
//
// =============================================================================
//
CWidgetUiWindow* CWidgetUiWindowManager::GetWindow( const TDesC& aUrl )
    {
    for ( TInt i = 0; i < iWindowList.Count(); i++ )
        {
        CWidgetUiWindow* window = iWindowList[i];
        if ( window->Url()->Des() == aUrl )
            {
            return window;
            }
        }
    return NULL;
    }

// =============================================================================
// CWidgetUiWindowManager::GetWindow()
// return the window of a widget with a particular Uid
//
// =============================================================================
//
CWidgetUiWindow* CWidgetUiWindowManager::GetWindow( const TUid& aUid )
    {
    for ( TInt i = 0; i < iWindowList.Count(); i++ )
        {
        CWidgetUiWindow* window = iWindowList[i];
        if ( window->Uid() == aUid )
            {
            return window;
            }
        }
    return NULL;
    }

// =============================================================================
// CWidgetUiWindowManager::CloseWindow()
// close window of widget
//
// =============================================================================
//
void CWidgetUiWindowManager::CloseWindow( CWidgetUiWindow* aWidgetWindow )
    {

    TBool lastOne( iWindowList.Count() == 1 );
    
    RemoveFromWindowList( aWidgetWindow );
     
    if ( !lastOne )
        {
        // Starting JS timer, since we stopped it for deactivating miniview widget
        CWidgetUiWindow* window = iWindowList[iWindowList.Count() - 1];
        if ( window)
               TRAP_IGNORE ( window->Engine()->HandleCommandL( 
                              (TInt)TBrCtlDefs::ECommandAppForeground + 
                              (TInt)TBrCtlDefs::ECommandIdBase));

        for ( TInt i = 0; i < iWindowList.Count(); ++i )// Fix needed. Do we need onShow here.
            {
            CWidgetUiWindow* window( iWindowList[i] );
            if ( window->WidgetMiniViewState() == EPublishStart )
                {
				TRAP_IGNORE (window->WidgetExtension()->HandleCommandL(
				                (TInt)TBrCtlDefs::ECommandAppForeground +   
								(TInt)TBrCtlDefs::ECommandIdBase));  

                    break;
                }
            }

        }
    }

// =============================================================================
// CWidgetUiWindowManager::RemoveFromWindowList()
// remove widget window from window list
//
// =============================================================================
//
void CWidgetUiWindowManager::RemoveFromWindowList( CWidgetUiWindow* aWidgetWindow )
    {
    __ASSERT_DEBUG( aWidgetWindow, User::Invariant() );
    if ( iDialogsProvider->IsDialogLaunched() )
        {
        return;
        }

    if ( iClientSession.IsWidgetInFullView ( aWidgetWindow->Uid()))
        {
        HideWindow( aWidgetWindow );
        if (aWidgetWindow == iActiveFsWindow)
            iActiveFsWindow = NULL;
        // make widgets act like separate applications by pushing to background
        // when widget is in fullview , this way user is sent back to app shell or idle 
        // to run another widget
        iAppUi.SendAppToBackground();
        }

    // update the status of the widget to not active.
    TRAP_IGNORE(iClientSession.SetFullViewL( aWidgetWindow->Uid(), EFalse ));
    iClientSession.SetActive( aWidgetWindow->Uid(), EFalse );

    iWindowList.Remove( iWindowList.Find( aWidgetWindow ) );
    TBool lastOne( iWindowList.Count() == 0 );
    if ( lastOne )
        {
        TRAP_IGNORE( aWidgetWindow->Engine()->HandleCommandL( 
                (TInt)TBrCtlDefs::ECommandIdBase +
                (TInt)TBrCtlDefs::ECommandDisconnect ) );

        delete aWidgetWindow;
        iAppUi.Exit();
        }
    else
        {
        delete aWidgetWindow;
        }
    }

// =============================================================================
// CWidgetUiWindowManager::CloseWindow()
// close window of widget with a particular Uid
//
// =============================================================================
//
void CWidgetUiWindowManager::CloseWindow( const TUid& aUid )
    {
    CWidgetUiWindow* widgetWindow = GetWindow( aUid );
    CloseWindow( widgetWindow );
    }

// =============================================================================
// CWidgetUiWindowManager::CloseAllWindowsExceptCurrent()
// close all window except the current widget
//
// =============================================================================
//
void CWidgetUiWindowManager::CloseAllWindowsExceptCurrent()
    {
    TInt idx(0);
    SuspendAllWidget();
    while (iWindowList.Count() > 1)
        {
        CWidgetUiWindow* window = iWindowList[idx];
        TRAP_IGNORE(iClientSession.SetMiniViewL( window->Uid(), EFalse ));
        if(CheckIfWindowNeedsToBeClosed(window))
            {
            RemoveFromWindowList( window );
            }
        else
            {
            idx++;// skip ActiveWindow
            }
        }
    }

// =============================================================================
// CWidgetUiWindowManager::CheckIfWindowNeedsToBeClosed()
// return true for the widgets that needs to be closed
//
// =============================================================================
//
TBool CWidgetUiWindowManager::CheckIfWindowNeedsToBeClosed(CWidgetUiWindow* aWindow) const
    {
    __ASSERT_DEBUG( aWindow, User::Invariant() );

    if( aWindow == iActiveFsWindow )
        {
        if ( aWindow->WidgetMiniViewState() == EPublishStart ||
             aWindow->WidgetMiniViewState() == EPublishSuspend )
            {
            // Incase when the widget is active and as in full as well as miniview. 
            // it will stop publishing
            TRAP_IGNORE(aWindow->SetWindowStateMiniViewL( EMiniViewEnabled ));
            }
        return EFalse;
        }
    return ETrue;
    }

// =============================================================================
// CWidgetUiWindowManager::Exit()
// Exit from widget and close widget window
//
// =============================================================================
//
void CWidgetUiWindowManager::Exit( TInt aCommand, const TUid& aUid )
    {
    CWidgetUiWindow* window( GetWindow( aUid ) );
    if( !window )
        return;
    if( ( window->WidgetMiniViewState() == EMiniViewEnabled ) ||
        ( window->WidgetMiniViewState() == EMiniViewNotEnabled ) ) 
        {
        // The widget is not publishing.
        if ( window->WidgetExtension() )
            {
            if ( window->WidgetExtension()->HandleCommandL( aCommand ) )
                return;
            }

        CloseWindow( window );
        if( window == iActiveFsWindow)
            iActiveFsWindow = NULL;
        }
    else
        {
        ExitPublishingWidget( aUid );
        }
    }

// =============================================================================
// CWidgetUiWindowManager::ExitPublishingWidget()
// Exit from widget in full view when it is publishing
//
// =============================================================================
//
void CWidgetUiWindowManager::ExitPublishingWidget( const TUid& aUid )
    {
    CWidgetUiWindow* window( GetWindow( aUid ) );
    if( !window )
        return;

    // make widgets act like separate applications by pushing to background
    // this way user is sent back to app shell or idle to run another widget
    iAppUi.SendAppToBackground();

    if ( iWindowList.Count() == 0 )
        {
        iAppUi.Exit(); //TODO Check if it is required for publishin widget
        }
    window->Engine()->MakeVisible( EFalse );
    window->SetWindowStateFullView( EFalse );
    //  Add registry info
    TRAP_IGNORE(iClientSession.SetFullViewL( window->Uid(), EFalse ));
    }

// =============================================================================
// CWidgetUiWindowManager::SetActiveWidgetAtExit()
// Set active widget to widget in full view which is not added to homescreen 
// incase no such widget is there set it to null
//
// =============================================================================
//
void CWidgetUiWindowManager::SetActiveWidgetAtExit()
    {
    //Get widget in Fullview that is not publishing and not same as active window
    for ( TInt i = 0; i < iWindowList.Count(); ++i )
        {
        CWidgetUiWindow* window( iWindowList[i] );
        if ( ( window != iActiveFsWindow ) &&   
             ( window->WidgetFullViewState() ) )
            {
            iActiveFsWindow = window;
            //ShowWindow( window );
            return;
            }
        }
        
    iActiveFsWindow = NULL;
    }

// =============================================================================
// CWidgetUiWindowManager::HandleLSKcommandL()
// Execute LSK Handler if present
//
// =============================================================================
//
TBool CWidgetUiWindowManager::HandleLSKCommandL( TInt aCommand )
    {
    if ( iActiveFsWindow && iActiveFsWindow->WidgetExtension() )

        {
         return iActiveFsWindow->WidgetExtension()->HandleCommandL( aCommand );
       }
    return EFalse;

    }

// =============================================================================
// CWidgetUiWindowManager::GetWidgetEntryForUrl()
// return Uid of widget with a particular Url
//
// =============================================================================
//
TInt CWidgetUiWindowManager::GetWidgetEntryForUrl( const TDesC& aUrl ) //const
    {
    return iClientSession.GetWidgetUidForUrl( aUrl );
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SetDisplayLandscape()
// switch the display orientation to landscape
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SetDisplayLandscape( )
    {
    TRAP_IGNORE(iAppUi.SetDisplayLandscapeL());
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SetDisplayPortrait()
// switch the display orientation to portrait
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SetDisplayPortrait( )
    {
    TRAP_IGNORE(iAppUi.SetDisplayPortraitL());
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SetDisplayAuto()
// switch the display orientation to device defined
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SetDisplayAuto( )
    {
    iAppUi.SetDisplayAuto();
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::HandleForegroundEvent
// called when application goes to background or comes back to foreground
//
// This method is called only for "Go to background" events. "Come to foreground"
// is filtered away by CWidgetUiAppUi::HandleForegroundEventL .
// But we need to check the aForeground value anyway since flipping back and
// forth from powersave mode doesn't involve OpenOrCreateWindowL.
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::HandleForegroundEvent( TBool aForeground )
    {
    if ( aForeground )
        {
        ShowWindow( iActiveFsWindow );
        }
    else
        {
        HideWindow( iActiveFsWindow );
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::HandleOOMEventL
// called when out of memory message is received by app ui
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::HandleOOMEventL( TBool /*aForeground*/ )
    {
    HBufC* message = StringLoader::LoadLC( R_WIDGETUI_OOM_EVENT );
    CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
    note->ExecuteLD(*message );
    CleanupStack::PopAndDestroy( message );// message
    
    CloseWindowsAsync( ETrue );// close all widgets
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::CloseWindowsAsync
// close all windows or current window async
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::CloseWindowsAsync( TBool aAllWindows )
    {
    iAppUi.AsyncExit(aAllWindows);
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SuspendWidget
// Suspend currently publishing widget (if so).
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SuspendWidget( const TUid& aUid )
    {
    CWidgetUiWindow* wdgt_window = GetWindow(aUid);

    if(!wdgt_window)
        return;

    if( (wdgt_window->WidgetMiniViewState() == EPublishStart) &&
        (wdgt_window->WidgetMiniViewState() != EPublishSuspend))
        {
        TRAP_IGNORE(
        wdgt_window->SetWindowStateMiniViewL(EPublishSuspend);

        wdgt_window->WidgetExtension()->HandleCommandL( 
            (TInt)TBrCtlDefs::ECommandAppBackground + 
            (TInt)TBrCtlDefs::ECommandIdBase);

        wdgt_window->Engine()->HandleCommandL( 
            (TInt)TBrCtlDefs::ECommandAppBackground + 
            (TInt)TBrCtlDefs::ECommandIdBase);
            );

        }

    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::ResumeWidget
// Resume publishing stuff, in case it is valid for the widget.
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::ResumeWidgetL( const TUid& aUid )
    {
    CWidgetUiWindow* wdgt_window( GetWindow( aUid ) );
    
    // Window can be null if WidgetUI has been killed due to low memory situation
    //__ASSERT_DEBUG( wdgt_window, User::Invariant() );
    if(!wdgt_window || !wdgt_window->GetBlanketPromptDisplayed())
        return;
    
    if( (wdgt_window ->WidgetMiniViewState() == EMiniViewEnabled) ||
        (wdgt_window->WidgetMiniViewState() == EPublishSuspend) )
        {
        //HideWindow( iActiveFsWindow );
        //iActiveFsWindow = NULL;
        // Publish should start only after widget is resumed.
        wdgt_window->SetWindowStateMiniViewL(EPublishStart);

        wdgt_window->Engine()->HandleCommandL( 
            (TInt)TBrCtlDefs::ECommandAppForeground + 
            (TInt)TBrCtlDefs::ECommandIdBase);

        wdgt_window->WidgetExtension()->HandleCommandL(
            (TInt)TBrCtlDefs::ECommandAppForeground + 
            (TInt)TBrCtlDefs::ECommandIdBase);
#ifdef BRDO_WRT_HS_FF  
        wdgt_window->Engine()->SetRect( iCpsPublisher->BitmapSize());
#endif
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowManager::SuspendAllWidget
// SUspend all publishing widgets, if not already done.
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowManager::SuspendAllWidget()
    {
    for ( TInt i = 0; i < iWindowList.Count(); i++ )
        {
        CWidgetUiWindow* window = iWindowList[i];
        if ( window->WidgetMiniViewState() == EPublishStart)
            {
            SuspendWidget(window->Uid());
            }
        }

    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::HideWindow
//
// Hide currently active window.
// ------------------------------------------------------------------------
void CWidgetUiWindowManager::HideWindow( CWidgetUiWindow* aWindow )
    {
    if ( aWindow )
        {
        // Hide the previously active widget.
        aWindow->SetCurrentWindow(EFalse);
        /*        
            if( iActiveFsWindow->WidgetMiniViewState() == EPublishSuspend ) 
                {
                iClientSession.SetFullView( aWindow->Uid(), EFalse );
                aWindow->SetWindowStateFullView( EFalse );   
                }
           */     
        }
    }
 
// ------------------------------------------------------------------------
// CWidgetUiWindowManager::IsWidgetAllowedL
// 
// Check widget's compatibility for running.
// ------------------------------------------------------------------------
TBool CWidgetUiWindowManager::IsWidgetAllowedL( 
    const TUid& aUid ) //const
    {
    __UHEAP_MARK;
    
    TBool res( EFalse );
    CWidgetPropertyValue* value( iClientSession.GetWidgetPropertyValueL(
        aUid, 
        ENokiaWidget ) );
    
    if ( iStrictMode )
        {
        if ( *value )
            {
            res = ETrue;
            }
        }
    else
        {
        res = *value;
        }
    
    delete value;
    
    __UHEAP_MARKEND;
    return res;
    }
 
// ------------------------------------------------------------------------
// CWidgetUiWindowManager::ShowNoteDialogL
// 
// Show a note to user.
// ------------------------------------------------------------------------
void CWidgetUiWindowManager::ShowNoteDialogL( TInt aResourceID ) const
    {
    CEikDialog* dialog( NULL );
    HBufC* message( StringLoader::LoadLC( aResourceID ) );
    
    dialog = new (ELeave) CAknNoteDialog( 
            &dialog,
            CAknNoteDialog::ENoTone, 
            CAknNoteDialog::ELongTimeout );
    
    CleanupStack::PushL( dialog );
    static_cast< CAknNoteDialog* >( dialog )->SetTextL( *message );
    CleanupStack::Pop( dialog );
    
    CleanupStack::PopAndDestroy( message );
    dialog->ExecuteLD(R_WIDGETUI_OK_NOTE);
    delete dialog;
    dialog = NULL;
    }

// ------------------------------------------------------------------------
// CWidgetUiWindowManager::CreateNewWidgetWindowL
//
// New widget, new window.
// ------------------------------------------------------------------------
CWidgetUiWindow* CWidgetUiWindowManager::CreateNewWidgetWindowL( 
    const TUid& aUid )
    {
#ifdef BRDO_WRT_HS_FF  
    CWidgetUiWindow* window(
        CWidgetUiWindow::OpenWindowL( *this, aUid, iCpsPublisher ) );
#else
    CWidgetUiWindow* window(
        CWidgetUiWindow::OpenWindowL( *this, aUid, NULL ) );
#endif
    
    CleanupStack::PushL( window );

    // Add it to the queue
    iWindowList.AppendL( window );
    
    CleanupStack::Pop( window );
    
    // reset the display orientation when the widget is launched
    iAppUi.SetDisplayAuto();

    window->ReloadWidget();
    
    return window;
    }
    
// ------------------------------------------------------------------------
// CWidgetUiWindowManager::DoesWidgetSupportMiniviewL
//
// Register widget to CPS listener.
// ------------------------------------------------------------------------
TBool CWidgetUiWindowManager::DoesWidgetSupportMiniviewL( const TUid& aUid )
    {
    __UHEAP_MARK;
    CWidgetPropertyValue* val( iClientSession.GetWidgetPropertyValueL( aUid, EMiniViewEnable ) );
    TBool res( *val );
    
    delete val;
    
    __UHEAP_MARKEND;
    
    return res;
    }

void CWidgetUiWindowManager::ShowWindow( CWidgetUiWindow* aWindow )
    {
    if ( !aWindow )
        return;
    if ( aWindow != iActiveFsWindow )
        {
        HideWindow( iActiveFsWindow );
        }
        
    iActiveFsWindow = aWindow;
    iActiveFsWindow->SetCurrentWindow( ETrue );
    iActiveFsWindow->Engine()->MakeVisible( ETrue );
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::StartLD
//
// Initialize AO.
// ------------------------------------------------------------------------
CGlobalQueryHandlerAO* CGlobalQueryHandlerAO::StartLD(
    CWidgetUiWindowManager& aManager,
    CWidgetUiWindow& aWindow,
    const TDesC& aMessage, 
    TInt aSoftkeys)
    {
    CGlobalQueryHandlerAO* self( new( ELeave ) CGlobalQueryHandlerAO( aManager, aWindow, aMessage, aSoftkeys) );
    TRAPD(error, self->ShowGlobalQueryDialogL ( aMessage, aSoftkeys ));
    if ( error )
        {
        delete self;
        User::Leave(error);
        }
    self->SetActive();
    self->iScheduler.Start();
    return self;
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::CGlobalQueryHandlerAO
//
// Constructor.
// ------------------------------------------------------------------------
CGlobalQueryHandlerAO::CGlobalQueryHandlerAO(
    CWidgetUiWindowManager& aManager,
    CWidgetUiWindow& aWindow,
    const TDesC& aMessage, 
    TInt aSoftkeys):CActive( EPriorityHigh ),
    iWindow ( aWindow ), 
    iManager(aManager),
    iConfirmationText(aMessage.AllocL())
    {
    CActiveScheduler::Add( this );
    }

// ------------------------------------------------------------------------
// CGlobalQueryHandlerAO::CGlobalQueryHandlerAO
//
// ISet network and platofrom access permission based on user response.
// ------------------------------------------------------------------------
void CGlobalQueryHandlerAO::RunL()
    {
    if (iScheduler.IsStarted())
        {
        iScheduler.AsyncStop();
        }
    
    RWidgetRegistryClientSession clientSession = iManager.WidgetUIClientSession();	
    if (iStatus == EAknSoftkeyOk)
        {
        clientSession.SetBlanketPermissionL( iWindow.Uid(), ETrue );
        }
    else if ( iStatus == EAknSoftkeyCancel)
        {
        //iWindow.SetNetworkAccessGrant(EDeny);
        clientSession.SetBlanketPermissionL( iWindow.Uid(), EFalse );
        //User::Leave( KErrAccessDenied );
        }
    iWindow.SetBlanketPromptDisplayed(ETrue); 
	iManager.ResumeWidgetL(iWindow.Uid());        
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
    iScheduler.AsyncStop();
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

// End of file





