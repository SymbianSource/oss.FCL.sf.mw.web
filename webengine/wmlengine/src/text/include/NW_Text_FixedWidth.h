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


#ifndef NW_TEXT_FixedWidth_H
#define NW_TEXT_FixedWidth_H


#include "NW_Text_Abstract.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_FixedWidth_Class_s NW_Text_FixedWidth_Class_t;
typedef struct NW_Text_FixedWidth_s NW_Text_FixedWidth_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_FixedWidth_ClassPart_s {
  size_t characterSize;
} NW_Text_FixedWidth_ClassPart_t;

struct NW_Text_FixedWidth_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Text_Abstract_ClassPart_t NW_Text_Abstract;
  NW_Text_FixedWidth_ClassPart_t NW_Text_FixedWidth;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Text_FixedWidth_s {
  NW_Text_Abstract_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Text_FixedWidth_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Text_FixedWidth))

#define NW_Text_FixedWidthOf(_object) \
  (NW_Object_Cast (_object, NW_Text_FixedWidth))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT const NW_Text_FixedWidth_Class_t NW_Text_FixedWidth_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXT_FixedWidth_H */
