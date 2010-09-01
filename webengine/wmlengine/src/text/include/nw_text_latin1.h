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


#ifndef NW_TEXT_Latin1_H
#define NW_TEXT_Latin1_H


#include "NW_Text_FixedWidth.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_Latin1_Class_s NW_Text_Latin1_Class_t;
typedef struct NW_Text_Latin1_s NW_Text_Latin1_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Text_Latin1_ClassPart_s {
  void** unused;
} NW_Text_Latin1_ClassPart_t;

struct NW_Text_Latin1_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_Text_Abstract_ClassPart_t NW_Text_Abstract;
  NW_Text_FixedWidth_ClassPart_t NW_Text_FixedWidth;
  NW_Text_Latin1_ClassPart_t NW_Text_Latin1;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Text_Latin1_s {
  NW_Text_FixedWidth_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Text_Latin1_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Text_Latin1))

#define NW_Text_Latin1Of(_object) \
  (NW_Object_Cast (_object, NW_Text_Latin1))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT const NW_Text_Latin1_Class_t NW_Text_Latin1_Class;

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
NW_Text_Latin1_t*
NW_Text_Latin1_New (void* storage,
                   NW_Text_Length_t characterCount,
                   NW_Uint16 flags);

NW_TEXT_EXPORT
TBrowserStatusCode
NW_Text_Latin1_Initialize (NW_Text_Latin1_t* Latin1,
                          void* storage,
                          NW_Text_Length_t characterCount,
                          NW_Uint16 flags);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXT_Latin1_H */
