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


#ifndef NW_LMGR_EPOC32ACCESSKEYI_H
#define NW_LMGR_EPOC32ACCESSKEYI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nw_lmgr_accesskeyi.h"
#include "nw_lmgr_epoc32accesskey.h"

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_Bool
_NW_LMgr_Epoc32AccessKey_IsValidForOptionsList (NW_LMgr_AccessKey_t* accesskey,
                                       NW_Uint32 val);

extern
NW_Bool
_NW_LMgr_Epoc32AccessKey_IsValidKeyPress (NW_LMgr_AccessKey_t* accesskey,
                                       NW_Uint32 val);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_LMGR_EPOC32ACCESSKEYI_H */
