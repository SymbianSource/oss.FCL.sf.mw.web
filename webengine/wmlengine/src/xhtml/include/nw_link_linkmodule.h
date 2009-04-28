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


#ifndef NW_XHTML_LINK_LINKMODULE_H
#define NW_XHTML_LINK_LINKMODULE_H

#include "nw_markup_wbxmlmodule.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_LinkModule_Class_s
  NW_XHTML_LinkModule_Class_t;
typedef struct NW_XHTML_LinkModule_s NW_XHTML_LinkModule_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_LinkModule_ClassPart_s {
  void** unused;
} NW_XHTML_LinkModule_ClassPart_t;

struct NW_XHTML_LinkModule_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Markup_Module_ClassPart_t NW_Markup_Module;
  NW_Markup_WBXMLModule_ClassPart_t NW_Markup_WBXMLModule;
  NW_XHTML_LinkModule_ClassPart_t NW_XHTML_LinkModule;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_LinkModule_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_LinkModule_GetClassPart(_object) \
  (((NW_XHTML_LinkModule_Class_t*) NW_Object_GetClass(_object))->\
   NW_XHTML_LinkModule)

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_LinkModule_Class_t NW_XHTML_LinkModule_Class;
extern const NW_XHTML_LinkModule_t NW_XHTML_LinkModule;

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_LinkModuleOf(_object) \
  ((NW_XHTML_LinkModule_t*) (_object))

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_LINK_LINKMODULE_H */