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


#include "nw_structure_noscriptelementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nwx_settings.h"

#include "BrsrStatusCodes.h"


/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_noscriptElementHandler_Class_t NW_XHTML_noscriptElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_noscriptElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_ElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_noscriptElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_noscriptElementHandler_t NW_XHTML_noscriptElementHandler = {
  { { &NW_XHTML_noscriptElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_noscriptElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode)
{
  
  NW_REQUIRED_PARAM(elementHandler);

  /* if the the prev script element is supported  , the <noscript> elements and all its
    children should not be initialized
    */  
  if(NW_Settings_GetEcmaScriptEnabled() && contentHandler->ignoreNoScriptElement)
    {
    contentHandler->skipToNode = elementNode;
    }
  return KBrsrSuccess;
  
}

/* ------------------------------------------------------------------------- */



