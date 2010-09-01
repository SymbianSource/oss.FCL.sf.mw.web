/*
* Copyright (c) 2000-2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_MARKUP_WMLVALIDATOR_H
#define NW_MARKUP_WMLVALIDATOR_H

#include "NW_Text_Abstract.h"  
#include "NW_FBox_Validator.h"
#include "NW_Markup_EXPORT.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- *
   public data types
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_WmlValidator_Class_s NW_Markup_WmlValidator_Class_t;
typedef struct NW_Markup_WmlValidator_s NW_Markup_WmlValidator_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */

 /* ------------------------------------------------------------------------- *
   class declaration
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_WmlValidator_ClassPart_s {
  void** unused;
} NW_Markup_WmlValidator_ClassPart_t;
  
struct NW_Markup_WmlValidator_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_Validator_ClassPart_t NW_FBox_Validator;
  NW_Markup_WmlValidator_ClassPart_t NW_Markup_WmlValidator;
};

/* ------------------------------------------------------------------------- *
   object declaration
 * ------------------------------------------------------------------------- */
struct NW_Markup_WmlValidator_s {
  NW_FBox_Validator_t super;

  /* member variables */
  const NW_Ucs2* formatUcs2;  /* Necessary because need UCS2 to pass to ISA. */
  NW_FBox_Validator_Mode_t mode;
  NW_FBox_Validator_EmptyOk_t emptyOk;
  NW_Bool fullMatch;
};
    
/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Markup_WmlValidator_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Markup_WmlValidator))

#define NW_Markup_WmlValidatorOf(object) \
  (NW_Object_Cast (object, NW_Markup_WmlValidator))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Markup_WmlValidator_Class_t NW_Markup_WmlValidator_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT
NW_Markup_WmlValidator_t*
NW_Markup_WmlValidator_New (const NW_Text_t* format,
                            const NW_FBox_Validator_Mode_t  mode,
                            const NW_FBox_Validator_EmptyOk_t emptyOk,
                            const NW_Bool fullMatch);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NW_MARKUP_WMLVALIDATOR_H */
