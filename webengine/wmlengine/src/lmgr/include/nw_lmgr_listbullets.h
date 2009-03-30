/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_LISTBULLETS_H
#define NW_LISTBULLETS_H

#include "nwx_defs.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Utilities to return numbers as strings for use in ordered list bullets.*/

/* Get a number string in a specified format. The currently supported
 * formats are limited to "A", "a", "R", "r" and "d". Later we may want to add length 
 * and decimal point style and location specifiers to the format strings too.
 */ 

NW_Ucs2*
NW_LMgr_ListBullets_StringFromUInt(NW_Ucs2 format, NW_Uint32 number);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_LISTBULLETS_H */
