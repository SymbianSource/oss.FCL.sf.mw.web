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
* Description:  
*
*/
 
// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <flogger.h>

#include <stddef.h>
#include <stdio.h>

// User Includes
#include "nwx_string.h" 
#include "ecmadll.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "nw_lmgr_statictablebox.h"
#include "nw_lmgr_verticaltablebox.h"
#include "nw_lmgr_bidiflowbox.h"
#include "nw_basictables_trelementhandler.h"
#include "nw_basictables_captionelementhandler.h"
#include "nw_basictables_rowgroupelementhandler.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_basicforms_formelementhandler.h"
#include "nw_structure_scriptelementhandler.h"
#include "nwx_settings.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "nw_basicforms_inputelementhandler.h"
#include "nw_structure_scriptelementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_htmlp_to_wbxml.h"
#include "nw_lmgr_slavepropertylist.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nw_text_ucs2.h"
#include <nw_dom_text.h>
#include "nw_dom_domvisitor.h"
#include <nw_tinydom.h>
#include <nw_dom_element.h>
#include <nw_dom_document.h>

#include "nw_ecma_contenthandler.h"
#include "nw_lmgr_textbox.h"
#include "nwx_http_defs.h"
#include "nwx_logger.h"

_LIT(KScriptLanguage, "javascript");
_LIT(KScriptType1, "text/javascript");
_LIT(KScriptType2, "text/ecmascript");
_LIT(KScriptType3, "application/x-javascript");

/* ------------------------------------------------------------------- */
// returns KBrsrSuccess if the language is supported
// returns KBrsrNotImplemented if the language is not supported
static
TBrowserStatusCode
NW_XHTML_isScriptLanguageSupported(NW_XHTML_ContentHandler_t* contentHandler,
                                   NW_DOM_ElementNode_t* elementNode)
    {
    NW_Text_t* language = NULL;
    NW_Ucs2* langPtr = NULL;


    /* if language is either NULL or starts with javascript then process the script
     else ignore the script tag*/
    TInt status = NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                                            NW_XHTML_AttributeToken_language, &language);
    if (status == KBrsrSuccess )
        {
        //if the language attr is not found then the default is javascript
        // if the language attr is not NULL and starts with 'javascript' then process the script tag
        if (NW_Text_GetCharCount(language) >= (TUint)KScriptLanguage().Length())
            {
            langPtr = NW_Text_GetUCS2Buffer (language, NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                                             NULL, NULL);
            if (langPtr)
                {
                TPtr languageDes(langPtr, KScriptLanguage().Length(), KScriptLanguage().Length());
                languageDes.LowerCase();
                if (languageDes.Compare(KScriptLanguage)) // if not equal
                    {
                    status = KBrsrNotImplemented;
                    }
                }
            else
                {
                status = KBrsrOutOfMemory;
                }
            }
        else
            {
            status = KBrsrNotImplemented;
            }
        }
    //if the language attr is not found then the default language type is javascript
    if (status == KBrsrNotFound)
        {
        status = KBrsrSuccess ;
        }

    if (language )
        {
        NW_Object_Delete (language );
        }
    if (langPtr )
        {
        NW_Mem_Free(langPtr );
        }
    return status ;
    }

