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


#include "nw_text_hrelementhandleri.h"

#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_dom_document.h"
#include "nw_lmgr_rulebox.h"
#include "NW_Text_Abstract.h"
#include "nw_dom_domvisitor.h"
#include "nwx_string.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   local constants
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_hrElementHandler_Class_t NW_XHTML_hrElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_hrElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_hrElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_hrElementHandler_t NW_XHTML_hrElementHandler = {
  { { &NW_XHTML_hrElementHandler_Class } }
};

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_hrElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode,
                                          NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_RuleBox_t *ruleBox = NULL;
  NW_LMgr_Box_t *box = NULL;
  TBrowserStatusCode getPropStatus;
  NW_LMgr_Property_t Prop;
  NW_Bool zeroSize = NW_FALSE;

  NW_TRY (status) 
  {
    ruleBox = NW_LMgr_RuleBox_New ();
    NW_THROW_OOM_ON_NULL (ruleBox, status);
    
    box = NW_LMgr_BoxOf(ruleBox);
    status = NW_LMgr_ContainerBox_AddChild (parentBox, box);  
    _NW_THROW_ON_ERROR (status);

    /* apply common attributes and styles */
    status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler,
                      elementNode, &box, 0);

    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if( !box )
      {
      NW_THROW_SUCCESS( status );
      }
    /* If either width is zero, or height is zero, rule will not be displayed.
     * This is the best place to deal with this, because we can still 
     * completely remove this block element from the tree.
     * (Attempting to remove it in lmgr will not remove newlines).
     */
    getPropStatus = NW_LMgr_Box_GetRawProperty(box, NW_CSS_Prop_width, &Prop);
    if (getPropStatus == KBrsrSuccess) {
      if ((Prop.type == NW_CSS_ValueType_Px && Prop.value.integer == 0) ||
          (Prop.type == NW_CSS_ValueType_Percentage && Prop.value.decimal == 0))
        zeroSize = NW_TRUE;
    }
    getPropStatus = NW_LMgr_Box_GetRawProperty(box, NW_CSS_Prop_height, &Prop);
    if (getPropStatus == KBrsrSuccess) {
      if (Prop.value.integer == 0)
          zeroSize = NW_TRUE;
    }
    
    if (zeroSize) {
      status = NW_LMgr_ContainerBox_RemoveChild (parentBox, box);
      if (status == KBrsrSuccess) {
        NW_Object_Delete(box);
      }
    }
  }
  NW_CATCH (status) 
  {
    if (box)
    {
      NW_Object_Delete (box);
    }
  }
  NW_FINALLY 
  {
    return status; 
  } NW_END_TRY
}

