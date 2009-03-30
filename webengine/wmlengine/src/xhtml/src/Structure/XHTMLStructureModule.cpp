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


#include "nw_structure_structuremodulei.h"

#include "nw_structure_headelementhandler.h"
#include "nw_structure_titleelementhandler.h"
#include "nw_text_coretextelementhandler.h"
#include "nw_frame_framesetelementhandler.h"
#include "nw_ignore_ignoreelementhandler.h"
#include "nw_structure_scriptelementhandler.h"
#include "nw_structure_noscriptelementhandler.h"

/* ------------------------------------------------------------------------- */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_StructureModule_ElementHandlers[] = {
  { NW_XHTML_ElementToken_body, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler },
  { NW_XHTML_ElementToken_head, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_headElementHandler },
  { NW_XHTML_ElementToken_html, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler },
  { NW_XHTML_ElementToken_title, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_titleElementHandler },
  { NW_XHTML_ElementToken_frameset, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_FramesetElementHandler },
  { NW_XHTML_ElementToken_noframes, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_IgnoreElementHandler },
  { NW_XHTML_ElementToken_noembed,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_IgnoreElementHandler },
  { NW_XHTML_ElementToken_noscript, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_IgnoreElementHandler }
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_StructureModule_Class_t NW_XHTML_StructureModule_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	              */
    /* getElementHandler              */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_Markup_WBXMLModule	      */
    /* numElementHandlers             */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_Structure),
    /* elementHandlers                */ _NW_XHTML_StructureModule_ElementHandlers,
  },
  { /* NW_XHTML_StructureModule       */
    /* unused                         */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_StructureModule_t NW_XHTML_StructureModule = {
	{&NW_XHTML_StructureModule_Class},
};
