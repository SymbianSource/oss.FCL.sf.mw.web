/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
#include <calslbs.h>            // for Latin.... font methods
#include <nwx_string.h>

#include "nw_fbox_formbox.h"
#include "nw_evt_ecmaevent.h"


#include "nw_fbox_epoc32inputskini.h"
#include "nw_wml1x_wml_1_3_tokens.h"
#include "nw_fbox_inputbox.h"
#include "nw_fbox_textareabox.h"
#include "nw_fbox_passwordbox.h"
#include "nw_fbox_passwordboxi.h"
#include "nw_lmgr_box.h"
#include "nw_lmgr_textbox.h"
#include "nw_lmgr_rootbox.h"
#include "LMgrBoxTreeListener.h"
#include "GDIDeviceContext.h"
#include "nw_fbox_formboxutils.h"
#include "BrsrStatusCodes.h"
#include "nw_lmgr_statictablecellbox.h"

const NW_GDI_Dimension2D_t extraPadding = {2,2};

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_GetMaxCharExtent(NW_FBox_InputBox_t* inputBox, 
                                         NW_GDI_Dimension2D_t* extent)
{
  CFont* epocCFont;
  CGDIFont* font;
  CGDIDeviceContext* deviceContext;
  NW_LMgr_RootBox_t* rootBox = NULL;

  /* parameter assertion block */
  NW_ASSERT(inputBox!=NULL);
  NW_ASSERT(NW_Object_IsInstanceOf(inputBox, &NW_FBox_InputBox_Class));
  NW_ASSERT(extent!=NULL);

  /* get device context */
  rootBox = NW_LMgr_Box_GetRootBox (NW_LMgr_BoxOf (inputBox));
  NW_ASSERT (rootBox != NULL);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  NW_ASSERT (deviceContext != NULL);

  /* get the box font */
  font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(inputBox));
  NW_ASSERT (font != NULL);
  epocCFont = (CFont*)
    deviceContext->GetScaledCFont(  font );
    
  /* get character extents */
  extent->width = (NW_GDI_Metric_t)epocCFont->MaxNormalCharWidthInPixels();
  extent->height = (NW_GDI_Metric_t)epocCFont->HeightInPixels();

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_TruncateText(NW_FBox_Epoc32InputSkin_t* thisObj, 
                                     NW_GDI_Metric_t wrapWidth,
                                     CGDIDeviceContext* deviceContext,
                                     NW_Text_t* text,
                                     CGDIFont* font,
                                     NW_GDI_FlowDirection_t dir)
  {
  NW_GDI_Dimension2D_t extent;
  NW_Text_Length_t valueLength = 0;
  NW_Text_Length_t displayLength = 0;
  NW_Ucs2* displayStorage = NULL;
  NW_Ucs2* valueStorage = NULL;

  NW_TRY(status)
    {
    status = deviceContext->GetTextExtent(text,
                                          font,
                                          0,
                                          dir,
                                          &extent);
    NW_THROW_ON_ERROR(status);
    /* Line fits in the space */
    if (extent.width - 1 <= wrapWidth)
      {
      NW_ADT_DynamicVector_InsertAt (thisObj->textList, &text, NW_ADT_Vector_AtEnd);     
      NW_THROW_SUCCESS(status);
      }

    /* Line does not fit - truncate it */
    valueStorage = NW_Text_GetUCS2Buffer(text, 0, &valueLength, NULL);
    NW_THROW_ON_NULL(valueStorage, status, KBrsrFailure);

    status = NW_FBox_FormBoxUtils_GetDisplayLength( deviceContext, 
                                                    text,
                                                    font,
                                                    valueLength, 
                                                    (NW_Uint16)wrapWidth,
                                                    &displayLength, 
                                                    NW_FALSE);

    NW_THROW_ON_ERROR (status);
    displayStorage = NW_Str_New(displayLength + NW_FBox_FormBoxUtils_Truncate_len);
    NW_THROW_OOM_ON_NULL(displayStorage, status);

    NW_Mem_memcpy(displayStorage, valueStorage, displayLength*sizeof(NW_Ucs2));
    NW_Mem_memcpy(displayStorage + displayLength, NW_FBox_FormBoxUtils_Truncate, 
                                   NW_FBox_FormBoxUtils_Truncate_len*sizeof(NW_Ucs2));
    displayLength += NW_FBox_FormBoxUtils_Truncate_len;

    status = NW_Text_SetStorage(text, displayStorage, displayLength, NW_TRUE);
    NW_THROW_ON_ERROR (status);
    NW_ADT_DynamicVector_InsertAt (thisObj->textList, &text, NW_ADT_Vector_AtEnd);     
    }
  NW_CATCH(status)
    {
    if (displayStorage)
      {
      NW_Mem_Free(displayStorage);
      }
    }
  NW_FINALLY
    {
    return status;
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
// NW_FBox_Epoc32GetInnerRectangle
// param: NW_LMgr_Box_t* - pointer to input box
// param: NW_GDI_Rectangle_t* - returns x,y and height,width of inner rectangle
// returns: KBrsrSuccess 
// Very similar to LMgrBox.cpp / NW_LMgr_Box_GetInnerRectangle() this routine
// is different in that GetFormatBounds is used here, instead of GetDisplayBounds.
//
static
TBrowserStatusCode
NW_FBox_Epoc32GetInnerRectangle(NW_LMgr_Box_t* box,
                              NW_GDI_Rectangle_t* innerRectangle)
{
  NW_GDI_Rectangle_t bounds;
  NW_LMgr_FrameInfo_t padding;
  NW_LMgr_FrameInfo_t borderWidth;

  /* Get all margin, border and padding settings */
  bounds = (NW_LMgr_Box_GetFormatBounds(box));
  NW_LMgr_Box_GetPadding(box, &padding, ELMgrFrameAll );
  NW_LMgr_Box_GetBorderWidth(box, &borderWidth, ELMgrFrameAll );

  innerRectangle->point.x = 
    (NW_GDI_Metric_t)(bounds.point.x + borderWidth.left + padding.left);
  
  innerRectangle->point.y = 
    (NW_GDI_Metric_t)(bounds.point.y + borderWidth.top + padding.top);

  innerRectangle->dimension.height = (NW_GDI_Metric_t)(bounds.dimension.height - 
    (borderWidth.top + borderWidth.bottom + padding.top + padding.bottom));

  innerRectangle->dimension.width = (NW_GDI_Metric_t)(bounds.dimension.width - 
    (borderWidth.left + borderWidth.right + padding.left + padding.right));
  
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_SplitText(NW_FBox_InputSkin_t* skin, 
                                  NW_FBox_InputBox_t* inputBox,
                                  NW_Text_t* text)
  {
  NW_FBox_Epoc32InputSkin_t* thisObj;
  NW_Text_t* splitText = NULL;
  CGDIFont *font = NULL;
  NW_Text_Length_t length;
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;
  NW_GDI_Metric_t wrapWidth;
  NW_GDI_Dimension2D_t maxCharExtent;
  NW_GDI_Rectangle_t innerRect;
  NW_ADT_Vector_Metric_t numTexts = 0;
  NW_ADT_Vector_Metric_t numSplits = 0;
  NW_LMgr_Property_t prop;
  NW_GDI_FlowDirection_t dir;

  NW_TRY(status)
    {
    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_ltr;
    NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(inputBox), NW_CSS_Prop_textDirection, &prop);

    dir = (prop.value.token == NW_CSS_PropValue_ltr) ? NW_GDI_FlowDirection_LeftRight : NW_GDI_FlowDirection_RightLeft;
    /* we're going to need the deviceContext to do text extent calculations */
    rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(inputBox));
    NW_ASSERT (rootBox != NULL);

    deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    NW_ASSERT (deviceContext != NULL);

    status = NW_FBox_Epoc32InputSkin_GetMaxCharExtent(inputBox, &maxCharExtent);
    NW_THROW_ON_ERROR(status);

    /* Set the font to match the parent. TODO: this may not be the best policy */
    font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(inputBox));
    NW_ASSERT(font != NULL);

    thisObj = NW_FBox_Epoc32InputSkinOf(skin);
  
    /* Get the box info */
    NW_FBox_Epoc32GetInnerRectangle(NW_LMgr_BoxOf(inputBox), &innerRect);
    innerRect.dimension.height -= extraPadding.height;
    innerRect.dimension.width -= extraPadding.width;
    numTexts = NW_FBox_InputBox_GetHeight(inputBox);

    // one extra character reserved so that EdWin looks good when wrapping 
    wrapWidth = innerRect.dimension.width;

    /* Munch until we have consumed the whole text */
    while ((numSplits < numTexts) && (splitText != text))
      {
      if (numSplits == (numTexts - 1))
        {
        status = NW_FBox_Epoc32InputSkin_TruncateText( thisObj, 
                                                       wrapWidth, 
                                                       deviceContext, 
                                                       text,
                                                       font,
                                                       dir); 
        NW_THROW_ON_ERROR(status);
        break;        
        }
      /* try to split at the newline if newline char is within the width */
      status = deviceContext->SplitText (text, 
                                         font,
                                         wrapWidth, 0,
                                         dir,
                                         &length,
                                         NW_GDI_SplitMode_Newline);
      if(status == KBrsrLmgrNoSplit)
        { /* Failed */    
        /* try to split at space to fit within current line */
        status = deviceContext->SplitText ( text, 
                                            font,
                                            wrapWidth, 
                                            0,
                                            dir,
                                            &length,
                                            NW_GDI_SplitMode_WordWrap );
    
        if(status == KBrsrLmgrNoSplit)
          { /* Failed */
          status = deviceContext->SplitText ( text,
                                              font,
                                              wrapWidth, 
                                              0,
                                              dir,
                                              &length,
                                              NW_GDI_SplitMode_Clip );
          if(status == KBrsrLmgrNoSplit) 
            {
            return KBrsrFailure;
            }
          }
        }

      if (status == KBrsrOutOfMemory) 
        {
        return status;
        }

      if (length != NW_Text_GetCharCount(text)) 
        {
        splitText = NW_Text_Split(text, length);
        if (splitText == NULL) 
          {
          return KBrsrOutOfMemory;
          }
        }
      else 
        {
        splitText = text;
        }
      numSplits++;
      NW_ADT_DynamicVector_InsertAt (thisObj->textList, &splitText, NW_ADT_Vector_AtEnd);     
      }
    }
  NW_CATCH(status)
    {
    }
  NW_FINALLY
    {
    return KBrsrSuccess;
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
void
NW_FBox_Epoc32InputSkin_ClearTextList(NW_FBox_Epoc32InputSkin_t* thisObj)
{
  NW_ADT_Vector_Metric_t index;
  NW_ADT_Vector_Metric_t size;
  NW_Text_t* text;

  size = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(thisObj->textList));
  /* release the dynamic objects pointed by the object array */
  for (index = 0; index < size; index++) 
  {
    text = *(NW_Text_t**)
           NW_ADT_Vector_ElementAt(NW_ADT_VectorOf(thisObj->textList), 0);
    NW_ADT_DynamicVector_RemoveAt(thisObj->textList, 0);
    NW_ASSERT(text);
    NW_Object_Delete(text);
  }
  thisObj->textList->size = 0;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_RecreateTextList(NW_FBox_Epoc32InputSkin_t* thisObj)
{
  NW_FBox_InputBox_t* inputBox = NULL;  
  NW_FBox_InputSkin_t* inputSkin = NULL;  
  NW_Text_t* text = NULL;
  TBrowserStatusCode status;

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_InputSkin_Class));

  /* for convenience */
  inputSkin = NW_FBox_InputSkinOf(thisObj);

  /* get input box */
  inputBox = NW_FBox_InputSkin_GetInputBox(inputSkin);
  NW_ASSERT(inputBox!=NULL);

  /* clears the list of texts */
  NW_FBox_Epoc32InputSkin_ClearTextList(thisObj);

  /* Now we try to get the text to be displayed */

  /* In case of password box, the following code, converts inputbox value
     to * and stores it in InputSkin */
  if (NW_Object_IsClass(inputBox, &NW_FBox_PasswordBox_Class))
  {
    status = _NW_FBox_PasswordBox_SetObscureDisplayText(inputBox, NW_TRUE);
    if (status != KBrsrSuccess){
      return status;
    }
    if (inputSkin->displayText){
      text = NW_Text_Copy(inputSkin->displayText, NW_FALSE);
    }
  }
  else
  {
    if (inputBox->value){
      text = NW_Text_Copy(inputBox->value, NW_FALSE);
    }
  }

  if (text == NULL){
    return KBrsrSuccess;
  }

  /* input text does not fit within the space, input box is on new line,
   we need to split the text into multiple rows */
  status = NW_FBox_Epoc32InputSkin_SplitText(inputSkin, inputBox, text);  
  if (status != KBrsrSuccess){
    return status;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
static
TBrowserStatusCode
NW_FBox_InputSkin_VKBCallback (void *timerData)
{
 NW_FBox_Epoc32InputSkin_t* thisObj = NW_FBox_Epoc32InputSkinOf(timerData);
  //retrieve the epoc32 input box
  if (thisObj && thisObj->cppEpoc32InputBox)
  {
    thisObj->cppEpoc32InputBox->ActivateVKB();
  }
  if (thisObj->vkbTimer != NULL) {
    NW_Object_Delete (thisObj->vkbTimer);
    thisObj->vkbTimer = NULL;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   global data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_Epoc32InputSkin_Class_t  NW_FBox_Epoc32InputSkin_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_InputSkin_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base             */
    /* interfaceList              */ NULL
  },
  { /* NW_Object_Dynamic          */
    /* instanceSize               */ sizeof (NW_FBox_Epoc32InputSkin_t),
    /* construct                  */ _NW_FBox_Epoc32InputSkin_Construct,
    /* destruct                   */ _NW_FBox_Epoc32InputSkin_Destruct
  },
  { /* NW_FBox_Skin               */
    /* NW_FBox_Skin_Draw          */ _NW_FBox_InputSkin_Draw,
    /* NW_FBox_Skin_GetSize       */ _NW_FBox_Epoc32InputSkin_GetSize
  },
  { /* NW_FBox_InputSkin          */
    /* drawText                   */ _NW_FBox_Epoc32InputSkin_DrawText,
    /* moveCursorLeft             */ NULL,
    /* moveCursorRight            */ NULL,
    /* clearLastChar              */ NULL,
    /* setDisplayText             */ _NW_FBox_Epoc32InputSkin_SetDisplayText,
    /* setActive                  */ _NW_FBox_Epoc32InputSkin_SetActive,
    /* setFocus                   */ _NW_FBox_Epoc32InputSkin_SetFocus,
    /* detailsDialog              */ NULL,
    /* split                      */ _NW_FBox_Epoc32InputSkin_Split,
    /* vkb                        */_NW_FBox_Epoc32InputSkin_SetVKB 
  },
  { /* NW_FBox_Epoc32InputSkin */
    /* unused                     */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                                       va_list* argp)
{
  TBrowserStatusCode status;
  NW_FBox_Epoc32InputSkin_t* thisObj;

  /* avoid "unreferenced formal parameter" warnings */
  (void) argp;

  /* for convenience */
  thisObj = NW_FBox_Epoc32InputSkinOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_FBox_InputSkin_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* create the TextBox and insert it into our parent */
  thisObj->textList = (NW_ADT_DynamicVector_t*)
                      NW_ADT_ResizableVector_New(sizeof(NW_Text_t*), 3,3);
  if (thisObj->textList == NULL){
    return KBrsrOutOfMemory;
  }

  thisObj->cppEpoc32InputBox = NULL;

  /* successful completion */
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
void
_NW_FBox_Epoc32InputSkin_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_Epoc32InputSkin_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_Epoc32InputSkinOf (dynamicObject);

  /* destroy edwin */
  delete thisObj->cppEpoc32InputBox;
  thisObj->cppEpoc32InputBox = NULL;
  
  if (thisObj->vkbTimer != NULL) {
    NW_Object_Delete (thisObj->vkbTimer);
    thisObj->vkbTimer = NULL;
  }

  NW_FBox_Epoc32InputSkin_ClearTextList(thisObj);
  if (thisObj->textList){
    NW_Object_Delete(thisObj->textList);
  }

}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_GetSize(NW_FBox_Skin_t* skin,
                                 NW_LMgr_Box_t* box,
                                 NW_GDI_Dimension3D_t* size)
{
  NW_FBox_InputBox_t* inputBox;
  NW_GDI_Dimension2D_t charExtent;
  NW_LMgr_PropertyValue_t sizeVal;
  NW_GDI_Metric_t defaultHeight;
  NW_REQUIRED_PARAM (skin);

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_InputBox_Class));
  NW_ASSERT (size!=NULL);

  /* for convenience */
  inputBox = NW_FBox_InputBoxOf (box);

  NW_TRY(status) 
  {
    status = NW_FBox_Epoc32InputSkin_GetMaxCharExtent(inputBox, &charExtent);
    NW_THROW_ON_ERROR (status);
    
    /* set width and height basing on the size of the input box */
    NW_GDI_Metric_t boxWidth = NW_FBox_InputBox_GetWidth(inputBox);
    size->width = (NW_GDI_Metric_t) 0;
    if (boxWidth > 0)
        {
    size->width = (NW_GDI_Metric_t)
                       ((NW_FBox_InputBox_GetWidth(inputBox)+1)*charExtent.width);
        }
    
    defaultHeight = (NW_GDI_Metric_t)
        (NW_FBox_InputBox_GetHeight(inputBox)*
            (charExtent.height + NW_FBOX_INPUTBOX_LINE_SPACING_IN_PIXELS));
    size->height = defaultHeight;

    /*  J-Sky extensions: if width or height properties specified use those. */
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
    if (status == KBrsrSuccess)
      size->width = sizeVal.integer;

    /* Zero height input box defaults to the height of the line */
    status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &sizeVal);
    if (status == KBrsrSuccess && sizeVal.integer > defaultHeight)
      size->height = sizeVal.integer;

    if (size->width == 0)
    {
      NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(inputBox));
      NW_ASSERT(rootBox);

      CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
      NW_ASSERT(deviceContext);

      const NW_GDI_Rectangle_t* rectangle = deviceContext->DisplayBounds();

      size->width = rectangle->dimension.width;      

      NW_LMgr_Box_t* tableBox = NW_LMgr_BoxOf(inputBox);
      while (tableBox != NULL) 
      {
         if (NW_Object_IsClass(tableBox, &NW_LMgr_StaticTableCellBox_Class)) 
         {
           break;
         }
         tableBox = (NW_LMgr_Box_t*) NW_LMgr_Box_GetParent(tableBox);
      }
      if (tableBox != NULL) 
      {
         size->width = size->width / 3;
      }
    }

    /* In case of password box, the following code, converts inputbox value
       to * and stores it in InputSkin */
    if (NW_Object_IsClass(inputBox, &NW_FBox_PasswordBox_Class))
    {
      status = _NW_FBox_PasswordBox_SetObscureDisplayText(inputBox, NW_TRUE);
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
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_Split(NW_FBox_InputSkin_t* inputSkin,
                               NW_GDI_Metric_t space,
                               NW_LMgr_Box_t* *splitBox,
                               NW_Uint8 flags)
{
  NW_LMgr_PropertyValue_t sizeVal;
  TBrowserStatusCode status;
  NW_LMgr_Box_t* box;
  NW_FBox_FormBox_t* formBox;

  formBox = NW_FBox_Skin_GetFormBox(NW_FBox_SkinOf(inputSkin));
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

  /* if width was specified we do not want to change it */
  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
  if (status == KBrsrSuccess){
    return status;
  }
  boxBounds.dimension.width = space;
  NW_LMgr_Box_SetFormatBounds( box, boxBounds );

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_DrawText (NW_FBox_InputSkin_t* skin,
                                   CGDIDeviceContext* deviceContext)
  {
  NW_FBox_Epoc32InputSkin_t* thisObj;
  NW_ADT_Vector_Metric_t numTexts;
  NW_ADT_Vector_Metric_t index;
  NW_GDI_Point2D_t start;
  NW_FBox_InputBox_t* inputBox;
  CGDIFont* font;
  TGDIFontInfo fontInfo;
  NW_GDI_Rectangle_t innerRect;
  NW_LMgr_PropertyValue_t colorVal;
  NW_GDI_Color_t fgColor;
  NW_LMgr_Property_t prop;
  NW_GDI_FlowDirection_t dir;

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(skin, &NW_FBox_InputSkin_Class));
  inputBox = NW_FBox_InputSkin_GetInputBox(skin);
  NW_ASSERT(NW_Object_IsInstanceOf(inputBox, &NW_FBox_InputBox_Class));

  thisObj = NW_FBox_Epoc32InputSkinOf(skin);

  NW_TRY(status) {
     /* Get the box font */
    font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(inputBox));
    NW_ASSERT(font != NULL);

    /* Get info */
    (void)font->GetFontInfo ( &fontInfo);

    /* Get the old foreground color */
    fgColor = deviceContext->ForegroundColor ();

    /* Get and set the box fg color */
    colorVal.integer = 0;
    NW_LMgr_Box_GetPropertyValue(NW_LMgr_BoxOf(inputBox), NW_CSS_Prop_color, NW_CSS_ValueType_Color, &colorVal);

    deviceContext->SetForegroundColor ( (NW_GDI_Color_t)colorVal.integer);

    /* Get the start coordinates */
    NW_LMgr_Box_GetInnerRectangle(NW_LMgr_BoxOf(inputBox), &innerRect);
    innerRect.dimension.height -= extraPadding.height;
    innerRect.dimension.width -= extraPadding.width;
    start = innerRect.point;

    /* Set number of rows to be displayed */
    numTexts = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(thisObj->textList));

    /* maybe we didn't split the texts */
    if (numTexts == 0)
      {
      status = NW_FBox_InputSkin_SetDisplayText(skin);
      NW_THROW_ON_ERROR (status);
      numTexts = NW_ADT_Vector_GetSize(NW_ADT_VectorOf(thisObj->textList));
      }
    if (numTexts > NW_FBox_InputBox_GetHeight(inputBox))
      {
      numTexts = NW_FBox_InputBox_GetHeight(inputBox);
      }

    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_ltr;
    NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(inputBox), NW_CSS_Prop_textDirection, &prop);

    dir = (prop.value.token == NW_CSS_PropValue_ltr) ? NW_GDI_FlowDirection_LeftRight : NW_GDI_FlowDirection_RightLeft;
    if (dir == NW_GDI_FlowDirection_RightLeft && start.x > 0)
      { 
      innerRect.dimension.width -= extraPadding.width;
      }

    /* draw Text */
    for (index = 0; index < numTexts; index++) 
      {
      NW_Text_t* text;
      NW_Text_t* convertedText;

      /* the index of the row to be drawn */
      text = *(NW_Text_t**)
             NW_ADT_Vector_ElementAt(thisObj->textList, index);
      NW_ASSERT (NW_Object_IsInstanceOf (text, &NW_Text_UCS2_Class));

      convertedText = NW_FBox_Skin_NonprintableToSpace (text);
      NW_THROW_OOM_ON_NULL (convertedText, status);

      status = deviceContext->DrawText ( &start,
          convertedText, font, 0, NW_GDI_TextDecoration_None, dir, innerRect.dimension.width);
      NW_THROW_ON_ERROR (status);

      if(convertedText != text)
        {
        NW_Object_Delete(convertedText);
        }
      start.y = (NW_GDI_Metric_t)(start.y + 
          fontInfo.height + NW_FBOX_INPUTBOX_LINE_SPACING_IN_PIXELS);
      }

    deviceContext->SetForegroundColor ( fgColor);
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
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_SetDisplayText(NW_FBox_InputSkin_t* skin)
{
  NW_FBox_Epoc32InputSkin_t* epoc32InputSkin;
  epoc32InputSkin = NW_FBox_Epoc32InputSkinOf(skin);
  return NW_FBox_Epoc32InputSkin_RecreateTextList(epoc32InputSkin);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_DetailsDialog (NW_FBox_InputSkin_t* thisObj)
{
  NW_REQUIRED_PARAM(thisObj);
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32InputSkin_SetVKB(NW_FBox_InputSkin_t* aSkin)
{
  NW_FBox_Epoc32InputSkin_t* thisObj = NW_FBox_Epoc32InputSkinOf(aSkin);
  
  //retrieve the epoc32 input box
  if (thisObj && thisObj->cppEpoc32InputBox)
  {
    thisObj->cppEpoc32InputBox->ActivateVKB();
  }
  
  // also try to launch async in case dialog is up
  if (thisObj->vkbTimer == NULL) 
    {
      thisObj->vkbTimer = NW_System_Timer_New (NW_FBox_InputSkin_VKBCallback,
                                                 thisObj, 4000, NW_FALSE);
    }


  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
_NW_FBox_Epoc32InputSkin_SetActive (NW_FBox_InputSkin_t* aSkin)
{
  NW_FBox_InputSkin_t* skin = (NW_FBox_InputSkin_t*)aSkin;
  NW_GDI_Rectangle_t innerRect;
  NW_LMgr_FrameInfo_t padding;
  TMargins8 margins;
  NW_Uint32 version; /* WBXML encoded to WAPFORUM WML spec version 1.x */  
  NW_LMgr_Property_t prop;
  NW_FBox_Epoc32InputSkin_t* thisObj = NW_FBox_Epoc32InputSkinOf(skin);

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(skin, &NW_FBox_Epoc32InputSkin_Class));

  NW_TRY(status)
  {
    /* for convenience */
    status = NW_FBox_Epoc32InputSkin_Redraw(thisObj);
    NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox (skin);
    NW_ASSERT(inputBox);
    
    /* get form liaison - required for WBXML version */
    NW_FBox_FormLiaison_t* formLiaison =  NW_FBox_FormBox_GetFormLiaison(inputBox);
    NW_ASSERT(formLiaison);

    /* get device context */
    NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(inputBox));
    NW_ASSERT(rootBox); 
    CGDIDeviceContext* deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    NW_ASSERT(deviceContext);

    /* get the container */
    MBoxTreeListener* listener = NW_LMgr_RootBox_GetBoxTreeListener(rootBox);
    NW_ASSERT(listener);
    CCoeControl* container = (CCoeControl*)listener->GetParentWindow();

    NW_ASSERT(container);

    /* get the epoc32rect */
    NW_LMgr_Box_GetInnerRectangle(NW_LMgr_BoxOf(inputBox), &innerRect);
    innerRect.dimension.height -= extraPadding.height;
    innerRect.dimension.width -= extraPadding.width;
    NW_GDI_Point2D_t origin = *(deviceContext->Origin());
    NW_GDI_Rectangle_t bounds = NW_LMgr_Box_GetDisplayBounds( NW_LMgr_BoxOf( inputBox ) );
    TRect epoc32Rect( TPoint( bounds.point.x - origin.x, bounds.point.y - origin.y), 
    TSize( bounds.dimension.width, bounds.dimension.height) );
  
    /* get format and emptyok properties */
    NW_FBox_Validator_EmptyOk_t isEmptyOk = NW_FBox_Validator_EmptyOk_None;
    const NW_Ucs2* formatBuffer = NULL;
    NW_FBox_Validator_Mode_t mode = NW_FBox_Validator_Mode_None;

    NW_Text_Length_t formatBufferLength = 0;
  
    if ( inputBox->validator )
    { 
      status = NW_FBox_Validator_GetFormat(inputBox->validator, &formatBuffer, &mode, &isEmptyOk);
      _NW_THROW_ON_ERROR(status);
    
      if (formatBuffer)
      {
        formatBufferLength = (NW_Text_Length_t) NW_Str_Strlen( formatBuffer );
      }
    }
  
    /* get initial text from */
    NW_Ucs2* initialStringBuffer=NULL;
    NW_Text_Length_t initialStringLength=0;
    if ( inputBox->value )
    {
      initialStringBuffer = (NW_Ucs2*)inputBox->value->storage;
      initialStringLength = inputBox->value->characterCount;
    }
    
    /* get isSecret parameter */
    TBool isSecret = EFalse;
    if(NW_Object_IsInstanceOf(inputBox, &NW_FBox_PasswordBox_Class))
    {
      isSecret = ETrue;
    }
  
    /* get font */
    CGDIFont* font = NW_LMgr_Box_GetFont( NW_LMgr_BoxOf( inputBox ) );
    NW_THROW_OOM_ON_NULL(font, status);
    CFont* epocFont = (CFont*) 
      deviceContext->GetScaledCFont(  font );

    /* get max char extent */
    NW_GDI_Dimension2D_t maxCharExtent;
    status = NW_FBox_Epoc32InputSkin_GetMaxCharExtent(inputBox, &maxCharExtent);
    _NW_THROW_ON_ERROR(status);

    /* Get box info */
    NW_LMgr_Box_GetPadding(NW_LMgr_BoxOf(inputBox), &padding, ELMgrFrameAll );

    margins.iLeft = (TInt8)(padding.left+1);
    margins.iTop = (TInt8)(padding.top+1);
    margins.iRight = (TInt8)padding.right;
    margins.iBottom = (TInt8)padding.bottom;

    /* if there's one line only - wrap width is one char more to 
     * avoid jumping while resizing */
    TInt wrapWidth = innerRect.dimension.width;
    status = NW_FBox_FormLiaison_GetWBXMLVersion(formLiaison, &version);
    _NW_THROW_ON_ERROR(status);


    prop.type = NW_CSS_ValueType_Token;
    prop.value.token = NW_CSS_PropValue_ltr;
    NW_LMgr_Box_GetProperty(NW_LMgr_BoxOf(inputBox), NW_CSS_Prop_textDirection, &prop);

    /* create CEpoc32InputBox */
    TRAPD( err, thisObj->cppEpoc32InputBox = 
      CEpoc32InputBox::NewL ( 
          container, 
          thisObj,
          epoc32Rect,
          margins,
          wrapWidth,
          TPtrC( initialStringBuffer, initialStringLength ),
          TPtrC( formatBuffer, formatBufferLength ), 
          mode,
          isSecret,
          isEmptyOk,             
          version, 
          NW_FBox_InputBox_GetMaxChars( inputBox ),
          *epocFont,
          (prop.value.token == NW_CSS_PropValue_ltr) ? ELayoutAlignLeft : ELayoutAlignRight) );
    if (err == KErrNoMemory)
    {
      NW_THROW_STATUS(status, KBrsrOutOfMemory);
    }
    if (err!=KErrNone)
    {
      NW_THROW_STATUS(status, KBrsrFailure);
    }
  
    (void) NW_FBox_FormLiaison_DelegateEcmaEvent (formLiaison, 
                                           NW_FBox_FormBox_GetFormCntrlID(inputBox), 
                                           NW_Ecma_Evt_OnClick);
  }
  NW_CATCH(status) { }
  NW_FINALLY 
  {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode 
_NW_FBox_Epoc32InputSkin_SetFocus( NW_FBox_InputSkin_t* aSkin, NW_Bool setFocus )
    {
    TBrowserStatusCode status = KBrsrSuccess;
    NW_FBox_InputSkin_t* skin = ( NW_FBox_InputSkin_t* ) aSkin;
    /* parameter assertion block */
    NW_ASSERT(NW_Object_IsInstanceOf( skin, &NW_FBox_Epoc32InputSkin_Class ) );
    /* for convenience */
    NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox( skin );
    NW_ASSERT( inputBox );
    if( setFocus && !inputBox->focus )
      {
      inputBox->focus = NW_TRUE;
      /* get form liaison - required for WBXML version */
      NW_FBox_FormLiaison_t* formLiaison =  
                        NW_FBox_FormBox_GetFormLiaison( inputBox );
      NW_ASSERT(formLiaison);
      // generate focus event
      status = NW_FBox_FormLiaison_DelegateEcmaEvent( 
                                    formLiaison, 
                                    NW_FBox_FormBox_GetFormCntrlID( inputBox ), 
                                    NW_Ecma_Evt_OnFocus );
      }
    else if ( !setFocus && inputBox->focus )
      inputBox->focus = NW_FALSE;
    return status;
    }

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

                                
/* ------------------------------------------------------------------------- */
void
NW_FBox_Epoc32InputSkin_Scroll(NW_FBox_Epoc32InputSkin_t* thisObj, TInt deltaY)
{
  NW_GDI_Point2D_t origin;
  NW_FBox_InputBox_t* inputBox;
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_Epoc32InputSkin_Class));

    /* get the device context */
    inputBox = NW_FBox_InputSkin_GetInputBox (thisObj);
    NW_ASSERT(inputBox);

    rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(inputBox));
    NW_ASSERT(rootBox);

    deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    NW_ASSERT(deviceContext);

    origin = *(deviceContext->Origin());

    origin.y = (NW_GDI_Metric_t) (origin.y - deltaY);

    deviceContext->SetOrigin(  &origin );
}
                                
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_GetViewRect(NW_FBox_Epoc32InputSkin_t* thisObj, TRect& viewRect)
{
  NW_GDI_Rectangle_t viewBounds;
  NW_FBox_InputBox_t* inputBox;
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_Epoc32InputSkin_Class));

  NW_TRY(status)
  {
    /* get the device context */
    inputBox = NW_FBox_InputSkin_GetInputBox (thisObj);
    NW_THROW_OOM_ON_NULL(inputBox, status);

    rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(inputBox));    
    NW_ASSERT(rootBox);
    deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
    NW_ASSERT(deviceContext);

    viewBounds = *(deviceContext->DisplayBounds());

    viewRect.iTl.iX = viewBounds.point.x;
    viewRect.iTl.iY = viewBounds.point.y;
    viewRect.iBr.iX = viewBounds.point.x + viewBounds.dimension.width;
    viewRect.iBr.iY = viewBounds.point.y + viewBounds.dimension.height;
  }
  NW_CATCH(status) {}
  NW_FINALLY 
  {
    return status;
  }
  NW_END_TRY
}

 
/* ------------------------------------------------------------------------- */
static
void NW_FBox_Epoc32InputSkin_SaveTextAndCloseEdwin(NW_FBox_Epoc32InputSkin_t* thisObj)
{
  NW_FBox_InputBox_t* inputBox; /* pointer to associated InputBox class */ 
  HBufC* buf = NULL;
  HBufC* newBuf = NULL;

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_Epoc32InputSkin_Class));

  NW_TRY(status)
  {
    thisObj->posCursorValue = thisObj->cppEpoc32InputBox->CurPos();
    thisObj->reallyEdited = NW_TRUE;

    inputBox = NW_FBox_InputSkin_GetInputBox (thisObj);

    NW_THROW_OOM_ON_NULL(inputBox, status);

    /* get text from edwin and save it to the form liaison */
    buf = thisObj->cppEpoc32InputBox->ReadableText();
    NW_THROW_OOM_ON_NULL(buf, status);

    /* deactivate input box */
    NW_FBox_InputBox_SetActive (inputBox, NW_FALSE);

    /* SetActive doesn't return error status, so make sure that we're not active */
    if (NW_FBox_InputBox_IsActive(inputBox)==NW_FALSE)
    {
      /* destroy edwin */
      delete thisObj->cppEpoc32InputBox;
      thisObj->cppEpoc32InputBox = NULL;

      /* replace all NW_TEXT_UCS2_PARASEP with NW_TEXT_UCS2_CR and NW_TEXT_UCS2_LF */
      TInt pos = 0;
      TPtr ptr = buf->Des();
      TInt len = buf->Length(); 

      // count how many NW_TEXT_UCS2_PARASEP so we know how big buffer we need
      TInt count = 0;
      while ( pos < len )
      {
        if ( ptr[pos++] == NW_TEXT_UCS2_PARASEP )
        {
          count++;
        }
      }

      if (count == 0)
      {
        status=NW_FBox_Epoc32InputSkin_SetValue(thisObj, 
                                                buf->Des().Ptr(), 
                                                (NW_Text_Length_t)buf->Length() );
      }
      else
      {
        // alloc the new buffer
        newBuf = HBufC::NewL( len + count);
        NW_THROW_OOM_ON_NULL(newBuf, status);

        TPtr newPtr(newBuf->Des());
        newPtr.Copy(ptr); 
        pos = 0;

        _LIT(Lf, "\n"); // For NW_TEXT_UCS2_LF
        while ( (pos=newPtr.Locate(TChar(NW_TEXT_UCS2_PARASEP))) != KErrNotFound )
        {
          newPtr[pos++] = NW_TEXT_UCS2_CR;
          newPtr.Insert(pos, Lf);
        }
        status=NW_FBox_Epoc32InputSkin_SetValue(thisObj, 
                                                newBuf->Des().Ptr(), 
                                                (NW_Text_Length_t)newBuf->Length() );
        delete newBuf;
      }

      NW_THROW_ON_ERROR(status);

      /* update value to the form liaison */
      status=NW_FBox_InputBox_SetFormLiaisonVal(inputBox);
      NW_THROW_ON_ERROR(status);

      if(status  == KBrsrSuccess)
      {
        NW_FBox_FormLiaison_t* formLiaison =  NW_FBox_FormBox_GetFormLiaison(inputBox);
        (void) NW_FBox_FormLiaison_DelegateEcmaEvent (formLiaison, 
                                               NW_FBox_FormBox_GetFormCntrlID(inputBox), 
                                               NW_Ecma_Evt_OnChange);
      }



      status=NW_FBox_Epoc32InputSkin_Redraw(thisObj);
      NW_THROW_ON_ERROR(status);
    }
  }
  NW_CATCH(status) {}
  NW_FINALLY 
  {
    delete buf;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_TryExitEditMode(NW_FBox_Epoc32InputSkin_t* thisObj, NW_Bool saveText)
{
  NW_FBox_InputBox_t* inputBox; /* pointer to associated InputBox class */

  /* parameter assertion block */
  NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_FBox_Epoc32InputSkin_Class));
  
  /* for convenience */
  NW_TRY(status)
  {
    inputBox = NW_FBox_InputSkin_GetInputBox (thisObj);
    NW_THROW_OOM_ON_NULL(inputBox, status);

    NW_THROW_ON_NULL(thisObj->cppEpoc32InputBox, status, KBrsrFailure);

    /* if user wants to save his input */
    if (saveText == NW_TRUE)
    {
      /* make sure that input is correct, so we can save it */
      if (thisObj->cppEpoc32InputBox->OkToExit())
      {
        NW_FBox_Epoc32InputSkin_SaveTextAndCloseEdwin( thisObj );
        NW_FBox_FormLiaison_t* formLiaison =  NW_FBox_FormBox_GetFormLiaison(inputBox);
        (void) NW_FBox_FormLiaison_DelegateEcmaEvent (formLiaison, 
                                               NW_FBox_FormBox_GetFormCntrlID(inputBox), 
                                               NW_Ecma_Evt_OnBlur);
      }
      else
        NW_THROW_STATUS(status, KBrsrFailure);
    }
    else
    {
      /* restore original value from the form liaison */
      NW_Text_t* buffer = NULL;
      NW_FBox_FormLiaison_t* formLiaison = NW_FBox_FormBox_GetFormLiaison(inputBox);
      NW_THROW_OOM_ON_NULL(formLiaison, status);

      void* controlId = NW_FBox_FormBox_GetFormCntrlID(inputBox);
      NW_Bool dummy = NW_FALSE;  /* Not used here, but informs us if "value" attribute used */
      status = NW_FBox_FormLiaison_GetStringValue(formLiaison, controlId, &buffer, &dummy);
      NW_THROW_ON_ERROR(status);
      
      status = NW_FBox_InputBox_SetDefaultVal(inputBox, buffer);
      NW_THROW_ON_ERROR(status);

      /* restore previous cursor position when cancel input */
      thisObj->posCursorValue = thisObj->prevCursorValue;

      // if NO static characters involved in format - cancel means NOT REALLY EDITED.
      // if static characters are involved - "reallyEdited" will determine what action
      // - read, "what initial value" will be displayed by CEpoc32InputBox::SetInitialTextL.
      if (!thisObj->cppEpoc32InputBox->IsStaticCharInvolved())
      {
        thisObj->reallyEdited = NW_FALSE;
      }
      NW_FBox_InputBox_SetActive(inputBox, NW_FALSE);

      /* SetActive doesn't return error status, so make sure that we're not active */
      if (NW_FBox_InputBox_IsActive(inputBox)==NW_FALSE)
      {
        /* destroy edwin */
        delete thisObj->cppEpoc32InputBox;
        thisObj->cppEpoc32InputBox = NULL;
      }

      /* text has changed - relayout required */
      status = NW_FBox_InputSkin_SetDisplayText(NW_FBox_InputSkinOf(thisObj));
      NW_THROW_ON_ERROR(status);
    }
  }
  NW_CATCH(status){
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_FBox_Epoc32InputSkin_TextChanged(NW_FBox_Epoc32InputSkin_t* thisObj,
                                    const NW_Ucs2* string)
{
  const NW_Ucs2* storage;
  NW_Text_Length_t length;
  NW_Bool freeNeeded;
  NW_Int32 ret;

  NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox (thisObj);
  if (inputBox->value == NULL){
    return NW_TRUE;
  }

  storage = NW_Text_GetUCS2Buffer (inputBox->value, 0, &length, &freeNeeded) ;
  if (storage == NULL) {
    return NW_TRUE;
  }

  ret = NW_Str_Stricmp(storage, string);
  if (freeNeeded){
    NW_Mem_Free((NW_Ucs2*)storage);
  }
  if (ret == 0){
    return NW_FALSE;
  }
  return NW_TRUE;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_SetValue(NW_FBox_Epoc32InputSkin_t* thisObj, 
                                 const NW_Ucs2* string,
                                 NW_Text_Length_t length)
{
  NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox (thisObj);


  if (inputBox->value == NULL)
  {
    inputBox->value = (NW_Text_t*)
        NW_Text_UCS2_New((void*)string, length, NW_Text_Flags_Copy);
    if (inputBox->value == NULL)
    {
      return KBrsrOutOfMemory;
    }
    return KBrsrSuccess;
  }
  TBrowserStatusCode status = NW_Text_SetStorage(inputBox->value, (void*)string, length, NW_Text_Flags_Copy);

  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_Redraw(NW_FBox_Epoc32InputSkin_t* thisObj)
{
  NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox (thisObj);
  NW_ASSERT(inputBox);
  return NW_LMgr_Box_Refresh(NW_LMgr_BoxOf(inputBox));
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_Epoc32InputSkin_RedrawScreen(NW_FBox_Epoc32InputSkin_t* thisObj)
{
  NW_LMgr_RootBox_t* rootBox;
  MBoxTreeListener* boxTreeListener;
  NW_FBox_InputBox_t* inputBox = NW_FBox_InputSkin_GetInputBox (thisObj);
  NW_ASSERT (inputBox != NULL);
  
  /* we're going to need the deviceContext to redraw the box */
  rootBox = NW_LMgr_Box_GetRootBox(NW_LMgr_BoxOf(inputBox));
  NW_ASSERT (rootBox != NULL);

  boxTreeListener = NW_LMgr_RootBox_GetBoxTreeListener(rootBox);
  NW_ASSERT (boxTreeListener != NULL);

  return boxTreeListener->RedrawDisplay (NW_TRUE);
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_InputSkin_t*
NW_FBox_InputSkin_New (NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_InputSkin_t*)
    NW_Object_New (&NW_FBox_Epoc32InputSkin_Class, formBox);
}

