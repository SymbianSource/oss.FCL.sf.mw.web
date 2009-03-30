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


#include "nw_text_coretextelementhandleri.h"
#include "nw_hed_documentroot.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_breakbox.h"
#include "nw_lmgr_nobrbox.h"
#include "nw_lmgr_marqueebox.h"
#include "BrsrStatusCodes.h"






/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_CoreTextElementHandler_Class_t NW_XHTML_CoreTextElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_CoreTextElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_Text_CoreTextElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_CoreTextElementHandler_t NW_XHTML_CoreTextElementHandler = {
  { { &NW_XHTML_CoreTextElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_CoreTextElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                                NW_XHTML_ContentHandler_t* contentHandler,
                                                NW_DOM_ElementNode_t* elementNode,
                                                NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t *box = NULL;

  NW_TRY (status )
  {
    if(NW_HED_DomHelper_GetElementToken (elementNode) != NW_XHTML_ElementToken_nobr)
    {
      box = (NW_LMgr_Box_t*)NW_LMgr_ContainerBox_New(0);
    }
    else
    {
      box = (NW_LMgr_Box_t*)NW_LMgr_NobrBox_New(0);
    }
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

    /*  if the display is none, remove the containerBox from the tree and return successfully */
    if(NW_XHTML_ElementToken_body == NW_HED_DomHelper_GetElementToken (elementNode))
    {
      /*
         Both frameset element and body elements should not belong in the same HTML
         page.  However, if both body and frameset do appear in the same page we will handle
         which ever one comes first.  For example if the body tag appears before the frameset
         tag, we will handle the body tag, but ignore the frameset tag (and its contents)
         So in this case, since we are handling the body tag first, set ignoreFrameset to TRUE
      */
      contentHandler->ignoreFramesetElement = NW_TRUE;
    }

    // No special fieldset handling required. CSS properties will define border etc.
    /* fieldset handling */
    //  if(NW_XHTML_ElementToken_fieldset == NW_HED_DomHelper_GetElementToken (elementNode))
    // {
    //   status = NW_XHTML_CoreTextElementHandler_HandleFieldSet(contentHandler, elementNode, (NW_LMgr_ContainerBox_t*)box);
    //   _NW_THROW_ON_ERROR (status);
    // }

    parentBox = NW_LMgr_ContainerBoxOf(box);

    /* invoke our superclass for completion */
    status = _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler, elementNode, parentBox);
    NW_THROW_ON (status, KBrsrOutOfMemory);
   }
   NW_CATCH (status)
   {
     if(box)
     {
        NW_Object_Delete(box);
     }
   }
   NW_FINALLY {
     return status;
   } NW_END_TRY
}
