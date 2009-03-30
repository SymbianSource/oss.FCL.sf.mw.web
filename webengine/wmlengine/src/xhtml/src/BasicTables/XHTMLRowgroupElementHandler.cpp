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


#include "nw_basictables_rowgroupelementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "nw_basictables_trelementhandler.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_rowgroupElementHandler_Class_t NW_XHTML_rowgroupElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_rowgroupElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_tableElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_rowgroupElementHandler_t NW_XHTML_rowgroupElementHandler = {
  { { &NW_XHTML_rowgroupElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_rowgroupElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                                NW_XHTML_ContentHandler_t* contentHandler,
                                                NW_DOM_ElementNode_t* elementNode,
                                                NW_LMgr_ContainerBox_t* parentBox)
{
	NW_REQUIRED_PARAM(elementHandler);
    
    NW_TRY (status) {
    NW_DOM_ElementNode_t* childNode;

    // We visit the children ourselves, rather than calling the superclass.
    // This is necessary so we can filter any unwanted elements from
    // the table. A row group can only handle <tr> elements. */
    for (childNode = NW_DOM_Node_getFirstChild (elementNode);
         childNode != NULL;
         childNode = NW_DOM_Node_getNextSibling (childNode)) {
      const NW_XHTML_ElementHandler_t* childElementHandler;

      // get the ElementHandler for the elementNode 
      childElementHandler =
        NW_XHTML_ContentHandler_GetElementHandler (contentHandler,
                                                   (NW_DOM_ElementNode_t*) childNode);
      if (childElementHandler == NULL) {
        continue;
      }

      // If this is not a table row, skip to next 
      // We need to remove this check because at the moment parser has no notion of tbody
      // It wraps tbody in a fake tr, td, so tbody will always be in a cell
      // Later on we need to update the parser to handle it properly
      //if (NW_Object_IsInstanceOf(childElementHandler, &NW_XHTML_trElementHandler_Class)) 
          {
        status = NW_XHTML_ElementHandler_CreateBoxTree (childElementHandler, contentHandler,
                             childNode, parentBox);
        _NW_THROW_ON_ERROR(status);
      } 
//      else {
  //      continue;
    //  }
    }

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

