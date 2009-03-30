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


#include "nw_basicforms_selectelementhandleri.h"

#include "nwx_string.h"
#include "nw_fbox_selectbox.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "nw_hed_documentroot.h"
#include "BrsrStatusCodes.h"



/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */
static
NW_LMgr_Box_t*
NW_XHTML_selectElementHandler_GetOptionBox(const NW_XHTML_ElementHandler_t* elementHandler,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_DOM_ElementNode_t* selectNode,
                                           NW_FBox_FormLiaison_t* formLiaison)
  {
  NW_LMgr_Box_t* optionBox;
  NW_Text_t* optionData = NULL;
  NW_Text_t* text = NULL;
  NW_Ucs2* storage = NULL;
  NW_HED_EventHandler_t* eventHandler;
  NW_Ucs2* emptyText = NULL;
  NW_Text_Length_t i;
  NW_Text_Length_t length;
  NW_Bool freeNeeded;

  NW_REQUIRED_PARAM (selectNode);


  /* To comply with WML, HTML 4.01, and XHtml, the text
  displayed in an option is the first of the following:
  1. (HTML)  Contents of label attribute of the option element.
  2. (all)   Content of the option element
  3. (WML)   Contents of the title attribute of the option element.
  4. (all)   Contents of the value attribute of the option element.
  */

  /* Query the DOM tree for the value of the "label" attribute.  Per
  *  HTML 4.01 Specification section 17.6.1 Pre-selected options: "When
  *  specified, user agents should use the value of this attribute
  *  rather than the content of the OPTION element as the option label."
  */

  NW_XHTML_GetDOMAttribute(contentHandler, elementNode, NW_XHTML_AttributeToken_label, &text);
  if (text == NULL)
    {
      // No label, try element content.
      text = NW_XHTML_ElementHandler_GetSubTreeText(contentHandler, elementNode);

      if (text == NULL)
        {
          // No content, try title.
          NW_XHTML_GetDOMAttribute(contentHandler, elementNode,
                                   NW_XHTML_AttributeToken_title, &text);
          if (text == NULL)
            {
              // No title, try value.
              NW_XHTML_GetDOMAttribute(contentHandler,  elementNode,
                                       NW_XHTML_AttributeToken_value, &text);
            }

          if(text == NULL )
            {
              //No value then use empty string by creating a single null character string.
              emptyText= NW_Str_New(1);
              if(emptyText == NULL)
                return NULL;/* out of memory */
              NW_Str_Strncpy(emptyText ,(const unsigned short *)"",1);
              text = NW_Text_New (HTTP_iso_10646_ucs_2, (void*) emptyText, 1,NW_Text_Flags_TakeOwnership);
              if(text == NULL)
                {
                  NW_Str_Delete(emptyText);
                  return NULL;/* out of memory */
                }
            }
        }
    }

  /* If we got this far then option data has been set to a non-null
   * value. Request storage forcing a copy of the data so it can be
   * manipulated it. The process is to first convert non-standard
   * whitespace characters to spaces. Next the 'compress whitespace'
   * utility can clean up the leading and excess internal spaces. Note
   * that depending on how the value of storage was determined the
   * whitespace may have already been removed or never existed in the
   * first place (e.g. emptyText). It is not worth optimizing out these
   * exception cases.
   */

  //Get a new copy of the storage
  storage = NW_Text_GetUCS2Buffer (text, NW_Text_Flags_Aligned | NW_Text_Flags_Copy , NULL, &freeNeeded);
  NW_Object_Delete(text);

  //If there are &nbsp's in the title, convert them into spaces
  if (storage)
    {
      length = NW_Str_Strlen(storage);
      for (i = 0; i < length; i++)
        {
          if ((storage[i] == NW_TEXT_UCS2_TAB) || (storage[i] == NW_TEXT_UCS2_NBSP))
            {
              storage[i] = ' ';
            }
        }

      // After converting all whitespace to spaces we can compress the
      // string. Note that this compress utility removes leading and
      // trailing whitespace so trimming isn't needed.
      NW_Str_CompressWhitespace(storage);
    }

  optionData = (NW_Text_t*) NW_Text_UCS2_New (storage, 0, NW_Text_Flags_TakeOwnership);
  if (optionData == NULL)
    {
      NW_Str_Delete(storage);
      return NULL;  /* out of memory */
    }


  /* create the activeBox */
  eventHandler = NW_HED_EventHandler_New(NW_HED_DocumentNodeOf(contentHandler), elementNode);
  if (eventHandler == NULL)
    {
      NW_Object_Delete(optionData);
      return NULL;  /* out of memory */
    }

  optionBox = (NW_LMgr_Box_t*)NW_FBox_OptionBox_New (1, NW_LMgr_EventHandlerOf (eventHandler),
                                                     elementNode, formLiaison, optionData);
  if (optionBox == NULL)
    {
      NW_Object_Delete(optionData);
      NW_Object_Delete(eventHandler);
      return NULL;  /* out of memory */
    }

  /* successful completion */

  /* apply common attributes and styles */
  NW_XHTML_ElementHandler_ApplyStyles(elementHandler, contentHandler, elementNode,
                                      &optionBox, 0);
  return optionBox;
  }

