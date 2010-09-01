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


#include "nw_basictables_basictablesmodulei.h"

#include "nw_basictables_captionelementhandler.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "nw_basictables_cellelementhandler.h"
#include "nw_basictables_trelementhandler.h"
#include "nw_basictables_rowgroupelementhandler.h"

/* ------------------------------------------------------------------------- */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_BasicTablesModule_ElementHandlers[] = {
  { NW_XHTML_ElementToken_caption, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_captionElementHandler },
  { NW_XHTML_ElementToken_table, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_tableElementHandler },
  { NW_XHTML_ElementToken_td, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_cellElementHandler },
  { NW_XHTML_ElementToken_th, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_cellElementHandler },
  { NW_XHTML_ElementToken_tr, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_trElementHandler },
  { NW_XHTML_ElementToken_tbody, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_rowgroupElementHandler },
  { NW_XHTML_ElementToken_thead, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_rowgroupElementHandler },
  { NW_XHTML_ElementToken_tfoot, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_rowgroupElementHandler }
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_BasicTablesModule_Class_t NW_XHTML_BasicTablesModule_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	              */
    /* getElementHandler              */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_Markup_WBXMLModule	      */
    /* numElementHandlers             */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_BasicTables),
    /* elementHandlers                */ _NW_XHTML_BasicTablesModule_ElementHandlers,
  },
  { /* NW_XHTML_BasicTablesModule     */
    /* unused                         */ 0
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_BasicTablesModule_t NW_XHTML_BasicTablesModule = {
	{&NW_XHTML_BasicTablesModule_Class},
};
