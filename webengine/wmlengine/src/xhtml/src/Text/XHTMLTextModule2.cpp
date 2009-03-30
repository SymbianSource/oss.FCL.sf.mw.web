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


#include "nw_text_textmodule2.h"


#include "nw_text_brelementhandler.h"
#include "nw_text_hrelementhandler.h"
#include "nw_text_qelementhandler.h"
#include "nw_text_coretextelementhandler.h"

/* The center and font elements are specific to i-Mode HTML */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_Text2Module_ElementHandlers[] = {
  { NW_XHTML_ElementToken_h6, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h6 */},
  { NW_XHTML_ElementToken_em, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* em */},
  { NW_XHTML_ElementToken_fieldset, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* fieldset*/},
  { NW_XHTML_ElementToken_marquee, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* marquee */},
  { NW_XHTML_ElementToken_dir, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dir */},
  { NW_XHTML_ElementToken_small,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* small */},
  { NW_XHTML_ElementToken_plaintext, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* plaintext */},
  { NW_XHTML_ElementToken_pre, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* pre */},
  { NW_XHTML_ElementToken_abbr, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* abbr */},
  { NW_XHTML_ElementToken_acronym, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* acronym */},
  { NW_XHTML_ElementToken_address, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* address */},
  { NW_XHTML_ElementToken_blockquote, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* blockquote */},
  { NW_XHTML_ElementToken_blink, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* blink */},
  { NW_XHTML_ElementToken_cite, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* cite */},
  { NW_XHTML_ElementToken_code, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* code */},
  { NW_XHTML_ElementToken_dfn, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dfn */},
  { NW_XHTML_ElementToken_u,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* u */},
  { NW_XHTML_ElementToken_kbd, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* kbd */},
  { NW_XHTML_ElementToken_q, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_qElementHandler /* q */},
  { NW_XHTML_ElementToken_samp, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* samp */},
  { NW_XHTML_ElementToken_strong, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* strong */},
  { NW_XHTML_ElementToken_var, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* var */},
  { NW_XHTML_ElementToken_tt, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* tt */},
  { NW_XHTML_ElementToken_sub, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* sub */},
  { NW_XHTML_ElementToken_sup, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* sup */},
  { NW_XHTML_ElementToken_ins, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* ins */},
  { NW_XHTML_ElementToken_del, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* del */},
  { NW_XHTML_ElementToken_strike, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* strike */},
  { NW_XHTML_ElementToken_s, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* s */}
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_TextModule2_Class_t NW_XHTML_TextModule2_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_XHTML_Module	              */
    /* getElementHandler              */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_XHTML_WBXMLModule	          */
    /* numElementHandlers             */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_Text2),
    /* elementHandlers                */ _NW_XHTML_Text2Module_ElementHandlers,
  },
  { /* NW_XHTML_TextModule            */
    /* unused                         */ 0
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_TextModule2_t NW_XHTML_TextModule2 = {
  {&NW_XHTML_TextModule2_Class},
};
