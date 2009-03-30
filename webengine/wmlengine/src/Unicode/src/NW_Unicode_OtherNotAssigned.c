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


#include "NW_Unicode_OtherNotAssignedI.h"

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Unicode_OtherNotAssigned_Class_t NW_Unicode_OtherNotAssigned_Class = {
  { /* NW_Object_Core              */
    /* superclass                  */ &NW_Unicode_Category_Class,
    /* queryInterface              */ _NW_Object_Core_QueryInterface
  },
  { /* NW_Unicode_Category         */
    /* tableSize                   */ 0,
    /* table                       */ NULL
  },
  { /* NW_Unicode_OtherNotAssigned */
    /* unused                      */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
const NW_Unicode_OtherNotAssigned_t NW_Unicode_OtherNotAssigned = {
  { { &NW_Unicode_OtherNotAssigned_Class } }
};
