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


#include "NW_LMgr_IEventListenerI.h"

/* ------------------------------------------------------------------------- */
const NW_LMgr_IEventListener_Class_t NW_LMgr_IEventListener_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Interface_Class,
    /* queryInterface          */ _NW_Object_Interface_QueryInterface
  },
  { /* NW_Object_Interface     */
    /* offset                  */ 0
  },
  { /* NW_LMgr_IEventListener  */
    /* processEvent            */ NULL
  }
};
