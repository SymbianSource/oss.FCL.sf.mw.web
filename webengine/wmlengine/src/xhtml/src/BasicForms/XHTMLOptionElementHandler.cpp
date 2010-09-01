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


#include "nw_basicforms_optionelementhandleri.h"
#include "nw_fbox_optionbox.h"
#include "nw_fbox_selectbox.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "nw_xhtml_textelementhandler.h"
#include "nw_text_brelementhandler.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"


/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_optionElementHandler_Class_t NW_XHTML_optionElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_optionElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_ElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_optionElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_optionElementHandler_t NW_XHTML_optionElementHandler = {
  { { &NW_XHTML_optionElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_optionElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode)
{
  NW_FBox_FormLiaison_t* formLiaison;
  
  NW_REQUIRED_PARAM(elementHandler);

  formLiaison = 
    NW_XHTML_ContentHandler_GetFormLiaison(contentHandler, elementNode);

  if (formLiaison == NULL){
    return KBrsrSuccess;
  }

  (void) NW_FBox_FormLiaison_AddControl(formLiaison, elementNode);

  return KBrsrSuccess;
  
}

/* ------------------------------------------------------------------------- */

/*CreateBoxTree is not overloaded for option element as it is taken care of in 
  select element createBoxTree - vk 09/18/2001
 */

