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
* Description: 
*
*/


#ifndef NW_XHTML_OBJECT_OBJELEMENTHANDLER_H
#define NW_XHTML_OBJECT_OBJELEMENTHANDLER_H

#include "nw_xhtml_elementhandler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* GENERATED */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ObjectElementHandler_Class_s NW_XHTML_ObjectElementHandler_Class_t;
typedef struct NW_XHTML_ObjectElementHandler_s NW_XHTML_ObjectElementHandler_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ObjectElementHandler_ClassPart_s 
    {
    void** unused;
    } NW_XHTML_ObjectElementHandler_ClassPart_t;

struct NW_XHTML_ObjectElementHandler_Class_s 
    {
    NW_Object_Core_ClassPart_t NW_Object_Core;
    NW_XHTML_ElementHandler_ClassPart_t NW_XHTML_ElementHandler;
    NW_XHTML_ObjectElementHandler_ClassPart_t NW_XHTML_ObjectElementHandler;
    };

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_ObjectElementHandler_s 
    {
    NW_XHTML_ElementHandler_t super;
    };

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ObjectElementHandler_GetClassPart(_object) \
  (((NW_XHTML_ObjectElementHandler_Class_t*) NW_Object_GetClass(_object))->\
   NW_XHTML_ObjectElementHandler)

#define NW_XHTML_ObjectElementHandlerOf(_object) \
  ((NW_XHTML_ObjectElementHandler_t*) (_object))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_ObjectElementHandler_Class_t NW_XHTML_ObjectElementHandler_Class;
extern const NW_XHTML_ObjectElementHandler_t NW_XHTML_ObjectElementHandler;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
extern TBrowserStatusCode
NW_XHTML_ObjectElementHandler_ApplyInfoProperties (const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode,
        NW_LMgr_Box_t* aBox);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_OBJECT_OBJELEMENTHANDLER_H */
