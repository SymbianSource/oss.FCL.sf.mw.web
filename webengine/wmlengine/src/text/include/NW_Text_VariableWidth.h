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


#ifndef NW_TEXT_VariableWidth_H
#define NW_TEXT_VariableWidth_H


#include "NW_Text_Abstract.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_VariableWidth_Class_s NW_Text_VariableWidth_Class_t;
typedef struct NW_Text_VariableWidth_s NW_Text_VariableWidth_t;

/* ------------------------------------------------------------------------- *
   virtual method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Text_VariableWidth_ReadCharacter_t) (NW_Text_VariableWidth_t* textVariableWidth,
                                          NW_Int32 offset,
                                          NW_Ucs4* character,
                                          NW_Uint32* numBytes);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_VariableWidth_ClassPart_s {
  NW_Text_VariableWidth_ReadCharacter_t readCharacter;
} NW_Text_VariableWidth_ClassPart_t;

struct NW_Text_VariableWidth_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Text_Abstract_ClassPart_t NW_Text_Abstract;
  NW_Text_VariableWidth_ClassPart_t NW_Text_VariableWidth;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Text_VariableWidth_s {
  NW_Text_Abstract_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Text_VariableWidth_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Text_VariableWidth))

#define NW_Text_VariableWidthOf(_object) \
  (NW_Object_Cast (_object, NW_Text_VariableWidth))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT const NW_Text_VariableWidth_Class_t NW_Text_VariableWidth_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXT_VariableWidth_H */
