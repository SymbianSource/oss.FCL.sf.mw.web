/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include    "MVCViewAsyncInit.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CViewAsyncInit::CViewAsyncInit
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CViewAsyncInit::CViewAsyncInit() : CActive(CActive::EPriorityUserInput)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CViewAsyncInit::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CViewAsyncInit::ConstructL()
    {
    TRequestStatus* status = &iStatus;
    SetActive();
    User::RequestComplete(status, KErrNone);
    }

// -----------------------------------------------------------------------------
// CViewAsyncInit::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CViewAsyncInit* CViewAsyncInit::NewL()
    {
    CViewAsyncInit* self = new( ELeave ) CViewAsyncInit;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CViewAsyncInit::~CViewAsyncInit()
    {
    
    }

// -----------------------------------------------------------------------------
// CViewAsyncInit::RunL
// Asynchronous handling of events.
// -----------------------------------------------------------------------------
//
void CViewAsyncInit::RunL()
{
    iView->AsyncInit();
}

// -----------------------------------------------------------------------------
// CViewAsyncInit::DoCancel
// Call NPP_UrlNotify.
// -----------------------------------------------------------------------------
//
void CViewAsyncInit::DoCancel()
{
}

// -----------------------------------------------------------------------------
// CViewAsyncInit::RunError
// Handle Leave from RunL.
// -----------------------------------------------------------------------------
//
TInt CViewAsyncInit::RunError(TInt /*aError*/)
{
    return KErrNone;
}


// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
