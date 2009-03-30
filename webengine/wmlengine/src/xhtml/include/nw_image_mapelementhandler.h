/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Object for containing map element handler specific methods.
*
*/


#ifndef NW_XHTML_IMAGE_MAPELEMENTHANDLER_H
#define NW_XHTML_IMAGE_MAPELEMENTHANDLER_H

//  INCLUDES
#include "nw_xhtml_elementhandler.h"
#include "nw_xhtml_elementhandleri.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// FORWARD DECLARATIONS
typedef struct NW_XHTML_mapElementHandler_Class_s NW_XHTML_mapElementHandler_Class_t;
typedef struct NW_XHTML_mapElementHandler_s NW_XHTML_mapElementHandler_t;

// CLASS DECLARATION
typedef struct NW_XHTML_mapElementHandler_ClassPart_s {
  void** unused;
} NW_XHTML_mapElementHandler_ClassPart_t;

struct NW_XHTML_mapElementHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_XHTML_ElementHandler_ClassPart_t NW_XHTML_ElementHandler;
  NW_XHTML_mapElementHandler_ClassPart_t NW_XHTML_mapElementHandler;
};

// OBJECT DEFINITION
struct NW_XHTML_mapElementHandler_s {
  NW_XHTML_ElementHandler_t super;
};

// CONVENIENCE MACROS
#define NW_XHTML_mapElementHandler_ClassPartOf(_object) \
  (((NW_XHTML_mapElementHandler_Class_t*) NW_Object_ClassOf(_object))->\
   NW_XHTML_mapElementHandler)

#define NW_XHTML_mapElementHandlerOf(_object) \
  ((NW_XHTML_mapElementHandler_t*) (_object))

// GLOBAL STATIC DATA
extern const NW_XHTML_mapElementHandler_Class_t NW_XHTML_mapElementHandler_Class;
extern const NW_XHTML_mapElementHandler_t NW_XHTML_mapElementHandler;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif // NW_XHTML_IMAGE_MAPELEMENTHANDLER_H 
