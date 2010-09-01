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
* Description:  Provides interfaces to user agent settings.
*
*/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include <bldvariant.hrh>
#include <e32def.h>  // resolves NULL redefinition warning

#include "CBrowserSettings.h"
#include "nwx_settings.h"
#include "nwx_http_defs.h"

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Macros
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*****************************************************************
**  Deprecated - DO NOT USE
**  Name:  NW_Settings_GetGateway
**  Description:  Get the gateway configuration from the settings
**  Parameters:   None
**  Return Value: pointer to the allocated string or NULL.
**                The caller has to free the memory.
******************************************************************/
NW_Ucs2 *NW_Settings_GetGateway(void)
{
  return NULL;
}

/*****************************************************************
**  Deprecated - DO NOT USE 
**  Name:  NW_Settings_SetGateway
**  Description:  Set the gateway configuration for the settings
**  Parameters:   *gateway - a pointer to the gateway settings
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
NW_Bool NW_Settings_SetGateway(const NW_Ucs2 *gateway)
{
  NW_REQUIRED_PARAM(gateway);
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_GetHomepage
**  Description:  Gets the homepage from the settings
**  Parameters:   None
**  Return Value: pointer to the allocated string or NULL.
**                The caller has to free the memory.
******************************************************************/
NW_Bool NW_Settings_GetStartFromHomepage(void)
{
  return CBrowserSettings::Instance()->GetStartFromHomepage();
}

/*****************************************************************
**  Name:  NW_Settings_GetEcmaScriptEnabled
**  Description:  Gets the EcmaScriptEnabled flag in the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetEcmaScriptEnabled(void)
{
  return CBrowserSettings::Instance()->GetEcmaScriptEnabled();	
}

/*****************************************************************
**  Name:  NW_Settings_SetEcmaScriptEnabled
**  Description:  Sets the EcmaScriptEnabled flag in the settings
**  Parameters:   NW_Bool isEcmaScriptEnabled
**  Return Value: void
******************************************************************/
void NW_Settings_SetEcmaScriptEnabled(NW_Bool isEnabled)
{
  CBrowserSettings::Instance()->SetEcmaScriptEnabled(isEnabled);	
}

/*****************************************************************
**  Name:  NW_Settings_GetFontSizeLevel
**  Description:  Gets the fonts size level from the settings
**  Parameters:   None
**  Return Value:  the stored value
******************************************************************/
NW_Uint8 NW_Settings_GetFontSizeLevel(void)
{
  return CBrowserSettings::Instance()->GetFontSizeLevel();		
}

