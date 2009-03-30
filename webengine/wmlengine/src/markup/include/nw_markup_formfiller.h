/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _NW_Markup_FormFiller_h_
#define _NW_Markup_FormFiller_h_

#include "NW_Markup_EXPORT.h"
#include "nw_object_aggregate.h"
#include "nw_hed_iformfiller.h"
#include "nw_lmgr_box.h"
#include "LMgrBoxTreeListener.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_FormFiller_Class_s NW_Markup_FormFiller_Class_t;
typedef struct NW_Markup_FormFiller_s NW_Markup_FormFiller_t;

/* ------------------------------------------------------------------------- *
   virtual method type definition(s)
 * ------------------------------------------------------------------------- */
typedef
NW_LMgr_Box_t*
(*NW_Markup_FormFiller_GetBoxTree_t) (NW_Markup_FormFiller_t* formFiller);


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_FormFiller_ClassPart_s {
  NW_Markup_FormFiller_GetBoxTree_t getBoxTree;
} NW_Markup_FormFiller_ClassPart_t;

struct NW_Markup_FormFiller_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
  NW_Object_Aggregate_ClassPart_t NW_Object_Aggregate;
  NW_Markup_FormFiller_ClassPart_t NW_Markup_FormFiller;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Markup_FormFiller_s {
  NW_Object_Aggregate_t super;

  /* interface implementation(s) */
  NW_HED_IFormFiller_t NW_HED_IFormFiller;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Markup_FormFiller_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_Markup_FormFiller))

#define NW_Markup_FormFillerOf(_object) \
  (NW_Object_Cast (_object, NW_Markup_FormFiller))

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

NW_MARKUP_EXPORT
TBrowserStatusCode 
NW_Markup_FormFiller_MapFromWalletStatus(WALLET_XHTML_Status_t walletStatus);

NW_MARKUP_EXPORT
WALLET_XHTML_Status_t
NW_Markup_FormFiller_MapToWalletStatus(TBrowserStatusCode status);

NW_MARKUP_EXPORT
NW_Bool 
NW_Markup_FormFiller_IsFormFillerBox(NW_LMgr_Box_t* box);

NW_MARKUP_EXPORT
NW_LMgr_Box_t* 
NW_Markup_FormFiller_GetInFocusBox(NW_HED_IFormFiller_t* formFiller,
                                   const MBoxTreeListener* boxTreeListener, 
                                   NW_Ucs2** title);

NW_MARKUP_EXPORT
NW_Bool
NW_Markup_FormFiller_VisibleCloseWallet(void);

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Markup_FormFiller_Class_t NW_Markup_FormFiller_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Markup_FormFiller_h_ */