/* ------------------------------------------------------------------------- */
static
NW_LMgr_Box_t*
NW_XHTML_selectElementHandler_GetOptGrpBox(const NW_XHTML_ElementHandler_t* elementHandler,
                                           NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* elementNode,
                                           NW_DOM_ElementNode_t* selectNode,
                                           NW_FBox_FormLiaison_t* formLiaison,
                                           NW_Bool* isFirstOption,
                                           NW_Bool multipleSelection)
{
  NW_LMgr_Box_t* optgrpBox;
  NW_HED_EventHandler_t* eventHandler;
  NW_Text_t* attrVal = NULL;
  TBrowserStatusCode status;
  NW_DOM_Node_t* childNode;
  NW_Bool true_value = NW_TRUE;
  NW_Ucs2* emptyText = NULL;

  /* successful completion */
  /* get label of the new box */
  NW_XHTML_GetDOMAttribute (contentHandler,  elementNode,
                                     NW_XHTML_AttributeToken_label, &attrVal);

  /* The label attribute is required;
  ** but if it does not exist, attempt to recover
  ** by getting the title attribute, if any.
  */
  if (attrVal == NULL) {
    NW_XHTML_GetDOMAttribute(contentHandler, elementNode,
                                       NW_XHTML_AttributeToken_title, &attrVal);
  }
  /* If no title exists, then it is an error.  If we really
  ** wanted to, we make a empty  label to use
  ** and continue but that isn't a specified behavior
  */
  if (attrVal == NULL) {
    /* if value is null then empty string is assigned for the option
    to be displayed and selected*/
    emptyText= NW_Str_New(1);
    if(emptyText == NULL)
      return NULL;/* out of memory */
    NW_Str_Strncpy(emptyText ,(const unsigned short *)"",1/*length*/);
    attrVal = NW_Text_New (HTTP_iso_10646_ucs_2, (void*) emptyText  ,
                              1/* characterCount*/,NW_Text_Flags_TakeOwnership);
    if(attrVal == NULL){
      NW_Str_Delete(emptyText);
      return NULL;/* out of memory */
    }
  }

  /* create the activeBox */
  eventHandler =
    NW_HED_EventHandler_New (NW_HED_DocumentNodeOf (contentHandler),
                                         elementNode);
  if (eventHandler == NULL){
    NW_Object_Delete(attrVal);
    return NULL;
  }

  optgrpBox = (NW_LMgr_Box_t*)NW_FBox_OptGrpBox_New(1, NW_LMgr_EventHandlerOf (eventHandler),
                                    elementNode, formLiaison, attrVal);
  if (optgrpBox == NULL)
  {
    NW_Object_Delete(attrVal);
    NW_Object_Delete(eventHandler);
    return NULL;
  }

  /* successful completion */

  /* apply common attributes and styles */
  status = NW_XHTML_ElementHandler_ApplyStyles(elementHandler, contentHandler, elementNode,
                                             &optgrpBox, 0);
  if (status == KBrsrOutOfMemory)
  {
    return NULL;
  }
  if (optgrpBox == NULL)
  {
    return NULL;
  }

  for (childNode = NW_DOM_Node_getFirstChild (elementNode);
       childNode != NULL;
       childNode = NW_DOM_Node_getNextSibling (childNode))
  {
    if (NW_DOM_Node_getNodeType(childNode) != NW_DOM_ELEMENT_NODE)
      continue;

    if (NW_HED_DomHelper_GetElementToken (childNode) == NW_XHTML_ElementToken_option)
    {
      NW_LMgr_Box_t* optionBox;

      optionBox = NW_XHTML_selectElementHandler_GetOptionBox(elementHandler,
                                                 contentHandler,
                                                 childNode,
                                                 selectNode,
                                                 formLiaison);
      if (optionBox == NULL)
      {
        return NULL; /* out of memory or other error */
      }
      
      // We want to select the first option in a select box if
      // 1) it is the first option
      // 2) if there is no value for this option (when we load a page vs. using the back button)
      // 3) it is not a multiple selection box
      NW_XHTML_FormLiaison_t* xhtmlFormLiaison = NW_XHTML_FormLiaisonOf (formLiaison);
      NW_Object_t* value = NW_XHTML_ControlSet_GetValue (xhtmlFormLiaison->controlSet, (NW_DOM_ElementNode_t*) childNode);
      if(*isFirstOption && !value && !multipleSelection)
      {
        (void) NW_FBox_FormLiaison_SetBoolValue(formLiaison, childNode, &true_value);
        *isFirstOption = NW_FALSE;
      }
      else if(!multipleSelection)
      {
        NW_Ucs2* selectedAttr = NULL;
        /* find if selected, this is to set the most recent option that is selected */
        status = NW_XHTML_ElementHandler_GetAttribute(contentHandler,
                                                      childNode,
                                                      NW_XHTML_AttributeToken_selected,
                                                      &selectedAttr);
        if (status == KBrsrSuccess){
          if ( NW_Str_StricmpConst(selectedAttr, "selected") == 0 ) {
            (void) NW_FBox_FormLiaison_SetBoolValue(formLiaison, childNode, &true_value);
          }
        }
        NW_Str_Delete( selectedAttr );
        selectedAttr = NULL;
      }
      NW_FBox_OptGrpBox_AddOption(NW_FBox_OptGrpBoxOf(optgrpBox), NW_FBox_OptionBoxOf(optionBox));
      NW_XHTML_FormLiaison_AddBox(formLiaison, optionBox, childNode);
    }
  }

  return optgrpBox;
}





