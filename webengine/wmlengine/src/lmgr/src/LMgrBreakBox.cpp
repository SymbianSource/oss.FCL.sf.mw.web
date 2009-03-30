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


#include "nw_lmgr_breakboxi.h"

#include "nw_adt_resizablevector.h"
#include "nw_lmgr_cssproperties.h"
#include "nw_lmgr_rootbox.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_text_ucs2.h"
#include "nwx_settings.h"
#include "nw_lmgr_verticaltablecellbox.h"
#include "nw_lmgr_containerbox.h"
#include "GDIDeviceContext.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   private final methods
 * ------------------------------------------------------------------------- */

/* This method checks if the previous 2 sibling of this break box are also break
   boxes - If yes we do not want to display this break box 
   */
static
NW_Bool
NW_LMgr_BreakBox_HandleVerticalLayout(NW_LMgr_Box_t *box)
{
  NW_Bool displayBreak = NW_TRUE;
  NW_LMgr_Property_t prop;
  TBrowserStatusCode status = KBrsrSuccess;
  NW_LMgr_ContainerBox_t* parent = box->parent;

  if (NW_Object_IsClass(parent, &NW_LMgr_VerticalTableCellBox_Class))
  {
    NW_ADT_Vector_Metric_t childIndex = 0;
  
    childIndex = NW_LMgr_ContainerBox_GetChildIndex(parent, box);
    /* If index is zero then we don't want to display the break */
    if (childIndex == 0)
    {
      displayBreak = NW_FALSE;
    }
    else
    {
      NW_LMgr_Box_t* child = NW_LMgr_ContainerBox_GetChild(parent, (NW_ADT_Vector_Metric_t)(childIndex -1));
      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( child );

      if (!NW_Object_IsInstanceOf (child, &NW_LMgr_ActiveContainerBox_Class) && 
          boxBounds.dimension.width == 0 )
      {
        child = NW_LMgr_ContainerBox_GetChild(parent, (NW_ADT_Vector_Metric_t)(childIndex -2));
        if ( child)
        {
          if(boxBounds.dimension.width == 0)
          {
            displayBreak = NW_FALSE;
          }
        }
        else
        {
          displayBreak = NW_FALSE;
        }
      }
    }
  }
  else 
  {
    status = NW_LMgr_Box_GetProperty(box, NW_CSS_Prop_flags, &prop);
    if ((status == KBrsrSuccess) && (prop.value.token == NW_CSS_PropValue_flags_thirdBr))
    {
      /* do not display this break box since previous 2 siblings were break boxes */
      displayBreak = NW_FALSE;
    }
  }

  if (!displayBreak)
  {
    NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
    boxBounds.dimension.height = 0;
    boxBounds.dimension.width = 0;
    NW_LMgr_Box_SetFormatBounds( box, boxBounds );
  }

  return displayBreak;
}


/* -------------------------------------------------------------------------
   The static instance of the Format_Box class object 
 ------------------------------------------------------------------------- */
const
NW_LMgr_BreakBox_Class_t  NW_LMgr_BreakBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_Box_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_BreakBox_t),
    /* construct                 */ _NW_LMgr_Box_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_BreakBox_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_Box_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_Box_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_BreakBox_Constrain,
    /* draw                      */ _NW_LMgr_BreakBox_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_BreakBox_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_BreakBox          */
    /* unused                    */ 0
  }
};

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_BreakBox_Resize(NW_LMgr_Box_t *box, NW_LMgr_FormatContext_t* context)
  {
  NW_Bool displayBreak = NW_TRUE;
  NW_REQUIRED_PARAM(context);
  
  NW_LMgr_RootBox_t* rootBox = NW_LMgr_Box_GetRootBox( box );

  /* handle vertical layout for <br/> element */
  if( NW_LMgr_RootBox_GetSmallScreenOn( rootBox ) )
    {
    /* we want to determine if we need to display this break */
    displayBreak = NW_LMgr_BreakBox_HandleVerticalLayout(box);
    }

  if (displayBreak)
    {
    CGDIFont *font;
    /* Set the height to font height */
    font = NW_LMgr_Box_GetFont (box);
    if (font != NULL) 
      {
      TGDIFontInfo fontInfo;

      TBrowserStatusCode status = font->GetFontInfo ( &fontInfo);
      NW_ASSERT (status == KBrsrSuccess);

      NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );
      boxBounds.dimension.height = fontInfo.height;
      boxBounds.dimension.width = 1;
      NW_LMgr_Box_SetFormatBounds( box, boxBounds );
      }
    else 
      {
	    NW_ASSERT(0);
      }
    }
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_LMgr_BreakBox_Constrain(NW_LMgr_Box_t *box,
                            NW_GDI_Metric_t constraint) {

  NW_REQUIRED_PARAM(constraint);

  NW_GDI_Rectangle_t boxBounds = NW_LMgr_Box_GetFormatBounds( box );

  boxBounds.dimension.width = 0;

  NW_LMgr_Box_SetFormatBounds( box, boxBounds );

  return KBrsrSuccess;

}

/* ------------------------------------------------------------------------- */
NW_GDI_Metric_t
_NW_LMgr_BreakBox_GetBaseline(NW_LMgr_Box_t* box){

  CGDIFont *font;

  /* Set the height to font height */
  font = NW_LMgr_Box_GetFont (box);
  if (font != NULL) 
    {
    TGDIFontInfo fontInfo;

    TBrowserStatusCode status = font->GetFontInfo ( &fontInfo);
    NW_ASSERT (status == KBrsrSuccess);
		/* To fix TI compiler warning */
		(void) status; 

    return fontInfo.baseline;
    }
  else 
    {
	  NW_ASSERT(0);
    }

  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_LMgr_BreakBox_Draw (NW_LMgr_Box_t* box,
                   CGDIDeviceContext* deviceContext,
                   NW_Uint8 hasFocus) {

  NW_REQUIRED_PARAM (box);
  NW_REQUIRED_PARAM (deviceContext);
  NW_REQUIRED_PARAM (hasFocus);

  return KBrsrSuccess;
}

/* -------------------------------------------------------------------------
  convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_BreakBox_t*
NW_LMgr_BreakBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_BreakBox_t*)
    NW_Object_New (&NW_LMgr_BreakBox_Class, numProperties);
}

