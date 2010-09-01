/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NWX_MULTIPART_GENERATOR_H
#define NWX_MULTIPART_GENERATOR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "nwx_defs.h"
#include "nwx_http_header.h"
#include "BrsrStatusCodes.h"

NW_Byte* NW_StoreUint(NW_Byte* p, NW_Uint32 i);
    
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
);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NWX_MULTIPART_GENERATOR_H */
