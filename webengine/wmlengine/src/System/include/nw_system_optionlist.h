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


#ifndef _NW_System_OptionList_h_
#define _NW_System_OptionList_h_

#include "nw_system.h"
#include "nwx_defs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- */
NW_SYSTEM_EXPORT
void
NW_System_OptionList_Initialize (void);

NW_SYSTEM_EXPORT
void
NW_System_OptionList_AddItem (NW_Ucs2* string,
                              NW_Uint32 elementID,
							  NW_Int16 aElType);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_System_OptionList_h_ */
