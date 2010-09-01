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


#include "nw_text_textmodulei.h"


#include "nw_text_brelementhandler.h"
#include "nw_text_hrelementhandler.h"
#include "nw_text_qelementhandler.h"
#include "nw_text_coretextelementhandler.h"

/* The center and font elements are specific to i-Mode HTML */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_TextModule_ElementHandlers[] = {
  { NW_XHTML_ElementToken_abbr, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* abbr */},
  { NW_XHTML_ElementToken_acronym, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* acronym */},
  { NW_XHTML_ElementToken_address, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* address */},
  { NW_XHTML_ElementToken_blockquote, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* blockquote */},
  { NW_XHTML_ElementToken_br, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_brElementHandler /* br */ },
  { NW_XHTML_ElementToken_blink, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* blink */},
  { NW_XHTML_ElementToken_center, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* center */},
  { NW_XHTML_ElementToken_cite, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* cite */},
  { NW_XHTML_ElementToken_code, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* code */},
  { NW_XHTML_ElementToken_dfn, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dfn */},
  { NW_XHTML_ElementToken_dir, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dir */},
  { NW_XHTML_ElementToken_div, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* div */},
  { NW_XHTML_ElementToken_em, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* em */},
  { NW_XHTML_ElementToken_font, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* font */},
  { NW_XHTML_ElementToken_h1, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h1 */},
  { NW_XHTML_ElementToken_h2, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h2 */},
  { NW_XHTML_ElementToken_h3, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h3 */},
  { NW_XHTML_ElementToken_h4, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h4 */},
  { NW_XHTML_ElementToken_h5, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h5 */},
  { NW_XHTML_ElementToken_h6, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* h6 */},
  { NW_XHTML_ElementToken_hr, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_hrElementHandler /* hr */},
  { NW_XHTML_ElementToken_b,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* b */},
  { NW_XHTML_ElementToken_big,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* big */},
  { NW_XHTML_ElementToken_i,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* i */},
  { NW_XHTML_ElementToken_menu, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* menu */},
  { NW_XHTML_ElementToken_nobr, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* nobr */},
  { NW_XHTML_ElementToken_noscript, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* noscript */},
  { NW_XHTML_ElementToken_small,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* small */},
  { NW_XHTML_ElementToken_script, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* script */},
  { NW_XHTML_ElementToken_u,  (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* u */},
  { NW_XHTML_ElementToken_kbd, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* kbd */},
  { NW_XHTML_ElementToken_p, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* p */},
  { NW_XHTML_ElementToken_pre, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* pre */},
  { NW_XHTML_ElementToken_q, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_qElementHandler /* q */},
  { NW_XHTML_ElementToken_samp, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* samp */},
  { NW_XHTML_ElementToken_span, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* span */},
  { NW_XHTML_ElementToken_strong, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* strong */},
  { NW_XHTML_ElementToken_var, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* var */},
  { NW_XHTML_ElementToken_dl, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dl */},
  { NW_XHTML_ElementToken_dt, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dt */},
  { NW_XHTML_ElementToken_dd, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* dd */},
  { NW_XHTML_ElementToken_ul, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* ul */},
  { NW_XHTML_ElementToken_ol, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* ol */},
  { NW_XHTML_ElementToken_li, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* li */},
  { NW_XHTML_ElementToken_fieldset, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* fieldset*/},
  { NW_XHTML_ElementToken_marquee, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* marquee */},
  { NW_XHTML_ElementToken_tt, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* tt */},
  { NW_XHTML_ElementToken_sub, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* sub */},
  { NW_XHTML_ElementToken_sup, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* sup */},
  { NW_XHTML_ElementToken_ins, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* ins */},
  { NW_XHTML_ElementToken_del, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* del */},
  { NW_XHTML_ElementToken_strike, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* strike */},
  { NW_XHTML_ElementToken_s, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_CoreTextElementHandler /* s */},
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_TextModule_Class_t NW_XHTML_TextModule_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_XHTML_Module	              */
    /* getElementHandler              */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_XHTML_WBXMLModule	      */
    /* numElementHandlers             */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_Text),
    /* elementHandlers                */ _NW_XHTML_TextModule_ElementHandlers,
  },
  { /* NW_XHTML_TextModule            */
    /* unused                         */ 0
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_TextModule_t NW_XHTML_TextModule = {
  {&NW_XHTML_TextModule_Class},
};
