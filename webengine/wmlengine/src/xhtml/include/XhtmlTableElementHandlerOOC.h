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


#ifndef XHTML_TABLEELEMENTHANDLER_H
#define XHTML_TABLEELEMENTHANDLER_H

#include "nw_xhtml_elementhandler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_tableElementHandler_Class_s NW_XHTML_tableElementHandler_Class_t;
typedef struct NW_XHTML_tableElementHandler_s NW_XHTML_tableElementHandler_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_tableElementHandler_ClassPart_s {
  void** unused;
} NW_XHTML_tableElementHandler_ClassPart_t;

struct NW_XHTML_tableElementHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_XHTML_ElementHandler_ClassPart_t NW_XHTML_ElementHandler;
  NW_XHTML_tableElementHandler_ClassPart_t NW_XHTML_tableElementHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_tableElementHandler_s {
  NW_XHTML_ElementHandler_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_tableElementHandler_GetClassPart(_object) \
  (((NW_XHTML_tableElementHandler_Class_t*) NW_Object_GetClass(_object))->\
   NW_XHTML_tableElementHandler)

#define NW_XHTML_tableElementHandlerOf(_object) \
  ((NW_XHTML_tableElementHandler_t*) (_object))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_tableElementHandler_Class_t NW_XHTML_tableElementHandler_Class;
extern const NW_XHTML_tableElementHandler_t NW_XHTML_tableElementHandler;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */
NW_Bool
NW_XHTML_tableElementHandler_DoesGridModeApply (NW_XHTML_ContentHandler_t* aContentHandler,
    NW_DOM_ElementNode_t* aElementNode);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* XHTML_TABLEELEMENTHANDLER_H */
