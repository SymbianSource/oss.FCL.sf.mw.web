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
* Description:  
*
*/

#ifndef WML_WAE_H
#define WML_WAE_H

/*
** Includes
*/
#include "nw_wae.h"
#include "BrsrStatusCodes.h"

/*
** Type Declarations
*/


/*
** Global Function Declarations
*/

/*
**  wml_msgs.c
*/
/* run the script */
TBrowserStatusCode
NW_Wml_RunScript(NW_Ucs2 *url, NW_Ucs2 *postfields,
              NW_Ucs2 *referer, NW_Byte *content,
              const NW_Uint32 len, const NW_Http_CharSet_t charset);

/* abort the running script */
TBrowserStatusCode NW_Wml_AbortScript(void);

#endif  /* WML_WAE_H */