/*****************************************************************
**  Name:  NW_Settings_SetFontSizeLevel
**  Description:  Sets the fonts size level to the settings
**  Parameters:   fontSizeLevel - new value
**  Return Value:  NW_Bool --  NW_TRUE is success
******************************************************************/
NW_Bool NW_Settings_SetFontSizeLevel(NW_Uint8 fontSizeLevel)
{
  CBrowserSettings::Instance()->SetFontSizeLevel(fontSizeLevel);		
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_Encoding
**  Description:  Gets the encoding from the settings
**  Parameters:   None
**  Return Value:  the stored value
******************************************************************/
NW_Uint16 NW_Settings_GetEncoding(void)
{
  NW_Uint16 customerEncoding;
  customerEncoding = CBrowserSettings::Instance()->GetEncoding();
  if(customerEncoding > EDummyLast)
    return EISO8859_1;
  else
    return customerEncoding;
}

/*****************************************************************
**  Name:  NW_Settings_SetEncoding
**  Description:  Sets the encoding to the settings
**  Parameters:   fontSizeLevel - new value
**  Return Value:  NW_Bool --  NW_TRUE is success
******************************************************************/
NW_Bool NW_Settings_SetEncoding(NW_Uint16 encoding)
{
  CBrowserSettings::Instance()->SetEncoding(encoding);
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_GetCookieEnabled
**  Description:  Gets the Http Cookies enabled flag from the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetCookiesEnabled(void)
{
  return CBrowserSettings::Instance()->GetCookiesEnabled();
}

/*****************************************************************
**  Name:  NW_Settings_SetCookieEnabled
**  Description:  Sets the Http Cookies enabled flag in the settings
**  Parameters:   enabled - NW_TRUE if Http Cookies enabled
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
NW_Bool NW_Settings_SetCookiesEnabled(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetCookiesEnabled(enabled);
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_GetImagesEnabled
**  Description:  Gets the Image-Loading enabled flag from the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetImagesEnabled(void)
{
  return CBrowserSettings::Instance()->GetImagesEnabled();  
}

/*****************************************************************
**  Name:  NW_Settings_SetImagesEnabled
**  Description:  Sets the Image-Loading enabled flag in the settings
**  Parameters:   enabled - NW_TRUE if Http Cookies enabled
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
NW_Bool NW_Settings_SetImagesEnabled(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetImagesEnabled(enabled);  
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_GetTextWrapEnabled
**  Description:  Gets the Text-Wrapping enabled flag from the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetTextWrapEnabled(void)
{
  return CBrowserSettings::Instance()->GetTextWrapEnabled(); 	
}

/*****************************************************************
**  Name:  NW_Settings_SetTextWrapEnabled
**  Description:  Sets the Text-Wrapping enabled flag in the settings
**  Parameters:   enabled - NW_TRUE if Http Cookies enabled
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
NW_Bool NW_Settings_SetTextWrapEnabled(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetTextWrapEnabled(enabled); 	
  return NW_TRUE;
}

/*****************************************************************
**  Deprecated - DO NOT USE
**  Name:  NW_Settings_SetWspConnType
**  Description:  Sets the Wsp Connection TYpe in the settings
**  Parameters:   connType - type of the connection :
**  0 - connection oriented
**  1 - connectionless
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
NW_Bool NW_Settings_SetWspConnType(const NW_Uint32 connType)
{
  NW_REQUIRED_PARAM(connType);
  return NW_TRUE;
}

/*****************************************************************
**  Deprecated - DO NOT USE
**  Name:  NW_Settings_GetGMTOffset
**  Description:  Determines the offset between GMT time and the
**                operating system time from the settings
**  Parameters:   None
**  Return Value: the offset needed to get GMT time from os time
******************************************************************/
NW_Int32 NW_Settings_GetGMTOffset(void)
{
  return 1;
}

/*****************************************************************
**  Deprecated - DO NOT USE
**  Name:  NW_Settings_SetGMTOffset
**  Description:  Sets the offset between GMT time and the
**                operating system time
**  Parameters:   GMTOffset - the offset needed to get GMT time from os time
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
NW_Bool NW_Settings_SetGMTOffset(const NW_Int32 GMTOffset)
{
  NW_REQUIRED_PARAM(GMTOffset); 
  return NW_TRUE;
}

/*****************************************************************
**  Deprecated - DO NOT USE
**  Name:  NW_Settings_GetAlwaysConfirmDtmfSending
**  Description:  Determines whether sending Dtmf should be confirmed  
**                always or once
**  Parameters:   None
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
NW_Bool NW_Settings_GetAlwaysConfirmDtmfSending(void)
{
  return NW_TRUE;
}

/*****************************************************************
**  Deprecated - DO NOT USE
**  Name:  NW_Settings_SetAlwaysConfirmDtmfSending
**  Description:  Sets whether sending Dtmf should be confirmed always 
**                or once
**  Parameters:   always - NW_TRUE for always, NW_FALSE for once
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
NW_Bool NW_Settings_SetAlwaysConfirmDtmfSending(const NW_Bool always)
{
  NW_REQUIRED_PARAM(always);
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_GetVerticalLayoutEnabled
**  Description:  Gets the VerticalLayoutEnabled flag from the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetVerticalLayoutEnabled(void)
{
  return CBrowserSettings::Instance()->GetVerticalLayoutEnabled(); 	
}

/*****************************************************************
**  Name:  NW_Settings_SetVerticalLayoutEnabled
**  Description:  Sets the VerticalLayoutEnabled flag in the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_SetVerticalLayoutEnabled(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetVerticalLayoutEnabled(enabled); 	
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_SetInternalVerticalLayoutEnabled
**  Description:  Sets the VerticalLayoutEnabled flag in the settings.
**  This method only changes the browser's cached value for whether or
**  not vertical layout is enabled. DO NOT USE if you want to be setting
**  the value in the ini file. Instead use
**  NW_Settings_SetVerticalLayoutEnabled
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_SetInternalVerticalLayoutEnabled(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetInternalVerticalLayoutEnabled(enabled); 	
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_GetOriginalEncoding
**  Description:  Sets the KWmlBrowserDefaultLatinEncoding flag in 
**    the settings
**  Parameters:   None
**  Return Value: NW_Uint32
******************************************************************/
NW_Uint32 NW_Settings_GetOriginalEncoding()
{
  return CBrowserSettings::Instance()->GetOriginalEncoding();
}

/*****************************************************************
**  Name:  NW_Settings_SetOriginalEncoding
**  Description:  Sets the KWmlBrowserDefaultLatinEncoding flag in 
**    the settings
**  Parameters:   NW_Uint32 encoding
**  Return Value: void
******************************************************************/
void NW_Settings_SetOriginalEncoding(NW_Uint32 encoding)
{
  CBrowserSettings::Instance()->SetOriginalEncoding(encoding);
}

/*****************************************************************
**  Name:  NW_Settings_SetDisableSmallScreenLayout
**  Description:  Sets the SetDisableSmallScreenLayout flag in the 
**    settings
**  Parameters:   NW_Bool --  disabled if NW_TRUE
**  Return Value: void
******************************************************************/
void NW_Settings_SetDisableSmallScreenLayout(const NW_Bool disabled)
{
  CBrowserSettings::Instance()->SetDisableSmallScreenLayout(disabled);
}

/*****************************************************************
**  Name:  NW_Settings_GetDisableSmallScreenLayout
**  Description:  Gets the SetDisableSmallScreenLayout flag in the 
**    settings
**  Parameters:   NW_Bool --  disabled if NW_TRUE
**  Return Value: void
******************************************************************/
NW_Bool NW_Settings_GetDisableSmallScreenLayout()
{
  return CBrowserSettings::Instance()->GetDisableSmallScreenLayout();
}

/*****************************************************************
**  Name:  NW_Settings_GetCSSFetchEnabled
**  Description:  Gets the CSSEnabled flag from the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetCSSFetchEnabled(void)
  {
  return CBrowserSettings::Instance()->GetCSSFetchEnabled(); 	
  }

/*****************************************************************
**  Name:  NW_Settings_SetCSSFetchEnabled
**  Description:  Sets the CSSEnabled flag in the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_SetCSSFetchEnabled(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetCSSFetchEnabled(enabled); 	
  return NW_TRUE;
}

/*****************************************************************
**  Deprecated - DO NOT USE
**  Name:  NW_Settings_GetBrowsingAllowedWhileMMSOngoing
**  Description:  Determines whether browsinf is allowed during MMS  
**  Parameters:   None
**  Return Value: NW_TRUE if browsing is allowed, otherwise NW_FALSE
******************************************************************/
NW_Uint32 NW_Settings_GetBrowsingAllowedWhileMMSOngoing(void)
{
  return 1;
}

/*****************************************************************
**  Name:  NW_Settings_DefaultCharset
**  Description:  Gets the default charset from the settings
**  Parameters:   None
**  Return Value:  the stored value
******************************************************************/
NW_Uint16 NW_Settings_GetDefaultCharset(void)
{
  NW_Uint16 defaultCharset;
  defaultCharset = CBrowserSettings::Instance()->GetDefaultCharset();
  if ((defaultCharset == EAutomatic) || (defaultCharset >= EDummyLast))
  {
    return EISO8859_1;
  }
  else
  {
    return defaultCharset;
  }
}

/*****************************************************************
**  Deprecated - DO NOT USE
**  Name:  NW_Settings_SetDefaultCharset
**  Description:  Sets the default charset to the settings
**  Parameters:   defaultCharset - new value
**  Return Value:  NW_Bool --  NW_TRUE is success
******************************************************************/
NW_Bool NW_Settings_SetDefaultCharset(NW_Uint16 defaultCharset)
{
  NW_REQUIRED_PARAM(defaultCharset);
  return NW_TRUE;
}

/*****************************************************************
**  Name:  NW_Settings_GetIsBrowserEmbedded
**  Description:  Gets the value of the embedded setting
**  Parameters:   None
**  Return Value:  the stored value
******************************************************************/
NW_Bool NW_Settings_GetIsBrowserEmbedded()
{
  return CBrowserSettings::Instance()->GetIsBrowserEmbedded();
}

/*****************************************************************
**  Name:  NW_Settings_SetIsBrowserEmbedded
**  Description:  Sets the value of the embedded setting
**  Parameters:   NW_Bool isEmbedded
**  Return Value:  void
******************************************************************/
void NW_Settings_SetIsBrowserEmbedded(NW_Bool isEmbedded)
{
  CBrowserSettings::Instance()->SetIsBrowserEmbedded(isEmbedded);
}

/*****************************************************************
**  Name:  NW_Settings_GetLocalFeatures
**  Description:  Gets the local features bitmask
**  Parameters:   void
**  Return Value:  TUint - the local features bitmask
******************************************************************/
TUint NW_Settings_GetLocalFeatures(void)
{
  return CBrowserSettings::Instance()->GetLocalFeatures();
}

/*****************************************************************
**  Name:  NW_Settings_GetTempFilesPath
**  Description:  Gets the path for temporary files
**  Parameters:   void
**  Return Value:  NW_Ucs2* - the path of temp files
******************************************************************/
NW_Ucs2* NW_Settings_GetTempFilesPath(void)
{
  return CBrowserSettings::Instance()->GetTempFilesPath();
}

/*****************************************************************
**  Name:  NW_Settings_GetMMCDrive
**  Description:  Gets the drive letter of MMC card
**  Parameters:   void
**  Return Value:  NW_Uint8 - the drive letter of MMC card
******************************************************************/
NW_Uint8 NW_Settings_GetMMCDrive(void)
{
  return CBrowserSettings::Instance()->GetMMCDrive();
}

/*****************************************************************
**  Name:  NW_Settings_GetIMEINotifyEnabled
**  Description:  Gets the IMEI Notify enabled flag from the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetIMEINotifyEnabled(void)
{
  return CBrowserSettings::Instance()->GetIMEINotifyEnabled();  
}

/*****************************************************************
**  Name:  NW_Settings_SetIMEINotifyEnabled
**  Description:  Sets the IMEI Notify enabled flag in the settings
**  Parameters:   enabled - NW_TRUE if IMEI Notify enabled
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
void NW_Settings_SetIMEINotifyEnabled(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetIMEINotifyEnabled(enabled);  
}

/*****************************************************************
**  Name:  NW_Settings_GetSendReferrerHeader
**  Description:  Gets the SendReferrerHeader flag from the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetSendReferrerHeader(void)
{
  return CBrowserSettings::Instance()->GetSendReferrerHeader();  
}

/*****************************************************************
**  Name:  NW_Settings_SetSendReferrerHeader
**  Description:  Sets the SendReferrerHeader flag in the settings
**  Parameters:   enabled - NW_TRUE if SendReferrerHeader enabled
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
void NW_Settings_SetSendReferrerHeader(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetSendReferrerHeader(enabled);  
}

/*****************************************************************
**  Name:  NW_Settings_GetHttpSecurityWarnings
**  Description:  Gets the HttpSecurityWarnings flag from the settings
**  Parameters:   None
**  Return Value: NW_Bool --  enabled if NW_TRUE
******************************************************************/
NW_Bool NW_Settings_GetHttpSecurityWarnings(void)
{
  return CBrowserSettings::Instance()->GetHttpSecurityWarnings();  
}

/*****************************************************************
**  Name:  NW_Settings_SetHttpSecurityWarnings
**  Description:  Sets the HttpSecurityWarnings flag in the settings
**  Parameters:   enabled - NW_TRUE if SendReferrerHeader enabled
**  Return Value: NW_TRUE if value set, otherwise NW_FALSE
******************************************************************/
void NW_Settings_SetHttpSecurityWarnings(const NW_Bool enabled)
{
  CBrowserSettings::Instance()->SetHttpSecurityWarnings(enabled);  
}

/*****************************************************************
**  Name:  NW_Settings_GetIAPId
**  Description:  Gets the Internet AP ID to use from the settings
**  Parameters:   None
**  Return Value: TUint --  Internet AP ID to use 
******************************************************************/
NW_Uint32 NW_Settings_GetIAPId(void)
{
  return CBrowserSettings::Instance()->GetIAPId();  
}

/*****************************************************************
**  Name:  NW_Settings_SetIAPId
**  Description:  Gets the Internet AP ID to use from the settings
**  Parameters:   TUint iapId - Access Point Id
**  Return Value: None
******************************************************************/
void NW_Settings_SetIAPId(NW_Uint32 aIapId)
{
  CBrowserSettings::Instance()->SetIAPId(aIapId);  
}
