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


#ifndef NW_EMPTYBOXI_H
#define NW_EMPTYBOXI_H

#include "nw_object_dynamici.h"
#include "nw_lmgr_boxi.h"
#include "nw_lmgr_emptybox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_EmptyBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                            va_list* argp);

void
_NW_LMgr_EmptyBox_Destruct (NW_Object_Dynamic_t* dynamicObject);
  
TBrowserStatusCode
_NW_LMgr_EmptyBox_GetMinimumContentSize(NW_LMgr_Box_t *box,
                                        NW_GDI_Dimension2D_t *size);

NW_Bool
_NW_LMgr_EmptyBox_HasFixedContentSize(NW_LMgr_Box_t *box);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif


