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


#include "nw_frame_framesetelementhandleri.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_hed_documentroot.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_breakbox.h"
#include "nwx_settings.h"

/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_FramesetElementHandler_Class_t NW_XHTML_FramesetElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_FramesetElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_Text_FramesetElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_FramesetElementHandler_t NW_XHTML_FramesetElementHandler = {
  { { &NW_XHTML_FramesetElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_FramesetElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                                NW_XHTML_ContentHandler_t* contentHandler,
                                                NW_DOM_ElementNode_t* elementNode,
                                                NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t *box = NULL;
  NW_Bool ignoreFrameSet = NW_FALSE;
  NW_DOM_ElementNode_t* node = elementNode;

  NW_TRY (status ) 
  {
    // The purpose of this while loop is to see if this frameset has a parent frameset
    // (nested framesets) If so, we don't want to process this frameset (mainly for 
    // display reasons)
    while ((node = NW_DOM_Node_getParentNode(node)) != NULL) 
    {
      if ((NW_HED_DomHelper_GetElementToken (node) == NW_XHTML_ElementToken_frameset))
      {
        /* found a parent frameset node */
        ignoreFrameSet = NW_TRUE;
        break;
      }
    }
    
    // If we are not processing the child frameset, invoke the superclass createBoxTree method
    // for completion
    if (ignoreFrameSet)
    {
      status = _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler, elementNode, parentBox);
      NW_THROW(status);
    }

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

    /* 
       Both frameset element and body elements should not belong in the same HTML
       page.  However, if both body and frameset do appear in the same page we will handle
       which ever one comes first.  For example if the frameset tag appears before the body
       tag, we will handle the framest tag, but ignore the body tag (and its contents)
       So in this case, since we are handling the framest tag first, set ignoreBody to TRUE
    */
    contentHandler->ignoreBodyElement = NW_TRUE;

    parentBox = NW_LMgr_ContainerBoxOf (box);

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
   NW_FINALLY 
   {
     return status;
   } NW_END_TRY
}
