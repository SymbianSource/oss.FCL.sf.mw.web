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


#ifndef NW_HED_MIMETABLE_H
#define NW_HED_MIMETABLE_H

#include "nw_object_base.h"
#include "nw_hed_contenthandler.h"
#include <urlloader_urlresponse.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_MimeTable_Class_s NW_HED_MimeTable_Class_t;
typedef struct NW_HED_MimeTable_s NW_HED_MimeTable_t;

typedef struct NW_HED_MimeTable_Entry_s NW_HED_MimeTable_Entry_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */
typedef
NW_HED_ContentHandler_t*
(*NW_HED_MimeTable_CreateContentHandler_t) (const NW_HED_MimeTable_t* mimeTable,
                                            NW_HED_DocumentNode_t* parent,
                                            NW_Http_ContentTypeString_t contentTypeString,
                                            NW_HED_UrlRequest_t* urlRequest,
                                            NW_Url_Resp_t* response,
                                            NW_Bool isTopLevel);

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_MimeTable_ClassPart_s {
  NW_Uint8 numEntries;
  const NW_HED_MimeTable_Entry_t* entries;
  NW_HED_MimeTable_CreateContentHandler_t createContentHandler;
} NW_HED_MimeTable_ClassPart_t;

struct NW_HED_MimeTable_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_HED_MimeTable_ClassPart_t NW_HED_MimeTable;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_MimeTable_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_MimeTable_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_MimeTable))

#define NW_HED_MimeTableOf(_object) \
  (NW_Object_Cast (_object, NW_HED_MimeTable))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_MimeTable_Class_t NW_HED_MimeTable_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_MimeTable_CreateContentHandler(_mimeTable, _parent, _contentType, _urlRequest, _response, _isTopLevel) \
  (NW_Object_Invoke (_mimeTable, NW_HED_MimeTable, createContentHandler) \
   ((_mimeTable), (_parent), (_contentType), (_urlRequest), (_response), (_isTopLevel)))

NW_HED_EXPORT
NW_Bool
NW_HED_MimeTable_IsSupportedContent(NW_HED_MimeTable_t* mimeTable, 
                                   NW_Http_ContentTypeString_t contentTypeString,
                                   NW_Bool isTopLevel);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_MIMETABLE_H */
