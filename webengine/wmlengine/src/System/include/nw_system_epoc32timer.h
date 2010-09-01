/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _NW_System_Epoc32Timer_h_
#define _NW_System_Epoc32Timer_h_

#include "nw_system_timer.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/** ----------------------------------------------------------------------- **
    @class:       NW_System_Epoc32Timer

    @scope:       public

    @description: 
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_System_Epoc32Timer_Class_s NW_System_Epoc32Timer_Class_t;
typedef struct NW_System_Epoc32Timer_s NW_System_Epoc32Timer_t;

/* ------------------------------------------------------------------------- *
   object class structure
 * ------------------------------------------------------------------------- */
typedef struct NW_System_Epoc32Timer_ClassPart_s {
  void** unused;
} NW_System_Epoc32Timer_ClassPart_t;

struct NW_System_Epoc32Timer_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_System_Timer_ClassPart_t NW_System_Timer;
  NW_System_Epoc32Timer_ClassPart_t NW_System_Epoc32Timer;
};

/* ------------------------------------------------------------------------- *
   object instance structure
 * ------------------------------------------------------------------------- */
struct NW_System_Epoc32Timer_s {
  NW_System_Timer_t super;

  /* member variables */
  void* epocTimer;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_System_Epoc32Timer_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_System_Epoc32Timer))

#define NW_System_Epoc32TimerOf(_object) \
  (NW_Object_Cast (_object, NW_System_Epoc32Timer))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_SYSTEM_EXPORT const NW_System_Epoc32Timer_Class_t NW_System_Epoc32Timer_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_System_Epoc32Timer_h_ */

