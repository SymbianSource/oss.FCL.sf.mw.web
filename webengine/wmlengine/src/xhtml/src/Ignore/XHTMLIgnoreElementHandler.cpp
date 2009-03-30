/*
* Copyright (c) 2000 - 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_ignore_ignoreelementhandleri.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_hed_documentroot.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_breakbox.h"
#include "BrsrStatusCodes.h"

/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_IgnoreElementHandler_Class_t NW_XHTML_IgnoreElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_IgnoreElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_Text_IgnoreElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_IgnoreElementHandler_t NW_XHTML_IgnoreElementHandler = {
  { { &NW_XHTML_IgnoreElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_IgnoreElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                                NW_XHTML_ContentHandler_t* contentHandler,
                                                NW_DOM_ElementNode_t* elementNode,
                                                NW_LMgr_ContainerBox_t* parentBox)
{
  NW_TRY (status) 
  {
    // Invoke our superclass for completion
    status = _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler, elementNode, parentBox);
    NW_THROW_ON (status, KBrsrOutOfMemory);
  }
  NW_CATCH (status) 
  {
  }
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY
}
