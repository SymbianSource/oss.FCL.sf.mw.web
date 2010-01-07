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

#include <e32def.h> // resolves NULL redefinition warning.
 
#include "nw_wml1x_wml1xcontenthandler.h"
#include "nw_wml1x_wml1xeventhandler.h"
#include "nw_wml1x_wml1xactiveevent.h"
#include "nw_wml1x_wml1xdefaultstylesheet.h"
#include "nw_fbox_checkbox.h"
#include "nw_fbox_radiobox.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_breakbox.h"
#include "nw_hed_appservices.h"
#include "nw_hed_documentroot.h"
#include "nw_fbox_buttonbox.h"
#include "nw_fbox_selectbox.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_system_optionlist.h"
#include "nw_wae.h"
#include "wml_elm_attr.h"
#include "WmlEngineInterface.h"
#include "nw_dom_domvisitor.h"
#include "nw_hed_domhelper.h"
#include "wml_deck.h"
#include "nw_wml_decoder.h"
#include "wml_decoder.h"
#include "nw_xhtml_xhtml_1_0_tokens.h"

#include "KimonoLocalizationStrings.h"

#include <e32std.h>
#include <coemain.h>
#include <bautils.h>
#include "BrsrStatusCodes.h"

NW_Ucs2*
NW_Str_FurtherTrim(NW_Ucs2 *string)
{
  NW_Text_Length_t trimmedLen; 
  NW_Text_Length_t i;
  NW_Text_Length_t length;  
  NW_Ucs2* Trimedstr = NULL;

  /*Trim leading and trailing whitespace*/
  Trimedstr = NW_Str_Trim(string, NW_Str_Both); 
  /*Deal with the tab, CR, LF*/
  trimmedLen = 0;
  length = NW_Str_Strlen(Trimedstr);
  for (i = 0; i < length; i++) {
	if (Trimedstr[i] == NW_TEXT_UCS2_SOFT_HYPHEN)
	{
	  continue;
	}
    else if ((Trimedstr[i] == NW_TEXT_UCS2_TAB) || (Trimedstr[i] == NW_TEXT_UCS2_NBSP) ||
		     (Trimedstr[i] == NW_TEXT_UCS2_LF) || (Trimedstr[i] == NW_TEXT_UCS2_CR)) {
      Trimedstr[trimmedLen++] = ' ';
	}
    else {
      Trimedstr[trimmedLen++] = Trimedstr[i];
	}
  }
  Trimedstr[trimmedLen] = 0;  
  NW_Str_CompressWhitespace(Trimedstr);
  return Trimedstr;
}

