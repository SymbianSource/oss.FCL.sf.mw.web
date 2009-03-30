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


#include "nw_fbox_epoc32optgrpskini.h"

#include "nw_fbox_optgrpbox.h"
#include "nw_fbox_passwordbox.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_string.h"
#include "nwx_generic_dlg.h"
#include "BrsrStatusCodes.h"


/* ------------------------------------------------------------------------- *
    private methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_Epoc32OptGrpSkin_Class_t  NW_FBox_Epoc32OptGrpSkin_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_FBox_OptGrpSkin_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_FBox_Epoc32OptGrpSkin_t),
    /* construct             */ _NW_FBox_Skin_Construct,
    /* destruct              */ NULL
  },
  { /* NW_FBox_Skin          */
    /* draw                  */ NULL,
    /* getSize               */ NULL
  },
  { /* NW_FBox_OptGrpSkin    */
    /* addOption             */ _NW_FBox_Epoc32OptGrpSkin_AddOption,
    /* addOptGrp             */ _NW_FBox_Epoc32OptGrpSkin_AddOptGrp
  },
  { /* NW_FBox_Epoc32OptGrpSkin */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32OptGrpSkin_AddOption(NW_FBox_OptGrpSkin_t* optGrpSkin,
                                 NW_FBox_OptionBox_t* optionBox)
{
  NW_REQUIRED_PARAM(optGrpSkin);
  NW_REQUIRED_PARAM(optionBox);
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Epoc32OptGrpSkin_AddOptGrp(NW_FBox_OptGrpSkin_t* optGrpSkin,
                                    NW_FBox_OptGrpBox_t* optGrpBox)
{
  NW_REQUIRED_PARAM(optGrpSkin);
  NW_REQUIRED_PARAM(optGrpBox);
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_OptGrpSkin_t*
NW_FBox_OptGrpSkin_New (NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_OptGrpSkin_t*)
    NW_Object_New (&NW_FBox_Epoc32OptGrpSkin_Class, formBox);
}
