/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content handler for XHTML content type
*
*/


// INCLUDE FILES
#include <e32def.h>  // To avoid NULL redefine warning (no #ifndef NULL)

#include "nw_lmgr_box.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_marqueebox.h"

#include "nw_lmgr_verticaltablebox.h"
#include "nw_lmgr_verticaltablerowbox.h"
#include "nw_lmgr_verticaltablecellbox.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_lmgr_textbox.h"

#include <f32file.h>
#include <flogger.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>


#include "nwx_string.h"

#include "nw_xhtml_xhtmlcontenthandleri.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "nw_basictables_trelementhandler.h"
#include "nw_basictables_cellelementhandler.h"
#include "nw_xhtml_textelementhandler.h"
#include "nw_text_coretextelementhandler.h"
#include "nw_xhtml_pielementhandler.h"
#include "nw_image_imgelementhandler.h"
#include "nw_hed_documentroot.h"
#include "nw_hed_inumbercollectori.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_dom_domvisitor.h"
#include "nw_tinydom.h"
#include "nw_dom_element.h"
#include "nw_dom_document.h"
#include "nw_adt_segmentedvector.h"
#include "nw_adt_resizableobjectvector.h"
#include "nw_adt_mapiterator.h"
#include "nw_xmlp_xmlp2wbxml.h"
#include "nw_htmlp_to_wbxml.h"
#include "nwx_url_utils.h"
#include "nwx_string.h"
#include "nwx_logger.h"
#include "nw_hed_context.h"
#include "NW_Text_Abstract.h"
#include "nw_text_ucs2.h"
#include "nw_xhtml_xhtmlentityset.h"
#include "nwx_ctx.h"
#include "nw_hed_documentroot.h"
#include "HEDDocumentListener.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nw_hed_domhelper.h"
#include "nw_msg_messagedispatcher.h"
#include "nw_markup_numbercollector.h"
//R->ul #include "urlloader_urlloaderint.h"
#include "nwx_osu_file.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nwx_http_defs.h"
#include "nwx_settings.h"
//#include "nw_ecma_contenthandler.h"
#include "MapElementList.h"
#include "nw_lmgr_simplepropertylist.h"
#include "BrsrStatusCodes.h"
#include "nwx_statuscodeconvert.h"
#include <flogger.h>
#include "nw_markup_wbxmlmodule.h"

#include "nw_structure_scriptelementhandler.h"
#include "XhtmlObjectElementHandlerOOCi.h"
#include "XHTMLMessageListener.h"
#include "BodyPart.h"
#include "MVCShell.h"
#include "BrCtl.h"


#include <stddef.h>
#include "CReferrerHelper.h"
#include "CSSVariableStyleSheet.h"

// MODULE DATA STRUCTURES

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- *
   forward method declarations
 * ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_ContentHandler_MetaRefreshTimerCallback (void* timerData);

// ============================= LOCAL FUNCTIONS ===============================


static
NW_LMgr_ContainerBox_t*
NW_XHTML_ContentHandler_GetContainerSpecialCase ( NW_DOM_Node_t* node,
        NW_DOM_Node_t* parentNode,
        NW_LMgr_ContainerBox_t* containerBox)
    {
    NW_ASSERT (parentNode);
    NW_ASSERT (node);
    NW_ASSERT (containerBox);

    // special handling for form inside table
    NW_Uint16 token = NW_HED_DomHelper_GetElementToken (parentNode);
    if (token == NW_XHTML_ElementToken_form)
        {
        // if chunk cuts after form
        token = NW_HED_DomHelper_GetElementToken (node);
        if (token == NW_XHTML_ElementToken_tr ||
            token == NW_XHTML_ElementToken_td ||
            token == NW_XHTML_ElementToken_th)
            {
            containerBox = NW_LMgr_Box_GetParent(containerBox);
            }
        else
            {
            NW_Uint32 type = NW_DOM_Node_getNodeType(node);
            // special handling for text node is under form and form is under table, th, tr
            if (type == NW_DOM_TEXT_NODE)
                {
                NW_DOM_Node_t* tempNode = NW_DOM_Node_getParentNode(parentNode);
                token = NW_HED_DomHelper_GetElementToken (tempNode);
                if (token == NW_XHTML_ElementToken_tr ||
                    token == NW_XHTML_ElementToken_th ||
                    token == NW_XHTML_ElementToken_table)
                    {
                    containerBox = NW_LMgr_Box_GetParent(containerBox);
                    }
                }
            }
        }
    else if (token == NW_XHTML_ElementToken_table)
        {
        // if chunk cuts above form
        token = NW_HED_DomHelper_GetElementToken (node);
        if (token == NW_XHTML_ElementToken_form)
            {
            containerBox = NW_LMgr_Box_GetParent(containerBox);
            }
        } // end of if (token == NW_XHTML_ElementToken_form)

    return containerBox;
    }

static
NW_DOM_Node_t*
NW_XHTML_ContentHandler_GetParentNodeSpecialCase ( NW_DOM_Node_t* node,
        NW_DOM_Node_t* parentNode,
        NW_Bool* formInTable)
    {
    NW_ASSERT (parentNode);
    NW_ASSERT (node);

    NW_Uint16 token = NW_HED_DomHelper_GetElementToken (parentNode);
    if (token == NW_XHTML_ElementToken_form)
        {
        // special handling for form inside table
        token = NW_HED_DomHelper_GetElementToken (node);
        if (token == NW_XHTML_ElementToken_tr ||
            token == NW_XHTML_ElementToken_td ||
            token == NW_XHTML_ElementToken_th)
            {
            parentNode = NW_DOM_Node_getParentNode(parentNode);
            }
        else
            {
            NW_Uint32 type = NW_DOM_Node_getNodeType(node);
            // special handling for text node is under form and form is under table, th, tr
            if (type == NW_DOM_TEXT_NODE)
                {
                NW_DOM_Node_t* tempNode = NW_DOM_Node_getParentNode(parentNode);
                token = NW_HED_DomHelper_GetElementToken (tempNode);
                if (token == NW_XHTML_ElementToken_tr ||
                    token == NW_XHTML_ElementToken_th ||
                    token == NW_XHTML_ElementToken_table)
                    {
                    parentNode = tempNode;
                    }
                }
            }
        }
    else if (token == NW_XHTML_ElementToken_table)
        {
        // if chunk cuts above form
        token = NW_HED_DomHelper_GetElementToken (node);
        if (token == NW_XHTML_ElementToken_form)
            {
            *formInTable = NW_TRUE;
            }
        } // end of if (token == NW_XHTML_ElementToken_form)

    return parentNode;
    }

static
TBrowserStatusCode
NW_XHTML_ContentHandler_GetContainerBox ( NW_DOM_Node_t* lastDomNode,
        NW_DOM_Node_t* currentDomNode,
        NW_LMgr_ContainerBox_t** containerBox,
        NW_LMgr_ContainerBox_t* parentBox )
    {
    NW_ASSERT (lastDomNode);
    NW_ASSERT (currentDomNode);
    NW_ASSERT (parentBox);

    // used to track the levels up in the box tree to find the right container box
    NW_LMgr_Box_t* lastBox;

    // check the levels of the current parent node and the last dom node
    NW_DOM_Node_t* currentParentNode = NW_DOM_Node_getParentNode(currentDomNode);
    lastBox = (NW_LMgr_Box_t *)parentBox ;

    // if the currentDomNode's ancestor is the last node, then return the lastContainerBox
    // this can happen when last dom node's children arrives in the next chunk
    if (currentParentNode == lastDomNode)
        {
        // get the last box from the box tree
        do
            {
            NW_ADT_Vector_Metric_t numChildren = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(lastBox ));
            // if the box has no children , then break , it could be a box or a last container box
            if (numChildren == 0 )
                {
                break;
                }
            // get the last box in the level
            numChildren --;
            lastBox = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(lastBox), numChildren );
            }
        while (NW_Object_IsInstanceOf (lastBox, &NW_LMgr_ContainerBox_Class));

        // if the last box is not an instance of container box , find the parent box
        // of the last box
        NW_LMgr_ContainerBox_t* lastContainerBox;
        if (!NW_Object_IsInstanceOf (lastBox, &NW_LMgr_ContainerBox_Class))
            {
            lastContainerBox = NW_LMgr_Box_GetParent(lastBox );
            }
        else
            {
            lastContainerBox = NW_LMgr_ContainerBoxOf(lastBox );
            }

        // special handling for form inside table
        *containerBox = NW_XHTML_ContentHandler_GetContainerSpecialCase( currentDomNode,
                        currentParentNode,
                        lastContainerBox);
        }
    else
        {
        // if the container box is not in the parent hirerchy
        NW_Bool formInsideTable = NW_FALSE;
        currentParentNode = NW_XHTML_ContentHandler_GetParentNodeSpecialCase (currentDomNode,
                            currentParentNode,
                            &formInsideTable);

        // traverse the last branch of the box tree
        NW_Bool found = NW_FALSE;
        do
            {
            NW_LMgr_PropertyValue_t value;
            value.object = NULL;
            (void)NW_LMgr_Box_GetPropertyValue(lastBox, NW_CSS_Prop_elementId, NW_CSS_ValueType_Object, &value);
            if (value.object)
                {
                NW_DOM_ElementNode_t* elementNode = (NW_DOM_ElementNode_t*)value.object;
                if (elementNode == currentParentNode)
                    {
                    found = NW_TRUE;
                    break;
                    }
                }

            NW_ADT_Vector_Metric_t numChildren = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(lastBox ));
            if (numChildren == 0 )
                {
                break;
                }
            numChildren --;
            lastBox = NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(lastBox), numChildren );
            }
        while (NW_Object_IsInstanceOf (lastBox, &NW_LMgr_ContainerBox_Class));

        // could be the case when header occupies first chunk
        if (!found)
            {
            NW_LMgr_Box_t* rootBox = NW_LMgr_BoxOf(NW_LMgr_Box_GetParent(parentBox));
            NW_ASSERT(NW_Object_IsInstanceOf (rootBox, &NW_LMgr_RootBox_Class));
            NW_LMgr_Box_t* bodyBox = NW_LMgr_RootBox_GetBody(NW_LMgr_RootBoxOf(rootBox));
            if (!bodyBox)
                {
                *containerBox = NW_LMgr_ContainerBoxOf(NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(rootBox), 0));
                }
            else
                {
                *containerBox = NW_LMgr_ContainerBoxOf(bodyBox);
                }
            }
        else
            {
            if (formInsideTable)
                {
                // if form is under table, attach form to the parent box of table box
                *containerBox = NW_LMgr_Box_GetParent(lastBox);
                }
            else
                {
                *containerBox = NW_LMgr_ContainerBoxOf(lastBox);
                }
            }
        } // end of if (currentParentNode == lastDomNode)

    return KBrsrSuccess;
    }

static
TBrowserStatusCode
NW_XHTML_ContentHandler_CreatePartialBoxTree (NW_XHTML_ContentHandler_t* thisObj,
        NW_LMgr_Box_t** boxTree)
    {
    NW_DOM_Node_t* domNode = NULL;
    NW_DOM_Node_t* lastDomNode;
    const NW_XHTML_ElementHandler_t* elementHandler;
    NW_cXML_DOM_DOMVisitor_t * domDOMVisitor = NULL;

    NW_LOG0( NW_LOG_LEVEL2, "NW_XHTML_ContentHandler_CreatePartialBoxTree START" );

    NW_TRY (status)
        {
        if (thisObj->prevInitializedDomNode == 0)
            NW_THROW_SUCCESS(status);

        // if there are new nodes to create the box tree
        domDOMVisitor =
            NW_cXML_DOM_DOMVisitor_New( NW_HED_DomTree_GetRootNode( thisObj->domTree));
        NW_THROW_ON_NULL( domDOMVisitor, status, KBrsrOutOfMemory );

        // skip DOM nodes for which boxes were created (of the previous chunks)
        domDOMVisitor->currentNode = thisObj->prevInitializedDomNode;
        lastDomNode = thisObj->prevInitializedDomNode;

#ifdef _DEBUG1

        NW_LMgr_Box_DumpBoxTree1((NW_LMgr_Box_t*)*boxTree);
#endif

        // get the next dom node - this will be the first node of the new chunk
        domNode = NW_cXML_DOM_DOMVisitor_Next( domDOMVisitor, NW_DOM_ELEMENT_NODE |
                                               NW_DOM_TEXT_NODE | NW_DOM_PROCESSING_INSTRUCTION_NODE );

        if (domNode == 0) // no new domNode to create a tree
            NW_THROW_SUCCESS(status);

        do
            {
            NW_LMgr_ContainerBox_t* containerBox = NULL;

            // find the correspoding container box for the dom node
            (void)NW_XHTML_ContentHandler_GetContainerBox(lastDomNode, domNode, &containerBox, NW_LMgr_ContainerBoxOf(*boxTree));
            NW_ASSERT(containerBox != NULL);
            NW_ASSERT(NW_Object_IsInstanceOf (containerBox, &NW_LMgr_ContainerBox_Class));
            NW_ASSERT(!NW_Object_IsInstanceOf (containerBox, &NW_LMgr_RootBox_Class));

            // create the box tree for the subtree
            // all the siblings can use the container box to create the box tree
            // traverse the sibling nodes and create the box tree for the sibling nodes.
            // skip the siblings and all its child nodes
            NW_DOM_Node_t* prevDomNode = domNode;

            for (;domNode != NULL ;domNode = NW_DOM_Node_getNextSibling (domNode))
                {
                NW_Uint32 type = NW_DOM_Node_getNodeType(domNode);
                thisObj->prevInitializedDomNode = domNode ;
                if (type == NW_DOM_TEXT_NODE)
                    {
                    NW_DOM_Node_t* parentNode = NW_DOM_Node_getParentNode(domNode);
                    NW_Uint16 token = NW_HED_DomHelper_GetElementToken (parentNode);

                    if (token == NW_XHTML_ElementToken_table ||
                        token == NW_XHTML_ElementToken_tr ||
                        token == NW_XHTML_ElementToken_th)
                        {
                        // special handling for text node under table, th, tr
                        lastDomNode = domNode;
                        continue;
                        }
                    else if (token == NW_XHTML_ElementToken_form &&
                             (NW_Object_IsInstanceOf (containerBox, &NW_LMgr_VerticalTableBox_Class) ||
                              NW_Object_IsInstanceOf (containerBox, &NW_LMgr_VerticalTableRowBox_Class)))
                        {
                        // special handling for text node under form in dom tree
                        // text node is under table, th, tr in box tree
                        lastDomNode = domNode;
                        continue;
                        }
                    }
                elementHandler = NW_XHTML_ContentHandler_GetElementHandler (thisObj,
                                 (NW_DOM_ElementNode_t*) domNode);
                NW_ASSERT(elementHandler != NULL);

                status = NW_XHTML_ElementHandler_CreateBoxTree (elementHandler,
                         thisObj,
                         domNode,
                         containerBox);
                _NW_THROW_ON_ERROR(status);
                lastDomNode = domNode;
                } // end of for (;domNode != NULL

            // go up the hirarchy and process all the dom nodes and all its children
            do
                {
                prevDomNode = NW_DOM_Node_getParentNode(prevDomNode);
                if (prevDomNode && NW_DOM_Node_getNextSibling (prevDomNode))
                    {
                    domNode = NW_DOM_Node_getNextSibling (prevDomNode);
                    break;
                    }
                }
            while (prevDomNode);

#ifdef _DEBUG1

            NW_LMgr_Box_DumpBoxTree((NW_LMgr_Box_t*)*boxTree);
#endif

            }
        while (domNode);
        } // end NW_TRY
    NW_CATCH (status)
    {}
    NW_FINALLY
    {
        if (domDOMVisitor)
        {
        NW_Object_Delete (domDOMVisitor);
            }
        NW_LOG0( NW_LOG_LEVEL2, "NW_XHTML_ContentHandler_CreatePartialBoxTree END" );
        return status;
    }
    NW_END_TRY
    }