/************************************************************************
  Function: NW_Wml1x_GetOptionBox
  Purpose: Handle <option> element within NW_Wml1x_HandleSelect()
  Parameters:   ctx    - pointer to the Wml1x Content Handler.
                elId   - WML element Id property of box
                box    - LMgr box pointer
  Return Values:  None
**************************************************************************/
static
NW_FBox_OptionBox_t*
NW_Wml1x_GetOptionBox (void         *ctx, 
                      NW_Uint16     elId,
                      NW_LMgr_Box_t *box)
    {
    NW_Ucs2                   *retString = NULL;
    NW_Ucs2                   *trimmedRetString = NULL;
    NW_FBox_OptionBox_t*      optionBox = NULL;
    NW_Text_t                 *optionData = NULL;

    NW_TRY (status) {
    NW_Wml1x_ContentHandler_t *thisObj;
    NW_Wml1x_EventHandler_t   *eventHandler;
    NW_LMgr_Box_t             *childBox = NULL;
    NW_Uint16                 childElId = 0;
    NW_LMgr_PropertyValue_t   value;
    NW_Uint32 elIdAsWord      = elId;               /* NOTE: this exists solely to eliminate a compiler warning */
    void      *elIdAsVoidStar = (void *)elIdAsWord; /* NOTE: this exists solely to eliminate a compiler warning */
    NW_ASSERT(ctx != NULL);
        
    thisObj = NW_Wml1x_ContentHandlerOf(ctx);
        
    /* create the eventHandler for the <option> element */
    eventHandler = NW_Wml1x_EventHandler_New(NW_Wml1x_ContentHandlerOf(thisObj));
    NW_THROW_OOM_ON_NULL ( eventHandler, status );
        
        /* get the NW_Text_t optionData from the <option> content or title */
    if (NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(box)) > 0)
        {
        /* <option> element had content */
        childBox = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), 0);
            
        /* if child count is greater than zero, then zeroth child had better not be null */
        NW_ASSERT(childBox != NULL);
            
            /* if the child is a text box, get the text string value */
        if (NW_Object_IsInstanceOf(childBox, &NW_LMgr_TextBox_Class))
            {
            /* if the box tree were complete at this point, we could get the 
            ** text from the box; but since the text box is a child of the option 
            ** box, the text object has not yet been set on it by NW_UI_ShowCard().
            ** So we must use the WML query service to get the text
            */
            /* get the child element id */
            status = NW_LMgr_Box_GetPropertyValue(childBox, 
                    NW_CSS_Prop_elementId, 
                    NW_CSS_ValueType_Integer, 
                    &value);
            _NW_THROW_ON_ERROR(status);
                
            /* get the child element type */
            childElId = NW_UINT16_CAST(value.integer);
                
            status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                    childElId,
                    STR_VALUE,
                    &retString);
             if (status == KBrsrOutOfMemory)
                {
                NW_THROW(status);
                }
             if (retString != NULL)
                {
                  trimmedRetString = NW_Str_FurtherTrim(retString);
                  NW_THROW_OOM_ON_NULL ( trimmedRetString, status );
                  NW_Mem_Free(retString);
                  retString = NULL;

                  optionData = (NW_Text_t*)
                  NW_Text_UCS2_New (trimmedRetString, (NW_Text_Length_t)NW_Str_Strlen(trimmedRetString),
                  NW_Text_Flags_TakeOwnership);
                  NW_THROW_OOM_ON_NULL ( optionData, status );
                  trimmedRetString = NULL;   /* prevent deletion */
                }
            }
        }
    if (optionData == NULL)
        {
        /* no children OR first child not a text box OR null text on text box: 
           if there is a title attribute, obtain the title string */
       

        status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                                 elId,
                                 TITLE_ATTR,
                                 &retString);
        if (status == KBrsrOutOfMemory)
            {
            NW_THROW(status);
            }

        if (retString == NULL)
            {
            retString = NW_Str_NewcpyConst(" "); /* set some text if none returned */
            NW_THROW_OOM_ON_NULL ( retString, status );
            }
        optionData = (NW_Text_t*)
            NW_Text_UCS2_New (retString, (NW_Text_Length_t)NW_Str_Strlen(retString), 
            NW_Text_Flags_TakeOwnership);
            NW_THROW_OOM_ON_NULL( optionData, status );
            retString = NULL;   /* prevent deletion */
         }
        
    /* create the NEW optionBox */
    optionBox = NW_FBox_OptionBox_New(1,
            NW_LMgr_EventHandlerOf (eventHandler),
            elIdAsVoidStar,
            NW_FBox_FormLiaisonOf (thisObj->formLiaison),
            optionData);
    NW_THROW_OOM_ON_NULL( optionBox, status );
    optionData = NULL;    /* prevent deletion */
    } NW_CATCH (status) {
    } NW_FINALLY {
    /* not necessary to delete original box's children before returning, 
    ** since they will be deleted when the original box is deleted 
     */
            
    /* deallocate retString if we still own it */
    if (retString != NULL)
        {
        NW_Mem_Free(retString);
        }
     /* deallocate trimmedRetString if we still own it */
    if (trimmedRetString != NULL)
        {
        NW_Mem_Free(trimmedRetString);
        trimmedRetString = NULL;
        }
   /* deallocate optionData if we still own it */
    if (optionData != NULL)
        {
        NW_Object_Delete(optionData);
        }
    return optionBox;
    } NW_END_TRY
}



