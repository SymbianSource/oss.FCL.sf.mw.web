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


#include "nw_lmgr_propertylisti.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_LMgr_PropertyList_Class_t NW_LMgr_PropertyList_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_Object_Dynamic_Class,
    /* queryInterface       */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base       */
    /* interfaceList        */ NULL
  },
  { /* NW_Object_Dynamic    */
    /* instanceSize         */ sizeof (NW_LMgr_PropertyList_t),
    /* construct            */ NULL,
    /* destruct             */ NULL
  },
  { /* NW_LMgr_PropertyList */
    /* get                  */ NULL,
    /* set                  */ NULL,
    /* set                  */ NULL,
    /* copy                 */ NULL,
    /* clear                */ NULL
  }
};
