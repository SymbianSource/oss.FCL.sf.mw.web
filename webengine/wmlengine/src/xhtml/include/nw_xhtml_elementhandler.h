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


#ifndef NW_XHTML_ELEMENTHANDLER_H
#define NW_XHTML_ELEMENTHANDLER_H

#include "nw_object_base.h"
#include "nw_lmgr_containerbox.h"
#include "nw_lmgr_activebox.h"
#include "nw_dom_node.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ElementHandler_Class_s NW_XHTML_ElementHandler_Class_t;
typedef struct NW_XHTML_ElementHandler_s NW_XHTML_ElementHandler_t;

typedef struct NW_XHTML_ContentHandler_s NW_XHTML_ContentHandler_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_XHTML_ElementHandler_Initialize_t) (const NW_XHTML_ElementHandler_t* elementHandler,
                                         NW_XHTML_ContentHandler_t* contentHandler,
                                         NW_DOM_ElementNode_t* elementNode);

typedef
TBrowserStatusCode
(*NW_XHTML_ElementHandler_CreateBoxTree_t) (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox);

typedef
NW_Uint8
(*NW_XHTML_ElementHandler_ProcessEvent_t) (const NW_XHTML_ElementHandler_t* elementHandler,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_LMgr_Box_t* box,
                                           NW_Evt_Event_t* event);

                                          
/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ElementHandler_ClassPart_s {
  NW_XHTML_ElementHandler_Initialize_t initialize;
  NW_XHTML_ElementHandler_CreateBoxTree_t createBoxTree;
  NW_XHTML_ElementHandler_ProcessEvent_t processEvent;
} NW_XHTML_ElementHandler_ClassPart_t;

struct NW_XHTML_ElementHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_XHTML_ElementHandler_ClassPart_t NW_XHTML_ElementHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_ElementHandler_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ElementHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_XHTML_ElementHandler))

#define NW_XHTML_ElementHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_XHTML_ElementHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_ElementHandler_Class_t NW_XHTML_ElementHandler_Class;
extern const NW_XHTML_ElementHandler_t NW_XHTML_ElementHandler;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ElementHandler_Initialize(_elementHandler, _contentHandler, _elementNode) \
  (NW_XHTML_ElementHandler_GetClassPart (_elementHandler).\
     initialize ((_elementHandler), (_contentHandler), (_elementNode)))

#define NW_XHTML_ElementHandler_CreateBoxTree(_elementHandler, _contentHandler, _elementNode, _parentBox) \
  (NW_XHTML_ElementHandler_GetClassPart (_elementHandler).\
     createBoxTree ((_elementHandler), (_contentHandler), (_elementNode), \
                    (_parentBox)))

#define NW_XHTML_ElementHandler_ProcessEvent(_elementHandler, _contentHandler, _elementNode, _activeBox, _event) \
  (NW_XHTML_ElementHandler_GetClassPart (_elementHandler).\
     processEvent ((_elementHandler), (_contentHandler), (_elementNode), \
                   (_activeBox), (_event)))

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */


TBrowserStatusCode
NW_XHTML_ElementHandler_ApplyStyles (const NW_XHTML_ElementHandler_t* thisObj,
                                     NW_XHTML_ContentHandler_t* contentHandler,
                                     NW_DOM_ElementNode_t* elementNode,
                                     NW_LMgr_Box_t** box,
                                     const NW_Evt_Event_t* event);

TBrowserStatusCode 
NW_XHTML_ElementHandler_GetAttribute (NW_XHTML_ContentHandler_t *contentHandler,
                                      NW_DOM_ElementNode_t *elementNode,
                                      const NW_Uint16 attrToken,
                                      NW_Ucs2 **attrValue);

/*
This should go away when the cXML library gets the ability to cope with
arbitrary byte orderings.  For now, the UCS-2 has to be fed to cXML in
network byte order.
*/
void
NW_XHTML_ElementHandler_Ucs2NativeToNetworkByteOrder(NW_Uint32 byteCount,
                                                       NW_Uint8* pBuf);

/* ------------------------------------------------------------------------- *
   convenience method
 * ------------------------------------------------------------------------- */
NW_Bool 
NW_XHTML_ElementHandler_IsBlockLevelTag(NW_Uint16 tagToken);

/* implementation of the callback function called from CSSHandler*/
TBrowserStatusCode
NW_XHTML_ElementHandler_ApplyCSSCallback (void* ch,
                                          NW_DOM_ElementNode_t* elementNode,
                                          NW_LMgr_Box_t** box);

NW_Bool 
NW_XHTML_ElementHandler_NextSiblingExists (NW_XHTML_ContentHandler_t *contentHandler,
                                           NW_DOM_ElementNode_t *elementNode,
                                           NW_Bool controlInitEH);

NW_Text_t*
NW_XHTML_ElementHandler_GetSubTreeText(NW_XHTML_ContentHandler_t* aContentHandler, 
                                       NW_DOM_ElementNode_t* aElementNode);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_ELEMENTHANDLER_H */
