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


#ifndef NW_CSS_TOKENTOINT_H
#define NW_CSS_TOKENTOINT_H

#include "NW_CSS_EXPORT.h"
#include "nw_object_base.h"
#include "nw_lmgr_box.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
 /* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */

typedef struct NW_CSS_TokenToInt_Class_s NW_CSS_TokenToInt_Class_t;
typedef struct NW_CSS_TokenToInt_s NW_CSS_TokenToInt_t;

typedef struct NW_CSS_TokenToInt_Entry_s
{
  NW_LMgr_PropertyValueToken_t token;
  NW_Uint32 val;
}NW_CSS_TokenToInt_Entry_t;

typedef struct NW_CSS_TokenToDec_Entry_s
{
  NW_LMgr_PropertyValueToken_t token;
  NW_Float32 val;
}NW_CSS_TokenToDec_Entry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_CSS_TokenToInt_ClassPart_s {
  NW_Uint32 numFontSizeVals;
  const NW_CSS_TokenToDec_Entry_t* fontSizeList;
  NW_Uint32 numBorderWidthVals;
  const NW_CSS_TokenToInt_Entry_t* borderWidthList;
} NW_CSS_TokenToInt_ClassPart_t;

struct NW_CSS_TokenToInt_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_CSS_TokenToInt_ClassPart_t NW_CSS_TokenToInt;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_CSS_TokenToInt_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_CSS_TokenToInt_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_CSS_TokenToInt))

#define NW_CSS_TokenToIntOf(_object) \
  (NW_Object_Cast (_object, NW_CSS_TokenToInt))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_CSS_EXPORT const NW_CSS_TokenToInt_Class_t NW_CSS_TokenToInt_Class;
NW_CSS_EXPORT const NW_CSS_TokenToInt_t NW_CSS_TokenToInt;

/* -------------------------------------------------------------------------- *
    final methods
 * ---------------------------------------------------------------------------*/

NW_CSS_EXPORT
NW_Bool
NW_CSS_TokenToInt_GetFontSize(NW_CSS_TokenToInt_t* tokenToInt,
                              NW_LMgr_PropertyValueToken_t token,
                              NW_LMgr_Box_t* box,
                              NW_Float32* value);

NW_CSS_EXPORT
NW_Int32
NW_CSS_TokenToInt_GetFontWeight(NW_CSS_TokenToInt_t* thisObj,
                                NW_LMgr_PropertyValueToken_t token,
                                NW_LMgr_Box_t* box);

NW_CSS_EXPORT
NW_Int32 
NW_CSS_TokenToInt_GetBorderWidth(NW_CSS_TokenToInt_t* tokenToInt,
                                NW_LMgr_PropertyValueToken_t token,
                                NW_LMgr_Box_t* box);

NW_CSS_EXPORT
NW_Int32 
NW_CSS_TokenToInt_GetColorVal(NW_CSS_TokenToInt_t* thisObj,
                              NW_Bool transparentAllowed,
                              NW_LMgr_PropertyValueToken_t token);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_CSS_TOKENTOINT_H */
