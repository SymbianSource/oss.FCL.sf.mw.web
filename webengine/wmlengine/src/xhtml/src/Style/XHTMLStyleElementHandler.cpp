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


#include "nw_style_styleelementhandleri.h"

#include "nw_link_linkelementhandleri.h"

#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_dom_text.h"
#include "nw_dom_document.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_styleElementHandler_Class_t NW_XHTML_styleElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_styleElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_styleElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_StyleElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_styleElementHandler_t NW_XHTML_styleElementHandler = {
  { { &NW_XHTML_styleElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_styleElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode)
  {
  NW_Uint32              encoding;
  NW_CSS_Processor_t*    processor;
  NW_Uint32              length;
  NW_String_t            str;
  NW_Bool                doNotDelete = NW_TRUE;
  NW_Bool                deleteBuffer = NW_TRUE;;
  NW_DOM_ElementNode_t*  firstChild = NULL;
  NW_Ucs2*               media = NULL;

  NW_REQUIRED_PARAM(elementHandler);

  NW_TRY (status)
    {
    // Get the media attribute.
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
        elementNode, NW_XHTML_AttributeToken_media, &media);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    if (media != NULL) 
      {
      NW_Bool  allowable;
      NW_Bool  isHandheld;

      // Ignore if this isn't an allowable style.
      allowable = NW_XHTML_ContentHandler_AllowableStylesheet(media, &isHandheld);
      if (!allowable) 
        {
        NW_THROW_SUCCESS(status);
        }

      // If a "handheld" stylesheet is found switch to mobile profile mode.
      if (isHandheld) 
        {
        NW_XHTML_ContentHandler_EnableMobileProfileMode(contentHandler);
        }
      }

    // Get the element's body.
    firstChild = NW_DOM_Node_getFirstChild(elementNode);
    NW_THROW_ON_NULL(firstChild, status, KBrsrUnexpectedError);

    status = NW_HED_DomHelper_GetText (&(contentHandler->domHelper), 
        firstChild, NW_TRUE, &str, &encoding);
    _NW_THROW_ON_ERROR(status);

    // Process the CSS
    processor = NW_XHTML_ContentHandler_CreateCSSProcessor(contentHandler);
    NW_THROW_ON_NULL(processor, status, KBrsrUnexpectedError);

    length = NW_String_getByteCount(&str);
    if (NW_String_getUserOwnsStorage(&str) == NW_TRUE)
      {
      doNotDelete = NW_FALSE;
      }

    status = NW_CSS_Processor_ProcessBuffer(processor, 
        (NW_Byte*)(str.storage), length, encoding, doNotDelete);
    _NW_THROW_ON_ERROR(status);

    deleteBuffer = NW_FALSE;
    }

  NW_CATCH (status) 
    {
    }

  NW_FINALLY 
    {
    if (deleteBuffer == NW_TRUE)
      {
      NW_String_deleteStorage(&str);
      }

    NW_Str_Delete(media);

    return status;
    } NW_END_TRY
  }
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_styleElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
  {
  NW_REQUIRED_PARAM(elementHandler);
  NW_REQUIRED_PARAM(contentHandler);
  NW_REQUIRED_PARAM(elementNode);
  NW_REQUIRED_PARAM(parentBox);


  return KBrsrSuccess;
  }

