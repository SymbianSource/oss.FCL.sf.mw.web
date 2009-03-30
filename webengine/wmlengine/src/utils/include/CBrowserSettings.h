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


#ifndef __UTIL_SETTINGS_H
#define __UTIL_SETTINGS_H

#include <bldvariant.hrh>
#include <coemain.h>
#include "nwx_defs.h"
#include "nwx_settings.h"

// FYI: All key literals come from \epoc32\include\oem\SharedDataKeysVariant.h
// Please do not define key literal strings here.

_LIT( KValueOn, "True" );
_LIT( KValueOff, "False" );

class CRepository;

enum TBrowserSetting
  {
  EFontSizeLevel              = 0x1,
  EEncoding                   = 0x2,
  ECookiesEnabled             = 0x4,
  EImagesEnabled              = 0x8,
  EEcmaScriptEnabled          = 0x10,
  ETextWrapEnabled            = 0x20,
  EOriginalEncoding           = 0x40,
  EVerticalLayoutEnabled      = 0x80,
  ECSSFetchEnabled            = 0x100,
  EDisableSmallScreenLayout   = 0x200,
  EIsEmbedded                 = 0x400,
  EIMEINotifyEnabled          = 0x800,
  ESendReferrerHeader         = 0x1000,
  EHttpSecurityWarnings       = 0x2000
  };

/*****************************************************************

  Description: abstract observer calss

******************************************************************/
/*class MBrowserSettingsObserver
  {
  //
  public:

    virtual void BrowserSettingChanged( enum TBrowserSetting aBrowserSetting ) = 0;
  };
*/
/*****************************************************************

  Description: This class keeps track of the settings.

******************************************************************/

NONSHARABLE_CLASS(CBrowserSettings) : public CBase
    {
    public:

        //Construction

        static CBrowserSettings* Instance();

        ~CBrowserSettings();

        // observer methods
        //void SetObserver( MBrowserSettingsObserver* aBrowserSettingsObserver,
         // TUint aObservedSettings );

        void RemoveObserver( void );

        // Access methods
        NW_Bool GetStartFromHomepage(void);

        NW_Uint8 GetFontSizeLevel(void);
        void SetFontSizeLevel(NW_Uint8 fontSizeLevel);

        NW_Uint16 GetEncoding(void);
        void SetEncoding(NW_Uint16 encoding);

        NW_Bool GetCookiesEnabled(void);
        void SetCookiesEnabled(const NW_Bool enabled);

        NW_Bool GetImagesEnabled(void);
        void SetImagesEnabled(const NW_Bool enabled);

        NW_Bool GetEcmaScriptEnabled(void);
        void SetEcmaScriptEnabled(const NW_Bool enabled);

        NW_Bool GetTextWrapEnabled(void);
        void SetTextWrapEnabled(const NW_Bool enabled);

        NW_Bool GetAlwaysConfirmDtmfSending(void);
        void SetAlwaysConfirmDtmfSending(const NW_Bool enabled);

        NW_Uint32 GetOriginalEncoding(void);
        void SetOriginalEncoding(NW_Uint32 encoding);

        NW_Bool GetVerticalLayoutEnabled(void);
        void SetVerticalLayoutEnabled(const NW_Bool enabled);

        void SetInternalVerticalLayoutEnabled(const NW_Bool enabled);

        void SetCSSFetchEnabled(const NW_Bool disabled);
        NW_Bool GetCSSFetchEnabled(void);

        NW_Uint16 GetDefaultCharset(void);
        void SetDefaultCharset(NW_Uint16 defaultCharset);

         void SetDisableSmallScreenLayout(const NW_Bool disabled);
        NW_Bool GetDisableSmallScreenLayout();

        NW_Bool GetIsBrowserEmbedded(void);
        void SetIsBrowserEmbedded(const NW_Bool isEmbedded);

        TUint GetLocalFeatures(void);

        NW_Ucs2* GetTempFilesPath(void);

        NW_Uint8 GetMMCDrive(void);

        NW_Uint8 GetIMEINotifyEnabled(void);
        void SetIMEINotifyEnabled(NW_Uint8 enabled);

        NW_Uint8 GetSendReferrerHeader(void);
        void SetSendReferrerHeader(NW_Uint8 enabled);

        NW_Bool GetHttpSecurityWarnings(void);
        void SetHttpSecurityWarnings(NW_Bool enabled);

        NW_Uint32 GetIAPId(void);
        void SetIAPId(NW_Uint32 aIapId);

	private:

        CBrowserSettings();

        // observer method
        void PublishSettingChange( enum TBrowserSetting aBrowserSetting );

        // Utility methods
        TInt GetIntValue(const TUint32 aKey);
        NW_Bool GetBooleanValue(const TUint32 aKey);
        NW_Ucs2* GetStringValue( const TUint32 aKey );

    private:

        NW_Bool   iStartFromHomepage;
        NW_Uint8  iFontSizeLevel;
        NW_Uint16 iEncoding;
        NW_Bool   iCookiesEnabled;
        NW_Bool   iImagesEnabled;
        NW_Uint8  iEcmaScriptEnabled;
        NW_Uint8  iTextWrapEnabled;
        NW_Uint32 iOriginalEncoding;
        NW_Uint8  iVerticalLayoutEnabled;
        NW_Bool   iCSSFetchEnabled;
        NW_Bool   iDisableSmallScreenLayout;
        NW_Uint16 iDefaultCharset;

        CRepository* iRepository;
        NW_Bool   iIsEmbedded;
        TUint     iFeatureBitmask;
        NW_Ucs2*  iTempFilesPath;
        NW_Uint8  iMMCDrive;
        NW_Uint8  iIMEINotifyEnabled;
        NW_Uint8  iSendReferrerHeader;
        NW_Bool   iHttpSecurityWarnings;
        NW_Bool   iSecUI;
		NW_Uint32 iIapId;

        //MBrowserSettingsObserver* iBrowserSettingsObserver;
        TUint      iObservedSettings;
    };

#endif /* __UTIL_SETTINGS_H */
