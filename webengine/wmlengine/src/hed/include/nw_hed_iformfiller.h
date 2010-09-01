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

#ifndef _NW_HED_IFormFiller_h_
#define _NW_HED_IFormFiller_h_

#include "nw_object_interface.h"
#include "nw_adt_dynamicvector.h"
#include "nw_lmgr_box.h"
#include "NW_HED_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @class:       NW_HED_IFormFiller

    @scope:       public

    @description: 
 ** ----------------------------------------------------------------------- **/
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_IFormFiller_Class_s NW_HED_IFormFiller_Class_t;
typedef struct NW_HED_IFormFiller_s NW_HED_IFormFiller_t;

typedef struct NW_HED_IFormFiller_ConfigEntry_s NW_HED_IFormFiller_ConfigEntry_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
NW_Bool
(*NW_HED_IFormFiller_ECMLExists_t) (NW_HED_IFormFiller_t* formFiller);
                                    
typedef
TBrowserStatusCode
(*NW_HED_IFormFiller_FillAll_t) (NW_HED_IFormFiller_t* formFiller, 
                                 NW_LMgr_Box_t** firstNotFilled);

typedef
TBrowserStatusCode
(*NW_HED_IFormFiller_FillOne_t) (NW_HED_IFormFiller_t* formFiller,
                                 NW_Ucs2* value, NW_LMgr_Box_t* node);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_IFormFiller_ClassPart_s {
  NW_HED_IFormFiller_ECMLExists_t ECMLExists;
  NW_HED_IFormFiller_FillAll_t fillAll;
  NW_HED_IFormFiller_FillOne_t fillOne;
} NW_HED_IFormFiller_ClassPart_t;

struct NW_HED_IFormFiller_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_HED_IFormFiller_ClassPart_t NW_HED_IFormFiller;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_IFormFiller_s {
  NW_Object_Interface_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_IFormFiller_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_HED_IFormFiller))

#define NW_HED_IFormFillerOf(_object) \
  (NW_Object_Cast (_object, NW_HED_IFormFiller))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

NW_HED_EXPORT const NW_HED_IFormFiller_Class_t NW_HED_IFormFiller_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @method:      NW_HED_IFormFiller_ECMLExists

    @synopsis:    

    @scope:       public
    @access:      virtual

    @parameters:

      
    @description: 
                  
                  
    @returns:     
 ** ----------------------------------------------------------------------- **/
#define NW_HED_IFormFiller_ECMLExists(_formFiller) \
  (NW_Object_Invoke (_formFiller, NW_HED_IFormFiller, ECMLExists) ( \
     NW_HED_IFormFillerOf (_formFiller)))

#define NW_HED_IFormFiller_FillAll(_formFiller, _firstNotFilled) \
  (NW_Object_Invoke (_formFiller, NW_HED_IFormFiller, fillAll) ( \
     NW_HED_IFormFillerOf (_formFiller), (_firstNotFilled)))

#define NW_HED_IFormFiller_FillOne(_formFiller, _value, _node) \
  (NW_Object_Invoke (_formFiller, NW_HED_IFormFiller, fillOne) ( \
     NW_HED_IFormFillerOf (_formFiller), (_value), (_node)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_INumberCollector_h_ */
