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


#ifndef NW_WML1XAPI_H
#define NW_WML1XAPI_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
const NW_WmlApi_t* NW_Api_GetWml1xCB();

TBrowserStatusCode 
NW_UI_LoadImage (NW_Wml1x_ContentHandler_t *thisObj,                                    
                 NW_Uint16 elId, NW_Bool showImage);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1XAPI_H */
