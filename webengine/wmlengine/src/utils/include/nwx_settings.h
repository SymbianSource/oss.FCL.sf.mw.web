/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NWX_SETTINGS_H
#define NWX_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/
#include <e32def.h>
#include "nwx_defs.h"

/*
** Global Function Declarations
*/

/* Basic browser settings */
NW_Ucs2 *NW_Settings_GetGateway(void); //Deprecated
NW_Bool NW_Settings_SetGateway(const NW_Ucs2 *gateway); //Deprecated

NW_Bool NW_Settings_GetStartFromHomepage(void); //Deprecated

NW_Uint8 NW_Settings_GetFontSizeLevel(void);
NW_Bool NW_Settings_SetFontSizeLevel(NW_Uint8 fontSizeLevel);

NW_Uint16 NW_Settings_GetEncoding(void);
NW_Bool NW_Settings_SetEncoding(NW_Uint16 encoding);

NW_Bool NW_Settings_GetCookiesEnabled(void);
NW_Bool NW_Settings_SetCookiesEnabled(const NW_Bool enabled);

NW_Bool NW_Settings_GetImagesEnabled(void);
NW_Bool NW_Settings_SetImagesEnabled(const NW_Bool enabled);

NW_Bool NW_Settings_GetVerticalLayoutEnabled(void);
NW_Bool NW_Settings_SetVerticalLayoutEnabled(const NW_Bool enabled);

// This method only changes the browser's cached value for whether or
// not vertical layout is enabled. DO NOT USE if you want to be setting
// the value in the ini file. Instead use, NW_Settings_SetVerticalLayoutEnabled
NW_Bool NW_Settings_SetInternalVerticalLayoutEnabled(const NW_Bool enabled);

NW_Bool NW_Settings_GetCSSFetchEnabled(void);
NW_Bool NW_Settings_SetCSSFetchEnabled(const NW_Bool enabled);

NW_Bool NW_Settings_SetWmlTextEnabled(const NW_Bool enabled); //Deprecated

NW_Bool NW_Settings_GetEcmaScriptEnabled(void);
void NW_Settings_SetEcmaScriptEnabled(const NW_Bool enabled);

NW_Bool NW_Settings_GetTextWrapEnabled(void);
NW_Bool NW_Settings_SetTextWrapEnabled(const NW_Bool enabled);

NW_Bool NW_Settings_Get2WayScrollEnabled(void);
NW_Bool NW_Settings_Set2WayScrollEnabled(const NW_Bool enabled);

NW_Bool NW_Settings_SetWspConnType(const NW_Uint32 connType); //Deprecated

NW_Int32 NW_Settings_GetGMTOffset(void); //Deprecated
NW_Bool NW_Settings_SetGMTOffset(const NW_Int32 GMTOffset); //Deprecated

NW_Bool NW_Settings_GetAlwaysConfirmDtmfSending(void); //Deprecated
NW_Bool NW_Settings_SetAlwaysConfirmDtmfSending(const NW_Bool always); //Deprecated

NW_Uint32 NW_Settings_GetOriginalEncoding();
void NW_Settings_SetOriginalEncoding(NW_Uint32 encoding);

void NW_Settings_SetDisableSmallScreenLayout(const NW_Bool disabled);
NW_Bool NW_Settings_GetDisableSmallScreenLayout();

NW_Uint16 NW_Settings_GetDefaultCharset(void);
NW_Bool NW_Settings_SetDefaultCharset(NW_Uint16 defaultCharset); //Deprecated

NW_Uint32 NW_Settings_GetBrowsingAllowedWhileMMSOngoing(void); //Deprecated

NW_Bool NW_Settings_GetIsBrowserEmbedded(void);
void NW_Settings_SetIsBrowserEmbedded(NW_Bool isEmbedded);

TUint NW_Settings_GetLocalFeatures(void);

NW_Ucs2* NW_Settings_GetTempFilesPath(void);

NW_Uint8 NW_Settings_GetMMCDrive(void);

NW_Bool NW_Settings_GetIMEINotifyEnabled(void);
void NW_Settings_SetIMEINotifyEnabled(const NW_Bool enabled);

NW_Bool NW_Settings_GetSendReferrerHeader(void);
void NW_Settings_SetSendReferrerHeader(const NW_Bool enabled);

NW_Bool NW_Settings_GetHttpSecurityWarnings(void);
void NW_Settings_SetHttpSecurityWarnings(const NW_Bool enabled);

NW_Uint32 NW_Settings_GetIAPId(void);
void NW_Settings_SetIAPId(NW_Uint32 aIapId);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_SETTINGS_H */
