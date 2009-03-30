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


#ifndef NW_FBOX_SKINI_H
#define NW_FBOX_SKINI_H

#include "nw_object_dynamici.h"
#include "nw_fbox_skin.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
NW_Text_t*
NW_FBox_Skin_NonprintableToSpace(NW_Text_t *text);

TBrowserStatusCode 
NW_LMgr_Text_GetDisplayLength(CGDIDeviceContext* deviceContext,
															NW_Text_t* text,
                              CGDIFont* font,
															NW_Text_Length_t totalLength,
															NW_Uint16 width,
															NW_Text_Length_t* displayLength);

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_FBox_Skin_Construct (NW_Object_Dynamic_t* dynamicObject,
                         va_list* argp);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FBOX_SKINI_H */
