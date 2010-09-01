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

#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include "nw_fbox_epoc32selectskini.h"
#include "nw_image_cannedimages.h"
#include "nw_fbox_selectboxi.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_string.h"
#include "nw_fbox_epoc32formimages.h"
#include "nw_gdi_types.h"
#include "GDIDeviceContext.h"
#include "nw_adt_resizablevector.h"
#include "nw_fbox_formboxutils.h"
#include "KimonoLocalizationStrings.h"
#include "nwx_logger.h"
#include "nw_lmgr_statictablecellbox.h"
#include "nw_browser_browserimages.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
    private data typed
 * ------------------------------------------------------------------------- */

static const NW_Ucs2 _NW_FBox_Epoc32SelectSkin_Separator[] = {',',' ','\0'};
#define NW_FBox_Epoc32SelectSkin_Separator_len ((sizeof(_NW_FBox_Epoc32SelectSkin_Separator)\
                                             /sizeof(NW_Ucs2))-1)

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
    private methods
 * ------------------------------------------------------------------------- */

NW_Ucs2*
NW_FBox_Epoc32SelectSkin_GetNewUcs2Str(NW_FBox_Epoc32SelectSkin_t* skin,
                                     NW_Text_t* text)
{
  NW_REQUIRED_PARAM(skin);

  return
    NW_Text_GetUCS2Buffer (text, NW_Text_Flags_Aligned | NW_Text_Flags_Copy,
                           NULL, NULL);
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_GetArrowSize(NW_FBox_SelectBox_t* selectBox,
                                      NW_GDI_Dimension3D_t* imageSize)
{
  const NW_Image_AbstractImage_t* image;
  NW_LMgr_RootBox_t* rootBox;

  NW_ASSERT(imageSize!=NULL);
  NW_ASSERT(selectBox!=NULL);

  /* get the root box */
  rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(selectBox));
  NW_ASSERT(rootBox);

  /* get the image from the canned images dictionary */
  NW_ASSERT(rootBox->cannedImages!=NULL);
  image = NW_Image_CannedImages_GetImage( rootBox->cannedImages,
                                          NW_Image_SelectArrow );
  NW_ASSERT(image);
  return NW_Image_AbstractImage_GetSize (image, imageSize);
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_GetTextExtent(NW_FBox_Epoc32SelectSkin_t* selectSkin,
                                       NW_FBox_SelectBox_t* selectBox,
                                       CGDIDeviceContext* deviceContext,
                                       NW_GDI_Dimension2D_t* extent,
                                       NW_Text_t* text)
  {
  NW_REQUIRED_PARAM(selectSkin);

  CGDIFont *font;
  TBrowserStatusCode status;
  TGDIFontInfo fontInfo;

  extent->width = 0;
  extent->height = 0;

  /* Set the font to match the parent. */
  font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(selectBox));
  NW_ASSERT(font != NULL);
  /* Get info */
  font->GetFontInfo ( &fontInfo);

  if (text == NULL)
    {
    return KBrsrSuccess;
    }

  status = deviceContext->GetTextExtent (text,
                                         font,
                                         0,
                                         NW_GDI_FlowDirection_LeftRight,
                                         extent);

  extent->height = fontInfo.height;

  return status;
  }

