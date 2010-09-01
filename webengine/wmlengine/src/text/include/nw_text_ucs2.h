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


#ifndef NW_TEXT_UCS2_H
#define NW_TEXT_UCS2_H


#include "NW_Text_FixedWidth.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   public constants
 * ------------------------------------------------------------------------- */
#define NW_TEXT_UCS2_HYPHEN        0x2d
#define NW_TEXT_UCS2_SOFT_HYPHEN   0xad
#define NW_TEXT_UCS2_NBSP          0xa0
#define NW_TEXT_UCS2_LF            0x0a
#define NW_TEXT_UCS2_CR            0x0d
#define NW_TEXT_UCS2_TAB           0x09
#define NW_TEXT_UCS2_SPACE         0x20
#define NW_TEXT_UCS2_PARASEP       0x2029

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_UCS2_Class_s NW_Text_UCS2_Class_t;
typedef struct NW_Text_UCS2_s NW_Text_UCS2_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_UCS2_ClassPart_s {
  void** unused;
} NW_Text_UCS2_ClassPart_t;

struct NW_Text_UCS2_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Text_Abstract_ClassPart_t NW_Text_Abstract;
  NW_Text_FixedWidth_ClassPart_t NW_Text_FixedWidth;
  NW_Text_UCS2_ClassPart_t NW_Text_UCS2;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Text_UCS2_s {
  NW_Text_FixedWidth_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Text_UCS2_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Text_UCS2))

#define NW_Text_UCS2Of(_object) \
  (NW_Object_Cast (_object, NW_Text_UCS2))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT const NW_Text_UCS2_Class_t NW_Text_UCS2_Class;

/* ------------------------------------------------------------------------- *
   public method prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_UCS2_SetStorage (NW_Text_UCS2_t* thisObj,
                          void* storage,
                          NW_Text_Length_t characterCount,
                          NW_Uint16 flags);

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Text_UCS2_SetStorage(_object, _storage, _characterCount, _flags) \
  (_NW_Text_UCS2_SetStorage ( \
     NW_Text_UCS2Of (_object), (_storage), (_characterCount), (_flags)))

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
NW_Text_UCS2_t*
NW_Text_UCS2_New (void* storage,
                  NW_Text_Length_t characterCount,
                  NW_Uint16 flags);

NW_TEXT_EXPORT
TBrowserStatusCode
NW_Text_UCS2_Initialize (NW_Text_UCS2_t* UCS2,
                         void* storage,
                         NW_Text_Length_t characterCount,
                         NW_Uint16 flags);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXT_UCS2_H */
