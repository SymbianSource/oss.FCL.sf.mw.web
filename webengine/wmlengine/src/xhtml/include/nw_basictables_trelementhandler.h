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


#ifndef NW_XHTML_BASICTABLES_TRELEMENTHANDLER_H
#define NW_XHTML_BASICTABLES_TRELEMENTHANDLER_H

#include "nw_xhtml_elementhandler.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* GENERATED */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_trElementHandler_Class_s NW_XHTML_trElementHandler_Class_t;
typedef struct NW_XHTML_trElementHandler_s NW_XHTML_trElementHandler_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_trElementHandler_ClassPart_s {
  void** unused;
} NW_XHTML_trElementHandler_ClassPart_t;

struct NW_XHTML_trElementHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_XHTML_ElementHandler_ClassPart_t NW_XHTML_ElementHandler;
  NW_XHTML_trElementHandler_ClassPart_t NW_XHTML_trElementHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_trElementHandler_s {
  NW_XHTML_ElementHandler_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_trElementHandler_GetClassPart(_object) \
  (((NW_XHTML_trElementHandler_Class_t*) NW_Object_GetClass(_object))->\
   NW_XHTML_trElementHandler)

#define NW_XHTML_trElementHandlerOf(_object) \
  ((NW_XHTML_trElementHandler_t*) (_object))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_trElementHandler_Class_t NW_XHTML_trElementHandler_Class;
extern const NW_XHTML_trElementHandler_t NW_XHTML_trElementHandler;


// global function
TBrowserStatusCode 
NW_XHTML_trElement_CreateBoxTree(NW_XHTML_ContentHandler_t* contentHandler,
                                       const NW_XHTML_ElementHandler_t* elementHandler,
                                       NW_DOM_ElementNode_t* elementNode,
                                       NW_LMgr_ContainerBox_t* trBox,
                                       NW_LMgr_ContainerBox_t* tableContainerParent);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_BASICTABLES_TRELEMENTHANDLER_H */
