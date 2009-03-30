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



#ifndef CSavedDeckHandler_H
#define CSavedDeckHandler_H

//  INCLUDES
#include "urlloader_urlloaderint.h"
#include <f32file.h>

// CONSTANTS

// MACROS

// DATA TYPES
// This data type does not have a destructor
// and does not take ownership of the strings
class CSavedDeckPart
{
public:
    CSavedDeckPart():iUrl(NULL,0),iData(NULL,0),iContentTypeString(NULL,0){}
    void Reset();
    TPtrC8 iUrl;
    TPtrC8 iData;
    TPtrC8 iContentTypeString;
    TUint32 iCharset;
	TUint32 iOriCharset;
};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Handling the loading and parsing of a saved deck.
*  
*
*  @lib fileloader.lib
*  @since 2.0
*/
class CSavedDeckHandler : public CBase
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aData The buffer loaded from the file system.
    */
    static CSavedDeckHandler* NewLC(TPtrC8& aData);

    /**
    * Destructor.
    */
    virtual ~CSavedDeckHandler();

public: // New functions

    /**
    * Parse the saved deck buffer into the parts and store each part in cache.
    * @since 2.0
    * @param aData The file content loaded from the file system.
    * @param aRequestUrl The original request URL.
    * @param aResponseData The body of the first data part.
	*						It will be sent back as the response.
    * @param aResponseUrl The Original URL of the first part of the response.
    * @param aContentTypeString The string representation of the content type
	*							 of the first data part. It is used only if a
	*							 WAP token cannot be identified.
    * @param aCharset The character set of the first data part.
    * @param aSavedDeck A flag indicating if this is a saved deck or not.
    * @return KErrCorrupt or KErrNone
    */
    static TInt ParseSavedDeck(TPtrC8& aData,
                               TPtrC& aRequestUrl,
                               TPtr8& aResponseData,
                               TPtr& aResponseUrl,
                               TPtr8& aContentTypeString,
                               TUint& aCharset,
                               TBool& aSavedDeck);


    /**
    * Parse the saved deck buffer into the parts and store each part in cache.
    * @since 2.0
    * @param aData The file content loaded from the file system.
    * @param aRequestUrl The original request URL.
    * @param aResponseData The body of the first data part.
	*						It will be sent back as the response.
    * @param aResponseUrl The Original URL of the first part of the response.
    * @param aContentType The WAP token content type of the first data part.
    * @param aContentTypeString The string representation of the content type
	*							 of the first data part. It is used only if a
	*							 WAP token cannot be identified.
    * @param aCharset The character set of the first data part.
    * @param aSavedDeck A flag indicating if this is a saved deck or not.
    * @return void
    */
    static void ParseSavedDeckL(TPtrC8& aData,
                                TPtrC& aRequestUrl,
                                TPtr8& aResponseData,
                                TPtr& aResponseUrl,
                                TPtr8&  aContentTypeString,
                                TUint& aCharset,
                                TBool& aSavedDeck);

private:

    /**
    * C++ default constructor.
    */
    CSavedDeckHandler(TPtrC8& aData);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * Check if the loaded file is a saved deck, based on the path and extension.
    */
    static TBool IsSavedDeck(TPtrC& aRequestUrl);

    /**
    * Extract the next data part from the buffer.
    */
    void GetNextPartL(CSavedDeckPart& aSavedDeckPart);

    /**
    * Copy the data and URL of the first part, to be returned as the response.
    */
    void PrepareResponseLC(CSavedDeckPart& aSavedDeckPart,
                           TPtr8& aResponseData,
                           TPtr& aResponseUrl,
                           TPtr8& aContentTypeString,
                           TUint& aCharset);

    /**
    * Send a response part to cache.
    */
    void LoadPartToCacheL(CSavedDeckPart& aSavedDeckPart);

	/**
	* Convert charset to string
	*/
    TPtrC8 ConvertCharsetToStr(TUint32 aCharSet);

    /**
    * Extract a TUint from a buffer and advance the pointer.
	*/
    TUint GetUintFromBuffer(TPtrC8& aPtr)
	{
		TUint i;
        TUint8* c = (TUint8*)(&i);
        Mem::Copy(c, aPtr.Ptr(), sizeof(TUint));
		aPtr.Set(aPtr.Mid(sizeof(TUint)));
		return i;
	}

private:    // Data

    // The file content loaded from the file system.
    TPtrC8 iData;

    // A descriptior pointing to the current position to process in the loaded file.
    TPtrC8 iDataReader;
};

#endif      // CSavedDeckHandler_H

// End of File
