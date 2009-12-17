/*
* ============================================================================
*  Name     : CBrCtlApiTestContainer from BrCtlBCTestContainer.h
*  Part of  : BrCtlBCTest
*  Created  : 04/17/2006 by 
*  Implementation notes:
*     Initial content was generated by Series 60 AppWizard.
*  Version  :
*  Copyright: 
* ============================================================================
*/

// INCLUDE FILES
#include "brctlapitestcontainer.h"

#include <eiklabel.h>  // for example label control
#include <aknview.h>
//#include <AknGlobalConfirmationQuery.h>
#include <brctlinterface.h>
#include <BrCtl.h>
#include <CHARCONV.H>
#include "brctlapitestobserver.h"
#include "brctlapiteststatechangeobserver.h"
#include <e32std.h>

_LIT8( KContentDataType, "text/html" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrCtlApiTestContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CBrCtlApiTestContainer::ConstructL(const TRect& aRect)
    {
    iBrCtlApiTestObserver = CBrCtlApiTestObserver::NewL();
    iBrCtlApiTestStateChangeObserver = CBrCtlApiTestStateChangeObserver::NewL( this );
    iBrCtlDialogs = CBrowserDialogsProviderProxy::NewL();
    iCommandBase = TBrCtlDefs::ECommandIdBase;
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
    }

// Destructor
CBrCtlApiTestContainer::~CBrCtlApiTestContainer()
    {
    if (iBrCtlInterface)
        {
        iBrCtlInterface->RemoveLoadEventObserver(iBrCtlApiTestObserver);
        iBrCtlInterface->RemoveStateChangeObserver(iBrCtlApiTestStateChangeObserver);
        delete iBrCtlInterface;
        }

    delete iBrCtlApiTestStateChangeObserver;
    delete iBrCtlApiTestObserver;
    delete iBrCtlDialogs;
    }

void CBrCtlApiTestContainer::CreateBrowserControlInterfaceL()
    {
    TPoint base( 0, 0 );
    TPoint extent( 352, 295 );
    TRect rect( base, extent );
    iBrCtlInterface = CreateBrowserControlL( this, rect, 
        TBrCtlDefs::ECapabilityDisplayScrollBar 
            | TBrCtlDefs::ECapabilityClientResolveEmbeddedURL 
            | TBrCtlDefs::ECapabilityLoadHttpFw
            | TBrCtlDefs::ECapabilitySavedPage
            | TBrCtlDefs::ECapabilityDisplayScrollBar,
        iCommandBase, iBrCtlApiTestObserver, iBrCtlApiTestObserver,
        iBrCtlApiTestObserver, iBrCtlApiTestObserver, iBrCtlDialogs, NULL, iBrCtlApiTestObserver );

    iBrCtlInterface->MakeVisible( ETrue );
    }

TInt CBrCtlApiTestContainer::SetFocus()
    {
    iBrCtlInterface->SetFocus( ETrue, EDrawNow );
    return KErrNone;
    }
    
void CBrCtlApiTestContainer::CancelLoad()
    {}
   
void CBrCtlApiTestContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {}
    
void CBrCtlApiTestContainer::HandleCommandL(
									 TBrCtlDefs::TBrCtlClientCommands aCommand, const CArrayFix<TPtrC>& aAttributesNames,
                                     const CArrayFix<TPtrC>& aAttributeValues ) 
    {}           
    
            
// End of File  
