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


/** ----------------------------------------------------------------------- **
    @package:     NW_HTMLP

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_HTMLP_TO_WBXML_H
#define NW_HTMLP_TO_WBXML_H

#include "nwx_buffer.h"
#include "nw_htmlp_dict.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef TBrowserStatusCode NW_HTMLP_CharsetConvertCB(void* context,
                                                     NW_Uint32 charsetLength,
                                                             NW_Uint32 charsetOffset,
                                                             NW_Buffer_t* body, 
                                                             NW_Int32* numUnconvertible, 
                                                             NW_Int32* indexFirstUnconvertible, 
                                                             NW_Buffer_t** outBuf,
                                                             NW_Uint32 *selectedCharset);


typedef TBrowserStatusCode NW_HTMLP_NotifyDocCompleteCB(void* context,
                                                        const NW_Buffer_t* residueWbxml,
                                                        NW_Uint32 encoding,
                                                        NW_Uint32 codePageSwitchCount,
                                                        NW_Int32 lastValid,
                                                        void* WBXMLEncStrTbl);

typedef struct NW_HTMLP_WbxmlEncoder_s
{
  NW_WBXML_Writer_t* pE;
  /*
  need access to last tag token to backpatch attribute and content flags
  */
  NW_Uint32 lastTagTokenIndex;
  NW_Uint32 publicID; /* needed to pass to start callback */
  NW_Uint32 encoding; /* needed to pass to start callback */
  
} NW_HTMLP_WbxmlEncoder_t;

extern
TBrowserStatusCode NW_HTMLP_NotifyDocComplete(void * parser,
                                              NW_Uint32* encoding, 
                                              void** WBXMLEncStrTbl,
                                              NW_HTMLP_NotifyDocCompleteCB* callBack,
                                              void* callBackContext);

extern
TBrowserStatusCode NW_HTMLP_SegToWbxml(NW_Buffer_t* pInBuf, 
                                 NW_Uint32 encoding,
                                 NW_Uint8 byteOrder,
                                 NW_Buffer_t** ppOutBuf, 
                                 NW_Uint32* pLine,
				                         NW_Uint32 publicID, 
				                         NW_HTMLP_ElementTableIndex_t elementCount,
                                 NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
                                 NW_Bool consumeSpaces,
                                 NW_Bool needCharsetDetect,
                                 NW_Bool isFirstSegment,     /* is this the first segment of a document? */
                                 NW_Bool isLastSegment,      /* is this the last segment of a document? */
                                 NW_Int32 *pLastValid,
                                 void** ppParser,
                                 NW_Uint32* pCodePageSwitchCount,
                                 void* charsetConvertContext,
                                 NW_HTMLP_CharsetConvertCB* charsetConvertCallback,
                                 void** WBXMLEncStrTbl,
                                 NW_Bool isScript);

/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_HtmlToWbxml

    @synopsis:    Converts HTML structure to WBXML structure.

    @scope:       public

    @parameters:
       [in] NW_Buffer_t* pInBuf
                  The buffer to process.

       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint8 byteOrder
                  The byte order.

       [out] NW_Buffer_t** ppOutBuf
                  Buffer to receive output.

       [out] NW_Uint32* line
                  Line count.

       [in] NW_Uint32 publicID
                  The publicID.

       [in] NW_HTMLP_ElementTableIndex_t elementCount
                  Element count.

       [in] NW_HTMLP_ElementDescriptionConst_t* pElementDictionary
                  Element dictionary.

       [in] void* charsetConvertContext
                  Context to use for callback.

       [in] NW_HTMLP_CharConvCB charsetConvertCallback
                  A callback to use when detecting a new charset to use.

    @description: Converts HTML structure to WBXML structure.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Document converted.

       [KBrsrFailure]
                  General error. Document not converted.

       [KBrsrOutOfMemory]
                  Out of memory failure.

 ** ----------------------------------------------------------------------- **/
extern
TBrowserStatusCode NW_HTMLP_HtmlToWbxml(NW_Buffer_t* pInBuf, NW_Uint32 encoding,
                                        NW_Uint8 byteOrder,
                                        NW_Buffer_t** ppOutBuf, NW_Uint32* line, 
                                        NW_Uint32 publicID, 
                                        NW_HTMLP_ElementTableIndex_t elementCount,
                                        NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
                                        NW_Bool consumeSpaces,
                                        NW_Bool needCharsetDetect,
                                        void* charsetConvertContext,
                                        NW_HTMLP_CharsetConvertCB* charsetConvertCallback,
                                        NW_Bool isScript);

// For UTF-8 and UCS-2 handle BOM (byte order mark) -- remove BOM.  Also, for UCS-2, handle
// network (big endian) to native (little endian) byte order conversion.
TBrowserStatusCode NW_HTMLP_HandleBOM(NW_Uint32 encoding, NW_Uint8 byteOrder,
    NW_Byte** pDocStartAddress, NW_Uint32* pDocByteLength);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

/* NW_HTMLP_TO_WBXML_H */
#endif
