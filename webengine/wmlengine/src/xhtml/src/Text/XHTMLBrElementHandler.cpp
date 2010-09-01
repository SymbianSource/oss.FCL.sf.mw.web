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


#include "nw_text_brelementhandleri.h"
#include "nw_lmgr_breakbox.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "BrsrStatusCodes.h"

/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_brElementHandler_Class_t NW_XHTML_brElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_brElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_brElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_brElementHandler_t NW_XHTML_brElementHandler = {
  { { &NW_XHTML_brElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
 * A break translates into two boxes in the box tree, an inline EmptyBox
 * placed at the end of the previous line, and a 0-height block box placed
 * immediately afterward.  The inline box is required for the cases when
 * there are several consecutive break elements in the source:  without
 * the extra box these would not result in additional whitespace.
 */
TBrowserStatusCode
_NW_XHTML_brElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode,
                                          NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t* breakBox = NULL;

  NW_TRY (status) 
  {
    NW_REQUIRED_PARAM(contentHandler);
    NW_REQUIRED_PARAM(elementNode);

    /* Create the break box */
    breakBox = (NW_LMgr_Box_t*)NW_LMgr_BreakBox_New ((NW_ADT_Vector_Metric_t)1);
    NW_THROW_OOM_ON_NULL (breakBox, status);

    status = NW_LMgr_ContainerBox_AddChild (parentBox, breakBox);
    _NW_THROW_ON_ERROR(status);

    /* apply common attributes and styles */
    status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler,
                                                  elementNode, &breakBox, 0);
    NW_THROW_ON (status, KBrsrOutOfMemory);
  } 
  NW_CATCH (status) 
  {
    NW_Object_Delete (breakBox);
  } NW_FINALLY 
  {
    return status;  
  } NW_END_TRY
}

