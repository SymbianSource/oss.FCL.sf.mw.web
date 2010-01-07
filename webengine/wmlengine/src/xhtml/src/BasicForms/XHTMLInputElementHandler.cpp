/*
* Copyright (c) 2000-2004 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nw_basicforms_inputelementhandleri.h"

#include "nw_basicforms_xhtmlformliaison.h"
#include "nw_text_ascii.h"
#include "nw_fbox_checkbox.h"
#include "nw_fbox_radiobox.h"
#include "nw_fbox_inputbox.h"
#include "nw_fbox_buttonbox.h"
#include "nw_fbox_imagebuttonbox.h"
#include "nw_fbox_passwordbox.h"
#include "nw_fbox_fileselectionbox.h"
#include "nw_fbox_hiddenbox.h"
#include "nw_hed_hedeventhandler.h"
#include "nw_evt_activateevent.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"
#include "nw_hed_documentroot.h"
#include "nwx_settings.h"

#include "nw_image_imgelementhandler.h"
#include "nw_image_cannedimages.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "nw_markup_wmlvalidator.h"
#include "HEDDocumentListener.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_http_defs.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_XHTML_inputElementHandler_Class_t NW_XHTML_inputElementHandler_Class = {
  { /* NW_Object_Core               */
    /* super                        */ &NW_XHTML_ElementHandler_Class,
    /* queryInterface               */ _NW_Object_Core_QueryInterface
  },
  { /* NW_XHTML_ElementHandler      */
    /* initialize                   */ _NW_XHTML_inputElementHandler_Initialize,
    /* createBoxTree                */ _NW_XHTML_inputElementHandler_CreateBoxTree,
    /* processEvent                 */ _NW_XHTML_ElementHandler_ProcessEvent
  },
  { /* NW_XHTML_inputElementHandler */
    /* unused                       */ 0
  }
};

/* ------------------------------------------------------------------------- *
   singleton definition
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
// Gets size attribute and converts to integer.  If no size attribute or its
// value is not a valid integer, then zero is returned, indicating that some
// default size should be used instead.
static
NW_Uint16
NW_XHTML_inputElementHandler_GetSize(NW_XHTML_ContentHandler_t* contentHandler,
                                     NW_DOM_ElementNode_t* elementNode)
{
  NW_Ucs2* sizeAttr = NULL;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Uint16 size = 0;

  status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
                                              elementNode,
                                              NW_XHTML_AttributeToken_size,
                                              &sizeAttr);
  /* If size is not set or is invalid, it is set to the default size. */
  if (status == KBrsrSuccess) 
  {
		if(NW_Str_StrIsValidLength(sizeAttr))
		{
			size = (NW_Uint16) NW_Str_Atoi(sizeAttr);
		}
    NW_Str_Delete (sizeAttr);
  } 
  return size;
}

/* ------------------------------------------------------------------------- */
// Sets maxLength and title - Assumption is that the box is InputBox
static
TBrowserStatusCode
NW_XHTML_inputElementHandler_SetMaxLengthTitle(NW_XHTML_ContentHandler_t* contentHandler,
                                     NW_DOM_ElementNode_t* elementNode,
                                     NW_LMgr_Box_t* box)
{
  NW_Ucs2* maxLengthAttr = NULL;
  NW_Ucs2* titleAttr = NULL;
  NW_FBox_InputBox_t* inputBox= NW_FBox_InputBoxOf(box);

  NW_TRY (status ) 
  {
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
                                                  elementNode,
                                                  NW_XHTML_AttributeToken_maxlength,
                                                  &maxLengthAttr);
    NW_THROW_ON (status, KBrsrOutOfMemory);

    /* If we couldn't find maxlength attribute, we leave it, so that default
       value will hold good */
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
    /* Get and set title */
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
                                                  elementNode,
                                                  NW_XHTML_AttributeToken_title,
                                                  &titleAttr);
    NW_THROW_ON (status, KBrsrOutOfMemory);
    if (status == KBrsrSuccess) 
    {
      NW_FBox_InputBox_SetTitle (box, titleAttr);
    }  
  }
  NW_CATCH (status) 
  {
  }
  NW_FINALLY 
  {
    return status;
  } 
  NW_END_TRY
}


