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


#include "nw_fbox_buttonboxskini.h"
#include "nw_fbox_buttonbox.h"
#include "nw_lmgr_rootbox.h"
#include "GDIDevicecontext.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
/* Get the dir attribute */
static
void
NW_FBox_ButtonBoxSkin_GetDirection(NW_LMgr_Box_t *box,
                                   NW_GDI_FlowDirection_t* dir)
{
  NW_LMgr_Property_t dirProp;
  NW_ASSERT(box != NULL);

  dirProp.type = NW_CSS_ValueType_Token;
  dirProp.value.token = NW_CSS_PropValue_ltr;
  NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_textDirection, &dirProp);
  if (dirProp.value.token == NW_CSS_PropValue_ltr) {
    *dir = NW_GDI_FlowDirection_LeftRight;
  }
  else {
    *dir = NW_GDI_FlowDirection_RightLeft;
  }
}


/* ------------------------------------------------------------------------- */
const
NW_FBox_ButtonBoxSkin_Class_t  NW_FBox_ButtonBoxSkin_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_FBox_Skin_Class,
    /* queryInterface              */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize                */ sizeof (NW_FBox_ButtonBoxSkin_t),
    /* construct                   */ _NW_FBox_Skin_Construct,
    /* destruct                    */ NULL
  },
  { /* NW_FBox_Skin                */
    /* NW_FBox_Skin_Draw_t         */ _NW_FBox_ButtonBoxSkin_Draw,
    /* NW_FBox_Skin_GetSize_t      */ _NW_FBox_ButtonBoxSkin_GetSize
  },
  { /* NW_FBox_ButtonBoxSkin */
    /* drawContent                 */ _NW_FBox_ButtonBoxSkin_DrawContent
  }
};


/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_FBox_ButtonBoxSkin_Draw(NW_FBox_Skin_t* skin,
                        NW_LMgr_Box_t* box,
                        CGDIDeviceContext* deviceContext,
                        NW_Uint8 hasFocus)
{
  NW_FBox_ButtonBoxSkin_t* thisObj;
  TBrowserStatusCode status;
  
  /* for convenience */
  thisObj = NW_FBox_ButtonBoxSkinOf(skin);

  /* Let the base class draw draw the box */
  status = NW_LMgr_Box_Class.NW_LMgr_Box.draw(box, deviceContext, hasFocus);
  if (status != KBrsrSuccess){
    return status;
  }

  return NW_FBox_ButtonBoxSkin_DrawContent(thisObj, box, deviceContext, hasFocus);
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_ButtonBoxSkin_DrawContent(NW_FBox_ButtonBoxSkin_t* skin,
                                   NW_LMgr_Box_t* box,
                                   CGDIDeviceContext* deviceContext,
                                   NW_Uint8 hasFocus)
{
  NW_FBox_ButtonBox_t* buttonBox;
  NW_GDI_Point2D_t textStart;
  NW_GDI_Rectangle_t innerRect;
  NW_Text_t* value;
  NW_GDI_FlowDirection_t dir = NW_GDI_FlowDirection_LeftRight;
  
  NW_REQUIRED_PARAM(skin);
  NW_REQUIRED_PARAM(hasFocus);

  /* for convenience */
  buttonBox = NW_FBox_ButtonBoxOf (box);

  /* this is the label */
  value = NW_FBox_ButtonBox_GetValue(buttonBox);

  /* Calculate the inner rectangle */
  NW_LMgr_Box_GetInnerRectangle(box, &innerRect);
 
  /* Get the start coordinates */
  textStart = innerRect.point;

  /* Make button label draw itself at the specified location */
  deviceContext->SetPaintMode ( NW_GDI_PaintMode_Copy);

  if ( value != NULL){
    NW_FBox_ButtonBoxSkin_GetDirection(box, &dir);
	(void) deviceContext->DrawText( &textStart, 
                                  value,
                                  NW_LMgr_Box_GetFont(box),
                                  0, 
                                  NW_GDI_TextDecoration_None, 
                                  dir, 
                                  innerRect.dimension.width);
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_ButtonBoxSkin_GetSize(NW_FBox_Skin_t* skin,
                                     NW_LMgr_Box_t* box,
                                     NW_GDI_Dimension3D_t* size)
  {
  NW_FBox_ButtonBox_t* buttonBox;
  NW_Text_t* value;
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;
  CGDIFont* font;
  TBrowserStatusCode status;
  NW_GDI_Dimension2D_t textExtent;
  NW_LMgr_PropertyValue_t sizeVal;
  NW_GDI_FlowDirection_t dir;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (box, &NW_FBox_ButtonBox_Class));
  
  sizeVal.integer = 0;
  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_width, NW_CSS_ValueType_Px, &sizeVal);
  size->width = sizeVal.integer;
  sizeVal.integer = 0;
  status = NW_LMgr_Box_GetPropertyValue(box, NW_CSS_Prop_height, NW_CSS_ValueType_Px, &sizeVal);
  size->height = sizeVal.integer;

  if (size->width != 0 && size->height != 0)
    {
    return KBrsrSuccess;
    }

  buttonBox = NW_FBox_ButtonBoxOf (box);
  (void)skin;

  /* Get the value */
  value = NW_FBox_ButtonBox_GetValue(buttonBox);

  /* we're going to need the deviceContext to do text extent calculations */
  rootBox = NW_LMgr_Box_GetRootBox (box);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);

  /* Set the font */
  font = NW_LMgr_Box_GetFont (box);
  NW_ASSERT(font != NULL);

  NW_FBox_ButtonBoxSkin_GetDirection(box, &dir);
  /* calculate the extents */
  status = deviceContext->GetTextExtent( value, font, 0,
                                         dir, &textExtent);
  if (status == KBrsrOutOfMemory) 
    {
    return KBrsrSuccess;
    }

  if (size->height == 0) 
    {
    size->height = textExtent.height;
    }
  
  if (size->width == 0)
    {
    size->width = textExtent.width;
    }

  return KBrsrSuccess;
  }




