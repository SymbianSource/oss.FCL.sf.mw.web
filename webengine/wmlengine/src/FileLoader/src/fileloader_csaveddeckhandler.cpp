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
* Description:  Handler for loading file scheme.
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <uri16.h>
#include <apmrec.h>
#include <apgcli.h>

#include "fileloader_csaveddeckhandler.h"
#include "fileloader_cfileloader.h"
#include "urlloader_loaderutils.h"
#include "nwx_http_defs.h"
#include "nwx_settings.h"



// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT8(KUtf8, "utf-8");
_LIT8(KUsAscii, "us-ascii");
_LIT8(KIso10646Ucs2, "iso-10646-ucs-2");
_LIT8(KIso88591, "iso-8859-1");
_LIT8(KBig5, "big5");
_LIT8(KGb2312, "gb2312");
_LIT8(KIso88592, "iso-8859-2");
_LIT8(KIso88593, "iso-8859-3");
_LIT8(KIso88594, "iso-8859-4");
_LIT8(KIso88595, "iso-8859-5");
_LIT8(KIso88596, "iso-8859-6");
_LIT8(KIso88597, "iso-8859-7");
_LIT8(KIso88598, "iso-8859-8");
_LIT8(KIso88599, "iso-8859-9");
_LIT8(KShiftJIS, "shift_JIS");
_LIT8(KWindows1250, "windows-1250");
_LIT8(KWindows1251, "windows-1251");
_LIT8(KWindows1253, "windows-1253");
_LIT8(KWindows1254, "windows-1254");
_LIT8(KWindows1255, "windows-1255");
_LIT8(KWindows1256, "windows-1256");
_LIT8(KWindows1257, "windows-1257");
_LIT8(KTis620,		"Tis-620");
_LIT8(KJisX0201_1997,    "jis_x0201-1997");
_LIT8(KJisX0208_1997,    "jis_x0208-1997");
_LIT8(KIso2022Jp,	"iso-2022-jp");
_LIT8(KEucJp,		"euc-jp");
_LIT8(KWindows874,	"windows-874");
_LIT8(KOI8R, "koi8-r");
_LIT8(KOI8U, "koi8-u");


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSavedDeckPart::Reset
// Zero out all members of the structure CSavedDeckPart.
// -----------------------------------------------------------------------------
//
void CSavedDeckPart::Reset()
{
    iUrl.Set(NULL, 0);
    iData.Set(NULL, 0);
    iContentTypeString.Set(NULL, 0);
    iCharset = 0;
	iOriCharset = 0;
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::CSavedDeckHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSavedDeckHandler::CSavedDeckHandler(TPtrC8& aData) : iData(aData), iDataReader(aData)
{
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSavedDeckHandler* CSavedDeckHandler::NewLC(TPtrC8& aData)
{
    CSavedDeckHandler* self = new( ELeave ) CSavedDeckHandler(aData);

    CleanupStack::PushL( self );

    return self;
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::~CSavedDeckHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CSavedDeckHandler::~CSavedDeckHandler()
{
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::ParseSavedDeck
// Parse the saved deck buffer into the parts and store each part in cache.
// -----------------------------------------------------------------------------
//
TInt CSavedDeckHandler::ParseSavedDeck(TPtrC8& aData, TPtrC& aRequestUrl,
                                       TPtr8& aResponseData,
                                       TPtr& aResponseUrl,
                                       TPtr8& aContentTypeString,
                                       TUint& aCharset, TBool& aSavedDeck)
{
    TRAPD(ret, ParseSavedDeckL(aData, aRequestUrl, aResponseData, aResponseUrl,
                               aContentTypeString, aCharset, aSavedDeck));
    LoaderUtils::LogLoaderEvents(ELogTypeSavedDeck, ret);
    return ret;
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::ParseSavedDeckL
// Parse the saved deck buffer into the parts and store each part in cache.
// -----------------------------------------------------------------------------
//
void CSavedDeckHandler::ParseSavedDeckL(TPtrC8& aData, TPtrC& aRequestUrl,
                                        TPtr8& aResponseData,
                                        TPtr& aResponseUrl,
                                        TPtr8& aContentTypeString,
                                        TUint& aCharset, TBool& aSavedDeck)
{
    // Is this a SavedDeck?
    aSavedDeck = IsSavedDeck(aRequestUrl);
    if (!(aSavedDeck))
    {
        return;
    }

    CSavedDeckHandler* self = CSavedDeckHandler::NewLC(aData);
    TUint i;
    CSavedDeckPart savedDeckPart;

    // Get the number of parts
    TUint count = self->GetUintFromBuffer(self->iDataReader);

    for (i = 0; i < count; i++)
    {
        self->GetNextPartL(savedDeckPart);
        if (i == 0)
        {
            NW_Settings_SetOriginalEncoding(savedDeckPart.iOriCharset);
			// First time through loop prepare and then push aResponseData,
			// aResponseUrl, aContentTypeString onto CleanupStack
            self->PrepareResponseLC(savedDeckPart, aResponseData, aResponseUrl,
                                    aContentTypeString, aCharset);
        }
        else
        {
            self->LoadPartToCacheL( savedDeckPart);
        }
    }

    CleanupStack::Pop(3); // aResponseData, aResponseUrl, aContentTypeString
    CleanupStack::PopAndDestroy(); // self
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::IsSavedDeck
// Check if the loaded file is a saved deck, based on the path and extension.
// -----------------------------------------------------------------------------
//
TBool CSavedDeckHandler::IsSavedDeck(TPtrC& aRequestUrl)
{
    _LIT(KFileExtension, ".saveddeck");
    _LIT(KFilePath, "saveddecks");
    return (aRequestUrl.FindF(KFileExtension()) != KErrNotFound &&
            aRequestUrl.FindF(KFilePath()) != KErrNotFound);
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::GetNextPartL
// Extract the next data part from the buffer.
// -----------------------------------------------------------------------------
//
void CSavedDeckHandler::GetNextPartL(CSavedDeckPart& aSavedDeckPart)
{
    TInt i;

    aSavedDeckPart.Reset();
    // Get the version
    const TUint8* version = (const TUint8*)SAVED_DECK_VERSION;
    TUint versionLen = User::StringLength(version);

    // Is it the correct version?
    if (iDataReader.Find(version, versionLen) != 0)
    {
        User::Leave(KErrCorrupt);
    }
    // The version is contained, so the buffer must be big enough
    iDataReader.Set(iDataReader.Mid(versionLen));

    // Get the URL
    if (iDataReader.Length() < (TInt)sizeof(TUint))
    {
        User::Leave(KErrCorrupt);
    }
    i = GetUintFromBuffer(iDataReader) + 1; // 1 for NULL terminator
    if (iDataReader.Length() < i)
    {
        User::Leave(KErrCorrupt);
    }
    aSavedDeckPart.iUrl.Set(iDataReader.Ptr(), i);
    // Find that NULL terminator is still there
    if (aSavedDeckPart.iUrl[i - 1] != '\0')
    {
        User::Leave(KErrCorrupt);
    }
    iDataReader.Set(iDataReader.Mid(i));

    // Get the content type string
    if (iDataReader.Length() < (TInt)sizeof(TInt)) // length constant type string
    {
        User::Leave(KErrCorrupt);
    }
    i = GetUintFromBuffer(iDataReader) + 1; // 1 for NULL terminator
    if (iDataReader.Length() < i)
    {
        User::Leave(KErrCorrupt);
    }
    aSavedDeckPart.iContentTypeString.Set(iDataReader.Ptr(), i);
    // Find that NULL terminator is still there
    if (aSavedDeckPart.iContentTypeString[i - 1] != '\0')
    {
        User::Leave(KErrCorrupt);
    }
    iDataReader.Set(iDataReader.Mid(i));

    // Get Charset
    aSavedDeckPart.iCharset = GetUintFromBuffer(iDataReader);

    // Get original Charset setting
   aSavedDeckPart.iOriCharset = GetUintFromBuffer(iDataReader);

    // Get the body
    if (iDataReader.Length() < (TInt)sizeof(TInt))
    {
        User::Leave(KErrCorrupt);
    }
    i = GetUintFromBuffer(iDataReader);
    if (iDataReader.Length() < i + 1) // 1 for NULL terminator
    {
        User::Leave(KErrCorrupt);
    }
    aSavedDeckPart.iData.Set(iDataReader.Ptr(), i);
    // Find that NULL terminator is still there
    if (iDataReader[i] != '\0')
    {
        User::Leave(KErrCorrupt);
    }
    iDataReader.Set(iDataReader.Mid(i+ 1));
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::PrepareResponseLC
// Copy the data and URL of the first part, to be returned as the response.
// -----------------------------------------------------------------------------
//
void CSavedDeckHandler::PrepareResponseLC(CSavedDeckPart& aSavedDeckPart,
										  TPtr8& aResponseData,
										  TPtr& aResponseUrl,
										  TPtr8& aContentTypeString,
										  TUint& aCharset)
{
    // Copy data
    TUint8* data = new(ELeave) TUint8 [aSavedDeckPart.iData.Length()];
    CleanupStack::PushL(data);
    aResponseData.Set(data, 0, aSavedDeckPart.iData.Length());
    aResponseData.Copy(aSavedDeckPart.iData);

    // Copy URL
    TUint16* url = new(ELeave) TUint16 [aSavedDeckPart.iUrl.Length()];
    CleanupStack::PushL(url);
    aResponseUrl.Set(url, 0, aSavedDeckPart.iUrl.Length());
    aResponseUrl.Copy(aSavedDeckPart.iUrl);

    // Copy content type string
    // note: string deleted in 'destructor' of reponse class
    TUint8* cts = new(ELeave) TUint8 [aSavedDeckPart.iContentTypeString.Length()];
    CleanupStack::PushL(cts);
    aContentTypeString.Set(cts, 0, aSavedDeckPart.iContentTypeString.Length());
    aContentTypeString.Copy(aSavedDeckPart.iContentTypeString);

    // Charset
    aCharset = aSavedDeckPart.iCharset;
}


// -----------------------------------------------------------------------------
// CSavedDeckHandler::LoadPartToCacheL
// Send a response part to cache.
// -----------------------------------------------------------------------------
//
void CSavedDeckHandler::LoadPartToCacheL(CSavedDeckPart& aSavedDeckPart)
{
    TPtrC8 charsetStr = ConvertCharsetToStr(aSavedDeckPart.iCharset);
    aSavedDeckPart.iUrl.Set(aSavedDeckPart.iUrl.Ptr(), aSavedDeckPart.iUrl.Length() - 1);
    aSavedDeckPart.iContentTypeString.Set(aSavedDeckPart.iContentTypeString.Ptr(), aSavedDeckPart.iContentTypeString.Length() - 1);
}


TPtrC8 CSavedDeckHandler::ConvertCharsetToStr(TUint32 aCharset)
{
    if (aCharset == HTTP_utf_8)
        return KUtf8().Ptr();
    if (aCharset == HTTP_us_ascii)
        return KUsAscii().Ptr();
    if (aCharset ==HTTP_iso_10646_ucs_2)
        return KIso10646Ucs2().Ptr();
    if (aCharset == HTTP_iso_8859_1)
        return KIso88591().Ptr();
    if (aCharset == HTTP_gb2312)
        return KGb2312().Ptr();
    if (aCharset == HTTP_big5)
        return KBig5().Ptr();
    if (aCharset == HTTP_iso_8859_2)
        return KIso88592().Ptr();
    if (aCharset == HTTP_iso_8859_3)
        return KIso88593().Ptr();
    if (aCharset == HTTP_iso_8859_4)
        return KIso88594().Ptr();
    if (aCharset == HTTP_iso_8859_5)
        return KIso88595().Ptr();
    if (aCharset == HTTP_iso_8859_6)
        return KIso88596().Ptr();
    if (aCharset == HTTP_iso_8859_7)
        return KIso88597().Ptr();
    if (aCharset == HTTP_iso_8859_8)
        return KIso88598().Ptr();
    if (aCharset == HTTP_iso_8859_9)
        return KIso88599().Ptr();
    if (aCharset == HTTP_shift_JIS )
        return KShiftJIS().Ptr();
    if (aCharset == HTTP_windows_1250)
        return KWindows1250().Ptr();
    if (aCharset == HTTP_windows_1251)
        return KWindows1251().Ptr();
    if (aCharset == HTTP_windows_1253)
        return KWindows1253().Ptr();
    if (aCharset == HTTP_windows_1254)
        return KWindows1254().Ptr();
    if (aCharset == HTTP_windows_1255)
        return KWindows1255().Ptr();
    if (aCharset == HTTP_windows_1256)
        return KWindows1256().Ptr();
    if (aCharset == HTTP_windows_1257)
        return KWindows1257().Ptr();
    if (aCharset == HTTP_tis_620)
        return KTis620().Ptr();
    if (aCharset == HTTP_jis_x0201_1997)
        return KJisX0201_1997().Ptr();
    if (aCharset == HTTP_jis_x0208_1997)
        return KJisX0208_1997().Ptr();
    if (aCharset == HTTP_iso_2022_jp)
        return KIso2022Jp().Ptr();
    if (aCharset == HTTP_euc_jp)
        return KEucJp().Ptr();
    if (aCharset == HTTP_windows_874)
        return KWindows874().Ptr();
    if (aCharset == HTTP_Koi8_r)
        return KOI8R().Ptr();
    if (aCharset == HTTP_Koi8_u)
        return KOI8U().Ptr();
    return KUtf8().Ptr();
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
