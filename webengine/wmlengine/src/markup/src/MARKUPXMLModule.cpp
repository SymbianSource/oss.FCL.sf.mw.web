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

/*lint --e{767} macro was defined differently in another module */

#include "nw_markup_xmlmodulei.h"
#include <nw_dom_element.h>

#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Markup_XMLModule_Class_t NW_Markup_XMLModule_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Markup_Module_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	         */
    /* getElementHandler         */ _NW_Markup_XMLModule_GetElementHandler
  },
  { /* NW_Markup_XMLModule	 */
    /* numElementHandlers        */ 0,
    /* elementHandlers           */ NULL,
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Markup_ElementHandler_t*
_NW_Markup_XMLModule_GetElementHandler (const NW_Markup_Module_t* module,
                                       const NW_DOM_ElementNode_t* domElement)
{
  NW_String_t tagName;
  TBrowserStatusCode status;
  NW_Uint32 numElementHandlers;
  const NW_Markup_XMLModule_ElementHandlerEntry_t* elementHandlers;
  NW_Uint32 index;

  /* get the tagName and build */
  /* TODO: deal with other character encodings than UCS2 */
  (void) NW_Mem_memset (&tagName, 0, sizeof (tagName));
  status = NW_DOM_ElementNode_getTagName ((NW_DOM_ElementNode_t*) domElement,
                                          &tagName);
  if (status != KBrsrSuccess) {
    return NULL;
  }

  /* traverse our list of ElementHandlers looking for a match for our token */
  numElementHandlers = NW_Markup_XMLModule_GetClassPart (module).numElementHandlers;
  elementHandlers = NW_Markup_XMLModule_GetClassPart (module).elementHandlers;
  for (index = 0; index < numElementHandlers; index++) {
    if (!NW_Mem_memcmp (elementHandlers[index].tagName, tagName.storage,
                        tagName.length) &&
        elementHandlers[index].tagName[tagName.length / 2 - 1] == '\0') {
      NW_String_deleteStorage(&tagName);
      return elementHandlers[index].elementHandler;
    }
  }

  /* if no element handler is found we must return NULL */
  NW_String_deleteStorage(&tagName);
  return NULL;
}
