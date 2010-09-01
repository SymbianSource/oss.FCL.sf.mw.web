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


#include "nw_basictables_trelementhandleri.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_verticaltablebox.h"
#include "nw_lmgr_verticaltablerowbox.h"
#include "nw_lmgr_rootbox.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_basictables_cellelementhandler.h"
#include "nwx_string.h"
#include "nwx_settings.h"
#include "nw_basicforms_formelementhandler.h"
#include "nw_basicforms_inputelementhandler.h"
//#include "nw_structure_scriptelementhandler.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "BrsrStatusCodes.h"
#include "nw_hed_documentroot.h"
#include "HEDDocumentListener.h"

static
TBrowserStatusCode 
NW_XHTML_trElementHandle_CreateSubTree(NW_XHTML_ContentHandler_t* contentHandler,
                                       NW_DOM_ElementNode_t* elementNode,
                                       NW_LMgr_ContainerBox_t* trBox,
                                       NW_LMgr_ContainerBox_t* tableContainerParent)
{
  NW_DOM_ElementNode_t* childNode;

  NW_TRY (status) 
  {
    /* We visit the children ourselves, rather than calling the superclass.
     * This is necessary so we can filter any unwanted elements from
     * the table. A table row can only handle <td>s and <th>s. 
     */
    for (childNode = NW_DOM_Node_getFirstChild (elementNode);
         childNode != NULL ;
         childNode = NW_DOM_Node_getNextSibling (childNode)) 
    {
      const NW_XHTML_ElementHandler_t* elementHandler;

      /* get the ElementHandler for the elementNode */
      elementHandler =
        NW_XHTML_ContentHandler_GetElementHandler (contentHandler,
                                                   (NW_DOM_ElementNode_t*) childNode);

      /* we want to ignore the entire script tag */
      if (elementHandler == NULL)
        continue;

      if (NW_Object_IsInstanceOf(elementHandler, &NW_XHTML_cellElementHandler_Class))
      {
        status = NW_XHTML_ElementHandler_CreateBoxTree (elementHandler, contentHandler,
          childNode, trBox);
        _NW_THROW_ON_ERROR(status);
      }
      /* for handling forms embedded in tables */
      else if (NW_Object_IsInstanceOf(elementHandler, &NW_XHTML_formElementHandler_Class))
      {
        NW_XHTML_FormLiaison_t* formLiaison = (NW_XHTML_FormLiaison_t*)   
            NW_XHTML_ContentHandler_GetFormLiaison(contentHandler, childNode);

        NW_REQUIRED_PARAM(formLiaison);
        status = NW_XHTML_trElementHandle_CreateSubTree(contentHandler, childNode, trBox, tableContainerParent);
        _NW_THROW_ON_ERROR(status);
      }
      else if (NW_Object_IsInstanceOf(elementHandler, &NW_XHTML_inputElementHandler_Class))
      {
        status = NW_XHTML_ElementHandler_CreateBoxTree (elementHandler, contentHandler,
                                         childNode, tableContainerParent);
        _NW_THROW_ON_ERROR(status);
      }
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY

}


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_trElementHandler_Class_t NW_XHTML_trElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_trElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_trElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_trElementHandler_t NW_XHTML_trElementHandler = {
  { { &NW_XHTML_trElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_trElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t* rowBox = NULL;
  NW_LMgr_Box_t* tableContainer = (NW_LMgr_Box_t*)NW_LMgr_Box_GetParent(parentBox);
  NW_LMgr_ContainerBox_t* tableContainerParent = tableContainer->parent;

  NW_TRY (status) 
  {
    /* We only create a row box if the parent is a table */
    if (!NW_Object_IsClass(parentBox, &NW_LMgr_StaticTableBox_Class) &&
        !NW_Object_IsClass(parentBox, &NW_LMgr_VerticalTableBox_Class)) 
    {
      return NW_XHTML_ElementHandler_Class.NW_XHTML_ElementHandler.
        createBoxTree (elementHandler, contentHandler, elementNode, parentBox);
    }

    NW_HED_DocumentRoot_t* documentRoot = 
      (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(contentHandler);
    NW_ASSERT(documentRoot != NULL);

    NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox(documentRoot);
    NW_ASSERT(rootBox != NULL);

    if ((NW_XHTML_tableElementHandler_DoesGridModeApply(
        contentHandler, elementNode) == NW_TRUE) ||
        (NW_LMgr_RootBox_GetSmallScreenOn(rootBox) == NW_FALSE))
    {
      rowBox = (NW_LMgr_Box_t*)NW_LMgr_StaticTableRowBox_New(0);
    }
    else
    {
      rowBox = (NW_LMgr_Box_t*)NW_LMgr_VerticalTableRowBox_New(0);
    }
    NW_THROW_OOM_ON_NULL (rowBox, status);

    status = NW_LMgr_ContainerBox_AddChild (parentBox, rowBox);
    _NW_THROW_ON_ERROR(status);

    /* apply common attributes and styles */
    status = NW_XHTML_ElementHandler_ApplyStyles(elementHandler, contentHandler, elementNode, 
                                                 &rowBox, 0);
    NW_THROW_ON (status, KBrsrOutOfMemory);
    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (rowBox == NULL)
    {
      NW_THROW_SUCCESS (status);
    }

    parentBox = NW_LMgr_ContainerBoxOf(rowBox);

    status = NW_XHTML_trElementHandle_CreateSubTree(contentHandler, elementNode, parentBox, tableContainerParent);
    _NW_THROW_ON_ERROR(status);

  } 
  NW_CATCH (status) 
  {
    if (rowBox)
    {
      NW_Object_Delete (rowBox);
    }
  } 
  NW_FINALLY 
  {
    return status;
  } NW_END_TRY
}

