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

#include "nw_xhtml_pielementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include <nw_dom_document.h>
#include "nw_htmlp_to_wbxml.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nwx_string.h"
#include "nw_link_linkelementhandler.h"
#include <nwx_status.h>
#include "nwx_settings.h"
#include "nwx_statuscodeconvert.h"
#include "BrsrStatusCodes.h"
#include "nwx_http_defs.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

static
void NW_XHTML_PiElementHandler_Doctype(NW_Ucs2* pAttrUcs2, NW_XHTML_ContentHandler_t* contentHandler)
{
  const NW_Ucs2 mobileProfileStr[] = { '/', '/', 'D', 'T', 'D', ' ', 'X', 'H', 'T', 'M', 'L', ' ', 'M', 'o', 'b', 'i', 'l', 'e', '\0'};
  if (NW_Str_Strstr(pAttrUcs2, mobileProfileStr)) {
    NW_XHTML_ContentHandler_EnableMobileProfileMode(contentHandler);
  }
}

static
TBrowserStatusCode
NW_XHTML_PiElementHandler_zzzunknown(const NW_XHTML_ElementHandler_t* elementHandler,
                                     NW_XHTML_ContentHandler_t* contentHandler,
                                     NW_DOM_ElementNode_t* node)
{
  NW_Ucs2* pAttrUcs2 = NULL;  /* NULL required, unconditional free */
  NW_Bool freeNeeded = NW_FALSE; /* NW_FALSE required by free logic */
  NW_Text_Length_t charCount;
  NW_Text_t* piData = NULL;
  const NW_Ucs2 htmlStr[] = {'h', 't', 'm', 'l'};
  NW_Uint32 htmlStrLength = 4;
  NW_Uint16 piTargetToken =
    NW_DOM_ProcessingInstructionNode_getTargetToken (node);

  NW_REQUIRED_PARAM(elementHandler);

  NW_TRY(status) {
    NW_XHTML_GetDOMAttribute(contentHandler, node, piTargetToken, &piData);
    NW_THROW_ON_NULL(piData, status, KBrsrSuccess);
    charCount = NW_Text_GetCharCount(piData);
    pAttrUcs2 = NW_Text_GetUCS2Buffer(piData, NW_Text_Flags_NullTerminated, &charCount, &freeNeeded);
    NW_THROW_ON_NULL(pAttrUcs2, status, KBrsrOutOfMemory);
    if (NW_Str_Strncmp(htmlStr, pAttrUcs2, htmlStrLength) ==0) {
      NW_XHTML_PiElementHandler_Doctype(pAttrUcs2, contentHandler);
    }
  }
  NW_CATCH(status) {
  }
  NW_FINALLY {
    NW_Object_Delete(piData);
    if (freeNeeded) {
      NW_Mem_Free(pAttrUcs2);
    }
    return status;
  }
  NW_END_TRY
}

static
TBrowserStatusCode
NW_XHTML_PiElementHandler_MakeDom(NW_Text_t* pPiData,
                                  NW_HED_DomTree_t** ppDomTree)
{
  NW_Ucs2* pAttrUcs2 = NULL;  /* NULL required, unconditional free */
  NW_Uint8* pBuf = NULL;  /* NULL required, unconditional free */
  NW_Buffer_t* pWbxmlBuffer = NULL; /* NULL required, unconditional free */
  NW_Uint32 sizeStart;
  NW_Uint32 sizeEnd;
  NW_Uint32 totalsize;
  NW_Uint32 line;
  NW_Bool freeNeeded = NW_FALSE; /* NW_FALSE required by free logic */

  NW_ASSERT(pPiData != NULL);
  *ppDomTree = NULL;

  /* Create an HTML document, parse and build a DOM tree. */
  NW_TRY(status) {
    /* The HTML document will have one element and be in UCS-2.
       The hr element is used just because it has a short name,
       normally has attributes, and is empty. */
    const NW_Ucs2 htmlElementStart[] = {'<', 'h', 'r', ' ',};
    const NW_Ucs2 htmlElementEnd[] = {'/', '>',};
    /* convert the attribute string to UCS-2 */
    NW_Text_Length_t charCount = NW_Text_GetCharCount(pPiData);
    if (charCount == 0) {
      NW_THROW_UNEXPECTED(status);
    }
    pAttrUcs2 = NW_Text_GetUCS2Buffer(pPiData, 0, &charCount, &freeNeeded);
    NW_THROW_ON_NULL(pAttrUcs2, status, KBrsrUnexpectedError);
    /* put it all together in one buffer */
    sizeStart = sizeof(htmlElementStart);
    sizeEnd = sizeof(htmlElementEnd);
    totalsize = (sizeStart + (sizeof(NW_Ucs2) * charCount) + sizeEnd);
    pBuf = (NW_Uint8*) NW_Mem_Malloc(totalsize);
    NW_THROW_OOM_ON_NULL(pBuf, status);
    NW_Mem_memcpy(pBuf,
                  htmlElementStart, sizeStart);
    NW_Mem_memcpy(pBuf + sizeStart,
                  pAttrUcs2, sizeof(NW_Ucs2) * charCount);
    NW_Mem_memcpy(pBuf + sizeStart + sizeof(NW_Ucs2) * charCount,
                  htmlElementEnd, sizeEnd);
    /* flip the byte order to network byte order (big-endian), required by cXML */
    NW_XHTML_ElementHandler_Ucs2NativeToNetworkByteOrder(totalsize, pBuf);
    /* parse the document */
    {
      NW_Buffer_t docBuf;
      docBuf.length = totalsize;
      docBuf.allocatedLength = totalsize;
      docBuf.data = pBuf;

      status = NW_HTMLP_HtmlToWbxml(&docBuf, HTTP_iso_10646_ucs_2,
                                    NW_BYTEORDER_BIG_ENDIAN,
                                    &pWbxmlBuffer, &line,
                                    NW_xhtml_1_0_PublicId,
                                    NW_HTMLP_Get_ElementTableCount(),
                                    NW_HTMLP_Get_ElementDescriptionTable(), 
                                    NW_FALSE, NW_FALSE, 
                                    NULL, NULL, NW_FALSE);
      NW_THROW_ON_ERROR(status);
    }
    /* build the DOM tree */
    *ppDomTree = NW_HED_DomTree_New(pWbxmlBuffer, HTTP_iso_10646_ucs_2,
                                    NW_xhtml_1_0_PublicId, NULL);
    NW_THROW_ON_NULL(*ppDomTree, status, KBrsrOutOfMemory);
  }
  NW_CATCH(status) {
    NW_Buffer_Free(pWbxmlBuffer);
  }
  NW_FINALLY {
    if (freeNeeded == NW_TRUE) {
      NW_Mem_Free(pAttrUcs2);
    }
    NW_Mem_Free(pBuf);
    return status;
  }
  NW_END_TRY
}

