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


#include "nw_lmgr_formatcontexti.h"
#include "nw_lmgr_linebox.h"
#include "nw_lmgr_box.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_LMgr_FormatContext_Class_t NW_LMgr_FormatContext_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_LMgr_FormatContext_t),
    /* construct               */ _NW_LMgr_FormatContext_Construct,
    /* destruct                */ _NW_LMgr_FormatContext_Destruct
  },
  { /* NW_LMgr_FormatContext   */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_LMgr_FormatContext_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list* argp)
{
  NW_TRY (status) {
    NW_LMgr_FormatContext_t* thisObj;

    /* avoid 'unreferenced formal parameter' warnings */
    (void) argp;

    /* for convenience */
    thisObj = NW_LMgr_FormatContextOf (dynamicObject);

    /* Initialize the member variables */
    thisObj->currentLine = NW_LMgr_LineBox_New();
    NW_THROW_OOM_ON_NULL (thisObj->currentLine, status);

  } NW_CATCH (status) {
  } NW_FINALLY {
    return status;
  } NW_END_TRY
}
  
/* ------------------------------------------------------------------------- */
void
_NW_LMgr_FormatContext_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_LMgr_FormatContext_t* thisObj;

  /* for convenience */
  thisObj = NW_LMgr_FormatContextOf (dynamicObject);

  /* Destroy the line box */
  NW_Object_Delete (thisObj->currentLine);
}

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */

NW_LMgr_FormatContext_t*
NW_LMgr_FormatContext_New ()
{
  return (NW_LMgr_FormatContext_t*)
    NW_Object_New (&NW_LMgr_FormatContext_Class);
}

