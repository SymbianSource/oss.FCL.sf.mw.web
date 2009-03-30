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


#ifndef _NW_Markup_NumberCollector_h_
#define _NW_Markup_NumberCollector_h_

#include "NW_Markup_EXPORT.h"
#include "nw_object_aggregate.h"
#include "nw_hed_inumbercollector.h"
#include "nw_lmgr_box.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_NumberCollector_Class_s NW_Markup_NumberCollector_Class_t;
typedef struct NW_Markup_NumberCollector_s NW_Markup_NumberCollector_t;

/* ------------------------------------------------------------------------- *
   virtual method type definition(s)
 * ------------------------------------------------------------------------- */
typedef
NW_LMgr_Box_t*
(*NW_Markup_NumberCollector_GetBoxTree_t) (NW_Markup_NumberCollector_t* numberCollector);

typedef
NW_Text_t*
(*NW_Markup_NumberCollector_GetHrefAttr_t) (NW_Markup_NumberCollector_t* numberCollector,
                                            NW_LMgr_Box_t* box);

typedef
NW_Text_t*
(*NW_Markup_NumberCollector_GetAltAttr_t) (NW_Markup_NumberCollector_t* numberCollector,
                                            NW_LMgr_Box_t* box);

typedef
NW_Bool
(*NW_Markup_NumberCollector_BoxIsValid_t) (NW_Markup_NumberCollector_t* numberCollector,
                                           NW_LMgr_Box_t* box);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_NumberCollector_ClassPart_s {
  NW_Markup_NumberCollector_GetBoxTree_t getBoxTree;
  NW_Markup_NumberCollector_BoxIsValid_t boxIsValid;
  NW_Markup_NumberCollector_GetHrefAttr_t getHrefAttr;
  NW_Markup_NumberCollector_GetAltAttr_t getAltAttr;
} NW_Markup_NumberCollector_ClassPart_t;

struct NW_Markup_NumberCollector_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
  NW_Object_Aggregate_ClassPart_t NW_Object_Aggregate;
  NW_Markup_NumberCollector_ClassPart_t NW_Markup_NumberCollector;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Markup_NumberCollector_s {
  NW_Object_Aggregate_t super;

  /* interface implementation(s) */
  NW_HED_INumberCollector_t NW_HED_INumberCollector;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Markup_NumberCollector_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_Markup_NumberCollector))

#define NW_Markup_NumberCollectorOf(_object) \
  (NW_Object_Cast (_object, NW_Markup_NumberCollector))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Markup_NumberCollector_Class_t NW_Markup_NumberCollector_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Markup_NumberCollector_h_ */