/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */
// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_CreatePartialDocument( NW_XHTML_ContentHandler_t* contentHandler,
        NW_Url_Resp_t* response, NW_Bool isFirst )
    {
    NW_Buffer_t* domBuffer;
    NW_Uint32 line = 0;
    TBrowserStatusCode status;
    NW_Uint32 codePageSwitchCount = 0;
    NW_Int32 lastValid = -1;
    NW_Bool encodingFound;
    void* WBXMLEncStrTbl = NULL;

    NW_LOG0(NW_LOG_LEVEL2, "NW_XHTML_ContentHandler_CreatePartialDocument");

    status = _NW_HED_CompositeContentHandler_ComputeEncoding(NW_HED_CompositeContentHandlerOf(contentHandler),
             response, &encodingFound);
    if (status != KBrsrSuccess)
        {
        return status;
        }

    if(response->contentLocationString != NULL)
        {
        contentHandler->contentLocation = NW_Str_CvtFromAscii((char *)response->contentLocationString);
        if(contentHandler->contentLocation  == NULL)
            {
            return KBrsrOutOfMemory;
            }
        }

    /* Change done to support saved deck */
    if ((0 == NW_Asc_stricmp((char*)HTTP_text_vnd_wap_wml_string,
                             (char*)response->contentTypeString)) ||
        (0 == NW_Asc_stricmp((char*)HTTP_application_xhtml_xml_string,
                             (char*)response->contentTypeString)) ||
        (0 == NW_Asc_stricmp((char*)HTTP_application_vnd_wap_xhtml_xml_string,
                             (char*)response->contentTypeString)) ||
        (0 == NW_Asc_stricmp((char*)HTTP_text_html_string,
                             (char*)response->contentTypeString)) ||
        (0 == NW_Asc_stricmp((char*)HTTP_application_vnd_wap_wml_plus_xml_string,
                             (char*)response->contentTypeString)))
        {
        NW_HED_CharsetConvContext ctx;
        ctx.contentHandler = NW_HED_CompositeContentHandlerOf( contentHandler );
        ctx.response = response;

        NW_LOG0(NW_LOG_LEVEL2, "CreateDocument: before NW_HTMLP_SegToWbxml ");

        NW_Bool detectEncoding = (encodingFound) ? NW_FALSE : NW_TRUE;
        status = NW_HTMLP_SegToWbxml( response->body, response->charset,
                                      response->byteOrder,
                                      &domBuffer, &line, NW_xhtml_1_0_PublicId,
                                      NW_HTMLP_Get_ElementTableCount(),
                                      NW_HTMLP_Get_ElementDescriptionTable(),
                                      NW_FALSE,       /* consume spaces */
                                      detectEncoding, /* need to detect charset */
                                      isFirst,        /* is this the first segment of a document? */
                                      NW_FALSE,       /* is this the last segment of a document? */
                                      &lastValid,
                                      &(contentHandler->htmlpParser) ,
                                      &codePageSwitchCount,
                                      &ctx,
                                      NW_HED_CompositeContentHandler_CharConvCB,
                                      &WBXMLEncStrTbl,
                                      NW_FALSE);
        if ( status == KBrsrBufferTooSmall )
          {
          NW_LOG0(NW_LOG_LEVEL2, "CreatePartialDocument: NW_HTMLP_SegToWbxml returns KBrsrBufferTooSmall.");
          return status;
          }
        if (BRSR_STAT_IS_FAILURE(status))
          {
          NW_LOG1(NW_LOG_LEVEL1, "!!!CreatePartialDocument: NW_HTMLP_SegToWbxml: BadContent: SegToWbxml failure. status: %d", status);
          NW_Buffer_Free(domBuffer);
          return KBrsrXhtmlBadContent;
          }
        if (domBuffer)
        {
          NW_LOG1(NW_LOG_LEVEL2, "CreatePartialDocument: NW_HTMLP_SegToWbxml succeeds, domBuffer length: %d", domBuffer->length);
        }
    }
    else
        {
        /* init our domBuffer */
        domBuffer = NW_Buffer_New(0);

        if (domBuffer != NULL)
            {
            domBuffer->allocatedLength = response->body->allocatedLength;
            domBuffer->length = response->body->length;
            domBuffer->data = response->body->data;

            /* This block is only used to switch the byte order of domBuffer. Each time the
            function NW_HED_DomTree_New is called, the byte order of domBuffer is changed.
            For a saved page, the domBuffer saved in the byte order after the change when
            this page was loaded from web. So this order is wrong when call function
            NW_HED_DomTree_New below.  Thus we call NW_HED_DomTree_New here to change the
            byte order back for saved page loading */
            contentHandler->domTree = NW_HED_DomTree_New (domBuffer, response->charset,
                                      NW_xhtml_1_0_PublicId,WBXMLEncStrTbl);

            if (contentHandler->domTree != NULL)
                {
                /* Orphan the Buffer so it isn't deleted then delete the temp DomTree */
                domBuffer = NW_HED_DomTree_OrphanDomBuffer (contentHandler->domTree);

                NW_HED_DomTree_Delete (contentHandler->domTree);
                contentHandler->domTree = NULL;
                }
            /* end of the byte order switch */

            delete response->body;
            response->body = NULL;
            }
        else
            {
            return KBrsrOutOfMemory;
            }
        }

    /* build the DOM tree */
    if (lastValid == -1)
    {
      contentHandler->domTree = NW_HED_DomTree_New (domBuffer,
                                             response->charset,
                                  NW_xhtml_1_0_PublicId,WBXMLEncStrTbl
                                             );
      NW_LOG0(NW_LOG_LEVEL2, "CreatePartialDocument: after NW_HED_DomTree_New ");
    }
    else
    {
      if (isFirst)
      {
        NW_HED_DomTree_Delete (contentHandler->domTree);
        contentHandler->domTree = NULL;
      }
      status = NW_HED_DomTree_Append (&(contentHandler->domTree),
                                      domBuffer,
                                      HTTP_iso_10646_ucs_2,
                                      NW_xhtml_1_0_PublicId,
                                      codePageSwitchCount,
                                        lastValid,
                                        WBXMLEncStrTbl
                                      );
      NW_LOG1(NW_LOG_LEVEL2, "CreatePartialDocument: after NW_HED_DomTree_Append, status: %x", status);
    }

    // set override the status if NW_HED_DomTree_New was called and failed
    if (contentHandler->domTree == NULL && status == KBrsrSuccess)
        {
      NW_LOG0(NW_LOG_LEVEL1, "!!!NW_XHTML_ContentHandler_CreatePartialDocument: BadContent: NULL domTree with success status after NW_HED_DomTree_Append ");
        status = KBrsrXhtmlBadContent;
        }

    /* init the domHelper */
    contentHandler->domHelper.entitySet =
        NW_HED_EntitySetOf (&NW_XHTML_EntitySet);
    contentHandler->domHelper.context = NULL;
    contentHandler->domHelper.resolvePlainTextVariablesAndEntities = NW_TRUE;

    /* find the body element, if it exists */
    if (contentHandler->domTree != NULL && status == KBrsrSuccess)
        {
        contentHandler->baseElement =
            NW_HED_DomHelper_FindElement (&(contentHandler->domHelper),
                                          NW_HED_DomTree_GetRootNode(contentHandler->domTree),
                                          3, NW_XHTML_ElementToken_base, 0, NULL);
        }
    else
    {
      NW_LOG1(NW_LOG_LEVEL2, "!!CreatePartialDocument: DOM Tree creation/appending failed, status: %x", status);
    }

    /* print the domTree */
#ifdef _DEBUG1
    if (status == KBrsrSuccess)
        {
        NW_HED_DomHelper_PrintTree
        (&contentHandler->domHelper,
         NW_HED_DomTree_GetRootNode (contentHandler->domTree));
        }
#endif

    return status;
    }

static
TBrowserStatusCode
NW_XHTML_ContentHandler_AppendPartialDocument( NW_XHTML_ContentHandler_t* contentHandler,
        NW_Url_Resp_t* response)
    {
    return NW_XHTML_ContentHandler_CreatePartialDocument( contentHandler, response, NW_FALSE );
    }

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_XHTML_ContentHandler_InitializeElementHandlers( NW_XHTML_ContentHandler_t* thisObj )
    {
    NW_cXML_DOM_DOMVisitor_t* domDOMVisitor = NULL;
    NW_DOM_Node_t* domNode = NULL;
    NW_DOM_Node_t* prevToLast = NULL;

    NW_ASSERT( thisObj != NULL );

    NW_TRY (status)
        {
        // should not crash browser even if domTree is not valid for some reason.
      if (!thisObj->domTree)
        NW_LOG0(NW_LOG_LEVEL1, "!!!NW_XHTML_ContentHandler_InitializeElementHandlers: BadContent: domTree NULL");
        NW_THROW_ON_NULL( thisObj->domTree, status, KBrsrXhtmlBadContent );
        // create the DOMVisitor object on the document and use it to traverse the
        // tree
        domDOMVisitor = NW_cXML_DOM_DOMVisitor_New( NW_HED_DomTree_GetRootNode( thisObj->domTree ) );
        NW_THROW_ON_NULL( domDOMVisitor, status, KBrsrOutOfMemory );

        NW_LOG0(NW_LOG_LEVEL2, "====== ! NW_XHTML_ContentHandler_Initialize: before while loop ");

        // initialize DOMVisitor to last node of the previous chunk as we do not
        // want to initialize already visited DOM nodes
        domDOMVisitor->currentNode = thisObj->lastInitializedDomNode;

        while ( ( domNode = NW_cXML_DOM_DOMVisitor_Next( domDOMVisitor,
                            NW_DOM_ELEMENT_NODE |
                            NW_DOM_TEXT_NODE |
                            NW_DOM_PROCESSING_INSTRUCTION_NODE ) )
                != NULL )
            {
            const NW_XHTML_ElementHandler_t * elementHandler;

            // skip the elements up to the skipToNode, if found set skipToNode to NULL
            if (thisObj->skipToNode != NULL )
                {

				NW_DOM_Node_t* parentNode;
                parentNode = NW_DOM_Node_getParentNode((NW_DOM_Node_t*) domNode) ;

                if (thisObj->skipToNode == parentNode )
                    {
                    continue;
                    }
                else
                    {
                    thisObj->skipToNode = NULL;
                    }
                }
          // set iBodyIsPresent to true if we hit the <body> tag
          if( !thisObj->iBodyIsPresent && NW_DOM_Node_getNodeType ((NW_DOM_Node_t*) domNode ) == NW_DOM_ELEMENT_NODE )
            {
            if( NW_HED_DomHelper_GetElementToken (domNode) == NW_XHTML_ElementToken_body )
                {
                //
                thisObj->iBodyIsPresent = NW_TRUE;
                }
            }

            prevToLast = thisObj->lastInitializedDomNode;
            thisObj->lastInitializedDomNode = domNode;

            // get the ElementHandler for the element - if there is no ElementHandler,
            // we must ignore the element and continue our traversal
            elementHandler = NW_XHTML_ContentHandler_GetElementHandler(
                                 thisObj, (NW_DOM_ElementNode_t*) domNode );

            if ( elementHandler == NULL )
                {
                continue;
                }

            // initialize the element
            status = NW_XHTML_ElementHandler_Initialize(
                         elementHandler, thisObj, (NW_DOM_ElementNode_t*)domNode );
            NW_THROW_ON( status, KBrsrOutOfMemory );

            // stopInitEH is a special flag used to stop initializing element
            // handlers. It is set by NW_XHTML_ElementHandler_NextSiblingExists(),
            // which is called by several of the ElementHandler_Initialize() methods.
            if (thisObj->stopInitEH)
                {
                break;
                }
            } // end while...


        if (thisObj->stopInitEH)
            {
            // in case select is cut between chunks, leave it processed in the next chunk
            thisObj->stopInitEH = NW_FALSE;

            if(!thisObj->lastChunk)
                {
        // Special handling for certain elements; leave their processing to the
        // next chunk. Set lastInitializedDomNode to be the node right before,
        // either sibling or parent. Note, this is used in conjunction with
        // stopInitEH flag set by NW_XHTML_ElementHandler_NextSiblingExists(),
        // which is called by several of the ElementHandler_Initialize() methods.
        NW_Uint16 token = NW_HED_DomHelper_GetElementToken (thisObj->lastInitializedDomNode);
        if (token == NW_XHTML_ElementToken_style ||
            token == NW_XHTML_ElementToken_title ||
            token == NW_XHTML_ElementToken_script ||
            token == NW_XHTML_ElementToken_select ||
            token == NW_XHTML_ElementToken_object ||
            token == NW_XHTML_ElementToken_map ||
            token == NW_XHTML_ElementToken_textarea ||
            token == NW_XHTML_ElementToken_table ||
            token == NW_XHTML_ElementToken_a)
            {
            thisObj->lastInitializedDomNode = prevToLast;
            }
                }
            }

        NW_LOG0(NW_LOG_LEVEL2, "===== ! NW_XHTML_ContentHandler_Initialize: after while loop ");
        }
    NW_CATCH (status )
        {}
    NW_FINALLY
    {
        NW_Object_Delete (domDOMVisitor);
        return status;
    }
    NW_END_TRY
    }

