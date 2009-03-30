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


#include "nw_basicforms_textareaelementhandleri.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "nw_fbox_textareabox.h"
#include "nw_hed_hedeventhandler.h"
#include "nwx_string.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nw_hed_documentroot.h"
#include "nw_markup_wmlvalidator.h"
#include "BrsrStatusCodes.h"
#include "nw_fbox_inputbox.h"

 /* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* GENERATED */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_textareaElementHandler_Class_t NW_XHTML_textareaElementHandler_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler     */
    /* initialize                  */ _NW_XHTML_textareaElementHandler_Initialize,
    /* createBoxTree               */ _NW_XHTML_textareaElementHandler_CreateBoxTree,
    /* processEvent                */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_textareaElementHandler */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_XHTML_textareaElementHandler_t NW_XHTML_textareaElementHandler = {
  { { &NW_XHTML_textareaElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_XHTML_textareaElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode)
{
  NW_FBox_FormLiaison_t* formLiaison;
  
  NW_REQUIRED_PARAM(elementHandler);

  formLiaison = 
    NW_XHTML_ContentHandler_GetFormLiaison(contentHandler, elementNode);

  if (formLiaison == NULL){
    return KBrsrSuccess;
  }

  // If this element is the last element in the chunk and this isn't the
  // last chunk then initialize it later.
  NW_Uint16 element = NW_HED_DomHelper_GetElementToken(elementNode);
  if ((element == NW_XHTML_ElementToken_textarea) &&
    (!NW_XHTML_ElementHandler_NextSiblingExists(contentHandler, elementNode, NW_TRUE))){
    return KBrsrSuccess;
  }

  (void) NW_FBox_FormLiaison_AddControl(formLiaison, elementNode);

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_XHTML_textareaElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                            NW_XHTML_ContentHandler_t* contentHandler,
                                            NW_DOM_ElementNode_t* elementNode,
                                            NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t* box = NULL;
  TBrowserStatusCode status;
  NW_HED_EventHandler_t* eventHandler = NULL;
  NW_Text_t* attrVal = NULL;
  const NW_Ucs2* storage = NULL;
  NW_Text_Length_t length;
  NW_Uint16 columnsVal = 0;
  NW_Uint16 rowsVal = 2; /* default number of rows for textarea */
  NW_FBox_FormLiaison_t* formLiaison = NULL;
  NW_HED_DocumentRoot_t* docRoot = NULL;
  NW_Ucs2* titleAttr = NULL;
  const NW_Text_t* format = NULL;
  NW_FBox_Validator_EmptyOk_t emptyOk;
  NW_Markup_WmlValidator_t* validator = NULL;
  NW_Text_t* buffer = NULL;
  void* formControlId = NULL;
  NW_Bool dummy;  /* used by NW_FBox_FormLiaison_GetStringValue to indicate if a value attribute was specified */
  NW_Ucs2* maxLengthAttr = NULL;
  NW_FBox_InputBox_t* inputBox;
  NW_FBox_Validator_Mode_t  mode;

  /* get the DocumentRoot */
  docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (contentHandler);
  NW_ASSERT(docRoot != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (docRoot, &NW_HED_DocumentRoot_Class)); 

  if (elementNode == NULL){
    return KBrsrSuccess;
  }

  formLiaison = NW_XHTML_ContentHandler_GetFormLiaison(contentHandler, elementNode);

  if (formLiaison == NULL){
    return KBrsrSuccess;
  }

  // If this element is the last element in the chunk and this isn't the
  // last chunk then initialize it later.
  NW_Uint16 element = NW_HED_DomHelper_GetElementToken(elementNode);
  if ((element == NW_XHTML_ElementToken_textarea) &&
    (!NW_XHTML_ElementHandler_NextSiblingExists(contentHandler, elementNode, NW_FALSE))) {
    return KBrsrSuccess;
  }

  /* create the activeBox */
  eventHandler = NW_HED_EventHandler_New(NW_HED_DocumentNodeOf(contentHandler), elementNode);
  if(eventHandler == NULL) {
    return KBrsrOutOfMemory;
  }

  NW_XHTML_GetDOMAttribute(contentHandler, elementNode, NW_XHTML_AttributeToken_cols, &attrVal);
  if (attrVal)
  {
    storage = NW_Text_GetUCS2Buffer(attrVal, 0, &length, NULL);
    if (storage)
    {
      NW_Int32 v;
      NW_Ucs2 *endPtr = NULL;

      v = NW_Str_Strtoul (storage, &endPtr, 10 );
      columnsVal = (NW_Uint16)v;
    }
    NW_Object_Delete(attrVal);
  }
  NW_XHTML_GetDOMAttribute(contentHandler, elementNode, NW_XHTML_AttributeToken_rows, &attrVal);
  if (attrVal)
  {
    storage = NW_Text_GetUCS2Buffer(attrVal, 0, &length, NULL);
    if (storage)
    {
      NW_Int32 v;
      NW_Ucs2 *endPtr = NULL;

      v = NW_Str_Strtoul (storage, &endPtr, 10 );
      rowsVal = (NW_Uint16)v;
    }
    NW_Object_Delete(attrVal);
  }

  box = (NW_LMgr_Box_t*)
        NW_FBox_TextAreaBox_New(0,
                                NW_LMgr_EventHandlerOf (eventHandler),
                                elementNode,
                                formLiaison,
                                columnsVal,
                                docRoot->appServices,
                                rowsVal);
  if (box == NULL) {
    NW_Object_Delete (eventHandler);
    return KBrsrOutOfMemory;
  }

  inputBox = NW_FBox_InputBoxOf(box);
  NW_ASSERT(NW_Object_IsInstanceOf(inputBox, &NW_FBox_InputBox_Class));

  status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
                                                  elementNode,
                                                  NW_XHTML_AttributeToken_maxlength,
                                                  &maxLengthAttr);
  if (status == KBrsrOutOfMemory)
  {
    return status;
  }
  if (status == KBrsrSuccess) 
  {
    NW_Int32 maxlength = -1; /* Initialize to invalid length */
    if(NW_Str_StrIsValidLength(maxLengthAttr))
    {
      maxlength = NW_Str_Atoi(maxLengthAttr);
    }
    NW_Str_Delete (maxLengthAttr);
    maxLengthAttr = NULL;
    if(maxlength >= 0)
    {
      NW_FBox_InputBox_SetMaxChars(inputBox, (NW_Text_Length_t)maxlength);
      NW_FBox_InputBox_SetIsMaxlength( inputBox, NW_TRUE);
    }
  }

  status = NW_LMgr_ContainerBox_AddChild (parentBox, box);
  if (status != KBrsrSuccess){
    NW_Object_Delete(box);
    return KBrsrFailure;
  }

  /* apply common attributes and styles */
  status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler, elementNode, &box, 0);
  if (status == KBrsrOutOfMemory)
  {
    return status;
  }
  
  // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
  // In case of 'display:none' or an error it removes the container box. That's why we check box
  if (box == NULL)
  {
    return KBrsrSuccess;
  }

  NW_XHTML_FormLiaison_AddBox(formLiaison, box, elementNode);

  if (NW_XHTML_ElementHandler_GetAttribute (contentHandler, elementNode,
                                            NW_XHTML_AttributeToken_title,
                                            &titleAttr) == KBrsrSuccess)
  {
    NW_FBox_InputBox_SetTitle (box, titleAttr);
  }

  /* Check for format and input required style.  Add input format
     validators to textarea box and form liaison. */
      
  /* Format does not have to be freed. */
  status = NW_FBox_InputBox_GetFormat( NW_FBox_InputBoxOf(box), &format, &mode, &emptyOk);


  if ( status != KBrsrSuccess ) {
    goto cleanup;
  }

  /* Create validators for textarea box and form liaison. */
  /* Prefix string match. */
  validator = NW_Markup_WmlValidator_New( format, mode, emptyOk, NW_FALSE);  

  if (validator == NULL) {
    status = KBrsrOutOfMemory;
    goto cleanup;
  }
  /* Takes ownership of validator. */
  NW_FBox_InputBox_SetValidator( box,
                                 (NW_FBox_Validator_t*) validator);

  /* Full string match. */
  validator = NW_Markup_WmlValidator_New( format, mode, emptyOk, NW_TRUE);  

  if (validator == NULL) {
    status = KBrsrOutOfMemory;
    goto cleanup;
  }
      
  /* Takes ownership of validator. */
  formControlId = NW_FBox_FormBox_GetFormCntrlID(box);
  NW_XHTML_FormLiaison_RegisterValidator(NW_XHTML_FormLiaisonOf(formLiaison),
                                         formControlId,
                                         NW_FBox_ValidatorOf(validator) );
  validator = NULL;
 

  /* Set form control and input box to initial values. 
     First set the form liaison to it's initial value and then use that
     value to set the input box. */
  status = NW_FBox_FormLiaison_SetInitialStringValue(formLiaison, formControlId,
																										 NW_FBox_InputBox_GetMaxChars(box));
  if (status == KBrsrWmlbrowserInputNonconformingToMask) 
  {
    /* Expected that may not match format mask.  Force to success so 
       will contine. */
    status = KBrsrSuccess;
  }
  if (status != KBrsrSuccess) {
    goto cleanup;
  }
  status = NW_FBox_FormLiaison_GetStringValue(formLiaison, formControlId, &buffer, &dummy);
  if (status != KBrsrSuccess) {
        goto cleanup;
  }
  status = NW_FBox_InputBox_SetDefaultVal(NW_FBox_InputBoxOf(box), buffer);
 
 
 cleanup:
  return status;
}

