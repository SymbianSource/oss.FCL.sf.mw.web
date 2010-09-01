/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Box class associated with the handling image maps
*
*/


#ifndef NW_IMAGEMAPBOXI_H
#define NW_IMAGEMAPBOXI_H

//  INCLUDES
#include "nw_lmgr_animatedimageboxi.h"
#include "nw_lmgr_imagemapbox.h"
#include "NW_LMgr_IEventListenerI.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

// ------------------------------------------------------------------------- 
//   virtual methods implementation declarations
// -------------------------------------------------------------------------
NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ImageMapBox_Construct (NW_Object_Dynamic_t* dynamicObject,
                                va_list* argp);

NW_LMGR_EXPORT
void
_NW_LMgr_ImageMapBox_Destruct (NW_Object_Dynamic_t* dynamicObject);


NW_LMGR_EXPORT
TBrowserStatusCode
_NW_LMgr_ImageMapBox_GetMinimumContentSize( NW_LMgr_Box_t *box,
                                            NW_GDI_Dimension2D_t *size );

NW_LMGR_EXPORT
NW_Uint8
_NW_LMgr_ImageMapBox_IEventListener_ProcessEvent (NW_LMgr_IEventListener_t* eventListener,
                                                  NW_Evt_Event_t* event);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif //NW_IMAGEMAPBOXI_H
