/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32base.h>
#include <e32hal.h>
#include <e32svr.h>

#include "StringUtils.h"
#include "nwx_string.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// StringUtils::CopyUsc2ToAscii
// Convert a UCS2 buffer to ascii.
// -----------------------------------------------------------------------------
//
TUint8* StringUtils::CopyUsc2ToAscii(const TUint16* aUsc2Buf)
    {
    if (aUsc2Buf == NULL)
	    {
        return NULL;
        }
	
	TUint len = User::StringLength(aUsc2Buf) + 1;
    TUint8* asciiBuf = new TUint8 [len];
    if (asciiBuf != NULL)
        {
        TPtr8 asciiPtr(asciiBuf, 0, len);
        TPtrC16 ucs2Ptr(aUsc2Buf);
        asciiPtr.Copy(ucs2Ptr);
        asciiPtr.ZeroTerminate();
        }
    return asciiBuf;
    }


// -----------------------------------------------------------------------------
// StringUtils::CopyAsciiToUsc2
// Convert a ascii buffer to UCS2.
// -----------------------------------------------------------------------------
//
TUint16* StringUtils::CopyAsciiToUsc2(const TUint8* aAsciiBuf)
    {
    if (aAsciiBuf == NULL)
	    {
        return NULL;
        }

	TUint len = User::StringLength(aAsciiBuf) + 1;
    TUint16* usc2Buf = new TUint16 [len];
    if (usc2Buf != NULL)
        {
        TPtrC8 asciiPtr(aAsciiBuf);
        TPtr16 ucs2Ptr(usc2Buf, 0, len);
        ucs2Ptr.Copy(asciiPtr);
        ucs2Ptr.ZeroTerminate();
        }
    return usc2Buf;
    }

// -----------------------------------------------------------------------------
// StringUtils::CopyUsc2ToUcs2
// Copy a UCS2 buffer.
// -----------------------------------------------------------------------------
//
TUint16* StringUtils::CopyUsc2ToUcs2(const TUint16* aUsc2Buf)
    {
    if (aUsc2Buf == NULL)
	    {
        return NULL;
        }
	
	TUint len = User::StringLength(aUsc2Buf) + 1;
    TUint16* buf = new TUint16[len];
    if (buf != NULL)
        {
        TPtr16 ptr(buf, 0, len);
        TPtrC16 ucs2Ptr(aUsc2Buf);
        ptr.Copy(ucs2Ptr);
        ptr.ZeroTerminate();
        }
    return buf;
    }

// -----------------------------------------------------------------------------
// StringUtils::CopyAsciiToAscii
// Copy a ascii buffer.
// -----------------------------------------------------------------------------
//
TUint8* StringUtils::CopyAsciiToAscii(const TUint8* aAsciiBuf)
    {
    if (aAsciiBuf == NULL)
	    {
        return NULL;
        }

	TUint len = User::StringLength(aAsciiBuf) + 1;
    TUint8* buf = new TUint8[len];
    if (buf != NULL)
        {
        TPtrC8 asciiPtr(aAsciiBuf);
        TPtr8 ptr(buf, 0, len);
        ptr.Copy(asciiPtr);
        ptr.ZeroTerminate();
        }
    return buf;
    }

// -----------------------------------------------------------------------------
// StringUtils::CopyStringToDes
// Convert a NW_String_t (holding an usc2 null-terminated string) to a TDesC16.
// -----------------------------------------------------------------------------
//
TDesC16* StringUtils::CopyStringToDes(const NW_String_t& aUcs2String)
    {
    HBufC16* des = NULL;

    TUint length = NW_String_getCharCount((NW_String_t*) &aUcs2String, 
            HTTP_iso_10646_ucs_2);

    des = HBufC16::New(length + 1);

    if (des)
        {
        des->Des().Append((const TUint16*) aUcs2String.storage, length);
        des->Des().ZeroTerminate();
        }

    return des;
    }

// -----------------------------------------------------------------------------
// StringUtils::CopyTDesC8ToTDesC16
// Convert a NW_String_t (holding an usc2 null-terminated string) to a TDesC16.
// -----------------------------------------------------------------------------
//
TDesC16* StringUtils::CopyTDesC8ToTDesC16(const TDesC8& aTDesC8String)
    {
    HBufC16* des = NULL;

    TUint length = aTDesC8String.Length();

    des = HBufC16::New(length + 1);

    if (des)
      {
      des->Des().Copy(aTDesC8String);
      des->Des().ZeroTerminate();
      }

    return des;
    }

// -----------------------------------------------------------------------------
// StringUtils::CopyTUint16ToTDesC16
// Convert a NW_String_t (holding an usc2 null-terminated string) to a TDesC16.
// -----------------------------------------------------------------------------
//
TDesC16* StringUtils::CopyTUint16ToTDesC16(const TUint16* aTUint16String)
    {
    HBufC16* des = NULL;

    TUint length = User::StringLength(aTUint16String);

    des = HBufC16::New(length + 1);

    if (des)
        {
        des->Des().Append((const TUint16*) aTUint16String, length);
        des->Des().ZeroTerminate();
        }

    return des;
    }

