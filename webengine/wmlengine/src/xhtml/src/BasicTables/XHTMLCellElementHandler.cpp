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


#include "nw_basictables_cellelementhandleri.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_lmgr_statictablerowbox.h"
#include "nw_lmgr_verticaltablecellbox.h"
#include "nw_lmgr_verticaltablerowbox.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_string.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nwx_settings.h"
#include "BrsrStatusCodes.h"
#include "nw_hed_documentroot.h"
#include "HEDDocumentListener.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_cellElementHandler_Class_t NW_XHTML_cellElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_cellElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_cellElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_cellElementHandler_t NW_XHTML_cellElementHandler = {
  { { &NW_XHTML_cellElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_cellElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t* cellBox = NULL;

  NW_TRY (status) 
  {  
    /* We only create a cell box if the parent is a row */
    if (NW_Object_IsClass(parentBox, &NW_LMgr_StaticTableRowBox_Class) ||
        NW_Object_IsClass(parentBox, &NW_LMgr_VerticalTableRowBox_Class)) 
    {
      /* Create the cell box */
      NW_HED_DocumentRoot_t* documentRoot = 
        (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(contentHandler);
      NW_ASSERT(documentRoot != NULL);

      NW_LMgr_RootBox_t* rootBox = NW_HED_DocumentRoot_GetRootBox(documentRoot);
      NW_ASSERT(rootBox != NULL);

      if ((NW_XHTML_tableElementHandler_DoesGridModeApply(
          contentHandler, elementNode) == NW_TRUE) ||
          (NW_LMgr_RootBox_GetSmallScreenOn(rootBox) == NW_FALSE))
      {
        cellBox = (NW_LMgr_Box_t*) NW_LMgr_StaticTableCellBox_New(0);
      }
      else
      {
        cellBox = (NW_LMgr_Box_t*) NW_LMgr_VerticalTableCellBox_New(0);
      }
      NW_THROW_OOM_ON_NULL (cellBox, status);

      /* add it to the boxtree so that relative styles may be resolved */
      status = NW_LMgr_ContainerBox_AddChild (parentBox, cellBox);
      _NW_THROW_ON_ERROR(status);

      /* apply common attributes and styles */
      status = NW_XHTML_ElementHandler_ApplyStyles(elementHandler, 
                                                   contentHandler, 
                                                   elementNode, 
                                                   &cellBox, 
                                                   NULL /* eventClass */);
      NW_THROW_ON (status, KBrsrOutOfMemory);
      // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
      // In case of 'display:none' or an error it removes the container box. That's why we check box
      if (cellBox == NULL)
      {
        NW_THROW_SUCCESS (status);
      }
      parentBox = NW_LMgr_ContainerBoxOf(cellBox);
    }

    /* invoke our superclass for completion */
    status = NW_XHTML_ElementHandler_Class.NW_XHTML_ElementHandler.
      createBoxTree (elementHandler, contentHandler, elementNode, parentBox);

    cellBox = NULL;

  } NW_CATCH (status) 
  {
    NW_Object_Delete (cellBox);
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}

