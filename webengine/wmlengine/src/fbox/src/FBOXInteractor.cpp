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


#include "nw_fbox_interactori.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- */
const
NW_FBox_Interactor_Class_t  NW_FBox_Interactor_Class = {
  { /* NW_Object_Core                    */
    /* super                             */ &NW_Object_Dynamic_Class,
    /* queryInterface                    */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                    */
    /* interfaceList                     */ NULL
  },
  { /* NW_Object_Dynamic                 */
    /* instanceSize                      */ sizeof (NW_FBox_Interactor_t),
    /* construct                         */ _NW_FBox_Interactor_Construct,
    /* destruct                          */ NULL
  },
  { /* NW_FBox_Interactor                */
    /* NW_FBox_Interactor_ProcessEvent_t */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_FBox_Interactor_Construct (NW_Object_Dynamic_t* dynamicObject,
                         va_list* argp)
{
  NW_FBox_Interactor_t* thisObj;

  /* for convenience */
  thisObj = NW_FBox_InteractorOf (dynamicObject);

  /* initialize the member variables */
  thisObj->formBox = va_arg (*argp, NW_LMgr_Box_t*);

  /* successful completion */
  return KBrsrSuccess;
}