// -----------------------------------------------------------------------------
// StringUtils::ConvertPtrUsc2ToAscii
// Convert a UCS2 buffer to ascii.
// -----------------------------------------------------------------------------
//
TInt StringUtils::ConvertPtrUsc2ToAscii(const TPtrC& aUsc2Ptr, TUint8** retPtr)
    {
    TUint len = aUsc2Ptr.Length() + 1; // 1 for NULL terminator

    TUint8* asciiBuf = new TUint8[len];
    if (asciiBuf != NULL)
        {
        TPtr8 asciiPtr(asciiBuf, 0, len);
        asciiPtr.Copy(aUsc2Ptr);
        asciiPtr.ZeroTerminate();
        *retPtr = asciiBuf;
        }
    else
        {
        return KErrNoMemory;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// StringUtils::ConvertPtrAsciiToUcs2
// Convert a ascii buffer to UCS2.
// -----------------------------------------------------------------------------
//
TInt StringUtils::ConvertPtrAsciiToUcs2(const TPtrC8& aAsciiPtr, TUint16** retPtr)
    {
    TUint len = aAsciiPtr.Length() + 1; // 1 for NULL terminator

    TUint16* ucs2Buf = new TUint16[len];
    if (ucs2Buf != NULL)
        {
        TPtr ucs2Ptr(ucs2Buf, 0, len);
        ucs2Ptr.Copy(aAsciiPtr);
        ucs2Ptr.ZeroTerminate();
        *retPtr = ucs2Buf;
        }
    else
        {
        return KErrNoMemory;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// StringUtils::ConvertPtrUcs2ToUcs2
// Convert a UCS2 buffer to UCS2.
// -----------------------------------------------------------------------------
//
TInt StringUtils::ConvertPtrUcs2ToUcs2(const TPtrC& aUcs2Ptr, TUint16** retPtr)
    {
    TUint len = aUcs2Ptr.Length() + 1; // 1 for NULL terminator

    TUint16* ucs2Buf = new TUint16[len];
    if (ucs2Buf != NULL)
        {
        TPtr ucs2Ptr(ucs2Buf, 0, len);
        ucs2Ptr.Copy(aUcs2Ptr);
        ucs2Ptr.ZeroTerminate();
        *retPtr = ucs2Buf;
        }
    else
        {
        return KErrNoMemory;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// StringUtils::ConvertUintToUcs2
// Convert a TUint to UCS2.
// -----------------------------------------------------------------------------
//
TInt StringUtils::ConvertUintToUcs2(TUint aUint, TUint16** retPtr)
    {
    TUint16 buf[10];

    NW_Str_Itoa((TInt) aUint, buf);
    TUint i = User::StringLength(buf);
    TUint16* ret = new TUint16[i + 1];
    if (ret == NULL)
        {
        return KBrsrOutOfMemory;
        }
    Mem::Copy(ret, (void*) buf, (i + 1) * sizeof(TUint16));
    *retPtr = ret;
    return KBrsrSuccess;
    }


// -----------------------------------------------------------------------------
// StringUtils::CopyHBufCZeroTerminateL
// Create HBufC.
// -----------------------------------------------------------------------------
//
HBufC* StringUtils::CopyHBufCZeroTerminateL(const TDesC& aInBuf)
    {
    TPtrC ptr(aInBuf);

    if ((ptr.Length() > 0) && (ptr[ptr.Length() - 1] == '\0'))
        {
        ptr.Set(ptr.Left(ptr.Length() - 1));
        }
    HBufC* buf = HBufC::NewL(ptr.Length() + 1);
    buf->Des().Copy(ptr);
    buf->Des().ZeroTerminate();
    return buf;
    }

// -----------------------------------------------------------------------------
// StringUtils::MakeHBufCLC
// Create HBufC.
// -----------------------------------------------------------------------------
//
HBufC* StringUtils::MakeHBufCLC(const TUint16* aBuffer, TUint aStart, TUint aEnd)
    {

    TPtrC ptr(aBuffer + aStart, aEnd - aStart);
    if (ptr.Length() > 0 && ptr[ptr.Length() - 1] == '\0')
        {
        ptr.Set(ptr.Left(ptr.Length() - 1));
        }

    HBufC* buf = HBufC::NewL(ptr.Length() + 1);
    buf->Des().Copy(ptr);
    buf->Des().ZeroTerminate();
    CleanupStack::PushL(buf);
    return buf;
    }
// -----------------------------------------------------------------------------
// StringUtils::DisplayAvailableMemory
// Show the amount of free memory in the system
// -----------------------------------------------------------------------------
//
void StringUtils::DisplayAvailableMemory()
    {
#ifdef _DEBUG
    RDebug::Print(_L("Available memory"));
#endif
    }

// -----------------------------------------------------------------------------
// StringUtils::ConvertToUcsAndMakeHBufCLC
// Create HBufC.
// -----------------------------------------------------------------------------
//

HBufC* StringUtils::ConvertToUnicodeLC(const TUint8* aBuffer, TUint aStart, TUint aEnd)
    {

    TPtrC8 ptr(aBuffer + aStart, aEnd - aStart);
    if (ptr.Length() > 0 && ptr[ptr.Length() - 1] == '\0')
        {
        ptr.Set(ptr.Left(ptr.Length() - 1));
        }

    HBufC* buf = HBufC::NewL(ptr.Length() + 1);
    buf->Des().Copy(ptr);
    TPtr16 bufDes16 = buf->Des();
    bufDes16.ZeroTerminate();
    CleanupStack::PushL(buf);
    return buf;
    }

// -----------------------------------------------------------------------------
// StringUtils::CopyHBufCZeroTerminateL
// Create HBufC.
// -----------------------------------------------------------------------------
//
HBufC* StringUtils::CopyConvertHBufCZeroTerminateL(const TDesC8& aInBuf)
    {
    TPtrC8 ptr(aInBuf);

    if ((ptr.Length() > 0) && (ptr[ptr.Length() - 1] == '\0'))
        {
        ptr.Set(ptr.Left(ptr.Length() - 1));
        }
    HBufC* buf = HBufC::NewL(ptr.Length() + 1);
    buf->Des().Copy(ptr);
    TPtr16 bufDes16 = buf->Des();
    bufDes16.ZeroTerminate();
    return buf;
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
