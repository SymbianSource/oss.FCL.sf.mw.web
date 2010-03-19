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


#include "nw_markup_wbxmlmodulei.h"
#include "nw_dom_element.h"
#include "nw_hed_domhelper.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Markup_WBXMLModule_Class_t NW_Markup_WBXMLModule_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Markup_Module_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	         */
    /* getElementHandler         */ _NW_Markup_WBXMLModule_GetElementHandler
  },
  { /* NW_Markup_WBXMLModule	 */
    /* numElementHandlers        */ 0,
    /* elementHandlers           */ NULL,
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Markup_ElementHandler_t*
_NW_Markup_WBXMLModule_GetElementHandler (const NW_Markup_Module_t* module,
                                         const NW_DOM_ElementNode_t* domElement)
{
  NW_Uint32 numElementHandlers;
  const NW_Markup_WBXMLModule_ElementHandlerEntry_t* elementHandlers;
  NW_Uint16 tagToken;
  NW_Uint32 index;

  /* traverse our list of ElementHandlers looking for a match for our token */
  tagToken = NW_HED_DomHelper_GetElementToken (domElement);
  numElementHandlers = NW_Markup_WBXMLModule_GetClassPart (module).numElementHandlers;
  elementHandlers = NW_Markup_WBXMLModule_GetClassPart (module).elementHandlers;
  for (index = 0; index < numElementHandlers; index++) {
    if (elementHandlers[index].tagToken == tagToken) {
      return elementHandlers[index].elementHandler;
    }
  }

  /* if no element handler is found we must return NULL */
  return NULL;
}

/* ------------------------------------------------------------------------- */
NW_Markup_ElementHandler_t*
NW_Markup_WBXMLModule_GetHandler (const NW_Markup_Module_t* module,
                                         NW_Uint16 tagToken)
{
  const NW_Markup_WBXMLModule_ElementHandlerEntry_t* elementHandlers;

  // traverse our list of ElementHandlers looking for a match for our token 
  NW_Uint32 numElementHandlers = NW_Markup_WBXMLModule_GetClassPart (module).numElementHandlers;
  elementHandlers = NW_Markup_WBXMLModule_GetClassPart (module).elementHandlers;
  NW_Uint16 moduleStartToken = elementHandlers[0].tagToken;
  NW_Uint16 moduleEndToken = (NW_Uint16)(moduleStartToken + numElementHandlers);
  // if the token is within the range, getthe element handler
  if ((tagToken >= moduleStartToken) && (tagToken < moduleEndToken))
      {
        // get the index
      NW_Uint16 index = (NW_Uint16)(tagToken - moduleStartToken);
      return elementHandlers[index].elementHandler;
      }

  /* if no element handler is found we must return NULL */
  return NULL;
}