/* ------------------------------------------------------------------------- */
// Based on the type attribute, this method returns the Input Box
static
TBrowserStatusCode
NW_XHTML_inputElementHandler_CreateBox(NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* elementNode,
                                        NW_LMgr_Box_t** box,
                                        NW_HED_DocumentRoot_t* docRoot,
                                        NW_FBox_FormLiaison_t* formLiaison,
                                        NW_LMgr_EventHandler_t* eventHandler)
{
  NW_Uint16 size;
  NW_Text_t* value = NULL;
  NW_Ucs2* typeAttr = NULL;
  NW_HED_AppServices_t* appServices = docRoot->appServices;
  TBool useDefault = NW_FALSE;

  NW_TRY (status ) 
  {
    // Get value for the button
	if (NW_XHTML_ElementToken_button == NW_DOM_ElementNode_getTagToken(elementNode))
	{
		// Get text
        value = NW_XHTML_ElementHandler_GetSubTreeText(contentHandler, elementNode);
	}
    status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, 
                                                  elementNode, 
                                                  NW_XHTML_AttributeToken_type,
                                                  &typeAttr);
    NW_THROW_ON (status, KBrsrOutOfMemory);

    /* Text is default if no type specified. */
    if ((typeAttr == NULL) || (typeAttr[0] == '\0') || 
         ( NW_Str_StricmpConst(typeAttr, "text") == 0)) 
    {
      useDefault = NW_TRUE;
    }
    /* Create checkbox. */
  	else if ( NW_Str_StricmpConst(typeAttr, "checkbox") == 0 ) 
    {
  	  *box = (NW_LMgr_Box_t*)
            NW_FBox_CheckBox_New(1, eventHandler, elementNode, formLiaison);
      NW_THROW_OOM_ON_NULL (*box, status);
    }
    /* Create radio box. */
    else if ( NW_Str_StricmpConst(typeAttr, "radio") == 0 ) 
    { 
      *box = (NW_LMgr_Box_t*)
            NW_FBox_RadioBox_New(1,eventHandler, elementNode, formLiaison);
      NW_THROW_OOM_ON_NULL (*box, status);
    }
    /* Ignore hidden. */
    else if ( NW_Str_StricmpConst(typeAttr, "hidden") == 0 ) 
    { 
      *box = (NW_LMgr_Box_t*)
            NW_FBox_HiddenBox_New(1,eventHandler, elementNode, formLiaison, appServices);
      NW_THROW_OOM_ON_NULL (*box, status);
    }
    // Create password box. 
    else if ( NW_Str_StricmpConst(typeAttr, "password") == 0 ) 
    {
      // Get Input Size
      size = NW_XHTML_inputElementHandler_GetSize(contentHandler, elementNode);

      *box = (NW_LMgr_Box_t*)
            NW_FBox_PasswordBox_New(1,eventHandler, elementNode, formLiaison, size, appServices);
      NW_THROW_OOM_ON_NULL (*box, status);
      
      // Set MaxLength And Title
      status = NW_XHTML_inputElementHandler_SetMaxLengthTitle(contentHandler, elementNode, *box);
      NW_THROW_ON (status, KBrsrOutOfMemory);
    }
    /* Create submit button. */
    else if ( NW_Str_StricmpConst(typeAttr, "submit") == 0 ) 
    {
        // in case of <button type = "submit"> we have the subtree value
        // that should be given a priority
        if (!value)
        {
      NW_XHTML_GetDOMAttribute(contentHandler, elementNode, NW_XHTML_AttributeToken_value, &value);
        }
      *box = (NW_LMgr_Box_t*)
            NW_FBox_ButtonBox_New(1,eventHandler, elementNode, formLiaison, 
                                  NW_FBox_ButtonBox_Submit, value, NULL, appServices);
      NW_THROW_OOM_ON_NULL (*box, status);
    }
    // Create reset button. 
    else if ( NW_Str_StricmpConst(typeAttr, "reset") == 0 ) 
    {
        // in case of <button type = "submit"> we have the subtree value
        // that should be given a priority
        if (!value)
        {
      NW_XHTML_GetDOMAttribute(contentHandler, elementNode, NW_XHTML_AttributeToken_value, &value);
        }
      *box = (NW_LMgr_Box_t*)
            NW_FBox_ButtonBox_New(1,eventHandler, elementNode, formLiaison, 
                                  NW_FBox_ButtonBox_Reset, value, NULL, appServices);
      NW_THROW_OOM_ON_NULL (*box, status);
    }
    /* Create generic button. */
    else if (( NW_Str_StricmpConst(typeAttr, "button") == 0 )  ||
        (NW_XHTML_ElementToken_button == NW_DOM_ElementNode_getTagToken(elementNode)))
    {
        if (!value)
        {
      NW_XHTML_GetDOMAttribute(contentHandler, elementNode, NW_XHTML_AttributeToken_value, &value);
        }
      *box = (NW_LMgr_Box_t*)
            NW_FBox_ButtonBox_New(1,eventHandler, elementNode, formLiaison, 
                                  NW_FBox_ButtonBox_Button, value, NULL, appServices);
      NW_THROW_OOM_ON_NULL (*box, status);
    }
    /* Create file-selection box. */
    else if ( NW_Str_StricmpConst(typeAttr, "file") == 0 )
    {
      size = NW_XHTML_inputElementHandler_GetSize(contentHandler, elementNode);
      *box = (NW_LMgr_Box_t*)
            NW_FBox_FileSelectionBox_New(1, eventHandler, elementNode, formLiaison, size, appServices);
      NW_THROW_OOM_ON_NULL (*box, status);
    }
    /* Create image button. */
    else if ( NW_Str_StricmpConst(typeAttr, "image") == 0 )
    {
      NW_ImageCH_Epoc32ContentHandler_t* imgCH;
      NW_HED_ContentHandler_t* hedCH;
      NW_HED_CompositeNode_t* compositeNode;
      NW_Image_AbstractImage_t* image;  
      NW_Image_CannedImages_t *cannedImages;
      
      /* extract the image from our canned images */
      cannedImages = (NW_HED_DocumentRoot_GetRootBox(docRoot))->cannedImages;
      NW_ASSERT(cannedImages!=NULL);
    
      compositeNode = (NW_HED_CompositeNode_t*)
            NW_Object_QueryAggregate (contentHandler, &NW_HED_CompositeNode_Class);
      NW_ASSERT (compositeNode != NULL);
    
      hedCH = (NW_HED_ContentHandler_t*)
        NW_HED_CompositeNode_LookupChild (compositeNode, elementNode); 
      imgCH = NW_ImageCH_Epoc32ContentHandlerOf (hedCH);

      if(imgCH && imgCH->image) 
      {
        /*case when image load is a success and we have a valid image*/
        image =  imgCH->image;
      }
      else 
      { /* (!imgCH || (imgCH && !imgCH->image)) */
        /*case when the load has not occured, display missing image and alt tag exists*/      
        image = (NW_Image_AbstractImage_t*)
          NW_Image_CannedImages_GetImage(cannedImages, NW_Image_Missing);
        NW_ASSERT(image !=NULL);
      }
      /*get the alt text*/
      NW_XHTML_GetDOMAttribute(contentHandler, elementNode, NW_XHTML_AttributeToken_alt, &value);
      /*create box*/
      *box = (NW_LMgr_Box_t*)
             NW_FBox_ImageButtonBox_New(1, eventHandler, elementNode, formLiaison, value, image);
      NW_THROW_OOM_ON_NULL (*box, status);
    }
    else {                   /* unrecognized type attribute value, treat as default */
      useDefault = NW_TRUE;
    }

    /* Create text box.  Text is default if no type or an unknown type is specified. */
    if (useDefault == NW_TRUE) {
      // Get Input Size
      size = NW_XHTML_inputElementHandler_GetSize(contentHandler, elementNode);

      *box = (NW_LMgr_Box_t*)
          NW_FBox_InputBox_New(1, eventHandler, elementNode, formLiaison, size, appServices);
      NW_THROW_OOM_ON_NULL (*box, status);
      
      /* Set MaxLength And Title */
      status = NW_XHTML_inputElementHandler_SetMaxLengthTitle(contentHandler, elementNode, *box);
      NW_THROW_ON (status, KBrsrOutOfMemory);
    }

  }
  NW_CATCH (status) 
  {
    if (*box)
    {
      NW_Object_Delete(*box);
      *box = NULL;
      value = NULL;
    }
    if (value)
    {
      NW_Object_Delete(value);
    }
  }
  NW_FINALLY 
  {
   NW_Str_Delete( typeAttr );
   return status;
  } 
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_XHTML_inputElementHandler_HandleValidation (NW_FBox_InputBox_t* box,
                                               NW_FBox_FormLiaison_t* formLiaison)
{
  const NW_Text_t* format = NULL;
  NW_FBox_Validator_EmptyOk_t emptyOk;
  NW_Markup_WmlValidator_t* validator = NULL;
  NW_Text_t* buffer = NULL;
  NW_Bool dummy = NW_FALSE;  /* used by NW_FBox_FormLiaison_GetStringValue to indicate if a value attribute was specified */
  void* formControlId = NULL;
  NW_FBox_Validator_Mode_t  mode;

  NW_TRY (status ) 
  {
    /* Set form control to initial values.  Done here because need to have setup input
       format string so won't set to an illegal value.  jwild 23-Jan-2002 */
    formControlId = NW_FBox_FormBox_GetFormCntrlID(box);

    /* Text or password box.  Check for format and input required style.
       Add input format validators to input box and form liaison. */
      
    /* Format does not have to be freed. */
    status = NW_FBox_InputBox_GetFormat(box, &format, &mode, &emptyOk);
    _NW_THROW_ON_ERROR (status);

    /* Create validators for input box and form liaison. */
    validator = NW_Markup_WmlValidator_New( format, mode, emptyOk, NW_FALSE);  /* Prefix string match. */
    NW_THROW_OOM_ON_NULL (validator, status);
    
    /* check to see if the box already has a validator
       if so, delete the old validator so it is not leaked */
    if (box->validator != NULL)
      {
      NW_Object_Delete(box->validator);
      }

    /* Takes ownership of validator. */
    NW_FBox_InputBox_SetValidator(box, NW_FBox_ValidatorOf(validator));
    validator = NW_Markup_WmlValidator_New(format, mode, emptyOk, NW_TRUE);  /* Full string match. */
    NW_THROW_OOM_ON_NULL (validator, status);
    
    /* Takes ownership of validator. */
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
    _NW_THROW_ON_ERROR (status);

    status = NW_FBox_FormLiaison_GetStringValue(formLiaison, formControlId, &buffer, &dummy);
    _NW_THROW_ON_ERROR (status);

    status = NW_FBox_InputBox_SetDefaultVal(box, buffer);
    _NW_THROW_ON_ERROR (status);
  }
  NW_CATCH (status) 
  {
  }
  NW_FINALLY 
  {
    return status;
  } 
  NW_END_TRY
}


