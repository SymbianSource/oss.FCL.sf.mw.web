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


#include "nw_text_qelementhandleri.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_lmgr_textbox.h"
#include "nw_text_ucs2.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "BrsrStatusCodes.h"



/* ------------------------------------------------------------------------- */
static
NW_LMgr_Box_t*
NW_XHTML_qElementHandler_NewQuoteBox (NW_Text_t* quoteText)
{
  NW_LMgr_Box_t* box = NULL;

  if (quoteText != NULL) {
    box = (NW_LMgr_Box_t*) NW_LMgr_TextBox_New(1, NW_TextOf (quoteText));
  }
  return box;
}

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_qElementHandler_Class_t NW_XHTML_qElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_qElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_Text_qElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_qElementHandler_t NW_XHTML_qElementHandler = {
  { { &NW_XHTML_qElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_qElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                         NW_XHTML_ContentHandler_t* contentHandler,
                                         NW_DOM_ElementNode_t* elementNode,
                                         NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t *box = NULL;
  NW_LMgr_PropertyValue_t quoteVal;
  NW_LMgr_Box_t* qBox1 = NULL;
  NW_LMgr_Box_t* qBox2 = NULL;
  NW_Text_t* quoteText = NULL;
  NW_Text_t* openQuote = NULL;
  NW_Text_t* closeQuote = NULL;
  NW_Bool freeNeeded = NW_FALSE;
  
  NW_TRY (status) 
  {
    box = (NW_LMgr_Box_t*)NW_LMgr_ContainerBox_New(0);
    NW_THROW_OOM_ON_NULL (box, status);

    /* Add the box to the tree */
    status = NW_LMgr_ContainerBox_AddChild (parentBox, box);
    _NW_THROW_ON_ERROR (status);

    /* apply common attributes and styles */
    status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler, 
                                         elementNode, &box, NULL);
    NW_THROW_ON (status, KBrsrOutOfMemory);

    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (box == NULL)
    {
      NW_THROW_SUCCESS (status);
    }

    parentBox = NW_LMgr_ContainerBoxOf(box);

    /* Get the quote string */
    quoteVal.object = NULL; 
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_imodeQuotes, 
                                          NW_CSS_ValueType_Text, &quoteVal);
    NW_THROW_ON (status, KBrsrOutOfMemory);

    quoteText = (NW_Text_t*)quoteVal.object;
    if (quoteText == NULL) 
    {
      NW_Ucs2* quoteStr = NW_Str_New(2);
      NW_THROW_OOM_ON_NULL (quoteStr, status);
      quoteStr[0] = '\"';
      quoteStr[1] = '\"';
      quoteText = (NW_Text_t*)NW_Text_UCS2_New(quoteStr, 2, 0);
      freeNeeded = NW_TRUE;
      NW_THROW_OOM_ON_NULL (quoteText, status);
    }

    openQuote = NW_Text_Copy(quoteText, 0);
    NW_THROW_OOM_ON_NULL (openQuote, status);

    closeQuote = NW_Text_Split(openQuote, 1);
    NW_THROW_OOM_ON_NULL (closeQuote, status);

    qBox1 = NW_XHTML_qElementHandler_NewQuoteBox(openQuote);
    NW_THROW_OOM_ON_NULL (qBox1, status);

    qBox2 = NW_XHTML_qElementHandler_NewQuoteBox(closeQuote);
    NW_THROW_OOM_ON_NULL (qBox2, status);

    /* add a quote-box before the boxtree */
    status = NW_LMgr_ContainerBox_AddChild (parentBox, qBox1);
    _NW_THROW_ON_ERROR (status);

    /* add the boxtree */
    status = _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler, elementNode, parentBox);
    _NW_THROW_ON_ERROR (status);

    /* add a quote-box after the boxtree */
    status = NW_LMgr_ContainerBox_AddChild (parentBox, qBox2);
    _NW_THROW_ON_ERROR (status);

  }
  NW_CATCH (status) 
  {
    if(box) 
    {
      NW_Object_Delete(box);
    }
    if (openQuote)
    {
      NW_Object_Delete (openQuote);
    }
    if (closeQuote)
    {
      NW_Object_Delete (closeQuote);
    }
    if (qBox1)
    {
      NW_Object_Delete (qBox1);
    }
    if (qBox2)
    {
      NW_Object_Delete (qBox2);
    }
  }
  NW_FINALLY 
  {
    if (freeNeeded) 
    {
      NW_Object_Delete(quoteText);
    }
   return status;
  } NW_END_TRY
}
