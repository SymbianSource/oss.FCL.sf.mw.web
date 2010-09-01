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


#include "nw_basictables_captionelementhandleri.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "BrsrStatusCodes.h"


/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_captionElementHandler_Class_t NW_XHTML_captionElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_captionElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_captionElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_captionElementHandler_t NW_XHTML_captionElementHandler = {
  { { &NW_XHTML_captionElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_captionElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t* captionBox = NULL;

  NW_TRY (status) 
  {
    NW_DOM_ElementNode_t* prevSibling;

    /* reject multiple caption elements */
    prevSibling = elementNode;
    do
    {
      prevSibling = NW_DOM_Node_getPreviousSibling(prevSibling);
    }while(prevSibling && (NW_DOM_Node_getNodeType(prevSibling) != NW_DOM_ELEMENT_NODE));

    if (prevSibling &&
        (NW_DOM_ElementNode_getTagToken(prevSibling) == NW_XHTML_ElementToken_caption))
    {
      NW_THROW_SUCCESS(status);
    }

    captionBox = (NW_LMgr_Box_t*)NW_LMgr_BidiFlowBox_New(0);
    NW_THROW_OOM_ON_NULL (captionBox, status);

    status = NW_LMgr_ContainerBox_InsertChildAt(parentBox, captionBox, 0);
    _NW_THROW_ON_ERROR(status);

    /* apply common attributes and styles */
    status = NW_XHTML_ElementHandler_ApplyStyles(elementHandler, contentHandler, elementNode, 
                                                 &captionBox, 0);
    NW_THROW_ON (status, KBrsrOutOfMemory);
    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (captionBox == NULL)
    {
      NW_THROW_SUCCESS (status);
    }

    parentBox = NW_LMgr_ContainerBoxOf(captionBox);

    /* invoke our superclass for completion */
    status = _NW_XHTML_ElementHandler_CreateBoxTree (elementHandler, contentHandler, elementNode, parentBox);
  } 
  NW_CATCH (status) 
  {
    if (captionBox) 
    {
      NW_Object_Delete (captionBox);
    }
  } NW_FINALLY 
  {
    return status;
  } NW_END_TRY
}

