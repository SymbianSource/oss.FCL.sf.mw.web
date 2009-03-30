/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class represents the marker image box
*
*/

#include "LMgrMarkerImage.h"
#include "nw_lmgr_animatedimageboxi.h"
/* ------------------------------------------------------------------------- */
const
LMgrMarkerImage_Class_t  LMgrMarkerImage_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_AnimatedImageBox_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ _NW_LMgr_AnimatedImageBox_SecondaryList
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (LMgrMarkerImage_t),
    /* construct                 */ _NW_LMgr_ImgContainerBox_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_ImgContainerBox_Split,
    /* resize                    */ _NW_LMgr_Box_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_ImgContainerBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_MediaBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_AnimatedImageBox_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_MediaBox          */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_ImgContainerBox   */
    /* unused                    */ NW_Object_Unused
  },
  { /* NW_LMgr_AnimatedImageBox  */
    /* unused                    */ NW_Object_Unused
  },
  { /* LMgrMarkerImage           */
    /* unused                    */ NW_Object_Unused
  }
};


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
LMgrMarkerImage_t*
LMgrMarkerImage_New (NW_ADT_Vector_Metric_t numProperties, NW_Image_AbstractImage_t* aVirtualImage, 
                             NW_Text_t* aAltText, NW_Bool aBrokenImage )
{
  return (LMgrMarkerImage_t*)
    NW_Object_New (&LMgrMarkerImage_Class, numProperties, aVirtualImage, aAltText, aBrokenImage );
}