/* ------------------------------------------------------------------------- */
static
NW_Text_t*
NW_XHTML_ContentHandler_UnEscapeText (const NW_Text_t* text)
    {
    NW_Ucs2* origText = NULL;
    NW_Bool freeOrigText = NW_FALSE;
    NW_Ucs2* escapedText = NULL;

    NW_ASSERT (text != NULL);

    /* get the ucs2 string from text */
    NW_THROWIF_NULL (origText = NW_Text_GetUCS2Buffer (text, NW_Text_Flags_NullTerminated |
                                NW_Text_Flags_Aligned, NULL, &freeOrigText));

    /* unescape the text */
    NW_THROWIF_NULL (escapedText = NW_Str_New(NW_Url_UnEscapeStringLen(origText)));
    NW_Url_UnEscapeString(origText, escapedText);

    if (freeOrigText == NW_TRUE)
        {
        NW_Str_Delete (origText);
        }

    /* create a new text item */
    return (NW_Text_t*) NW_Text_UCS2_New ((void*) escapedText, 0, NW_Text_Flags_TakeOwnership);

    NW_CATCH_ERROR
    if (freeOrigText == NW_TRUE)
        {
        NW_Str_Delete (origText);
        }

    NW_Str_Delete (escapedText);

    return NULL;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_ShowFragment (NW_XHTML_ContentHandler_t* thisObj,
                                      const NW_Text_t* fragment)
    {
    NW_HED_DocumentRoot_t* docRoot = NULL;
    NW_Bool mustCallCompleted = NW_FALSE;
    TBrowserStatusCode status = KBrsrFailure;

    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
    NW_THROWIF_NULL (docRoot);

    /* notify the docRoot that we are performing an intra-document navigation */
    NW_THROWIF_ERROR (status = NW_HED_DocumentRoot_HandleIntraPageNavigationStarted (docRoot));
    mustCallCompleted = NW_TRUE;

    /* navigate to the fragment */
    if (fragment != NULL)
        {
        NW_LMgr_Box_t* boxTree;
        NW_Text_t* frag = NULL;

        /* unescape the fragment */
        frag = NW_XHTML_ContentHandler_UnEscapeText (fragment);

        /* show the box */
        status = NW_HED_DocumentNode_GetBoxTree (thisObj, &boxTree);
        status = docRoot->documentListener->ShowNamedBox (boxTree, frag);
        NW_Object_Delete (frag);

        if (status != KBrsrSuccess && status != KBrsrNotFound)
            {
            NW_HED_DocumentNode_HandleError (thisObj, NULL,
                                             BRSR_STAT_CLASS_GENERAL, (NW_WaeError_t) status);
            }
        NW_THROWIF_ERROR(status);
        }

    /* "navigate" to the top */
    /* TODO see if there is a better way to "scroll" to the top */
    else
        {
        (void) NW_HED_DocumentNode_NodeChanged (thisObj, NULL);
        }

    /* notify the docRoot that we are done navigating */
    NW_HED_DocumentRoot_HandleIntraPageNavigationCompleted (docRoot);

    return KBrsrSuccess;

    NW_CATCH_ERROR
    if (mustCallCompleted == NW_TRUE)
        {
        NW_HED_DocumentRoot_HandleIntraPageNavigationCompleted (docRoot);
        }

    return status;
    }

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_XHTML_ContentHandler_GetFragment (NW_XHTML_ContentHandler_t* thisObj,
                                     const NW_Text_t* url,
                                     const NW_Text_t** fragment)
    {
    TBrowserStatusCode status = KBrsrFailure;
    const NW_Ucs2* urlStorage = NULL;
    NW_Text_Length_t urlLength;
    NW_Uint8 urlFreeNeeded = NW_FALSE;
    NW_Ucs2* fragmentStorage = NULL;
    const NW_Ucs2* currentUrlStorage = NULL;
    NW_Ucs2* urlBase = NULL;
    NW_Ucs2* currentUrlBase = NULL;
    NW_Int32 match;

    /* pram check */
    NW_THROWIF_NULL (url);
    NW_THROWIF_NULL (fragment);

    *fragment = NULL;

    /* get the base urls */
    NW_THROWIF_NULL (urlStorage = NW_Text_GetUCS2Buffer (url, NW_Text_Flags_Aligned,
                                  &urlLength, &urlFreeNeeded));

    if (NW_XHTML_ContentHandler_IsSaveddeck(thisObj))
        {
        NW_THROWIF_NULL (currentUrlStorage = NW_HED_UrlResponse_GetRawUrl (
                                             NW_HED_ContentHandler_GetUrlResponse (thisObj)));
        }
    else
        {
        NW_THROWIF_NULL (currentUrlStorage = NW_HED_UrlRequest_GetRawUrl (
                                             NW_HED_ContentHandler_GetUrlRequest (thisObj)));
        }

    NW_THROWIF_ERROR (status = NW_Url_GetBase(urlStorage, &urlBase));
    NW_THROWIF_ERROR (status = NW_Url_GetBase(currentUrlStorage, &currentUrlBase));

    /* compare the current url with the specified */
    match = (NW_Str_Strlen (currentUrlBase) != NW_Str_Strlen (urlBase)) || NW_Str_Strncmp (currentUrlBase, urlBase, NW_Str_Strlen (currentUrlStorage));

    /* create the fragment Text object */
    if (match == 0)
        {
        if (NW_Url_GetFragId (urlStorage, &fragmentStorage, 0) == KBrsrSuccess)
            {
            *fragment = (NW_Text_t*)
                        NW_Text_UCS2_New (fragmentStorage, 0, NW_Text_Flags_TakeOwnership);
            }

        /*
        ** else the frag is missing.  In this case we want to show the page's default
        ** fragment (the top of the page).  This is done by returning a null fragment
        ** with a return value of KBrsrSuccess.
        */
        }

    /* the url bases do not match so return failure */
    else
        {
        NW_Mem_Free (fragmentStorage);

        status = KBrsrFailure;
        NW_THROW_ERROR();
        }

    /* clean up after us */
    if (urlFreeNeeded)
        {
        NW_Mem_Free ((NW_Ucs2*) urlStorage);
        urlFreeNeeded = NW_FALSE;
        }

    NW_Mem_Free ((NW_Ucs2*) urlBase);
    NW_Mem_Free ((NW_Ucs2*) currentUrlBase);

    return KBrsrSuccess;

    NW_CATCH_ERROR
    if (urlFreeNeeded)
        {
        NW_Mem_Free ((NW_Ucs2*) urlStorage);
        }

    NW_Mem_Free (fragmentStorage);
    NW_Mem_Free ((NW_Ucs2*) urlBase);
    NW_Mem_Free ((NW_Ucs2*) currentUrlBase);

    return status;
    }

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_XHTML_ContentHandler_LoadFragment (NW_XHTML_ContentHandler_t* thisObj,
                                      NW_HED_UrlRequest_t* urlRequest)
    {
    NW_HED_DocumentRoot_t* docRoot = NULL;
    NW_HED_HistoryStack_t* history = NULL;
    NW_HED_HistoryEntry_t* entry = NULL;
    NW_HED_HistoryEntry_t* currentEntry = NULL;
    const NW_Text_t* url = NULL;
    const NW_Text_t* fragment = NULL;
    TBrowserStatusCode status = KBrsrFailure;

    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
    NW_THROWIF_NULL (docRoot);

    NW_THROWIF_NULL (history = NW_HED_DocumentRoot_GetHistoryStack (docRoot));

    /*
    ** get the fragment, an error will be thrown if the url's base doesn't match the
    ** base of the url used to load this content handler.
    */
    NW_THROWIF_NULL (url = NW_HED_UrlRequest_GetUrl (urlRequest));
    NW_THROWIF_ERROR (status = NW_XHTML_ContentHandler_GetFragment (thisObj, url,
                               (const NW_Text_t**) & fragment));

    /* urls that don't contain a fragment shouldn't be handled internally. */
    if (fragment == NULL)
        {
        status = KBrsrFailure;
        NW_THROW_ERROR ();
        }

    /* show the frag */
    status = NW_XHTML_ContentHandler_ShowFragment (thisObj, fragment);
    if (status != KBrsrNotFound)
        {

        NW_THROWIF_ERROR(status);

        /* add history entry */
        if (NW_HED_UrlRequest_IsHistoricRequest (urlRequest) == NW_FALSE)
            {
            NW_THROWIF_NULL (entry = NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry (thisObj,
                                     url, url));
            NW_THROWIF_ERROR (status = NW_HED_HistoryStack_PushEntry (history, entry));
            entry = NULL;

            NW_Object_Delete (urlRequest);
            urlRequest = NULL;
            }

        /* update the content handler to point to the associated history entry */
        NW_THROWIF_NULL (currentEntry = NW_HED_HistoryStack_GetEntry (history,
                                        NW_HED_HistoryStack_Direction_Current));
        NW_THROWIF_ERROR (status = NW_HED_ContentHandler_SetAssociatedHistoryEntry (
                                       thisObj, currentEntry));

        }

    NW_Object_Delete ((NW_Text_t*) fragment);

    return KBrsrSuccess;

    NW_CATCH_ERROR
    NW_Object_Delete ((NW_Text_t*) fragment);
    NW_Object_Delete (entry);

    return status;
    }

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_XHTML_ContentHandler_DeleteFormLiaison (NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_ADT_Vector_Metric_t nodeCtxSize = NW_ADT_Vector_GetSize(thisObj->nodeContexts);
    for (NW_ADT_Vector_Metric_t i = 0 ; i < nodeCtxSize; i++)
        {
        NW_XHTML_Form_Node_Context_t* formCtx =
            *(NW_XHTML_Form_Node_Context_t**)NW_ADT_Vector_ElementAt
            (thisObj->nodeContexts, i);
        NW_XHTML_FormLiaison_t* formLiaison = (NW_XHTML_FormLiaison_t*)formCtx->formLiaison;
        NW_Object_Delete(formLiaison);
		formCtx->formLiaison = NULL;
        delete formCtx ;
        }
    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_XHTML_ContentHandler_RestoreControlSet (NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_HED_DocumentRoot_t* documentRoot;
    NW_HED_HistoryEntry_t* entry;
    NW_Object_Dynamic_t* controlValueSet;
    const NW_HED_UrlRequest_t *urlReq;

    documentRoot =
        (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
    NW_THROWIF_NULL (documentRoot);

    NW_THROWIF_NULL (entry = NW_HED_HistoryStack_GetEntry (documentRoot->historyStack,
                             NW_HED_HistoryStack_Direction_Current));

    NW_THROWIF_NULL (urlReq = NW_HED_HistoryEntry_GetUrlRequest (entry));

    NW_THROWIF_NULL (thisObj->historyContentData =
                         NW_HED_HistoryEntry_GetContentDataHolder (entry));

    NW_THROWIF_ERROR(NW_Object_Shared_OrphanObject(thisObj->historyContentData,
                     &controlValueSet));

    /*
    ** clear the cached form values if we are reloading the document,
    ** otherwise restore them.
    */
    if (NW_HED_UrlRequest_GetReason (urlReq) == NW_HED_UrlRequest_Reason_ShellReload)
        {
        NW_Object_Delete (controlValueSet);
        }
    else
        {
        thisObj->controlValueSet = NW_ADT_DynamicVectorOf(controlValueSet);
        }

    return KBrsrSuccess;

    NW_CATCH_ERROR
    return KBrsrFailure;
    }

// -----------------------------------------------------------------------------
static
NW_HED_Context_t*
NW_XHTML_GetGlobalContext (NW_XHTML_ContentHandler_t* thisObj)
    {
#if 1    /* use this one if we want to support wml-variables in xhtml */
    NW_HED_DocumentRoot_t* docRoot = NULL;

    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);

    return NW_HED_DocumentRoot_GetContext (docRoot, NW_HED_DocumentRoot_ContextGlobalId);
#else /* otherwise use this one */

    NW_REQUIRED_PARAM (thisObj);

    return NULL;
#endif

    }

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_XHTML_ContentHandler_MetaRefreshCreateTimer(NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_ASSERT(thisObj);

    NW_TRY (status)
        {
        if (thisObj->metaRefreshTimer == NULL)
            {
            thisObj->metaRefreshTimer =
                NW_System_Timer_New(NW_XHTML_ContentHandler_MetaRefreshTimerCallback,
                                    thisObj, (thisObj->metaRefreshSeconds * 1000), NW_FALSE);
            NW_THROW_OOM_ON_NULL(thisObj->metaRefreshTimer, status);
            }
        }
    NW_CATCH (status)
        {}
    NW_FINALLY {
        return status;
    } NW_END_TRY
    }

// -----------------------------------------------------------------------------
static
void
NW_XHTML_ContentHandler_MetaRefreshDestroyTimer(NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_ASSERT(thisObj);

    if (thisObj->metaRefreshTimer != NULL)
        {
        NW_Object_Delete (thisObj->metaRefreshTimer);
        thisObj->metaRefreshTimer = NULL;
        }
    }

// -----------------------------------------------------------------------------
static
void
NW_XHTML_ContentHandler_MetaRefreshSuspendTimer(NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_ASSERT(thisObj);

    if (thisObj->metaRefreshTimer != NULL)
        {
        /* return status is ignored */
        (void) NW_System_Timer_Stop(thisObj->metaRefreshTimer);
        }
    }

// -----------------------------------------------------------------------------
static
void
NW_XHTML_ContentHandler_MetaRefreshResumeTimer(NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_ASSERT(thisObj);

    if (thisObj->metaRefreshTimer != NULL)
        {
        /* return status is ignored */
        (void) NW_System_Timer_Resume(thisObj->metaRefreshTimer);
        }
    }

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_XHTML_ContentHandler_MetaRefreshStartLoad(NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_HED_DocumentRoot_t* docRoot;

    NW_ASSERT(thisObj);

    NW_ASSERT(thisObj->metaRefreshResolvedUrl);

    NW_TRY (status)
        {
        docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(thisObj);
        NW_THROWIF_NULL(docRoot);

        status = NW_HED_DocumentRoot_StartLoad(docRoot,
                                               NW_HED_DocumentNodeOf(docRoot),
                                               thisObj->metaRefreshResolvedUrl,
                                               thisObj->metaRefreshLoadReason,
                                               NULL,
                                               NW_UrlRequest_Type_Any, NW_CACHE_NOCACHE);
        _NW_THROW_ON_ERROR(status);
        }
    NW_CATCH (status)
        {
        NW_CATCH_ERROR
        if (status != KBrsrSuccess)
            {
            NW_HED_DocumentNode_HandleError(thisObj,
                                            NW_HED_ContentHandler_GetUrlRequest(thisObj),
                                            BRSR_STAT_CLASS_GENERAL,
                                            (NW_WaeError_t) status);
            }
        }
    NW_FINALLY {
        /* metaRefreshResolvedUrl will never be used again. */
        if (thisObj->metaRefreshResolvedUrl != NULL)
        {
        NW_Object_Delete (thisObj->metaRefreshResolvedUrl);
            thisObj->metaRefreshResolvedUrl = NULL;
            }
        return status;
    } NW_END_TRY
    }

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_XHTML_ContentHandler_MetaRefreshTimerCallback(void* timerData)
    {
    TBrowserStatusCode status;
    NW_XHTML_ContentHandler_t* thisObj;

    NW_ASSERT(timerData);

    /* cast timerData to aggregate */
    thisObj = NW_XHTML_ContentHandlerOf(timerData);

    NW_XHTML_ContentHandler_MetaRefreshDestroyTimer(thisObj);
    status = NW_XHTML_ContentHandler_MetaRefreshStartLoad(thisObj);

    return status;
    }


// -----------------------------------------------------------------------------
static
void
NW_XHTML_ContentHandler_ActivateImageMapBox( NW_XHTML_ContentHandler_t* thisObj )
    {
    NW_ASSERT( thisObj );

    // loop through the image map boxes and create area list for each of them
    if ( thisObj->imageMapLookupTable )
        {
        // imageElement list
        CMapElementList* mapElementList = (CMapElementList*)thisObj->imageMapLookupTable;
        TInt mapElementCount = mapElementList->MapElementCount();
        for ( TInt i = 0; i < mapElementCount; i++ )
            {
            CMapElementEntry* entry = mapElementList->MapElementEntry( i );

            // get image map boxes. note that one useMap can hold multiple
            // image map boxes.
            CImageMapBoxList* imageMapBoxList = entry->ImageMapBoxList();
            NW_DOM_ElementNode_t* domNode = entry->DomNode();
            // initailaze image map unless either the <img usemap= or
            // <map name= is missing.
            if ( imageMapBoxList && domNode )
                {
                TInt imageListCount = imageMapBoxList->Count();
                for ( TInt j = 0; j < imageListCount; j++ )
                    {
                    // get image element handler
                    const NW_XHTML_ElementHandler_t* imageElementHandler =
                        NW_XHTML_ElementHandlerOf( &NW_XHTML_imgElementHandler );
                    NW_LMgr_ImageMapBox_t* imageMapBox = imageMapBoxList->At( j );
                    // call create area list on every image boxes unless the image box
                    // has already been initialized with the area boxes -do not create
                    // areaboxes twice
                    if ( !NW_LMgr_ImageMapBox_GetAreaBoxList( imageMapBox ) )
                        {
                        // ignore return value
                        (void)NW_XHTML_imgElementHandler_CreateAreaList(
                            imageElementHandler, thisObj, domNode, NW_LMgr_BoxOf( imageMapBox ) );
                        }
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
static
TBrowserStatusCode
NW_XHTML_ContentHandler_DeleteAllElementHandlerInstances(NW_XHTML_ContentHandler_t *thisObj)
{
  NW_ADT_MapIterator_t iterator;

  NW_Object_Invalidate (&iterator);

  NW_TRY (status) {
    /* create an iterator */
    status = NW_ADT_MapIterator_Initialize (&iterator, thisObj->iElementHandlerMap);
    NW_THROW_ON_ERROR (status);

    while (NW_ADT_Iterator_HasMoreElements (&iterator)) {
      void* key;
      CBase* elementHandler;

      status = NW_ADT_Iterator_GetNextElement (&iterator, &key);
      NW_ASSERT (status == KBrsrSuccess);

      status = NW_ADT_Map_Get (thisObj->iElementHandlerMap, &key, &elementHandler);
      NW_THROW_ON_ERROR (status);

      if (elementHandler) {
        delete elementHandler;
      }
    }

    NW_ADT_Map_Clear (thisObj->iElementHandlerMap);
  }

  NW_CATCH (status) {
  }

  NW_FINALLY {
    NW_Object_Terminate (&iterator);
    return status;
  } NW_END_TRY
}

// -----------------------------------------------------------------------------
const
NW_XHTML_ContentHandler_Class_t NW_XHTML_ContentHandler_Class =
    {
        { /* NW_Object_Core                 */
            /* super                          */ &NW_HED_CompositeContentHandler_Class,
            /* querySecondary                 */ _NW_Object_Base_QueryInterface
        },
        { /* NW_Object_Base                 */
            /* interfaceList                  */ NW_XHTML_InterfaceList
        },
        { /* NW_Object_Dynamic              */
            /* instanceSize                   */ sizeof (NW_XHTML_ContentHandler_t),
            /* construct                      */ _NW_XHTML_ContentHandler_Construct,
            /* destruct                       */ _NW_XHTML_ContentHandler_Destruct
        },
        { /* NW_HED_DocumentNode            */
            /* cancel                         */ _NW_XHTML_ContentHandler_Cancel,
            /* partialLoadCallback           */ NULL,
            /* initialize                     */ _NW_XHTML_ContentHandler_Initialize,
            /* nodeChanged                    */ _NW_HED_DocumentNode_NodeChanged,
            /* getBoxTree                     */ _NW_HED_ContentHandler_GetBoxTree,
            /* processEvent                   */ _NW_XHTML_ContentHandler_ProcessEvent,
            /* handleError                    */ _NW_HED_DocumentNode_HandleError,
            /* suspend                        */ _NW_XHTML_ContentHandler_Suspend,
            /* resume                         */ _NW_XHTML_CompositeContentHandler_Resume,
            /* allLoadsCompleted              */ _NW_XHTML_ContentHandler_AllLoadsCompleted,
            /* intraPageNavigationCompleted   */ _NW_HED_DocumentNode_IntraPageNavigationCompleted,
            /* loseFocus                      */ _NW_XHTML_ContentHandler_LoseFocus,
            /* gainFocus                      */ _NW_XHTML_ContentHandler_GainFocus,
            /* handleLoadComplete             */ _NW_XHTML_DocumentNode_HandleLoadComplete,
        },
        { /* NW_HED_ContentHandler          */
            /* partialNextChunk               */ _NW_XHTML_ContentHandler_PartialNextChunk,
            /* getTitle                       */ _NW_XHTML_ContentHandler_GetTitle,
            /* getUrl                         */ _NW_HED_ContentHandler_GetURL,
            /* resolveUrl                     */ _NW_XHTML_ContentHandler_ResolveURL,
            /* createHistoryEntry             */ _NW_HED_ContentHandler_CreateHistoryEntry,
            /* createIntraHistoryEntry        */ _NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry,
            /* newUrlResponse                 */ _NW_HED_ContentHandler_NewUrlResponse,
            /* createBoxTree                  */ _NW_XHTML_ContentHandler_CreateBoxTree,
            /* handleRequest                  */ _NW_XHTML_ContentHandler_HandleRequest,
            /* featureQuery                   */ _NW_HED_ContentHandler_FeatureQuery,
            /* responseComplete               */ _NW_HED_ContentHandler_ResponseComplete
        },
        { /* NW_HED_CompositeContentHandler */
            /* documentDisplayed              */ _NW_XHTML_ContentHandler_DocumentDisplayed,
        },
        /* (Kimono has this in Epoc32XhtmlContentHandler)  */
        { /* NW_XHTML_ContentHandler        */
            /* numModules                     */ 0,
            /* modules                        */ NULL
        }
    };

// -----------------------------------------------------------------------------
const
NW_Object_Class_t* const NW_XHTML_InterfaceList[] =
    {
        &NW_XHTML_ContentHandler_NumberCollector_Class,
        NULL
    };

// -----------------------------------------------------------------------------
const
NW_Markup_NumberCollector_Class_t NW_XHTML_ContentHandler_NumberCollector_Class =
    {
        { /* NW_Object_Core            */
            /* super                     */ &NW_Markup_NumberCollector_Class,
            /* querySecondary            */ _NW_Object_Core_QuerySecondary
        },
        { /* NW_Object_Secondary       */
            /* offset                    */ offsetof (NW_XHTML_ContentHandler_t,
                    NW_Markup_NumberCollector)
        },
        { /* NW_Object_Aggregate       */
            /* secondaryList             */ _NW_Markup_NumberCollector_SecondaryList,
            /* construct                 */ NULL,
            /* destruct                  */ NULL
        },
        { /* NW_Markup_NumberCollector */
            /* getBoxTree                */ _NW_XHTML_ContentHandler_NumberCollector_GetBoxTree,
            /* boxIsValid                */ _NW_Markup_NumberCollector_BoxIsValid,
            /* getHrefAttr               */ _NW_XHTML_ContentHandler_NumberCollector_GetHrefAttr,
            /* getAltAttr                */ _NW_XHTML_ContentHandler_NumberCollector_GetAltAttr
        }
    };

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_ContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                    va_list* argp)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    TBrowserStatusCode status;
    const NW_HED_UrlRequest_t* urlRequest;

    NW_LOG0(NW_LOG_LEVEL2, "NW_XHTML_ContentHandler_Construct START");
    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (dynamicObject);

    /* invoke our superclass constructor */
    status = _NW_HED_CompositeContentHandler_Construct (dynamicObject, argp);
    if (status != KBrsrSuccess)
        {
        return status;
        }

    thisObj->domTree = NULL;
    thisObj->title = NULL;
    thisObj->baseElement = NULL;
	thisObj->contentLocation = NULL;
    thisObj->iBodyNode = NULL;
    thisObj->nodeContexts = (NW_ADT_DynamicVector_t*)
                            NW_ADT_ResizableVector_New (sizeof (NW_XHTML_Form_Node_Context_t*),
                                                        0, 5);

    if (thisObj->nodeContexts == NULL)
        {
        return KBrsrOutOfMemory;
        }
    thisObj->iElementHandlerMap = (NW_ADT_Map_t*) NW_ADT_ResizableMap_New (
        sizeof(NW_DOM_ElementNode_t*), sizeof(CBase*), 1, 1);

    if (thisObj->iElementHandlerMap == NULL)
        {
        NW_Object_Delete(thisObj->nodeContexts);
        thisObj->nodeContexts = NULL;

        return KBrsrOutOfMemory;
        }

    thisObj->processor = NULL;
    thisObj->historyContentData = NULL;
    thisObj->controlValueSet = NULL;
    thisObj->controlSet = NULL;
    thisObj->saveddeck = NW_FALSE;
    thisObj->histload = NW_FALSE;
	thisObj->reload = NW_FALSE;

    thisObj->htmlSubset = NW_XHTML_CONTENTHANDLER_IMODEHTMLEXT;
    thisObj->metaRefreshSet = NW_FALSE;
    thisObj->metaRefreshResolvedUrl = NULL;
    thisObj->metaRefreshTimer = NULL;

    thisObj->ignoreFramesetElement = NW_FALSE;
    thisObj->ignoreBodyElement = NW_FALSE;
    thisObj->ignoreNoScriptElement = NW_TRUE;

    thisObj->imageMapLookupTable = NULL;

    // ZAL: find a better init value
    thisObj->lastInitializedDomNode = 0;

    thisObj->stopInitEH = NW_FALSE;
    thisObj->lastChunk = NW_FALSE;
    thisObj->iScriptIsExecuted = NW_FALSE;
    thisObj->iScriptIsResumed = NW_FALSE;

    //
    thisObj->switchBackToNormalLayout = NW_FALSE;

    thisObj->formattingComplete = NW_FALSE;

    thisObj->scriptLoadComplete = NW_FALSE;

    thisObj->iBodyIsPresent = NW_FALSE;

    thisObj->metaNoCache = NW_FALSE;
	thisObj->metaCacheExpires = NW_FALSE;
	thisObj->metaExpiresDate = NULL;

	thisObj->iScriptCancelled = NW_FALSE;
    thisObj->scriptLoadTransactionIds = (void*)(new RArray<TExternalScriptNode>);
    if (!thisObj->scriptLoadTransactionIds)
        {
        return KBrsrOutOfMemory;
        }

    thisObj->scriptNodes = (void*)(new RPointerArray<NW_XHTML_Script_Node_Context_t>(1));
    if (!thisObj->scriptNodes)
        {
        return KBrsrOutOfMemory;
        }

    thisObj->formNodes = (void*)(new RPointerArray<NW_DOM_ElementNode_t>(1));

    if (!thisObj->formNodes)
        {
        return KBrsrOutOfMemory;
        }

    TRAPD(ret, thisObj->iMessageListener = new (ELeave) TMessageListener (thisObj));
    if (ret == KErrNoMemory)
      {
      return KBrsrOutOfMemory;
      }

    /*
    ** reset the global context if the request that created the content-handler
    ** was started from a shell request, like the goto dialog or
    ** hotlist dialog.  See WML June2000 Specification section 10.4.
    */
    urlRequest = va_arg (*argp, NW_HED_UrlRequest_t*);
    NW_ASSERT (urlRequest);
    NW_ASSERT (NW_Object_IsInstanceOf (urlRequest, &NW_HED_UrlRequest_Class));

    if (NW_HED_UrlRequest_GetReason (urlRequest) == NW_HED_UrlRequest_Reason_ShellLoad)
        {
        NW_HED_Context_t* context;

        context = NW_XHTML_GetGlobalContext (thisObj);

        NW_HED_Context_Reset (context);
        }

    /*
     * Initialize a local copy of the CCSSVariableStyleSheet. Note that
     * if it already exists this will efficiently get a copy of the
     * reference. If not then it will be created on the spot.
     */
    thisObj->variableStyleSheet = CCSSVariableStyleSheet::CreateSingleton();

    NW_LOG0(NW_LOG_LEVEL2, "NW_XHTML_ContentHandler_Construct END");
    /* successful completion */
    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
void
_NW_XHTML_ContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject)
    {
    NW_XHTML_ContentHandler_t* thisObj;

    /* cast the 'object' for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (dynamicObject);

    if (thisObj->iBodyNode && NW_Settings_GetEcmaScriptEnabled())
        {
        NW_HED_DocumentRoot_t* documentRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
        if (!documentRoot->documentListener->Exiting())
            {
#if !defined(__SERIES60_28__) && !defined(__SERIES60_27__)
            // Don't execute scripts if the browser is exiting. It will crash
            //R->ecma
            //NW_Ecma_HandleEvent(documentRoot , thisObj->iBodyNode, thisObj->iBodyNode, NW_Ecma_Evt_OnUnload);
#endif
            }
        }

    NW_XHTML_ContentHandler_DeleteFormLiaison (thisObj);


    NW_HED_DomTree_Delete (thisObj->domTree);
	thisObj->domTree = NULL;
    thisObj->baseElement = NULL;
	NW_Str_Delete(thisObj->contentLocation);
    NW_Object_Delete (thisObj->title);
	thisObj->title = NULL;
    thisObj->iScriptIsResumed = NW_FALSE;

    // if the document root reference the referrer url from this document
    // set it to NULL
    NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*)
                                     NW_HED_DocumentNode_GetRootNode (thisObj);
    const NW_Text_t* referrerUrl = NW_HED_DocumentRoot_GetDocumentReferrer(docRoot);
    const NW_HED_UrlResponse_t* urlResponse = NW_HED_ContentHandler_GetUrlResponse( thisObj );

    if ( urlResponse && referrerUrl && referrerUrl == NW_HED_UrlResponse_GetUrl( urlResponse ) )
        {
        NW_HED_DocumentRoot_SetDocumentReferrer(docRoot , NULL);
        }
    RArray<TExternalScriptNode>* scriptIds = STATIC_CAST(RArray<TExternalScriptNode>*, thisObj->scriptLoadTransactionIds);
    if (scriptIds)
        {
        scriptIds->Close();
        delete scriptIds;
        }

    RPointerArray<NW_XHTML_Script_Node_Context_t>* scriptNodeList = STATIC_CAST(RPointerArray<NW_XHTML_Script_Node_Context_t>*,
            thisObj->scriptNodes);
    if (scriptNodeList)
        {
        scriptNodeList->ResetAndDestroy();
        scriptNodeList->Close();
        delete scriptNodeList;
        }

    RPointerArray<NW_DOM_ElementNode_t>* formNodes = (RPointerArray<NW_DOM_ElementNode_t>*)thisObj->formNodes;
      {
        formNodes->Reset();
        delete formNodes;
      }


    NW_Object_Delete (thisObj->nodeContexts);
	thisObj->nodeContexts = NULL;
    NW_XHTML_ContentHandler_DeleteAllElementHandlerInstances(thisObj);
    NW_Object_Delete(thisObj->iElementHandlerMap);
	thisObj->iElementHandlerMap = NULL;

    NW_Object_Delete (thisObj->processor);

    if (thisObj->metaRefreshResolvedUrl != NULL)
        {
        NW_Object_Delete (thisObj->metaRefreshResolvedUrl);
		thisObj->metaRefreshResolvedUrl = NULL;
        }

    if (thisObj->metaRefreshTimer != NULL)
        {
        NW_XHTML_ContentHandler_MetaRefreshDestroyTimer(thisObj);
        }

    /* save any possible form control values to history */
    if (thisObj->controlSet != NULL)
        {
        if (thisObj->historyContentData != NULL)
            {
            NW_Object_Dynamic_t* controlValueSet;

            controlValueSet = (NW_Object_Dynamic_t*)
                              NW_XHTML_ControlSet_GetValueVector (thisObj->controlSet);

            (void) NW_Object_Shared_AdoptObject (thisObj->historyContentData, controlValueSet);
            }

        NW_Object_Delete (thisObj->controlSet);
        }

    if (thisObj->controlValueSet != NULL)
        {
        NW_Object_Delete (thisObj->controlValueSet);
        }

    if (thisObj->imageMapLookupTable != NULL)
        {
        delete static_cast<CMapElementList*>(thisObj->imageMapLookupTable);
        thisObj->imageMapLookupTable = NULL;
        }

	NW_Str_Delete(thisObj->metaExpiresDate);
    delete thisObj->iMessageListener;
  }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_ContentHandler_Cancel(NW_HED_DocumentNode_t* documentNode,
                                NW_HED_CancelType_t cancelType)
    {
    TBrowserStatusCode status;
    NW_XHTML_ContentHandler_t* thisObj;
    // parameter assertion block
    NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                       &NW_XHTML_ContentHandler_Class));
    // cast into the 'thisObj' pointer for convenience
    thisObj = NW_XHTML_ContentHandlerOf (documentNode);

    // stop the META-Refresh timer if it is running
    NW_XHTML_ContentHandler_MetaRefreshDestroyTimer(thisObj);

	thisObj->iScriptCancelled = NW_TRUE;
    // cancel any executing event script
    NW_HED_DocumentRoot_t *documentRoot =
        (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);


    RArray<TExternalScriptNode>* scriptIds = STATIC_CAST(RArray<TExternalScriptNode>*, thisObj->scriptLoadTransactionIds);
    scriptIds->Reset();

  if (thisObj->scriptNodes)
      {
      RPointerArray<NW_XHTML_Script_Node_Context_t>* scriptNodeList = STATIC_CAST(RPointerArray<NW_XHTML_Script_Node_Context_t>*,
              thisObj->scriptNodes);
      scriptNodeList->ResetAndDestroy();
      }

    // invoke super class  cancel
    status = NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
             cancel(documentNode, cancelType);

    return status;
    }


static TBrowserStatusCode NW_HTMLP_NotifyDocCompleteCallBack(void* context,
        const NW_Buffer_t* residueWbxml, NW_Uint32 encoding,
        NW_Uint32 codePageSwitchCount, NW_Int32 lastValid, void* WBXMLEncStrTbl)
    {
    TBrowserStatusCode          status = KBrsrSuccess;
    NW_XHTML_ContentHandler_t*  thisObj = (NW_XHTML_ContentHandler_t*) context;
    NW_Buffer_t*                buffer = (NW_Buffer_t*) residueWbxml;

    if (buffer != NULL)
        {
        if (!thisObj->domTree)
            {
            thisObj->domTree = NW_HED_DomTree_New(buffer,
                                                (NW_Http_CharSet_t) encoding,
                                                NW_xhtml_1_0_PublicId,
                                                WBXMLEncStrTbl);

            /* init the domHelper */
            thisObj->domHelper.entitySet = NW_HED_EntitySetOf(&NW_XHTML_EntitySet);
            thisObj->domHelper.context = NULL;
            thisObj->domHelper.resolvePlainTextVariablesAndEntities = NW_TRUE;

            /* find the body element, if it exists */
            if (thisObj->domTree != NULL)
                {
                thisObj->baseElement = NW_HED_DomHelper_FindElement(&(thisObj->domHelper),
                                             NW_HED_DomTree_GetRootNode(thisObj->domTree),
                                             3, NW_XHTML_ElementToken_base, 0, NULL);
                }
            else
                {
                status = KBrsrUnexpectedError;
                }
            }

        else
            {
            status = NW_HED_DomTree_Append(&thisObj->domTree, buffer,
                    (NW_Http_CharSet_t) encoding, NW_xhtml_1_0_PublicId, codePageSwitchCount,
                    lastValid, WBXMLEncStrTbl);
            }
        }

        return status;
    }


// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_ContentHandler_Initialize(NW_HED_DocumentNode_t* documentNode,
                                    TBrowserStatusCode aInitStatus)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    NW_HED_ContentHandler_t* contentHandler;
    const NW_HED_UrlRequest_t* urlRequest;
    NW_Uint32 encoding;
    void* WBXMLEncStrTbl = NULL;

    NW_REQUIRED_PARAM(aInitStatus);

    NW_TRY(status)
        {
        NW_LOG0(NW_LOG_LEVEL2, "_NW_XHTML_ContentHandler_Initialize START");

        // cast into the 'thisObj' pointer for convenience
        thisObj = NW_XHTML_ContentHandlerOf(documentNode);

        NW_LOG0(NW_LOG_LEVEL2,
                "in _NW_XHTML_ContentHandler_Initialize documentLoadComplete");

        contentHandler = NW_HED_ContentHandlerOf(thisObj);
        if ( NW_Asc_stricmp((char*)HTTP_application_xhtml_xml_saved_string,
                            (char*)contentHandler->response->contentTypeString) == 0 ||
             NW_Asc_stricmp((char*)HTTP_application_saved_string,
                            (char*)contentHandler->response->contentTypeString) == 0 )
            {
            NW_XHTML_ContentHandler_SetSaveddeck(thisObj, NW_TRUE);
            }

        urlRequest = NW_HED_ContentHandler_GetUrlRequest(contentHandler);
        if(urlRequest != NULL && (NW_HED_UrlRequest_GetReason(urlRequest) == NW_HED_UrlRequest_Reason_ShellPrev))
            {
            NW_XHTML_ContentHandler_SetHistoryLoad(thisObj, NW_TRUE);
            }

        if(urlRequest != NULL && (NW_HED_UrlRequest_GetReason(urlRequest) == NW_HED_UrlRequest_Reason_ShellReload))
            {
            NW_XHTML_ContentHandler_SetReLoad(thisObj, NW_TRUE);
            }


        // do not call node initialize on the handler if there are external
        // script loads pending. If load count > 0 then super class is
        // initialized in the function _NW_XHTML_DocumentNode_HandleLoadComplete


        // when document loading is complete, let the parser know about it
        // as it might have some leftovers to process
        if (thisObj->htmlpParser)
            {
            NW_HTMLP_NotifyDocComplete(thisObj->htmlpParser, &encoding,
                    &WBXMLEncStrTbl, NW_HTMLP_NotifyDocCompleteCallBack, thisObj);

            thisObj->htmlpParser = NULL;
            }

        // in case select is the last part of page, we always leave it processed in the next chunk
        // if there's no more chunk, we still want to process select
        thisObj->lastChunk = NW_TRUE;
        // make sure we've got the dom tree
        if (!thisObj->domTree)
            {
            NW_LOG0(NW_LOG_LEVEL1, "_NW_XHTML_ContentHandler_Initialize: BadContent error: domTree null");
            }


        NW_THROW_ON_NULL(thisObj->domTree, status, KBrsrXhtmlBadContent);

        status = NW_XHTML_ContentHandler_InitializeElementHandlers( thisObj );
        // Do we really wanna jump to the end and leaving out
        // initialization if something fails on initelemhandlers?
        _NW_THROW_ON_ERROR(status);


        NW_Uint16 token = NW_HED_DomHelper_GetElementToken( thisObj->lastInitializedDomNode );
        if(token == NW_XHTML_ElementToken_style ||
                        token == NW_XHTML_ElementToken_title ||
                        token == NW_XHTML_ElementToken_script ||
                        token == NW_XHTML_ElementToken_select ||
                        token == NW_XHTML_ElementToken_object ||
                        token == NW_XHTML_ElementToken_map ||
                        token == NW_XHTML_ElementToken_textarea ||
                        token == NW_XHTML_ElementToken_table ||
                        token == NW_XHTML_ElementToken_a)
        {

            NW_HED_DocumentRoot_t* docRoot = NULL;

            // set the default priority on this chunk
            CActive :: TPriority priorirty = (( CActive::TPriority )( CActive::EPriorityStandard ));

            docRoot = ( NW_HED_DocumentRoot_t* ) NW_HED_DocumentNode_GetRootNode( thisObj );
            ( void ) docRoot->documentListener->DocumentChangedPartial( docRoot, priorirty );
        }

        // create area list on each image map box
        NW_XHTML_ContentHandler_ActivateImageMapBox(thisObj);

        // no more partial content comes.

        // invalidate prev ecma script context

        // invoke our superclass method

        status = NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
                 initialize(documentNode, KBrsrSuccess);

        NW_LOG0(NW_LOG_LEVEL2, "_NW_XHTML_ContentHandler_Initialize END");

#ifdef _DEBUG1
        if (thisObj->domTree != NULL)
            {
            NW_HED_DomHelper_PrintTree (&thisObj->domHelper, NW_HED_DomTree_GetRootNode (thisObj->domTree)  );

            }
#endif
        }
    NW_CATCH( status )
        {
        }
    NW_FINALLY
        {
        return status;
        }
    NW_END_TRY
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_ContentHandler_PartialNextChunk( NW_HED_ContentHandler_t* contentHandler,
        NW_Int32 chunkIndex, NW_Url_Resp_t* response,
        void* context)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    NW_ASSERT( contentHandler != NULL );
    NW_LOG1( NW_LOG_LEVEL2, "_NW_XHTML_ContentHandler_PartialNextChunk starts: chunkIndex: %d", chunkIndex);

#ifdef __WINS__

    if ( response != NULL && response->body != NULL )
        {
        char* buffer;
        buffer = (char*)malloc( response->body->length + 1 );
        if (buffer != NULL)
            {
            memcpy( (void*)buffer, (const void*)response->body->data, response->body->length );
            buffer[ response->body->length ] = 0;
            FILE* stream = fopen( "chunks.txt", "a+" ); /* open for reading and appending */
            if (stream != NULL)
                {
                fprintf((FILE*)stream, "\n\n========== Chunk %d ==============\n", chunkIndex );
                fprintf((FILE*)stream, "%s", buffer );
                fflush((FILE*)stream);
                fclose((FILE*)stream);
                }
            }
        free( (void*)buffer);
        }
#endif // _WINS_

    NW_TRY( status )
        {
        // set the default priority on this chunk
        CActive::TPriority priorirty = ((CActive::TPriority)( CActive::EPriorityLow + 2 ));
        NW_HED_DocumentRoot_t* docRoot;
        const NW_HED_UrlRequest_t* urlRequest;
        // cast into the 'thisObj' pointer for convenience
        thisObj = NW_XHTML_ContentHandlerOf( contentHandler );
        docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode( thisObj );
        NW_Bool oldBodyIsPresent = thisObj->iBodyIsPresent;
        // make sure we have a valid response
        NW_THROW_ON_NULL( response, status, KBrsrUnexpectedError );
        // first chunk.
        if ( chunkIndex == 0 )
            {
            // set body to false
            thisObj->iBodyIsPresent = NW_FALSE;
            //
            NW_Bool disableSmallScreen;
            // invoke superclass
            status = NW_HED_ContentHandler_Class.NW_HED_ContentHandler.partialNextChunk(
                         contentHandler, chunkIndex, response, context );
            // response has been released by the base class call.
            response = NULL;
            _NW_THROW_ON_ERROR( status );

            // disable ss on xhtml pages.
            disableSmallScreen = (NW_Bool)( NW_Asc_stricmp(
                                                (char*)HTTP_application_vnd_wap_xhtml_xml_string,
                                                (char*)contentHandler->response->contentTypeString ) == 0 );
            NW_Settings_SetDisableSmallScreenLayout( disableSmallScreen );

            // if the current layout is Normal layout or ss is enabled, switch to vertical layout.
            if ( !NW_Settings_GetVerticalLayoutEnabled() && !disableSmallScreen )
                {
                NW_Settings_SetInternalVerticalLayoutEnabled( NW_TRUE );
                thisObj->switchBackToNormalLayout = NW_TRUE;
                }

            // get rid of the old title (if any)
            (void) NW_XHTML_ContentHandler_SetTitle( thisObj, NULL );

            // get the controlSet from the history
            status = NW_XHTML_ContentHandler_RestoreControlSet( thisObj );
            _NW_THROW_ON_ERROR( status );

            // build the dom tree out of the first chunk
            status = NW_XHTML_ContentHandler_CreatePartialDocument( thisObj,
                     contentHandler->response, NW_TRUE );
            _NW_THROW_ON_ERROR( status );

            // Must mark as a saved deck before we initialize the element
            // handlers and make requests
            if( NW_Asc_stricmp((char*)HTTP_application_xhtml_xml_saved_string,
                               (char*)contentHandler->response->contentTypeString) == 0 ||
                NW_Asc_stricmp((char*)HTTP_application_saved_string,
                               (char*)contentHandler->response->contentTypeString) == 0 )
                {
                NW_XHTML_ContentHandler_SetSaveddeck(thisObj, NW_TRUE);
                }
            urlRequest = NW_HED_ContentHandler_GetUrlRequest(contentHandler);
            if(urlRequest != NULL && (NW_HED_UrlRequest_GetReason(urlRequest) == NW_HED_UrlRequest_Reason_ShellPrev))
                {
                NW_XHTML_ContentHandler_SetHistoryLoad(thisObj, NW_TRUE);
                }

            if(urlRequest != NULL && (NW_HED_UrlRequest_GetReason(urlRequest) == NW_HED_UrlRequest_Reason_ShellReload))
                {
                NW_XHTML_ContentHandler_SetReLoad(thisObj, NW_TRUE);
                }

            // initialize partial element handlers
            status = NW_XHTML_ContentHandler_InitializeElementHandlers( thisObj );
            _NW_THROW_ON_ERROR( status );
            }
        else if ( chunkIndex != -1 )
            {
            // build the dom tree out of the chunk
            status = NW_XHTML_ContentHandler_AppendPartialDocument( thisObj, response );
            _NW_THROW_ON_ERROR( status );

            status = NW_XHTML_ContentHandler_InitializeElementHandlers( thisObj );
            _NW_THROW_ON_ERROR( status );
            }
        else
            {
            // this function musn't be called with the last partial response that has no body.
            // Initialize must be called instead
            NW_ASSERT( NW_TRUE );
            }
        // if this is the first chunk where the body is present, then set
        // the formatting priority higher
        if( !oldBodyIsPresent && thisObj->iBodyIsPresent )
          {
          // higher priority
          priorirty = CActive::EPriorityStandard;
          }
        // let the shell know about the new partial document so that it can initiate
        // update display message.
        // if small screen layout is not disabled do not update.
        // mobile profile pages do not format correctly when tables span
        // chunks.
        if( thisObj->iBodyIsPresent && !NW_Settings_GetDisableSmallScreenLayout() )
          {
          (void)docRoot->documentListener->DocumentChangedPartial( docRoot, priorirty );
          }
        }
    NW_CATCH( status )
        {
        // special handling for the "need more data" case which means
        // that the parser needs more chunks to process the buffer.
        // it is not an error case so the status has to be
        // modified to success
        if ( status == KBrsrBufferTooSmall )
            {
            status = KBrsrSuccess;
            }
        }
    NW_FINALLY
    {
        // body ownership is taken by the parser
        // free the response
        UrlLoader_UrlResponseDelete( response );
        NW_LOG1( NW_LOG_LEVEL2, "_NW_XHTML_ContentHandler_PartialNextChunk ends: status: %d", status);
        return status;
    }
    NW_END_TRY
    }


// -----------------------------------------------------------------------------
NW_Uint8
_NW_XHTML_ContentHandler_ProcessEvent (NW_HED_DocumentNode_t* documentNode,
                                       NW_LMgr_Box_t* box,
                                       NW_Evt_Event_t* event,
                                       void* context)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    const NW_XHTML_ElementHandler_t* elementHandler;

    /* cast the 'object' for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (documentNode);

    /* the context that was passed in is in reality a 'NW_DOM_Node_t*' - use that
       to find the appropriate ElementHandler and invoke its processEvent
       method */
    elementHandler =
        NW_XHTML_ContentHandler_GetElementHandler (NW_XHTML_ContentHandlerOf (documentNode),
                (NW_DOM_ElementNode_t*) context);
    NW_ASSERT (elementHandler != NULL);
    return NW_XHTML_ElementHandler_ProcessEvent (elementHandler, thisObj,
            (NW_DOM_ElementNode_t*) context,
            box, event);
    }

