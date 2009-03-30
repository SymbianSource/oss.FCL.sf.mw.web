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


#ifndef NW_MARKUP_MODULE_H
#define NW_MARKUP_MODULE_H

#include "nw_object_base.h"
#include "nw_markup_elementhandler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_Module_Class_s NW_Markup_Module_Class_t;
typedef struct NW_Markup_Module_s NW_Markup_Module_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_Markup_ElementHandler_t*
(*NW_Markup_Module_GetElementHandler_t) (const NW_Markup_Module_t* module,
                                        const NW_DOM_ElementNode_t* domElement);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_Module_ClassPart_s {
  NW_Markup_Module_GetElementHandler_t getElementHandler;
} NW_Markup_Module_ClassPart_t;

struct NW_Markup_Module_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Markup_Module_ClassPart_t NW_Markup_Module;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Markup_Module_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Markup_Module_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_Markup_Module))

#define NW_Markup_ModuleOf(_object) \
  (NW_Object_Cast (_object, NW_Markup_Module))

 /* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Markup_Module_Class_t NW_Markup_Module_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Markup_Module_GetElementHandler(module, domElement) \
  (NW_Markup_Module_GetClassPart (module).\
    getElementHandler ((module), (domElement)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_MARKUP_MODULE_H */
