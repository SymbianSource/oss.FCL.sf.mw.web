/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/*
    $Workfile: wml_decoder.c $

    Purpose:

        Class: DeckDecoder

        Implementation of DeckDecoder class. Declares and defines 
        private functions for reading shared elements.

*/

#include "wml_decoder.h"
#include "wml_var.h"
#include "nwx_string.h"
#include "wml_elm_attr.h"
#include "nw_text_ucs2.h"

#include "nw_hed_domtree.h"
#include "wml_deck_iter.h"
#include "nw_hed_contextvisitor.h"
#include "nw_wml1x_wml1xentityset.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
NW_DeckDecoder_t* NW_DeckDecoder_New (void)
{
  NW_DeckDecoder_t* decoder;

  decoder = (NW_DeckDecoder_t*) NW_Mem_Malloc (sizeof (NW_DeckDecoder_t));

  if (decoder != NULL) {
    NW_Mem_memset (decoder, 0x00, sizeof (NW_DeckDecoder_t));
  }
  
  return decoder;
}

/* ------------------------------------------------------------------------- */
void NW_DeckDecoder_Delete (NW_DeckDecoder_t* thisObj)
{
  if (thisObj == NULL) {
    return;
  }

  NW_Wml_Deck_Delete (thisObj->domDeck);
  NW_Mem_Free(thisObj);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_InitializeAndValidate (NW_DeckDecoder_t* thisObj, 
                                                  NW_Buffer_t* domBuffer, 
                                                  NW_Http_CharSet_t charset,
                                                  NW_Bool contentWasPlainText)
{
  TBrowserStatusCode       status = KBrsrSuccess;

  NW_ASSERT(thisObj != NULL);
  NW_ASSERT(domBuffer != NULL);
  
  if (domBuffer->length == 0 || domBuffer->data == NULL) {
    return KBrsrWmlbrowserBadContent;
  }

  /* create a dom-tree for this new content */
  thisObj->domDeck = NW_Wml_Deck_New (domBuffer, charset, contentWasPlainText);

  if (thisObj->domDeck == NULL) {
    status = KBrsrWmlbrowserBadContent;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetRootElement (const NW_DeckDecoder_t* thisObj, 
                                           NW_Wml_Element_t* root)
{
  NW_DOM_Node_t* r;
  TBrowserStatusCode status = KBrsrFailure;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (root != NULL);

  root->id = INVALID_ID;

  if ((r = NW_HED_DomTree_GetRootNode (NW_Wml_Deck_GetDomTree (thisObj->domDeck))) != NULL) {
    root->domElement = NW_DOM_Node_getFirstChild (r);
    status = KBrsrSuccess;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
NW_Bool NW_DeckDecoder_GetCardElement (const NW_DeckDecoder_t* thisObj, 
                                       NW_Wml_Element_t* elem)
{
  NW_DOM_Node_t* domElem;
  NW_Bool found = NW_FALSE;
  
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);

  if ((domElem = NW_Wml_Deck_GetCurrentCard (thisObj->domDeck)) != NULL) {
    elem->domElement = domElem;

    found = NW_TRUE;
  }

  return found;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetTemplateElement (const NW_DeckDecoder_t* thisObj, 
                                               NW_Wml_Element_t* elem)
{
  NW_DOM_Node_t* domElem;
  TBrowserStatusCode status = KBrsrFailure;
  
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);

  domElem = NW_HED_DomHelper_FindElement (NW_Wml_Deck_GetDomHelper (thisObj->domDeck, NULL), 
      NW_HED_DomTree_GetRootNode (NW_Wml_Deck_GetDomTree (thisObj->domDeck)), 2,
      TEMPLATE_ELEMENT, 0, NULL);

  if (domElem != NULL) {
    elem->domElement = domElem;

    status = KBrsrSuccess;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetHeadElement (const NW_DeckDecoder_t *thisObj, 
                                           NW_Wml_Element_t *elem)
{
  NW_DOM_Node_t* domElem;
  TBrowserStatusCode status = KBrsrFailure;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);

  domElem = NW_HED_DomHelper_FindElement (NW_Wml_Deck_GetDomHelper (thisObj->domDeck, NULL),
      NW_HED_DomTree_GetRootNode (NW_Wml_Deck_GetDomTree (thisObj->domDeck)), 2, 
      HEAD_ELEMENT, 0, NULL);

  if (domElem != NULL) {
    elem->domElement = domElem;
    status = KBrsrSuccess;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_SetCardByName (NW_DeckDecoder_t *thisObj, 
                                          const NW_Ucs2* cardName)
{
  NW_String_t card;
  TBrowserStatusCode status;
  
  NW_ASSERT (thisObj != NULL);

  NW_String_initialize (&card, (void *) cardName, HTTP_iso_10646_ucs_2);

  if ((status = NW_Wml_Deck_SetCurrentCardByName (thisObj->domDeck, &card)) == KBrsrSuccess) {
    thisObj->card_el.domElement = NW_Wml_Deck_GetCurrentCard (thisObj->domDeck);
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_SetCardByElement (NW_DeckDecoder_t* thisObj, 
                                      const NW_Wml_Element_t* newElement)
{
  NW_ASSERT (thisObj != NULL);

  NW_Wml_Deck_SetCurrentCardByElement (thisObj->domDeck, newElement->domElement);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetCardName (const NW_DeckDecoder_t *thisObj, 
                                        NW_Ucs2 **cardName)
{
  NW_Wml_Element_t card;
  TBrowserStatusCode status = KBrsrFailure;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (cardName != NULL);

  *cardName = NULL;

  /* get the id attribute of the current card */
  if (NW_DeckDecoder_GetCardElement (thisObj, &card)) {
    status = NW_DeckDecoder_GetAttribute (thisObj, &card, ID_ATTR, NULL, 
        NW_MEM_SEGMENT_MANUAL, cardName);
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetCardTitle (const NW_DeckDecoder_t* thisObj, 
                                          const NW_Wml_VarList_t* varList, 
                                          NW_Ucs2** title)
{
  NW_Wml_Element_t card;
  TBrowserStatusCode status = KBrsrFailure;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (title != NULL);

  NW_REQUIRED_PARAM (varList);

  *title = NULL;

  /* get the id attribute of the current card */
  if (NW_DeckDecoder_GetCardElement (thisObj, &card)) {
    status = NW_DeckDecoder_GetAttribute (thisObj, &card, TITLE_ATTR, varList, 
        NW_MEM_SEGMENT_MANUAL, title);
  }

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetDocumentPublicId (const NW_DeckDecoder_t* thisObj, 
                                                NW_Uint32* publicId)
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (publicId != NULL);

  *publicId = NW_HED_DomTree_GetPublicId (NW_Wml_Deck_GetDomTree (thisObj->domDeck));

  return status;
}

/* ------------------------------------------------------------------------- */
NW_Uint8 NW_DeckDecoder_GetVersion (const NW_DeckDecoder_t* thisObj)
{
  NW_Uint8 version;

  NW_ASSERT (thisObj != NULL);

  version = NW_HED_DomTree_GetVersion (NW_Wml_Deck_GetDomTree (thisObj->domDeck));

  return version;
}

/* ------------------------------------------------------------------------- */
NW_Uint16 NW_DeckDecoder_GetEncoding (const NW_DeckDecoder_t* thisObj)
{
  NW_Uint16 encoding;

  NW_ASSERT (thisObj != NULL);

  encoding = (NW_Uint16) NW_HED_DomTree_GetCharEncoding (NW_Wml_Deck_GetDomTree (thisObj->domDeck));

  return encoding;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_ReadString (const NW_DeckDecoder_t* thisObj, 
                                       const NW_Wml_Element_t* elem, 
                                       const NW_Wml_VarList_t* varList, 
                                       NW_Mem_Segment_Id_t memScope, 
                                       NW_Ucs2** retString)
{
  NW_String_t str;
  NW_HED_DomHelper_t* domHelper;
  TBrowserStatusCode status = KBrsrFailure;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);
  NW_ASSERT (retString != NULL);

  NW_REQUIRED_PARAM (varList);
  NW_REQUIRED_PARAM (memScope);

  /* get the domHelper and set the context */
  domHelper = NW_Wml_Deck_GetDomHelper (thisObj->domDeck, varList);

  /* get the string and convert it to ucs2 */
  status = NW_HED_DomHelper_GetText (domHelper, elem->domElement, NW_TRUE, &str, NULL);
  if (status == KBrsrSuccess) {
    *retString = (NW_Ucs2*) NW_String_getStorage (&str);
  }

  /* 
  ** get the domHelper again, passing null in the var-list pram so the context 
  ** is deleted right away 
  */
  (void) NW_Wml_Deck_GetDomHelper (thisObj->domDeck, NULL);

  return status;
}

/* ------------------------------------------------------------------------- */
NW_Bool NW_DeckDecoder_FirstContainedElement (const NW_DeckDecoder_t* thisObj, 
                                              const NW_Wml_Element_t* container, 
                                              NW_Wml_Element_t* elem)
{
  NW_DOM_Node_t* domElem;
  NW_Bool found = NW_FALSE;

  NW_ASSERT (thisObj != NULL);

	NW_REQUIRED_PARAM(thisObj);
  if ((domElem = NW_DOM_Node_getFirstChild (container->domElement)) != NULL) {
    elem->domElement = domElem;

    found = NW_TRUE;
  }

  return found;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetType (const NW_DeckDecoder_t* thisObj, 
                                    const NW_Wml_Element_t* elem, 
                                    NW_Wml_ElType_e* type)
{
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);
  NW_ASSERT (type != NULL);
  NW_ASSERT (elem->domElement != NULL);

	NW_REQUIRED_PARAM(thisObj);
  /* if the element is a text-node set the type to STRING_ELEMENT */
  if (NW_DOM_Node_getNodeType(elem->domElement) == NW_DOM_TEXT_NODE) {
    *type = STRING_ELEMENT;
  }
  
  /* otherwise get the element's type */
  else {
    *type = NW_HED_DomHelper_GetElementToken (elem->domElement);
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_SearchDoElements(NW_DeckDecoder_t* thisObj, 
                                            NW_Wml_VarList_t* varList, 
                                            NW_Wml_Element_t* container, 
                                            NW_Wml_Attr_t attribute, 
                                            NW_Ucs2* searchString)
{
  NW_Ucs2 *attrValue = 0;
  NW_Bool match = NW_FALSE;
  NW_Wml_Element_t elem;
  TBrowserStatusCode status;
  NW_Wml_ElType_e elType;
  NW_DeckDecoderIter_t iter;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (container != NULL);
  NW_ASSERT (searchString != NULL);

  if (NW_DeckDecoderIter_Initialize(&iter, thisObj, container) != KBrsrSuccess)
    return KBrsrFailure;

  while ((match == NW_FALSE) && NW_DeckDecoderIter_GetNextElement(&iter, &elem))
  {
    NW_DeckDecoder_GetType(thisObj, &elem, &elType);

    if (elType == DO_ELEMENT)
    {
      if ((status = NW_DeckDecoder_GetAttribute(thisObj, &elem, attribute, varList,
           NW_MEM_SEGMENT_MANUAL, &attrValue)) == KBrsrOutOfMemory)
      {
        return status;
      }
      if ((attrValue == NULL ) && (attribute == NAME_ATTR))
      {
        /* Getting the Do Name and there isn't one.  If no Do Name, use the the
           Do Type as name.  WML 1.1 Spec section 9.7 */
        if ((status = NW_DeckDecoder_GetAttribute(thisObj, &elem, TYPE_ATTR,
             varList, NW_MEM_SEGMENT_MANUAL, &attrValue)) == KBrsrOutOfMemory)
        {
          return status;
        }
      }
      if (attrValue && !NW_Str_Strcmp(searchString, attrValue))
        match = NW_TRUE;
      
      if (attrValue)
        NW_Mem_Free(attrValue);
    }
    else if(elType == P_ELEMENT || elType == FIELDSET_ELEMENT)
    {
      /* elements which can contain DO elements */
      if (NW_DeckDecoder_SearchDoElements(thisObj, varList, &elem, attribute,
          searchString) == KBrsrSuccess)
        match = NW_TRUE;
    }
    else if (elType == UNKNOWN_ELEMENT)
    {
      /* 
       * If this unknown element has content, recurse on it 
       */
      if (NW_DeckDecoder_HasContent(thisObj, &elem))
      {
        if (NW_DeckDecoder_SearchDoElements(thisObj, varList, &elem, attribute,
            searchString) == KBrsrSuccess)
          match = NW_TRUE;
      }
    }
  }

  if (match == NW_TRUE) {
    return KBrsrSuccess;
  } else {
    return KBrsrFailure;
  }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetPostfieldData (const NW_DeckDecoder_t* thisObj,
                                             const NW_Wml_VarList_t* varList, 
                                             const NW_Wml_Element_t* elem, 
                                             NW_NVPair_t** postfieldsArg)
{
  NW_Wml_Element_t el;
  TBrowserStatusCode status = KBrsrFailure;
  NW_Int32 pass;
  NW_NVPair_t* postFields;
  NW_DeckDecoderIter_t iter;
  NW_Ucs2* name = NULL;
  NW_Ucs2* value = NULL;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);
  NW_ASSERT (postfieldsArg != NULL);

  *postfieldsArg = NULL; 

  postFields = NW_NVPair_New();
  if ( postFields == NULL ) {
    status = KBrsrOutOfMemory;
    goto cleanup_memory;
  }

  for ( pass = 0; pass < 2; pass++) 
  {
    if (NW_DeckDecoderIter_Initialize(&iter, thisObj, elem) == KBrsrSuccess)
    {
      while (NW_DeckDecoderIter_GetNextElement(&iter, &el))
      {
        NW_Wml_ElType_e elType;

        NW_DeckDecoder_GetType(thisObj, &el, &elType);
        
        if (elType == POSTFIELD_ELEMENT) {   
          /* get name and value strings */
          status = NW_DeckDecoder_GetAttribute(thisObj, &el, NAME_ATTR, 
                                               varList, NW_MEM_SEGMENT_MANUAL, &name);
          if (status != KBrsrSuccess) {
            if (status != KBrsrOutOfMemory) {
              status = KBrsrWmlbrowserBadContent;
            }
            goto cleanup_memory;
          }

          NW_ASSERT(name != NULL);

          status = NW_DeckDecoder_GetAttribute(thisObj, &el, VALUE_ATTR, 
                                               varList, NW_MEM_SEGMENT_MANUAL, &value);
          if (status == KBrsrOutOfMemory) {
            goto cleanup_memory;
          }
 
          if (pass == 0) {
            /* 1st pass calculate the size. */
            status = NW_NVPair_CalSize(postFields, name, value);
            if ( status != KBrsrSuccess ) {
              goto cleanup_memory;
            }
          } else {
            /* 2nd pass add the name-value pairs. */
            if(postFields->iterPtr == NULL || postFields->strTable == NULL)
				{
				status = KBrsrWmlbrowserBadContent;
				}
			else
				{
				status = NW_NVPair_Add(postFields, name, value);
				}
            if ( status != KBrsrSuccess ) {
              goto cleanup_memory;
            }
          }

          NW_Str_Delete(name);
          name = NULL;

          NW_Str_Delete(value);
          value = NULL;
        }
      }
    } 
    
    if ( pass == 0 ) {
      /* Completed sizing pass(1st).  Allocate storage for name-value pairs. */
      status = NW_NVPair_AllocStorage( postFields );
      if ( status != KBrsrSuccess ) {
        goto cleanup_memory;
      }
    } else {
      /* Completed pass 2. Finished add pass. Reset iterator
         to beginning to be safe. */
      (void) NW_NVPair_ResetIter( postFields );
    }
  } /* for */
  
  if ( NW_NVPair_IsEmpty(postFields) ) {
    /* Does not contain any postfields.  Free and return null pointer. */
    NW_NVPair_Delete( postFields );
    *postfieldsArg = NULL;
  } else {
    *postfieldsArg = postFields;
  } 

  return KBrsrSuccess;

cleanup_memory:
  NW_Str_Delete(name);
  NW_Str_Delete(value);

  *postfieldsArg = NULL;  
  NW_NVPair_Delete( postFields );

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_GetAttribute (const NW_DeckDecoder_t* thisObj, 
                                         const NW_Wml_Element_t* elem, 
                                         NW_Wml_Attr_t attribute, 
                                         const NW_Wml_VarList_t* varList, 
                                         NW_Mem_Segment_Id_t memScope, 
                                         NW_Ucs2** attributeValue)
{
  NW_String_t value;
  NW_HED_DomHelper_t* domHelper;
  TBrowserStatusCode status;

  NW_REQUIRED_PARAM (memScope);

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);
  NW_ASSERT (attributeValue != NULL);

  *attributeValue = NULL;

  /* get the domHelper and set the context */
  domHelper = NW_Wml_Deck_GetDomHelper (thisObj->domDeck, varList);

  /* get the attribute value as a ucs2 NW_String_t */
  if ((status = NW_HED_DomHelper_GetAttributeValue (domHelper, elem->domElement, 
      attribute, &value)) == KBrsrSuccess) {

    *attributeValue = (NW_Ucs2*) value.storage;
  }

  /* 
  ** get the domHelper again, passing null in the var-list pram so the context 
  ** is deleted right away 
  */
  (void) NW_Wml_Deck_GetDomHelper (thisObj->domDeck, NULL);

  if (status == KBrsrNotFound) {
    status = KBrsrFailure;
  }

  return status;
}

/* ------------------------------------------------------------------------- */
NW_Bool NW_DeckDecoder_HasAttributeWithValue (const NW_DeckDecoder_t* thisObj, 
                                              const NW_Wml_Element_t* elem,
                                              NW_Wml_Attr_t attribute,
                                              const char* attributeValue,
                                              const NW_Wml_VarList_t* varList) 
{
  NW_Ucs2 *methodStr = NULL;
  NW_Bool hasAtt = NW_FALSE;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);

  if (NW_DeckDecoder_GetAttribute(thisObj, elem, attribute,
      varList, NW_MEM_SEGMENT_MANUAL, &methodStr) == KBrsrSuccess) {
    if (! NW_Str_StrcmpConst(methodStr, attributeValue)) {
      hasAtt = NW_TRUE;
    }
  }

  if (methodStr != NULL)
    NW_Mem_Free(methodStr);

  return hasAtt;
}

/* ------------------------------------------------------------------------- */
NW_Bool NW_DeckDecoder_HasContent (const NW_DeckDecoder_t* thisObj, 
                                   const NW_Wml_Element_t* elem)
{
  NW_Bool hasContent;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);
	NW_REQUIRED_PARAM(thisObj);

  hasContent = NW_DOM_Node_hasChildNodes (elem->domElement);

  return hasContent;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoder_CopyElement (const NW_DeckDecoder_t* thisObj, 
                                        const NW_Wml_Element_t* srcEl, 
                                        NW_Wml_Element_t* destEl)
{
  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (srcEl != NULL);
  NW_ASSERT (destEl != NULL);
	NW_REQUIRED_PARAM(thisObj);

  destEl->id = srcEl->id;
  destEl->domElement = srcEl->domElement;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoderIter_Initialize (NW_DeckDecoderIter_t *thisObj,
                                           const NW_DeckDecoder_t* decoder,
                                           const NW_Wml_Element_t* container)
{
  TBrowserStatusCode status;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (decoder != NULL);
  NW_ASSERT (container != NULL);

  thisObj->decoder = (NW_DeckDecoder_t*) decoder;
  status = NW_Wml_DeckIter_Initialize (&thisObj->deckIter, container->domElement, NW_TRUE);

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_DeckDecoderIter_InitializeAll_Children (NW_DeckDecoderIter_t *thisObj,
                                           const NW_DeckDecoder_t* decoder,
                                           const NW_Wml_Element_t* container)
{
  TBrowserStatusCode status;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (decoder != NULL);
  NW_ASSERT (container != NULL);

  thisObj->decoder = (NW_DeckDecoder_t*) decoder;
  status = NW_Wml_DeckIter_Initialize (&thisObj->deckIter, container->domElement, NW_FALSE);

  return status;
}


/* ------------------------------------------------------------------------- */
NW_Bool NW_DeckDecoderIter_GetNextElement (NW_DeckDecoderIter_t* thisObj, 
                                           NW_Wml_Element_t* elem)
{
  NW_DOM_Node_t* domElement;
  NW_Bool result= NW_FALSE;

  NW_ASSERT (thisObj != NULL);
  NW_ASSERT (elem != NULL);

  if ((domElement = NW_Wml_DeckIter_GetNextElement (&thisObj->deckIter)) != NULL) {
    elem->domElement = domElement;

    result = NW_TRUE;
  }

  return result;
}

/* ------------------------------------------------------------------------- */
NW_HED_Context_t*
NW_DeckDecoder_VarListToContext (const NW_Wml_VarList_t* varList)
{
  NW_Wml_Var_t* var = NULL;
  NW_HED_Context_t* context = NULL;
  TBrowserStatusCode status = KBrsrSuccess;

  if (varList == NULL) {
    return NULL;
  }

  if ((context = NW_HED_Context_New()) == NULL) {
    return NULL;
  }
 
  while ((var = NW_WmlVar_Next((NW_Wml_VarList_t*) varList, var)) != NULL) {
    NW_String_t name;
    NW_String_t value;

    if (var->name && var->val) {
      NW_String_initialize (&name, var->name, HTTP_iso_10646_ucs_2);
      NW_String_initialize (&value, var->val, HTTP_iso_10646_ucs_2);
    
      status = NW_HED_Context_SetOrAddVariable (context, &name, &value);
      if (status != KBrsrSuccess) {
        break;
      }
    }
  }
  
  if (status != KBrsrSuccess) {
    NW_Object_Delete (context);
    context = NULL;
  } 

  return context;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_DeckDecoder_ContextToVarList (const NW_HED_Context_t* context,
                                 NW_Wml_VarList_t* varList,
                                 NW_Mem_Segment_Id_t memScope)
{
  NW_HED_ContextVisitor_t vistor;
  NW_HED_Context_Variable_t* var;
  TBrowserStatusCode status = KBrsrSuccess;

  /* clear the varList */
  NW_WmlVar_Clear (varList);

  /* apply the global variables to the wml interpreter */
  NW_HED_ContextVisitor_Initialize (&vistor, (NW_HED_Context_t*) context);

  while((var = (NW_HED_Context_Variable_t *) NW_HED_ContextVisitor_NextVariable (&vistor)) != NULL) {
    NW_Ucs2* name;
    NW_Ucs2* value;

    name = (NW_Ucs2 *) NW_String_getStorage(&var->name);
    value = (NW_Ucs2* ) NW_String_getStorage(&var->value);

    /* add the variable to the interpreter */
    status = NW_WmlVar_AddToList(varList, name, value, memScope);
  }

  return status;
}

