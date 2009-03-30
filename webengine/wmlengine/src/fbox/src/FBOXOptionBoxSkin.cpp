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


#include "nw_fbox_optionboxskini.h"

/* ------------------------------------------------------------------------- */
const
NW_FBox_OptionBoxSkin_Class_t  NW_FBox_OptionBoxSkin_Class = {
  { /* NW_Object_Core              */
    /* super                       */ &NW_FBox_Skin_Class,
    /* queryInterface              */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base              */
    /* interfaceList               */ NULL
  },
  { /* NW_Object_Dynamic           */
    /* instanceSize                */ sizeof (NW_FBox_OptionBoxSkin_t),
    /* construct                   */ _NW_FBox_Skin_Construct,
    /* destruct                    */ NULL
  },
  { /* NW_FBox_Skin                */
    /* NW_FBox_Skin_Draw_t         */ NULL,
    /* NW_FBox_Skin_GetSize_t      */ NULL
  },
  { /* NW_FBox_OptionBoxSkin */
    /* unused                      */ NW_Object_Unused
  }
};
