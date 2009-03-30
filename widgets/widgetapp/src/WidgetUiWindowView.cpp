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
* Description: 
*
*/

#include <AknUtils.h>
#include <eikmenub.h>
#include <barsread.h>
#include "WidgetUiWindowView.h"
#include "WidgetUiWindowContainer.h"
#include "WidgetUiWindowManager.h"
#include "WidgetUiWindow.h"
#include <WidgetUi.rsg>
#include <Widgetmenu.rsg>
#include <AknViewAppUi.h>
#include <avkon.RSG>
#include "WidgetUi.hrh"
#include <aknenv.h>
#include <aknindicatorcontainer.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <aknEditStateIndicator.h>
#include <stringloader.h>
#include <e32math.h>
#include <AknSgcc.h>
#include <akntitle.h>
#include <centralrepository.h>
#include <BrowserUiSDKCRKeys.h>
#ifdef RD_SCALABLE_UI_V2
#include <LayoutMetaData.cdl.h> // For Layout_Meta_Data landscape/portrait status
#include <akntoolbar.h>
#endif

// CONSTANTS
    const TInt KRightSoftkeyPosition = 2;
    const TInt KLeftSoftkeyPosition = 0;

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::CWidgetUiWindowView()
// C++ default constructor can NOT contain any code, that
// might leave.
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowView::CWidgetUiWindowView( CWidgetUiWindowManager& aWindowManager )
    : iWindowManager( aWindowManager ),
    iActivatedObjectVisible(ETrue),
    iIsOptionsMenuActivated(EFalse)
    {
    iPenEnabled = AknLayoutUtils::PenEnabled();
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::ConstructL()
// Symbian constructor
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowView::ConstructL( )
    {
    BaseConstructL( R_WIDGETUI_WINDOW_VIEW );

    ConstructMenuAndCbaEarlyL();
    Cba()->AddCommandSetToStackL(R_WIDGETUI_WINDOW_VIEW_SOFTKEYS);

    iContainer = CWidgetUiWindowContainer::NewL(TRect(), iWindowManager);
    iContainer->SetMopParent( this );
#ifdef RD_SCALABLE_UI_V2
    if (iPenEnabled && AppUi()->CurrentFixedToolbar())
        {
        AppUi()->CurrentFixedToolbar()->SetToolbarVisibility(EFalse);
        }
#endif
    ShowActivatedObject(EFalse);

    // Navipane
    iNaviCont = (CAknNavigationControlContainer*)StatusPane()->ControlL(
        TUid::Uid( EEikStatusPaneUidNavi ) );
    iNaviDecorator = iNaviCont->CreateEditorIndicatorContainerL();
    iIndiContainer = ( CAknIndicatorContainer* )iNaviDecorator->DecoratedControl();

    iNaviCont->PushL( *iNaviDecorator );

    CAknTitlePane* title =
        STATIC_CAST( CAknTitlePane*, StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetTextL( KNullDesC() );

    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::NewLC()
// Two-phase constructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowView* CWidgetUiWindowView::NewLC( CWidgetUiWindowManager& aWindowManager )
    {
    CWidgetUiWindowView* view = new (ELeave) CWidgetUiWindowView( aWindowManager );
    CleanupStack::PushL(view);
    view->ConstructL();

    return view;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::~CWidgetUiWindowView()
// Destructor
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowView::~CWidgetUiWindowView()
    {
    DoDeactivate();
    if (iNaviCont && iNaviDecorator)
        {
        iNaviCont->Pop( iNaviDecorator );
        }
    delete iNaviDecorator;
    delete iContainer;
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::Container()
// Called by framework when the view size is changed
//
// -----------------------------------------------------------------------------
//
CWidgetUiWindowContainer* CWidgetUiWindowView::Container()
    {
    return iContainer;
    }


// -----------------------------------------------------------------------------
// CWidgetUiWindowView::Id()
// return Id
//
// -----------------------------------------------------------------------------
//
TUid CWidgetUiWindowView::Id() const
    {
    return TUid::Uid( 50 );
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::HandleCommandL()
// Handles user commands
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowView::HandleCommandL( TInt aCommand )
    {
    if (!iWindowManager.ActiveWindow() ||
        !iWindowManager.ActiveWindow()->Engine() || !iContainer)
        {
        return;
        }

    DeActivateOptionsMenu();

    TBrCtlDefs::TBrCtlElementType elementtype =
            iWindowManager.ActiveWindow()->Engine()->FocusedElementType();

    TBool editing = (elementtype == TBrCtlDefs::EElementActivatedInputBox) ||
                    (elementtype == TBrCtlDefs::EElementActivatedObjectBox);

    switch( aCommand )
        {
        case EAknCmdExit:
        case EEikCmdExit: // Options->Exit
        case EAknSoftkeyExit: // Right Softkey (Exit or JS defined)
            ShowActivatedObject(EFalse); // deactivates any open edit boxes
            iWindowManager.Exit( aCommand, iWindowManager.ActiveWindow()->Uid());
            break;
        case EAknSoftkeyCancel:
            // cancel must be handled before hiding status pane
            iContainer->HandleCommandL( (TInt)TBrCtlDefs::ECommandCancel + (TInt)TBrCtlDefs::ECommandIdBase );
            ShowActivatedObject(EFalse); // deactivates any open edit boxes
        	break;
        case EAknSoftkeyDone:
            // accept must be handled before hiding status pane
            iContainer->HandleCommandL( (TInt)TBrCtlDefs::ECommandAccept + (TInt)TBrCtlDefs::ECommandIdBase );
            ShowActivatedObject(EFalse); // deactivates any open edit boxes
        	break;
        case ECmdMsk:
            if (editing)
                {
                break; // ignore MSK if editing
                }
            // fall through if not editing
        default:
            iContainer->HandleCommandL( aCommand );
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::DynInitMenuPaneL()
// Dynamically initialises menu pane
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowView::DynInitMenuPaneL( TInt aResourceId,
                                         CEikMenuPane* aMenuPane )
    {
    if (!iWindowManager.ActiveWindow())
        return;
    CBrCtlInterface* engine = iWindowManager.ActiveWindow()->Engine();
    if( engine && (aResourceId == R_WIDGETUI_MENU || aResourceId >= R_CASCADE_MENU_1) )
        {
        iIsOptionsMenuActivated = ETrue;
        engine->AddOptionMenuItemsL( *aMenuPane, aResourceId );
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::DoActivateL()
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                    TUid /*aCustomMessageId*/,
                                    const TDesC8& /*aCustomMessage*/ )
    {
    StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_SMALL);

    //Cba()->MakeVisible(EFalse);

    iContainer->ActivateL();
    AppUi()->AddToStackL( *this, iContainer );

    //iContainer->SetRect(ClientRect());// resize without cba
    }

// -----------------------------------------------------------------------------
// CWidgetUiWindowView::DoDeactivate()
// Deactivate view
//
// -----------------------------------------------------------------------------
//
void CWidgetUiWindowView::DoDeactivate()
    {
    AppUi()->RemoveFromStack( iContainer );
    }

// ---------------------------------------------------------------------------
// CWidgetUiContentView::HandleStatusPaneSizeChange
// Handles change in the position or size of the status pane
//
// ---------------------------------------------------------------------------
//
void CWidgetUiWindowView::HandleStatusPaneSizeChange()
    {
     // Don't update status pane if widget is in miniview state.
    if( iWindowManager.ActiveWindow() &&
        iWindowManager.ActiveWindow()->WidgetFullViewState() )
        {
        Redraw();
        }
    }

// ---------------------------------------------------------------------------
// CWidgetUiContentView::ShowActivatedObject
// Show the StatusPane and Softkeys - needed for text boxes
// Or just Softkeys for flash etc...
//
// ---------------------------------------------------------------------------
//
void CWidgetUiWindowView::ShowActivatedObject(TBool aVisible, TBool aShowStatusPane)
    {
    if (iActivatedObjectVisible == aVisible)
        {
        return;
        }
    iActivatedObjectVisible = aVisible;
    iStatusPaneVisible = (aVisible && aShowStatusPane);

    if ( iWindowManager.ActiveWindow() )
        {
        // Override Right Softkey with Cancel when editing
        if (iActivatedObjectVisible)
            {
            iCbaVisible = Cba()->IsVisible();// hold state
            TRAPD(err,Cba()->AddCommandToStackL(KRightSoftkeyPosition,R_WIDGETUI_WINDOW_VIEW_SOFTKEYS_CANCEL));
            if (err == KErrNone)
                {
                TRAP(err,Cba()->AddCommandToStackL(KLeftSoftkeyPosition,R_WIDGETUI_WINDOW_VIEW_SOFTKEYS_DONE));
                }
            iWindowManager.ActiveWindow()->SetSoftkeysVisible(ETrue);
            }
        else
            {
            // remove the cancel softkey and hide softkeys if they were previously hidden
            Cba()->RemoveCommandFromStack(KRightSoftkeyPosition,EAknSoftkeyCancel);
            Cba()->RemoveCommandFromStack(KLeftSoftkeyPosition,EAknSoftkeyDone);

            iWindowManager.ActiveWindow()->SetSoftkeysVisible(iCbaVisible);
            }
        }
    UpdateStatusPane();
    }

// ---------------------------------------------------------------------------
// CWidgetUiContentView::UpdateStatusPane
// Show the StatusPane if needed
//
// ---------------------------------------------------------------------------
//
void CWidgetUiWindowView::UpdateStatusPane()
    {
#ifdef RD_SCALABLE_UI_V2
    // no need for the status pane on touch phones
    if (PenEnabled())
        {
        TInt resId = StatusPane()->CurrentLayoutResId();
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            TInt newResId = Cba()->IsVisible() ?
                  R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT :
                  R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT_NO_SOFTKEYS;
            if (resId != newResId)
                {
                StatusPane()->SwitchLayoutL(newResId);
                }
            }
        else //Portrait
            {
            if (resId != R_AVKON_STATUS_PANE_LAYOUT_SMALL)
                {
                StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_SMALL);
                }
            }
        StatusPane()->ApplyCurrentSettingsL();
        StatusPane()->MakeVisible(EFalse);
        return;
        }
#endif

    TBool visible(iStatusPaneVisible);

    // non-touch: always show status pane in landscape if softkeys are visible
    if (!visible && Cba()->IsVisible() &&
        AknLayoutUtils::CbaLocation() != AknLayoutUtils::EAknCbaLocationBottom)
        {
        visible = ETrue;
        }

    StatusPane()->MakeVisible(visible);

    if (visible && StatusPane()->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT)
        {
        StatusPane()->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT);
        StatusPane()->ApplyCurrentSettingsL();
        }
    }

// ---------------------------------------------------------
// CWidgetUiWindowView::Redraw
// redraw/relayout when application returns from background
// or status pane shows/hides
//
// ---------------------------------------------------------
//
void CWidgetUiWindowView::Redraw() const
    {
    if ( iContainer )
        {
        iContainer->SetRect( AppUi()->ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// CWidgetUiWindowView::StateChanged
// ---------------------------------------------------------------------------
//
void CWidgetUiWindowView::StateChanged(
        TBrCtlDefs::TBrCtlState aState,
        TInt aValue )
    {
    switch(aState)
        {
        case TBrCtlDefs::EStateScriptLog:
            {
                CRepository*  iRepository = CRepository::NewL( KCRUidBrowser );
                if ( iRepository )
                    {
                    iRepository->Set( KBrowserNGScriptLog, aValue );
                    }
                delete iRepository;
                iRepository = NULL;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CWidgetUiWindowView::IsOptionsMenuActivated
// ---------------------------------------------------------------------------
//
TBool CWidgetUiWindowView::IsOptionsMenuActivated()
 	{
 	return iIsOptionsMenuActivated;
 	}


// ---------------------------------------------------------------------------
// CWidgetUiWindowView::DeActivateOptionsMenu
// ---------------------------------------------------------------------------
//
void CWidgetUiWindowView::DeActivateOptionsMenu()
 	{
 	iIsOptionsMenuActivated = EFalse;
 	}

// End of File