/* ------------------------------------------------------------------- */
// returns KBrsrSuccess if the script  is supported
static
TBrowserStatusCode
NW_XHTML_isScriptContentSupported(NW_XHTML_ContentHandler_t* contentHandler,
                                  NW_DOM_ElementNode_t* elementNode)
    {
    NW_Text_t* scriptType = NULL;
    NW_Ucs2* scriptTypePtr = NULL;
    NW_Text_Length_t scriptTypePtrLen ;

    // if the script type is either text/javascrip or text/ecmascript then the script is
    // is supported . If script type is not available then check for the language attr.
    TInt status = NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                                            NW_XHTML_AttributeToken_type, &scriptType);
    if (status == KBrsrSuccess )
        {
        scriptTypePtr = NW_Text_GetUCS2Buffer (scriptType, NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                                               &scriptTypePtrLen , NULL);
        if (scriptTypePtr)
            {
            TPtr scriptTypeDes(scriptTypePtr , scriptTypePtrLen, scriptTypePtrLen);
            scriptTypeDes.LowerCase();
            // check if scriptType is either "text/javascript" or "text/ecmascript"
            if (scriptTypeDes.Compare(KScriptType1) && scriptTypeDes.Compare(KScriptType2) && scriptTypeDes.Compare(KScriptType3) )
                {
                // change the byte order , sometimes NW_XHTML_GetDOMAttribute gets in bigendian or little endian - unpredicable
                NW_Uint16 c_ucs2;
                NW_Uint32 i = 0;
                NW_Byte* data = (NW_Byte*)scriptTypePtr;
                for (i = 0; i < 2*scriptTypePtrLen; i += 2)
                    {
                    /* this is a clever trick: pick up bytes in big endian order,
                    force the result to memory via taking address of result which
                    will put the 16-bits into native byte order, then copy the
                      result over the original bytes */
                    c_ucs2 = (NW_Uint16)((data[i] << 8) | data[i + 1]);
                    (void)NW_Mem_memcpy(data + i, &c_ucs2, sizeof(NW_Uint16));
                    }
                if (scriptTypeDes.Compare(KScriptType1) && scriptTypeDes.Compare(KScriptType2) && scriptTypeDes.Compare(KScriptType3) )
                  {
                  status = KBrsrNotImplemented;
                  }
                }
            }
        else
            {
            status = KBrsrOutOfMemory;
            }
        }
    //if the script type is not found then check for the language attribute
    if (status == KBrsrNotFound )
        {
        status = NW_XHTML_isScriptLanguageSupported(contentHandler, elementNode);
        }
    if (scriptType )
        {
        NW_Object_Delete (scriptType );
        }
    if (scriptTypePtr )
        {
        NW_Mem_Free(scriptTypePtr );
        }
    return status ;
    }
/* ------------------------------------------------------------------- */
// returns KBrsrSuccess if OK, returns KBrsrOutOfMemory if OOM

