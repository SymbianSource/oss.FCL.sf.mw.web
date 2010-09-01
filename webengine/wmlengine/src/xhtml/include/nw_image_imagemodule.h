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


#ifndef NW_XHTML_IMAGE_IMAGEMODULE_H
#define NW_XHTML_IMAGE_IMAGEMODULE_H

#include "nw_markup_wbxmlmodule.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct NW_XHTML_ElementHandler_s NW_XHTML_ElementHandler_t;

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ImageModule_Class_s
  NW_XHTML_ImageModule_Class_t;
typedef struct NW_XHTML_ImageModule_s NW_XHTML_ImageModule_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ImageModule_ClassPart_s {
  void** unused;
} NW_XHTML_ImageModule_ClassPart_t;

struct NW_XHTML_ImageModule_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Markup_Module_ClassPart_t NW_Markup_Module;
  NW_Markup_WBXMLModule_ClassPart_t NW_Markup_WBXMLModule;
  NW_XHTML_ImageModule_ClassPart_t NW_XHTML_ImageModule;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_ImageModule_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ImageModule_GetClassPart(_object) \
  (((NW_XHTML_ImageModule_Class_t*) NW_Object_GetClass(_object))->\
   NW_XHTML_ImageModule)

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_ImageModule_Class_t NW_XHTML_ImageModule_Class;
extern const NW_XHTML_ImageModule_t NW_XHTML_ImageModule;

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ImageModuleOf(_object) \
  ((NW_XHTML_ImageModule_t*) (_object))

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

NW_Bool NW_XHTML_ImageModule_IsImageElementHandler(const NW_XHTML_ElementHandler_t* elementHandler);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_IMAGE_IMAGEMODULE_H */
