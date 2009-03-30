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


#include "nw_markup_modulei.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Markup_Module_Class_t NW_Markup_Module_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Base_Class,
    /* queryInterface            */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Markup_Module	         */
    /* getElementHandler         */ NULL
  }
};
