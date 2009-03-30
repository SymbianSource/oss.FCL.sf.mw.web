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


#include "nw_fbox_epoc32optionskini.h"

#include "nw_fbox_optionbox.h"
#include "nw_fbox_passwordbox.h"
#include "nw_lmgr_rootbox.h"
#include "nwx_string.h"
#include "nwx_generic_dlg.h"


/* ------------------------------------------------------------------------- *
    private methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_FBox_Epoc32OptionSkin_Class_t  NW_FBox_Epoc32OptionSkin_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_FBox_OptionBoxSkin_Class,
    /* queryInterface        */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base        */
    /* interfaceList         */ NULL
  },
  { /* NW_Object_Dynamic     */
    /* instanceSize          */ sizeof (NW_FBox_Epoc32OptionSkin_t),
    /* construct             */ _NW_FBox_Skin_Construct,
    /* destruct              */ NULL
  },
  { /* NW_FBox_Skin          */
    /* draw                  */ NULL,
    /* getSize               */ NULL
  },
  { /* NW_FBox_OptionSkin    */
    /* unused                */ NW_Object_Unused
  },
  { /* NW_FBox_ISAOptionSkin */
    /* unused                */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_FBox_OptionBoxSkin_t*
NW_FBox_OptionBoxSkin_New (NW_FBox_FormBox_t* formBox)
{
  return (NW_FBox_OptionBoxSkin_t*)
    NW_Object_New (&NW_FBox_Epoc32OptionSkin_Class, formBox);
}