/************************************************************************
  Function: NW_Wml1x_GetOptgroupBox
  Purpose: Handle <optgroup> element within NW_Wml1x_HandleSelect()
  Parameters:   ctx    - pointer to the Wml1x Content Handler.
                elId   - WML element Id property of box
                box    - LMgr box pointer
  Return Values:  None
**************************************************************************/
static
NW_FBox_OptGrpBox_t*
NW_Wml1x_GetOptgroupBox (void         *ctx, 
                        NW_Uint16     elId,
                        NW_LMgr_Box_t *box)
{
  NW_Ucs2                   *retString = NULL;
  NW_Text_t                 *optgrpTitle = NULL;
  NW_ADT_Vector_Metric_t    numChildren = 0;
  NW_LMgr_Box_t             *childBox = NULL;
  NW_FBox_OptGrpBox_t       *optgrpBox = NULL;

  NW_TRY (status) {
    NW_Wml1x_ContentHandler_t *thisObj;
    NW_Wml1x_EventHandler_t   *eventHandler;
  
    NW_ADT_Vector_Metric_t    currentChild = 0;
    NW_LMgr_PropertyValue_t   value;
    NW_Wml_ElType_e           childElType = UNKNOWN_ELEMENT;
    NW_Uint16                 childElId = 0;
    NW_Wml_Element_t          *childEl;
    NW_Uint32 elIdAsWord      = elId;               /* NOTE: this exists solely to eliminate a compiler warning */
    void      *elIdAsVoidStar = (void *)elIdAsWord; /* NOTE: this exists solely to eliminate a compiler warning */

    NW_ASSERT(ctx != NULL);

    thisObj = NW_Wml1x_ContentHandlerOf(ctx);

    /* don't bother with this element if no children */
    if ((numChildren = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(box))) == 0)
    {
      NW_THROW(status);
    }

    /* get title for the optgrpBox */
    /* see CUIS 27.0 2.10.1.2 "Optgroup Element": label then title */
    status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               elId,
                               LABEL_ATTR,
                               &retString);
    if (status == KBrsrOutOfMemory) {
      NW_THROW(status);
    }

    if (retString == NULL) {
      status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                                   elId,
                                   TITLE_ATTR,
                                   &retString);
      if (status == KBrsrOutOfMemory) {
        NW_THROW(status);
      }
    }

    if (retString != NULL) {
      optgrpTitle = (NW_Text_t*)
                        NW_Text_UCS2_New (retString, (NW_Text_Length_t)NW_Str_Strlen(retString), 
                        NW_Text_Flags_TakeOwnership);
      NW_THROW_OOM_ON_NULL( optgrpTitle, status );
      retString = NULL;   /* prevent deletion */
    }

    /* create the eventHandler for the <optgroup> element */
    eventHandler = NW_Wml1x_EventHandler_New(NW_Wml1x_ContentHandlerOf(thisObj));
    NW_THROW_OOM_ON_NULL( eventHandler, status );

    /* create the NEW optgrpBox */
    optgrpBox = NW_FBox_OptGrpBox_New(1,
                                      NW_LMgr_EventHandlerOf (eventHandler),
                                      elIdAsVoidStar,
                                      NW_FBox_FormLiaisonOf (thisObj->formLiaison),
                                      optgrpTitle);
    NW_THROW_OOM_ON_NULL( optgrpBox, status );
    optgrpTitle = NULL; /* prevent deletion */

    /* iterate over the child nodes of the original <optgroup> box, adding option elements to the new optgrpBox */
    currentChild = 0;
    while (currentChild < numChildren)
    {
    childBox = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), currentChild);
    if (childBox)
    {
      /* get the child element id */
      status = NW_LMgr_Box_GetPropertyValue(childBox, 
                                            NW_CSS_Prop_elementId, 
                                            NW_CSS_ValueType_Integer, 
                                            &value);
      _NW_THROW_ON_ERROR(status);

      /* get the child element type */
      childElId = NW_UINT16_CAST(value.integer);
      status = NW_Wml_GetElementType(&(thisObj->wmlInterpreter),
                                       childElId,
                                       &childElType,
                                       &childEl);
      _NW_THROW_ON_ERROR(status);

      /* WAP-191-WML allows nested <optgroup> elements */
      if (childElType == OPTGRP_ELEMENT)
      {
        NW_FBox_OptGrpBox_t* childOptgrpBox;

        childOptgrpBox = NW_Wml1x_GetOptgroupBox(thisObj,
                                                 childElId,
                                                 childBox);
        NW_THROW_OOM_ON_NULL( childOptgrpBox, status );

        NW_FBox_OptGrpBox_AddOptGrp(optgrpBox, childOptgrpBox); 

      }
      else if (childElType == OPTION_ELEMENT)
      {
        NW_FBox_OptionBox_t* optionBox;

        optionBox = NW_Wml1x_GetOptionBox(thisObj,
                                          childElId,
                                          childBox);
        NW_THROW_OOM_ON_NULL( optionBox, status );

        NW_FBox_OptGrpBox_AddOption(optgrpBox, optionBox); 

      }
      else
      {
        NW_ASSERT(0);
        status = KBrsrWmlbrowserBadContent;
        NW_THROW(status);
      }
    }
    else
    {
      NW_ASSERT(childBox);
      status = KBrsrFailure;   /* we asked for a childBox using a valid index and got a NULL */
      NW_THROW(status);
    }
    currentChild++;
    }
  } NW_CATCH (status) {
  } NW_FINALLY {
    /* not necessary to delete original box's children before returning, 
    ** since they will be deleted when the original box is deleted 
    */

    /* deallocate retString if we still own it */
    if (retString != NULL)
    {
    NW_Mem_Free(retString);
    }
    /* deallocate optgrpTitle if we still own it */
    if (optgrpTitle != NULL)
    {
    NW_Object_Delete(optgrpTitle);
    }
    return optgrpBox;
  } NW_END_TRY
}
 
