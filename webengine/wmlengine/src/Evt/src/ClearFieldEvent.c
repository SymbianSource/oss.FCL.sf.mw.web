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
* Description:  ClearFieldEvent OOC class implementation -- an event for
*   clearing a field on a form.
*
*/


// INCLUDE FILES
#include "nw_evt_clearfieldeventi.h"
#include "BrsrStatusCodes.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES
const
NW_Evt_ClearFieldEvent_Class_t NW_Evt_ClearFieldEvent_Class = {
    {   /* NW_Object_Core        */
        /* super                 */ &NW_Evt_Event_Class,
        /* queryInterface        */ _NW_Object_Base_QueryInterface
    },
    {   /* NW_Object_Base        */
        /* interfaceList         */ NULL
    },
    {   /* NW_Object_Dynamic     */
        /* instanceSize          */ sizeof (NW_Evt_ClearFieldEvent_t),
        /* construct             */ NULL,
        /* destruct              */ NULL
    },
    {   /* NW_Evt_Event          */
        /* unused                */ 0
    },
    {   /* NW_Evt_ClearFieldEvent*/
        /* unused                */ 0
    }
};

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NW_FBox_ClearFieldEvent::New
// OOC method for creating a new instance of this class.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
NW_Evt_ClearFieldEvent_t*
NW_Evt_ClearFieldEvent_New( void )
{
    return (NW_Evt_ClearFieldEvent_t*)NW_Object_New( &NW_Evt_ClearFieldEvent_Class );
}

// -----------------------------------------------------------------------------
// NW_FBox_FileSelectionBox::Initialize
// OOC method for initializing this instance of the class.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_Evt_ClearFieldEvent_Initialize( NW_Evt_ClearFieldEvent_t* event )
{
    return NW_Object_Initialize( &NW_Evt_ClearFieldEvent_Class, event );
}