static
TBrowserStatusCode
NW_XHTML_ExecuteScript (const NW_XHTML_ElementHandler_t* elementHandler,
                        NW_XHTML_ContentHandler_t* contentHandler,
                        NW_DOM_ElementNode_t* elementNode,
                        NW_Ecma_ContentHandler_t** retEcmaCH)
    {
    NW_DOM_Node_t* childNode;
    NW_Text_t* script = NULL;
    NW_Ecma_ContentHandler_t* ecmaCH = NULL;
    NW_Ucs2* scriptUcs2 = NULL;

    /* parameter assertion block */
    NW_ASSERT (NW_Object_IsInstanceOf (elementHandler,
                                       &NW_XHTML_ElementHandler_Class));
    NW_ASSERT (NW_Object_IsInstanceOf (contentHandler,
                                       &NW_XHTML_ContentHandler_Class));
    NW_ASSERT (elementNode != NULL);
    NW_REQUIRED_PARAM(elementHandler);

    NW_TRY (status)
        {
      if(contentHandler->iScriptIsResumed )
      {
        NW_HED_CompositeNode_t* compositeNode;
        NW_HED_ContentHandler_t* ch = NULL;
          /* get the associated content handler if any */
        compositeNode = (NW_HED_CompositeNode_t*)
        NW_Object_QueryAggregate (contentHandler, &NW_HED_CompositeNode_Class);
        NW_ASSERT (compositeNode != NULL);
        ch = (NW_HED_ContentHandler_t*) NW_HED_CompositeNode_LookupChild (compositeNode, elementNode);
        //R->ecma *retEcmaCH= NW_Ecma_ContentHandlerOf(ch);
        status = KBrsrSuccess;
      }
      else
      {
        NW_LOG0( NW_LOG_OFF, "NW_XHTML_scriptElementHandler_ExecuteScript Updated" );

          childNode = NW_DOM_Node_getFirstChild(elementNode);
        NW_THROW_ON_NULL(childNode, status, KBrsrSuccess);

        if (NW_DOM_Node_getNodeType(childNode) == NW_DOM_TEXT_NODE)
            {
            NW_Text_Length_t length;

            script = NW_XHTML_GetDOMTextNodeData(contentHandler, childNode);
            NW_THROW_OOM_ON_NULL (script, status);

            length = NW_Text_GetCharCount(script);

            if (length > 1)
                {

                scriptUcs2=
                    NW_Text_GetUCS2Buffer (script, NW_Text_Flags_NullTerminated | NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                                           NULL, NULL);
                NW_THROW_OOM_ON_NULL (scriptUcs2, status);
                NW_Object_Delete(script);
                script = NULL;

                // quick fix: ecma content handler sometimes is constructed twice on the same element node
                // due to a bug in the xhtmlcontent handler lastInitializedDomNode vs. prevInitializedDomNode case.
                // this is just to prevent the double init (memory leak). The proper fix would be to change the xhtmch but
                // as the error scenario cannot be reproduced anymore, I can neither debug it nor fix it.
                NW_HED_CompositeNode_t* compositeNode;

                compositeNode = (NW_HED_CompositeNode_t*)
                  (NW_Object_QueryAggregate( NW_HED_DocumentNodeOf(contentHandler), &NW_HED_CompositeNode_Class));
                NW_ASSERT( compositeNode != NULL );
                // create new ecma content handler unless xhtml content handler already has it
                if( !NW_HED_CompositeNode_LookupChild( compositeNode, elementNode ) )
                  {
                //R->ecma
                //ecmaCH = NW_Ecma_ContentHandler_New(NW_HED_DocumentNodeOf(contentHandler), NULL, elementNode);
                NW_THROW_OOM_ON_NULL (ecmaCH, status);
                ecmaCH->script = scriptUcs2;
                scriptUcs2 = NULL; //the ownership of the pointer is now ecmaCH 
                *retEcmaCH = ecmaCH;
                status = NW_HED_DocumentNode_Initialize (NW_HED_DocumentNodeOf(ecmaCH), KBrsrSuccess);
                NW_THROW_ON_ERROR(status);
                status = NW_HED_CompositeContentHandler_AddChild( NW_HED_CompositeContentHandlerOf( contentHandler),
                           NW_HED_DocumentNodeOf( ecmaCH), elementNode);
                NW_THROW_ON_ERROR(status);
                  }
                }
            }
        }
    }
    NW_CATCH (status)
        {
        NW_Object_Delete(script);
        }
    NW_FINALLY {
        return status;
    } NW_END_TRY

    }