/* ------------------------------------------------------------------------- */
static
NW_Int32
NW_FBox_Epoc32SelectSkin_GetMaxTextWidth(NW_FBox_Epoc32SelectSkin_t* selectSkin,
                                         NW_FBox_SelectBox_t* selectBox)
  {
  NW_REQUIRED_PARAM(selectSkin);

  CGDIFont *font;
  CGDIDeviceContext* deviceContext;
  TGDIFontInfo fontInfo;
  NW_Int32 maxWidth = 0;
  NW_Text_t* text = NULL;
  NW_ADT_Vector_Metric_t index;
  NW_GDI_Dimension2D_t extent;
  NW_FBox_OptionBox_t* optionBox = NULL;

  /* we're going to need the deviceContext to do text extent calculations */
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (NW_LMgr_Box_GetRootBox(selectBox));
  NW_ASSERT (deviceContext != NULL);

  /* Set the font to match the parent. */
  font = NW_LMgr_Box_GetFont(NW_LMgr_BoxOf(selectBox));
  NW_ASSERT(font != NULL);
  font->GetFontInfo( &fontInfo);

  NW_Int32 numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(selectBox->children));

  for(index = 0; index < numItems; index++)
    {
      optionBox = *(NW_FBox_OptionBox_t**)NW_ADT_Vector_ElementAt(selectBox->children, index);

      if (!NW_Object_IsInstanceOf(optionBox, &NW_FBox_OptionBox_Class))
        continue;

      text = NW_FBox_OptionBox_GetText(optionBox);
      if (text == NULL)
        continue;

      extent.width = 0;
      extent.height = 0;
      deviceContext->GetTextExtent (text, font, 0, NW_GDI_FlowDirection_LeftRight, &extent);
      if (maxWidth < extent.width)
        {
          maxWidth = extent.width;
        }
    }

  return maxWidth;
  }

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_SetDisplayText(NW_FBox_Epoc32SelectSkin_t* thisObj)
  {
  CGDIDeviceContext* deviceContext = NULL;
  NW_FBox_SelectBox_t* selectBox = NULL;
  NW_GDI_Dimension2D_t extent;
  NW_GDI_Dimension2D_t charExtent;
  NW_GDI_Dimension3D_t imageSize;
  NW_Uint16 displayWidth = 0;
  NW_GDI_Rectangle_t innerRect;
  NW_Text_Length_t valueLength = 0;
  NW_Text_Length_t displayLength = 0;
  CGDIFont *font = NULL;
  NW_Ucs2* displayStorage = NULL;
  NW_Ucs2* valueStorage = NULL;

  NW_TRY(status)
    {
    selectBox = NW_FBox_SelectSkin_GetSelectBox(thisObj);
    NW_ASSERT (selectBox);

    /* we're going to need the deviceContext to do text extent calculations */
    deviceContext = NW_LMgr_RootBox_GetDeviceContext (NW_LMgr_Box_GetRootBox(selectBox));
    NW_ASSERT (deviceContext != NULL);

    /* Set the font to match the parent. TODO: this may not be the best policy */
    font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(selectBox));
    NW_ASSERT(font != NULL);

    /* extract the image from our canned images */
    status = NW_FBox_Epoc32SelectSkin_GetArrowSize(selectBox, &imageSize);
    NW_THROW_ON_ERROR (status);

    status = NW_FBox_SelectSkin_GetMCharExtent(NW_FBox_SelectSkinOf(thisObj), &charExtent);
    NW_THROW_ON_ERROR (status);

    if (thisObj->value == NULL)
      {
      /* nothing to display */
      NW_THROW_SUCCESS(status);
      }

    /* Find out how much space is required to display the value */
    status = NW_FBox_Epoc32SelectSkin_GetTextExtent(thisObj, selectBox, 
                                                   deviceContext, &extent, NW_TextOf(thisObj->value));
    NW_THROW_ON_ERROR (status);

    displayWidth = (NW_Uint16)(extent.width + imageSize.width);
    NW_LMgr_Box_GetInnerRectangle(NW_LMgr_BoxOf(selectBox), &innerRect);

    valueStorage =
      NW_Text_GetUCS2Buffer(NW_TextOf(thisObj->value), 0, &valueLength, NULL);

    /* The text fits within the space */
    if (displayWidth <= innerRect.dimension.width)
      {
      /* Make a copy of the value */
      if (valueStorage)
        {
        displayStorage = NW_Str_Newcpy(valueStorage);
        NW_THROW_OOM_ON_NULL(displayStorage, status);
        }

      if (thisObj->displayText)
        {
        status = NW_Text_SetStorage(NW_TextOf(thisObj->displayText), displayStorage,
                           valueLength, NW_Text_Flags_TakeOwnership);
        NW_THROW_ON_ERROR (status);
        }
      else
        {
        thisObj->displayText = NW_Text_UCS2_New(displayStorage, valueLength, NW_Text_Flags_TakeOwnership);
        NW_THROW_OOM_ON_NULL(thisObj->displayText, status);
        }
      NW_THROW_SUCCESS(status);
      }

    /* The text does not fit the select box. We have to truncate it */
    status = NW_FBox_FormBoxUtils_GetDisplayLength(deviceContext,
                                                   (NW_Text_t*)thisObj->value,
                                                   font,
                                                   valueLength,
                                                   (NW_Uint16)(innerRect.dimension.width - imageSize.width),
                                                   &displayLength, NW_FALSE);

    NW_THROW_ON_ERROR (status);
    displayStorage = NW_Str_New(displayLength + NW_FBox_FormBoxUtils_Truncate_len);
    NW_THROW_OOM_ON_NULL(displayStorage, status);

    NW_Mem_memcpy(displayStorage, (valueStorage), displayLength*sizeof(NW_Ucs2));
    NW_Mem_memcpy(displayStorage + displayLength, NW_FBox_FormBoxUtils_Truncate,
                                   NW_FBox_FormBoxUtils_Truncate_len*sizeof(NW_Ucs2));
    displayLength += NW_FBox_FormBoxUtils_Truncate_len;

    if (thisObj->displayText == NULL)
      {
      thisObj->displayText =  NW_Text_UCS2_New(displayStorage, displayLength, NW_Text_Flags_TakeOwnership);
      if (thisObj->displayText == NULL)
        {
        NW_Mem_Free(displayStorage);
        status = KBrsrOutOfMemory;
        }
      }
    else
      {
      status = NW_Text_SetStorage(NW_TextOf(thisObj->displayText), displayStorage,
                                  displayLength, NW_TRUE);
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
static
NW_Bool
NW_FBox_Epoc32SelectSkin_OptionStateChanged(NW_FBox_Epoc32SelectSkin_t *skin,
                                        NW_Ucs2* data,
                                        NW_FBox_OptionBox_t* optionBox)
{
  NW_Ucs2* textStr;
  NW_Uint8 freeNeeded;
  NW_Text_Length_t length;
  NW_Text_Length_t dataLength;
  NW_Text_t* text;
  NW_Bool changed;

  NW_REQUIRED_PARAM(skin);

  dataLength = (NW_Text_Length_t)NW_Str_Strlen(data);

  changed = NW_FALSE;
  text = NW_FBox_OptionBox_GetText(optionBox);
  textStr = (NW_Ucs2*) NW_Text_GetUCS2Buffer (text, 0, &length, &freeNeeded);

  if (length == dataLength)
  {
    if (!NW_Mem_memcmp (data, textStr, length * sizeof (NW_Ucs2))) {
      NW_FBox_OptionBox_ChangeState(optionBox);
      changed = NW_TRUE;
    }
  }

  if (freeNeeded){
    NW_Mem_Free(textStr);
  }

  return changed;
}

/* ------------------------------------------------------------------------- */
static
NW_Bool
NW_FBox_Epoc32SelectSkin_OptGrpStateChanged(NW_FBox_Epoc32SelectSkin_t *skin,
                                        NW_Ucs2* data,
                                        NW_FBox_OptGrpBox_t* optGrpBox)
{
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;

  numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(optGrpBox->children));

  for(index = 0; index < numItems; index++)
  {
    NW_FBox_FormBox_t* formBox;

    formBox = *(NW_FBox_FormBox_t**)
      NW_ADT_Vector_ElementAt(optGrpBox->children, index);

    if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptionBox_Class))
    {
      if (NW_FBox_Epoc32SelectSkin_OptionStateChanged(skin, data, NW_FBox_OptionBoxOf(formBox)))
        return NW_TRUE;
    }
    else if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptGrpBox_Class))
    {
      /* recurse here */
      if (NW_FBox_Epoc32SelectSkin_OptGrpStateChanged(skin, data, NW_FBox_OptGrpBoxOf(formBox)))
        return NW_TRUE;
    }
    else{
      NW_ASSERT(0);
    }
  }
  return NW_FALSE;
}

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
static
NW_Bool
NW_FBox_Epoc32SelectSkin_ChangeState(NW_FBox_Epoc32SelectSkin_t *skin,
                                  NW_Ucs2* data)
{
  NW_ADT_Vector_Metric_t numItems;
  NW_ADT_Vector_Metric_t index;
  NW_FBox_SelectBox_t* selectBox;

  selectBox = (NW_FBox_SelectBox_t*)NW_FBox_Skin_GetFormBox(NW_FBox_SkinOf(skin));
  numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(selectBox->children));

  for(index = 0; index < numItems; index++)
  {
    NW_FBox_FormBox_t* formBox;

    formBox = *(NW_FBox_FormBox_t**)
      NW_ADT_Vector_ElementAt(selectBox->children, index);

    if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptionBox_Class))
    {
      if (NW_FBox_Epoc32SelectSkin_OptionStateChanged(skin, data, NW_FBox_OptionBoxOf(formBox)))
        return NW_TRUE;
    }
    else if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptGrpBox_Class))
    {
      if (NW_FBox_Epoc32SelectSkin_OptGrpStateChanged(skin, data, NW_FBox_OptGrpBoxOf(formBox)))
        return NW_TRUE;
    }
    else{
      NW_ASSERT(0);
    }
  }
  return NW_FALSE;
}


