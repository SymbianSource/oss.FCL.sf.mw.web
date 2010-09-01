/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File-name related utility methods.
*
*/



#ifndef FILENAMEUTIL_H
#define FILENAMEUTIL_H


//  INCLUDES
#include <e32std.h>


// CLASS DECLARATION

/**
* A static class which provides file-name related utility methods.
*
* @lib BrowserEngine
* @since 2.6
*/
class FileNameUtils
    {
    public:  // Constructors and destructor
        // None.

    public:  // New functions

        /**
        * Gets the localized file name, in full, which includes the path but
        * not the drive. This is intended for use as a displayable path and
        * as such the drive is not included since drive letters are never shown
        * to the user.
        *
        * @since 2.6
        * @param aFullFileName Ptr to file name that is to be localized.
        * @param aLocalizedFullFileName Ptr into which the localized file name
        *   is returned, which is a pointer to a newly allocated heap buffer.
        *   The caller is responsible for deallocating the buffer when it is no
        *   longer needed.
        * @return KBrsrSuccess, KBrsrOutOfMemory or KBrsrFailure.
        */
        static TBrowserStatusCode GetLocalizedFullFileName( TPtrC& aFullFileName,
            HBufC** aLocalizedFullFileName );

    };

#endif      // FILENAMEUTIL_H

// End of File
