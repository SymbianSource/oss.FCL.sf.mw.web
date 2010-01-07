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
* Description:  String related utility methods.
*
*/

#ifndef STRINGUTIL_H
#define STRINGUTIL_H

//  INCLUDES
#include <e32std.h>

#include "nwx_defs.h"
#include <nw_string_string.h>

// CLASS DECLARATION

/**
* A static class which provides string related utility methods.
*
* @lib Browser Engine
* @since 2.6
*/
class StringUtils
    {
    public:  // Constructors and destructor

        /**
        * Convert a UCS2 buffer to ascii.
        *
        * @since 2.6
        * @param aUsc2Buf The buffer to convert.
        * @return The converted buffer or NULL, if failed to allocate a buffer.
        */
        static TUint8* CopyUsc2ToAscii(const TUint16* aUsc2Buf);

        /**
        * Convert a ascii buffer to unicode.
        *
        * @since 2.6
        * @param aAsciiBuf The buffer to convert.
        * @return The converted buffer or NULL, if failed to allocate a buffer.
        */
        static TUint16* CopyAsciiToUsc2(const TUint8* aAsciiBuf);

        /**
        * Copy a UCS2 buffer.
        *
        * @since 2.6
        * @param aUsc2Buf The buffer to copy.
        * @return The opied buffer or NULL, if failed to allocate a buffer.
        */
        static TUint16* CopyUsc2ToUcs2(const TUint16* aUsc2Buf);

        /**
        * Copy a ascii buffer.
        *
        * @since 2.6
        * @param aAsciiBuf The buffer to copy.
        * @return The copied buffer or NULL, if failed to allocate a buffer.
        */
        static TUint8* CopyAsciiToAscii(const TUint8* aAsciiBuf);

        /**
        * Convert a NW_String_t (holding an usc2 null-terminated string) to a TDes16.
        *
        * @since 2.6
        * @param aUcs2String The string to copy
        * @return The copied descriptor or NULL, if failed to allocate a buffer.
        */
        static TDesC16* CopyStringToDes(const NW_String_t& aUcs2String);
        
        /**
        * Convert a TDesC8* to a TDes16*
        *
        * @since 2.6
        * @param aTDes8String The string to copy
        * @return The copied descriptor or NULL, if failed to allocate a buffer.
        */
        static TDesC16* CopyTDesC8ToTDesC16(const TDesC8& aTDesC8String);

        /**
        * Convert a TUint16* to a TDesC16
        *
        * @since 2.6
        * @param aTUint16String The string to copy
        * @return The copied descriptor or NULL, if failed to allocate a buffer.
        */
        static TDesC16* CopyTUint16ToTDesC16(const TUint16* aTUint16String);        
        

        /**
        * Convert a UCS2 buffer to ascii.
        *
        * @since 2.6
        * @param aUsc2Ptr A descriptor to the buffer to convert.
        * @param retPtr The returned bufer.
        * @return KErrNone or KErrNoMemory.
        */
        static TInt ConvertPtrUsc2ToAscii(const TPtrC& aUsc2Ptr, TUint8** retPtr);

        /**
        * Convert a ascii buffer to UCS2.
        *
        * @since 2.6
        * @param aAsciiPtr A descriptor to the buffer to convert.
        * @param retPtr The returned bufer.
        * @return KErrNone or KErrNoMemory.
        */
        static TInt ConvertPtrAsciiToUcs2(const TPtrC8& aAsciiPtr, TUint16** retPtr);

        /**
        * Convert a UCS2 Desc to UCS2.
        *
        * @since 2.6
        * @param aUcs2Ptr A descriptor to the buffer to convert.
        * @param retPtr The returned bufer.
        * @return KErrNone or KErrNoMemory.
        */
        static TInt ConvertPtrUcs2ToUcs2(const TPtrC& aUcs2Ptr, TUint16** retPtr);

        /**
        * Convert a TUint to UCS2.
        *
        * @since 2.6
        * @param aUint The number to convert.
        * @param retPtr The returned bufer.
        * @return KErrNone or KErrNoMemory.
        */
        static TInt ConvertUintToUcs2(TUint aUint, TUint16** retPtr);

        /**
        * Create HBufC.
        *
        * @since 2.6
        * @param aInBuf The buffer to copy.
        * @return The new HBufC.
        */
        static HBufC* CopyHBufCZeroTerminateL(const TDesC& aInBuf);

        /**
        * Create HBufC.
        *
        * @since 2.6
        * @param aBuffer The buffer to copy from.
        * @param aStart The starting point in the buffer to copy.
        * @param aEnd The ending point in the buffer to copy.
        * @return The new HBufC.
        */
        static HBufC* MakeHBufCLC(const TUint16* aBuffer, TUint aStart, TUint aEnd);

        /**
        * Convert To Unicode.
        *
        * @since 2.6
        * @param aBuffer The buffer to copy from.
        * @param aStart The starting point in the buffer to copy.
        * @param aEnd The ending point in the buffer to copy.
        * @return The new HBufC.
        */
        static HBufC* ConvertToUnicodeLC(const TUint8* aBuffer, TUint aStart, TUint aEnd);

        /**
        * Create HBufC. from the TDesC8
        *
        * @since 2.6
        * @param aInBuf The buffer to copy.
        * @return The new HBufC.
        */
        static HBufC* CopyConvertHBufCZeroTerminateL(const TDesC8& aInBuf);
        /**
        * General panic for StringUtils.
        *
        * @since 2.6
        * @param aError The error code from the panicked method.
        * @return void.
        */
        static void Panic(TInt aError);
        /**
        * Show the amount of free memory in the system
        *
        * @since 2.8
        * @return void.
        */
        static void DisplayAvailableMemory();

    };


#endif      // CSTRINGUTIL_H

// End of File
