/*
 *  CDlStylusPopupMenu.cpp
 *
 *  Created on: Dec 24, 2009
 *      Author: hari and billa
 */

#include "CDownloadsStylusPopupMenu.h"
#include "CDownloadsListDlg.h"
#include "DownloadMgrUiLib.rsg"
#include "CDownloadsListArray.h"
#include "DownloadMgrUilib.hrh"


// -----------------------------------------------------------------------------
// BrowserStylusPopupMenu::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CDlStylusPopupMenu* CDlStylusPopupMenu::NewL()
{   
    CDlStylusPopupMenu* container =
        new (ELeave) CDlStylusPopupMenu;
    container->ConstructL();
    return container;
}

void CDlStylusPopupMenu::ConstructL()
{
    ilongtapRunning = EFalse;
    iLongTapDetector = CAknLongTapDetector::NewL(this);
    TResourceReader readerFolder;
    TResourceReader readerItem;
}

// -----------------------------------------------------------------------------
// BrowserStylusPopupMenu::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// -----------------------------------------------------------------------------
//
void CDlStylusPopupMenu::HandlePointerEventL(const TPointerEvent& aPointerEvent, CDownloadsListDlg *view)
{    
    ilongtapRunning = EFalse;
    iLongTapDetector->SetLongTapDelay(KLONG_TAP_TIMER_DELAY);
    iLongTapDetector->PointerEventL(aPointerEvent);
    iLongTapDetector->EnableLongTapAnimation(ETrue);
    iDlView = view;
    CCoeControl::HandlePointerEventL(aPointerEvent);
}

void CDlStylusPopupMenu::HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation )
{    
    ilongtapRunning = ETrue;
    TBool isAiwcall = EFalse;
    if (iStylusPopupMenu)
        {
        delete iStylusPopupMenu;
        iStylusPopupMenu = NULL;
        iStylusPopupMenu = CAknStylusPopUpMenu::NewL( this , aPenEventScreenLocation);
        }
    else 
        {
        iStylusPopupMenu = CAknStylusPopUpMenu::NewL( this , aPenEventScreenLocation);
        }
    
    iStylusPopupMenu->SetPosition(aPenEventScreenLocation, CAknStylusPopUpMenu::EPositionTypeLeftBottom);
    
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader,R_STYLUS_LONG_TAP_POPUP_DL_MENU);
    iStylusPopupMenu->ConstructFromResourceL(reader);
    TInt currentItemIndex = iDlView->CurrentItemIndex();
    CDownloadsListArray* DownloadsListArray = iDlView->DownloadsListArray();
    RHttpDownload& currDownload = DownloadsListArray->Download( currentItemIndex );
   
   // Get the UI data for the current download
   TDownloadUiData& dlData = DownloadsListArray->DlUiData( iDlView->CurrentItemIndex() );
   TInt32 state( dlData.iDownloadState );
   HBufC8* contentType = HBufC8::NewLC(KMaxContentTypeLength);
   TPtr8 contentTypePtr = contentType->Des(); 
   User::LeaveIfError
           ( currDownload.GetStringAttribute( EDlAttrContentType, contentTypePtr ) );
   TInt typeAudio = !contentType->Find(KAudio);
   CleanupStack::PopAndDestroy( contentType ); 
             
    if (state == EHttpDlInprogress)
        {
        isAiwcall = ETrue;
        iStylusPopupMenu->SetItemDimmed(EAiwCmdContact,ETrue);
        iStylusPopupMenu->SetItemDimmed(EAiwCmdRingtone,ETrue);
        iStylusPopupMenu->SetItemDimmed(EDownloadsListCmdDelete,ETrue);
        iStylusPopupMenu->SetItemDimmed(EDownloadsListCmdFileManager,ETrue);
        iStylusPopupMenu->SetItemDimmed(EDownloadsListCmdResume,ETrue);
        }

   if ((state == EHttpDlCompleted) || (state ==EHttpDlMultipleMOCompleted))
       {
       iStylusPopupMenu->SetItemDimmed(EDownloadsListCmdCancel,ETrue);
       iStylusPopupMenu->SetItemDimmed(EDownloadsListCmdResume,ETrue);
       }
    if (state == EHttpDlPaused)
        {
        isAiwcall = ETrue;
        iStylusPopupMenu->SetItemDimmed(EAiwCmdContact,ETrue);
        iStylusPopupMenu->SetItemDimmed(EAiwCmdRingtone,ETrue);
        iStylusPopupMenu->SetItemDimmed(EDownloadsListCmdDelete,ETrue);
        iStylusPopupMenu->SetItemDimmed(EDownloadsListCmdFileManager,ETrue);
        }
   
    if(typeAudio != 1) //Not an audio type 
        {
        isAiwcall = ETrue;
        iStylusPopupMenu->SetItemDimmed(EAiwCmdContact,ETrue);
        iStylusPopupMenu->SetItemDimmed(EAiwCmdRingtone,ETrue);
        }
    
    CleanupStack::PopAndDestroy();  
    if (!isAiwcall)
        iDlView->AddAiwItemsL();
    iStylusPopupMenu->ShowMenu(); 
}

void CDlStylusPopupMenu::ProcessCommandL(TInt aCommand)
{    
    iDlView->ProcessCommandL(aCommand);
}

CDlStylusPopupMenu::~CDlStylusPopupMenu()
{   
    if(iLongTapDetector)
        {
        delete iLongTapDetector;
        iLongTapDetector = NULL; 
        }
    if(iStylusPopupMenu)
        {
        delete iStylusPopupMenu;
        iStylusPopupMenu = NULL;    
        }
}

void CDlStylusPopupMenu::cancelLongTapL()
{   
    iLongTapDetector->CancelAnimationL();
}

TBool CDlStylusPopupMenu::islongtapRunning()
{
    return ilongtapRunning;
}

void CDlStylusPopupMenu::reSetLongTapFlag()
{
    ilongtapRunning = EFalse;
}