/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_selectElementHandler_Class_t NW_XHTML_selectElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_selectElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_selectElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_selectElementHandler */
    /* unused                      */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_selectElementHandler_t NW_XHTML_selectElementHandler = {
  { { &NW_XHTML_selectElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_selectElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode)
{
  NW_FBox_FormLiaison_t* formLiaison;

  NW_REQUIRED_PARAM(elementHandler);

  NW_TRY (status)
  {
    // special handling if select is the last part of the chunk:
    // leave select to be processed in the next chunk
    // in case this is the last chunk, we need to process select in this chunk
    if (!NW_XHTML_ElementHandler_NextSiblingExists (contentHandler, elementNode, NW_TRUE))
    {
      NW_THROW_SUCCESS (status);
    }

    formLiaison =
      NW_XHTML_ContentHandler_GetFormLiaison(contentHandler, elementNode);

    if (formLiaison == NULL){
      NW_THROW_SUCCESS (status);
    }

    status = NW_FBox_FormLiaison_AddControl(formLiaison, elementNode);
    _NW_THROW_ON_ERROR (status);
  }
  NW_CATCH (status)
  {
  }
  NW_FINALLY
  {
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */


TBrowserStatusCode
_NW_XHTML_selectElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
{
  const NW_XHTML_selectElementHandler_t* thisObj;
  NW_LMgr_Box_t* box;
  NW_FBox_FormLiaison_t* formLiaison;
  NW_HED_EventHandler_t* eventHandler;
  NW_HED_DocumentRoot_t* docRoot;
  NW_Ucs2* multipleAttr = NULL;
  NW_Ucs2* selectedAttr = NULL;
  NW_Bool multipleSelection = NW_FALSE;
  NW_DOM_ElementNode_t* childNode;
  NW_Text_t* name = NULL;
  NW_Text_t* title = NULL;
  NW_Bool isFirstOption = NW_TRUE;
  NW_Bool true_value = NW_TRUE;

  NW_TRY (status)
  {
    if (!NW_XHTML_ElementHandler_NextSiblingExists (contentHandler, elementNode, NW_FALSE))
    {
      NW_THROW_SUCCESS (status);
    }

  /* for convenience */
  thisObj = NW_XHTML_selectElementHandlerOf (elementHandler);

  /* get the DocumentRoot */
  docRoot =
    (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (contentHandler);
  NW_ASSERT(docRoot != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (docRoot, &NW_HED_DocumentRoot_Class));

  formLiaison =
  NW_XHTML_ContentHandler_GetFormLiaison(contentHandler, elementNode);

  if (formLiaison == NULL){
    NW_THROW_SUCCESS (status);
  }

  /* find if multiple */
  status = NW_XHTML_ElementHandler_GetAttribute(contentHandler,
                                                elementNode,
                                                NW_XHTML_AttributeToken_multiple,
                                                &multipleAttr);
  if (status == KBrsrSuccess){
    if ( NW_Str_StricmpConst(multipleAttr, "multiple") == 0 ) {
      multipleSelection = NW_TRUE;
    }
    NW_Str_Delete( multipleAttr );
    multipleAttr = NULL;
  }

  /* find name attribute for select */
  NW_XHTML_GetDOMAttribute (NW_XHTML_ContentHandlerOf (contentHandler),
                                     elementNode,
                                     NW_XHTML_AttributeToken_name, &name);
  NW_XHTML_GetDOMAttribute(contentHandler,
                                     elementNode,
                                     NW_XHTML_AttributeToken_title, &title);

  /* create the activeBox */
  eventHandler =
    NW_HED_EventHandler_New (NW_HED_DocumentNodeOf (contentHandler),
                             elementNode);

  if (eventHandler == NULL) {
    NW_Object_Delete(name);
    NW_Object_Delete(title);
    NW_THROW_STATUS (status, KBrsrOutOfMemory);
  }

  box = (NW_LMgr_Box_t*)
          NW_FBox_SelectBox_New(1,
                               NW_LMgr_EventHandlerOf (eventHandler),
                               elementNode,
                               formLiaison,
                               docRoot->appServices,
                               multipleSelection,
                               name,
                               title);
  if (box == NULL) {
    NW_Object_Delete(eventHandler);
    NW_Object_Delete(name);
    NW_Object_Delete(title);
      NW_THROW_STATUS (status, KBrsrOutOfMemory);
  }

  status = NW_LMgr_ContainerBox_AddChild (parentBox, box);
  if (status != KBrsrSuccess){
    NW_Object_Delete(box);
    NW_Object_Delete(eventHandler);
      NW_THROW (status);
  }

  /* apply common attributes and styles */
  status = NW_XHTML_ElementHandler_ApplyStyles(elementHandler, contentHandler, elementNode, &box, 0);
  if (status == KBrsrOutOfMemory)
  {
      NW_THROW_STATUS (status, KBrsrOutOfMemory);
  }
  if (box == NULL)
  {
      NW_THROW_SUCCESS (status);
  }
  NW_XHTML_FormLiaison_AddBox(formLiaison, (NW_LMgr_Box_t*)box, elementNode);

  for (childNode = NW_DOM_Node_getFirstChild (elementNode);
       childNode != NULL;
       childNode = NW_DOM_Node_getNextSibling (childNode))
  {
    NW_Uint16 token;

    token = NW_HED_DomHelper_GetElementToken (childNode);
    if (token == NW_XHTML_ElementToken_option)
    {
      NW_LMgr_Box_t* optionBox;
      NW_LMgr_Box_t* styleBox = NULL;

      optionBox = NW_XHTML_selectElementHandler_GetOptionBox(elementHandler,
                                                 contentHandler,
                                                 childNode,
                                                 elementNode,
                                                 formLiaison);

      if (!optionBox)
        continue;

      // We want to select the first option in a select box if
      // 1) it is the first option
      // 2) if there is no value for this option (when we load a page vs. using the back button)
      // 3) it is not a multiple selection box
      NW_XHTML_FormLiaison_t* xhtmlFormLiaison = NW_XHTML_FormLiaisonOf (formLiaison);
      NW_Object_t* value = NW_XHTML_ControlSet_GetValue (xhtmlFormLiaison->controlSet, (NW_DOM_ElementNode_t*) childNode);
      if(isFirstOption && !value && !multipleSelection)
          {
          (void) NW_FBox_FormLiaison_SetBoolValue(formLiaison, childNode, &true_value);
          isFirstOption = NW_FALSE;
          }
      else if(!multipleSelection)
          {
          /* find if selected, this is to set the most recent option that is selected */
          status = NW_XHTML_ElementHandler_GetAttribute(contentHandler,
                                                      childNode,
                                                      NW_XHTML_AttributeToken_selected,
                                                      &selectedAttr);
          if (status == KBrsrSuccess)
              {
              if ( NW_Str_StricmpConst(selectedAttr, "selected") == 0 ) 
                {
                (void) NW_FBox_FormLiaison_SetBoolValue(formLiaison, childNode, &true_value);
                }
              }
          NW_Str_Delete( selectedAttr );
          selectedAttr = NULL;
          }

      /* TODO: NW_FBox_SelectBox_AddOption does not return a status when that changes this code
      should be modified - vishy 07/08/2002 */
      NW_FBox_SelectBox_AddOption(NW_FBox_SelectBoxOf(box), NW_FBox_OptionBoxOf(optionBox), &styleBox);

      if (styleBox)
          {
          NW_XHTML_ElementHandler_ApplyStyles(NW_XHTML_ElementHandlerOf (thisObj), contentHandler, childNode, &styleBox, 0);
          }
       NW_XHTML_FormLiaison_AddBox(formLiaison, optionBox, childNode);
   }
    else if (token == NW_XHTML_ElementToken_optgroup)
    {
      NW_LMgr_Box_t* optgrpBox;

      optgrpBox = NW_XHTML_selectElementHandler_GetOptGrpBox(elementHandler,
                                                 contentHandler,
                                                 childNode,
                                                 elementNode,
                                                 formLiaison,
                                                 &isFirstOption,
                                                 multipleSelection);
      if (!optgrpBox)
        continue;
      NW_FBox_SelectBox_AddOptGrp(NW_FBox_SelectBoxOf(box), NW_FBox_OptGrpBoxOf(optgrpBox));
      NW_XHTML_FormLiaison_AddBox(formLiaison, optgrpBox , childNode);
    }
    // if script tag  , the script content should be evaluated so invoke createboxtree
    else if(token == NW_XHTML_ElementToken_script || token == NW_XHTML_ElementToken_noscript )
      {
      /* get the ElementHandler for the elementNode */
      const NW_XHTML_ElementHandler_t* elementHandler=
      NW_XHTML_ContentHandler_GetElementHandler (contentHandler,
                                               (NW_DOM_ElementNode_t*) childNode);

      status = NW_XHTML_ElementHandler_CreateBoxTree (elementHandler, contentHandler,
                                           childNode, parentBox);
      _NW_THROW_ON_ERROR (status);
      }
    else
      {
      continue;
      }
    }
  }
  NW_CATCH (status)
  {
  }
  NW_FINALLY
  {
    return status;
  } NW_END_TRY
}

