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


#ifndef _NW_Object_Interface_h_
#define _NW_Object_Interface_h_

#include "nw_object_secondary.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @class:       NW_Object_Interface

    @scope:       public

    @description: Base class for all interfaces.
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Object_Secondary_Class_s NW_Object_Interface_Class_t;
typedef struct NW_Object_Secondary_s NW_Object_Interface_t;
typedef struct NW_Object_Secondary_ClassPart_s NW_Object_Interface_ClassPart_t;

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Object_Interface_GetClassPart(_object) \
  (NW_Object_Secondary_GetClassPart (_object))

#define NW_Object_InterfaceOf(_object) \
  (NW_Object_SecondaryOf (_object))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
#define NW_Object_Interface_Class NW_Object_Secondary_Class

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Object_Interface_GetImplementer \
  NW_Object_GetOuter

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Object_Interface_h_ */
