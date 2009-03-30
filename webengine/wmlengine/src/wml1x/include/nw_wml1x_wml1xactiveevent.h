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


#ifndef NW_WML1XACTIVEEVENT_H
#define NW_WML1XACTIVEEVENT_H

#include "nw_hed_appservices.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

TBrowserStatusCode  NW_Wml1x_HandleDo (void          *ctx, 
                                NW_Uint16     elId,
                                NW_LMgr_Box_t *box,
                                NW_LMgr_Box_t **deleteBox);

TBrowserStatusCode  NW_Wml1x_HandleSelect (void          *ctx, 
                                    NW_Uint16     elId,
                                    NW_LMgr_Box_t *box,
                                    NW_LMgr_Box_t **deleteBox,
                                    NW_Bool       *currentSelectMultipleState,
                                    NW_Int32      *currentSelectTabIndexVal);

TBrowserStatusCode  NW_Wml1x_HandleOption (void          *ctx, 
                                    NW_Uint16     elId,
                                    NW_LMgr_Box_t *box,
                                    NW_Bool       currentSelectMultipleState,
                                    NW_Int32      currentSelectTabIndexVal);

TBrowserStatusCode  NW_Wml1x_HandleOptgroup (void          *ctx, 
                                      NW_Uint16     elId,
                                      NW_LMgr_Box_t *box);

TBrowserStatusCode NW_Wml1x_FixupAnchorText (NW_Wml_t                *wmlInterpreter, 
                                      NW_Uint16               elId,
                                      NW_LMgr_ContainerBox_t  *containerBox,
                                      NW_HED_AppServices_t*   appServices); /* for access to localization function */

TBrowserStatusCode NW_Wml1x_GetURL(NW_Wml_t                *wmlInterpreter, 
                            NW_Uint16               elId,
                            NW_Ucs2                 **retURL,
                            NW_Bool                 *isPrev);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1XACTIVEEVENT_H */
