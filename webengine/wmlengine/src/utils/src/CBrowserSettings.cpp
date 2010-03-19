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


#include <bldvariant.hrh>
#include "CBrowserSettings.h"

#include <BrowserUiSDKCRKeys.h>
#include <centralrepository.h>

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <f32file.h>
#include "nwx_string.h"
#include "nwx_ctx.h"
#include "nwx_http_defs.h"
#include "TEncodingMapping.h"

_LIT(KTempFilesPath, "c:\\system\\temp\\\0");

/************************************************************************
Function:constructor
Purpose: Class constructor. This is protected and is created via the
static method Instance. Reason for this is that this class
is a singleton and there is only one about.
Paramters:
Return Values:
**************************************************************************/
CBrowserSettings::CBrowserSettings()
{
    //lint --e{774} --e{666} Significant prototype coercion, Expression with side effects passed to repeated parameter in macro

    TInt err = KErrNone;
    TRAP(err, iRepository = CRepository::NewL(KCRUidBrowser));
    if(err != KErrNone)
    {
    return;
    }

    CRepository* tempRep = NULL;
    TRAP(err, tempRep =  CRepository::NewL(KCRUidBrowserUiLV));
    if(err != KErrNone)
    {
    //delete iRepository and return;
    delete iRepository;
    iRepository = NULL;
    return;
    }
    iFeatureBitmask = 0;
    tempRep->Get(KBrowserEngineFlags, (TInt&)iFeatureBitmask );
    delete tempRep;

    // read values from the ini file
    //iStartFromHomepage = GetBooleanValue( KBrowserStartFromHomepage );
    iFontSizeLevel = NW_UINT8_CAST( GetIntValue( KBrowserNGFontSize ) );
    // translate UID to internal encoding
    NW_Uint32 uid = GetIntValue( KBrowserNGEncoding );
    iEncoding = EAutomatic;
    NW_Uint32 i = 0;
    for (i = 0; supportedCharset[i].uid != 0; i++)
    {
        if (supportedCharset[i].uid == uid)
        {
            iEncoding = supportedCharset[i].internalEncoding;
        }
    }
    //
    iCookiesEnabled = GetBooleanValue ( KBrowserNGCookiesEnabled );
    iImagesEnabled = GetBooleanValue( KBrowserNGImagesEnabled );
    iEcmaScriptEnabled = NW_UINT8_CAST( GetIntValue( KBrowserNGECMAScriptSupport ));
    //we use wmlengine only for wml pages and text wrap was for xhtml small screen mode.
    iTextWrapEnabled = NW_FALSE;
    //we use wmlengine only for wml pages and vertical layout is not supported for WML pages
    iVerticalLayoutEnabled = NW_FALSE;
    iDisableSmallScreenLayout = NW_FALSE;
    iCSSFetchEnabled = GetBooleanValue(KBrowserCSSFetch);
    // translate UID to internal encoding
    uid = GetIntValue( KBrowserDefaultCharset );
    iDefaultCharset = EISO8859_1;
    for (i = 0; supportedCharset[i].uid != 0; i++)
    {
        if (supportedCharset[i].uid == uid)
        {
            iDefaultCharset = supportedCharset[i].internalEncoding;
        }
    }
    //
    iIsEmbedded = NW_FALSE;
    iIMEINotifyEnabled = NW_UINT8_CAST( GetIntValue( KBrowserIMEINotification ));
    iSendReferrerHeader = NW_UINT8_CAST( GetIntValue( KBrowserSendReferrerHeader ));
    //iHttpSecurityWarnings = GetBooleanValue( KWmlBrowserHttpSecurityWarnings );
    iSecUI = GetBooleanValue( KBrowserSecurityUI );

    iIapId = 0;
    iObservedSettings = 0;
  //iBrowserSettingsObserver = NULL;
}

/************************************************************************
Function: Instance
Purpose: Static method to create the only instance of this class if one
does not already exist.
Paramters:
Return Values: pointer to the instance of the WBSettings class
**************************************************************************/
CBrowserSettings*
CBrowserSettings::Instance()
{
  CBrowserSettings* inst = (CBrowserSettings*)NW_Ctx_Get(NW_CTX_SETTINGS, 0);
  if (inst == NULL)
  {
    inst = new CBrowserSettings();
    NW_Ctx_Set(NW_CTX_SETTINGS, 0, inst);
  }
  return inst;
}