/* Caller should call NW_Mem_Free(*ppValue) when done with value. */
static
TBrowserStatusCode
NW_XHTML_PiElementHandler_GetAttributeValue(NW_HED_DomTree_t* pDomTree,
                                            NW_Uint16 attrNameToken,
                                            NW_Ucs2** ppValue)
{
  NW_DOM_Node_t* pDomNode;
  NW_String_t attrName;
  NW_DOM_AttributeHandle_t attributeHandle;
  NW_DOM_AttrVal_t av;
  NW_String_t nativeText;
  NW_Status_t cxml_status;

  NW_ASSERT(pDomTree != NULL);
  *ppValue = NULL;

  nativeText.length = 0;
  nativeText.storage = NULL;

  NW_TRY(status) {
    /* The root node is just a place holder. */
    pDomNode = NW_HED_DomTree_GetRootNode(pDomTree);
    NW_THROW_ON_NULL(pDomNode, status, KBrsrUnexpectedError);
    /* By construction, the document has only one element. */
    pDomNode = NW_DOM_Node_getFirstChild(pDomNode);
    NW_THROW_ON_NULL(pDomNode, status, KBrsrUnexpectedError);
    status =
      NW_HED_DomHelper_GetAttributeName(pDomNode, attrNameToken, &attrName);
    NW_THROW_ON_ERROR(status);
    /* The next call searches the attribute list and finds it, if there. */
    status =
      StatusCodeConvert(NW_DOM_ElementNode_getAttributeHandleByName(pDomNode,
                                                  &attrName,
                                                  &attributeHandle));
    NW_THROW_ON_ERROR(status);
    /* Going to cheat a bit here.  We know that the attribute name does not
       contain a value prefix so we can skip calling
       NW_DOM_AttributeHandle_getValuePrefix().  Also, we can skip looking
       for multipart attributes since the value will be encoded as a single
       string. */
    cxml_status =
      NW_DOM_AttributeHandle_getNextVal(&attributeHandle, &av);
    if ((cxml_status != NW_STAT_WBXML_ITERATE_MORE)
        || ((NW_DOM_AttrVal_getType(&av) != NW_WBXML_ATTR_COMPONENT_TOKEN )
         && (NW_DOM_AttrVal_getType(&av) != NW_WBXML_ATTR_COMPONENT_STRING))) {
      /* This seems counter intuitive, but if it found a value it
         will want to iterate once more at which time it will indicate
         there is no more. Also, we expect a string value. */
      NW_THROW_STATUS(status, KBrsrUnexpectedError);
    }
    status = StatusCodeConvert(NW_DOM_AttrVal_toString(&av, &nativeText, HTTP_iso_10646_ucs_2));
    NW_THROW_ON_ERROR(status);
    /* This next call forces an aligned, null terminated copy to be made. */
    status = StatusCodeConvert(NW_String_stringToUCS2Char(&nativeText, HTTP_iso_10646_ucs_2,
                                        ppValue));
    NW_THROW_ON_ERROR(status);
  }
  NW_CATCH(status) {
    *ppValue = NULL;
  }
  NW_FINALLY {
    NW_String_deleteStorage(&nativeText);
    return status;
  }
  NW_END_TRY
}