/************************************************************************
  Function:       NW_Wml1x_HandleDo ()
  Purpose:        Handle do element UI event.
  Parameters:     ctx    - pointer to the Wml1x Content Handler.
                  elId   - WML element Id property of box
                  box    - LMgr box pointer
                  deletebox - pointer to pointer to box to be deleted
  Return Values:  None
**************************************************************************/
TBrowserStatusCode  NW_Wml1x_HandleDo (void          *ctx,
                                NW_Uint16     elId,
                                NW_LMgr_Box_t *box,
                                NW_LMgr_Box_t **deleteBox)
{
  TBrowserStatusCode   status;
  NW_Wml1x_ContentHandler_t *thisObj;
  NW_Ucs2* labelString = NULL;
  NW_Ucs2* typeString = NULL;
  NW_Uint16 elementType = 0;

  static const NW_Ucs2 acceptStr[] = {'a','c','c','e','p','t',0};
  static const NW_Ucs2 prevStr[] = {'p','r','e','v',0};
  static const NW_Ucs2 helpStr[] = {'h','e','l','p',0};
  static const NW_Ucs2 resetStr[] = {'r','e','s','e','t',0};
  static const NW_Ucs2 optionsStr[] = {'o','p','t','i','o','n','s',0};
  static const NW_Ucs2 deleteStr[] = {'d','e','l','e','t','e',0};
  static const NW_Ucs2 unknownStr[] = {'u','n','k','n','o','w','n',0};
  static const NW_Ucs2 experimentalStr[] = {'x','-'};
  static const NW_Ucs2 vendorStr[] = {'v','n','d','.'};

  NW_ASSERT(ctx != NULL);
  thisObj = NW_Wml1x_ContentHandlerOf(ctx);

  NW_REQUIRED_PARAM (box);
  NW_REQUIRED_PARAM (deleteBox);
  
  /* get label attribute */
  status = NW_Wml_GetAttribute (&(thisObj->wmlInterpreter),
                                      elId,
                                      LABEL_ATTR,
                                      &labelString);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* get type attribute */
  status = NW_Wml_GetAttribute (&(thisObj->wmlInterpreter),
                                        elId,
                                        TYPE_ATTR,
                                        &typeString);

  if (status != KBrsrSuccess) {
    return status;
  }
 
  /* determine element type */
  if (typeString == NULL) {
	  elementType = TWmlEngineDefs::WML_OPTION_UNKNOWN;
  }
  else if (!NW_Str_Strcmp(typeString, acceptStr)) {
    elementType = TWmlEngineDefs::WML_OPTION_ACCEPT;
  }
  else if (!NW_Str_Strcmp(typeString, prevStr)) {
    elementType = TWmlEngineDefs::WML_OPTION_PREV;
  }
  else if (!NW_Str_Strcmp(typeString, helpStr)) {
    elementType = TWmlEngineDefs::WML_OPTION_HELP;
  }
  else if (!NW_Str_Strcmp(typeString, resetStr)) {
    elementType = TWmlEngineDefs::WML_OPTION_RESET;
  }
  else if (!NW_Str_Strcmp(typeString, optionsStr)) {
    elementType = TWmlEngineDefs::WML_OPTION_OPTIONS;
  }
  else if (!NW_Str_Strcmp(typeString, deleteStr)) {
    elementType = TWmlEngineDefs::WML_OPTION_DELETE;
  }
  else if ((*typeString == 0) ||(!NW_Str_Strcmp(typeString, unknownStr))) {
    elementType = TWmlEngineDefs::WML_OPTION_UNKNOWN;
  }
  else if (!NW_Str_Strnicmp(typeString,experimentalStr,NW_Str_Strlen(experimentalStr))) {
    elementType = 0;
  }
  else if (!NW_Str_Strnicmp(typeString,vendorStr,NW_Str_Strlen(vendorStr))) {
    elementType = 0;
  }
  else {
    elementType = TWmlEngineDefs::WML_OPTION_UNKNOWN;
  }


  /* if there's a label - use it */
  if (labelString != NULL) { 
    /* make a copy of the retString pointer to array for future clean up */
    if (thisObj->optionItemList == NULL) {
      thisObj->optionItemList = NW_Ds_DarNew_DefaultHeap(16);
    }
  
    /* Add the item pointer to the array */
    NW_Ds_DarAppend(thisObj->optionItemList, labelString);

    /* delete unnecessary typeString */
    NW_Str_Delete(typeString);
  }
  else {
    /* if element type is defined - use it to determine label */
    if (elementType!=0) {
      NW_Str_Delete(typeString);
    }
    /* if element type is not known - use it as a label */
    else {
      labelString = typeString;
    }
    /* make a copy of the retString pointer to array for future clean up */
    if (thisObj->optionItemList == NULL) {
      thisObj->optionItemList = NW_Ds_DarNew_DefaultHeap(16);
    }
    /* Add the item pointer to the array */
    NW_Ds_DarAppend(thisObj->optionItemList, labelString);
  }
  
  NW_HED_DocumentRoot_t *docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(thisObj);
 
  NW_ASSERT(docRoot != NULL);
  NW_ASSERT(docRoot->appServices != NULL);

  NW_System_OptionList_AddItem(labelString, (NW_Uint32) elId, elementType);

  return status;
}


