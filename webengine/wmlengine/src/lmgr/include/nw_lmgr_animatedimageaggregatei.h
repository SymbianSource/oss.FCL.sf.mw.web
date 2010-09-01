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


#ifndef NW_LMgr_AnimatedImageAggregateI_H
#define NW_LMgr_AnimatedImageAggregateI_H

#include "nw_object_aggregatei.h"
#include "nw_lmgr_animatedimageaggregate.h"
#include "BrsrStatusCodes.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
void
_NW_LMgr_AnimatedImageAggregate_Destruct (NW_Object_Aggregate_t* aggregate);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AnimatedImageAggregate_Animate (NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate);

NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_AnimatedImageAggregate_StartTimer (NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMgr_AnimatedImageAggregateI_H */
