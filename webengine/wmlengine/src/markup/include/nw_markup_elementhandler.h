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


#ifndef NW_MARKUP_ELEMENTHANDLER_H
#define NW_MARKUP_ELEMENTHANDLER_H

#include "nw_object_base.h"
#include "nw_lmgr_containerbox.h"
#include "nw_lmgr_activebox.h"
#include <nw_dom_node.h>
#include "NW_Markup_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_ElementHandler_Class_s NW_Markup_ElementHandler_Class_t;
typedef struct NW_Markup_ElementHandler_s NW_Markup_ElementHandler_t;

typedef struct NW_Markup_ContentHandler_s NW_Markup_ContentHandler_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_Markup_ElementHandler_Initialize_t) (const NW_Markup_ElementHandler_t* elementHandler,
                                         NW_Markup_ContentHandler_t* contentHandler,
                                         NW_DOM_ElementNode_t* elementNode);

typedef
TBrowserStatusCode
(*NW_Markup_ElementHandler_CreateBoxTree_t) (const NW_Markup_ElementHandler_t* elementHandler,
                                            NW_Markup_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox);

typedef
NW_Uint8
(*NW_Markup_ElementHandler_ProcessEvent_t) (const NW_Markup_ElementHandler_t* elementHandler,
                                           NW_Markup_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_LMgr_Box_t* box,
                                           NW_Evt_Event_t* event);

                                          
/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Markup_ElementHandler_ClassPart_s {
  NW_Markup_ElementHandler_Initialize_t initialize;
  NW_Markup_ElementHandler_CreateBoxTree_t createBoxTree;
  NW_Markup_ElementHandler_ProcessEvent_t processEvent;
} NW_Markup_ElementHandler_ClassPart_t;

struct NW_Markup_ElementHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Markup_ElementHandler_ClassPart_t NW_Markup_ElementHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Markup_ElementHandler_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Markup_ElementHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_Markup_ElementHandler))

#define NW_Markup_ElementHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_Markup_ElementHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Markup_ElementHandler_Class_t NW_Markup_ElementHandler_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_Markup_ElementHandler_Initialize(_elementHandler, _contentHandler, _elementNode) \
  (NW_Markup_ElementHandler_GetClassPart (_elementHandler).\
     initialize ((_elementHandler), (_contentHandler), (_elementNode)))

#define NW_Markup_ElementHandler_CreateBoxTree(_elementHandler, _contentHandler, _elementNode, _parentBox) \
  (NW_Markup_ElementHandler_GetClassPart (_elementHandler).\
     createBoxTree ((_elementHandler), (_contentHandler), (_elementNode), \
                    (_parentBox)))

#define NW_Markup_ElementHandler_ProcessEvent(_elementHandler, _contentHandler, _elementNode, _activeBox, _event) \
  (NW_Markup_ElementHandler_GetClassPart (_elementHandler).\
     processEvent ((_elementHandler), (_contentHandler), (_elementNode), \
                   (_activeBox), (_event)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_MARKUP_ELEMENTHANDLER_H */
