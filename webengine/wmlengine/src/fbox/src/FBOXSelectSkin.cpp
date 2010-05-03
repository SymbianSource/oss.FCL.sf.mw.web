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


#include "nw_fbox_selectskini.h"
#include "nw_lmgr_rootbox.h"
#include "nw_text_ucs2.h"
#include "GDIDevicecontext.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_SelectSkin_Class_t  NW_FBox_SelectSkin_Class = {
  { /* NW_Object_Core             */
    /* super                      */ &NW_FBox_ButtonBoxSkin_Class,
    /* queryInterface             */ _NW_Object_Base_QueryInterface
  },
  {
    /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize                */ sizeof (NW_FBox_SelectSkin_t),
    /* construct                   */ _NW_FBox_SelectSkin_Construct,
    /* destruct                    */ _NW_FBox_SelectSkin_Destruct
  },
  { /* NW_FBox_Skin                */
    /* NW_FBox_Skin_Draw_t         */ NULL,
    /* NW_FBox_Skin_GetSize_t      */ NULL
  },
  { /* NW_FBox_SelectSkin */
    /* setActive                   */ NULL,
    /* addOption                   */ NULL,
    /* addOptGrp                   */ NULL,
    /* detailsDialog               */ NULL,
    /* refreshText                 */ NULL,
    /* split                       */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_SelectSkin_Construct (NW_Object_Dynamic_t* dynamicObject,
                              va_list* argp)
{
  NW_FBox_SelectSkin_t* thisObj;
  TBrowserStatusCode status;

  /* for convenience */
  thisObj = NW_FBox_SelectSkinOf (dynamicObject);

  /* invoke our superclass constructor */
  status = _NW_FBox_Skin_Construct (dynamicObject, argp);
  if (status != KBrsrSuccess) {
    return status;
  }

  /* 
  ** create the NW_Object_Shared that allows us to treat history-entry as a 
  ** reference counted obj.  The weird thing about this is that thisObj points
  ** to thisObj->sharingObject which in turn points to thisObj.  This is on 
  ** purpose...  When thisObj->sharingObject is released for the last time it
  ** WILL delete "thisObj".
  */
  thisObj->sharingObject = NW_Object_Shared_New (NW_Object_DynamicOf (thisObj));

  /* successful completion */
  return KBrsrSuccess;
}

/* --------------------------------------------------------------------------*/
void
_NW_FBox_SelectSkin_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  /* parameter assertions */
  NW_ASSERT (dynamicObject != NULL);
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_FBox_SelectSkin_Class));
	NW_REQUIRED_PARAM(dynamicObject);
}

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_SelectSkin_GetMCharExtent(NW_FBox_SelectSkin_t* skin,
                                 NW_GDI_Dimension2D_t *charExtent)
  {
  static const NW_Ucs2 mWidth[] = {'M', '\0'};
  NW_LMgr_RootBox_t* rootBox;
  CGDIDeviceContext* deviceContext;
  NW_Text_UCS2_t* text;
  TBrowserStatusCode status;
  NW_FBox_SelectBox_t* box;
  CGDIFont* font;

  NW_ASSERT(NW_Object_IsInstanceOf(skin, &NW_FBox_SelectSkin_Class));
  box = NW_FBox_SelectSkin_GetSelectBox(skin);
  NW_ASSERT(NW_Object_IsInstanceOf(box, &NW_FBox_SelectBox_Class));

  text = NW_Text_UCS2_New ((NW_Ucs2*) mWidth, 1, 0);
  /* we're going to need the deviceContext to do text extent calculations */
  rootBox = NW_LMgr_Box_GetRootBox (box);
  NW_ASSERT (rootBox != NULL);
  deviceContext = NW_LMgr_RootBox_GetDeviceContext (rootBox);
  NW_ASSERT (deviceContext != NULL);

  font = NW_LMgr_Box_GetFont (NW_LMgr_BoxOf(box));
  NW_ASSERT(font != NULL);

  /* calculate the extents */
  status = deviceContext->GetTextExtent ( NW_TextOf(text), 
                                          font,
                                          0,
                                          NW_GDI_FlowDirection_LeftRight,
                                          charExtent);
  if (status == KBrsrOutOfMemory) 
    {
    return status;
    }

  NW_Object_Delete(text);
  return KBrsrSuccess;
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_FBox_SelectSkin_Reserve (NW_FBox_SelectSkin_t* thisObj)
{
  return NW_Object_Shared_Reserve (thisObj->sharingObject);
}

/* ------------------------------------------------------------------------- */
NW_Bool
NW_FBox_SelectSkin_Release (NW_FBox_SelectSkin_t* thisObj)
{
  return NW_Object_Shared_Release (thisObj->sharingObject);
}

