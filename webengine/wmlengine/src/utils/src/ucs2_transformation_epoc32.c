/*
* Copyright (c) 2001-2002 Nokia Corporation and/or its subsidiary(-ies).
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
 

/*
**----------------------------------------------------------------
**  Component Generic Include
**----------------------------------------------------------------
*/

/*
**----------------------------------------------------------------
**  Module Specific Includes
**----------------------------------------------------------------
*/
#include "nwx_string.h"
#define EPOC32_API_AVAILABLE 0 /* 0 when not available */
/*
**----------------------------------------------------------------
**  Constants
**----------------------------------------------------------------
*/

/*
**----------------------------------------------------------------
**  Internal Types
**----------------------------------------------------------------
*/
#if (EPOC32_API_AVAILABLE)
#include "type_def.h"
#include "ui_type.h"
#include "d_xhtml_interface.h"
#include "BrsrStatusCodes.h"


typedef enum
{
   NW_Transformation_ToUpper = 0,
   NW_Transformation_ToLower,
   NW_Transformation_ToTitleCase
} TextTransformationMode;
#endif
/*
**----------------------------------------------------------------
**  Macros
**----------------------------------------------------------------
*/

/*
**----------------------------------------------------------------
**  Internal Prototypes
**----------------------------------------------------------------
*/

/*
**----------------------------------------------------------------
**  File Scoped Static Variables
**----------------------------------------------------------------
*/

/*
**----------------------------------------------------------------
**  Global Variable Definitions
**----------------------------------------------------------------
*/

/*
**----------------------------------------------------------------
**  Internal Functions
**----------------------------------------------------------------
*/
/**** Below is the code that uses ISA DO APIs (to be delivered in Jan 2002) ***/
#if (EPOC32_API_AVAILABLE)
static TBrowserStatusCode DoTextTransformation(NW_Ucs2 **ppStr, TextTransformationMode mode)
{
    ucs2_char   *result;
    ucs2_char   *tmpResult = NULL;
    TBrowserStatusCode nwStatus  = KBrsrSuccess;
    uint16      ret;
    uint16      size;
    uint16      length;

    NW_ASSERT(ppStr && *ppStr);

    length = NW_Str_Strlen(*ppStr);

    if (2*length > NW_UINT16_MAX) {
        nwStatus = KBrsrFailure; 
        goto FuncExit;
    }

    /* Case transformation can result in a different string size.
       We reserve the result string buffer for the worst case scenario */
    size = 2*length;
    tmpResult = NW_Str_New(size);
    if (tmpResult == NULL) {
        nwStatus = KBrsrOutOfMemory;
        goto FuncExit;
    }
    switch(mode)
    {
        case NW_Transformation_ToUpper:
            ret = D1_ToUpper((ucs2_char *)*ppStr, tmpResult, length, &size);
            break;
        case NW_Transformation_ToLower:
            ret = D1_ToLower((ucs2_char *)*ppStr, tmpResult, length, &size);
            break;
        case NW_Transformation_ToTitleCase:
            ret = D1_ToTitleCase((ucs2_char *)*ppStr, tmpResult, length, &size);
            break;
        default:
            NW_ASSERT(0);
            ret = 0xFFFF;
            break;
    }

    if (ret == 0xFFFF) {
        nwStatus = KBrsrFailure;
        goto FuncExit;
    }

    if (size == length) {
        (void) NW_Str_Strcpy(*ppStr, tmpResult);
    }
    else {
        result = NW_Str_New(size);
        if (result == NULL) {
            nwStatus = KBrsrOutOfMemory; 
        }
        else {
            (void) NW_Str_Strcpy(result, tmpResult);
            *ppStr = result;
        }
    }

FuncExit:
    NW_Str_Delete(tmpResult);
    return nwStatus;
}
#endif
/*
**----------------------------------------------------------------
**  External Public (Exported) Functions
**----------------------------------------------------------------
*/

/*****************************************************************

  Name:       NW_Str_Ucs2StrUpr

  Description: Transforms each letter of the string to upper case

  Parameters: pointer to a Ucs2 string pointer 

  Return Value: Transformed to upper case string
                NW_Status: KBrsrSuccess

*****************************************************************/
TBrowserStatusCode NW_Str_Ucs2StrUpr(NW_Ucs2 **ppStr)
{
    TBrowserStatusCode nwStatus;

#if (EPOC32_API_AVAILABLE)
    nwStatus = DoTextTransformation(ppStr, NW_Transformation_ToUpper);
#else
    NW_ASSERT(ppStr);
    nwStatus = KBrsrSuccess;
    NW_Str_Strupr(*ppStr);
#endif
    return nwStatus;
}

/*****************************************************************

  Name:       NW_Str_Ucs2StrLwr

  Description: Transforms each letter of the string to lower case

  Parameters: pointer to a Ucs2 string pointer 

  Return Value: Transformed to lower case string
                NW_Status: KBrsrSuccess

*****************************************************************/
TBrowserStatusCode NW_Str_Ucs2StrLwr(NW_Ucs2 **ppStr)
{
    TBrowserStatusCode nwStatus;

#if (EPOC32_API_AVAILABLE)
    nwStatus = DoTextTransformation(ppStr, NW_Transformation_ToLower);
#else
    NW_ASSERT(ppStr);
    nwStatus = KBrsrSuccess;
    NW_Str_Strlwr(*ppStr);
#endif
    return nwStatus;
}

/*****************************************************************

  Name:       NW_Str_Ucs2StrTitle

  Description: Transforms first character of each word to upper case

  Parameters: pointer to a Ucs2 string pointer 

  Return Value: Transformed to lower case string
                NW_Status: KBrsrSuccess

*****************************************************************/
TBrowserStatusCode NW_Str_Ucs2StrTitle(NW_Ucs2 **ppStr)
{
    NW_Bool     capChar = NW_TRUE;
    TBrowserStatusCode nwStatus;

#if (EPOC32_API_AVAILABLE)
    nwStatus = DoTextTransformation(ppStr, NW_Transformation_ToTitleCase);
#else
    NW_Ucs2     *tmp;
    NW_ASSERT(ppStr && *ppStr);
    nwStatus = KBrsrSuccess;
    for (tmp = *ppStr; *tmp; tmp++) {
        if (NW_Str_Isspace (*tmp) == NW_TRUE) {
            capChar = NW_TRUE;
        }
        else if (capChar == NW_TRUE) {
            *tmp = NW_Str_ToUpper (*tmp);
            capChar = NW_FALSE;
        }
    }
#endif 
    return nwStatus;
}
