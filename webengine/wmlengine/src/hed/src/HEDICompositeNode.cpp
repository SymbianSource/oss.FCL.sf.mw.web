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


#include "nw_hed_icompositenodei.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_HED_ICompositeNode_Class_t NW_HED_ICompositeNode_Class = {
  { /* NW_Object_Core        */
    /* super                 */ &NW_Object_Secondary_Class,
    /* querySecondary        */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary   */
    /* offset                */ 0
  },
  { /* NW_HED_ICompositeNode */
    /* removeChild           */ NULL,
    /* getChildren           */ NULL
  }
};
