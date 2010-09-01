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


#include "nw_basicforms_formelementhandleri.h"

#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_formElementHandler_Class_t NW_XHTML_formElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_formElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_formElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_formElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_formElementHandler_t NW_XHTML_formElementHandler = {
  { { &NW_XHTML_formElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_formElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                         NW_XHTML_ContentHandler_t* contentHandler,
                                         NW_DOM_ElementNode_t* elementNode)
{
  TBrowserStatusCode status;

  // parameter assertions 
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_XHTML_ContentHandler_Class));
  NW_ASSERT (elementNode != NULL);

  status = NW_XHTML_ContentHandler_AddFormNodes(contentHandler, elementNode);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* invoke our superclass method */
  status = _NW_XHTML_ElementHandler_Initialize(elementHandler, contentHandler, elementNode);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* create the FormLiaison */
  return NW_XHTML_ContentHandler_AddFormLiaison(contentHandler, elementNode);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_formElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t *box = NULL;
  
  // parameter assertions 
  NW_ASSERT (elementHandler != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (elementHandler, &NW_XHTML_formElementHandler_Class));
  NW_ASSERT (contentHandler != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler, &NW_XHTML_ContentHandler_Class));
  NW_ASSERT (elementNode != NULL);
  NW_ASSERT (parentBox != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (parentBox, &NW_LMgr_ContainerBox_Class));

  NW_TRY (status) 
  {
    box = (NW_LMgr_Box_t*)NW_LMgr_ContainerBox_New(0);
    NW_THROW_OOM_ON_NULL (box, status);

    // Add the box to the tree 
    status = NW_LMgr_ContainerBox_AddChild (parentBox, box);
    _NW_THROW_ON_ERROR (status);

    // apply common attributes and styles 
    status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler, elementNode, 
                                         &box, NULL);
    NW_THROW_ON (status, KBrsrOutOfMemory);

    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (box == NULL)
    {
      NW_THROW_SUCCESS (status);
    }

    parentBox = NW_LMgr_ContainerBoxOf(box);
    // invoke our superclass for completion 
    status = _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler, elementNode, parentBox);
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
