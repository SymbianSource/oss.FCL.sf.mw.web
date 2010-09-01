/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_FBOX_FORMBOX_H
#define NW_FBOX_FORMBOX_H

#include "nw_lmgr_activebox.h"
#include "nw_fbox_skin.h"
#include "nw_fbox_interactor.h"
#include "nw_fbox_formliaison.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_FormBox_Class_s NW_FBox_FormBox_Class_t;
typedef struct NW_FBox_FormBox_s NW_FBox_FormBox_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

typedef
NW_FBox_Skin_t*
(*NW_FBox_FormBox_InitSkin_t) (NW_FBox_FormBox_t* thisObj);

typedef
NW_FBox_Interactor_t*
(*NW_FBox_FormBox_InitInteractor_t) (NW_FBox_FormBox_t* thisObj);

typedef
TBrowserStatusCode
(*NW_FBox_FormBox_Reset_t) (NW_FBox_FormBox_t* thisObj);

 /* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_FBox_FormBox_ClassPart_s {
  NW_FBox_FormBox_InitSkin_t initSkin;
  NW_FBox_FormBox_InitInteractor_t initInteractor;
  NW_FBox_FormBox_Reset_t reset;
} NW_FBox_FormBox_ClassPart_t;
  
struct NW_FBox_FormBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ActiveBox_ClassPart_t NW_LMgr_ActiveBox;
  NW_FBox_FormBox_ClassPart_t NW_FBox_FormBox;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_FBox_FormBox_s {
  NW_LMgr_ActiveBox_t super;

  /* member variables */
  void* formCntrlId;
  NW_FBox_FormLiaison_t* formLiaison;
  NW_FBox_Skin_t* skin;
  NW_FBox_Interactor_t* interactor;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_FBox_FormBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_FBox_FormBox))

#define NW_FBox_FormBoxOf(object) \
  (NW_Object_Cast (object, NW_FBox_FormBox))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_FBOX_EXPORT const NW_FBox_FormBox_Class_t NW_FBox_FormBox_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

#define NW_FBox_FormBox_GetSkin(_box) \
  (NW_FBox_FormBoxOf(_box)->skin)

#define NW_FBox_FormBox_GetInteractor(_box) \
  (NW_FBox_FormBoxOf(_box)->interactor)

#define NW_FBox_FormBox_GetFormCntrlID(_box) \
  (NW_FBox_FormBoxOf(_box)->formCntrlId)

#define NW_FBox_FormBox_SetFormCntrlID(_box, _formCntrlId) \
  (NW_FBox_FormBoxOf(_box)->formCntrlId = _formCntrlId)

#define NW_FBox_FormBox_GetFormLiaison(_box) \
  (NW_FBox_FormBoxOf(_box)->formLiaison)

#define NW_FBox_FormBox_InitSkin(_object) \
  (NW_Object_Invoke (_object, NW_FBox_FormBox, initSkin) \
  ((_object)))

#define NW_FBox_FormBox_InitInteractor(_object) \
  (NW_Object_Invoke (_object, NW_FBox_FormBox, initInteractor) \
  ((_object)))

#define NW_FBox_FormBox_Reset(_object) \
  (NW_Object_Invoke (_object, NW_FBox_FormBox, reset) \
  ((_object)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif
