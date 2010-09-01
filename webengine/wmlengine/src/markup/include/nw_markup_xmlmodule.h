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


#ifndef NW_MARKUP_XMLMODULE_H
#define NW_MARKUP_XMLMODULE_H

#include "nw_markup_module.h"
#include "nw_markup_elementhandler.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_XMLModule_Class_s NW_Markup_XMLModule_Class_t;
typedef struct NW_Markup_XMLModule_s NW_Markup_XMLModule_t;

typedef struct NW_Markup_XMLModule_ElementHandlerEntry_s NW_Markup_XMLModule_ElementHandlerEntry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_XMLModule_ClassPart_s {
  NW_Uint32 numElementHandlers;
  const NW_Markup_XMLModule_ElementHandlerEntry_t* elementHandlers;
} NW_Markup_XMLModule_ClassPart_t;

struct NW_Markup_XMLModule_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Markup_Module_ClassPart_t NW_Markup_Module;
  NW_Markup_XMLModule_ClassPart_t NW_Markup_XMLModule;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Markup_XMLModule_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Markup_XMLModule_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_Markup_XMLModule))

#define NW_Markup_XMLModuleOf(_object) \
  (NW_Object_Cast (_object, NW_Markup_XMLModule))

 /* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Markup_XMLModule_Class_t NW_Markup_XMLModule_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_MARKUP_XMLMODULE_H */
