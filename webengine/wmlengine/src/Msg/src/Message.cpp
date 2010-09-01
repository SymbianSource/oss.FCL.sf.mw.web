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


#include "nw_msg_messagei.h"
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Msg_Message_Class_t NW_Msg_Message_Class = {
  { /* NW_Object_Core     */
    /* super              */ &NW_Object_Dynamic_Class,
    /* queryInterface     */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base     */
    /* interfaceList      */ NULL
  },
  { /* NW_Object_Dynamic  */
    /* instanceSize       */ sizeof (NW_Msg_Message_t),
    /* construct          */ _NW_Msg_Message_Construct,
    /* destruct           */ _NW_Msg_Message_Destruct
  },
  { /* NW_Msg_Message */
    /* unused             */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   public/protected methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Msg_Message_Construct (NW_Object_Dynamic_t* dynamicObject,
                           va_list* argList)
{
  NW_Msg_Message_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject,
                                     &NW_Msg_Message_Class));
  NW_ASSERT (argList != NULL);

  /* for convenience */
  thisObj = NW_Msg_MessageOf (dynamicObject);

  /* initialize the member variables */
  thisObj->msgId = (NW_Msg_Type_t) va_arg (*argList, NW_Uint32);
  thisObj->data = (NW_Object_t*) va_arg (*argList, NW_Object_t*);
  thisObj->ownsData = (NW_Bool) va_arg (*argList, NW_Uint32);
  thisObj->iPriority = ((CActive::TPriority)( CActive::EPriorityLow + 2 ));
  return KBrsrSuccess;
}

void
_NW_Msg_Message_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_Msg_Message_t* thisObj;

  /* cast the 'object' for convenience */
  thisObj = NW_Msg_MessageOf (dynamicObject);

  if (thisObj->ownsData) {
    NW_Object_Delete (thisObj->data);
    thisObj->data = NULL;
  }
}

/* ------------------------------------------------------------------------- */
NW_Msg_Message_t*
NW_Msg_Message_New (NW_Msg_Type_t messageId, NW_Object_t* data, NW_Bool ownsData)
{
  return (NW_Msg_Message_t*) NW_Object_New (&NW_Msg_Message_Class, (NW_Uint32)messageId, 
      data, (NW_Uint32)ownsData);
}
