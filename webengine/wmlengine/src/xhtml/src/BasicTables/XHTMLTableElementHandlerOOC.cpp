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


#include "XhtmlTableElementHandlerOOCi.h"
#include "XhtmlTableElementHandler.h"

#include "nw_xhtml_xhtmlcontenthandler.h"


/* ------------------------------------------------------------------------- */
static TBrowserStatusCode
NW_XHTML_TableElementHandler_CreateImpl(const NW_XHTML_ElementHandler_t* aElementHandler,
        NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode)
  {
  CXHTMLTableElementHandler*  tableElementHandler = NULL;

  NW_ASSERT(aElementHandler != NULL);
  NW_ASSERT(aContentHandler != NULL);
  NW_ASSERT(aElementNode != NULL);

  NW_TRY (status) 
    {
    // Create the instance.
	TInt err;
    TRAP(err, tableElementHandler = CXHTMLTableElementHandler::NewL(aElementHandler,
        aContentHandler, aElementNode));
    NW_THROW_OOM_ON_NULL(tableElementHandler, status);

    // Associate the inst with the aElementNode.
    status = NW_XHTML_ContentHandler_SetElementHandlerInstance(aContentHandler,
        aElementNode, tableElementHandler);
    _NW_THROW_ON_ERROR(status);
    } 

  NW_CATCH (status) 
    {
    delete tableElementHandler;
    tableElementHandler = NULL;
    } 

  NW_FINALLY 
    {
    return status;
    } NW_END_TRY
  }
        

/* ------------------------------------------------------------------------- */
static CXHTMLTableElementHandler*
NW_XHTML_TableElementHandler_GetImpl(NW_XHTML_ContentHandler_t* aContentHandler,
        NW_DOM_ElementNode_t* aElementNode)
  {
  CXHTMLTableElementHandler*  tableElementHandler = NULL;

  NW_ASSERT(aContentHandler != NULL);
  NW_ASSERT(aElementNode != NULL);

  // Get the instance associated with the node
  tableElementHandler = (CXHTMLTableElementHandler*) 
      NW_XHTML_ContentHandler_GetElementHandlerInstance(aContentHandler, aElementNode);

  return tableElementHandler;
  }


/* ------------------------------------------------------------------------- */
static CXHTMLTableElementHandler*
NW_XHTML_TableElementHandler_FindImpl(NW_XHTML_ContentHandler_t* aContentHandler,
        NW_DOM_ElementNode_t* aElementNode)
  {
  NW_DOM_Node_t*              node = NULL;
  
  // Searching up the DOM tree for the table element.
  node = aElementNode;

  while (node && (NW_HED_DomHelper_GetElementToken(node) != NW_XHTML_ElementToken_table))
    {
    node = NW_DOM_Node_getParentNode(node);
    }

  if (node == NULL)
    {
    return NULL;
    }

  // Return the instance.
  return NW_XHTML_TableElementHandler_GetImpl(aContentHandler, node);
  }


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_tableElementHandler_Class_t NW_XHTML_tableElementHandler_Class = 
{
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_tableElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_tableElementHandler_CreateBoxTree,
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
const NW_XHTML_tableElementHandler_t NW_XHTML_tableElementHandler = 
{
  { { &NW_XHTML_tableElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_tableElementHandler_Initialize(const NW_XHTML_ElementHandler_t* aElementHandler,
    NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode)
  {
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT(aElementHandler != NULL);
  NW_ASSERT(aContentHandler != NULL);
  NW_ASSERT(aElementNode != NULL);

  // Create the TableElementHandler instance.
  if (!NW_XHTML_TableElementHandler_GetImpl(aContentHandler, aElementNode))
    {
    status = NW_XHTML_TableElementHandler_CreateImpl(aElementHandler, aContentHandler, 
        aElementNode);

    if (status != KBrsrSuccess) 
      {
      return status;
      }
    }

  // Pass it on to the implementation
  CXHTMLTableElementHandler* imp;

  imp = NW_XHTML_TableElementHandler_GetImpl(aContentHandler, aElementNode);
  NW_ASSERT(imp != NULL);

  return imp->Initialize();
  }


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_tableElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* aElementHandler,
    NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode,
    NW_LMgr_ContainerBox_t* aParentBox)
  {
  NW_REQUIRED_PARAM(aElementHandler);

  NW_ASSERT(aContentHandler != NULL);
  NW_ASSERT(aElementNode != NULL);
  NW_ASSERT(aParentBox != NULL);

  // Pass it on to the implementation
  CXHTMLTableElementHandler* imp;

  imp = NW_XHTML_TableElementHandler_GetImpl(aContentHandler, aElementNode);
  NW_ASSERT(imp != NULL);

  return imp->CreateBoxTree(*aParentBox);
  }


/* ------------------------------------------------------------------------- *
   Final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Bool
NW_XHTML_tableElementHandler_DoesGridModeApply (NW_XHTML_ContentHandler_t* aContentHandler,
    NW_DOM_ElementNode_t* aElementNode)
  {
  CXHTMLTableElementHandler*  tableElementHandler;

  tableElementHandler = NW_XHTML_TableElementHandler_FindImpl(aContentHandler, aElementNode);
  NW_ASSERT(tableElementHandler != NULL);

  // Return the value.
  return (NW_Bool) tableElementHandler->DoesGridModeApply();
  }