/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_GetOptionValue(NW_FBox_OptionBox_t* optionBox,
                                     NW_Ucs2** val,
                                     NW_Bool* updated)
{
  NW_Ucs2* textStr = NULL;
  NW_Uint8 freeNeeded = NW_FALSE;
  NW_Text_Length_t length;
  NW_Text_t* text;
  NW_Bool state;
  TBrowserStatusCode status = KBrsrSuccess;

  if (updated == NULL)
  {
    return KBrsrFailure;
  }

  state = NW_FBox_CheckBox_GetState(NW_FBox_CheckBoxOf(optionBox));
  if (state == NW_FALSE)
  {
    *updated = NW_FALSE;
    goto cleanup;
  }

  text = NW_FBox_OptionBox_GetText(optionBox);
  textStr =
    NW_Text_GetUCS2Buffer (text, NW_Text_Flags_Aligned, &length, &freeNeeded);

  if (textStr == NULL)
  {
    *updated = NW_FALSE;
    status = KBrsrOutOfMemory;
    goto cleanup;
  }

  if (*val)
  {
    NW_Ucs2* str;
    NW_Uint16 newLength;

    newLength = (NW_Uint16)
      (NW_Str_Strlen (*val) + NW_FBox_Epoc32SelectSkin_Separator_len + length);
    str = NW_Str_New(newLength);

    if (str == NULL)
    {
      *updated = NW_FALSE;
      status = KBrsrOutOfMemory;
      goto cleanup;
    }

    NW_Mem_memset(str, 0, newLength);
    NW_Str_Strcpy(str, *val);
    NW_Str_Strcat(str, _NW_FBox_Epoc32SelectSkin_Separator);
    NW_Str_Strcat(str, textStr);
    NW_Str_Delete(*val);
    *val = str;
  }
  else{
    *val = NW_Str_Newcpy(textStr);

    if (*val == NULL)
    {
      *updated = NW_FALSE;
      status = KBrsrOutOfMemory;
      goto cleanup;
    }
  }

  *updated = NW_TRUE;

cleanup:
  if (freeNeeded){
    NW_Mem_Free(textStr);
  }

  return status;
}


/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_GetChildrenValue(NW_FBox_Epoc32SelectSkin_t *skin,
                                       NW_FBox_FormBox_t*       parentFormBox,
                                       NW_ADT_Vector_Metric_t   numParentItems,
                                       NW_Bool                  multiple,
                                       NW_Ucs2**                newStr,
                                       NW_Bool*                 updated)
{
  NW_ADT_Vector_Metric_t index;
  NW_FBox_OptionBox_t* optionBox;
  TBrowserStatusCode status = KBrsrSuccess;
  *updated = NW_FALSE;

  for(index = 0; index < numParentItems; index++)
  {
    NW_FBox_FormBox_t* childFormBox;

    childFormBox = NULL;

    /* get the next child of the parent */
    if (NW_Object_IsInstanceOf(parentFormBox, &NW_FBox_SelectBox_Class))
    {
      childFormBox = *(NW_FBox_FormBox_t**)
        NW_ADT_Vector_ElementAt(NW_FBox_SelectBoxOf(parentFormBox)->children, index);
    }
    else if (NW_Object_IsInstanceOf(parentFormBox, &NW_FBox_OptGrpBox_Class))
    {
      childFormBox = *(NW_FBox_FormBox_t**)
        NW_ADT_Vector_ElementAt(NW_FBox_OptGrpBoxOf(parentFormBox)->children, index);
    }
    else
    {
      /* bad content - only <select> and <optgroup> are allowed as parent */
      NW_ASSERT((NW_Object_IsInstanceOf(parentFormBox, &NW_FBox_SelectBox_Class))
             || (NW_Object_IsInstanceOf(parentFormBox, &NW_FBox_OptGrpBox_Class)));

    }

    /* handle the child */
    if (childFormBox == NULL)
    {
      NW_LOG0(NW_LOG_LEVEL1, "!!! NW_FBox_Epoc32SelectSkin_GetChildrenValue: BadContent: childFormBox==NULL");
      status = KBrsrXhtmlBadContent;
      break;
    }
    else if (NW_Object_IsInstanceOf(childFormBox, &NW_FBox_OptionBox_Class))
    {
      optionBox = NW_FBox_OptionBoxOf(childFormBox);

      status = NW_FBox_Epoc32SelectSkin_GetOptionValue(NW_FBox_OptionBoxOf(optionBox),
                                                    newStr,
                                                    updated);
      if (status != KBrsrSuccess)
        break;

      if (*updated && !multiple)
        break;
    }
    else if (NW_Object_IsInstanceOf(childFormBox, &NW_FBox_OptGrpBox_Class))
    {
      NW_FBox_OptGrpBox_t* optGrpBox;
      NW_ADT_Vector_Metric_t numOptGrpItems;

      optGrpBox = NW_FBox_OptGrpBoxOf(childFormBox);
      numOptGrpItems = NW_FBox_OptGrpBox_GetNumOptions(optGrpBox);

      /* recurse here */
      status = NW_FBox_Epoc32SelectSkin_GetChildrenValue(skin,
                                                      childFormBox,
                                                      numOptGrpItems,
                                                      multiple,
                                                      newStr,
                                                      updated);
      if (status != KBrsrSuccess)
        break;

      if (*updated && !multiple)
        break;
    }
    else
    {
      /* bad content - only <optgroup> and <option> are allowed as children */
      NW_ASSERT((NW_Object_IsInstanceOf(childFormBox, &NW_FBox_OptionBox_Class))
             || (NW_Object_IsInstanceOf(childFormBox, &NW_FBox_OptGrpBox_Class)));

      NW_LOG0(NW_LOG_LEVEL1, "!!! NW_FBox_Epoc32SelectSkin_GetChildrenValue: BadContent: instance type error");
      status = KBrsrXhtmlBadContent;
      break;
    }
  }

  return status;

}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_UpdateValue(NW_FBox_Epoc32SelectSkin_t *skin)
{
  NW_FBox_SelectBox_t* selectBox;
  NW_ADT_Vector_Metric_t numItems;
  NW_Bool multiple;
  NW_Ucs2* newStr;
  NW_Bool updated;
  TBrowserStatusCode status;

  selectBox = (NW_FBox_SelectBox_t*)NW_FBox_Skin_GetFormBox(NW_FBox_SkinOf(skin));
  numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(selectBox->children));
  multiple = NW_FBox_SelectBox_IsMultiple(selectBox);
  newStr = NULL;

  status = NW_FBox_Epoc32SelectSkin_GetChildrenValue(skin,
                                                  NW_FBox_FormBoxOf(selectBox),
                                                  numItems,
                                                  multiple,
                                                  &newStr,
                                                  &updated);

  if (status != KBrsrSuccess) {
    goto cleanup;
  }

  if (newStr == NULL){
    return NW_Text_SetStorage (skin->value, NULL, 0, 0);
  }
  else{
    status = NW_Text_SetStorage(NW_TextOf(skin->value),
                                newStr,
                                0,
                                NW_Text_Flags_TakeOwnership);
  }

