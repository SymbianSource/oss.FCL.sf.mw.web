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

#include    "WidgetUiApp.h"
#include    "WidgetUiDocument.h"
#include    "MemoryManager.h"
#include    "widgetappdefs.rh"
#include    <eikstart.h>
#include <u32std.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWidgetUiApp::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CWidgetUiApp::AppDllUid() const
    {
    return KUidWidgetUi;
    }

// -----------------------------------------------------------------------------
// CWidgetUiApp::OpenIniFileLC()
// overrides CAknApplication::OpenIniFileLC to enable INI file support
// -----------------------------------------------------------------------------
//
CDictionaryStore* CWidgetUiApp::OpenIniFileLC(
    RFs& aFs ) const
    {
    return CEikApplication::OpenIniFileLC( aFs );
    }

// -----------------------------------------------------------------------------
// CWidgetUiApp::CreateDocumentL()
// Creates CWidgetUiDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CWidgetUiApp::CreateDocumentL()
    {
    return CWidgetUiDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

// -----------------------------------------------------------------------------
// NewApplication
//
//
// -----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication( )
    {
    return new CWidgetUiApp;
    }

// -----------------------------------------------------------------------------
// SetupThreadHeap - Called for heap creation of thread in this process.
// This approach used to keep correct heap for pointers held in static data objects
// when they are destructed after E32Main() by OS.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt UserHeap::SetupThreadHeap(TBool aSubThread, SStdEpocThreadCreateInfo& aInfo)
    {
    TInt r = KErrNone;
    if (!aInfo.iAllocator && aInfo.iHeapInitialSize>0)
        {
        // new heap required
        RHeap* pH = NULL;
        r = CreateThreadHeap(aInfo, pH);
        if (r == KErrNone && !aSubThread)
            {
            // main thread - new allocator created and set as default heap      
            MemoryManager::CreateFastAllocator();
            }
        }
    else if (aInfo.iAllocator)
        {
        // sharing a heap
        RAllocator* pA = aInfo.iAllocator;
        pA->Open();
        User::SwitchAllocator(pA);
        }

    return r;
    }
    
    
// -----------------------------------------------------------------------------
// E32Main
//
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
        // initialize MemmoryManager
        MemoryManager::InitFastAllocator();    
        
    	return EikStart::RunApplication(NewApplication); 
    }
        
// End of File  