// -----------------------------------------------------------------------------
void
_NW_XHTML_ContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode, NW_Bool aDestroyBoxTree)
    {
    NW_XHTML_ContentHandler_t* thisObj;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                       &NW_XHTML_ContentHandler_Class));
    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (documentNode);

    /* stop the META-Refresh timer if it is running */
    NW_XHTML_ContentHandler_MetaRefreshDestroyTimer(thisObj);

    if (thisObj->processor)
        {
        NW_CSS_Processor_Suspend(thisObj->processor);
        }

    //reset the script nodes as the boxes in the scriptNode contexts are deleted
    if (thisObj->scriptNodes)
        {
        RPointerArray<NW_XHTML_Script_Node_Context_t>* scriptNodeList = STATIC_CAST(RPointerArray<NW_XHTML_Script_Node_Context_t>*,
                thisObj->scriptNodes);
        scriptNodeList->ResetAndDestroy();
        }

    //R->ecma
    //NW_HED_DocumentRoot_t* documentRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
    //(void) NW_Ecma_InvalidateFormsElements(documentRoot);

    /* invoke our superclass */
    NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.suspend(documentNode, aDestroyBoxTree);
    }
//------------------------------------------------------------------------------
void
_NW_XHTML_CompositeContentHandler_Resume (NW_HED_DocumentNode_t* documentNode)
    {
    NW_XHTML_ContentHandler_t* thisObj;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                       &NW_XHTML_ContentHandler_Class));
    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (documentNode);

    /* invoke our superclass */
    NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.resume(documentNode);

    thisObj->iScriptIsResumed = NW_TRUE;
    }

