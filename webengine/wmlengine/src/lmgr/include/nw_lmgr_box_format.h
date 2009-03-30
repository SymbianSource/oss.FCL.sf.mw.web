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


#ifndef NW_LMGR_BOX_FORMAT_H
#define NW_LMGR_BOX_FORMAT_H
  
#include "nw_object_dynamic.h"
#include "nw_lmgr_propertylist.h"
#include "nw_adt_vector.h"
#include "NW_LMgr_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
//typedef struct NW_LMgr_Box_FormatChildren_s  NW_LMgr_Box_FormatChildren_t;
 extern struct NW_LMgr_Box_FormatChildren_s  NW_LMgr_Box_FormatChildren_t;

struct NW_LMgr_Box_FormatChildren_s {
  NW_Bool formatChildren; /* is NW_TRUE if the box has the children to be formatted*/
  NW_LMgr_Box_t* formatBox;/* the formatbox to be used to format the children*/
  NW_LMgr_FormatContext_t* newFormatContext;/* new formatcontext to be used to format the children*/
  NW_Uint8  referenceCount;
};

#ifdef __cplusplus
}; // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_BOX_FORMAT_H*/