CBrowserSettings::~CBrowserSettings()
{
  delete iRepository;
  iRepository = NULL;
}

/*
void
CBrowserSettings::SetObserver( MBrowserSettingsObserver* aBrowserSettingsObserver,
                               TUint aObservedSettings )
  {
  iBrowserSettingsObserver = aBrowserSettingsObserver;
  iObservedSettings = aObservedSettings;
  }

void
CBrowserSettings::RemoveObserver( void )
  {
  iBrowserSettingsObserver = NULL;
  iObservedSettings = 0;
  }
*/
NW_Bool
CBrowserSettings::GetStartFromHomepage(void)
  {
  return iStartFromHomepage;
}

NW_Uint8
CBrowserSettings::GetFontSizeLevel(void)
{
  return iFontSizeLevel;
}

void
CBrowserSettings::SetFontSizeLevel(NW_Uint8 fontSizeLevel)
{
  iFontSizeLevel = fontSizeLevel;
  PublishSettingChange( EFontSizeLevel );
}

NW_Uint16
CBrowserSettings::GetEncoding(void)
    {
    if(iEncoding >= EDummyLast)
        {
        return EISO8859_1;
        }
    else
        {
        return iEncoding;
        }
    }

void
CBrowserSettings::SetEncoding(NW_Uint16 encoding)
{
  iEncoding = encoding;
  PublishSettingChange( EEncoding );
}

NW_Bool
CBrowserSettings::GetCookiesEnabled(void)
{
  return iCookiesEnabled;
}

void
CBrowserSettings::SetCookiesEnabled(const NW_Bool enabled)
{
  iCookiesEnabled = enabled;
  PublishSettingChange( ECookiesEnabled );
}

NW_Bool
CBrowserSettings::GetImagesEnabled(void)
{
  return iImagesEnabled;
}

void
CBrowserSettings::SetImagesEnabled(const NW_Bool enabled)
{
  iImagesEnabled = enabled;
  PublishSettingChange( EImagesEnabled );
}

NW_Bool
CBrowserSettings::GetEcmaScriptEnabled(void)
{
  return (NW_Bool)iEcmaScriptEnabled;
}

void
CBrowserSettings::SetEcmaScriptEnabled(const NW_Bool enabled)
{
  iEcmaScriptEnabled = (NW_Uint8) enabled;
  PublishSettingChange( EEcmaScriptEnabled );
}

NW_Bool
CBrowserSettings::GetTextWrapEnabled(void)
{
  return (NW_Bool)iTextWrapEnabled;
}

void
CBrowserSettings::SetTextWrapEnabled(const NW_Bool /*enabled*/)
{
  iTextWrapEnabled = NW_FALSE;
  PublishSettingChange( ETextWrapEnabled );
}

NW_Uint32
CBrowserSettings::GetOriginalEncoding()
{
  return iOriginalEncoding;
}

void
CBrowserSettings::SetOriginalEncoding(NW_Uint32 encoding)
{
  iOriginalEncoding = encoding;
  PublishSettingChange( EOriginalEncoding );
}

void
CBrowserSettings::SetDisableSmallScreenLayout(const NW_Bool disabled)
{
  iDisableSmallScreenLayout = disabled;
  PublishSettingChange( EDisableSmallScreenLayout );
}

NW_Bool
CBrowserSettings::GetDisableSmallScreenLayout()
{
  return iDisableSmallScreenLayout;
}

NW_Bool
CBrowserSettings::GetCSSFetchEnabled(void)
{
  return iCSSFetchEnabled;
}

void
CBrowserSettings::SetCSSFetchEnabled(const NW_Bool enabled)
{
  iCSSFetchEnabled = enabled;
  PublishSettingChange( ECSSFetchEnabled );
}

NW_Bool
CBrowserSettings::GetVerticalLayoutEnabled(void)
{
  if (iDisableSmallScreenLayout)
  {
    return NW_FALSE;
  }
  return (NW_Bool)iVerticalLayoutEnabled;
}

