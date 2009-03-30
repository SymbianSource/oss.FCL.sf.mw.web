/*
* Copyright (c) 1998-1999 Nokia Corporation and/or its subsidiary(-ies).
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


#include "nwx_defs.h"
#include "nwx_http_defs.h"
#include "nwx_string.h"
#include "nw_loadreq.h"
#include "nwx_multipart_generator.h"
#include "BrsrStatusCodes.h"

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/************************************************************************
 
  Function: NW_StoreUint

  Description:  Store a number into a buffer
 
  Parameters:   p     - in: The buffer to store in
                i     - in: The number to store
    
  Return Value: The new offset in the buffer
                  
**************************************************************************/
NW_Byte* NW_StoreUint(NW_Byte* p, NW_Uint32 i)
{
    NW_Byte* pI;
    NW_Uint32 j = i;
    
    NW_ASSERT(p != NULL);

    pI = (NW_Byte*)(&j);
    NW_Mem_memcpy(p, pI, sizeof(NW_Uint32));
    return p + sizeof(NW_Uint32);
}


/************************************************************************
 
  Function: NW_CreateMultipartSegment

  Description:  Construct a multipart segment.  It is the caller's 
                responsibility to delete the resulting buffer. 
 
  Parameters:   encoding        - in: character encoding
                contentTypeString - in: eg HTTP_application_xhtml_xml_string
                contentLocation - in: content location header
                data            - in: Data for multipart segment
                dataLen         - in: Length of data
                result          - out: Resultant allocated buffer
                resultLen       - out: Length of output buffer
    
  Return Value: KBrsrSuccess
                KBrsrOutOfMemory
                  
**************************************************************************/
TBrowserStatusCode NW_CreateMultipartSegment
(
    NW_Uint32 encoding,
	NW_Uint32 oriEncoding,
    NW_Http_ContentTypeString_t contentTypeString,
    char* contentLocation,
    const NW_Byte* data,
    NW_Uint32 dataLen,
    NW_Byte** result,
    NW_Uint32* resultLen
)
{
    NW_Byte *p = NULL;
    NW_Uint32 urlLen;
    NW_Uint32 contentTypeStrLen;
    NW_Uint32 len;
    const char* version = SAVED_DECK_VERSION;

    NW_ASSERT(data != NULL || dataLen == 0);
    NW_ASSERT(contentLocation != NULL);
    NW_ASSERT(result != NULL);
    NW_ASSERT(resultLen != NULL);
    NW_ASSERT(contentTypeString != NULL);

    urlLen = NW_Asc_strlen(contentLocation);
    contentTypeStrLen = NW_Asc_strlen((char*)contentTypeString);

    /* 
        4 bytes for URL length + 4 byte for content charset + 4 byte for original charset
        4 bytes for content string length + 4 bytes for dataLen + NULL terminator for URL +
        NULL terminator for body + NULL terminator for contentTypeString.
		These NULL terminators will be used when the file is loaded to determine the validity of the file.
     */
    len = 23 + urlLen + contentTypeStrLen + dataLen + NW_Asc_strlen(version);

    *result = (NW_Byte*) NW_Mem_Malloc(len);
    if (*result == NULL)
        return KBrsrOutOfMemory;
    *resultLen = len;

    p = *result;
    
    // Put the version
    NW_Asc_strcpy((char*)p, version);
    p += NW_Asc_strlen(version);    
    
    // Put the length of the URL;
    p = NW_StoreUint(p, urlLen);
    // Copy the Url
    NW_Asc_strcpy((char*)p, contentLocation);
    p = p + urlLen + 1; // skip the NULL terminator

    // Put the length of the content type string;
    p = NW_StoreUint(p, contentTypeStrLen);
    // Copy the content type string
    NW_Asc_strcpy((char*)p, (char*)contentTypeString);
    p = p + contentTypeStrLen + 1; // skip the NULL terminator

    // Characterset
    p = NW_StoreUint(p, encoding);
    // save original encoding for saved page
    p = NW_StoreUint(p, oriEncoding);

    // dataLen
    p = NW_StoreUint(p, dataLen);
    NW_Mem_memcpy(p, data, dataLen);
    p += dataLen;
    *p = '\0';
    return KBrsrSuccess;
}