/************************************************************************
  Function: NW_Wml1x_HandleSelect
  Purpose: Handle <select> element within UI ShowCard
  Parameters:   ctx    - pointer to the Wml1x Content Handler.
                elId   - WML element Id property of box
                box    - LMgr box pointer
                currentSelectMultipleState  - ptr to var to get value of MULTIPLE attribute
                currentSelectTabIndexVal    - ptr to var to get value of TABINDEX attribute
  Return Values:  None
**************************************************************************/
TBrowserStatusCode  NW_Wml1x_HandleSelect (void          *ctx, 
                                    NW_Uint16     elId,
                                    NW_LMgr_Box_t *box,
                                    NW_LMgr_Box_t **deleteBox,
                                    NW_Bool       *currentSelectMultipleState,
                                    NW_Int32      *currentSelectTabIndexVal)
{
  NW_ADT_Vector_Metric_t    numChildren = 0;
  NW_FBox_SelectBox_t       *selectBox = NULL;
  NW_LMgr_Box_t             *childBox = NULL;

  NW_TRY (status) {
    NW_Ucs2                   *retString = NULL;
    NW_Ucs2                   *titleStr = NULL;
    NW_Wml1x_ContentHandler_t *thisObj = NULL;
    NW_HED_DocumentRoot_t     *docRoot;
    NW_LMgr_ContainerBox_t    *parentBox = NULL;
    NW_FBox_SelectBox_t       *tempSelectBox = NULL;
    NW_Wml1x_EventHandler_t   *eventHandler;
    NW_Text_t                 *temptext;
    NW_Text_t                 *titleText;
    NW_ADT_Vector_Metric_t    currentChild = 0;
    NW_LMgr_PropertyValue_t   value;
    NW_Wml_ElType_e           childElType = UNKNOWN_ELEMENT;
    NW_Uint16                 childElId = 0;
    NW_Wml_Element_t          *childEl = NULL;
    NW_Uint32 elIdAsWord      = elId;               /* NOTE: this exists solely to eliminate a compiler warning */
    void      *elIdAsVoidStar = (void *)elIdAsWord; /* NOTE: this exists solely to eliminate a compiler warning */

    NW_ASSERT(ctx != NULL);
    NW_REQUIRED_PARAM (box);

    thisObj = NW_Wml1x_ContentHandlerOf(ctx);
    docRoot = (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode(thisObj);

    NW_ASSERT(docRoot != NULL);
    NW_ASSERT(docRoot->appServices != NULL);


    *currentSelectMultipleState = NW_FALSE;  /* defaults to NW_FALSE */
    *currentSelectTabIndexVal = 0;

    /* don't bother with this element if no children */
    if ((numChildren = NW_LMgr_ContainerBox_GetChildCount(NW_LMgr_ContainerBoxOf(box))) == 0)
    {
      NW_THROW(status);
    }

    /* determine value of multiple attribute */
    retString = NULL;
    status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               elId,
                               MULTIPLE_ATTR,
                               &retString);
    _NW_THROW_ON_ERROR(status);

    if (retString && (NW_Str_StrcmpConst(retString, WAE_ASC_TRUE_STR) == 0) )
      *currentSelectMultipleState = NW_TRUE;

    NW_Str_Delete(retString);
    retString = NULL;

    /* get the TABINDEX_ATTR value if any */
    status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter), 
                               elId, 
                               TABINDEX_ATTR,
                               &retString);
    _NW_THROW_ON_ERROR(status);

    if (retString != NULL)
    {
      /* convert string to tab index value */
      *currentSelectTabIndexVal = NW_Str_Atoi(retString);
      *currentSelectTabIndexVal = (*currentSelectTabIndexVal > 0) ? *currentSelectTabIndexVal : 0;
      NW_Str_Delete(retString);
      retString = NULL;
    }

    /* get the name attribute if any */
    status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               elId,
                               NAME_ATTR,
                               &retString);
    _NW_THROW_ON_ERROR(status);

    if (retString == NULL)
    {
      retString = NW_Str_NewcpyConst(" "); /* set some text if none returned */
      NW_THROW_OOM_ON_NULL (retString, status);
    }

    temptext = (NW_Text_t*) NW_Text_UCS2_New (retString, 0, NW_Text_Flags_TakeOwnership);
    NW_THROW_OOM_ON_NULL (temptext, status);

    /* get the title attribute if any */
    status = NW_Wml_GetAttribute(&(thisObj->wmlInterpreter),
                               elId,
                               TITLE_ATTR,
                               &titleStr);
    _NW_THROW_ON_ERROR(status);

    titleText = NULL;
    if(titleStr != NULL) {
      titleText = (NW_Text_t*) NW_Text_UCS2_New (titleStr, 0, NW_Text_Flags_TakeOwnership);
      NW_THROW_OOM_ON_NULL ( titleText, status );
    }

    /* create the eventHandler for the select box */
    eventHandler = NW_Wml1x_EventHandler_New(NW_Wml1x_ContentHandlerOf(thisObj));
    NW_THROW_OOM_ON_NULL ( eventHandler, status );

    /* create the selectBox and insert in box tree */
    selectBox =
          NW_FBox_SelectBox_New(1,
                                NW_LMgr_EventHandlerOf (eventHandler),
                                elIdAsVoidStar,  /* elementNode, */
                                NW_FBox_FormLiaisonOf (thisObj->formLiaison),
                                docRoot->appServices,
                                *currentSelectMultipleState,
                                temptext,
                                titleText);
    NW_THROW_OOM_ON_NULL ( selectBox, status );

    /* first, determine the parent of the original container box
    * and then insert the new select box in front of the original container box
    * finally, mark the original container box for later deletion
    */
    parentBox = NW_LMgr_Box_GetParent(box);

    status = NW_LMgr_ContainerBox_InsertChild(parentBox,
                                              NW_LMgr_BoxOf(selectBox),
                                              box);
    _NW_THROW_ON_ERROR(status);
    /* add style properties only after box has been added to box tree (in case of relative styles) */
    NW_Wml1x_SelectDefaultStyle(NW_LMgr_BoxOf(selectBox));

    /* add a break box per CUIS 
    childBox = (NW_LMgr_Box_t*)NW_LMgr_BreakBox_New((NW_ADT_Vector_Metric_t)1);
    NW_THROW_OOM_ON_NULL ( childBox, status );

    status = NW_LMgr_ContainerBox_InsertChild(parentBox, childBox, box);
    _NW_THROW_ON_ERROR(status);*/

    tempSelectBox = selectBox;  /* save pointer for later use */
    selectBox = NULL;   /* prevent deletion during cleanup */

    /* iterate over the child nodes, adding optgroup and option elements to the select box */
    currentChild = 0;
    while (currentChild < numChildren)
    {
    childBox = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), currentChild);
    if (childBox)
    {
      /* get the child element id */
      status = NW_LMgr_Box_GetPropertyValue(childBox, 
                                            NW_CSS_Prop_elementId, 
                                            NW_CSS_ValueType_Integer, 
                                            &value);
      _NW_THROW_ON_ERROR(status);

      /* get the child element type */
      childElId = NW_UINT16_CAST(value.integer);
      status = NW_Wml_GetElementType(&(thisObj->wmlInterpreter),
                                     childElId,
                                     &childElType,
                                     &childEl);
      _NW_THROW_ON_ERROR(status);

      /* handle <optgroup> and <option> elements only */
      if (childElType == OPTGRP_ELEMENT)
      {
        NW_FBox_OptGrpBox_t* optgrpBox;

        optgrpBox = NW_Wml1x_GetOptgroupBox(thisObj, childElId, childBox);

        NW_THROW_OOM_ON_NULL (optgrpBox, status);

        NW_FBox_SelectBox_AddOptGrp(tempSelectBox, optgrpBox); 

      }
      else if (childElType == OPTION_ELEMENT)
      {
        NW_FBox_OptionBox_t* optionBox;

        optionBox = NW_Wml1x_GetOptionBox(thisObj, childElId, childBox);

        NW_THROW_OOM_ON_NULL (optionBox, status);

        NW_FBox_SelectBox_AddOption(tempSelectBox, optionBox,NULL); 

      }
      else
      {
        NW_ASSERT((childElType == OPTGRP_ELEMENT) || (childElType == OPTION_ELEMENT));
        status = KBrsrWmlbrowserBadContent;
        NW_THROW(status);
      }
    }
    else
    {
      NW_ASSERT(childBox);
      NW_THROW_STATUS( status, KBrsrFailure ); /* we asked for a childBox using a valid index and got a NULL */
    }
    currentChild++;
    }

  } NW_CATCH (status) {
  } NW_FINALLY {
    /* delete original box's children before returning, so the boxVisitor won't get confused */
    while (numChildren > 0)
    {
      numChildren--;  /* predecrement for use as index */
      childBox = NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBoxOf(box), numChildren);
      if (childBox)
      {
        NW_LMgr_ContainerBox_RemoveChild (NW_LMgr_ContainerBoxOf(box), childBox);
        NW_Object_Delete(childBox);
      }
    }

    *deleteBox = box;   /* mark old container for later deletion */

    /* deallocate selectBox if we still own it */
    if (selectBox != NULL)
    {
      NW_Object_Delete(selectBox);
    }
    return status;
  } NW_END_TRY
}


