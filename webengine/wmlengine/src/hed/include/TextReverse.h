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



#ifndef TEXTREVERSE_H
#define TEXTREVERSE_H

#ifdef __cplusplus
extern "C" {
#endif


//  INCLUDES
#include "nwx_defs.h"
#include "nwx_http_defs.h"
#include "nw_string_string.h"
#include "BrsrStatusCodes.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES



TBrowserStatusCode ReverseText(NW_String_t* string);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif      // TEXTREVERSE_H

// End of File
