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


#include "NW_Unicode_%NAME%I.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_%NAME%_Class_t NW_Unicode_%NAME%_Class = {
  { /* NW_Object_Core     %SPACE%*/
    /* superclass         %SPACE%*/ &NW_Unicode_Category_Class,
    /* queryInterface     %SPACE%*/ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category%SPACE%*/
    /* tableSize          %SPACE%*/ 0,
    /* table              %SPACE%*/ NULL
  },
  { /* NW_Unicode_%NAME% */
    /* unused             %SPACE%*/ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_%NAME%_t NW_Unicode_%NAME% = {
  { { &NW_Unicode_%NAME%_Class } }
};
