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


#include <e32std.h>
#include "nw_fbox_formliaisoni.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_FormLiaison_Class_t NW_FBox_FormLiaison_Class = {
  { /* NW_Object_Core      */
    /* super               */ &NW_Object_Dynamic_Class,
    /* queryInterface      */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base      */
    /* interfaceList       */ NULL
  },
  { /* NW_Object_Dynamic   */
    /* instanceSize        */ sizeof (NW_FBox_FormLiaison_t),
    /* construct           */ NULL,
    /* destruct            */ _NW_FBox_FormLiaison_Destruct
  },
  { /* NW_FBox_FormLiaison   */
    /* getWBXMLVersion       */ NULL,
    /* addControl            */ NULL,
    /* setInitialStringValue */ NULL,
    /* setStringValue        */ NULL,
    /* validateStringValue   */ NULL,
    /* getStringValue        */ NULL,
    /* getStringName         */ NULL,
    /* getStringTitle        */ NULL,
    /* setBoolValue          */ NULL,
    /* toggleBoolValue       */ NULL,
    /* getBoolValue          */ NULL,
    /* reset                 */ NULL,
    /* submit                */ NULL,
    /* isOptionMultiple      */ NULL,
    /* isOptionSelected      */ NULL,
    /* optionHasOnPick       */ NULL,
    /* isLocalNavOnPick      */ NULL,
    /* getInitialValue       */ NULL,
    /* getDocRoot            */ NULL,
    /* delegateEcmaEvent     */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

void
_NW_FBox_FormLiaison_Destruct( NW_Object_Dynamic_t* aDynamicObject )
    {
    NW_FBox_FormLiaison_t* thisObj;

    // Parameter assertions.
    NW_ASSERT( NW_Object_IsInstanceOf( aDynamicObject,
        &NW_FBox_FormLiaison_Class) );

    // For convenience.
    thisObj = NW_FBox_FormLiaisonOf( aDynamicObject );

    delete thisObj->iMostRecentFileSelectionPath;
    }

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
// NW_FBox_FormLiaison::GetMostRecentFileSelectionPath
// Gets the most-recent-file-selection-path.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_FBox_FormLiaison_GetMostRecentFileSelectionPath(
    NW_FBox_FormLiaison_t* aThisObj, TPtrC& aPath )
    {
    // Parameter assertion block.
    NW_ASSERT( aThisObj );

    if ( aThisObj->iMostRecentFileSelectionPath )
        {
        aPath.Set( *aThisObj->iMostRecentFileSelectionPath );
        }
    else
        {
        aPath.Set( NULL, 0 );
        }
    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// NW_FBox_FormLiaison::SetMostRecentFileSelectionPath
// Sets the most-recent-file-selection-path.
// (Other items are commented in header.)
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_FBox_FormLiaison_SetMostRecentFileSelectionPath(
    NW_FBox_FormLiaison_t* aThisObj, TPtrC& aPath )
    {
    // Parameter assertion block.
    NW_ASSERT( aThisObj );

    // Allocate buffer if needed.
    if ( !aThisObj->iMostRecentFileSelectionPath )
        {
        aThisObj->iMostRecentFileSelectionPath = HBufC::New( KMaxFileName );
        if ( !aThisObj->iMostRecentFileSelectionPath )
            {
            return KBrsrOutOfMemory;
            }
        }

    // Set the value.
    TPtr ptrNewBuf( aThisObj->iMostRecentFileSelectionPath->Des() );
    ptrNewBuf.Copy( aPath );

    return KBrsrSuccess;
    }