/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_scriptElementHandler_Class_t NW_XHTML_scriptElementHandler_Class =
    {
        { /* NW_Object_Core               */
            /* super                        */ &NW_XHTML_ElementHandler_Class,
            /* queryInterface               */ _NW_Object_Core_QueryInterface
        },
        { /* NW_XHTML_ElementHandler      */
            /* initialize                   */ _NW_XHTML_scriptElementHandler_Initialize,
            /* createBoxTree                */ _NW_XHTML_scriptElementHandler_CreateBoxTree,
            /* processEvent                 */ _NW_XHTML_ElementHandler_ProcessEvent
        },
        { /* NW_XHTML_scriptElementHandler */
            /* unused                       */ 0
        }
    };

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_scriptElementHandler_t NW_XHTML_scriptElementHandler =
    {
        {
            {
                &NW_XHTML_scriptElementHandler_Class
            }
        }
    };

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_scriptElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
        NW_XHTML_ContentHandler_t* contentHandler,
        NW_DOM_ElementNode_t* elementNode)
    {
    NW_Text_t* url = NULL;
    (void) elementHandler;
    NW_LOG0( NW_LOG_OFF, "NW_XHTML_scriptElementHandler_ExecuteScript Udpar" );

    NW_TRY (status)
        {
        if (!NW_Settings_GetEcmaScriptEnabled())
            {
            NW_THROW_SUCCESS (status);
            }

        // special handling if script is the last node of the chunk:
        // leave it to be processed in the next chunk
        if (!NW_XHTML_ElementHandler_NextSiblingExists (contentHandler, elementNode, NW_TRUE))
            {
            NW_THROW_SUCCESS (status);
            }

        status = NW_XHTML_isScriptContentSupported(contentHandler, elementNode);
        if (status != KBrsrSuccess)
            {

            //      must render content inside <noscript> whenever one of the
            //      following conditions is true:
            //      1) scripting is disabled
            //      2) scripting was not included in the build
            //      3) an unsupported <script> type "e.g. "VBScript" was
            //      encountered (subsequent, but not previous, <noscript>'s must be rendered).

            if (status == KBrsrNotImplemented)
                {
                contentHandler->ignoreNoScriptElement = NW_TRUE;
                status = KBrsrSuccess;
                }
            NW_THROW (status);
            }


        contentHandler->ignoreNoScriptElement = NW_TRUE;
        /* find the 'src' attribute and get its string value */
        status = NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                                           NW_XHTML_AttributeToken_src, &url);

        if (status == KBrsrNotFound)
            {
            status = KBrsrSuccess;
            }
        else
            {
            if (url)
                {
                TExternalScriptNode scriptNodeId ;
                
                NW_Uint16 aTransationId = 0;
                /* load the script*/
                status = NW_XHTML_ContentHandler_StartLoad_tId (contentHandler,
                         url, NW_HED_UrlRequest_Reason_DocLoadChild, elementNode, NW_UrlRequest_Type_JavaScript, &aTransationId);
                scriptNodeId.iScriptNode = elementNode;
                scriptNodeId.iTransactionId= aTransationId ;
                // if the document load is complete , it is not necessary to track the script
                // load as all the inline scripts are processed. The external script load request
                // can take its sweet load time to load and process.
                if ( status == KBrsrSuccess && !contentHandler->scriptLoadComplete)
                    {
                    status = NW_XHTML_ContentHandler_AddToPendingScriptTids(contentHandler, scriptNodeId);
                    }

                /* cleanup after ourselves */
                NW_Object_Delete (url);
                }
            else
                {
                status = KBrsrOutOfMemory;
                }
            }
        }
    NW_CATCH (status)
        {}
    NW_FINALLY
    {
        return status;
    } NW_END_TRY
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_scriptElementHandler_ExecuteScript (const NW_XHTML_ElementHandler_t* elementHandler,
        NW_XHTML_ContentHandler_t* contentHandler,
        NW_DOM_ElementNode_t* elementNode,
        NW_LMgr_ContainerBox_t* parentBox,
        NW_Bool* aBoxTreeUpdated)
    {
    NW_Ecma_ContentHandler_t* ecmaCH = NULL;
    NW_LMgr_ContainerBox_t *placeHolderBox = NULL;
    NW_XHTML_Script_Node_Context_t* scriptNodeContext = NULL;
    NW_LOG0( NW_LOG_OFF, "NW_XHTML_scriptElementHandler_ExecuteScript START" );

    NW_TRY (status)
        {
        // set ecma script flag on. This flag must be set back to false
        // right after the script finished -at the end of this function
        // so pay attention to it if you introduce any new !!!return!!!!
        contentHandler->iScriptIsExecuted = NW_TRUE;

        // if the script is not supported return
        if (!NW_Settings_GetEcmaScriptEnabled())
            {
            NW_THROW_SUCCESS (status);
            }

        // special handling if script is the last node of the chunk:
        // leave it to be processed in the next chunk
        if (!NW_XHTML_ElementHandler_NextSiblingExists (contentHandler, elementNode, NW_FALSE))
            {
            NW_THROW_SUCCESS (status);
            }

        status = NW_XHTML_isScriptContentSupported(contentHandler, elementNode);
        NW_THROW_ON (status, KBrsrOutOfMemory );

        if (status != KBrsrSuccess)
            {
            
            // must render content inside <noscript> whenever one of the 
            // following conditions is true:
            // 1) scripting is disabled 
            // 2) scripting was not included in the build
            // 3) an unsupported <script> type "e.g. "VBScript" was 
            // encountered (subsequent, but not previous, <noscript>'s must be rendered).
            
            if (status == KBrsrNotImplemented)
                {
                contentHandler->ignoreNoScriptElement = NW_FALSE;
                }
            NW_THROW_SUCCESS (status);
            }

        contentHandler->ignoreNoScriptElement = NW_TRUE;

        // if there any external scripts load requests pending , then we do not want to
        // process script nodes with inline scripts . We suspend the box tree creating until
        // the external scripts are loaded.
        // if there are external script load requests pending
        if (NW_XHTML_ContentHandler_IsScriptLoadPending(contentHandler, elementNode))
            {
            // find the 'src' attribute and get its string value
            status = NW_HED_DomHelper_HasAttribute(elementNode, NW_XHTML_AttributeToken_src);
            // if the src is not present then  the script element is assumed to be inline script
            if (status == KBrsrNotFound)
                {
                // a node context that contains the script node and the placehoder box
                scriptNodeContext = new NW_XHTML_Script_Node_Context_t;
                NW_THROW_OOM_ON_NULL(scriptNodeContext, status);

                // create a placeholder container box for the script.
                placeHolderBox = NW_LMgr_ContainerBox_New(0);
                NW_THROW_OOM_ON_NULL(placeHolderBox, status);
                // add the box to the box tree
                status = NW_LMgr_ContainerBox_AddChild (parentBox, (NW_LMgr_Box_t*)placeHolderBox);
                _NW_THROW_ON_ERROR (status);

                // add the script node to a pending list , the list is processed after the
                // document load is complete
                RPointerArray<NW_XHTML_Script_Node_Context_t>* scriptNodeList =  STATIC_CAST(RPointerArray<NW_XHTML_Script_Node_Context_t>*, 
                                                                                     contentHandler->scriptNodes);
                scriptNodeContext->box = (NW_LMgr_Box_t*)placeHolderBox;
                scriptNodeContext->scriptNode = elementNode;
                TInt err= scriptNodeList->Append(scriptNodeContext);

                // placeHolderBox is part of boxtree set it to NULL
                placeHolderBox = NULL;

                if(err != KErrNone)
                  {
                  if(err == KErrNoMemory)
                    {
                    NW_THROW_STATUS (status, KBrsrOutOfMemory);
                    }
                  else
                    {
                    NW_THROW_STATUS (status, KBrsrFailure);
                    }
                  }
                // set scriptNodeContext NULL 'coz scriptNodeList has to taken 
                // the ownership of the objects
                scriptNodeContext = NULL;
                }
            //  ignore any other error
            NW_THROW_ON (status, KBrsrOutOfMemory );
            status = KBrsrSuccess;
            }
        else
            {
            status = NW_XHTML_ExecuteScript(elementHandler, contentHandler, elementNode, &ecmaCH);
            NW_THROW_ON (status, KBrsrOutOfMemory );            
            if (ecmaCH != NULL)
                {
                // if there any document write's , create a subtree
                // and attach the tree to the box tree that is the inprocess
                // of creation
                if (ecmaCH->documentWriteText)
                    {
                    NW_Buffer_t* domBuffer;
                    NW_Uint32 line;
                    NW_Buffer_t docBuf;

                    TInt len = sizeof(TText) * NW_Str_Strlen(ecmaCH->documentWriteText);
                    docBuf.length = len;
                    docBuf.allocatedLength = len;
                    docBuf.data = (NW_Byte*)ecmaCH->documentWriteText;

                    /* flip the byte order to network byte order, required by cXML */
                    NW_XHTML_ElementHandler_Ucs2NativeToNetworkByteOrder(len, docBuf.data);


                    status = NW_HTMLP_HtmlToWbxml (&docBuf, HTTP_iso_10646_ucs_2,
                                                   NW_BYTEORDER_BIG_ENDIAN,
                                                   &domBuffer, &line, NW_xhtml_1_0_PublicId,
                                                   NW_HTMLP_Get_ElementTableCount(),
                                                   NW_HTMLP_Get_ElementDescriptionTable(), 
                                                   NW_FALSE, NW_FALSE, 
                                                   NULL, NULL,
                                                   NW_TRUE);
                    if (status == KBrsrOutOfMemory)
                        {
                        // free the buffer
                        NW_THROW (KBrsrOutOfMemory);
                        }
                    if (BRSR_STAT_IS_SUCCESS(status))
                        {
                        ecmaCH->domTree = NW_HED_DomTree_New (domBuffer, HTTP_iso_10646_ucs_2,
                                                          NW_xhtml_1_0_PublicId,NULL);
                        if (ecmaCH->domTree )
                            {
                            NW_cXML_DOM_DOMVisitor_t* domVisitor = NULL;
                            NW_DOM_Node_t* domNode;
                            domVisitor = NW_cXML_DOM_DOMVisitor_New (NW_HED_DomTree_GetRootNode (ecmaCH->domTree ));
                            if (!domVisitor)
                                {
                                // free the buffer
                                NW_THROW (KBrsrOutOfMemory);
                                }
                            while ((domNode = NW_cXML_DOM_DOMVisitor_Next (domVisitor,
                                              NW_DOM_ELEMENT_NODE |
                                              NW_DOM_TEXT_NODE |
                                              NW_DOM_PROCESSING_INSTRUCTION_NODE))
                                   != NULL)
                                {
                                const NW_XHTML_ElementHandler_t * childElementHandler;

                                /* get the ElementHandler for the element - if there is no ElementHandler,
                                   we must ignore the element and continue our traversal */
                                childElementHandler =
                                    NW_XHTML_ContentHandler_GetElementHandler (contentHandler,
                                            (NW_DOM_ElementNode_t*) domNode);

                                if (childElementHandler == NULL)
                                    {
                                    continue;
                                    }

                                /* initialize the element*/
                                status =
                                    NW_XHTML_ElementHandler_Initialize (childElementHandler,
                                                                        contentHandler,
                                                                        (NW_DOM_ElementNode_t*) domNode);
                                }


                            /* create the child BoxTree, invoke super class method */
                            status =
                                _NW_XHTML_ElementHandler_CreateBoxTree(elementHandler, contentHandler,
                                                                       (NW_HED_DomTree_GetRootNode (ecmaCH->domTree )), (NW_LMgr_ContainerBox_t*)parentBox);
                            
                            if(aBoxTreeUpdated)
                              {
                              *aBoxTreeUpdated = NW_TRUE;
                              }
                            NW_Object_Delete (domVisitor);
                            }
                        else
                            {
                            // dom tree failed to create,
                            // free dombuffer since it is not ownedby
                            // dom tree
                            NW_LOG0(NW_LOG_LEVEL1, "NW_XHTML_scriptElementHandler_ExecuteScript: BadContent: ecmaCH->domTree NULL");
                            status = KBrsrXhtmlBadContent;
                            NW_Buffer_Free(domBuffer);
                            }
                        }
                    else
                        {
                        NW_LOG0(NW_LOG_LEVEL1, "NW_XHTML_scriptElementHandler_ExecuteScript: BadContent: status NOT SUCCESS");
                        status = KBrsrXhtmlBadContent;
                        }
                    }
                }
            }
        }
    NW_CATCH (status)
        {
        NW_Object_Delete(placeHolderBox);
        delete scriptNodeContext;
        }
    NW_FINALLY
        {
        // set ecma script flag off
        contentHandler->iScriptIsExecuted = NW_FALSE;
        NW_LOG0( NW_LOG_OFF, "NW_XHTML_scriptElementHandler_ExecuteScript End" );
        return status;
        }
    NW_END_TRY
    }

TBrowserStatusCode
_NW_XHTML_scriptElementHandler_CreateBoxTree(const NW_XHTML_ElementHandler_t* elementHandler,
        NW_XHTML_ContentHandler_t* contentHandler,
        NW_DOM_ElementNode_t* elementNode,
        NW_LMgr_ContainerBox_t* parentBox)
    {
    return NW_XHTML_scriptElementHandler_ExecuteScript(elementHandler,contentHandler,
                                                                           elementNode,parentBox,NULL/*box tree updated*/);
    }
