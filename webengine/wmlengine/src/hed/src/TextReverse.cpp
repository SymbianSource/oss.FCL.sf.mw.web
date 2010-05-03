/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Reverse ISO-8859-8 text
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include "TextReverse.h"
#include <bidivisual.h>
#include "nwx_settings.h"
#include "nwx_statuscodeconvert.h"
#include "BrsrStatusCodes.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

void ReverseTextL(NW_String_t* toString, NW_String_t* fromString)
    {
    const TInt additionalCharsLen = 4;
    NW_ASSERT(toString != NULL);
    NW_ASSERT(fromString != NULL);
    // Get the storage we know it is UCS2 encoded
    TUint16* stringToReverse = (TUint16*)NW_String_getStorage(fromString);
    TUint stringLen = NW_String_getByteCount(fromString) / (sizeof (TUint16));
    if (stringToReverse[stringLen - 1] == 0) 
        {
        stringLen--;
        }
    if (stringLen > 0)
        {
        TPtrC fromStringPtr(stringToReverse, stringLen);
        TUint16* reversedString = new(ELeave) TUint16 [stringLen + additionalCharsLen];
        CleanupStack::PushL(reversedString);
        TPtr reversedStringPtr(reversedString, stringLen + additionalCharsLen);
    
        TBidirectionalState::TRunInfo aRunInfoArray;
        TBidirectionalState::TRunInfo *mRunInfoArray = NULL;
        TBidiLogicalToVisual *bdLtoV = new(ELeave) TBidiLogicalToVisual(fromStringPtr, &aRunInfoArray, 1);
        TInt runArrayCount = bdLtoV->Reorder();
        delete bdLtoV;

        mRunInfoArray = new(ELeave) TBidirectionalState::TRunInfo [runArrayCount];
        CleanupStack::PushL(mRunInfoArray);
        bdLtoV = new TBidiLogicalToVisual(fromStringPtr, mRunInfoArray, runArrayCount);
        CleanupStack::PushL(bdLtoV);
        bdLtoV->Reorder();

        bdLtoV->GetVisualLine(reversedStringPtr, 0, stringLen, 0xFFFF);
        reversedStringPtr.ZeroTerminate();

        if (StatusCodeConvert(NW_String_ucs2CharToString (toString, reversedString, HTTP_iso_10646_ucs_2)) != KBrsrSuccess)
            {
            User::Leave(KErrNoMemory);
            }

        CleanupStack::PopAndDestroy(3); // bdLtoV, mRunInfoArray, reversedString
        }
    else
        {
        // transfer ownership
        NW_String_shallowCopy(toString, fromString);
        NW_String_clearUserOwnsStorage(fromString);
        }
    }


TBrowserStatusCode ReverseText(NW_String_t* string)
    {
    NW_ASSERT(string != NULL);
    NW_String_t toString;
    TBrowserStatusCode status = KBrsrSuccess;
    // Try to reverse only iso-8859-8
    if (NW_Settings_GetOriginalEncoding() == HTTP_iso_8859_8)
        {
        NW_String_initialize (&toString, NULL, 0);
        TRAPD(ret, ReverseTextL(&toString, string));
        if (ret != KErrNone)
            {
            status = KBrsrOutOfMemory;
            }
        else
            {
                //transfer ownership back to the original string
                if (NW_String_getUserOwnsStorage(string))
                    {
                    NW_String_deleteStorage(string);
                    }

                NW_String_shallowCopy(string, &toString);
                NW_String_clearUserOwnsStorage(&toString);
            }
        }
    return status;
    }



// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