// -----------------------------------------------------------------------------
void
_NW_XHTML_ContentHandler_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    NW_Msg_Message_t* message = NULL;
    NW_Text_t* fragment = NULL;
    const NW_Text_t* url = NULL;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                       &NW_XHTML_ContentHandler_Class));

    /* call our super class to notify the children */
    NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.
    allLoadsCompleted (documentNode);

    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (documentNode);


    NW_TRY (status)
        {

        /* get the fragment */
        url = NW_HED_UrlRequest_GetUrl (NW_HED_ContentHandler_GetUrlRequest
                                        (thisObj));
        status = NW_XHTML_ContentHandler_GetFragment (thisObj, url,
                 (const NW_Text_t**) & fragment);

        /* show the frag */
        if ((status == KBrsrSuccess) && (fragment != NULL))
            {
            /*
            ** we need to use a message to display the fragment because we need
            ** to wait until the box-tree is laid out -- which is also done via
            ** a message.
            */

            /* create the message */
            message = NW_Msg_Message_New (NW_XHTML_ContentHandler_Msg_ShowFragment,
                                          fragment, NW_TRUE);
            NW_THROW_OOM_ON_NULL (message, status);

            fragment = NULL;

            /* dispatch it */
            status = NW_Msg_MessageDispatcher_DispatchMessage (&NW_Msg_MessageDispatcher, message, thisObj->iMessageListener);
            NW_THROW_ON_ERROR (status);

            message = NULL;
            }
        }
    NW_CATCH (status)
        {
        NW_Object_Delete (fragment);
        NW_Object_Delete (message);
        }
    NW_FINALLY {
    } NW_END_TRY
    }

// -----------------------------------------------------------------------------
void
_NW_XHTML_ContentHandler_LoseFocus (NW_HED_DocumentNode_t* documentNode)
    {
    NW_XHTML_ContentHandler_t* thisObj;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                       &NW_XHTML_ContentHandler_Class));
    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (documentNode);

    /* suspend the META-Refresh timer if it is running */
    NW_XHTML_ContentHandler_MetaRefreshSuspendTimer(thisObj);

    /* invoke our superclass */
    NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.loseFocus(documentNode);
    }

// -----------------------------------------------------------------------------
void
_NW_XHTML_ContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode)
    {
    NW_XHTML_ContentHandler_t* thisObj;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                       &NW_XHTML_ContentHandler_Class));
    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (documentNode);


    /* invoke our superclass */
    NW_HED_CompositeContentHandler_Class.NW_HED_DocumentNode.gainFocus(documentNode);

    /* resume the META-Refresh timer if it was running */
    NW_XHTML_ContentHandler_MetaRefreshResumeTimer(thisObj);
    }

// -----------------------------------------------------------------------------
// after loading last chunk of embedded content the function is called
// by documentroot.
// If the transactionId exits in scriptIds list , it is removed from the pendginglist
void
_NW_XHTML_DocumentNode_HandleLoadComplete(NW_HED_DocumentNode_t* documentNode,
        const NW_HED_UrlRequest_t *urlRequest,
        NW_Int16 errorClass,
        TBrowserStatusCode error,
        NW_Uint16 /*aTransactionId*/,
        void* context )
    {
    //check if the transaction is a script load request and decrement the counter
    NW_REQUIRED_PARAM(urlRequest);
    NW_REQUIRED_PARAM(errorClass);
    NW_REQUIRED_PARAM(error);
    NW_REQUIRED_PARAM(context);
	NW_REQUIRED_PARAM(documentNode);
	
    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                       &NW_XHTML_ContentHandler_Class));
    /* for convenience */
    NW_LOG0( NW_LOG_LEVEL2, "NW_XHTML_DocumentNode_HandleLoadComplete Start" );
    NW_LOG0( NW_LOG_LEVEL2, "NW_XHTML_DocumentNode_HandleLoadComplete End" );
   }


