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


#include "nw_fbox_inputskini.h"
#include "nw_lmgr_rootbox.h"
#include "nw_text_ucs2.h"
#include "nw_fbox_inputbox.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"
/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_InputSkin_Class_t  NW_FBox_InputSkin_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_FBox_ButtonBoxSkin_Class,
    /* queryInterface              */ _NW_Object_Base_QueryInterface
  },
  {
    /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize                */ sizeof (NW_FBox_InputSkin_t),
    /* construct                   */ _NW_FBox_InputSkin_Construct,
    /* destruct                    */ _NW_FBox_InputSkin_Destruct
  },
  { /* NW_FBox_Skin                */
    /* NW_FBox_Skin_Draw_t         */ _NW_FBox_InputSkin_Draw,
    /* NW_FBox_Skin_GetSize_t      */ _NW_FBox_InputSkin_GetSize
  },
  { /* NW_FBox_InputSkin */
    /* drawText                    */ NULL,
    /* moveCursorLeft              */ NULL,
    /* moveCursorRight             */ NULL,
    /* clearLastChar               */ NULL,
    /* setDisplayText              */ NULL,
    /* setActive                   */ _NW_FBox_InputSkin_SetActive,
    /* detailsDialog               */ NULL,
    /* split                       */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InputSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_FBox_InputSkin_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_FBox_InputSkinOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_FBox_Skin_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }
  thisObj->displayText = NULL;

  /* successful completion */
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
void
_NW_FBox_InputSkin_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_FBox_InputSkin_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_InputSkinOf (dynamicObject);

  if (thisObj->displayText){
    NW_Object_Delete(thisObj->displayText);
    thisObj->displayText = NULL;
  }
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InputSkin_Draw(NW_FBox_Skin_t* skin,
                        NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext, 
                        NW_Uint8 hasFocus)
{
  NW_FBox_InputSkin_t* thisObj;
  TBrowserStatusCode status = KBrsrSuccess;
  /* for convenience */
  thisObj = NW_FBox_InputSkinOf(skin);
  /* Let the base class draw draw the box */
  (void) NW_LMgr_Box_Class.NW_LMgr_Box.draw(box, deviceContext, hasFocus);
  status = NW_FBox_InputSkin_DrawText(thisObj, deviceContext);
  if ( status != KBrsrSuccess)
    return status;
  status = NW_FBox_InputSkin_SetFocus( thisObj, hasFocus);
  return status;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InputSkin_GetSize(NW_FBox_Skin_t* skin,
                           NW_LMgr_Box_t* box,
                           NW_GDI_Dimension3D_t* size)
{
  NW_FBox_InputBox_t* inputBox;
  NW_GDI_Dimension2D_t mExtent;
  NW_GDI_Metric_t inputWidth;
  NW_GDI_Metric_t inputHeight;
  NW_Text_Length_t width;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_InputBox_Class));

  /* for convenience */
  inputBox = NW_FBox_InputBoxOf (box);

  TBrowserStatusCode status = NW_FBox_InputSkin_GetMCharExtent(NW_FBox_InputSkinOf(skin), &mExtent);
  NW_ASSERT(status == KBrsrSuccess);

  /* Calculate the size */
  width = NW_FBox_InputBox_GetWidth(inputBox);
  if (width == 0) {
    width = 4;
  }
  inputWidth = (NW_GDI_Metric_t)
               ((mExtent.width)*width);
	/* one pixel is added for space between the line */
  inputHeight = (NW_GDI_Metric_t)
                ((mExtent.height + 1)*(NW_FBox_InputBox_GetHeight(inputBox)));

	/* For the first line we don't need to add one pixel. */
	inputHeight--;
  size->height = inputHeight;
  size->width = inputWidth;
 
  /* Get the size of the image itself */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_InputSkin_SetActive (NW_FBox_InputSkin_t* skin)
{
  NW_REQUIRED_PARAM(skin);
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_InputSkin_GetMCharExtent(NW_FBox_InputSkin_t* skin,
                                 NW_GDI_Dimension2D_t *charExtent)
  {
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;
  static const NW_Ucs2 mWidth[] = {'M'};
  NW_Text_UCS2_t* text;
  TBrowserStatusCode status;
  NW_FBox_InputBox_t* box;
  CGDIFont* font;

  NW_ASSERT(NW_Object_IsInstanceOf(skin, &NW_FBox_InputSkin_Class));
  box = NW_FBox_InputSkin_GetInputBox(skin);
  NW_ASSERT(NW_Object_IsInstanceOf(box, &NW_FBox_InputBox_Class));

  text = NW_Text_UCS2_New ((NW_Ucs2*) mWidth, 1, 0);
  /* we're going to need the deviceContext to do text extent calculations */
  rootBox = NW_LMgr_Box_GetRootBox (box);
  NW_ASSERT (rootBox != NULL);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  NW_ASSERT (deviceContext != NULL);

  font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(box));
  NW_ASSERT(font != NULL);

  /* calculate the extents */
  status = deviceContext->GetTextExtent( NW_TextOf(text), 
                                         font,
                                         0, 
                                         NW_GDI_FlowDirection_LeftRight,
                                         charExtent );
  NW_Object_Delete(text);

  if (status == KBrsrOutOfMemory) 
    {
    return status;
    }

  return KBrsrSuccess;
  }
