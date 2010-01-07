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

#include "nw_structure_titleelementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_text_ucs2.h"
#include <nw_dom_text.h>
#include "nwx_string.h"
#include "BrsrStatusCodes.h"
#include <MemoryManager.h>

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_titleElementHandler_Class_t NW_XHTML_titleElementHandler_Class = {
  { /* NW_Object_Core               */
    /* super                        */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface               */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler      */
    /* initialize                   */ _NW_XHTML_titleElementHandler_Initialize,
    /* createBoxTree                */ _NW_XHTML_titleElementHandler_CreateBoxTree,
    /* processEvent                 */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_titleElementHandler */
    /* unused                       */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_titleElementHandler_t NW_XHTML_titleElementHandler = {
  { { &NW_XHTML_titleElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_titleElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode)
{
  NW_DOM_Node_t* childNode;
  NW_Text_t* title = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (elementHandler,
                                     &NW_XHTML_ElementHandler_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler,
                                     &NW_XHTML_ContentHandler_Class));
  NW_ASSERT (elementNode != NULL);

  
  NW_TRY (status) {

    childNode = NW_DOM_Node_getFirstChild(elementNode);  
    NW_THROW_ON_NULL(childNode, status, KBrsrSuccess);

    if (NW_DOM_Node_getNodeType(childNode) == NW_DOM_TEXT_NODE) {
      NW_Ucs2* storage;
      NW_Text_Length_t length;
      NW_Bool freeNeeded;
	  NW_Text_Length_t trimmedLen; 
      NW_Text_Length_t i;

      title = NW_XHTML_GetDOMTextNodeData(contentHandler, childNode);
      NW_THROW_OOM_ON_NULL (title, status);

      storage =
        NW_Text_GetUCS2Buffer (title, NW_Text_Flags_NullTerminated |
                                      NW_Text_Flags_CompressWhitespace,
                               &length, &freeNeeded);
      NW_THROW_OOM_ON_NULL (storage, status);


	  /*If there are &nbsp's in the title, convert them into spaces*/
	  /* Trim the string */
      trimmedLen = 0;
      for (i = 0; i < length; i++) {
        if ((storage[i] == NW_TEXT_UCS2_TAB) || (storage[i] == NW_TEXT_UCS2_NBSP)) {
          storage[trimmedLen++] = ' ';
		}
        else {
          storage[trimmedLen++] = storage[i];
		}
	  }
      storage[trimmedLen] = 0;  



      if (freeNeeded) {
        NW_Object_Delete (title);
        title = (NW_Text_t*)
          NW_Text_UCS2_New (storage, length, NW_Text_Flags_TakeOwnership);
        NW_THROW_OOM_ON_NULL (title, status);
      }

      /* set the contentHandler's title */
      (void) NW_XHTML_ContentHandler_SetTitle(contentHandler, title);

      status = NW_XHTML_ElementHandler_Class.NW_XHTML_ElementHandler.
           initialize (elementHandler, contentHandler, elementNode);
    }
  }
  
  NW_CATCH (status) {
    NW_Object_Delete(title);
    (void) NW_XHTML_ContentHandler_SetTitle(contentHandler, NULL);
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_titleElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                             NW_XHTML_ContentHandler_t* contentHandler,
                                             NW_DOM_ElementNode_t* elementNode,
                                             NW_LMgr_ContainerBox_t* parentBox)
{
  NW_REQUIRED_PARAM(elementHandler);
  NW_REQUIRED_PARAM(contentHandler);
  NW_REQUIRED_PARAM(elementNode);
  NW_REQUIRED_PARAM(parentBox);

  return KBrsrSuccess;
}