// -----------------------------------------------------------------------------
const NW_Text_t*
_NW_XHTML_ContentHandler_GetTitle (NW_HED_ContentHandler_t* contentHandler)
    {
    NW_XHTML_ContentHandler_t* thisObj;

    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (contentHandler);

    /* return the title */
    return thisObj->title;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_ContentHandler_ResolveURL (NW_HED_ContentHandler_t* thisObj,
                                     NW_Text_t* url, NW_Text_t** retUrl)
    {
    NW_Uint8 freeLoadUrl = NW_FALSE;
    NW_String_t baseUrl;
    NW_String_t href;
    NW_String_t contentLoc;
    NW_Ucs2* compositeBase = NULL;
    NW_DOM_Node_t* baseElement;
    NW_Ucs2 *contentLocation = NULL;
    void* docUrl = NULL;
    const NW_Ucs2* loadUrl = NULL;
    NW_Text_UCS2_t* absUrlText = NULL;
    NW_Ucs2* absUrl = NULL;
    NW_Bool isRelative;

    baseUrl.length = 0;
    baseUrl.storage = NULL;

    href.length = 0;
    href.storage = NULL;
    contentLoc.length = 0;
    contentLoc.storage = NULL;

    NW_TRY(status)
        {
        /* get the url as a ucs2 char string */
        loadUrl = NW_Text_GetUCS2Buffer (url, NW_Text_Flags_Aligned, NULL, &freeLoadUrl);
        NW_THROW_OOM_ON_NULL((void*)loadUrl, status);

        if(!NW_Text_GetCharCount(url))
            {
            isRelative = NW_TRUE;
            }
        else
            {
            /* if the supplied URL is absolute we simply return that */
            status = NW_Url_IsRelative (loadUrl, &isRelative);
            _NW_THROW_ON_ERROR (status);
            }
        if ( isRelative == NW_FALSE)
            {
            if (freeLoadUrl)
                {
                NW_Mem_Free ((NW_Ucs2*) loadUrl);
                }
            /*make a deep copy of the text */
            *retUrl = NW_Text_Copy(url, NW_TRUE);
            NW_THROW_OOM_ON_NULL(*retUrl, status);
            NW_Object_Delete (url);
            return KBrsrSuccess;
            }

        /* Get the the document's url. The result is not owned by us. */
        docUrl =
            NW_HED_UrlResponse_GetUrl( NW_HED_ContentHandler_GetUrlResponse( thisObj ) )->storage;
        NW_THROW_OOM_ON_NULL(docUrl, status);

        /*
        * If the URL was the result of a file operation, then we want the
        * response URL rather than
        * the request URL. This is to support save-to-file operations. If it wasn't the result of
        * a file operation, then the response URL should be the same as the request.
        */
        if (NW_Str_Strncmp((const NW_Ucs2*)docUrl, (const NW_Ucs2*)NW_STR("file://"), 7) == 0)
            {
            docUrl = NW_HED_UrlResponse_GetUrl
                (NW_HED_ContentHandler_GetUrlResponse (thisObj))->storage;
            NW_THROW_OOM_ON_NULL(docUrl, status);
            }

        /* get the base url as found in the "base" element */
        baseElement = NW_XHTML_ContentHandler_GetBaseElement(thisObj);

        /*get the content location id present*/
        contentLocation = NW_XHTML_ContentHandler_GetContentLocation(thisObj);

        if(contentLocation != NULL && baseElement == NULL)
            {

            status = StatusCodeConvert(NW_String_ucs2CharToString (&contentLoc, contentLocation, HTTP_iso_10646_ucs_2));
            NW_THROW_ON_ERROR (status);
            status = NW_Url_IsRelative (contentLocation, &isRelative);
            NW_THROW_ON_ERROR (status);
            if (isRelative == NW_TRUE)
                {
                status = NW_Url_RelToAbs ((const NW_Ucs2*)docUrl, (const NW_Ucs2 *) contentLoc.storage,
                    &compositeBase);
                NW_THROW_ON_ERROR (status);

                status = NW_HED_DomHelper_CatUcs2String (&baseUrl, compositeBase);
                NW_THROW_ON_ERROR (status);

                NW_Mem_Free (compositeBase);
                compositeBase = NULL;
                }
            /* otherwise use the base */
            else
                {
                status = NW_HED_DomHelper_CatString (&baseUrl, &contentLoc, HTTP_iso_10646_ucs_2);
                NW_THROW_ON_ERROR (status);
                }
            }


        if (baseElement != NULL)
            {
            status =
                NW_HED_DomHelper_GetHTMLAttributeValue
                (&((NW_XHTML_ContentHandler_t *) thisObj)->domHelper, baseElement,
                NW_XHTML_AttributeToken_href, &href);

            if (status == KBrsrSuccess)
                {
                /*
                * if the base element is a relative url the base-url is formed by cat'ing the
                * document's url with the base.
                */
                status = NW_Url_IsRelative ((const NW_Ucs2*) href.storage, &isRelative);
                NW_THROW_ON_ERROR (status);
                if (isRelative == NW_TRUE)
                    {
                    status = NW_Url_RelToAbs ((const NW_Ucs2*)docUrl, (const NW_Ucs2 *) href.storage,
                        &compositeBase);
                    NW_THROW_ON_ERROR (status);

                    status = NW_HED_DomHelper_CatUcs2String (&baseUrl, compositeBase);
                    NW_THROW_ON_ERROR (status);

                    NW_Mem_Free (compositeBase);
                    compositeBase = NULL;
                    }
                /* otherwise use the base */
                else
                    {
                    status = NW_HED_DomHelper_CatString (&baseUrl, &href, HTTP_iso_10646_ucs_2);
                    NW_THROW_ON_ERROR (status);
                    }

                NW_String_deleteStorage (&href);
                }
            }

            /*
            * if the base-url is still null just use the document's url as the
        * base-url */
        if (baseUrl.storage == NULL)
            {
            status = NW_HED_DomHelper_CatUcs2String (&baseUrl, (const NW_Ucs2*)docUrl);
            NW_THROW_ON_ERROR (status);
            }

        if(!NW_Str_Strlen(loadUrl))
            {
            absUrl = NW_Str_Newcpy((const NW_Ucs2*) baseUrl.storage);
            NW_THROW_OOM_ON_NULL(absUrl, status);
            }
        else
            {
            /* construct the absolute URL */
            status = NW_Url_RelToAbs ((const NW_Ucs2*) baseUrl.storage, loadUrl, &absUrl);
            NW_THROW_ON_ERROR (status);
            }

        absUrlText = NW_Text_UCS2_New (absUrl, 0, NW_Text_Flags_TakeOwnership);
        NW_THROW_OOM_ON_NULL(absUrlText, status);
        absUrl = NULL;

        /* fall through */

        }

        NW_CATCH (status)
            { /* empty */
            }

        NW_FINALLY {

            if (freeLoadUrl == NW_TRUE)
                {
                NW_Mem_Free ((NW_Ucs2*) loadUrl);
                }

            NW_String_deleteStorage (&baseUrl);
            NW_String_deleteStorage (&href);
            NW_String_deleteStorage (&contentLoc);
            NW_Mem_Free (absUrl);
            NW_Mem_Free (compositeBase);

            if (status == KBrsrSuccess)
                {
                NW_Object_Delete (url);
                }

            *retUrl = (NW_Text_t*) absUrlText;
            return status;
            }
        NW_END_TRY
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_ContentHandler_CreateBoxTree (NW_HED_ContentHandler_t* contentHandler,
                                        NW_LMgr_Box_t** boxTree)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    NW_DOM_Node_t* domNode;
    const NW_XHTML_ElementHandler_t* elementHandler;
    NW_LMgr_BidiFlowBox_t* bidiFlowBox = NULL;
    NW_LMgr_Property_t prop;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (contentHandler,
                                       &NW_XHTML_ContentHandler_Class));

    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (contentHandler);

    thisObj->ignoreFramesetElement = NW_FALSE;
    thisObj->ignoreBodyElement = NW_FALSE;

    NW_TRY (status)
        {
#ifdef _DEBUG1
            NW_HED_DomHelper_PrintTree(&thisObj->domHelper, NW_HED_DomTree_GetRootNode(thisObj->domTree));
#endif
        // if the box tree is Not null then update the box tree with the new content
        if (*boxTree)
            {

            status = NW_XHTML_ContentHandler_CreatePartialBoxTree(thisObj, boxTree);
            // Set the prevInitializedDom. This denotes the last node for which the
            // box tree was created and will be used as a starting point when the
            // next chunk comes
            _NW_THROW_ON_ERROR(status);
            }
        else
            {
            /* create a BidiFlowBox in which to build the box tree */
            bidiFlowBox = NW_LMgr_BidiFlowBox_New (1);
            NW_THROW_OOM_ON_NULL (bidiFlowBox, status);

            /* Set the display property on the container */
            prop.type = NW_CSS_ValueType_Token;
            prop.value.token = NW_CSS_PropValue_display_block;
            NW_LMgr_Box_SetProperty (NW_LMgr_BoxOf (bidiFlowBox),
                                     NW_CSS_Prop_display, &prop);

            /* The root node is the so-called document node and is basically
               a holder for some general document info such as encoding. */
            domNode = NW_HED_DomTree_GetRootNode (thisObj->domTree);
            NW_THROW_ON_NULL (domNode, status, KBrsrUnexpectedError);

            /* The actual content of the document starts with the root's children. */
            for (domNode = NW_DOM_Node_getFirstChild (domNode);
                 domNode != NULL ;
                 domNode = NW_DOM_Node_getNextSibling (domNode))
                {
                thisObj->prevInitializedDomNode = domNode;

                elementHandler = NW_XHTML_ContentHandler_GetElementHandler (thisObj,
                                 (NW_DOM_ElementNode_t*) domNode);
                NW_ASSERT(elementHandler != NULL);
                if (elementHandler != NULL)
                    {
                    status = NW_XHTML_ElementHandler_CreateBoxTree (elementHandler,
                             thisObj,
                             domNode,
                             NW_LMgr_ContainerBoxOf(bidiFlowBox));
                    _NW_THROW_ON_ERROR(status);
                    }
                }

            /* next is a little optimization to eliminate excess top-level nesting */
            if (NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(bidiFlowBox))
                == 1)
                {
                NW_LMgr_BidiFlowBox_t* tempFlowBox =
                    ((NW_LMgr_BidiFlowBox_t*)
                     NW_LMgr_ContainerBox_GetChild(NW_LMgr_ContainerBoxOf(bidiFlowBox), 0));
                if (NW_Object_IsClass(tempFlowBox, &NW_LMgr_BidiFlowBox_Class))
                    {
                    NW_LMgr_Box_Detach(NW_LMgr_BoxOf(tempFlowBox));
                    NW_Object_Delete(bidiFlowBox);
                    bidiFlowBox = tempFlowBox;
                    }
                }
            } // else
        }
    NW_CATCH (status)
        {
        if (bidiFlowBox)
            {
            NW_Object_Delete(bidiFlowBox);
            bidiFlowBox = NULL;
            }
        if (status != KBrsrOutOfMemory)
            {
            // ZAL: changed temporarily to distinguish create box
            // tree error from parser errors.
            status = KBrsrSavedPageFailed;
            }
        NW_HED_DocumentNode_HandleError( contentHandler,
                                         NW_HED_ContentHandler_GetUrlRequest(contentHandler),
                                         BRSR_STAT_CLASS_GENERAL, status);
        }
    NW_FINALLY
    {
#ifdef _DEBUG1
        NW_HED_DomHelper_PrintTree(&thisObj->domHelper, NW_HED_DomTree_GetRootNode(thisObj->domTree));
#endif
        if (bidiFlowBox)
        {
        *boxTree = NW_LMgr_BoxOf(bidiFlowBox);
            }
        // if the boxtree creation is exited , then thisObj->prevInitializedDomNode  is
        // set by the function call NW_XHTML_ContentHandler_BreakBoxTreeCreation(...)
        thisObj->prevInitializedDomNode = thisObj->lastInitializedDomNode;
        return status;
    }
    NW_END_TRY
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_ContentHandler_HandleRequest (NW_HED_ContentHandler_t* thisObj,
                                        NW_HED_UrlRequest_t* urlRequest)
    {
    TBrowserStatusCode status;

    NW_ASSERT (NW_HED_UrlRequestOf (urlRequest));

    /* don't handle reload requests here */
    if (NW_HED_UrlRequest_GetReason (urlRequest) == NW_HED_UrlRequest_Reason_ShellReload)
        {
        return KBrsrFailure;
        }

    /* don't handle post requests here */
    if (NW_HED_UrlRequest_GetMethod (urlRequest) == NW_URL_METHOD_POST)
        {
        return KBrsrFailure;
        }

    /* jump to the fragment if one is provided */
    status = NW_XHTML_ContentHandler_LoadFragment (NW_XHTML_ContentHandlerOf (
                 thisObj), urlRequest);

    return status;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
_NW_XHTML_ContentHandler_DocumentDisplayed (NW_HED_ContentHandler_t* contentHandler)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    
    /* for convenience */
    thisObj = NW_XHTML_ContentHandlerOf (contentHandler);

    NW_TRY (status)
        {
        if (thisObj->metaRefreshSet)
            {
            // Clear RefreshSet to only allow refresh once, in case URL is
            // invalid or some other type of error occurs.
            thisObj->metaRefreshSet = FALSE;
            if (thisObj->metaRefreshSeconds <= 0)
                {
                status = NW_XHTML_ContentHandler_MetaRefreshStartLoad(thisObj);
                _NW_THROW_ON_ERROR(status);
                }
            else
                {
                /* start a timer for the refresh delay */
                status = NW_XHTML_ContentHandler_MetaRefreshCreateTimer(thisObj);
                _NW_THROW_ON_ERROR(status);
                }
            }

		    const NW_Ucs2* rawUrl = NW_HED_UrlRequest_GetRawUrl (NW_HED_ContentHandler_GetUrlRequest(NW_HED_ContentHandlerOf(contentHandler)));
		    if(thisObj->metaNoCache)
			{
//R->ul			  UrlLoader_ClearItemInCache( rawUrl );
			}

			if(thisObj->metaCacheExpires == NW_TRUE)
			{
//R->ul				UrlLoader_SetHeaderInCache(rawUrl, (unsigned char*) expires, thisObj->metaExpiresDate);
			}


        NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);


        // check the content type of the response
        if ( thisObj->switchBackToNormalLayout )
            {
            // switch back to normal
            // but first make sure we have to switch at all as imagine the following case
            // 1. original status is normal
            // 2. first chunk cames in, we switch to ss mode ( content type is text/html )
            // 3. pi element handler disables ss mode ( mobile profile )
            // 4. formatting is carried on in normal mode
            // 5. execution comes here and we trying to switch to normal mode
            // 6. MHEDDocumentListener::SwitchLayout replaces boxes ( tables )
            //    so all the static table box will be transformed to vertical box eventhough
            //    we are in normal mode
            //    so change only if we are really really in ss mode.
            NW_Bool smallScreenMode = NW_Settings_GetVerticalLayoutEnabled();
            NW_Settings_SetInternalVerticalLayoutEnabled( NW_FALSE );
            if ( smallScreenMode )
                {
                (void) docRoot->documentListener->SwitchLayout (NW_FALSE, NW_TRUE /*reformatBoxTree*/);

                NW_CSS_Processor_t* processor = NW_XHTML_ContentHandler_GetCSSProcessor( thisObj );

                if ( processor != NULL )
                    {
                    NW_CSS_Processor_ProcessPendingLoads( processor );
                    }
                }
            thisObj->switchBackToNormalLayout = NW_FALSE;
            }
        }
    NW_CATCH (status)
        {}
    NW_FINALLY
    {
        return status;
    }
    NW_END_TRY
    }

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
const NW_XHTML_ElementHandler_t*
NW_XHTML_ContentHandler_GetElementHandler (NW_XHTML_ContentHandler_t* contentHandler,
        NW_DOM_ElementNode_t* domElement)
    {
    int i;
    int numModules;

    //  NW_LOG1(NW_LOG_LEVEL2, "contetnHandler: %x", contentHandler  );

    /* text nodes handled at content handler level */
    if (NW_DOM_Node_getNodeType (domElement) == NW_DOM_TEXT_NODE)
        {
        //     NW_LOG1(NW_LOG_LEVEL2, "NW_XHTML_TextElementHandler: %x", &NW_XHTML_TextElementHandler );
        return NW_XHTML_ElementHandlerOf (&NW_XHTML_TextElementHandler);
        }

    /* PI nodes handled at content handler level */
    if (NW_DOM_Node_getNodeType (domElement)
        == NW_DOM_PROCESSING_INSTRUCTION_NODE)
        {
        //     NW_LOG1(NW_LOG_LEVEL2, "NW_XHTML_PiElementHandler: %x", &NW_XHTML_PiElementHandler );
        return NW_XHTML_ElementHandlerOf (&NW_XHTML_PiElementHandler);
        }

    /* iterate through the module table */
    NW_Uint16 tagToken = NW_HED_DomHelper_GetElementToken (domElement);
    numModules = NW_XHTML_ContentHandler_GetClassPart (contentHandler).numModules;
    for (i = 0; i < numModules; i++)
        {
        NW_XHTML_ElementHandler_t* elementHandler;

        /* query the module for an appropriate ElementHandler */
        const NW_Markup_Module_t* module =
            NW_XHTML_ContentHandler_GetClassPart (contentHandler).modules[i];
        elementHandler = (NW_XHTML_ElementHandler_t*)
                         NW_Markup_WBXMLModule_GetHandler (module, tagToken);

        /* we found one, return it */
        if (elementHandler != NULL)
            {
            //       NW_LOG1(NW_LOG_LEVEL2, "ElementHandler: %x", elementHandler );
            return elementHandler;
            }
        }
    /* no ElementHandler was found, so we return the generic
       ElementHandler - this will at least allow us to traverse the
       children of this unknown node */
    //   NW_LOG1(NW_LOG_LEVEL2, "NW_XHTML_ElementHandler: %x", &NW_XHTML_ElementHandler );
    return &NW_XHTML_ElementHandler;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_GetDOMAttribute (NW_XHTML_ContentHandler_t* thisObj,
                          NW_DOM_ElementNode_t* elementNode,
                          const NW_Uint16 token, NW_Text_t** text_t)
    {
    TBrowserStatusCode status;
    NW_String_t value;
    NW_Text_t *attributeValue = NULL;

    /* get the attribute */
    status = NW_HED_DomHelper_GetHTMLAttributeValue
             (&thisObj->domHelper, elementNode, token, &value);

    /* convert it to a NW_Text */
    if (status == KBrsrSuccess)
        {
        attributeValue = NW_Text_New (HTTP_iso_10646_ucs_2, (void*) NW_String_getStorage (&value),
                                      NW_String_getCharCount (&value, HTTP_iso_10646_ucs_2),
                                      (NW_Uint8) (NW_String_getUserOwnsStorage (&value) ? NW_Text_Flags_TakeOwnership : 0));
        }
    *text_t = attributeValue;
    /* return the result */
    return status;
    }

// -----------------------------------------------------------------------------
NW_Text_t*
NW_XHTML_GetDOMTextNodeData(NW_XHTML_ContentHandler_t* thisObj,
                            NW_DOM_TextNode_t* textNode)
    {
    TBrowserStatusCode status;
    NW_String_t value;
    NW_Text_t *text = NULL;
    NW_Uint32 encoding;

    NW_REQUIRED_PARAM (thisObj);
    NW_ASSERT(textNode != NULL);

    /* extract the text from the element */
    status = NW_HED_DomHelper_GetText (&thisObj->domHelper, textNode,
                                       NW_TRUE, &value, &encoding);


    /* convert it to a NW_Text */
    if (status == KBrsrSuccess)
        {
        NW_Uint8 flag = 0;

        if (NW_String_getUserOwnsStorage (&value) == NW_TRUE)
            {
            flag = NW_Text_Flags_TakeOwnership;
            }

        text = NW_Text_New (encoding, value.storage, NW_String_getCharCount (&value,
                            encoding), flag);
        }

    return text;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_StartRequest(NW_XHTML_ContentHandler_t* thisObj,
                                     NW_HED_UrlRequest_t* urlRequest,
                                     void* clientData )
    {
    return NW_XHTML_ContentHandler_StartRequest_tId(thisObj, urlRequest, clientData , NULL);
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_StartRequest_tId(NW_XHTML_ContentHandler_t* thisObj,
        NW_HED_UrlRequest_t* urlRequest,
        void* clientData , NW_Uint16* aTransationId)
    {
    TBrowserStatusCode status;
    const NW_Ucs2* origUrl = NULL;
    NW_Ucs2* escapedUrl = NULL;
    NW_Text_t* resolvedUrl = NULL;
    NW_Text_t* retUrl = NULL;
    NW_HED_DocumentRoot_t* docRoot;
    NW_HED_DocumentNode_t* owner = NULL;
    NW_Url_Schema_t schema = NW_SCHEMA_INVALID;
    NW_Ucs2* resolvedUrlUcs2 = NULL;
    CBodyPart* bodyPart = NULL;
    TBool isUrlInMultipart = EFalse;

    NW_ASSERT (thisObj != NULL);
    NW_ASSERT (urlRequest != NULL);

    /* get the orig url as a ucs2 string */
    status = KBrsrOutOfMemory;
    NW_THROWIF_NULL (origUrl = NW_HED_UrlRequest_GetRawUrl (urlRequest));

    /* ensure that the url is escaped correctly */
    status = KBrsrOutOfMemory;

    NW_THROWIF_NULL (escapedUrl = NW_Str_New(NW_Url_EscapeUnwiseLen(origUrl)));
    NW_Url_EscapeUnwise(origUrl, escapedUrl);

    NW_Url_GetSchemeNoValidation(escapedUrl, &schema);

    /* resolve the url */
    status = KBrsrOutOfMemory;
    resolvedUrl = NW_Text_New (HTTP_iso_10646_ucs_2, escapedUrl, 0, NW_Text_Flags_TakeOwnership);
    NW_THROWIF_NULL (resolvedUrl);
    escapedUrl = NULL;


    if (schema != NW_SCHEMA_WTAI) {
      status = NW_HED_ContentHandler_ResolveURL (thisObj, resolvedUrl, &retUrl );
      _NW_THROW_ON_ERROR(status);

      /* give ownership back to resolvedUrl */
      resolvedUrl = retUrl;
      retUrl = NULL;
    }

    /* update the UrlRequest with the resolved url */
    NW_THROWIF_ERROR (status = NW_HED_UrlRequest_UpdateUrl (urlRequest, resolvedUrl));

    /* issue the request */
    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);

    // load owner is the docroot unless this is a non top level request
    owner = NW_HED_DocumentNodeOf( docRoot );
    if ( NW_HED_UrlRequest_IsTopLevelRequest( urlRequest ) != NW_TRUE )
        {
        owner = NW_HED_DocumentNodeOf( thisObj );
        }

    // compare url with body part
    NW_Uint8 freeNeeded;
    resolvedUrlUcs2 = NW_Text_GetUCS2Buffer( resolvedUrl, NULL, NULL, &freeNeeded);
    isUrlInMultipart = _NW_HED_CompositeContentHandler_IsUrlInMultipart(
                                             NW_HED_CompositeContentHandlerOf(thisObj),
                                                                       resolvedUrlUcs2, &bodyPart );
    if( freeNeeded )
        {
        NW_Mem_Free( resolvedUrlUcs2 );
        }

    if( isUrlInMultipart )
        {
        TDataType dataType( bodyPart->ContentType() );

        TUint8* charset = (TUint8*) bodyPart->Charset().Ptr();
        TInt lenCh = bodyPart->Charset().Length();
        CShell* shell = REINTERPRET_CAST(CShell*, NW_Ctx_Get(NW_CTX_BROWSER_APP, 0));
        NW_ASSERT(shell);
        TInt32 uidInt = shell->GetUidFromCharset( charset, lenCh );
        TUid uid = TUid::Uid( uidInt );
		//R
		/*
        CBrCtl* brCtl = shell->BrCtl();
        brCtl->LoadDataL( bodyPart->Url(), bodyPart->Body(), dataType, uid,
                          owner, clientData, NULL, aTransationId,
                          urlRequest->loadType, urlRequest->method );*/
        }
    else
        {
	    // Check if SendReferrerHeader is enabled
	    if ( NW_Settings_GetSendReferrerHeader() && docRoot->referrerUrl != NULL)
	    {   // Set Referrer Header
		    NW_Text_Length_t urlLength;
		    NW_Uint8 urlFreeNeeded = NW_FALSE;

		    // convert the url to Unicode and get the storage
		    NW_Ucs2* referrerUrl = (NW_Ucs2*) NW_Text_GetUCS2Buffer (docRoot->referrerUrl,
					    NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned,
					    &urlLength, &urlFreeNeeded);

		    SetReferrerHeader(urlRequest, referrerUrl);

		    if (urlFreeNeeded)
		    {
			    NW_Mem_Free(referrerUrl);
		    }
	    }

        status = NW_HED_DocumentRoot_StartRequest_tId (docRoot, owner, urlRequest, clientData , aTransationId);
        }

    NW_Object_Delete (resolvedUrl);
    resolvedUrl = NULL;

    return status;

_NW_CATCH:
    NW_CATCH_ERROR
    NW_Str_Delete (escapedUrl);
    NW_Object_Delete (resolvedUrl);

    return status;
    }
// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_StartLoad(NW_XHTML_ContentHandler_t* thisObj,
                                  const NW_Text_t* url,
                                  NW_Uint8 reason,
                                  void* clientData,
                                  NW_Uint8 loadType)
    {
    return NW_XHTML_ContentHandler_StartLoad_tId (thisObj, url, reason,
            clientData, loadType, NULL);

    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_StartLoad_tId (NW_XHTML_ContentHandler_t* thisObj,
                                       const NW_Text_t* url,
                                       NW_Uint8 reason,
                                       void* clientData,
                                       NW_Uint8 loadType, NW_Uint16* aTransationId)
    {

    NW_HED_UrlRequest_t* request;

    NW_ASSERT (thisObj != NULL);
    NW_ASSERT (url != NULL);

    NW_TRY (status)
        {
        /* make a UrlRequest */
        if (NW_HED_UrlRequest_Reason_DocLoadHead != reason)
            {
            request = NW_HED_UrlRequest_New (NW_TextOf (url), NW_URL_METHOD_GET, NULL,
                                             NULL, reason, NW_HED_UrlRequest_LoadNormal, loadType);
            }
        else
            {
            request = NW_HED_UrlRequest_New(NW_TextOf (url), NW_URL_METHOD_HEAD, NULL,
                                            NULL, reason, NW_HED_UrlRequest_LoadNormal, loadType);
            }

        NW_THROW_OOM_ON_NULL (request, status);

        if (NW_XHTML_ContentHandler_IsSaveddeck(thisObj) &&
            NW_HED_UrlRequest_Reason_DocLoadChild == reason &&
            // Allow to load a plugin from a saved page, because the user explicitly requested it
            loadType != NW_UrlRequest_Type_Plugin)
            {
            NW_HED_UrlRequest_SetCacheMode(request, NW_CACHE_ONLYCACHE);
            }

        if(NW_XHTML_ContentHandler_IsHistoryLoad(thisObj) &&
           NW_HED_UrlRequest_Reason_DocLoadChild == reason)
           {
           NW_HED_UrlRequest_SetCacheMode(request, NW_CACHE_HISTPREV);
           }

         if(NW_XHTML_ContentHandler_IsReLoad(thisObj) &&
           NW_HED_UrlRequest_Reason_DocLoadChild == reason)
           {
           NW_HED_UrlRequest_SetCacheMode(request, NW_CACHE_NOCACHE);
           }

		/* pass it to StartRequest to be satisfied */
        status = NW_XHTML_ContentHandler_StartRequest_tId (thisObj, request, clientData , aTransationId);
        _NW_THROW_ON_ERROR (status);
        }
    NW_CATCH (status)
        {
        if (request)
            {
                NW_Object_Delete(request);
            }
        }
    NW_FINALLY {
        return status;
    } NW_END_TRY
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_SetLoadObserver(NW_XHTML_ContentHandler_t* thisObj,
                                        NW_Uint16 aTransationId,
                                        MHEDLoadObserver* aLoadObserver)
    {
    NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*)
            NW_HED_DocumentNode_GetRootNode (thisObj);

    return NW_HED_DocumentRoot_SetLoadObserver(docRoot, aTransationId, aLoadObserver);
    }

// -----------------------------------------------------------------------------
NW_CSS_Processor_t*
NW_XHTML_ContentHandler_CreateCSSProcessor(NW_XHTML_ContentHandler_t* thisObj)
    {
    if (thisObj->processor == NULL)
        {
        thisObj->processor = NW_CSS_Processor_New();
        if (thisObj->processor != NULL)
            {
            NW_CSS_Processor_SetOwner(thisObj->processor, NW_HED_DocumentNodeOf(thisObj));
            NW_CSS_Processor_SetDictionary(thisObj->processor,
                                           (NW_WBXML_Dictionary_t *) & NW_XHTML_WBXMLDictionary);
            NW_CSS_Processor_SetApplyCSSCallback(thisObj->processor,
                                                 NW_XHTML_ElementHandler_ApplyCSSCallback);
            thisObj->processor->isSavedDeck = NW_XHTML_ContentHandler_IsSaveddeck(thisObj);
            thisObj->processor->isHistLoad = NW_XHTML_ContentHandler_IsHistoryLoad(thisObj);
			thisObj->processor->isReLoad = NW_XHTML_ContentHandler_IsReLoad(thisObj);

            }
        }
    return thisObj->processor;
    }

// -----------------------------------------------------------------------------
NW_WBXML_Dictionary_t*
NW_XHTML_ContentHandler_GetDictionary(NW_XHTML_ContentHandler_t* contentHandler)
    {
    NW_REQUIRED_PARAM(contentHandler);
    return (NW_WBXML_Dictionary_t *) & NW_XHTML_WBXMLDictionary;
    }

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_AddFormLiaison(
    NW_XHTML_ContentHandler_t* contentHandler,
    NW_DOM_ElementNode_t* formNode)
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_XHTML_FormLiaison_t* formLiaison;
    NW_XHTML_ContentHandler_t* thisObj = NW_XHTML_ContentHandlerOf (contentHandler);

    /* make sure we have a control set */
    if (contentHandler->controlSet == NULL)
        {
        /* no control set; make sure we have a value set */
        if (contentHandler->controlValueSet == NULL)
            {
            /* no value set; make an empty one */
            contentHandler->controlValueSet =
                (NW_ADT_DynamicVector_t*) NW_ADT_ResizableObjectVector_New(0, 5);
            if (contentHandler->controlValueSet == NULL)
                {
                return KBrsrOutOfMemory;
                }
            }

        contentHandler->controlSet =
            NW_XHTML_ControlSet_New(contentHandler->controlValueSet);
        if (contentHandler->controlSet == NULL)
            {
            return KBrsrOutOfMemory;
            }
        contentHandler->controlValueSet = NULL; /* controlSet now has ownership */
        }

    formLiaison = NW_XHTML_FormLiaison_New(formNode,
                                           contentHandler,
                                           contentHandler->controlSet);
    if (formLiaison == NULL)
        {
        return KBrsrOutOfMemory;
        }

    NW_XHTML_Form_Node_Context_t* formContext = new NW_XHTML_Form_Node_Context_t;
    //lint -e{774} Boolean within 'if' always evaluates to False
    if (formContext != NULL)
        {
        formContext->formNode = formNode;
        formContext->formLiaison = (NW_FBox_FormLiaison_t*)formLiaison;

        // append formContext to nodeContext vector
        if (NW_ADT_DynamicVector_InsertAt
            (thisObj->nodeContexts, &formContext, NW_ADT_Vector_AtEnd ) == NULL)
            {
            status = KBrsrOutOfMemory;
            }
        }
    else
        {
        status = KBrsrOutOfMemory;
        }


    if (status != KBrsrSuccess)
        {
        NW_Object_Delete(formLiaison);
        }

    return status;
    }

// -----------------------------------------------------------------------------
NW_FBox_FormLiaison_t*
NW_XHTML_ContentHandler_GetFormLiaison(NW_XHTML_ContentHandler_t* contentHandler,
                                       NW_DOM_ElementNode_t* elementNode)
    {
    NW_XHTML_FormLiaison_t* formLiaison = NULL;
    NW_DOM_ElementNode_t* node = NULL;
    NW_XHTML_ContentHandler_t* thisObj = NW_XHTML_ContentHandlerOf (contentHandler);


    if (elementNode != NULL)
        {
        node = NW_XHTML_ContentHandler_GetFormNode(contentHandler,elementNode);
        if (node != NULL)
            {
             //if we found the form, get the associated form liaison */
            NW_ADT_Vector_Metric_t nodeCtxSize = NW_ADT_Vector_GetSize(thisObj->nodeContexts);
            for (NW_ADT_Vector_Metric_t i = 0 ; i < nodeCtxSize; i++)
                {
                NW_XHTML_Form_Node_Context_t* formCtx =
                    *(NW_XHTML_Form_Node_Context_t**)NW_ADT_Vector_ElementAt (thisObj->nodeContexts, i);
                if (formCtx->formNode == node )
                    {
                    formLiaison = (NW_XHTML_FormLiaison_t*)formCtx->formLiaison;
                    break;
                    }
                }
            }
        }
    return NW_FBox_FormLiaisonOf(formLiaison);
    }


/* ------------------------------------------------------------------------- *
   NW_Markup_NumberCollector methods
 * ------------------------------------------------------------------------- */

// -----------------------------------------------------------------------------
NW_LMgr_Box_t*
_NW_XHTML_ContentHandler_NumberCollector_GetBoxTree (NW_Markup_NumberCollector_t*
        numberCollector)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    NW_LMgr_Box_t* boxTree;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (numberCollector,
                                       &NW_XHTML_ContentHandler_NumberCollector_Class));

    /* for convenience */
    thisObj = static_cast<NW_XHTML_ContentHandler_t*>
              (NW_Object_Aggregate_GetAggregator (numberCollector));
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_ContentHandler_Class));

    /* return the box tree */
    (void) NW_HED_DocumentNode_GetBoxTree (thisObj, &boxTree);
    return boxTree;
    }

