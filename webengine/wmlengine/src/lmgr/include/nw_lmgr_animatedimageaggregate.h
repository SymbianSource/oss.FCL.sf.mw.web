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


#ifndef NW_LMgr_AnimatedImageAggregate_H
#define NW_LMgr_AnimatedImageAggregate_H

#include "NW_LMgr_EXPORT.h"
#include "nw_object_aggregate.h"
#include "nw_system_timer.h"
#include "nw_image_abstractimage.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_AnimatedImageAggregate_Class_s NW_LMgr_AnimatedImageAggregate_Class_t;
typedef struct NW_LMgr_AnimatedImageAggregate_s NW_LMgr_AnimatedImageAggregate_t;
 
/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_LMgr_AnimatedImageAggregate_Animate_t) (NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate);

typedef
NW_Image_AbstractImage_t*
(*NW_LMgr_AnimatedImageAggregate_GetImage_t) (NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate);

typedef
TBrowserStatusCode
(*NW_LMgr_AnimatedImageAggregate_StartTimer_t) (NW_LMgr_AnimatedImageAggregate_t* AnimatedImageAggregate);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_AnimatedImageAggregate_ClassPart_s {
  NW_LMgr_AnimatedImageAggregate_Animate_t animate;
  NW_LMgr_AnimatedImageAggregate_GetImage_t getImage;
  NW_LMgr_AnimatedImageAggregate_StartTimer_t startTimer;
} NW_LMgr_AnimatedImageAggregate_ClassPart_t;

struct NW_LMgr_AnimatedImageAggregate_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Secondary_ClassPart_t NW_Object_Secondary;
  NW_Object_Aggregate_ClassPart_t NW_Object_Aggregate;
  NW_LMgr_AnimatedImageAggregate_ClassPart_t NW_LMgr_AnimatedImageAggregate;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_AnimatedImageAggregate_s {
  NW_Object_Aggregate_t super;

  /* member variables */
  NW_Bool started;
  NW_System_Timer_t* timer;
  NW_Uint32 nowHigh;
  NW_Uint32 nowLow;
  NW_Bool nowSet;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_AnimatedImageAggregate_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_AnimatedImageAggregate))
  
#define NW_LMgr_AnimatedImageAggregateOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_AnimatedImageAggregate))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_AnimatedImageAggregate_Class_t NW_LMgr_AnimatedImageAggregate_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_LMgr_AnimatedImageAggregate_Animate(_object) \
  (NW_Object_Invoke ((_object), NW_LMgr_AnimatedImageAggregate, animate) \
  ((_object)))

#define NW_LMgr_AnimatedImageAggregate_GetImage(_object) \
  (NW_Object_Invoke ((_object), NW_LMgr_AnimatedImageAggregate, getImage) \
  ((_object)))


#define NW_LMgr_AnimatedImageAggregate_StartTimer(_object) \
  (NW_Object_Invoke ((_object), NW_LMgr_AnimatedImageAggregate, startTimer) \
  ((_object)))


TBrowserStatusCode
NW_LMgr_AnimatedImageAggregate_CreateTimer (NW_LMgr_AnimatedImageAggregate_t* thisObj,
                                            NW_Uint32 delay);
void
NW_LMgr_AnimatedImageAggregate_DestroyTimer (NW_LMgr_AnimatedImageAggregate_t* thisObj);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMgr_AnimatedImageAggregate_H */