void
CBrowserSettings::SetVerticalLayoutEnabled(const NW_Bool enabled)
{
  iVerticalLayoutEnabled = (NW_Uint8) enabled;
  PublishSettingChange( EVerticalLayoutEnabled );
}

void
CBrowserSettings::SetInternalVerticalLayoutEnabled(const NW_Bool enabled)
{
  iVerticalLayoutEnabled = (NW_Uint8) enabled;
  PublishSettingChange( EVerticalLayoutEnabled );
}

NW_Uint16 CBrowserSettings::GetDefaultCharset(void)
{
  if ((iDefaultCharset == EAutomatic) || (iDefaultCharset >= EDummyLast))
  {
    return EISO8859_1;
  }
  else
  {
    return iDefaultCharset;
  }
}

NW_Bool
CBrowserSettings::GetIsBrowserEmbedded(void)
{
  PublishSettingChange( EIsEmbedded );
  return iIsEmbedded;
}

void
CBrowserSettings::SetIsBrowserEmbedded(const NW_Bool isEmbedded)
  {
  iIsEmbedded = isEmbedded;
  }

TUint
CBrowserSettings::GetLocalFeatures(void)
{
  return iFeatureBitmask;
}

NW_Ucs2* CBrowserSettings::GetTempFilesPath(void)
{
  NW_Ucs2* ret = NW_Str_Newcpy(KTempFilesPath().Ptr());
  return ret;
}

NW_Uint8 CBrowserSettings::GetMMCDrive(void)
{
  return 'e';
}

NW_Uint8
CBrowserSettings::GetIMEINotifyEnabled(void)
{
  return iIMEINotifyEnabled;
}

void
CBrowserSettings::SetIMEINotifyEnabled(NW_Uint8 enabled)
{
  iIMEINotifyEnabled = enabled;
  PublishSettingChange( EIMEINotifyEnabled );
}

NW_Uint8
CBrowserSettings::GetSendReferrerHeader(void)
{
  return iSendReferrerHeader;
}

void
CBrowserSettings::SetSendReferrerHeader(NW_Uint8 enabled)
{
  iSendReferrerHeader = enabled;
  PublishSettingChange( ESendReferrerHeader );
}

NW_Bool
CBrowserSettings::GetHttpSecurityWarnings(void)
{
  // iSecUI = 1 means "supressing security UI"
  if( iSecUI )
    {
    return NW_FALSE;
    }

  return iHttpSecurityWarnings;
}

void
CBrowserSettings::SetIAPId(NW_Uint32 aIapId)
{
  iIapId = aIapId;
}

NW_Uint32
CBrowserSettings::GetIAPId(void)
{
  return iIapId;
}

void
CBrowserSettings::SetHttpSecurityWarnings(NW_Bool enabled)
{
  iHttpSecurityWarnings = enabled;
  PublishSettingChange( EHttpSecurityWarnings );
}

void
CBrowserSettings::PublishSettingChange( enum TBrowserSetting /*aBrowserSetting*/)
  {
  }

/*****************************************************************************
*    Utility methods
******************************************************************************/
TInt
CBrowserSettings::GetIntValue(const TUint32 aKey)
{
  TInt retVal = 0;

  if ( iRepository )
  {
    iRepository->Get(aKey, retVal);
  }
  return retVal;
}


NW_Bool
CBrowserSettings::GetBooleanValue(const TUint32 aKey)
{
  TFileName value;
  NW_Bool retVal = NW_FALSE;
  TInt err = KErrGeneral;

  if ( iRepository)
  {
    err = iRepository->Get( aKey, value );
  }

  if(err == KErrNone)
  {
    if ( value.Compare( KValueOn ) == 0 )
    {
      retVal = NW_TRUE;
    }
  }

  return retVal;
}


NW_Ucs2* CBrowserSettings::GetStringValue( const TUint32 aKey )
{
  NW_Ucs2* retVal = NULL;
  TFileName value;

  if ( iRepository && ( KErrNone == iRepository->Get( aKey, value ) ) )
    {
      retVal = NW_Str_Substr( value.Ptr(), 0, value.Length() );
    }
return retVal;
}