/* ------------------------------------------------------------------------- *
   NW_Markup_NumberCollector methods
 * ------------------------------------------------------------------------- */


// -----------------------------------------------------------------------------
NW_Text_t*
_NW_XHTML_ContentHandler_NumberCollector_GetHrefAttr (NW_Markup_NumberCollector_t*
        numberCollector,
        NW_LMgr_Box_t* box)
    {
    NW_XHTML_ContentHandler_t* thisObj;
    NW_HED_EventHandler_t* eventHandler = NULL;
    NW_DOM_ElementNode_t* elementNode;
    NW_Text_t* text = NULL;
    NW_Text_t* retUrl = NULL;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (numberCollector,
                                       &NW_XHTML_ContentHandler_NumberCollector_Class));
    NW_ASSERT(NW_Object_IsInstanceOf(box, &NW_LMgr_ActiveContainerBox_Class));

    /* for convenience */
    thisObj = static_cast<NW_XHTML_ContentHandler_t*>
              (NW_Object_Aggregate_GetAggregator (numberCollector));
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_ContentHandler_Class));


    eventHandler = (NW_HED_EventHandler_t *)((NW_LMgr_ActiveContainerBoxOf(box))->eventHandler);
    elementNode = (NW_DOM_ElementNode_t *)(eventHandler->context);
    if(NW_LMgr_ActiveContainerBoxOf(box)->hrefAttr != NULL)
    {
      text = NW_Text_Copy(NW_LMgr_ActiveContainerBoxOf(box)->hrefAttr, NW_TRUE);
    }
    else
    {
      NW_XHTML_GetDOMAttribute (thisObj, elementNode, NW_XHTML_AttributeToken_href, &text);
    }
    if (text)
        {
        (void)NW_HED_ContentHandler_ResolveURL ((NW_HED_ContentHandler_t *)thisObj, text, &retUrl);
        text = retUrl;
        }
    return text;
    }

// -----------------------------------------------------------------------------
NW_Text_t*
_NW_XHTML_ContentHandler_NumberCollector_GetAltAttr (NW_Markup_NumberCollector_t*
        numberCollector,
        NW_LMgr_Box_t* box)
    {
    NW_LMgr_PropertyValue_t value;
    NW_XHTML_ContentHandler_t* thisObj;
    NW_Text_t* text = NULL;
    NW_DOM_ElementNode_t* elementNode;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (numberCollector,
                                       &NW_XHTML_ContentHandler_NumberCollector_Class));

    /* for convenience */
    thisObj = static_cast<NW_XHTML_ContentHandler_t*>
              (NW_Object_Aggregate_GetAggregator (numberCollector));
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_XHTML_ContentHandler_Class));

    value.object = NULL;
    (void)NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_elementId, NW_CSS_ValueType_Object,
                                       &value);
    if (value.object)
        {
        elementNode = (NW_DOM_ElementNode_t *)(value.object);
        NW_XHTML_GetDOMAttribute (thisObj, elementNode, NW_XHTML_AttributeToken_alt, &text);
        }
    return text;
    }

// -----------------------------------------------------------------------------
// NW_XHTML_ContentHandler_IsSaveddeck
// Description: Used for cache-only loading for embeded elements in saved file
//   (other items were commented in a header).
// Returns: NW_Bool: true if the xhtml content is retrieved from saved deck
//                   false, otherwise
// -----------------------------------------------------------------------------
//
NW_Bool
NW_XHTML_ContentHandler_IsSaveddeck(const NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_ASSERT (thisObj);
    return thisObj->saveddeck;
    }

// -----------------------------------------------------------------------------
// NW_XHTML_ContentHandler_IsHistoryLoad
// Description: Used for setting cache mode for embeded elements
//   (other items were commented in a header).
// Returns: NW_Bool: true if the xhtml content is result of history load
//                   false, otherwise
// -----------------------------------------------------------------------------
//
NW_Bool
NW_XHTML_ContentHandler_IsHistoryLoad(const NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_ASSERT (thisObj);
    return thisObj->histload;
    }


// -----------------------------------------------------------------------------
// NW_XHTML_ContentHandler_IsReLoad
// Description: Used for setting cache mode for embeded elements
//   (other items were commented in a header).
// Returns: NW_Bool: true if the xhtml content is result of reload
//                   false, otherwise
// -----------------------------------------------------------------------------
//
NW_Bool
NW_XHTML_ContentHandler_IsReLoad(const NW_XHTML_ContentHandler_t* thisObj)
    {
    NW_ASSERT (thisObj);
    return thisObj->reload;
    }

