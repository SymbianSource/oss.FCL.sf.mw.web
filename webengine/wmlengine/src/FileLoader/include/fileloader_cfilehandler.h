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



#ifndef CFILEHANDLER_H
#define CFILEHANDLER_H

//  INCLUDES
#include "urlloader_urlloaderint.h"
#include "urlloader_murlschemehandler.h"
#include <f32file.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CKFileLoader;
/**
*  Handling the load of a specific file. A CFileHandler object is instantiated
*  to handle each file load request.
*
*  @lib fileloader.lib
*  @since 2.0
*/
class CFileHandler : public CActive
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aUrl The request URL.
    * @param aTransId The request's transaction id.
    * @param aLoadContext The load context that should be returned with the response.
    * @param aLoadType The type of the load.
    * @param aLoadCallback The callback to call when the response is ready.
    * @param aFileLoader The context of the file loader, used to send back the response.
    * @return CFileHandler
	*/
    static CFileHandler* NewL(const TUint16 *aUrl,
                              TUint8 aMethod,
                              TUint16 aTransId,
                              TUint8 aLoadType,
                              void *aLoadContext,
                              NW_Url_RespCallback_t *aLoadCallback,
                              CKFileLoader* aFileLoader);

    /**
    * Destructor.
    */
    virtual ~CFileHandler();

public: // New functions

    /**
    * Create a CFileHandler object and initiate the load.
    * @since 2.0
    * @param aUrl The request URL.
    * @param aTransId The request's transaction id.
    * @param aLoadType The type of the load.
    * @param aLoadContext The load context that should be returned with the response.
    * @param aLoadCallback The callback to call when the response is ready.
    * @param aFileLoader The context of the file loader, used to send back the response.
    * @return void
	*/
    static void LoadFileL(const TUint16 *aUrl,
                          TUint16 aTransId,
                          TUint8 aLoadType,
                          void *aLoadContext,
                          NW_Url_RespCallback_t *aLoadCallback,
                          CKFileLoader* aFileLoader);

    /**
    * Create a CFileHandler object and initiate a head request.
    * @since 2.6
    * @param aUrl The request URL.
    * @param aTransId The request's transaction id.
    * @param aLoadType The type of the load.
    * @param aLoadContext The load context that should be returned with the response.
    * @param aLoadCallback The callback to call when the response is ready.
    * @param aFileLoader The context of the file loader, used to send back the response.
    * @return void
	*/
    static void GetFileInfoL(const TUint16 *aUrl,
                          TUint16 aTransId,
                          TUint8 aLoadType,
                          void *aLoadContext,
                          NW_Url_RespCallback_t *aLoadCallback,
                          CKFileLoader* aFileLoader);

public: // Functions from base classes

    /**
    * From CActive Return the response after file loading from the File system is complete.
    * @since 2.0
    * @param 
    * @return void
    */
    void RunL();

    /**
    * From CActive Noop. File loading cannot be cancelled.
    * @since 2.0
    * @param 
    * @return void
    */
    void DoCancel();

private:

    /**
    * C++ default constructor.
    */
    CFileHandler(const TUint16 *aUrl,
                 TUint8 aMethod,
                 TUint16 aTransId,
                 TUint8 aLoadType,
                 void *aLoadContext,
                 NW_Url_RespCallback_t *aLoadCallback,
                 CKFileLoader* aFileLoader);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * Determine the content type of the file.
    */
    void ContentTypeL(TUint8** aContentTypeString);

    /**
    * Determine the content encoding of the file.
    */
    TUint16 ContentEncoding(TUint8* aContentTypeString);

    /**
    * Translate the file name from a URL to a valid file name in the system.
    */
    TBool GetFileNameL();

    /**
    * Determine if the file is a saved deck or not. If it is saved deck, parse the file.
    */
    TBool ParseSavedDeck();

private:    // Data

    // The URL of the file to load
    const TUint16* iUrl;

    // The method
    TUint8 iMethod;

    // The loadType
    TUint8 iLoadType;

    // The URL of the loaded file. Could be different than the original URL in case of saved deck.
    TUint16* iRedirectedUrl;

    // The context to return in the response callback.
    void *iLoadContext;

    // The callback to call for the response.
    NW_Url_RespCallback_t *iLoadCallback;

    // The CFileLOader object used to return the response.
    CKFileLoader* iFileLoader;

    // The transaction's id.
    TUint16 iTransId;

    // A session to the file system.
    RFs iRfs;

    // A handle to the file being loaded.
    RFile iFile;

    // The file's content.
    TPtr8 iData;

    // The loaded file's name. This is different from the URL. It does not contain the scheme.
    TPtr iFileName;

    // A flag if opening RFs was successful and it needs to be closed.
    TBool iRfsOpen;

    // A flag if opening the file was successful and it needs to be closed.
    TBool iFileOpen;

    // The number of bytes in the file.
    TInt iSize;

    // The content-type of the file.
    TUint8* iContentType;

};

#endif      // CFILEHANDLER_H

// End of File
