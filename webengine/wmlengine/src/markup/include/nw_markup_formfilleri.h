/*
* Copyright (c) 2002 - 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _NW_Markup_FormFillerI_h_
#define _NW_Markup_FormFillerI_h_

#include "nw_object_aggregatei.h"
#include "nw_hed_iformfilleri.h"
#include "nw_markup_formfiller.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected global data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Object_Class_t* const _NW_Markup_FormFiller_SecondaryList[];
NW_MARKUP_EXPORT const NW_HED_IFormFiller_Class_t NW_Markup_FormFiller_IFormFiller_Class;

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_MARKUP_EXPORT
NW_Bool
_NW_Markup_FormFiller_IFormFiller_ECMLExists (NW_HED_IFormFiller_t* formFiller);

NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_FormFiller_IFormFiller_FillAll (NW_HED_IFormFiller_t* formFiller, 
                                           NW_LMgr_Box_t** firstNotFilled);

NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_FormFiller_IFormFiller_FillOne (NW_HED_IFormFiller_t* formFiller,
                                           NW_Uint16 *value, NW_LMgr_Box_t* node);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Markup_FormFiller_GetBoxTree(_formFiller) \
  (NW_Object_Invoke (_formFiller, NW_Markup_FormFiller, getBoxTree) ( \
     NW_Markup_FormFillerOf (_formFiller)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Markup_FormFillerI_h_ */