cleanup:
  return status;

}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_ClearOptionsList( NW_FBox_Epoc32SelectSkin_t* thisObj )
{
  NW_ASSERT (thisObj != NULL);

  if (thisObj->optionsList != NULL)
  {
    NW_Int32 index;
    NW_Dlg_SelectItemData_t* entry;
    NW_Uint32 optionsListSize;

    optionsListSize = NW_ADT_Vector_GetSize (NW_ADT_VectorOf (thisObj->optionsList)) ;
    index = 0;
    for (index = optionsListSize - 1; index >= 0; index--)
    {
      entry = (NW_Dlg_SelectItemData_t*)
               NW_ADT_Vector_ElementAt (NW_ADT_VectorOf (thisObj->optionsList),
                                        (NW_ADT_Vector_Metric_t) index) ;
      NW_Mem_Free (entry->text);
      NW_ADT_DynamicVector_RemoveAt(thisObj->optionsList,
                                    (NW_ADT_Vector_Metric_t)index);
    }
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_SelectPromptResp (NW_Dlg_SelectData_t *data,
                                          void* callbackCtx,
                                          NW_Bool selectionChanged)
{
  NW_FBox_Epoc32SelectSkin_t* skin;
  NW_FBox_SelectBox_t* selectBox;
  NW_FBox_FormLiaison_t* formLiason;
  TBrowserStatusCode status = KBrsrSuccess;
  TUint32 ii;
  TUint32 count;
  NW_Dlg_SelectItemData_t* selectItemData;
  TBool updateDisplay = NW_TRUE;

  skin = (NW_FBox_Epoc32SelectSkin_t*)callbackCtx;
  selectBox = (NW_FBox_SelectBox_t*)NW_FBox_Skin_GetFormBox(NW_FBox_SkinOf(skin));
  formLiason = NW_FBox_FormBox_GetFormLiaison(selectBox);


  count = NW_ADT_Vector_GetSize(data->options);

  /* first set all options without onpick property */
  for ( ii = 0; ii < count; ++ii )
  {
    selectItemData = (NW_Dlg_SelectItemData_t*)NW_ADT_Vector_ElementAt( data->options, (NW_ADT_Vector_Metric_t)ii );

    if ( !selectItemData->isOptGroup && !selectItemData->hasOnPick )
    {
      NW_FBox_OptionBox_t* optionBox = (NW_FBox_OptionBox_t*)selectItemData->context;

      if ( selectItemData->isSelected != (NW_FBox_CheckBox_GetState(NW_FBox_CheckBoxOf(optionBox)) ))
      {
        NW_FBox_OptionBox_ChangeState( optionBox );
        updateDisplay = NW_TRUE;
      }
    }
  }

  /* now set options with onpick property */
  for ( ii = 0; ii < count; ++ii )
  {
    selectItemData = (NW_Dlg_SelectItemData_t*)NW_ADT_Vector_ElementAt( data->options, (NW_ADT_Vector_Metric_t)ii );

    if ( !selectItemData->isOptGroup && selectItemData->hasOnPick )
    {
      NW_FBox_OptionBox_t* optionBox = (NW_FBox_OptionBox_t*)selectItemData->context;

      if ( !data->isCanceled
       &&  (( !selectBox->multiple && selectItemData->isSelected)
             || (selectBox->multiple && selectItemData->isSelected != NW_FBox_CheckBox_GetState(NW_FBox_CheckBoxOf(optionBox))) ) )
      {
        updateDisplay = NW_TRUE;
        if (selectItemData->isLocalNavOnpick)
        {
          updateDisplay = NW_FALSE;
        }
        // CAUTION: this eventually calls NW_UI_CreateCard which may remove 'skin' if onpick to intradeck card exists in content
        NW_FBox_OptionBox_ChangeState( optionBox );
        break;
      }
    }
  }

    /* if OnPick has been activated there's no skin anymore */
  if (updateDisplay == NW_TRUE)
  {
    NW_FBox_Epoc32SelectSkin_ClearOptionsList(skin);
    status = NW_FBox_Epoc32SelectSkin_UpdateValue(skin);
    if (status == KBrsrSuccess)
    {
      status = NW_FBox_Epoc32SelectSkin_SetDisplayText(skin);
    }
    if (status == KBrsrSuccess)
    {
      status = NW_LMgr_Box_Refresh(NW_LMgr_BoxOf(selectBox));
    }
    // if the selection changed then notify ecma of the change event
    if(selectionChanged)
      {
      (void) NW_FBox_FormLiaison_DelegateEcmaEvent (formLiason,
                                       NW_FBox_FormBox_GetFormCntrlID(selectBox),
                                       NW_Ecma_Evt_OnChange);
      }
  }

  return status;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_DrawText(NW_FBox_Epoc32SelectSkin_t* thisObj,
                                 CGDIDeviceContext* deviceContext,
                                 NW_Bool hasFocus,
                                 NW_GDI_FlowDirection_t* dir)
  {
  NW_GDI_Point2D_t start;
  NW_FBox_SelectBox_t* selectBox;
  CGDIFont* font;
  NW_GDI_Rectangle_t innerRect;
  NW_LMgr_PropertyValue_t textDir;
  NW_GDI_Color_t fgColor;
  NW_LMgr_Property_t color;
  NW_GDI_Dimension2D_t extent;

  NW_REQUIRED_PARAM(hasFocus);
  selectBox = NW_FBox_SelectSkin_GetSelectBox(thisObj);
  NW_ASSERT(selectBox);

  NW_TRY(status)
    {
     /* Get the box font */
    font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(selectBox));
    NW_ASSERT(font != NULL);

    /* Get the old foreground color */
    fgColor = deviceContext->ForegroundColor ();

    // Get and set the box fg color, not inherited from parent (same as IE not following CSS spec)
    status = NW_LMgr_Box_GetRawProperty(NW_LMgr_BoxOf(selectBox), NW_CSS_Prop_color, &color);
    if (status == KBrsrSuccess)
      {
      deviceContext->SetForegroundColor ( (NW_GDI_Color_t)color.value.integer);
      }
    else
      {
      deviceContext->SetForegroundColor ( (NW_GDI_Color_t)0);
      }

    /* Get the start coordinates */
    NW_LMgr_Box_GetInnerRectangle(NW_LMgr_BoxOf(selectBox), &innerRect);
    start = innerRect.point;

    if (thisObj->displayText == NULL)
      {
      status = NW_FBox_SelectSkin_RefreshText(NW_FBox_SelectSkinOf(thisObj));
      NW_THROW_ON_ERROR(status);
      }

    /* Get the text direction attribute */
    textDir.token = NW_CSS_PropValue_ltr;
    NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(selectBox), NW_CSS_Prop_textDirection, NW_CSS_ValueType_Token, &textDir);

    if (thisObj->displayText != NULL)
      {
      /* If RTL, change drawing coordinates */
      *dir = (textDir.token == NW_CSS_PropValue_ltr) ? NW_GDI_FlowDirection_LeftRight : NW_GDI_FlowDirection_RightLeft;

      status = deviceContext->GetTextExtent( NW_Text_AbstractOf(thisObj->displayText),
                                             font,
                                             0,
                                             *dir,
                                             &extent);
      NW_THROW_ON_ERROR(status);
      if (*dir == NW_GDI_FlowDirection_RightLeft)
        {
        start.x = innerRect.point.x + innerRect.dimension.width - extent.width;
        }
      status = deviceContext->DrawText( &start,
                                        NW_TextOf(thisObj->displayText),
                                        font,
                                        0,
                                        NW_GDI_TextDecoration_None,
                                        *dir,
                                        extent.width);
      }
    }
  NW_CATCH(status)
    {
    }
  NW_FINALLY
    {
    deviceContext->SetForegroundColor ( fgColor);
    return KBrsrSuccess;
    }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_FillOptionsList( NW_FBox_Epoc32SelectSkin_t* thisObj,
                                          NW_FBox_FormBox_t* formBox,
                                          NW_Uint32 level)
{
  _LIT(KPrefix, "    ");// 4 spaces to match look of browser engine select dialog
  NW_Dlg_SelectItemData_t entry;
  TBrowserStatusCode status;
  NW_Uint32 ii;
  NW_Uint32 count;
  NW_Text_t* text;

  NW_ASSERT(thisObj!=NULL);
  NW_ASSERT(formBox!=NULL);

  status = KBrsrSuccess;

  if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptionBox_Class))
  {
    /* just option - add to the list */
    NW_FBox_OptionBox_t* optionBox;

    optionBox = NW_FBox_OptionBoxOf(formBox);

    text = NW_FBox_OptionBox_GetText(optionBox);

    entry.hasOnPick = NW_FBox_OptionBox_HasOnPick(optionBox);
    entry.isLocalNavOnpick = NW_FBox_OptionBox_IsLocalNavOnPick(optionBox);
    entry.isOptGroup = NW_FALSE;
    entry.isSelected = NW_FBox_CheckBox_GetState(NW_FBox_CheckBoxOf(optionBox));
                                               
    // child of option group elements must be indented (level 0 = <select>)
    if (level > 1)
        {
        HBufC* newText(NULL);
        TRAPD(err, newText = HBufC::NewL(text->characterCount + KPrefix().Length() + 1));// add 1 for zero terminate
        if (err == KErrNone)
            {
            newText->Des().Copy(KPrefix);
            newText->Des().Append((NW_Ucs2*)text->storage, text->characterCount);
            NW_Text_t* nt = NW_Text_New (HTTP_iso_10646_ucs_2, (void*)(newText->Des().PtrZ()), 
                            newText->Length(), NW_TRUE);

            entry.text = NW_FBox_Epoc32SelectSkin_GetNewUcs2Str(thisObj, nt);
            NW_Mem_Free(nt);
            }
        else
            {
            entry.text = NW_FBox_Epoc32SelectSkin_GetNewUcs2Str(thisObj, text);
            }
        }
    else
        {
        entry.text = NW_FBox_Epoc32SelectSkin_GetNewUcs2Str(thisObj, text);
        }
    
    entry.context = optionBox;

    NW_ADT_DynamicVector_InsertAt (thisObj->optionsList, &entry, NW_ADT_Vector_AtEnd);
  }
  else if (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptGrpBox_Class))
  {
    /* optgroup - add to the list if not nested and process children */
    NW_FBox_OptGrpBox_t* optGrpBox;

    optGrpBox = NW_FBox_OptGrpBoxOf(formBox);

      text = NW_FBox_OptGrpBox_GetText(optGrpBox);

      if (text)
      {
        /* add optgroup only if it has a title */
        entry.text = NW_FBox_Epoc32SelectSkin_GetNewUcs2Str(thisObj, text);
        entry.isLocalNavOnpick = NW_FALSE;
        entry.hasOnPick = NW_FALSE;
        entry.isOptGroup = NW_TRUE;
        entry.isSelected = NW_FALSE;
        entry.context = optGrpBox;

        NW_ADT_DynamicVector_InsertAt (thisObj->optionsList, &entry, NW_ADT_Vector_AtEnd);
      }


    /* process children recursively */
    if (optGrpBox->children)
      count = NW_ADT_Vector_GetSize(optGrpBox->children);
    else count = 0;
    for ( ii = 0; ii < count; ii++ )
    {
      NW_FBox_FormBox_t* child =
        *(NW_FBox_FormBox_t**)NW_ADT_Vector_ElementAt(optGrpBox->children,
        (NW_ADT_Vector_Metric_t)ii);
      status = NW_FBox_Epoc32SelectSkin_FillOptionsList(thisObj,
                                                        child, level + 1);
    }

  }
  else if (NW_Object_IsInstanceOf(formBox, &NW_FBox_SelectBox_Class))
  {
    /* select - just process children recursively */
    NW_FBox_SelectBox_t* selectBox;

    selectBox = NW_FBox_SelectBoxOf(formBox);
    count = NW_ADT_Vector_GetSize(selectBox->children);
    for ( ii = 0; ii < count; ii++ )
    {
      NW_FBox_FormBox_t* child =
        *(NW_FBox_FormBox_t**)NW_ADT_Vector_ElementAt(selectBox->children,
        (NW_ADT_Vector_Metric_t)ii);
      status = NW_FBox_Epoc32SelectSkin_FillOptionsList(thisObj,
                                                        child, level + 1);
    }
  }
  else
  {
    /* unexpected box type  */
    NW_ASSERT((NW_Object_IsInstanceOf(formBox, &NW_FBox_OptionBox_Class))
      || (NW_Object_IsInstanceOf(formBox, &NW_FBox_OptGrpBox_Class)));

    status = KBrsrFailure;
  }

  return status;
}

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_Epoc32SelectSkin_Class_t  NW_FBox_Epoc32SelectSkin_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_FBox_SelectSkin_Class,
    /* queryInterface              */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize                */ sizeof (NW_FBox_Epoc32SelectSkin_t),
    /* construct                   */ _NW_FBox_Epoc32SelectSkin_Construct,
    /* destruct                    */ _NW_FBox_Epoc32SelectSkin_Destruct
  },
  { /* NW_FBox_Skin                */
    /* NW_FBox_Skin_Draw_t         */ _NW_FBox_Epoc32SelectSkin_Draw,
    /* NW_FBox_Skin_GetSize_t      */ _NW_FBox_Epoc32SelectSkin_GetSize
  },
  { /* NW_FBox_SelectSkin */
    /* setActive                   */ _NW_FBox_Epoc32SelectSkin_SetActive,
    /* addOption                   */ _NW_FBox_Epoc32SelectSkin_AddOption,
    /* addOptGrp                   */ _NW_FBox_Epoc32SelectSkin_AddOptGrp,
    /* detailsDialog               */ _NW_FBox_Epoc32SelectSkin_DetailsDialog,
    /* refeshText                  */ _NW_FBox_Epoc32SelectSkin_RefreshText,
    /* split                       */ _NW_FBox_Epoc32SelectSkin_Split
  },
  { /* NW_FBox_Epoc32SelectSkin */
    /* getBaseline                 */ _NW_FBox_Epoc32SelectSkin_GetBaseline
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp)
{
  NW_FBox_Epoc32SelectSkin_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_FBox_Epoc32SelectSkinOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_FBox_SelectSkin_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* Set initial value to empty string. */
  thisObj->value = NW_Text_UCS2_New (NULL, 0, 0);

  thisObj->displayText = NULL;

  thisObj->optionsList = (NW_ADT_DynamicVector_t*)
                         NW_ADT_ResizableVector_New(sizeof(NW_Dlg_SelectItemData_t), 3, 3);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_FBox_Epoc32SelectSkin_Destruct(NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_Epoc32SelectSkin_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_Epoc32SelectSkinOf (dynamicObject);

  if (thisObj->displayText){
    NW_Object_Delete(thisObj->displayText);
  }
  if (thisObj->value){
    NW_Object_Delete(thisObj->value);
  }
  if (thisObj->optionsList){
    NW_FBox_Epoc32SelectSkin_ClearOptionsList(thisObj);
    NW_Object_Delete(thisObj->optionsList);
  }
}

// -------------------------------------------------------------------------
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_Draw(NW_FBox_Skin_t* skin,
                        NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus)
  {
  NW_FBox_SelectBox_t* selectBox;
  NW_FBox_Epoc32SelectSkin_t* thisObj;
  NW_GDI_Rectangle_t innerRectangle;
  NW_GDI_Point2D_t imagePoint;
  NW_GDI_Dimension3D_t imageSize;
  NW_LMgr_RootBox_t* rootBox;
  NW_Image_AbstractImage_t* image;
  NW_GDI_FlowDirection_t dir;

  // for convenience
  selectBox = NW_FBox_SelectBoxOf (box);
  thisObj = NW_FBox_Epoc32SelectSkinOf(skin);

  NW_TRY(status)
    {
    // standard paint mode
    deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);

    // let the base class draw draw the box
    status =_NW_LMgr_Box_Draw(box, deviceContext, hasFocus);
    NW_THROW_ON_ERROR (status);

    // get the root box
    rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(selectBox));
    NW_THROW_ON_NULL (rootBox, status, KBrsrFailure);

    // get the image from the canned images dictionary
    image = CONST_CAST (NW_Image_AbstractImage_t*, NW_Image_CannedImages_GetImage(rootBox->cannedImages, NW_Image_SelectArrow));
    NW_THROW_ON_NULL (image, status, KBrsrFailure);

    // get the image size
    status = NW_Image_AbstractImage_GetSize(image, &imageSize);
    NW_THROW_ON_ERROR (status);

    // draw text first
    status = NW_FBox_Epoc32SelectSkin_DrawText(thisObj, deviceContext, hasFocus, &dir);
    NW_THROW_ON_ERROR (status);

    // get the inner rectangle
    status = NW_LMgr_Box_GetInnerRectangle(box, &innerRectangle);
    NW_THROW_ON_ERROR (status);

    // position the select image on the correct side of the box depending on direction (RTL)
    if(dir == NW_GDI_FlowDirection_RightLeft)
      {
      imagePoint.x = innerRectangle.point.x;
      }
    else
      {
      imagePoint.x =
        (NW_GDI_Metric_t)(innerRectangle.point.x + innerRectangle.dimension.width - imageSize.width);
      }

    // position the image so that it is centered in the box
    if (innerRectangle.dimension.height > imageSize.height)
      {
      imagePoint.y =
        (NW_GDI_Metric_t)(innerRectangle.point.y + ((innerRectangle.dimension.height - imageSize.height) / 2));
      }
    else
      {
      imagePoint.y = (NW_GDI_Metric_t)(innerRectangle.point.y);
      }

    status = NW_Image_AbstractImage_Draw (image, deviceContext, &imagePoint);
    NW_THROW_ON_ERROR(status);
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

// -------------------------------------------------------------------------
NW_GDI_Metric_t
_NW_FBox_Epoc32SelectSkin_GetBaseline(NW_FBox_Epoc32SelectSkin_t* aThisObj,
                                      NW_LMgr_Box_t* aLmgrBox)
  {
  CGDIFont* font;
  TBrowserStatusCode status;
  NW_GDI_Metric_t baseline;

  NW_REQUIRED_PARAM(aThisObj);

  // parameter assertion block
  NW_ASSERT (aLmgrBox != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (aLmgrBox, &NW_FBox_FormBox_Class));

  // set the font to match the parent
  font = NW_LMgr_Box_GetFont (aLmgrBox);
  if (font)
    {
    TGDIFontInfo fontInfo;
    NW_LMgr_FrameInfo_t padding;
    NW_LMgr_FrameInfo_t borderWidth;
    NW_GDI_Dimension3D_t imageSize;

    status = font->GetFontInfo ( &fontInfo);
    NW_ASSERT (status == KBrsrSuccess);

    // Get padding and border width
    NW_LMgr_Box_GetPadding(aLmgrBox, &padding, ELMgrFrameTop );
    NW_LMgr_Box_GetBorderWidth(aLmgrBox, &borderWidth, ELMgrFrameTop );

    status = NW_FBox_Epoc32SelectSkin_GetArrowSize(NW_FBox_SelectBoxOf(aLmgrBox), &imageSize);

    if (status == KBrsrSuccess)
      {
      // Use the bigger of font baseline or image height and then adjust for border and padding
      baseline = (NW_GDI_Metric_t)
        ((fontInfo.baseline < imageSize.height) ? imageSize.height : fontInfo.baseline) + borderWidth.top + padding.top;
      }
    else
      {
      baseline = fontInfo.baseline + borderWidth.top + padding.top;
      }
    }
  else
    {
    baseline = NW_LMgr_Box_GetFormatBounds(aLmgrBox).dimension.height;
    }

  return baseline;
  }

// -------------------------------------------------------------------------
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_GetSize(NW_FBox_Skin_t* skin,
                                  NW_LMgr_Box_t* box,
                                  NW_GDI_Dimension3D_t* size)
  {
  NW_FBox_Epoc32SelectSkin_t* thisObj;
  NW_GDI_Dimension2D_t extent;
  CGDIDeviceContext* deviceContext;
  const NW_GDI_Rectangle_t* rectangle;
  NW_GDI_Dimension3D_t imageSize;
  NW_LMgr_FrameInfo_t padding;
  NW_LMgr_FrameInfo_t borderWidth;

  NW_TRY(status)
    {
    // parameter assertion block
    NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_SelectBox_Class));

    // for convenience
    thisObj = NW_FBox_Epoc32SelectSkinOf(skin);

    size->width = 0;
    size->height = 0;

    // get the device context
    deviceContext = NW_LMgr_RootBox_GetDeviceContext (NW_LMgr_Box_GetRootBox(box));
    NW_ASSERT (deviceContext != NULL);

    // get the display bounds
    rectangle = deviceContext->DisplayBounds();

    // get the max character extents
    status = NW_FBox_SelectSkin_GetMCharExtent(NW_FBox_SelectSkinOf(skin), &extent);
    NW_THROW_ON_ERROR (status);

    // get the image size
    status = NW_FBox_Epoc32SelectSkin_GetArrowSize(NW_FBox_SelectBoxOf(box), &imageSize);

    // get the padding and border width
    NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameLeft|ELMgrFrameRight );
    NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameLeft|ELMgrFrameRight );

    size->height = (NW_GDI_Metric_t)(extent.height);

    // height must not be smaller than the icon height
    if (size->height < imageSize.height)
      {
      size->height = imageSize.height;
      }

    //Determine if this box sits in a table cell. Different rules apply based on this.
    NW_LMgr_Box_t* parent = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent(box);
    if (NW_Object_IsClass(parent, &NW_LMgr_StaticTableCellBox_Class))
      {
        // Calculate the minimum width. This is used for forms inside tables.
        // In this case the input selection fields should not span the entire screen width.
        size->width = imageSize.width +  NW_FBox_Epoc32SelectSkin_GetMaxTextWidth(thisObj, NW_FBox_SelectBoxOf(box));
      }
    else
      {
        // Calculate the full width. This is used for forms not inside tables.
        // In this case the input selection fields should span the entire screen width.
        size->width = rectangle->dimension.width - padding.left - padding.right - borderWidth.left - borderWidth.right;
      }

    // Update the value of the select box
    status = NW_FBox_Epoc32SelectSkin_UpdateValue(thisObj);
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

