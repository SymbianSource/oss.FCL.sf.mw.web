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


#ifndef NW_HED_MIMETABLEI_H
#define NW_HED_MIMETABLEI_H

#include "nw_object_basei.h"
#include "nw_hed_mimetable.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */
struct NW_HED_MimeTable_Entry_s {
  const NW_Http_ContentTypeString_t contentTypeString;
  NW_HED_ContentHandler_Class_t* objClass;
  NW_Bool acceptTopLevel;
};

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_ContentHandler_t*
_NW_HED_MimeTable_CreateContentHandler (const NW_HED_MimeTable_t* mimeTable,
                                        NW_HED_DocumentNode_t* parent,
                                        NW_Http_ContentTypeString_t contentTypeString,
                                        NW_HED_UrlRequest_t* urlRequest,
                                        NW_Url_Resp_t* response,
                                        NW_Bool isTopLevel);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_MIMETABLEI_H */
