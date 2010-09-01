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

#include "nw_link_linkelementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include <nw_dom_document.h>
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nwx_string.h"
#include "BrsrStatusCodes.h"

/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_linkElementHandler_Class_t NW_XHTML_linkElementHandler_Class = 
{
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_linkElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_ElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_linkElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_linkElementHandler_t NW_XHTML_linkElementHandler = 
{
  { { &NW_XHTML_linkElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_linkElementHandler_Initialize(const NW_XHTML_ElementHandler_t* elementHandler,
    NW_XHTML_ContentHandler_t* contentHandler, NW_DOM_ElementNode_t* elementNode)
  {
  NW_Ucs2* type = NULL;
  NW_Ucs2* relation = NULL;
  NW_Ucs2* media = NULL;
  NW_Ucs2* href = NULL;
  NW_Ucs2* charset = NULL;

  NW_REQUIRED_PARAM(elementHandler);

  NW_TRY (status) 
    {
    // Extract the type attribute.
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
        elementNode, NW_XHTML_AttributeToken_type, &type);
    NW_THROW_ON(status, KBrsrOutOfMemory);
    
    // Extract the relation attribute.
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
        elementNode, NW_XHTML_AttributeToken_rel, &relation);
    NW_THROW_ON(status, KBrsrOutOfMemory);
    
    // Extract the media attribute.
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
        elementNode, NW_XHTML_AttributeToken_media, &media);
    NW_THROW_ON(status, KBrsrOutOfMemory);
    
    // Extract the href attribute.
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
        elementNode, NW_XHTML_AttributeToken_href, &href);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Extract the charset attribute.
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
        elementNode, NW_XHTML_AttributeToken_charset, &charset);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Load the stylesheet.
    status = NW_XHTML_ContentHandler_LoadStylesheet(contentHandler,
        type, relation, media, href, charset, NULL);
    NW_THROW_ON(status, KBrsrOutOfMemory);
    }

  NW_CATCH (status) 
    {
    }

  NW_FINALLY 
    {
    NW_Str_Delete(type);
    NW_Str_Delete(relation);
    NW_Str_Delete(media);
    NW_Str_Delete(href);
    NW_Str_Delete(charset);

    return status;
    } NW_END_TRY
  }
