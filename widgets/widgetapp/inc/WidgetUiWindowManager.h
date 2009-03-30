/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef WIDGETUIWINDOWMANAGER_H_
#define WIDGETUIWINDOWMANAGER_H_

// INCLUDES FILES

#include <WidgetRegistryClient.h>
#include <aknglobalconfirmationquery.h>
#include <coecntrl.h>
#include "WidgetUiObserver.h"
#include "Browser_platform_variant.hrh"
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDocumentHandler;
class CWidgetUiWindowView;
class CWidgetUiWindow;
class CWidgetUiAppUi;
class CWidgetUiCpsObserver;
class CAknNoteDialog;
class CBrowserDialogsProvider;
class CBrCtlInterface;
class CLiwServiceHandler;
class MLiwInterface;
class CInternetConnectionManager;
class CSchemeHandler;

#ifdef BRDO_WRT_HS_FF
class CCpsPublisher;
#endif
// CLASS DECLARATION

/**
*  CWidgetUiWindowManager
*  @lib WidgetUi.app
*  @since 3.1
*/
class CWidgetUiWindowManager : public CBase
    {
    public:  // constructors / destructor

        /**
        * NewL
        * Two-phased constructor.
        * @since 3.1
        * @param none
        * @return CWidgetUiWindowManager*
        */
        static CWidgetUiWindowManager* NewL( CWidgetUiAppUi& aAppUi );

        /**
        * ~CWidgetUiWindowManger
        * Destructor
        * @since 3.1
        * @param none
        * @return none
        */
        virtual ~CWidgetUiWindowManager();

    public:  

// Create window
        
        /**
        * HandleWidgetCommandL
        * @since 5.0
        * @param aUid UID of widget.
        * @param aOperation Whether to open or close the widget, and how.
        */
        void HandleWidgetCommandL( 
            const TUid& aUid,
            TUint32 aOperation );

// Suspend/resume miniview stuff.        
        /**
        * Suspend a widget.
        * @since 5.0
        * @param aUid UID of widget.
        */
        void SuspendWidget( const TUid& aUid );
        
        /**
        * Resume a suspended widget.
        * @since 5.0
        * @param aUid UID of widget.
        */
        void ResumeWidgetL( const TUid& aUid );

        /**
        * Suspend all the publishing widget.
        * @since 5.0
        * @return none.
        */
        void SuspendAllWidget();

        /**
        * Stop mini view and exit
        * @since 5.0
        * @param aUrl
        * @return none
        */
        void DeactivateMiniViewL( const TUid& aUid );
 
        /**
        * Suspend publishing on home screen
        * @since 5.0
        * @param aUid UID of widget.
        * @param aSize Size of miniview.
        * @return Successful or not.
        */
        TBool ActivateMiniViewL( const TUid& aUid, const TRect& aRect );
        
// Close / terminate widgets.
        
        /**
        * CloseWindow
        * Closes the window of the widget identified by a Uid
        * @since 3.1
        * @param aUid - Uid to identify the widget
        * @return none
        */
        void CloseWindow( const TUid& aUid );

        /**
        * CloseWindow
        * Closes the window of the widget
        * @since 3.1
        * @param aWidgetWindow - window of the widget
        * @return none
        */
        void CloseWindow( CWidgetUiWindow* aWidgetWindow );

         /**
        * RemoveFromWindowList
        * Remove  the widget window from window list
        * @since 5.0
        * @param aWidgetWindow - window of the widget
        * @return none
        */
        void RemoveFromWindowList( CWidgetUiWindow* aWidgetWindow );

        /**
        * CloseWindowsAsync
        * Close window async
        * @param aAllWindows ETrue closes all windows; EFalse closes active window
        * @since 3.1
        */
        void CloseWindowsAsync( TBool aAllWindows );
        
        /**
        * CloseAllWindowsExceptCurrent
        * Closes all of the window except the current widget
        * @since 3.1
        * @return none
        */            
        void CloseAllWindowsExceptCurrent();

        /**
        * CheckIfWindowNeedsToBeClosed
        * Returns false only for current window
        * @since 5.0
        * @return true if widget needs to be closed
        */ 
        TBool CheckIfWindowNeedsToBeClosed(CWidgetUiWindow* aWindow) const;
        
        /**
        * Exit the widget, if the widget is the only widget running, exit the application
        * otherwise, close the widget window and activate the next widget window who is running
        * @since 3.1
        */
        void Exit( TInt aCommand , const TUid& aUid);

        /**
        * SetActiveWidgetAtExit
        * Sets the FullView window which is not publishing to active
        * @since 5.0
        */ 
        void SetActiveWidgetAtExit();

// Query/set states.

        /**
        * SetDisplayLandscape
        * Called to change the display orientation to landscape
        * @since 3.1
        */
        void SetDisplayLandscape( );
        
        /**
        * GetWidgetEntryForUrl
        * Returns the UId of widget entry with a particular Url
        * @since 3.1
        * @param none
        * @return UId
        */
        TInt GetWidgetEntryForUrl( const TDesC& aUrl ); //const;
        
        /**
        * ActiveWindow
        * @since 3.1
        * @param none
        * @return CWidgetUiWindow*
        */
        inline CWidgetUiWindow* ActiveWindow() const { return iActiveFsWindow; }

        /**
        * GetWindow
        * Gets the window for certain widget identified by a URL
        * @since 3.1
        * @param aUrl - a URL to identify the widget
        * @return the widget running, null if not found
        */
        CWidgetUiWindow* GetWindow( const TDesC& aUrl );

        /**
        * GetWindow
        * Gets the window for certain widget identified by a Uid
        * @since 3.1
        * @param aUid - Uid to identify the widget
        * @return the widget running, null if not found
        */
        CWidgetUiWindow* GetWindow( const TUid& aUid );

        /**
        * SetView
        * @since 3.1
        * @param aContentView
        * @return void
        */
        void SetView( CWidgetUiWindowView& aContentView ) { iContentView = &aContentView; }

        /**
        * View
        * @since 3.1
        * @param none
        * @return CWidgetUiWindowView*
        */
        CWidgetUiWindowView* View() const { return iContentView; }
     
        /**
        * SetDisplayPortrait
        * Called to change the display orientation to portrait
        * @since 3.1
        */
        void SetDisplayPortrait( );
        
        /**
        * SetDisplayAuto
        * Called to change the display orientation to device defined
        * @since 3.1
        */
        void SetDisplayAuto( );

        /**
        * HandleForegroundEvent
        * Called when application goes to background or comes back to foreground
        * @since 3.1
        */
        void HandleForegroundEvent( TBool aForeground );

        /**
        * HandleOOMEventL
        * Called when out of memory message is received by app ui
        * @since 3.1
        */
        void HandleOOMEventL( TBool aForeground );

        /**
        * AllowPlatformAccessL
        * Prompt for Allow Platform Access
        * @since 5.0
        */
        void AllowPlatformAccessL( const TUid& aUid );

// Utility stuff.

        /**
        * WindowList
        * Gets the list of running windows
        * @since 3.1
        * @param none
        * @return CArrayPtrFlat<CWidgetUiWindow>*
        */
        void WindowList( RPointerArray<CWidgetUiWindow>& )  { /*return iWindowList; */ User::Invariant(); }

        /**
        * WidgetUIClientSession
        * @since 3.1
        * @param none
        * @return RWidgetUIRegistryClientSession*
        */
        RWidgetRegistryClientSession& WidgetUIClientSession() { return iClientSession; }

        /**
        * DialogsProvider
        * @since 3.1
        * @param none
        * @return CBrowserDialogsProvider
        */
        CBrowserDialogsProvider* DialogsProvider() { return iDialogsProvider; }
        
        /**
         * HandleLSKCommandL
         * @since 5.0
         * @param TInt - command to execute
         * @return TBool - True if LSK handler is executed
         */
        TBool HandleLSKCommandL( TInt aCommand );
        
        /**
        * DocHandler
        * @since 5.0
        * @return Document Handler.
        */
        inline CDocumentHandler& DocHandler() { return *iHandler; }

        /**
        * GetConnection
        * @since 5.0
        * @return CInternetConnectionManager
        */
        CInternetConnectionManager* GetConnection() {return iConnection;}

        /**
        * GetDb
        * @since 5.0
        * @return CActiveApDb
        */
        CActiveApDb*  GetDb() {return iDb;}
       
    protected:

        /**
        * CWidgetUiWindowManager
        * Constructor
        * @since 3.1
        * @param none
        * @return none
        */
        CWidgetUiWindowManager( CWidgetUiAppUi& aAppUi );

        /**
        * ConstructL
        * EPOC default constructor
        * @since 3.1
        * @param none
        * @return void
        */
        void ConstructL();

    private:

        /**
        * Hide the window.
        * @param aWindow Window. Ownership is retained.
        */
        void HideWindow( CWidgetUiWindow* aWindow );
        
        /** 
        * Is the widget allowed to run or not
        *
        * @param aUid out-param, contains the UID of widget aon return.
        */
        TBool IsWidgetAllowedL( const TUid& aUid );// const;
            
        /**
        * Notify user if widget is not allowed to run.
        *
        * @param aResourceID String identifier.
        */        
        void ShowNoteDialogL( TInt aResourceID ) const;
            
        /**
        * Create new window for a widget and add it to window list.
        *
        * @param aUid UID
        * @return Newly created window.
        */
        CWidgetUiWindow* CreateNewWidgetWindowL( const TUid& aUid );
        
        /**
        * Check if widget is supposed to support miniviews.
        *
        * @param aUid UID.
        * @return Whether widgets are supported or not.
        */
        TBool DoesWidgetSupportMiniviewL( const TUid& aUid );
		
    		/**
        * OpenOrCreateWindowL
        * @since 5.0
        * @param aUid UID of widget.
        * @param aOperation Whether to open widget to miniview or full screen.
        */
        void OpenOrCreateWindowL( 
            const TUid& aUid,
            TUint32 aOperation );
            
        /**
        * Set the given window as "active full-screen window".
        * The method handles swapping of active window.
        * @since 5.0
        * @param aWindow Window. Onwership is retained.
        */
        void ShowWindow(
            CWidgetUiWindow* aWindow );
            
        /**
        * Exit Publishing Widget
        * @since 5.0
        * @param aUid
        * @return none
        */
        void ExitPublishingWidget( const TUid& aUid );

    private:

        CWidgetUiWindow*                    iActiveFsWindow;    // reference.
        CWidgetUiWindowView*                iContentView;   // not owned, not responsible for deleting
        RPointerArray<CWidgetUiWindow>      iWindowList;    // owned, responsible for deleting
        CWidgetUiAppUi&                     iAppUi;          // not owned, not responsible for deleting.
        CDocumentHandler*                   iHandler;           // own        
        RWidgetRegistryClientSession        iClientSession;     // owned
        TBool                               iServerConnected;   // connected to Widget Registry server ?
        TBool                               iStrictMode;
        CBrowserDialogsProvider*            iDialogsProvider;// owned, responsible for deleting
        CInternetConnectionManager*         iConnection;        // owned, responsible for deleting 
#ifdef BRDO_WRT_HS_FF       
        CCpsPublisher*                      iCpsPublisher;      // Owned, interface to publish bitmap to CPS
#endif
        // TODO should this be created only when needed?
        CActiveApDb*                        iDb;                // owned, responsible for deleting

    };

#endif  // WIDGETUIWINDOWMANAGER_H_

// End of File
