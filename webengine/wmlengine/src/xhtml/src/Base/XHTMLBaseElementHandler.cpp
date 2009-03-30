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


#include "nw_base_baseelementhandleri.h"
#include "BrsrStatusCodes.h"


/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_baseElementHandler_Class_t NW_XHTML_baseElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_baseElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_baseElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_baseElementHandler_ProcessEvent
  },
  { /* NW_XHTML_baseElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_baseElementHandler_t NW_XHTML_baseElementHandler = {
  { { &NW_XHTML_baseElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_baseElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                         NW_XHTML_ContentHandler_t* contentHandler,
                                         NW_DOM_ElementNode_t* elementNode)
{
  NW_REQUIRED_PARAM(elementHandler);
  NW_REQUIRED_PARAM(contentHandler);
  NW_REQUIRED_PARAM(elementNode);

  /* by default we are successful */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_baseElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
{
  /* invoke our superclass for completion */
  return NW_XHTML_ElementHandler_Class.NW_XHTML_ElementHandler.
    createBoxTree (elementHandler, contentHandler, elementNode, parentBox);
}

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_XHTML_baseElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* elementHandler,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_LMgr_Box_t* box,
                                           NW_Evt_Event_t* event)
{
  NW_REQUIRED_PARAM(elementHandler);
  NW_REQUIRED_PARAM(contentHandler);
  NW_REQUIRED_PARAM(elementNode);
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(event);

  /* by default we don't handle the event */
  return 0;
}