// -----------------------------------------------------------------------------
// NW_XHTML_ContentHandler_SetSaveddeck
// Description: set the saveddeck boolean member
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_XHTML_ContentHandler_SetSaveddeck(NW_XHTML_ContentHandler_t *thisObj, NW_Bool isSaveddeck)
    {
    NW_ASSERT (thisObj);
    thisObj->saveddeck = isSaveddeck;
    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// NW_XHTML_ContentHandler_SetHistoryLoad
// Description: set the histload boolean member
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_XHTML_ContentHandler_SetHistoryLoad(NW_XHTML_ContentHandler_t *thisObj, NW_Bool isHistLoad)
    {
    NW_ASSERT (thisObj);
    thisObj->histload = isHistLoad;
    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// NW_XHTML_ContentHandler_SetReLoad
// Description: set the reload boolean member
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_XHTML_ContentHandler_SetReLoad(NW_XHTML_ContentHandler_t *thisObj, NW_Bool isReLoad)
    {
    NW_ASSERT (thisObj);
    thisObj->reload = isReLoad;
    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
// NW_XHTML_ContentHandler_MetaRefreshInitialize
// Description: Initialize the META refresh. If and only if returns success, the
//   content handler object will take ownership of and assume responsibility for
//   destructing the resolvedUrl object.  Once initialized, the content handler
//   object performs the META refresh when needed.
// Returns: TBrowserStatusCode
// -----------------------------------------------------------------------------
//
TBrowserStatusCode
NW_XHTML_ContentHandler_MetaRefreshInitialize(NW_XHTML_ContentHandler_t* thisObj,
        NW_Int32 seconds,
        NW_Text_t* resolvedUrl,
        NW_Uint8 loadReason)
    {
    NW_ASSERT (thisObj);
    NW_ASSERT (resolvedUrl);

    /* Cannot be set more than once. */
    if (thisObj->metaRefreshSet)
        {
        return KBrsrUnexpectedError;
        }
    /* Save the variables for later. */
    thisObj->metaRefreshSet = NW_TRUE;
    thisObj->metaRefreshSeconds = seconds;
    thisObj->metaRefreshLoadReason = loadReason;
    thisObj->metaRefreshResolvedUrl = resolvedUrl; // We own the object now.
    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
void
NW_XHTML_ContentHandler_SetImageMapLookupTable(NW_XHTML_ContentHandler_t *thisObj,
        void* imageMapLookupTable)
    {
    thisObj->imageMapLookupTable = imageMapLookupTable;
    }

// -----------------------------------------------------------------------------
void
NW_XHTML_ContentHandler_SetReferrerUrl(NW_XHTML_ContentHandler_t *thisObj)
    {
    const NW_Text_t* url =
        NW_HED_UrlResponse_GetUrl( NW_HED_ContentHandler_GetUrlResponse(thisObj));

    NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*)
                                     NW_HED_DocumentNode_GetRootNode (thisObj);

    // The HTTP referrer header is being SET only if it is request from http or https.
    //If it is any other request then don't set this. For example, link from the static
    //file.

    if( (NW_Str_Strnicmp((const NW_Ucs2*) url->storage, (const NW_Ucs2*) NW_STR("http:"), 5) == 0) ||
        (NW_Str_Strnicmp((const NW_Ucs2*) url->storage, (const NW_Ucs2*) NW_STR("https:"), 6) == 0) )
    {
    NW_HED_DocumentRoot_SetDocumentReferrer(docRoot , url);
    }
    else
    {
     NW_HED_DocumentRoot_SetDocumentReferrer(docRoot , NULL);
    }
}

// -----------------------------------------------------------------------------
void*
NW_XHTML_ContentHandler_GetImageMapLookupTable(NW_XHTML_ContentHandler_t *thisObj)
    {
    return thisObj->imageMapLookupTable;
    }


// the function appends the transactionId to a list of pending script load
// requests. The function is called from scriptelementhandler after issuing
// a load request from external scripts
TBrowserStatusCode
NW_XHTML_ContentHandler_AddToPendingScriptTids(NW_XHTML_ContentHandler_t *thisObj, const TExternalScriptNode& aScriptNodeId )
    {
    TBrowserStatusCode status = KBrsrSuccess;
    TBool foundScriptIndex = EFalse;
    RArray<TExternalScriptNode>* scriptIds = STATIC_CAST(RArray<TExternalScriptNode>*, thisObj->scriptLoadTransactionIds);
    TInt length = scriptIds->Count();
    for (TInt index = 0 ; index < length ; index++)
        {
        TExternalScriptNode& scriptNode = (*scriptIds)[index];
        if (scriptNode.iTransactionId == aScriptNodeId.iTransactionId )
            {
            foundScriptIndex = ETrue;
            break;
            }
      }


    // append to the scriptTid list
    if (!foundScriptIndex)
        {
        TInt err = scriptIds->Append(aScriptNodeId);
        if ( err != KErrNone)
            {
            status = (err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure;
            }
        }
    return status;
    }
// the function returns the count of outstanding pending requests for external scripts
NW_Uint8
NW_XHTML_ContentHandler_PendingScriptLoadCount(NW_XHTML_ContentHandler_t *thisObj)
    {
    RArray<TExternalScriptNode>* scriptIds = STATIC_CAST(RArray<TExternalScriptNode>*, thisObj->scriptLoadTransactionIds);
    return (NW_Uint8)scriptIds->Count();
    }
// the function checks if the element node depends on the external scripts ,
// To check for dependency , check if there are any external script nodes that were before aElementNode
NW_Bool
NW_XHTML_ContentHandler_IsScriptLoadPending(NW_XHTML_ContentHandler_t *thisObj, NW_DOM_ElementNode_t* aElementNode)
    {
    NW_Bool depends = NW_FALSE;
    RArray<TExternalScriptNode>* scriptIds = STATIC_CAST(RArray<TExternalScriptNode>*, thisObj->scriptLoadTransactionIds);
    // if the pending script count is 0 then return NW_FALSE
    // check if the first script node in the pending list > aElementNode then return NW_FALSE, else NW_TRUE
    if(scriptIds->Count())
      {
      NW_DOM_ElementNode_t* scriptNode = (*scriptIds)[0].iScriptNode;
      if (scriptNode->source_offset > aElementNode->source_offset)
        {
        depends = NW_FALSE;
        }
      else
        {
        depends = NW_TRUE;
        }
      }
    else
      {
      depends = NW_FALSE;
      }
    return depends ;
    }
// add the form node to the document form nodes list
TBrowserStatusCode
NW_XHTML_ContentHandler_AddFormNodes(NW_XHTML_ContentHandler_t *aThisObj, NW_DOM_ElementNode_t* aElementNode)
  {
  TBrowserStatusCode status = KBrsrSuccess;
  RPointerArray<NW_DOM_ElementNode_t>* formNodes = (RPointerArray<NW_DOM_ElementNode_t>*)aThisObj->formNodes;
  TInt err = formNodes->Append(aElementNode);
  if ( err != KErrNone)
      {
      status = (err == KErrNoMemory) ? KBrsrOutOfMemory : KBrsrFailure;
      }
  return status;
  }

// Returns the form node for a given input element node. The mechanism used
// here to find the form node is to search through the array of form nodes in
// reverse, comparing the element's source offset with that of the form node
// until the form node's source offset is <= to the element's source offset.
//
// WARNING:
//
// This approach has a flaw in it. It assumes that the source offset is
// accurate; that all the items are inserted sequentially. However, this is not
// the case for script-generated forms. Script forms get parsed separately as
// though they are separate streams/files. The source offset then ends up being
// relative to the start of the script and not the top-level HTML. Hence, for
// these cases, the source offset is not reliable.
//
// One example use case as of this writing is the delete button of the inbox
// email for www.excite.com wasn't working because it has several forms on the
// page along with a script-generated form.
//
NW_DOM_ElementNode_t*
NW_XHTML_ContentHandler_GetFormNodeFromSourceOffset(
    NW_XHTML_ContentHandler_t *aThisObj, NW_DOM_ElementNode_t* aElementNode)
  {
  NW_DOM_ElementNode_t* node = NULL;
  // get all the form nodes in the document
  RPointerArray<NW_DOM_ElementNode_t>* formNodes = (RPointerArray<NW_DOM_ElementNode_t>*)aThisObj->formNodes;
  // check the under what umberall does the input element fall under
  for (TInt nodeIndex = formNodes->Count()-1; nodeIndex >=0; nodeIndex--)
      {
      // the first formNode to have the offset <= elementNode offset , it is the candidate
      NW_DOM_ElementNode_t* formNode = (*formNodes)[nodeIndex];
      if(formNode->source_offset <=  aElementNode->source_offset)
        {
        node = formNode;
        break;
        }
      }
  return node;
  }

// Returns the form node for a given input element node. The mechanism used
// here to find the form nodes is to walk up the DOM tree until the parent
// form node is found.
//
// WARNING:
//
// This approach assumes the DOM tree is accurate with the form node always
// being the parent node of its form elements - as it should be. However,
// under certain circumstances of poorly structured HTML, the current DOM tree
// processing does not place the form node as a parent of the form elements.
//
// One example use case of an incorrect DOM tree as of this writing is from the
// Nokia test suite:
//   testsuite.nokia-boston.com/s60accept/cannedSites/fedexCopy/fedex.html
//
NW_DOM_ElementNode_t*
NW_XHTML_ContentHandler_GetFormNodeFromDomTree(
    NW_XHTML_ContentHandler_t *aThisObj, NW_DOM_ElementNode_t* aElementNode)
  {
  NW_REQUIRED_PARAM(aThisObj);

  NW_DOM_ElementNode_t* node = aElementNode;

  // Walk up the DOM tree until parent form node is found or top is reached.
  while (node && (NW_DOM_Node_getNodeToken(node) != NW_XHTML_ElementToken_form))
        {
        node = NW_DOM_Node_getParentNode(node);
      }
  return node;
  }

// Returns the form node for a given input element node or NULL if none.
// An attempt is made to first get the form node by walking up the DOM tree.
// The DOM tree isn't always accurate (described above); hence, as a backup if
// the form node isn't found by walking up the DOM tree, then a second approach
// is used - search the form node array using source offset. This 2nd approach
// is also not always accruate (described above) and is even less reliable.
//
// Given that both mechanisms of finding the form node do not always work,
// there is the possibility that the wrong form node (or none) will be returned
// for some theoretical poorly structured HTML. If this should arise, the more
// rebust solution is to fix the DOM parsing such that form element nodes are
// always descendants of their form node.
//
NW_DOM_ElementNode_t*
NW_XHTML_ContentHandler_GetFormNode(NW_XHTML_ContentHandler_t *aThisObj, NW_DOM_ElementNode_t* aElementNode)
  {
  NW_DOM_ElementNode_t* node = NULL;

  node = NW_XHTML_ContentHandler_GetFormNodeFromDomTree(aThisObj, aElementNode);
  if (!node)
      {
      node = NW_XHTML_ContentHandler_GetFormNodeFromSourceOffset(aThisObj, aElementNode);
      }
  return node;
  }

// -----------------------------------------------------------------------------
CBase*
NW_XHTML_ContentHandler_GetElementHandlerInstance(NW_XHTML_ContentHandler_t *thisObj,
        NW_DOM_ElementNode_t* elementNode)
{
  TBrowserStatusCode    status;
  CBase*                value;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(elementNode != NULL);

  status = NW_ADT_Map_Get(thisObj->iElementHandlerMap, &elementNode, &value);

  if ((status == KBrsrSuccess) && (value != NULL)) {
    return value;
  }
  else {
    return NULL;
  }
}

// -----------------------------------------------------------------------------
TBrowserStatusCode
NW_XHTML_ContentHandler_SetElementHandlerInstance(NW_XHTML_ContentHandler_t *thisObj,
        NW_DOM_ElementNode_t* elementNode, CBase* elementHandler)
{
  CBase*  oldInstance;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(elementNode != NULL);

  // Delete the previous value
  oldInstance = NW_XHTML_ContentHandler_GetElementHandlerInstance(thisObj, elementNode);

  // Disallow adding an existing context
  if (oldInstance == elementHandler) {
    return KBrsrSuccess;
  }

  // Remove the previous context
  if (oldInstance) {
    (void) NW_ADT_Map_Remove(thisObj->iElementHandlerMap, &elementNode);

    delete oldInstance;
  }

  // Add the context
  return NW_ADT_Map_Set(thisObj->iElementHandlerMap, &elementNode, &elementHandler);
}

// -----------------------------------------------------------------------------
// Returns true if the media string passed to it contains one of the supported
// media types, all, handheld, or NULL.  The syntax is defined in section 6.13
// of the HTML 4.01 specification.
NW_Bool
NW_XHTML_ContentHandler_AllowableStylesheet(const NW_Ucs2* media, NW_Bool* isHandheld)
  {
  NW_Bool allowable = NW_FALSE;

  NW_ASSERT(isHandheld != NULL);

  *isHandheld = NW_FALSE;

  // A NULL value is acceptable.
  if (media == NULL)
    {
    return NW_TRUE;
    }

  do
    {
    const NW_Ucs2* valueEnd;
    NW_Uint32 len;

    // Skip leading white space.
    while (NW_Str_Isspace(*media))
      {
      media++;
      }

    // Scan forward until hit something other than alpha, digit, hyphen.
    valueEnd = media;
    while (NW_Str_Isalnum(*valueEnd) || (*valueEnd == (NW_Ucs2)'-'))
      {
      valueEnd++;
      }

    // Try to match (case insensitive) with one of accepted media type strings.
    len = valueEnd - media;

    if ((len == 3) && (NW_Str_StrnicmpConst(media, "all", 3) == 0))
      {
      allowable = NW_TRUE;
      }

    if ((len == 8) && (NW_Str_StrnicmpConst(media, "handheld", 8) == 0))
      {
      *isHandheld = NW_TRUE;
      allowable = NW_TRUE;
      }

    // If doesn't match, scan forward for comma and if found repeat.
    while (*valueEnd)
      {
      if (*valueEnd == ',')
        {
        valueEnd++;
        break;
        }

      valueEnd++;
      }

    media = valueEnd;
    } while (*media);

  return allowable;
  }

// -----------------------------------------------------------------------------
// Enables mobile profile mode by disabling small screen layout and loading any
// pending external stylesheets.
void NW_XHTML_ContentHandler_EnableMobileProfileMode(NW_XHTML_ContentHandler_t* thisObj)
  {
  NW_CSS_Processor_t* processor = NULL;

  NW_ASSERT(thisObj != NULL);

  NW_Settings_SetDisableSmallScreenLayout(NW_TRUE);

  processor = NW_XHTML_ContentHandler_GetCSSProcessor(thisObj);
  if(processor != NULL)
    {
    NW_CSS_Processor_ProcessPendingLoads(processor);
    }
  }

// -----------------------------------------------------------------------------
// Load or queue a request for an external stylesheet.
TBrowserStatusCode
NW_XHTML_ContentHandler_LoadStylesheet(NW_XHTML_ContentHandler_t* thisObj,
    const NW_Ucs2* type, const NW_Ucs2* relation, const NW_Ucs2* media,
    const NW_Ucs2* url, const NW_Ucs2* charset, const NW_Ucs2* alternate)
  {
  NW_Bool              isHandheld;
  NW_Bool              allowable;
  NW_CSS_Processor_t*  processor = NULL;
  NW_Text_t*           urlText = NULL;
  NW_Text_t*           resolvedUrl = NULL;
  NW_Text_t*           charsetText = NULL;

  NW_ASSERT(thisObj != NULL);

  NW_TRY (status)
    {
    // Verify that the url isn't NULL.
    if (url == NULL)
      {
      NW_THROW_STATUS(status, KBrsrFailure);
      }

    // Verify that the type is text/css.
    if ((type != NULL) && (NW_Str_StricmpConst(type, "text/css") != 0))
      {
      NW_THROW_STATUS(status, KBrsrFailure);
      }

    // Verify that the rel is stylesheet.
    if ((relation != NULL) && (NW_Str_StricmpConst(relation, "stylesheet") != 0))
      {
      NW_THROW_STATUS(status, KBrsrFailure);
      }

    // Verify that the alternate is no.
    if ((alternate != NULL) && (NW_Str_StricmpConst(alternate, "no") != 0))
      {
      NW_THROW_STATUS(status, KBrsrFailure);
      }

    // Verify that the media is allowable.
    if (media != NULL)
      {
      allowable = NW_XHTML_ContentHandler_AllowableStylesheet(media, &isHandheld);

      if (!allowable)
        {
        NW_THROW_STATUS(status, KBrsrFailure);
        }

      // If a "handheld" stylesheet is found switch to mobile profile mode.
      if (isHandheld)
        {
        NW_XHTML_ContentHandler_EnableMobileProfileMode(thisObj);
        }
      }

    // Resolve the url.
    urlText = (NW_Text_t*) NW_Text_UCS2_New((void *) url, 0, 0);
    NW_THROW_OOM_ON_NULL (urlText, status);

    status = NW_HED_ContentHandler_ResolveURL(thisObj, urlText, &resolvedUrl);
    NW_THROW_ON_ERROR(status);
    urlText = NULL;

    // Wrap the charset
    if (charset != NULL)
      {
      charsetText = (NW_Text_t*) NW_Text_UCS2_New((void *) charset, 0, 0);
      NW_THROW_OOM_ON_NULL(charsetText, status);
      }

    // Get the CSS processor
    processor = NW_XHTML_ContentHandler_CreateCSSProcessor(thisObj);
    NW_THROW_ON_NULL(processor, status, KBrsrUnexpectedError);

    // Load or queue the stylesheet request.
    status = NW_CSS_Processor_Load(processor, resolvedUrl, charsetText,
        NW_XHTML_ContentHandler_IsSaveddeck(thisObj), NW_XHTML_ContentHandler_IsHistoryLoad(thisObj),
        NW_XHTML_ContentHandler_IsReLoad(thisObj));
    NW_THROW_ON_ERROR(status);

    if (resolvedUrl)
      {
      NW_Object_Delete(resolvedUrl);
      }
    resolvedUrl = NULL;
    charsetText = NULL;
  }

  NW_CATCH (status)
    {
    NW_Object_Delete(urlText);
    NW_Object_Delete(resolvedUrl);
    NW_Object_Delete(charsetText);
    }

  NW_FINALLY
    {
    return status;
    } NW_END_TRY
  }



TBrowserStatusCode
NW_XHTML_ContentHandler_WindowOpen(NW_XHTML_ContentHandler_t* thisObj,
                                   NW_Text_t* url, NW_Text_t* target)
    {
    NW_HED_DocumentRoot_t* docRoot = NULL;
    TBrowserStatusCode status = KBrsrSuccess;
    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
    NW_ASSERT(docRoot != NULL);
    NW_Uint32 length;
    NW_Bool urlFreeNeeded = NW_FALSE;
    NW_Bool targetFreeNeeded = NW_FALSE;
    NW_Ucs2* urlStorage = NW_Text_GetUCS2Buffer (url, NW_Text_Flags_Aligned,
                                  &length, &urlFreeNeeded);
    NW_Ucs2* targetStorage = NW_Text_GetUCS2Buffer (target, NW_Text_Flags_Aligned,
                                  &length, &targetFreeNeeded);

    if (urlStorage == NULL || targetStorage == NULL)
        {
        status = KBrsrOutOfMemory;
        }
    else
        {
        TRAP(status, docRoot->documentListener->WindowOpenL(urlStorage, targetStorage, NW_Ecma_Evt_OnClick));
        if (status != KBrsrSuccess)
            status = KBrsrOutOfMemory;
        }
    if (urlFreeNeeded)
        {
        NW_Mem_Free(urlStorage);
        }
    if (targetFreeNeeded)
        {
        NW_Mem_Free(targetStorage);
        }
    return status;
    }

TBrowserStatusCode
NW_XHTML_ContentHandler_WindowOpenAndPost(NW_XHTML_ContentHandler_t* thisObj,
    NW_Text_t* url, NW_Text_t* target, NW_Uint8* contentType,
    NW_Uint32 contentTypeLength, NW_Uint8* postData, NW_Uint32 postDataLength,
    NW_Uint8* boundary)
    {
    NW_HED_DocumentRoot_t* docRoot = NULL;
    TBrowserStatusCode status = KBrsrSuccess;
    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
    NW_ASSERT(docRoot != NULL);
    NW_Uint32 length;
    NW_Bool urlFreeNeeded = NW_FALSE;
    NW_Bool targetFreeNeeded = NW_FALSE;
    NW_Ucs2* urlStorage = NW_Text_GetUCS2Buffer (url, NW_Text_Flags_Aligned,
                                  &length, &urlFreeNeeded);
    NW_Ucs2* targetStorage = NW_Text_GetUCS2Buffer (target, NW_Text_Flags_Aligned,
                                  &length, &targetFreeNeeded);

    if (urlStorage == NULL || targetStorage == NULL)
        {
        status = KBrsrOutOfMemory;
        }
    else
        {
        NW_ASSERT(postData != NULL);
        TPtrC8 contentTypePtr (contentType, contentTypeLength);
        TPtrC8 postDataPtr (postData, postDataLength);
        TPtrC8 boundaryPtr(NULL, 0);
        TDesC8* ptrForBoundary = NULL;
        if (boundary != NULL)
            {
            boundaryPtr.Set(boundary, User::StringLength(boundary));
            ptrForBoundary = &boundaryPtr;
            }
        TRAP(status, docRoot->documentListener->WindowOpenAndPostL(urlStorage, targetStorage, NW_Ecma_Evt_OnClick,
            TBrCtlDefs::EMethodPost, &contentTypePtr, &postDataPtr, ptrForBoundary, NULL));
        if (status != KBrsrSuccess)
            status = KBrsrOutOfMemory;
        }
    if (urlFreeNeeded)
        {
        NW_Mem_Free(urlStorage);
        }
    if (targetFreeNeeded)
        {
        NW_Mem_Free(targetStorage);
        }
    return status;
    }

/*
 Get the target in base tag defined in header, this value can be used to decide if to
 open a link in a new window or not
*/
void
NW_XHTML_ContentHandler_GetBaseTagTarget (NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_Text_t** targetVal)
	{
	NW_DOM_Node_t* baseElement;
	NW_Text_t* target = NULL;

	NW_ASSERT (targetVal);

	/* get the base url as found in the "base" element */
	baseElement = NW_HED_DomHelper_FindElement (&contentHandler->domHelper, NW_HED_DomTree_GetRootNode (
	contentHandler->domTree), 3, NW_XHTML_ElementToken_base, 0, NULL);

	if (baseElement != NULL)
		{
		NW_XHTML_GetDOMAttribute (contentHandler, baseElement, NW_XHTML_AttributeToken_target, &target);
		if (target != NULL && target->characterCount == 0)
			{
			NW_Object_Delete(target);
			target = NULL;
			}
		if (target != NULL)
			{
			*targetVal = (NW_Text_t*) NW_Text_UCS2_New (target->storage, NW_Text_GetCharCount(target), NW_Text_Flags_TakeOwnership);
			}
		}
	}
