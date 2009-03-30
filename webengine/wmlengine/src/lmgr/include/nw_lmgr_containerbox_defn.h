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


#ifndef NW_LMGR_ACTIVECONTAINERBOX_DEFN_H
#define NW_LMGR_ACTIVECONTAINERBOX_DEFN_H



#include "nw_lmgr_containerbox.h"
#include "NW_LMgr_IEventListener.h"
#include "nw_lmgr_eventhandler.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


struct NW_LMgr_ContainerBox_s {
  NW_LMgr_Box_t super;

  /* member variables */
  NW_ADT_DynamicVector_t* children;
};

#ifdef __cplusplus
}; // extern "C" {
#endif /* __cplusplus */

#endif /* NW_LMGR_ACTIVECONTAINERBOX_DEFN_H*/
