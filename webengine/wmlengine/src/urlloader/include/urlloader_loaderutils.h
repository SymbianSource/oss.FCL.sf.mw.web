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
* Description:  Handling of general url loading
*
*/


#ifndef LOADERUTILS_H
#define LOADERUTILS_H

//  INCLUDES
#include <e32def.h>
#include "BrsrStatusCodes.h"

// CONSTANTS

// MACROS

// DATA TYPES
typedef enum {
    ELogTypeRequest,
    ELogTypeResponse,
    ELogTypeFailedResponse,
    ELogTypeSavedDeck
} TUrlLoaderLogType;

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Utilities used by url loader.
*
*  @lib urlloader.lib
*  @since 2.0
*/
class LoaderUtils
{

public: // New functions

    /**
    * General purpose panic function for url loader related errors.
    * @since 2.0
    * @param aError The reason for panic. Could be left out.
    * @return void
    */
    static void PanicLoader(TInt aError = KErrNone);

    /**
    * Map the Symbian errors to rainbow errors.
    * @since 2.0
    * @param aError The Symbian error code.
    * @return The matching Rainbow error code
    */
    static TBrowserStatusCode MapErrors(TInt aError);

    /**
    * Log the request and the response.
    * @since 2.0
    * @param aUrlLoaderLogType The log type ELogTypeRequest or ELogTypeResponse.
    * @return void
    */
    static void LogLoaderEvents(TUrlLoaderLogType aUrlLoaderLogType, ...);
};

#endif      // LOADERUTILS_H

// End of File
