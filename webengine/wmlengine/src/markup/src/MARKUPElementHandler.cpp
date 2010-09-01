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


#include "nw_markup_elementhandleri.h"
#include "BrsrStatusCodes.h"

/* -------------------------------------------------------------------------- *
    class definition
 * ---------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------- */
const
NW_Markup_ElementHandler_Class_t NW_Markup_ElementHandler_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Core_Class,
    /* queryInterface            */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Markup_ElementHandler	 */
    /* initialize                */ _NW_Markup_ElementHandler_Initialize,
    /* createBoxTree             */ _NW_Markup_ElementHandler_CreateBoxTree,
    /* processEvent              */ _NW_Markup_ElementHandler_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/*const NW_Markup_ElementHandler_t NW_Markup_ElementHandler = {
  { &NW_Markup_ElementHandler_Class }
};*/

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Markup_ElementHandler_Initialize (const NW_Markup_ElementHandler_t* thisObj,
                                      NW_Markup_ContentHandler_t* contentHandler,
                                     NW_DOM_ElementNode_t* elementNode)
{
  /* avoid 'unreferenced formal parameter' warnings */
  (void) thisObj;
  (void) contentHandler;
  (void) elementNode;

  /* by default, we do nothing ... */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Markup_ElementHandler_CreateBoxTree (const NW_Markup_ElementHandler_t* thisObj,
                                         NW_Markup_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode,
                                        NW_LMgr_ContainerBox_t* parentBox)
{
  (void) thisObj;
  (void) contentHandler;
  (void) elementNode;
  (void) parentBox;

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_Markup_ElementHandler_ProcessEvent (const NW_Markup_ElementHandler_t* thisObj,
                                        NW_Markup_ContentHandler_t* contentHandler,
                                       NW_DOM_ElementNode_t* elementNode,
                                       NW_LMgr_Box_t* box,
                                        NW_Evt_Event_t* event)
{
  /* avoid 'unreferenced formal parameter' warnings */
  (void) thisObj;
  (void) contentHandler;
  (void) elementNode;
  (void) box;
  (void) event;

  /* by default, we don't absorb the event ... */
  return 0;
}
