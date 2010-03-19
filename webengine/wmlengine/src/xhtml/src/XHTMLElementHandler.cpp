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

#include "BrsrStatusCodes.h"
#include "nw_xhtml_elementhandleri.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_text_ucs2.h"
#include "nw_text_ascii.h"
#include "nwx_string.h"

#include "nw_list_listelementhandler.h"
#include "nw_list_listitemelementhandler.h"
#include "nw_structure_headelementhandler.h"
#include "nw_text_brelementhandler.h"
#include "nw_text_hrelementhandler.h"
#include "nw_basicforms_formelementhandler.h"
#include "nw_basicforms_inputelementhandler.h"
#include "nw_basicforms_selectelementhandler.h"
#include "XhtmlTableElementHandlerOOC.h"
#include "nw_basictables_trelementhandler.h"
#include "nw_image_imgelementhandler.h"
//#include "XhtmlObjectElementHandlerOOC.h"
#include "nw_metainformation_metaelementhandler.h"
#include "nw_link_linkelementhandler.h"
#include "nw_base_baseelementhandler.h"
#include "nw_xhtml_textelementhandler.h"
#include "nw_dom_element.h"
#include "nw_dom_document.h"
#include "nw_xhtml_attributepropertytable.h"
#include "nw_css_defaultstylesheet.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nwx_settings.h"
#include "nw_ignore_ignoreelementhandler.h"
//#include "nw_ecma_contenthandler.h"
#include "nw_image_mapelementhandler.h"
#include "nw_hypertext_aelementhandler.h"
#include "nw_structure_noscriptelementhandler.h"
#include "LMgrObjectBoxOOC.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_dom_domvisitor.h"
#include "CSSVariableStyleSheet.h"

/* -------------------------------------------------------------------------- *
    private functions
 * ---------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------- *
    static data
 * ---------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------- */
const
NW_XHTML_ElementHandler_Class_t NW_XHTML_ElementHandler_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Core_Class,
    /* queryInterface            */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler  */
    /* initialize                */ _NW_XHTML_ElementHandler_Initialize,
    /* createBoxTree             */ _NW_XHTML_ElementHandler_CreateBoxTree,
    /* processEvent              */ _NW_XHTML_ElementHandler_ProcessEvent
  }
};

