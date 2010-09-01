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

#include <e32std.h>
#include <stdarg.h>

#include "nw_hed_mimetablei.h"
#include "nwx_http_defs.h"
#include "nwx_string.h"
#include "nw_imagech_epoc32contenthandler.h"

static
NW_Bool NW_HED_MimeTable_IsImageContentType(NW_Http_ContentTypeString_t contentTypeString)
  {
  NW_Int32 pos = -1;
  NW_Bool imageType = NW_FALSE;

  pos = NW_Asc_Strchr((char*)contentTypeString, '/');
  if(pos != -1)
    {
    if( NW_Asc_strnicmp((char*)contentTypeString, (char*)HTTP_image_string, pos) == 0 )
      {
     imageType = NW_TRUE;
      }
    }
  return imageType;
  }

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_HED_MimeTable_Class_t NW_HED_MimeTable_Class = {
  { /* NW_Object_Core            */
    /* super                     */ &NW_Object_Core_Class,
    /* queryInterface            */ _NW_Object_Core_QueryInterface
  },
  { /* NW_HED_MimeTable          */
    /* numEntries                */ 0,
    /* entries                   */ NULL,
    /* createContentHandler	 */ _NW_HED_MimeTable_CreateContentHandler
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_HED_ContentHandler_t*
_NW_HED_MimeTable_CreateContentHandler (const NW_HED_MimeTable_t* mimeTable,
                                        NW_HED_DocumentNode_t* parent,
                                        NW_Http_ContentTypeString_t contentTypeString,
                                        NW_HED_UrlRequest_t* urlRequest,
                                        NW_Url_Resp_t* response,
                                        NW_Bool isTopLevel)
  {
  NW_ASSERT( mimeTable != NULL );
  NW_ASSERT( parent != NULL );
  NW_ASSERT( urlRequest != NULL );

  NW_Uint32 numEntries;
  NW_Uint16 index;
  NW_Bool  respNoStore = NW_FALSE;
  NW_HED_ContentHandler_t* contentHandler = NULL;

  if (response)
    {
    respNoStore = response->noStore;
    }
  if( NW_HED_MimeTable_IsImageContentType( contentTypeString ) || 
      ( urlRequest->loadType == NW_UrlRequest_Type_Image) ) 
    {
    if( !isTopLevel )
      {
      contentHandler = (NW_HED_ContentHandler_t*)
          NW_Object_New ((NW_Object_Dynamic_Class_t*) &NW_ImageCH_Epoc32ContentHandler_Class, 
          parent, urlRequest, respNoStore);
      }
    }
  else
    {
    // Hotmail bug workaround: 
    // Hotmail sends a response with content type text/vnd.wap.wml, but the response
    // contains script and meta elements
    TUint len = 6; // size of <html>
    if (response != NULL && response->body != NULL && response->body->data != NULL && 
        response->body->length > 6 && response->contentTypeString != NULL)
    {
      TPtrC8 wmlPtr((TUint8*)HTTP_text_vnd_wap_wml_string);
      if (wmlPtr.CompareF(TPtrC8(response->contentTypeString)) == 0)
      {
        _LIT8(KhtmlTag, "<html>");
        TPtr8 ptr(response->body->data, len, len);
        if (ptr.CompareF(KhtmlTag()) == 0)
        {
          contentTypeString = (NW_Uint8*)HTTP_text_html_string;
        }
      }
    }

    // traverse the MimeTable entries one by one looking for a match for
    // the supplied content type 
    numEntries = NW_HED_MimeTable_GetClassPart (mimeTable).numEntries;

    for( index = 0; index < numEntries; index++ ) 
      {
      const NW_HED_MimeTable_Entry_t* entry;
      
      entry = &NW_HED_MimeTable_GetClassPart (mimeTable).entries[index];
      if( ( NW_Asc_stricmp((char*)entry->contentTypeString, (char*)contentTypeString)) == 0	&& 
		      (!isTopLevel || entry->acceptTopLevel)) 
        {
        contentHandler = (NW_HED_ContentHandler_t*)
          NW_Object_New ((NW_Object_Dynamic_Class_t*) entry->objClass, parent, urlRequest, respNoStore);
        break;
        }
      }
    }
  // if no match were found we return NULL 
  return contentHandler;
  }

/* ------------------------------------------------------------------------- */
NW_Bool
NW_HED_MimeTable_IsSupportedContent(NW_HED_MimeTable_t* mimeTable, 
                                   NW_Http_ContentTypeString_t contentTypeString,
                                   NW_Bool isTopLevel)
{
  NW_Uint32 numEntries;
  NW_Uint16 index;

  /* traverse the MimeTable entries one by one looking for a match for
     the supplied content type */
  numEntries = NW_HED_MimeTable_GetClassPart (mimeTable).numEntries;
  for (index = 0; index < numEntries; index++) {
    const NW_HED_MimeTable_Entry_t* entry;

    entry = &NW_HED_MimeTable_GetClassPart (mimeTable).entries[index];
    if ((0 == NW_Asc_stricmp((char*)entry->contentTypeString, (char*)contentTypeString)) && 
		(!isTopLevel || entry->acceptTopLevel)) {
      return NW_TRUE;
    }
  }

  /* if no match were found we return NULL */
  return NW_FALSE;

}