/* ------------------------------------------------------------------------- */
const NW_XHTML_inputElementHandler_t NW_XHTML_inputElementHandler = {
  { { &NW_XHTML_inputElementHandler_Class } }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_XHTML_inputElementHandler_Initialize (const NW_XHTML_ElementHandler_t* elementHandler,
                                          NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_DOM_ElementNode_t* elementNode)
{
  TBrowserStatusCode status;
  NW_Ucs2* typeAttr = NULL;
  NW_FBox_FormLiaison_t* formLiaison;
  NW_Int32 comResult;

  NW_REQUIRED_PARAM (elementHandler);

  formLiaison = NW_XHTML_ContentHandler_GetFormLiaison(contentHandler, elementNode);
  if (formLiaison == NULL)
  {
    return KBrsrSuccess;
  }
  (void) NW_FBox_FormLiaison_AddControl(formLiaison, elementNode);

  status = NW_XHTML_ElementHandler_GetAttribute(contentHandler, elementNode, 
                                                NW_XHTML_AttributeToken_type,
                                                &typeAttr);
  
  if (status == KBrsrSuccess) 
  {
    comResult = NW_Str_StricmpConst(typeAttr, "image");
  } else if (status == KBrsrNotFound) 
  {
    comResult = 1;  /* No type attribute, so type!="image". */
  } else 
  {
    NW_Str_Delete( typeAttr );
    return status;
  }
  
  NW_Str_Delete( typeAttr );  /* No longer needed. */
  typeAttr = NULL;

  if ( comResult == 0 ) 
  {
    NW_Text_t* url = NULL;

    /* NW_Settings_GetImageEnabled() called to determine whether 
    autoloading of images is turned on. If not, then just display
    the initial "x" and alt text, and don't issue any load requests.
    */
    if (!NW_Settings_GetImagesEnabled()) 
    {
      return KBrsrSuccess;
    }
    /* find the 'src' attribute and get its string value */
    status = NW_XHTML_GetDOMAttribute (contentHandler, elementNode,
      NW_XHTML_AttributeToken_src, &url);
    if (status != KBrsrSuccess) 
    {
      return status;
    }

    /* load the image */
    status = NW_XHTML_ContentHandler_StartLoad (contentHandler,
        url, NW_HED_UrlRequest_Reason_DocLoadChild, elementNode, 
        NW_UrlRequest_Type_Image);

    /* cleanup after ourselves */
    NW_Object_Delete (url);    

    if (status == KBrsrOutOfMemory) 
    {
      return KBrsrOutOfMemory;
    }    
  }
  
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_XHTML_inputElementHandler_CreateBoxTree (const NW_XHTML_ElementHandler_t* elementHandler,
                                             NW_XHTML_ContentHandler_t* contentHandler,
                                             NW_DOM_ElementNode_t* elementNode,
                                             NW_LMgr_ContainerBox_t* parentBox)
{
  NW_LMgr_Box_t* box = NULL;
  NW_HED_EventHandler_t* eventHandler = NULL;
  NW_FBox_FormLiaison_t* formLiaison = NULL;
  NW_HED_DocumentRoot_t* docRoot = NULL;

  NW_TRY (status ) 
  {
    // Get the Form Liaison associated with the form element that encloses this input
    formLiaison = NW_XHTML_ContentHandler_GetFormLiaison(contentHandler, elementNode);
    if (formLiaison == NULL)
    {
      NW_THROW_SUCCESS(status);     
    }

    /* create the activeBox */
    eventHandler = NW_HED_EventHandler_New(NW_HED_DocumentNodeOf(contentHandler), elementNode);
    NW_THROW_OOM_ON_NULL (eventHandler, status);

    /* get the DocumentRoot as we need to pass AppServices to the box */
    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (contentHandler);
    NW_ASSERT(docRoot != NULL);
    NW_ASSERT (NW_Object_IsInstanceOf (docRoot, &NW_HED_DocumentRoot_Class)); 

    /* This method creates appropriate input boxes based on type attribute */
    status = NW_XHTML_inputElementHandler_CreateBox(contentHandler, elementNode, &box, docRoot,
                                       formLiaison, NW_LMgr_EventHandlerOf(eventHandler));
    NW_THROW_ON (status, KBrsrOutOfMemory);
    if (box == NULL)
    {
      if(status == KBrsrSuccess)
      {
        // Add the NULL box to boxvector- for hidden box
        NW_XHTML_FormLiaison_AddBox(formLiaison, box, elementNode);
      }
      NW_THROW_SUCCESS (status);
    }

    /* Add the box to the tree */
    status = NW_LMgr_ContainerBox_AddChild (parentBox, box);
    _NW_THROW_ON_ERROR (status);

    /* apply common attributes and styles */
    status = NW_XHTML_ElementHandler_ApplyStyles (elementHandler, contentHandler, elementNode, &box, 0);
    NW_THROW_ON (status, KBrsrOutOfMemory);

    // Apply Styles will take care of replacing container box if display was block, list-item or marquee.
    // In case of 'display:none' or an error it removes the container box. That's why we check box
    if (box == NULL)
    {
      NW_THROW_SUCCESS (status);
    }

    if (NW_Object_IsInstanceOf(box, &NW_FBox_InputBox_Class))
    {
      status = NW_XHTML_inputElementHandler_HandleValidation(NW_FBox_InputBoxOf(box), formLiaison);
      _NW_THROW_ON_ERROR (status);
    }

    // Adds the box to the associated node in formLiaison
    NW_XHTML_FormLiaison_AddBox(formLiaison, box, elementNode);
  }
  NW_CATCH (status) 
  {
    if(box) 
    {
      NW_Object_Delete(box);
      eventHandler = NULL;
    }
    if (eventHandler)
    {
      NW_Object_Delete(eventHandler);
    }
  }
  NW_FINALLY 
  {
    return status;
  } 
  NW_END_TRY
}