/* ------------------------------------------------------------------------- */
const
NW_XHTML_ElementHandler_t NW_XHTML_ElementHandler = {
  { &NW_XHTML_ElementHandler_Class }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_ElementHandler_Initialize (const NW_XHTML_ElementHandler_t* thisObj,
                                      NW_XHTML_ContentHandler_t* contentHandler,
                                      NW_DOM_ElementNode_t* elementNode)
{
  /* avoid 'unreferenced formal parameter' warnings */
  (void) thisObj;
  (void) contentHandler;
  (void) elementNode;

  /* by default, we do nothing ... */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_ElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* thisObj,
                                        NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode,
                                        NW_LMgr_ContainerBox_t* parentBox)
{
  NW_DOM_Node_t* childNode;

  /* iterate through our children calling their 'createBoxTree' method */
  for (childNode = NW_DOM_Node_getFirstChild (elementNode);
       childNode != NULL ;
       childNode = NW_DOM_Node_getNextSibling (childNode))
  {
    const NW_XHTML_ElementHandler_t* elementHandler;
    TBrowserStatusCode status;

    /* get the ElementHandler for the elementNode */
    elementHandler =
      NW_XHTML_ContentHandler_GetElementHandler (contentHandler,
                                                 (NW_DOM_ElementNode_t*) childNode);
    if (elementHandler == NULL)
    {
      continue;
    }

    if(NW_XHTML_ElementToken_body == NW_HED_DomHelper_GetElementToken (childNode))
    {
      NW_HED_DocumentRoot_t* documentRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (contentHandler);
      //R->ecma
      //NW_Ecma_HandleEvent(documentRoot , childNode, childNode, NW_Ecma_Evt_OnLoad);

      contentHandler->iBodyNode = childNode;
    }

    if ((NW_Object_IsInstanceOf(elementHandler, &NW_XHTML_TextElementHandler_Class)) &&
        ((NW_Object_IsInstanceOf(thisObj, &NW_XHTML_headElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_brElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_hrElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_inputElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_selectElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_tableElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_trElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_imgElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_metaElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_linkElementHandler_Class))||
         (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_baseElementHandler_Class))))
         //||
         //(NW_Object_IsInstanceOf(thisObj, &NW_XHTML_mapElementHandler_Class))))
    {
      continue;
    }

    // If the parent element is an ignore element handler, ignore this element
    // and all sibling elements.  Currently, the <noframe> and <noembed> tags
    // are "ignored" elements.
    if (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_IgnoreElementHandler_Class))
    {
      continue;
    }


  /* Check if script handling is enabled or not.
   * If the the prev script element is supported,
   * the <noscript> elements and all its
   *  children should not be rendered
  */

  if ( NW_Settings_GetEcmaScriptEnabled() &&
       (NW_Object_IsInstanceOf(thisObj, &NW_XHTML_noscriptElementHandler_Class)) &&
        contentHandler->ignoreNoScriptElement)
    {
      continue;
    }


    /*
    If both frameset and body elements are in the same page, the first instance takes precedence,
    the second one should be ignored.  For example, if the body tag appears before the frameset
    tag, ignore the frameset tag and vice versa.
    */
    if ((contentHandler->ignoreFramesetElement && (NW_XHTML_ElementToken_frameset == NW_HED_DomHelper_GetElementToken (elementNode))) ||
        (contentHandler->ignoreBodyElement && (NW_XHTML_ElementToken_body == NW_HED_DomHelper_GetElementToken (elementNode))))
    {
      continue;
    }

    /* create the child BoxTree */
    status =
      NW_XHTML_ElementHandler_CreateBoxTree (elementHandler, contentHandler,
                                             childNode, parentBox);

    if (status != KBrsrSuccess) {
      return status;
    }
  }

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Uint8
_NW_XHTML_ElementHandler_ProcessEvent (const NW_XHTML_ElementHandler_t* thisObj,
                                        NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode,
                                        NW_LMgr_Box_t* box,
                                        NW_Evt_Event_t* event)
{
  /* avoid 'unreferenced formal parameter' warnings */
  (void) thisObj;
  (void) contentHandler;
  (void) elementNode;
  (void) box;
  (void) event;

  /* by default, we don't absorb the event ... */
  return 0;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
// Applies styles associated with style attribute
static
TBrowserStatusCode
NW_XHTML_ElementHandler_ApplyStyleAttribute(NW_XHTML_ContentHandler_t* contentHandler,
                                    NW_DOM_ElementNode_t* elementNode,
                                    NW_LMgr_Box_t* box)
{
  NW_TRY (status)
  {
    NW_CSS_Processor_t* processor = NULL;
    NW_Text_t* styleVal = NULL;
    NW_LMgr_Property_t elementIdProp;

    processor = NW_XHTML_ContentHandler_GetCSSProcessor(contentHandler);

    /* apply the element attribute - copy mask will prevent the node being deleted
       This id will be used to identify the container box with al text and then to
       replace it with the image
    */
    elementIdProp.type = NW_CSS_ValueType_Object;
    elementIdProp.value.object = elementNode;
    status = NW_LMgr_Box_SetProperty(box, NW_CSS_Prop_elementId, &elementIdProp);

    /* Apply styles in the style attributes */
    NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
                                        NW_XHTML_AttributeToken_style, &styleVal);
    if (styleVal!= NULL)
    {
      if (!processor){
        processor = NW_XHTML_ContentHandler_CreateCSSProcessor(contentHandler);
        NW_THROW_OOM_ON_NULL (processor, status);
      }

      status = NW_CSS_Processor_ApplyStyleAttribute(processor, styleVal, box);
      NW_Object_Delete(styleVal);
    }
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* -------------------------------------------------------------------------*/
// Applies styles associated with HTML attributes and default stylesheet
static
TBrowserStatusCode
NW_XHTML_ElementHandler_ApplyDefaultStyles (NW_XHTML_ContentHandler_t* contentHandler,
                                    NW_DOM_ElementNode_t* elementNode,
                                    NW_LMgr_Box_t** box,
                                    const NW_Evt_Event_t* event)
{
  NW_TRY (status)
  {
    CCSSVariableStyleSheet* defaultSS = NULL;
    NW_Uint16 elementToken = NW_HED_DomHelper_GetElementToken (elementNode);

    /* Apply the styles which were specified by HTML attributes */
    status = NW_XHTML_AttributePropertyTable_ApplyStyles(contentHandler, elementNode, *box);
    NW_THROW_ON(status , KBrsrOutOfMemory);

    /* Apply default styleSheet styles */
    defaultSS = NW_XHTML_ContentHandler_GetVariableStyleSheet(contentHandler);
    NW_THROW_ON_NULL (defaultSS, status, KBrsrFailure);

    status = defaultSS->ApplyStyles(elementNode,
                                    *box,
                                    NW_XHTML_ContentHandler_GetDictionary(contentHandler),
                                    event,
                                    NW_HED_DocumentNodeOf(contentHandler),
                                    &(contentHandler->domHelper));
    NW_THROW_ON(status , KBrsrOutOfMemory);

    switch(elementToken)
    {
    case NW_XHTML_ElementToken_a:
    case NW_XHTML_ElementToken_frame:
      if (NW_Object_IsClass(*box, &NW_LMgr_ActiveContainerBox_Class))
      {
        status = NW_XHTML_AttributePropertyTable_AddActiveBoxDecoration(*box, NW_FALSE);
      }
    break;
    case NW_XHTML_ElementToken_img:
      if (NW_LMgr_Box_FindBoxOfClass (*box, &NW_LMgr_ActiveContainerBox_Class, NW_FALSE) != NULL)
      {
        status = NW_XHTML_AttributePropertyTable_AddActiveBoxDecoration(*box, NW_TRUE);
      }
    break;
    default:
      break;
    }
  }
  NW_CATCH (status)
  {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* -------------------------------------------------------------------------*/
TBrowserStatusCode
NW_XHTML_ElementHandler_ApplyStyles (const NW_XHTML_ElementHandler_t* thisObj,
                                    NW_XHTML_ContentHandler_t* contentHandler,
                                    NW_DOM_ElementNode_t* elementNode,
                                    NW_LMgr_Box_t** box,
                                    const NW_Evt_Event_t* event)
{
  NW_REQUIRED_PARAM(thisObj);
  NW_REQUIRED_PARAM(event);

  NW_ASSERT(thisObj);

  NW_TRY (status)
  {
    NW_LMgr_PropertyValue_t displayVal;

    status = NW_XHTML_ElementHandler_ApplyCSSCallback((void*)contentHandler, elementNode, box);
    NW_THROW_ON(status , KBrsrOutOfMemory);

    displayVal.token = NW_CSS_PropValue_display_inline;
    status = NW_LMgr_Box_GetPropertyValue(*box, NW_CSS_Prop_display,
                                          NW_CSS_ValueType_Token, &displayVal);
    /*if(displayVal.token == NW_CSS_PropValue_display_block && NW_Object_IsInstanceOf (thisObj,
                                                                                    &NW_XHTML_aElementHandler_Class))
    {
      NW_LMgr_Property_t        prop;
      prop.type = NW_CSS_ValueType_Token;
      prop.value.token = NW_CSS_PropValue_display_inline;
      NW_LMgr_Box_SetProperty(*box, NW_CSS_Prop_display, &prop);
    }
    else */
    if (displayVal.token == NW_CSS_PropValue_none)
    {
      // remove the associated box as the display is none and we do not want
      // it to participate in the layout
      NW_Object_Delete(*box);
      *box = NULL;
      NW_THROW_STATUS(status,KBrsrSuccess);
    }
  // Check display Val and replace box is necessary
    status = NW_CSS_Processor_HandleDisplayVal(box, &displayVal);
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------- */
/*
This should go away when the cXML library gets the ability to cope with
arbitrary byte orderings.  For now, the UCS-2 has to be fed to cXML in
network byte order.
*/

void
NW_XHTML_ElementHandler_Ucs2NativeToNetworkByteOrder(NW_Uint32 byteCount,
                                                       NW_Uint8* pBuf)
{
  NW_Uint32 i;
  NW_Uint16 c_ucs2 = 1;

  if (((NW_Uint8*)&c_ucs2)[0] == 1) { /* test for little endian host */
    for (i = 0; i < byteCount; i += 2) {
      (void)NW_Mem_memcpy(&c_ucs2, pBuf + i, sizeof(NW_Uint16));
      pBuf[i] = (NW_Uint8)((c_ucs2 >> 8) & 0xff);
      pBuf[i+1] = (NW_Uint8)(c_ucs2 & 0xff);
    }
  }
}

/* ------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_ElementHandler_ApplyCSSCallback (void* ch,
                                          NW_DOM_ElementNode_t* elementNode,
                                          NW_LMgr_Box_t** box)
{
  NW_TRY (status)
  {
    NW_CSS_Processor_t* processor = NULL;
    NW_XHTML_ContentHandler_t* contentHandler = (NW_XHTML_ContentHandler_t*)ch;

    /* apply styles associated with style attribute */
    status = NW_XHTML_ElementHandler_ApplyStyleAttribute(contentHandler, elementNode, *box);
    NW_THROW_ON(status , KBrsrOutOfMemory);

    /* Apply the styles which were specified in the styleSheet */
    processor = NW_XHTML_ContentHandler_GetCSSProcessor(contentHandler);
    if (processor)
    {
      status = NW_CSS_Processor_ApplyStyles(processor, elementNode, *box, NULL, &(contentHandler->domHelper));
      NW_THROW_ON(status , KBrsrOutOfMemory);
    }
    /* Apply the styles which were specified by HTML attributes and default stylesheet */
    status = NW_XHTML_ElementHandler_ApplyDefaultStyles(contentHandler, elementNode, box, NULL);
    NW_THROW_ON(status , KBrsrOutOfMemory);
  }
  NW_CATCH (status) {
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_ElementHandler_GetAttribute (NW_XHTML_ContentHandler_t *contentHandler,
                                      NW_DOM_ElementNode_t *elementNode,
                                      const NW_Uint16 attrToken,
                                      NW_Ucs2 **attrValue)
{
  NW_String_t value;

  /* get the attribute value, returned as:
     malloced, UCS-2, aligned, null terminated */
  NW_TRY (status) {
    status = NW_HED_DomHelper_GetHTMLAttributeValue(&(contentHandler->domHelper),
                                                elementNode, attrToken, &value);
    _NW_THROW_ON_ERROR (status);

    *attrValue = (NW_Ucs2*)(value.storage);

  }
  NW_CATCH (status) {
    if (status != KBrsrOutOfMemory) {
      status = KBrsrNotFound;
    }
  }
  NW_FINALLY {
    return status;
  } NW_END_TRY
}


/* ------------------------------------------------------------------- */
NW_Bool
NW_XHTML_ElementHandler_IsBlockLevelTag(NW_Uint16 tagToken)
{
    switch(tagToken)
    {
    case NW_XHTML_ElementToken_p:
    case NW_XHTML_ElementToken_table:
    case NW_XHTML_ElementToken_td:
    case NW_XHTML_ElementToken_tr:
    case NW_XHTML_ElementToken_map:
    case NW_XHTML_ElementToken_marquee:
    case NW_XHTML_ElementToken_form:
    case NW_XHTML_ElementToken_address:
    case NW_XHTML_ElementToken_blockquote:
    case NW_XHTML_ElementToken_caption:
    case NW_XHTML_ElementToken_center:
    case NW_XHTML_ElementToken_code:
    case NW_XHTML_ElementToken_dd:
    case NW_XHTML_ElementToken_dt:
    case NW_XHTML_ElementToken_ul:
    case NW_XHTML_ElementToken_ol:
    case NW_XHTML_ElementToken_li:
    case NW_XHTML_ElementToken_dir:
    case NW_XHTML_ElementToken_div:
    case NW_XHTML_ElementToken_fieldset:
    case NW_XHTML_ElementToken_menu:
    case NW_XHTML_ElementToken_h1:
    case NW_XHTML_ElementToken_h2:
    case NW_XHTML_ElementToken_h3:
    case NW_XHTML_ElementToken_h4:
    case NW_XHTML_ElementToken_h5:
    case NW_XHTML_ElementToken_h6:
    case NW_XHTML_ElementToken_hr:
    case NW_XHTML_ElementToken_textarea:
    case NW_XHTML_ElementToken_tt:
      return NW_TRUE;
    default:
      return NW_FALSE;
    }
}

/* ------------------------------------------------------------------- */
// should be called from select, a, script, map, object element handler
// if called from initializae of element handler, controlInitEH is true
// if called from creatboxtree of element handler, controlInitEH is false
NW_Bool
NW_XHTML_ElementHandler_NextSiblingExists (NW_XHTML_ContentHandler_t *contentHandler,
                                           NW_DOM_ElementNode_t *elementNode,
                                           NW_Bool controlInitEH)
{
  NW_Bool exists = NW_TRUE;

  // special handling if the element node is the last part of the chunk:
  // leave the node to be processed in the next chunk
  // in case this is the last chunk or the ecma script is adding new elements,
  // we need to process the node in this chunk
  if( !contentHandler->lastChunk && !contentHandler->iScriptIsExecuted )
  {
    NW_cXML_DOM_DOMVisitor_t visitor;
    NW_DOM_Node_t *domNode = NULL;
    NW_cXML_DOM_DOMVisitor_Initialize(&visitor, NW_HED_DomTree_GetRootNode(contentHandler->domTree));
    visitor.currentNode = elementNode;
    // search for the next node provided skip children
    domNode = NW_cXML_DOM_DOMVisitor_NextNode( &visitor,
                                         NW_DOM_ELEMENT_NODE |
                                         NW_DOM_TEXT_NODE |
                                         NW_DOM_PROCESSING_INSTRUCTION_NODE,
                                         NW_TRUE);
    if (domNode == NULL)
    {
      if (controlInitEH)
      {
        contentHandler->stopInitEH = NW_TRUE;
      }
      exists = NW_FALSE;
    }
  }

  return exists;
}

// Concatenates the Text of subtree. For e.g. <button><div> Search Flights </div><br/> + Hotels </button>
NW_Text_t*
NW_XHTML_ElementHandler_GetSubTreeText(NW_XHTML_ContentHandler_t* aContentHandler, NW_DOM_ElementNode_t* aElementNode)
    {
    NW_DOM_Node_t* childNode = NULL;
    NW_Text_t* subTreeValue = NULL;
    NW_DOM_Node_t* parent = NULL;

    childNode = aElementNode;
    while ( childNode )
        {
        parent = childNode;
        // First go depth first looking for the first child
        childNode = NW_DOM_Node_getFirstChild( childNode );
        // if childNode is not there look for sibling
        // keep on going up the tree till the parent is the root
        while ((!childNode) && (parent != aElementNode))
            {
            // childNode is NULL -> means parent was a leaf node
            // Find the sibling of parent
            childNode = NW_DOM_Node_getNextSibling( parent );
            // sibling parent is NULL, go up the tree
            if ( !childNode )
                {
                parent = NW_DOM_Node_getParentNode( parent );
                }
            }
        if (childNode && (NW_DOM_Node_getNodeType(childNode) == NW_DOM_TEXT_NODE))
            {
            NW_Text_t* value = NW_XHTML_GetDOMTextNodeData(aContentHandler, childNode);
            if (value)
                {
                if (!subTreeValue)
                    {
                    subTreeValue = value;
                    }
                else
                    {
                    // concatenate it
                    NW_Text_t* newText = NW_Text_Concatenate(subTreeValue, value);
                    if (newText)
                        {
                        NW_Object_Delete( subTreeValue );
                        subTreeValue = newText;
                        }
                    NW_Object_Delete(value);
                    }
                }
            }
        };
    return subTreeValue;
}
