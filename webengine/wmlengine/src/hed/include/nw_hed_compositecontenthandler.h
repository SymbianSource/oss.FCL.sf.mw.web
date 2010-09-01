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


#ifndef _NW_HED_CompositeContentHandler_h_
#define _NW_HED_CompositeContentHandler_h_

#include <e32def.h>
#include <e32std.h>
#include "nw_hed_contenthandler.h"
#include "nw_hed_compositenode.h"
#include "nwx_osu_file.h"
#include "BrsrStatusCodes.h"
#include "urlloader_urlresponse.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
class CBodyPart;

typedef struct NW_HED_CompositeContentHandler_Class_s NW_HED_CompositeContentHandler_Class_t;
typedef struct NW_HED_CompositeContentHandler_s NW_HED_CompositeContentHandler_t;

typedef struct {
  NW_HED_CompositeContentHandler_t* contentHandler;
  NW_Url_Resp_t* response;
} NW_HED_CharsetConvContext;

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeContentHandler_SaveImagesToFile(NW_HED_CompositeContentHandler_t* thisObj,
                                                 NW_Osu_File_t fh);

NW_HED_EXPORT
TBrowserStatusCode
_NW_HED_CompositeContentHandler_SaveScriptsToFile(NW_HED_CompositeContentHandler_t* thisObj,
                                                 NW_Osu_File_t fh);


TBrowserStatusCode
_NW_HED_CompositeContentHandler_ComputeEncoding(NW_HED_CompositeContentHandler_t* thisObj,
                                                NW_Url_Resp_t* response,
                                                NW_Bool* encodingFound);

TBrowserStatusCode NW_HED_CompositeContentHandler_CharConvCB(void* context,
                                                             NW_Uint32 charsetLength,
                                                             NW_Uint32 charsetOffset,
                                                             NW_Buffer_t* body, 
                                                             NW_Int32* numUnconvertible, 
                                                             NW_Int32* indexFirstUnconvertible, 
                                                             NW_Buffer_t** outBuf,
                                                             NW_Uint32 *selectedCharset);

TBool
_NW_HED_CompositeContentHandler_IsUrlInMultipart( NW_HED_CompositeContentHandler_t* thisObj,
                                                  const NW_Ucs2* aUrl,
                                                  CBodyPart** aBodyPart );

/* ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_CompositeContentHandler_DocumentDisplayed_t) (NW_HED_ContentHandler_t* thisObj);


/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_CompositeContentHandler_ClassPart_s {
  NW_HED_CompositeContentHandler_DocumentDisplayed_t documentDisplayed;
} NW_HED_CompositeContentHandler_ClassPart_t;

struct NW_HED_CompositeContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
  NW_HED_CompositeContentHandler_ClassPart_t NW_HED_CompositeContentHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_CompositeContentHandler_s {
  NW_HED_ContentHandler_t super;

  /* aggregate(s) */
  NW_HED_CompositeNode_t NW_HED_CompositeNode;

  // array of body parts if multipart page
  RPointerArray<CBodyPart> iBodyPartArray;
  // old response body which holds multipart buffer
  NW_Buffer_t* iOldResponseBody;
  // multipart page or not
  TBool iMultipart;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_CompositeContentHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_CompositeContentHandler))

#define NW_HED_CompositeContentHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_HED_CompositeContentHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_CompositeContentHandler_Class_t NW_HED_CompositeContentHandler_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_CompositeContentHandler_SaveImagesToFile(_object, _fh) \
  (_NW_HED_CompositeContentHandler_SaveImagesToFile \
     (NW_HED_CompositeContentHandlerOf (_object), _fh))

#define NW_HED_CompositeContentHandler_SaveScriptsToFile(_object, _fh) \
  (_NW_HED_CompositeContentHandler_SaveScriptsToFile \
     (NW_HED_CompositeContentHandlerOf (_object), _fh))


NW_HED_EXPORT
NW_Uint32
NW_HED_CompositeContentHandler_GetImageCount(NW_HED_CompositeContentHandler_t* thisObj);

NW_HED_EXPORT
NW_Uint32
NW_HED_CompositeContentHandler_GetScriptCount(NW_HED_CompositeContentHandler_t* thisObj);



NW_HED_EXPORT
TBrowserStatusCode
NW_HED_CompositeContentHandler_AddChild( NW_HED_CompositeContentHandler_t* thisObj,
                                         NW_HED_DocumentNode_t* childNode,
                                         void* context );

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_CompositeContentHandler_HandleImageLoad( NW_HED_CompositeContentHandler_t* thisObj,
                                                NW_Int32 chunkIndex,
					                                      NW_HED_ContentHandler_t* newContentHandler,
                                                NW_HED_UrlRequest_t* urlRequest );


NW_HED_EXPORT
NW_HED_ContentHandler_t*
NW_HED_CompositeContentHandler_CreateContentHandler( NW_HED_ContentHandler_t* thisObj,
                                            NW_Uint8* contentTypeString, NW_HED_UrlRequest_t* urlRequest, 
                                            NW_Url_Resp_t* response, void* clientData );
NW_HED_EXPORT
NW_Int16 NW_HED_CompositeContentHandler_Encoding(NW_Uint8* charset, NW_Uint32 charsetLength);

NW_Int32 NW_HED_CompositeContentHandler_GetCharsetString(NW_Uint16 aCharset,
                                                     const NW_Uint8** aCharsetString);

TBrowserStatusCode
_NW_HED_CompositeContentHandler_ConvertEncoding(NW_HED_CompositeContentHandler_t* thisObj,
                                                NW_Uint32 foreignEncoding, 
                                                NW_Buffer_t* inBuf, 
                                                NW_Int32* numUnconvertible, 
                                                NW_Int32* indexFirstUnconvertible, 
                                                NW_Buffer_t** outBuf);                       


/* ------------------------------------------------------------------------- */
#define NW_HED_CompositeContentHandler_DocumentDisplayed(_object) \
  (NW_Object_Invoke (_object, NW_HED_CompositeContentHandler, documentDisplayed) \
   (NW_HED_ContentHandlerOf (_object)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_CompositeContentHandler_h_ */