/************************************************************************
  Function: NW_Wml1x_HandleOption
  Purpose: Handle <option> element within UI ShowCard
  Parameters:   ctx    - pointer to the Wml1x Content Handler.
                elId   - WML element Id property of box
                box    - LMgr box pointer
                currentSelectMultipleState  - value of MULTIPLE attribute
                currentSelectTabIndexVal    - value of TABINDEX attribute
  Return Values:  None
**************************************************************************/
TBrowserStatusCode  NW_Wml1x_HandleOption (void          *ctx, 
                                    NW_Uint16     elId,
                                    NW_LMgr_Box_t *box,
                                    NW_Bool       currentSelectMultipleState,
                                    NW_Int32      currentSelectTabIndexVal)
{
  NW_REQUIRED_PARAM(ctx);
  NW_REQUIRED_PARAM(elId);
  NW_REQUIRED_PARAM(box);
  NW_REQUIRED_PARAM(currentSelectMultipleState);
  NW_REQUIRED_PARAM(currentSelectTabIndexVal);

  return KBrsrFailure;   /* this function ought not get invoked by NW_UI_Show_Card() or anyone else */
}


/************************************************************************
  Function: NW_Wml1x_HandleOptgroup
  Purpose: Handle <optgroup> element within UI ShowCard
  Parameters:   ctx    - pointer to the Wml1x Content Handler.
                elId   - WML element Id property of box
                box    - LMgr box pointer
                currentSelectMultipleState  - ptr to var containing value of MULTIPLE attribute
                currentSelectTabIndexVal    - ptr to var containing value of TABINDEX attribute
  Return Values:  None
**************************************************************************/
TBrowserStatusCode  NW_Wml1x_HandleOptgroup (void          *ctx, 
                                      NW_Uint16     elId,
                                      NW_LMgr_Box_t *box)
{

  NW_REQUIRED_PARAM(ctx);
  NW_REQUIRED_PARAM(elId);
  NW_REQUIRED_PARAM(box);

  return KBrsrFailure;   /* this function ought not get invoked by NW_UI_Show_Card() or anyone else */
}

