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


#include "nw_frame_framemodulei.h"
#include "nw_frame_frameelementhandler.h"

/* ------------------------------------------------------------------------- */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_FrameModule_ElementHandlers[] = {
  { NW_XHTML_ElementToken_frame, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_frameElementHandler }
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_FrameModule_Class_t NW_XHTML_FrameModule_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	              */
    /* getElementHandler              */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_Markup_WBXMLModule	        */
    /* numElementHandlers             */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_Frame),
    /* elementHandlers                */ _NW_XHTML_FrameModule_ElementHandlers,
  },
  { /* NW_XHTML_FrameModule           */
    /* unused                         */ 0
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_FrameModule_t NW_XHTML_FrameModule = {
	{&NW_XHTML_FrameModule_Class},
};
