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


#include "nw_xhtml_textelementhandleri.h"

#include "nw_lmgr_textbox.h"
#include <nw_dom_text.h>
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_hed_documentroot.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_adt_types.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_TextElementHandler_Class_t NW_XHTML_TextElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_TextElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_TextElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_TextElementHandler_t NW_XHTML_TextElementHandler = {
  { { &NW_XHTML_TextElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode  /*ARGSUSED*/
_NW_XHTML_TextElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* containerBox)
{
  NW_Text_t* text = NULL;
  NW_LMgr_TextBox_t* textBox = NULL;
  NW_LMgr_ContainerBox_t *parentBox = NULL;
  NW_Text_Length_t textLen,i;

  NW_TRY (status) {

    NW_REQUIRED_PARAM(elementHandler);

    text = NW_XHTML_GetDOMTextNodeData(contentHandler, (NW_DOM_TextNode_t*) elementNode);
    
    // check for whitespace
    textLen = NW_Text_GetCharCount(text);

    for (i = 0; i < textLen; i++) 
      {
      if (!NW_Str_Isspace((NW_Ucs2)NW_Text_GetAt(text, i))) 
        {
        contentHandler->ignoreFramesetElement = NW_TRUE;
        break;
        }
      }

    if (text == NULL) {
      return KBrsrSuccess; /* TODO: should we discriminate our "success"? */
    }
    parentBox = containerBox;

    /* create the TextBox and insert it into our parent */
    textBox = (NW_LMgr_TextBox_t*) NW_LMgr_TextBox_New (0, text);
    NW_THROW_OOM_ON_NULL (textBox, status);
    text = NULL;

    status = NW_LMgr_ContainerBox_AddChild (parentBox, NW_LMgr_BoxOf (textBox));
    _NW_THROW_ON_ERROR(status);
    textBox = NULL;

  } NW_CATCH (status) {
    if(textBox) {
      NW_Object_Delete(textBox);
    }
    if(text) {
      NW_Object_Delete (text);
    }
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

