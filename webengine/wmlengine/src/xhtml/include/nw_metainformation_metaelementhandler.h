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


#ifndef NW_XHTML_METAINFORMATION_METAELEMENTHANDLER_H
#define NW_XHTML_METAINFORMATION_METAELEMENTHANDLER_H

#include "nw_xhtml_elementhandler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* GENERATED */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_metaElementHandler_Class_s NW_XHTML_metaElementHandler_Class_t;
typedef struct NW_XHTML_metaElementHandler_s NW_XHTML_metaElementHandler_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_metaElementHandler_ClassPart_s {
  void** unused;
} NW_XHTML_metaElementHandler_ClassPart_t;

struct NW_XHTML_metaElementHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_XHTML_ElementHandler_ClassPart_t NW_XHTML_ElementHandler;
  NW_XHTML_metaElementHandler_ClassPart_t NW_XHTML_metaElementHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_metaElementHandler_s {
  NW_XHTML_ElementHandler_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_metaElementHandler_GetClassPart(_object) \
  (((NW_XHTML_metaElementHandler_Class_t*) NW_Object_GetClass(_object))->\
   NW_XHTML_metaElementHandler)

#define NW_XHTML_metaElementHandlerOf(_object) \
  ((NW_XHTML_metaElementHandler_t*) (_object))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_metaElementHandler_Class_t NW_XHTML_metaElementHandler_Class;
extern const NW_XHTML_metaElementHandler_t NW_XHTML_metaElementHandler;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_METAINFORMATION_METAELEMENTHANDLER_H */
