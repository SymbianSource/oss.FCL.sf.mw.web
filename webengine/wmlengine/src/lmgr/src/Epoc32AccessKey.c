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


#include "nw_lmgr_epoc32accesskeyi.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_LMgr_Epoc32AccessKey_Class_t NW_LMgr_Epoc32AccessKey_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_LMgr_AccessKey_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_LMgr_Epoc32AccessKey_t),
    /* construct               */ _NW_LMgr_AccessKey_Construct,
    /* destruct                */ NULL
  },
  { /* NW_LMgr_AccessKey       */
    /* isValidForOptionsList   */ _NW_LMgr_Epoc32AccessKey_IsValidForOptionsList,
    /* isValidKeyPress         */ _NW_LMgr_Epoc32AccessKey_IsValidKeyPress
  },
  { /* NW_LMgr_Epoc32AccessKey  */
    /* unused                  */ 0
  }
};

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_LMgr_Epoc32AccessKey_IsValidForOptionsList (NW_LMgr_AccessKey_t* accesskey,
                                     NW_Uint32 val)
{
  /* avoid unreferenced formal parameter warnings */
  (void) accesskey;

  return _NW_LMgr_Epoc32AccessKey_IsValidKeyPress(accesskey, val);
}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_LMgr_Epoc32AccessKey_IsValidKeyPress (NW_LMgr_AccessKey_t* accesskey,
                                        NW_Uint32 val)
{
  (void)val;
  (void)accesskey;

  if ((val >= '0') && (val <= '9')) {
    return NW_TRUE;
  }
  return NW_FALSE;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_LMgr_AccessKey_t*
NW_LMgr_AccessKey_New()
{
  return (NW_LMgr_AccessKey_t *)
    NW_Object_New (&NW_LMgr_Epoc32AccessKey_Class);
}


