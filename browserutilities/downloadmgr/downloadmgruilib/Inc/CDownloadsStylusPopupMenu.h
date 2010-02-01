/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors: Hari and Billa
*
* Description:  Stylus Popup Menu Containing Download UI Options
*
*/

#ifndef CDlTYLUSPOPUPMENU_H_
#define CDlTYLUSPOPUPMENU_H_

#include <aknlongtapdetector.h>
#include <aknstyluspopupmenu.h>
#include <coecntrl.h>
#include <coemain.h>
#include <barsread.h>
#include <EIKMOBS.H> 
#include <aknpopup.h>
#include <CDownloadsListDlg.h>
class CDownloadsListDlg;

_LIT8(KAudio, "audio/");
_LIT8(KVideo, "video/");
_LIT8(KImage, "image/");
_LIT8(KFlash, "application/x-shockwave-flash");
_LIT8(Ksdp, "application/sdp");
_LIT8(Krng, "application/vnd.nokia.ringing-tone");
_LIT8(Krn, "application/vnd.rn-realmedia");
_LIT8(Kpn, "application/x-pn-realmedia");
_LIT8(KSisxContentType, "x-epoc/x-sisx-app");

#define KLONG_TAP_TIMER_DELAY 0.4000000 //0.4 seconds

// CLASS DECLARATION
class CDlStylusPopupMenu :  public CCoeControl, public MAknLongTapDetectorCallBack, public MEikMenuObserver
{   
    public:
        static CDlStylusPopupMenu* NewL();
        void ConstructL();
        void HandlePointerEventL(const TPointerEvent& aPointerEvent,CDownloadsListDlg* view);
        virtual void HandleLongTapEventL( const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation );
        void ProcessCommandL(TInt aCommandId);  
        void SetEmphasis(CCoeControl* /*aMenuControl*/,TBool /*aEmphasis*/) {   }   
        void cancelLongTapL();
        TBool islongtapRunning();
        void reSetLongTapFlag();
        ~CDlStylusPopupMenu();
    
    private:    
        CAknLongTapDetector* iLongTapDetector;
        CAknStylusPopUpMenu* iStylusPopupMenu;  
        TInt iCount; 
        TBool ilongtapRunning;
        CDownloadsListDlg			*iDlView;       
};
#endif /* CDlTYLUSPOPUPMENU_H_ */