// -------------------------------------------------------------------------
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_SetActive(NW_FBox_SelectSkin_t* skin)
  {
  NW_FBox_SelectBox_t* selectBox;
  NW_FBox_FormBox_t* formBox;
  NW_FBox_Epoc32SelectSkin_t* epoc32Skin;
  NW_Dlg_SelectData_t* data;
  NW_Ucs2* textStr;
  NW_Uint8 freeNeeded;
  NW_Text_Length_t length;
  NW_Ucs2* newStr;
  NW_Text_t* text;

  formBox = NW_FBox_Skin_GetFormBox(NW_FBox_SkinOf(skin));
  selectBox = NW_FBox_SelectBoxOf(formBox);
  epoc32Skin = NW_FBox_Epoc32SelectSkinOf(skin);

  /* Use the title attribute for the select dialog title. */
  text = selectBox->title;
  if (text == NULL)
    {
    newStr = selectBox->appServices->getLocalizedStringAPI.getLocalizedString( EKmLocSelectLabel );
    if (!newStr)
      {
      newStr = NW_Str_NewcpyConst("Select");
      if (!newStr)
        {
        return KBrsrOutOfMemory;
        }
      }
    }
  else
    {
    textStr = NW_Text_GetUCS2Buffer (text, NW_Text_Flags_Aligned, &length, &freeNeeded);

    if (freeNeeded)
      {
      newStr = textStr;
      }
    else
      {
      newStr = NW_Str_Newcpy(textStr);
      if (!newStr)
        {
        return KBrsrOutOfMemory;
        }
      }
    }

  NW_FBox_Epoc32SelectSkin_FillOptionsList(epoc32Skin, formBox, 0);

  data = (NW_Dlg_SelectData_t*)NW_Mem_Malloc(sizeof(NW_Dlg_SelectData_t));

  if (!data)
    {
    return KBrsrOutOfMemory;
    }

  data->title = newStr;
  data->multiple = selectBox->multiple;
  data->options = NW_ADT_VectorOf(epoc32Skin->optionsList);

  selectBox->appServices->genDlgApi.DialogSelectOption( data,
                                                        epoc32Skin,
                                                        NW_FBox_Epoc32SelectSkin_SelectPromptResp);

  NW_Mem_Free(data->title);
  NW_Mem_Free(data);

  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_Split(NW_FBox_SelectSkin_t* skin,
                               NW_GDI_Metric_t space,
                               NW_LMgr_Box_t* *splitBox,
                               NW_Uint8 flags)
  {
  NW_LMgr_PropertyValue_t sizeVal;
  TBrowserStatusCode status;
  NW_LMgr_Box_t* box;
  NW_FBox_FormBox_t* formBox;

  formBox = NW_FBox_Skin_GetFormBox(NW_FBox_SkinOf(skin));
  box = NW_LMgr_BoxOf(formBox);

  *splitBox = NULL;
  /* If box does not fit in the space and the box is not
   on the new line the input box needs to be pushed on the new line */
  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
  if ((boxBounds.dimension.width > space) && !(flags & NW_LMgr_Box_SplitFlags_AtNewLine))
    {
    return KBrsrLmgrNoSplit;
    }

  if(boxBounds.dimension.width <= space)
    {
    return KBrsrSuccess;
    }

  /* we are more than space */

  /* if width was specified we do not want to change it */
  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
  if (status == KBrsrSuccess)
    {
    return status;
    }
  boxBounds.dimension.width = space;
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );

  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_AddOption(NW_FBox_SelectSkin_t* selectSkin,
                                    NW_FBox_OptionBox_t* optionBox,
                                    NW_LMgr_Box_t** styleBox)
{
  NW_REQUIRED_PARAM(selectSkin);
  NW_REQUIRED_PARAM(optionBox);
  NW_REQUIRED_PARAM(styleBox);
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_AddOptGrp(NW_FBox_SelectSkin_t* selectSkin,
                                    NW_FBox_OptGrpBox_t* optGrpBox)
{
  NW_REQUIRED_PARAM(selectSkin);
  NW_REQUIRED_PARAM(optGrpBox);
  return KBrsrSuccess;

}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_DetailsDialog(NW_FBox_SelectSkin_t* skin)
{
  NW_REQUIRED_PARAM(skin);
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32SelectSkin_RefreshText(NW_FBox_SelectSkin_t* selectSkin)
{
  TBrowserStatusCode status;
  NW_FBox_Epoc32SelectSkin_t* skin = NW_FBox_Epoc32SelectSkinOf(selectSkin);
  status = NW_FBox_Epoc32SelectSkin_UpdateValue(skin);
  NW_ASSERT(status == KBrsrSuccess);
  if (status != KBrsrSuccess){
    return status;
  }
  return NW_FBox_Epoc32SelectSkin_SetDisplayText(skin);
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_GetValue(NW_FBox_Epoc32SelectSkin_t *skin, NW_Ucs2** newStr)
{
  NW_FBox_SelectBox_t* selectBox;
  NW_ADT_Vector_Metric_t numItems;
  NW_Bool multiple;
  NW_Bool updated;
  TBrowserStatusCode status;

  NW_ASSERT(newStr != NULL);
  if (newStr == NULL) {
    return KBrsrBadInputParam;
  }

  selectBox = (NW_FBox_SelectBox_t*)NW_FBox_Skin_GetFormBox(NW_FBox_SkinOf(skin));
  numItems = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(selectBox->children));
  multiple = NW_FBox_SelectBox_IsMultiple(selectBox);

  status = NW_FBox_Epoc32SelectSkin_GetChildrenValue(skin,
                                                NW_FBox_FormBoxOf(selectBox),
                                                numItems,
                                                multiple,
                                                newStr,
                                                &updated);
  return status;
}

TBrowserStatusCode
NW_FBox_Epoc32SelectSkin_UpdateSelection(NW_FBox_Epoc32SelectSkin_t* skin,
                                             NW_Ucs2* selectedText,
                                             NW_Bool* selectUpdated)
{
  TBrowserStatusCode status = KBrsrSuccess;

  NW_FBox_SelectSkin_Reserve(NW_FBox_SelectSkinOf(skin));
  *selectUpdated = NW_FBox_Epoc32SelectSkin_ChangeState(skin, selectedText);
  if (NW_FBox_SelectSkinOf(skin)->hasFormBox == NW_TRUE)
  {
    status = NW_FBox_Epoc32SelectSkin_UpdateValue(skin);
    if (status == KBrsrSuccess)
    {
      status = NW_FBox_Epoc32SelectSkin_SetDisplayText(skin);
    }
  }
  NW_FBox_SelectSkin_Release(NW_FBox_SelectSkinOf(skin));
  return status;
}


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_SelectSkin_t*
NW_FBox_SelectSkin_New (NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_SelectSkin_t*)
    NW_Object_New (&NW_FBox_Epoc32SelectSkin_Class, formBox);
}
