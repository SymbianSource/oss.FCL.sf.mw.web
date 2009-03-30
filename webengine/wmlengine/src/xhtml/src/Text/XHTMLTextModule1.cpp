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


#include "nw_text_textmodule1i.h"


#include "nw_text_brelementhandler.h"
#include "nw_text_hrelementhandler.h"
#include "nw_text_qelementhandler.h"
#include "nw_text_coretextelementhandler.h"

/* The center and font elements are specific to i-Mode HTML */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_Text1Module_ElementHandlers[] = {
  { NW_XHTML_ElementToken_div, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* div */},
  { NW_XHTML_ElementToken_font, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* font */},
  { NW_XHTML_ElementToken_h1, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h1 */},
  { NW_XHTML_ElementToken_h2, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h2 */},
  { NW_XHTML_ElementToken_h3, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h3 */},
  { NW_XHTML_ElementToken_h4, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h4 */},
  { NW_XHTML_ElementToken_h5, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h5 */},
  { NW_XHTML_ElementToken_hr, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_hrElementHandler /* hr */},
  { NW_XHTML_ElementToken_b,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* b */},
  { NW_XHTML_ElementToken_big,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* big */},
  { NW_XHTML_ElementToken_i,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* i */},
  { NW_XHTML_ElementToken_menu, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* menu */},
  { NW_XHTML_ElementToken_span, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* span */},
  { NW_XHTML_ElementToken_p, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* p */},
  { NW_XHTML_ElementToken_center, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* center */},
  { NW_XHTML_ElementToken_dl, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dl */},
  { NW_XHTML_ElementToken_dt, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dt */},
  { NW_XHTML_ElementToken_dd, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dd */},
  { NW_XHTML_ElementToken_ul, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* ul */},
  { NW_XHTML_ElementToken_ol, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* ol */},
  { NW_XHTML_ElementToken_li, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* li */},
  { NW_XHTML_ElementToken_br, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_brElementHandler /* br */ },
  { NW_XHTML_ElementToken_nobr, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* nobr */}
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_TextModule1_Class_t NW_XHTML_TextModule1_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_XHTML_Module	              */
    /* getElementHandler              */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_XHTML_WBXMLModule	      */
    /* numElementHandlers             */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_Text1),
    /* elementHandlers                */ _NW_XHTML_Text1Module_ElementHandlers,
  },
  { /* NW_XHTML_TextModule            */
    /* unused                         */ 0
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_TextModule1_t NW_XHTML_TextModule1 = {
  {&NW_XHTML_TextModule1_Class},
};