static
TBrowserStatusCode
NW_XHTML_PiElementHandler_XmlStylesheet(const NW_XHTML_ElementHandler_t* elementHandler,
                                        NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* node)
{
  NW_Text_t*         piData = NULL;
  NW_Uint16          piTargetToken;
  NW_HED_DomTree_t*  pDomTree = NULL;
  NW_Ucs2*           type = NULL;
  NW_Ucs2*           relation = NULL;
  NW_Ucs2*           media = NULL;
  NW_Ucs2*           href = NULL;
  NW_Ucs2*           alt = NULL;
  NW_Ucs2*           charset = NULL;

  NW_REQUIRED_PARAM(elementHandler);

  NW_TRY(status) {
    /*
      You could get the PI data directly from the DOM but it wouldn't
      expand entity references or wml variables:

      status = NW_DOM_ProcessingInstructionNode_getData (node, &piData);

      Instead we have use the fact that PI nodes look like element nodes
      and the PI target and data just look like an attribute value pair.
    */
    piTargetToken = NW_DOM_ProcessingInstructionNode_getTargetToken (node);
    NW_XHTML_GetDOMAttribute(contentHandler, node, piTargetToken, &piData);
    NW_THROW_ON_NULL(piData, status, KBrsrSuccess);

    /*
      The horrible thing about the xml-stylesheet PI is that while it
      has attributes exactly like any element, the attributes can't be
      parsed along with the rest of the document because, by XML/HTML
      specification, PI is considered to be outside the scope of
      document parsing.  This means that we now have to go back and
      parse again!  Stupid specification authors ;-) They did
      something similar with the media attribute too :-(

      To process the pseudo attributes, create an HTML document with
      the attributes on an element, parse and build a DOM tree.
    */
    status = NW_XHTML_PiElementHandler_MakeDom(piData, &pDomTree);
    NW_Object_Delete(piData);
    NW_THROW_ON_ERROR(status);
    NW_THROW_ON_NULL(pDomTree, status, KBrsrFailure);

    /*
      Now we can access the DOM for the attributes like a typical
      element handler.  We need to access the DOM directly and not
      through the "helper" methods because entity expansion has
      already been done and we must not do further entity
      expansion.
    */

    // Get the type attribute.
    status = NW_XHTML_PiElementHandler_GetAttributeValue(pDomTree,
        NW_XHTML_AttributeToken_type, &type);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Get the alternate attribute.
    status = NW_XHTML_PiElementHandler_GetAttributeValue(pDomTree,
        NW_XHTML_AttributeToken_alternate, &alt);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Get the media attribute.
    status = NW_XHTML_PiElementHandler_GetAttributeValue(pDomTree,
        NW_XHTML_AttributeToken_media, &media);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Get the href attribute.
    status = NW_XHTML_PiElementHandler_GetAttributeValue(pDomTree,
        NW_XHTML_AttributeToken_href, &href);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Get the charset attribute.
    status = NW_XHTML_PiElementHandler_GetAttributeValue(pDomTree,
        NW_XHTML_AttributeToken_charset, &charset);
    NW_THROW_ON(status, KBrsrOutOfMemory);

    // Load the stylesheet.
    status = NW_XHTML_ContentHandler_LoadStylesheet(contentHandler,
        type, relation, media, href, charset, alt);
    NW_THROW_ON(status, KBrsrOutOfMemory);
  }
  
  NW_CATCH(status) {
  }

  NW_FINALLY {
    NW_HED_DomTree_Delete(pDomTree);

    NW_Str_Delete(type);
    NW_Str_Delete(relation);
    NW_Str_Delete(media);
    NW_Str_Delete(href);
    NW_Str_Delete(alt);
    NW_Str_Delete(charset);
  
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_PiElementHandler_Class_t NW_XHTML_PiElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_PiElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_ElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_piElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_PiElementHandler_t NW_XHTML_PiElementHandler = {
  { { &NW_XHTML_PiElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_PiElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                       NW_XHTML_ContentHandler_t* contentHandler,
                                       NW_DOM_ElementNode_t* node)
{
  NW_Uint16 piTargetToken;

  NW_REQUIRED_PARAM(elementHandler);

  /* parameter assertion */
  NW_ASSERT (NW_Object_IsInstanceOf (elementHandler,
                                     &NW_XHTML_PiElementHandler_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (contentHandler,
                                     &NW_XHTML_ContentHandler_Class));
  NW_ASSERT (node != NULL);

  piTargetToken =
    NW_DOM_ProcessingInstructionNode_getTargetToken (node);
  
  /* Literal means as there is no corresponding token in the Dictionary. so treat as "zzzunknown". */
  if (piTargetToken == NW_WBXML_LITERAL)
        piTargetToken = NW_XHTML_AttributeToken_zzzunknown;

  if (piTargetToken == NW_XHTML_AttributeToken_xml_stylesheet) {
  /* currently only processes's xml-stylesheet PI */
        return NW_XHTML_PiElementHandler_XmlStylesheet
      (elementHandler, contentHandler, node);
    }
  if (piTargetToken == NW_XHTML_AttributeToken_zzzunknown) {
    return NW_XHTML_PiElementHandler_zzzunknown
      (elementHandler, contentHandler, node);
  }
      
  return KBrsrSuccess;
}
