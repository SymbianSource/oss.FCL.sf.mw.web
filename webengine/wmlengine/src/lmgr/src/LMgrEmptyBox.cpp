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


#include "nw_lmgr_emptyboxi.h"

#include "nw_adt_resizablevector.h"
#include "nw_lmgr_cssproperties.h"
#include "BrsrStatusCodes.h"

/* -------------------------------------------------------------------------
   The static instance of the Format_Box class object 
 ------------------------------------------------------------------------- */
const
NW_LMgr_EmptyBox_Class_t  NW_LMgr_EmptyBox_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_LMgr_Box_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base            */
    /* interfaceList             */ NULL
  },
  { /* NW_Object_Dynamic         */
    /* instanceSize              */ sizeof (NW_LMgr_EmptyBox_t),
    /* construct                 */ _NW_LMgr_Box_Construct,
    /* destruct                  */ NULL
  },
  { /* NW_LMgr_Box               */
    /* split                     */ _NW_LMgr_Box_Split,
    /* resize                    */ _NW_LMgr_Box_Resize,
    /* postResize                */ _NW_LMgr_Box_PostResize,
    /* getMinimumContentSize     */ _NW_LMgr_EmptyBox_GetMinimumContentSize,
    /* hasFixedContentSize       */ _NW_LMgr_EmptyBox_HasFixedContentSize,
    /* constrain                 */ _NW_LMgr_Box_Constrain,
    /* draw                      */ _NW_LMgr_Box_Draw,
    /* render                    */ _NW_LMgr_Box_Render,
    /* getBaseline               */ _NW_LMgr_Box_GetBaseline,
    /* shift                     */ _NW_LMgr_Box_Shift,
    /* clone                     */ _NW_LMgr_Box_Clone
  },
  { /* NW_LMgr_EmptyBox          */
    /* unused                    */ 0
  }
};

/* ------------------------------------------------------------------------- */
 
TBrowserStatusCode
_NW_LMgr_EmptyBox_GetMinimumContentSize(NW_LMgr_Box_t *box,
                                        NW_GDI_Dimension2D_t *size){
  NW_REQUIRED_PARAM(box);
  NW_ASSERT(size != NULL);

  /* If the properties are not set, the default values are (0,0) */
  (void) NW_LMgr_Box_GetSizeProperties (box, size);
  if (size->width == -1) {
    size->width = 0;
  }
  if (size->height == -1) {
    size->height = 0;
  }

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */

NW_Bool
_NW_LMgr_EmptyBox_HasFixedContentSize(NW_LMgr_Box_t *box){
  NW_REQUIRED_PARAM(box);
  return NW_TRUE;
}

/* -------------------------------------------------------------------------
  convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_LMgr_EmptyBox_t*
NW_LMgr_EmptyBox_New (NW_ADT_Vector_Metric_t numProperties)
{
  return (NW_LMgr_EmptyBox_t*)
    NW_Object_New (&NW_LMgr_EmptyBox_Class, numProperties);
}

