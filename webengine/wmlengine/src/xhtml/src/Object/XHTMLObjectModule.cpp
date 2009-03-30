/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nw_object_objectmodulei.h"
#include "XhtmlObjectElementHandlerOOC.h"

/* ------------------------------------------------------------------------- */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_ObjectModule_ElementHandlers[] = {
  { NW_XHTML_ElementToken_object, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_ObjectElementHandler },
  { NW_XHTML_ElementToken_embed, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_ObjectElementHandler }
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_ObjectModule_Class_t NW_XHTML_ObjectModule_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	              */
    /* getElementHandler              */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_Markup_WBXMLModule	      */
    /* numElementHandlers             */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_Object),
    /* elementHandlers                */ _NW_XHTML_ObjectModule_ElementHandlers,
  },
  { /* NW_XHTML_ObjectModule           */
    /* unused                         */ NW_Object_Unused //?
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_ObjectModule_t NW_XHTML_ObjectModule = {
	{&NW_XHTML_ObjectModule_Class},
};
