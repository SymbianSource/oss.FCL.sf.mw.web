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


#include "nw_basicforms_basicformsmodulei.h"

#include "nw_basicforms_formelementhandler.h"
#include "nw_basicforms_inputelementhandler.h"
#include "nw_basicforms_labelelementhandler.h"
#include "nw_basicforms_optionelementhandler.h"
#include "nw_basicforms_selectelementhandler.h"
#include "nw_basicforms_textareaelementhandler.h"

/* ------------------------------------------------------------------------- */
static const
NW_Markup_WBXMLModule_ElementHandlerEntry_t _NW_XHTML_BasicFormsModule_ElementHandlers[] = {
  { NW_XHTML_ElementToken_form, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_formElementHandler},
  { NW_XHTML_ElementToken_input, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_inputElementHandler},
  { NW_XHTML_ElementToken_label, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_labelElementHandler},
  { NW_XHTML_ElementToken_select, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_selectElementHandler},
  { NW_XHTML_ElementToken_option, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_optionElementHandler},
  { NW_XHTML_ElementToken_optgroup, NULL},
  { NW_XHTML_ElementToken_button, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_inputElementHandler},
  { NW_XHTML_ElementToken_textarea, (struct NW_Markup_ElementHandler_s*) &NW_XHTML_textareaElementHandler},
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_BasicFormsModule_Class_t NW_XHTML_BasicFormsModule_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Markup_WBXMLModule_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	         */
    /* getElementHandler         */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_Markup_WBXMLModule	 */
    /* numElementHandlers        */ NW_MARKUP_NUM_ELEMENT_HANDLERS (XHTML_BasicForms),
    /* elementHandlers           */ _NW_XHTML_BasicFormsModule_ElementHandlers,
  },
  { /* NW_XHTML_BasicFormsModule */
    /* unused                    */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_XHTML_BasicFormsModule_t NW_XHTML_BasicFormsModule = {
	{&NW_XHTML_BasicFormsModule_Class},
};