/*****************************************************************

  Name:         NW_Wml1x_GetUrl()

  Description:  finds the URL for <a> or <anchor> elements, looking in <go> and <prev> if needed.
  
  Parameters:   
    wmlInterpreter - In      - pointer to the wml interpreter with deck info
    elId           - In      - the element id of the <a> or <anchor> element being processed
    retUrl         - Out     - pointer to the resulting URL
	isPrev         - Out     - pointer to Bool indicating a <prev> task.

  Algorithm:    

  Return Value: KBrsrSuccess
                KBrsrOutOfMemory

*****************************************************************/
TBrowserStatusCode
NW_Wml1x_GetURL(NW_Wml_t                *wmlInterpreter, 
                NW_Uint16               elId,
                NW_Ucs2                 **retURL,
				NW_Bool                 *isPrev)
{
  NW_Wml_Element_t          *el;
  NW_Wml_ElType_e           elType;
  NW_cXML_DOM_DOMVisitor_t  *visitor = NULL;
  NW_DOM_Node_t             *nextNode = NULL;
  NW_HED_DomHelper_t        *domHelper;
  
  NW_TRY (status) {
	  
      *isPrev = NW_FALSE;

      /* establish *el */
	  status = NW_Wml_GetElementType(wmlInterpreter, elId, &elType, &el);
	  _NW_THROW_ON_ERROR(status);
	  
	  /* get the domHelper and set the context */
	  domHelper = NW_Wml_Deck_GetDomHelper (wmlInterpreter->decoder->domDeck, wmlInterpreter->var_list);
	  status = NW_Wml_GetAttribute(wmlInterpreter, 
	                               elId, 
							       HREF_ATTR,
							       retURL);
	   _NW_THROW_ON_ERROR(status);
	  if (*retURL == NULL)
	  {
		  /* this could be an <anchor> element, which has no HREF_ATTR.  Find the <go> element if any and get its href */
		  
		  /* create another dom vistor to cruise this sub- DOM tree for the task element */
		  visitor = NW_cXML_DOM_DOMVisitor_New ((NW_DOM_Node_t*) el->domElement);
		  NW_THROW_OOM_ON_NULL (visitor, status);
		  
		  /* while there are more element nodes and <go> has not been found */
		  status = KBrsrSuccess;
		  while ((nextNode = NW_cXML_DOM_DOMVisitor_Next(visitor, NW_DOM_ELEMENT_NODE)) != NULL)
		  {
			  if (NW_HED_DomHelper_GetElementToken(nextNode) == NW_Wml_1_3_ElementToken_go)
			  {
				  /* get the Href from DOM */
				  NW_String_t value;
				  
				  status = NW_HED_DomHelper_GetAttributeValue(domHelper, 
					  (NW_DOM_ElementNode_t*)nextNode, 
					  NW_Wml_1_3_AttributeToken_href, 
					  &value);
				  if (status == KBrsrSuccess) {
					  *retURL = (NW_Ucs2*) value.storage;
					  value.storage = NULL;
				  }
				  _NW_THROW_ON_ERROR(status);
				  break;
			  }
			  else if (NW_HED_DomHelper_GetElementToken(nextNode) == NW_Wml_1_3_ElementToken_prev)
			  {
				  *isPrev = NW_TRUE;
				  /* it's a <prev> - caller decides what to use. */
				  break;
			  }
		  }
		  
		  NW_Object_Delete (visitor);
		  visitor = NULL;
	  }
	  
  } NW_CATCH (status) {
  } NW_FINALLY {

    /* get the domHelper again, passing null in the var-list param so the context 
    ** is deleted
    */
    (void) NW_Wml_Deck_GetDomHelper (wmlInterpreter->decoder->domDeck, NULL);
    NW_Object_Delete (visitor);

    return status;
  } NW_END_TRY
}
/*****************************************************************

  Name:         NW_Wml1x_FixupAnchorText()

  Description:  CUIS requires that an <a> or <anchor> element that
                  lacks content be represented by the title attribute
                  value or the href attribute value if no title.
                  This function enforces that behavior.

  Parameters:   
    wmlInterpreter - In      - pointer to the wml interpreter with deck info
    elId           - In      - the element id of the <a> or <anchor> element being processed
    containerBox   - In      - pointer to the <a> or <anchor> box
    appServices    - In      - provides access point to "GetLocalizationString" for "Back" option labeling

  Algorithm:    

  Return Value: KBrsrSuccess
                KBrsrOutOfMemory


*****************************************************************/
TBrowserStatusCode 
NW_Wml1x_FixupAnchorText (NW_Wml_t                *wmlInterpreter, 
                          NW_Uint16               elId,
                          NW_LMgr_ContainerBox_t  *containerBox,
                          NW_HED_AppServices_t*   appServices)
{
  NW_Bool                   found = NW_FALSE;
  NW_Wml_Element_t          *el;
  NW_Wml_ElType_e           elType;
  NW_cXML_DOM_DOMVisitor_t  *visitor = NULL;
  NW_DOM_Node_t             *nextNode = NULL;
  NW_String_t               childText;
  NW_HED_DomHelper_t        *domHelper;
  NW_LMgr_TextBox_t         *newTextBox = NULL;
  NW_Ucs2                   *retString = NULL;
  NW_Text_t                 *temptext = NULL;
  NW_Uint8                  textFlag = NW_Text_Flags_TakeOwnership;
  NW_Bool                   isPrev = NW_FALSE;
  
  static const NW_Ucs2 BackStr[] = {'B','a','c','k',0};

  /* if the containerbox has no TEXT or IMAGE children, insert as child
  ** a text box containing the title attribute (1st priority) or 
  ** the href attribute value (2nd priority))
  ** If there is neither a title nor an href, perhaps this was an <anchor>
  ** element, so look for a contained <go> task (use its href) or a <prev>
  ** task (use "text_softkey_back").
  */

  NW_TRY (status) {

    /* establish *el */
    status = NW_Wml_GetElementType(wmlInterpreter, elId, &elType, &el);
    _NW_THROW_ON_ERROR(status);

    /* get the domHelper and set the context */
    domHelper = NW_Wml_Deck_GetDomHelper (wmlInterpreter->decoder->domDeck, wmlInterpreter->var_list);

    if (NW_LMgr_ContainerBox_GetChildCount(containerBox) != 0)
    {
      /* create a dom vistor to cruise this sub- DOM tree for text or image content */
      visitor = NW_cXML_DOM_DOMVisitor_New ((NW_DOM_Node_t*) el->domElement);
      NW_THROW_OOM_ON_NULL (visitor, status);

      /* while there are more nodes and neither Image nor Text have been found */
      while ((nextNode = NW_cXML_DOM_DOMVisitor_Next(visitor, 
                                                      NW_DOM_TEXT_NODE | NW_DOM_ELEMENT_NODE)) != NULL)
      {
        if (NW_HED_DomHelper_GetElementToken (nextNode) == NW_Wml_1_3_ElementToken_img)
        {
          found = NW_TRUE;
          break;
        }

        if (NW_DOM_Node_getNodeType(nextNode) == NW_DOM_TEXT_NODE)
        {
          /* get the text */
          status = NW_HED_DomHelper_GetText (domHelper, nextNode, NW_TRUE, &childText, NULL);
          _NW_THROW_ON_ERROR(status);

          /* check the text */
          if ((childText.storage != NULL) && ((childText.storage[0] != '\0') || (childText.storage[1] != '\0')))
          {
            found = NW_TRUE;
          }
          /* discard the text */
          NW_String_deleteStorage (&childText);
          if (found)
          {
            break;
          }
        }
      }

      NW_Object_Delete (visitor);
      visitor = NULL;

    }

    if (!found)    /* no content - fixup required */
    {
      /* get the TITLE_ATTR value if any */
      retString = NULL;
      status = NW_Wml_GetAttribute(wmlInterpreter, 
                                   elId, 
                                   TITLE_ATTR,
                                   &retString);
      _NW_THROW_ON_ERROR(status);
      if (retString == NULL)
      {
		    status = NW_Wml1x_GetURL(wmlInterpreter, 
                                 elId,
                                 &retString,
								                 &isPrev);
		    if (isPrev)
		    {
			    retString = appServices->getLocalizedStringAPI.getLocalizedString( EKmLocBackSoftkeyLabel );
          if (!retString)
          {
			      retString = (NW_Ucs2*) BackStr;
			      textFlag = 0;
          }
		    }
      }

      if (retString != NULL)
      {
        temptext = (NW_Text_t*)NW_Text_UCS2_New(retString, 
                                                NW_Str_Strlen(retString),
                                                textFlag);
        NW_THROW_OOM_ON_NULL (temptext, status);

        newTextBox = NW_LMgr_TextBox_New(1, NW_TextOf(temptext));
        NW_THROW_OOM_ON_NULL (newTextBox, status);

        temptext = NULL;

        status = NW_LMgr_ContainerBox_AddChild(NW_LMgr_ContainerBoxOf(containerBox),
                                              NW_LMgr_BoxOf(newTextBox));
        _NW_THROW_ON_ERROR(status);
        newTextBox = NULL;
      }
    }

  } NW_CATCH (status) {
  } NW_FINALLY {

    /* get the domHelper again, passing null in the var-list param so the context 
    ** is deleted
    */
    (void) NW_Wml_Deck_GetDomHelper (wmlInterpreter->decoder->domDeck, NULL);
    NW_Object_Delete (visitor);

    NW_Object_Delete(temptext);
    NW_Object_Delete(newTextBox);
    return status;
  } NW_END_TRY
}

