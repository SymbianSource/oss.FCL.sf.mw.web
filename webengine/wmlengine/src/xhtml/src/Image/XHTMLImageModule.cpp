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


#include "nw_image_imagemodulei.h"

#include "nw_image_imgelementhandler.h"
#include "nw_image_mapelementhandler.h"
#include "nw_image_areaelementhandler.h"

/* ------------------------------------------------------------------------- */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_ImageModule_ElementHandlers[] = {
  { NW_XHTML_ElementToken_img, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_imgElementHandler },
  { NW_XHTML_ElementToken_map, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_mapElementHandler },
  { NW_XHTML_ElementToken_area,(struct NW_Markup_ElementHandler_s*) &NW_XHTML_areaElementHandler}
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_ImageModule_Class_t NW_XHTML_ImageModule_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	              */
    /* getElementHandler              */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_Markup_WBXMLModule	      */
    /* numElementHandlers             */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_Image),
    /* elementHandlers                */ _NW_XHTML_ImageModule_ElementHandlers,
  },
  { /* NW_XHTML_ImageModule           */
    /* unused                         */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_ImageModule_t NW_XHTML_ImageModule = {
	{&NW_XHTML_ImageModule_Class},
};

NW_Bool NW_XHTML_ImageModule_IsImageElementHandler(const NW_XHTML_ElementHandler_t* elementHandler)
    {
    return (NW_Object_IsClass(elementHandler , &NW_XHTML_imgElementHandler_Class) ||
            NW_Object_IsClass(elementHandler , &NW_XHTML_mapElementHandler_Class) ||
            NW_Object_IsClass(elementHandler , &NW_XHTML_areaElementHandler_Class));
    }
