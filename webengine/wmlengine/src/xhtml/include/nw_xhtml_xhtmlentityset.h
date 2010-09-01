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


#ifndef NW_XHTML_XHTMLENTITYSET_H
#define NW_XHTML_XHTMLENTITYSET_H

#include "nw_hed_entityset.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_EntitySet_Class_s NW_XHTML_EntitySet_Class_t;
typedef struct NW_XHTML_EntitySet_s NW_XHTML_EntitySet_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_EntitySet_ClassPart_s {
  void** unused;
} NW_XHTML_EntitySet_ClassPart_t;

struct NW_XHTML_EntitySet_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_HED_EntitySet_ClassPart_t NW_HED_EntitySet;
  NW_XHTML_EntitySet_ClassPart_t NW_XHTML_EntitySet;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_EntitySet_s {
  NW_HED_EntitySet_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_EntitySet_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_XHTML_EntitySet))

#define NW_XHTML_EntitySetOf(_object) \
  (NW_Object_Cast (_object, NW_XHTML_EntitySet))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_EntitySet_Class_t NW_XHTML_EntitySet_Class;
extern const NW_XHTML_EntitySet_t NW_XHTML_EntitySet;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_XHTML_EntitySet_GetEntityChar(_entitySet, _name) \
  (NW_Object_Invoke (_entitySet, NW_HED_EntitySet, getEntityChar) \
   ((_entitySet), (_name)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_XHTMLENTITYSET_H */
