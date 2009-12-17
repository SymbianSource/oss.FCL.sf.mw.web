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


#include "nw_hed_compositecontenthandleri.h"
#include "nw_hed_documentroot.h"
#include "nw_system_optionlist.h"
#include "nw_image_epoc32simpleimage.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "nwx_multipart_generator.h"
#include "nw_image_epoc32cannedimage.h"
#include "nw_image_cannedimages.h"
#include "HEDDocumentListener.h"
#include "nw_lmgr_rootbox.h"
#include "nw_image_virtualimage.h"
#include "BrsrStatusCodes.h"
#include "nwx_settings.h"
#include <nwx_http_defs.h>
#include "stringutils.h"
#include "TEncodingMapping.h"
#include "bodypart.h"
#include "MemoryManager.h"


/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_HED_CompositeContentHandler_Class_t NW_HED_CompositeContentHandler_Class = {
  { /* NW_Object_Core                 */
    /* super                          */ &NW_HED_ContentHandler_Class,
    /* queryInterface                 */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base                 */
    /* interfaceList                  */ NW_HED_CompositeContentHandler_SecondaryList
  },
  { /* NW_Object_Dynamic              */
    /* instanceSize                   */ sizeof (NW_HED_CompositeContentHandler_t),
    /* construct                      */ _NW_HED_CompositeContentHandler_Construct,
    /* destruct                       */ _NW_HED_CompositeContentHandler_Destruct
  },
  { /* NW_HED_DocumentNode            */
    /* cancel                         */ _NW_HED_CompositeContentHandler_Cancel,
    /* partialLoadCallback	          */ NULL,
    /* initialize                     */ _NW_HED_ContentHandler_Initialize,
    /* nodeChanged                    */ _NW_HED_DocumentNode_NodeChanged,
    /* getBoxTree                     */ _NW_HED_ContentHandler_GetBoxTree,
    /* processEvent                   */ _NW_HED_DocumentNode_ProcessEvent,
    /* handleError                    */ _NW_HED_DocumentNode_HandleError,
    /* suspend                        */ _NW_HED_CompositeContentHandler_Suspend,
    /* resume                         */ _NW_HED_CompositeContentHandler_Resume,
    /* allLoadsCompleted              */ _NW_HED_CompositeContentHandler_AllLoadsCompleted,
    /* intraPageNavigationCompleted   */ _NW_HED_DocumentNode_IntraPageNavigationCompleted,
    /* loseFocus                      */ _NW_HED_CompositeContentHandler_LoseFocus,
    /* gainFocus                      */ _NW_HED_CompositeContentHandler_GainFocus,
    /* handleLoadComplete           */ _NW_HED_DocumentNode_HandleLoadComplete,
  },
  { /* NW_HED_ContentHandler          */  
    /* partialNextChunk               */ _NW_HED_ContentHandler_PartialNextChunk,
    /* getTitle                       */ NULL,
    /* getUrl                         */ _NW_HED_ContentHandler_GetURL,
    /* resolveUrl                     */ _NW_HED_ContentHandler_ResolveURL,
    /* createHistoryEntry             */ _NW_HED_ContentHandler_CreateHistoryEntry,
    /* createIntraHistoryEntry        */ _NW_HED_ContentHandler_CreateIntraDocumentHistoryEntry,
    /* newUrlResponse                 */ _NW_HED_ContentHandler_NewUrlResponse,
    /* createBoxTree                  */ NULL,
    /* handleRequest                  */ _NW_HED_ContentHandler_HandleRequest,
    /* featureQuery                   */ _NW_HED_ContentHandler_FeatureQuery,
    /* responseComplete               */ _NW_HED_ContentHandler_ResponseComplete
  },
  { /* NW_HED_CompositeContentHandler */
    /* documentDisplayed              */ _NW_HED_CompositeContentHandler_DocumentDisplayed
  }
};

/* ------------------------------------------------------------------------- */
const NW_Object_Class_t* const NW_HED_CompositeContentHandler_SecondaryList[] = {
  &NW_HED_CompositeContentHandler_CompositeNode_Class,
  NULL,
};

/* ------------------------------------------------------------------------- */
const NW_HED_CompositeNode_Class_t NW_HED_CompositeContentHandler_CompositeNode_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_HED_CompositeNode_Class,
    /* querySecondary       */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary  */
    /* offset               */ offsetof (NW_HED_CompositeContentHandler_t,
                                         NW_HED_CompositeNode)
  },
  { /* NW_Object_Aggregate  */
    /* secondaryList        */ NULL,
    /* construct            */ NULL,
    /* destruct             */ NULL
  },
  { /* NW_HED_CompositeNode */
    /* addChild             */ _NW_HED_CompositeContentHandler_CompositeNode_AddChild
  }
};

/* ------------------------------------------------------------------------- */
static NW_Int32 NW_HED_CompositeContentHandler_FindCharset(NW_Uint8* charset, NW_Uint32 charsetLength)
{
  NW_Uint32 i = 0;
  NW_Ucs2* supportedCharStr = NULL;

  for (; supportedCharset[i].charset != NULL; i++)
  {
    supportedCharStr = NW_Asc_toUcs2((char*)supportedCharset[i].charset);
    if (charsetLength == NW_Str_Strlen(supportedCharStr)*sizeof(NW_Ucs2) &&
      NW_Byte_Strnicmp((const NW_Byte*)supportedCharStr, (const NW_Byte*)charset, charsetLength) == 0)
    {
      NW_Str_Delete(supportedCharStr);
      return i;
    }
    NW_Str_Delete(supportedCharStr);
  }
  return -1;
}

NW_Int16 NW_HED_CompositeContentHandler_Encoding(NW_Uint8* charset, NW_Uint32 charsetLength)
    {
    NW_Int16 encoding = 0;
    NW_Ucs2* supportedCharStr = NULL;
    StringUtils::ConvertPtrAsciiToUcs2(TPtrC8(charset, charsetLength), &supportedCharStr);
    NW_Int32 index = NW_HED_CompositeContentHandler_FindCharset((NW_Byte*)supportedCharStr, charsetLength * sizeof(NW_Ucs2));
    if (index != -1 )
        {
        encoding = supportedCharset[index].ianaEncoding ;
        }
    delete supportedCharStr;
    return encoding;
    }

NW_Int32 NW_HED_CompositeContentHandler_GetCharsetString(NW_Uint16 aCharset,
                                                     const NW_Uint8** aCharsetString)
    {
    TUint i = 0;
    
    for (; supportedCharset[i].charset != NULL; i++)
        {
        if (supportedCharset[i].ianaEncoding == aCharset)
            {
            *aCharsetString = supportedCharset[i].charset;
            return KErrNone;
            }
        }
    return KErrArgument;
    }

static NW_Int32 NW_HED_CompositeContentHandler_FindCharsetFromInternalEncoding(NW_Uint16 internalEncoding)
{
  NW_Uint32 i = 0;

  // First check if "automatic" encoding mode set on phone, if so return -1.  This is
  // needed to allow the follow on code to determine the encoding from the response.
  if (internalEncoding == EAutomatic)
  {
    return -1;
  }
	
  // If not "automatic" encoding, get the charset encoding that is set on phone
  for (; supportedCharset[i].charset != NULL; i++)
  {
    if (supportedCharset[i].internalEncoding == internalEncoding)
    {
      return i;
    }
  }
  return -1;
}

static NW_Int32 NW_HED_CompositeContentHandler_FindCharsetFromIanaEncoding(NW_Uint16 ianaEncoding)
{
  NW_Uint32 i = 0;
  for (; supportedCharset[i].charset != NULL; i++)
  {
    if (supportedCharset[i].ianaEncoding == ianaEncoding)
    {
      return i;
    }
  }
  return -1;
}


/* ------------------------------------------------------------------------- *
   NW_HED_DocumentNode methods
 * ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_HED_CompositeContentHandler_Construct (NW_Object_Dynamic_t* dynamicObject,
                                             va_list* argp)
    {
    NW_HED_CompositeContentHandler_t* thisObj;
    TBrowserStatusCode status;

    /* for convenience */
    thisObj = NW_HED_CompositeContentHandlerOf (dynamicObject);

    /* invoke our superclass constructor */
    status = _NW_HED_ContentHandler_Construct (dynamicObject, argp);
    if (status != KBrsrSuccess)
        {
        return status;
        }

    thisObj->iMultipart = EFalse;

    return KBrsrSuccess;
    }

// -----------------------------------------------------------------------------
void
_NW_HED_CompositeContentHandler_Destruct (NW_Object_Dynamic_t* dynamicObject)
    {
    NW_HED_CompositeContentHandler_t* thisObj;

    /* cast the 'object' for convenience */
    thisObj = NW_HED_CompositeContentHandlerOf (dynamicObject);


    // multipart related cleanup
    if( thisObj->iMultipart )
        {
        // Free original body
        NW_Buffer_Free( thisObj->iOldResponseBody );

        // clean up body part array and its content
        TInt size = thisObj->iBodyPartArray.Count();
        TInt i;
        for (i = 0; i < size; i++)
            {
            delete thisObj->iBodyPartArray[i];
            }
        thisObj->iBodyPartArray.Reset();
        }
  }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeContentHandler_Cancel (NW_HED_DocumentNode_t* documentNode, 
					NW_HED_CancelType_t cancelType)
{
  NW_HED_CompositeContentHandler_t* thisObj;
  NW_HED_CompositeNode_t* compositeNode;
  NW_ADT_Iterator_t* iterator = NULL;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_CompositeContentHandler_Class));
  
  /* for convenience */
  thisObj = NW_HED_CompositeContentHandlerOf (documentNode);
  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  NW_TRY (status) {
    NW_HED_DocumentNode_t* childNode;

    /* create the child iterator */
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iterator);
    if (status != KBrsrSuccess) {
      if (status != KBrsrNotFound) {
        NW_THROW (status);
      }
    } else {
      /* iterate through the children */
      while (NW_ADT_Iterator_HasMoreElements (iterator)) {
        status = NW_ADT_Iterator_GetNextElement (iterator, &childNode);
        NW_ASSERT (status == KBrsrSuccess);

        /* invoke the child method */
        (void) NW_HED_DocumentNode_Cancel (childNode, cancelType);
      }
    }

    /* invoke our superclass */
   status = NW_HED_ContentHandler_Class.NW_HED_DocumentNode.
      cancel (documentNode, cancelType);
  }
  
  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Object_Delete (iterator);
	return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_CompositeContentHandler_Suspend (NW_HED_DocumentNode_t* documentNode,
                                         NW_Bool aDestroyBoxTree)
{
  NW_HED_CompositeContentHandler_t* thisObj;
  NW_HED_CompositeNode_t* compositeNode;
  NW_ADT_Iterator_t* iterator = NULL;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_CompositeContentHandler_Class));
  
  /* for convenience */
  thisObj = NW_HED_CompositeContentHandlerOf (documentNode);
  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  NW_TRY (status) {
    NW_HED_DocumentNode_t* childNode;

    /* create the child iterator */
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iterator);
    if (status != KBrsrSuccess) {
      if (status != KBrsrNotFound) {
        NW_THROW (status);
      }
    } else {
      /* iterate through the children */
      while (NW_ADT_Iterator_HasMoreElements (iterator)) {
        status = NW_ADT_Iterator_GetNextElement (iterator, &childNode);
        NW_ASSERT (status == KBrsrSuccess);

        /* invoke the child method */
        NW_HED_DocumentNode_Suspend (childNode, aDestroyBoxTree);
      }
    }

    /* invoke our superclass */
    NW_HED_ContentHandler_Class.NW_HED_DocumentNode.
      suspend (documentNode, aDestroyBoxTree);
  }
  
  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Object_Delete (iterator);
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_CompositeContentHandler_Resume (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_CompositeContentHandler_t* thisObj;
  NW_HED_CompositeNode_t* compositeNode;
  NW_ADT_Iterator_t* iterator = NULL;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_CompositeContentHandler_Class));
  
  /* for convenience */
  thisObj = NW_HED_CompositeContentHandlerOf (documentNode);
  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  NW_TRY (status) {
    NW_HED_DocumentNode_t* childNode;

    /* create the child iterator */
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iterator);
    if (status != KBrsrSuccess) {
      if (status != KBrsrNotFound) {
        NW_THROW (status);
      }
    } else {
      /* iterate through the children */
      while (NW_ADT_Iterator_HasMoreElements (iterator)) {
        status = NW_ADT_Iterator_GetNextElement (iterator, &childNode);
        NW_ASSERT (status == KBrsrSuccess);

        /* invoke the child method */
        NW_HED_DocumentNode_Resume (childNode);
      }
    }

    /* invoke our superclass */
    NW_HED_ContentHandler_Class.NW_HED_DocumentNode.
      resume (documentNode);
  }
  
  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Object_Delete (iterator);
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_CompositeContentHandler_AllLoadsCompleted (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_CompositeContentHandler_t* thisObj;
  NW_HED_CompositeNode_t* compositeNode;
  NW_ADT_Iterator_t* iterator = NULL;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_CompositeContentHandler_Class));
  
  /* for convenience */
  thisObj = NW_HED_CompositeContentHandlerOf (documentNode);
  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  NW_TRY (status) {
    NW_HED_DocumentNode_t* childNode;

    /* create the child iterator */
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iterator);
    if (status != KBrsrSuccess) {
      if (status != KBrsrNotFound) {
        NW_THROW (status);
      }
    } else {
      /* iterate through the children */
      while (NW_ADT_Iterator_HasMoreElements (iterator)) {
        status = NW_ADT_Iterator_GetNextElement (iterator, &childNode);
        NW_ASSERT (status == KBrsrSuccess);

        /* invoke the child method */
        NW_HED_DocumentNode_AllLoadsCompleted (childNode);
      }
    }

    /* invoke our superclass */
    NW_HED_ContentHandler_Class.NW_HED_DocumentNode.
      allLoadsCompleted (documentNode);
  }
  
  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Object_Delete (iterator);
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_CompositeContentHandler_LoseFocus (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_CompositeContentHandler_t* thisObj;
  NW_HED_CompositeNode_t* compositeNode;
  NW_ADT_Iterator_t* iterator = NULL;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_CompositeContentHandler_Class));
  
  /* for convenience */
  thisObj = NW_HED_CompositeContentHandlerOf (documentNode);
  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  NW_TRY (status) {
    NW_HED_DocumentNode_t* childNode;

    /* create the child iterator */
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iterator);
    if (status != KBrsrSuccess) {
      if (status != KBrsrNotFound) {
        NW_THROW (status);
      }
    } else {
      /* iterate through the children */
      while (NW_ADT_Iterator_HasMoreElements (iterator)) {
        status = NW_ADT_Iterator_GetNextElement (iterator, &childNode);
        NW_ASSERT (status == KBrsrSuccess);

        /* invoke the child method */
        NW_HED_DocumentNode_LoseFocus (childNode);
      }
    }

    /* invoke our superclass */
    NW_HED_ContentHandler_Class.NW_HED_DocumentNode.
      loseFocus (documentNode);
  }
  
  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Object_Delete (iterator);
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
void
_NW_HED_CompositeContentHandler_GainFocus (NW_HED_DocumentNode_t* documentNode)
{
  NW_HED_CompositeContentHandler_t* thisObj;
  NW_HED_CompositeNode_t* compositeNode;
  NW_ADT_Iterator_t* iterator = NULL;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (documentNode,
                                     &NW_HED_CompositeContentHandler_Class));
  
  /* for convenience */
  thisObj = NW_HED_CompositeContentHandlerOf (documentNode);
  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  NW_TRY (status) {
    NW_HED_DocumentNode_t* childNode;

    /* create the child iterator */
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iterator);
    if (status != KBrsrSuccess) {
      if (status != KBrsrNotFound) {
        NW_THROW (status);
      }
    } else {
      /* iterate through the children */
      while (NW_ADT_Iterator_HasMoreElements (iterator)) {
        status = NW_ADT_Iterator_GetNextElement (iterator, &childNode);
        NW_ASSERT (status == KBrsrSuccess);

        /* invoke the child method */
        NW_HED_DocumentNode_GainFocus (childNode);
      }
    }

    /* invoke our superclass */
    NW_HED_ContentHandler_Class.NW_HED_DocumentNode.
      gainFocus (documentNode);
  }
  
  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Object_Delete (iterator);
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeContentHandler_DetachChildren (NW_HED_CompositeContentHandler_t* thisObj)
{
  NW_HED_CompositeNode_t* compositeNode;
  NW_ADT_Iterator_t* iterator = NULL;
  
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj,
                                     &NW_HED_CompositeContentHandler_Class));
  
  /* for convenience */
  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  NW_TRY (status) {
    NW_HED_DocumentNode_t* childNode;

    /* create the child iterator */
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iterator);
    if (status == KBrsrNotFound) {
      NW_THROW_SUCCESS (status);
    }
    NW_THROW_ON_ERROR (status);

    /* iterate through the children */
    while (NW_ADT_Iterator_HasMoreElements (iterator)) {
      status = NW_ADT_Iterator_GetNextElement (iterator, &childNode);
      NW_ASSERT (status == KBrsrSuccess);

      /* detach the child */
      if (NW_HED_ContentHandlerOf (childNode)->boxTree != NULL) {
        (void) NW_LMgr_Box_Detach (NW_HED_ContentHandlerOf (childNode)->boxTree);
      }
    }
  }
  
  NW_CATCH (status) { /* empty */ }

  NW_FINALLY {
    NW_Object_Delete (iterator);
    return status;
  } NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeContentHandler_DocumentDisplayed (NW_HED_ContentHandler_t* thisObj)
{
  NW_REQUIRED_PARAM(thisObj);

  /* Default implementation does nothing. */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */

static
NW_Bool
_NW_HED_CompositeContentHandler_DetectEncoding(NW_Buffer_t* pInBuf,
                                               NW_Uint16* encoding,
                                               NW_Uint8* byteOrder)
{
  /*
    The goal here is to permit the content author to do something
    which is within current specifications and can be depended on to
    work.
 
    Despite of the charset in response header, we apply the following 
    constraint to determine the charset:
 
    Content authors must place a byte order mark (BOM) at the
    beginning of the document only if the encoding is UCS-2 or UTF-8.

    If the first bytes are:
    0xFE 0xFF ## ## (big endian) or 0xFF 0xFE ## ## (little endian), where the
    two ## bytes are not both zero then the encoding is UCS-2.
    (UCS-2 is roughly a subset of UTF-16 in our use, aka Unicode)
 
    0xEF 0xBB 0xBF then the encoding is UTF-8
 
    otherwise the encoding is left as Latin-1.
 
    Differentiating ASCII from Latin-1 is never an issue because it is
    a subset of Latin-1 which is the default.
 
    You can find a discussion of this type of encoding detection at
    http://www.w3.org/TR/2000/REC-xml-20001006#sec-guessing
  */

  *byteOrder = NW_BYTEORDER_SINGLE_BYTE;

  if ((pInBuf == NULL) || (pInBuf->data == NULL))
  {
    return NW_FALSE;
  }

  if (pInBuf->length >= 3)
  {
    if (((pInBuf->data)[0] == 0xef)
        && ((pInBuf->data)[1] == 0xbb)
        && ((pInBuf->data)[2] == 0xbf))
    {
      *encoding = HTTP_utf_8;
      *byteOrder = NW_BYTEORDER_SINGLE_BYTE;
      return NW_TRUE;
    }
  }

  if (pInBuf->length >= 4)
  {
    if (((pInBuf->data)[0] == 0xfe) && ((pInBuf->data)[1] == 0xff))
    {
      *encoding = HTTP_iso_10646_ucs_2;
      *byteOrder = NW_BYTEORDER_BIG_ENDIAN;
      // The order is suppose to be big-endian but check to see if it looks
      // like little-endian.
      if (((pInBuf->data)[2] != 0) && ((pInBuf->data)[3] == 0))
      {
        *byteOrder = NW_BYTEORDER_LITTLE_ENDIAN;
      }
      return NW_TRUE;
    }        
    if (((pInBuf->data)[0] == 0xff) && ((pInBuf->data)[1] == 0xfe))
    {
      *encoding = HTTP_iso_10646_ucs_2;
      *byteOrder = NW_BYTEORDER_LITTLE_ENDIAN;
      // The order is suppose to be little-endian but check to see if it looks
      // like big-endian.
      if (((pInBuf->data)[2] == 0) && ((pInBuf->data)[3] != 0))
      {
        *byteOrder = NW_BYTEORDER_BIG_ENDIAN;
      }
      return NW_TRUE;
    }
  }

  // Special case: content authors write a ucs-2 document but do not place BOM
  // at beginning of document. If first two chars (4 bytes) look like ucs-2
  // then set encoding to ucs-2 and check the byte-order. Note, this is not
  // exact. It looks for zero bytes followed by non-zero bytes or vice-versa.
  // It is highly probably that if these conditions are met then the contents
  // are in fact ucs-2. However, it is theoretically possible for both of the
  // first two chars to not have a zero-byte but still be ucs-2. If that were
  // to occur this logic would not be able to determine that it is ucs-2 and
  // would end up returning FALSE, indicating encoding not found.
  if (pInBuf->length >= 4)
  {
    // Check to see if it looks like big-endian ucs-2.
    if ((((pInBuf->data)[0] == 0) && ((pInBuf->data)[1] != 0)) &&
         (((pInBuf->data)[2] == 0) && ((pInBuf->data)[3] != 0)))
    {
      *encoding = HTTP_iso_10646_ucs_2;
      *byteOrder = NW_BYTEORDER_BIG_ENDIAN;
      return NW_TRUE;
    }
    // Check to see if it looks like little-endian ucs-2.
    if ((((pInBuf->data)[0] != 0) && ((pInBuf->data)[1] == 0)) &&
         (((pInBuf->data)[2] != 0) && ((pInBuf->data)[3] == 0)))
    {
      *encoding = HTTP_iso_10646_ucs_2;
      *byteOrder = NW_BYTEORDER_LITTLE_ENDIAN;
      return NW_TRUE;
    }
  }

  return NW_FALSE;
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeContentHandler_ConvertEncoding(NW_HED_CompositeContentHandler_t* thisObj,
                                                NW_Uint32 foreignEncoding, 
                                                NW_Buffer_t* inBuf, 
                                                NW_Int32* numUnconvertible, 
                                                NW_Int32* indexFirstUnconvertible, 
                                                NW_Buffer_t** outBuf)                       
{
  TBrowserStatusCode status = KBrsrSuccess;
  NW_HED_DocumentRoot_t* docRoot;
  NW_HED_AppServices_t* appServices;

  docRoot = (NW_HED_DocumentRoot_t*)NW_HED_DocumentNode_GetRootNode(thisObj);
  appServices = (NW_HED_AppServices_t *) NW_HED_DocumentRoot_GetAppServices(docRoot);
  status = appServices->characterConversionAPI.convertFromForeignChunk(NW_HED_DocumentRoot_GetBrowserAppCtx(docRoot), 
              foreignEncoding, inBuf, (TInt *)numUnconvertible, (TInt*)indexFirstUnconvertible, (void**)outBuf);
  if (status == KBrsrSuccess && *outBuf == NULL)
    *outBuf = inBuf;
  return status;
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeContentHandler_ComputeEncoding(NW_HED_CompositeContentHandler_t* thisObj,
                                                NW_Url_Resp_t* response,
                                                NW_Bool* encodingFound)
{  
  TBrowserStatusCode status = KBrsrSuccess;
  NW_Uint16 settingEncoding = NW_Settings_GetEncoding();
  NW_Int32 index = NW_HED_CompositeContentHandler_FindCharsetFromInternalEncoding(settingEncoding);
  NW_ASSERT(encodingFound != NULL);
  NW_REQUIRED_PARAM(thisObj);
  // If the setting is not automatic, use the charset from the settings
  if (index >= 0)
  {
    response->charset = supportedCharset[index].ianaEncoding;
    *encodingFound = NW_TRUE;
  }
  else
  {
    *encodingFound = _NW_HED_CompositeContentHandler_DetectEncoding(
	  response->body, &response->charset, &response->byteOrder);
    if (*encodingFound)
	{
	  NW_Settings_SetOriginalEncoding(response->charset);
	}
  }
  
  return status;
}

// take a url repsonse, and try to find to find its charset in http header,
// if not found, use default locale charset, if that's not supported,
// use latin-1 as default. 
// also set the response charset to whatever is determined
// finally returns the index of the charset.
static NW_Int32 findDefaultCharsetIndex(NW_Url_Resp_t * response)
{
  NW_Int32 index = -1;
  if (response->charsetString != NULL)
  {
    index = NW_HED_CompositeContentHandler_FindCharsetFromIanaEncoding(response->charset);
  }
  if (index < 0)
  {
    // No valid http header was found, use the locale specific default
    index = NW_HED_CompositeContentHandler_FindCharsetFromInternalEncoding(NW_Settings_GetDefaultCharset());
  }
  if (index < 0)
  {
    index = NW_HED_CompositeContentHandler_FindCharsetFromInternalEncoding(HTTP_iso_8859_1);
  }
  response->charset = supportedCharset[index].ianaEncoding;
  NW_Settings_SetOriginalEncoding(response->charset);
  return index;
}

// this function serves 3 purposes: 
// 1. when charset is found in META or XML, 
//             charsetLength and charsetOffset not zero,
//             (Mainly called from handle_Meta or handle_xml_charset in parser).
// 2. when encoding is set auto, and charset not found in META or XML, 
//             charsetLength and charsetOffset would be zero, selectedCharset should be 0;
//             (Mainly called from BodyStart in parser).
// 3. when encoding is set auto, and parser needs to perform the char conv based on previous found charset.
//             charsetLength and charsetOffset would be zero, selectedCharset is not 0;
//             also, in this case, we should expect the response->charset has been set to
//             the correct value.
//             (Mainly called from htmlp_to_wbxml function to perform transcoding).
TBrowserStatusCode NW_HED_CompositeContentHandler_CharConvCB(void* context,
                                                             NW_Uint32 charsetLength,
                                                             NW_Uint32 charsetOffset,
                                                             NW_Buffer_t* body, 
                                                             NW_Int32* numUnconvertible, 
                                                             NW_Int32* indexFirstUnconvertible, 
                                                             NW_Buffer_t** outBuf,
                                                             NW_Uint32* selectedCharset)
{
  NW_HED_CharsetConvContext* ctx = NULL;
  NW_Int32 index = -1;
  TBrowserStatusCode status = KBrsrSuccess;
  
  ctx = (NW_HED_CharsetConvContext*) context;
  NW_ASSERT(ctx != NULL);
  NW_ASSERT(ctx->contentHandler != NULL);
  NW_ASSERT(ctx->response != NULL);
  NW_ASSERT(body != NULL);
  
  if (charsetLength == 0 && charsetOffset == 0)
  {
    if (*selectedCharset == NULL)
    {
      // case 2:
      // We are here because the Encoding setting is automatic and there was no Meta tag or XML declaration
      // Look for Http header, but only if it was set by the server and we can support it,
      // if not, we'll use default encoding
      index = findDefaultCharsetIndex(ctx->response);
    }
    else
    {
      // case 3: use selectedCharset to set response->charset
      ctx->response->charset = (NW_Uint16)*selectedCharset; 
      status = _NW_HED_CompositeContentHandler_ConvertEncoding(ctx->contentHandler, ctx->response->charset, 
        body, numUnconvertible, indexFirstUnconvertible, 
        outBuf);
    }
  }
  else // case 1: find the charset from the META or XML 
  {
    index = NW_HED_CompositeContentHandler_FindCharset(body->data + charsetOffset, charsetLength);
    if (index < 0)
    {
      index = findDefaultCharsetIndex(ctx->response);
    }
    else{
      ctx->response->charset = supportedCharset[index].ianaEncoding;
    }
  }
  *selectedCharset = ctx->response->charset;
  NW_Settings_SetOriginalEncoding(ctx->response->charset);
  return status;
}

/************************************************************************
 
  Function: NW_HED_CompositeContentHandler_SaveImagesToFile

  Description:  Write images to a file in the form of multipart segments.
                Each segment consists of:
                      header length (including content type length)
                      image data length
                      content type (including character encoding)
                      headers
                      image
 
  Parameters:   thisObj  - in: content handler
                file     - in: file name
    
  Return Value: KBrsrSuccess
                KBrsrFailure
                KBrsrOutOfMemory
                KBrsrSavedPageFailed
                  
**************************************************************************/
TBrowserStatusCode
_NW_HED_CompositeContentHandler_SaveImagesToFile(NW_HED_CompositeContentHandler_t* thisObj,
                                                 NW_Osu_File_t fh)
{
  TBrowserStatusCode status = KBrsrFailure;
  NW_HED_CompositeNode_t* compositeNode;
  NW_HED_ContentHandler_t* childNode;
  NW_ADT_Iterator_t* iter = NULL;
  NW_Image_Epoc32Simple_t* image = NULL;
  const NW_Byte* rawData = NULL;
  NW_Uint32 rawDataLen = 0;
  NW_Byte* segment;
  NW_Uint32 segmentLen;
  char* urlAscii = NULL;
  NW_Image_CannedImages_t     *cannedImages;
  NW_LMgr_RootBox_t           *rootBox;
  MHEDDocumentListener    *documentListener;
  NW_HED_DocumentRoot_t         *documentRoot;
        

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj,
                                     &NW_HED_CompositeContentHandler_Class));

  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  TUint originalEncoding = NW_Settings_GetOriginalEncoding();
  
  /* Create a child iterator */
  if (thisObj != NULL)
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iter);

  /* If there are no children, return success status */
  if (status == KBrsrNotFound)
    return KBrsrSuccess;
  
  if (status == KBrsrSuccess)
  {
    /* Loop through the child content handlers; each contatins an image. */
    while (NW_ADT_Iterator_HasMoreElements (iter))
    {
      NW_ImageCH_Epoc32ContentHandler_t* imageCH;

      status = NW_ADT_Iterator_GetNextElement (iter, &childNode);

      if (status != KBrsrSuccess)
      {
        status = KBrsrFailure;
        break;
      }

      if (!NW_Object_IsClass(childNode, &NW_ImageCH_Epoc32ContentHandler_Class))
        continue;

      imageCH = NW_ImageCH_Epoc32ContentHandlerOf(childNode);

      if (imageCH->image != NULL)
      {
        if (NW_Object_IsInstanceOf(imageCH->image, &NW_Image_Virtual_Class))
          continue;

        image = NW_Image_Epoc32SimpleOf(imageCH->image);
        rawData = (const NW_Byte *)image->rawData;
        rawDataLen = image->rawDataLength;
      }
      else
      {        
        documentRoot =
          (NW_HED_DocumentRoot_t*) NW_HED_DocumentNode_GetRootNode (thisObj);
        NW_ASSERT(documentRoot != NULL);
        
        documentListener = documentRoot->documentListener;
        NW_ASSERT(documentListener != NULL);
        
        rootBox = documentListener->GetRootBox();
        if (rootBox==NULL) {
          return KBrsrOutOfMemory;
        }
        
        cannedImages = rootBox->cannedImages;
        NW_ASSERT(cannedImages!=NULL);
        image = NULL;
        image = (NW_Image_Epoc32Simple_t *)
          NW_Image_CannedImages_GetImage(cannedImages, NW_Image_Missing);
        NW_ASSERT(image != NULL);

        rawData = (const NW_Byte *)image->rawData;
        rawDataLen = image->rawDataLength;
      }

      /* Get response URL. If it is NULL, use it anyway. */
      if (imageCH->requestUrl)
        urlAscii = NW_Str_CvtToAscii(imageCH->requestUrl);

      status = NW_CreateMultipartSegment
                  (
                    HTTP_iso_8859_1, // encoding - does it matter for images?
					originalEncoding, // used by saved foreign content loading
                    childNode->response->contentTypeString,
                    urlAscii,
                    rawData,
                    rawDataLen,
                    &segment,
                    &segmentLen
                  );
      NW_Mem_Free(urlAscii);
      if (status != KBrsrSuccess)
        break;

      /* write segment to file */
      status = NW_Osu_WriteFile(fh, segment, segmentLen);
      NW_Mem_Free(segment);
      if (status != KBrsrSuccess)
        break;
    }
    
    NW_Object_Delete (iter);
  }
  return status;
}


/************************************************************************
 
  Function: NW_HED_CompositeContentHandler_GetImageCount

  Description:  Gets the number of images embedded in the document.
  This does not include the images loaded via CSS
 
  Parameters:   thisObj  - in: content handler
    
  Return Value: Number of images
                  
**************************************************************************/
NW_Uint32
NW_HED_CompositeContentHandler_GetImageCount(NW_HED_CompositeContentHandler_t* thisObj)
{
  TBrowserStatusCode status = KBrsrFailure;
  NW_HED_CompositeNode_t* compositeNode;
  NW_HED_ContentHandler_t* childNode;
  NW_ADT_Iterator_t* iter = NULL;
  NW_Uint32 numImages = 0;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj,
                                     &NW_HED_CompositeContentHandler_Class));

  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary (thisObj, &NW_HED_CompositeNode_Class);
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));

  /* Create a child iterator */
  if (thisObj != NULL)
    status = NW_HED_CompositeNode_GetChildren (compositeNode, &iter);

  /* If there are no children, return success status */
  if (status == KBrsrNotFound)
    return 0;
  
  if (status == KBrsrSuccess)
  {
    /* Loop through the child content handlers; each contatins an image. */
    while (NW_ADT_Iterator_HasMoreElements (iter))
    {
      NW_ImageCH_Epoc32ContentHandler_t* imageCH;

      status = NW_ADT_Iterator_GetNextElement (iter, &childNode);

      if (status != KBrsrSuccess)
        break;

      if (!NW_Object_IsClass(childNode, &NW_ImageCH_Epoc32ContentHandler_Class))
        continue;

      imageCH = NW_ImageCH_Epoc32ContentHandlerOf(childNode);

      if (imageCH->image != NULL)
      {
        if (!NW_Object_IsInstanceOf(imageCH->image, &NW_Image_Virtual_Class))     
          numImages++;
      }
    }
  }
  return numImages;
}

                                           
/************************************************************************
 
  Function: NW_HED_CompositeContentHandler_AddChild

  Description:  Add a child to the list
 
  Parameters:   thisObj - in: composite content handler
                childNode - in: child to be added
    
  Return Value: 
                  
**************************************************************************/
TBrowserStatusCode
NW_HED_CompositeContentHandler_AddChild( NW_HED_CompositeContentHandler_t* thisObj,
                                         NW_HED_DocumentNode_t* childNode, 
                                         void* context )
  {
  NW_HED_CompositeNode_t* compositeNode;
  
  // parameter assertion block 
  NW_ASSERT( NW_Object_IsInstanceOf( thisObj, &NW_HED_CompositeContentHandler_Class ) );
  NW_ASSERT( NW_Object_IsInstanceOf( childNode, &NW_HED_ContentHandler_Class ) );
  
  // for convenience 
  compositeNode = (NW_HED_CompositeNode_t *)
    NW_Object_QuerySecondary( thisObj, &NW_HED_CompositeNode_Class );
  
  NW_ASSERT( NW_Object_IsInstanceOf( compositeNode,
    &NW_HED_CompositeContentHandler_CompositeNode_Class ) );

  NW_TRY( status ) 
    {
    // insert the childNode into our children list
    status = NW_HED_CompositeNode_AddChild( compositeNode, childNode, context );
    _NW_THROW_ON_ERROR( status );
    }
  NW_CATCH( status ) 
    { 
    }
  NW_FINALLY 
    {
    return status;
    } 
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- *
   NW_HED_CompositeNode methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_HED_CompositeContentHandler_CompositeNode_AddChild (NW_HED_CompositeNode_t* compositeNode,
                                                        NW_HED_DocumentNode_t* childNode,
                                                        void* key)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (compositeNode,
                                     &NW_HED_CompositeContentHandler_CompositeNode_Class));
  NW_ASSERT (NW_Object_IsInstanceOf (childNode, &NW_HED_DocumentNode_Class));

  /* if the content handler is not embeddable do not add it.There is this
     prevents nested <html> tags created inadvertently */
  if (NW_HED_ContentHandlerOf (childNode)->isEmbeddable == NW_FALSE) {
    return KBrsrBadInputParam;
  }

  return _NW_HED_CompositeNode_AddChild (compositeNode, childNode, key);
}


/*****************************************************************

  Name:         NW_HED_CompositeContentHandler_HandleImageLoad

  Description:  If the image corresponsing to the newContentHandler
                (which is an ImageCH) exists more than once in the document,
                this function creates new ImageCH for all those other
                transactions and sets Virtual Image in them

  Parameters:   See below

  Return Value:

*****************************************************************/
TBrowserStatusCode
NW_HED_CompositeContentHandler_HandleImageLoad(
           NW_HED_CompositeContentHandler_t* thisObj,
           NW_Int32 chunkIndex,
					 NW_HED_ContentHandler_t* newContentHandler,
           NW_HED_UrlRequest_t* urlRequest )
  {
  // note that this function should be in a utility class instead
  NW_TRY( status )
    {
    NW_ADT_Vector_Metric_t index;
    NW_ADT_Vector_Metric_t numItems;
    // get document root load list
    NW_HED_DocumentRoot_t* docRoot = (NW_HED_DocumentRoot_t*)
      _NW_HED_DocumentNode_GetRootNode( NW_HED_DocumentNodeOf( thisObj ) );

    // handle unlikely error cases
    NW_THROW_ON_NULL( newContentHandler, status, KBrsrUnexpectedError );
    NW_THROW_ON_NULL( newContentHandler->response, status, KBrsrUnexpectedError );

    numItems = NW_ADT_Vector_GetSize( docRoot->loadList );
    index = numItems;

    while (index > 0)
      {
      NW_HED_DocumentRoot_LoadContext_t *entry = NULL;

      index--;
      entry = *(NW_HED_DocumentRoot_LoadContext_t**)
        NW_ADT_Vector_ElementAt( docRoot->loadList, index );

      NW_ASSERT( entry );
      // find all the corresponding virtual entries.
      if( ( entry ) && ( entry->virtualRequest == NW_TRUE ) &&
          ( entry->transactionId == newContentHandler->response->transId ) )
        {
        NW_ImageCH_Epoc32ContentHandler_t* virtualHandler = NULL;

        // first chunk. let's create the virtual image content handler
        if( chunkIndex == 0 )
          {
          // clone image handler
          status = NW_ImageCH_Epoc32ContentHandler_GetVirtualHandler(
            NW_HED_DocumentNodeOf( thisObj ), urlRequest,
            NW_ImageCH_Epoc32ContentHandlerOf( newContentHandler ), &virtualHandler );
          // check only OOM
          NW_THROW_ON( KBrsrOutOfMemory, status );

          if( virtualHandler &&
              NW_HED_CompositeContentHandler_AddChild( NW_HED_CompositeContentHandlerOf( thisObj ),
                   NW_HED_DocumentNodeOf( virtualHandler ), entry->clientData ) == KBrsrSuccess )
            {
            // set the context
            virtualHandler->context = entry->clientData;
            }
          else
            {
            // if the virtual content handler is either NULL or could not be added to the child list
            // then erase and set it to NULL.
            NW_Object_Delete( virtualHandler );
            virtualHandler = NULL;
            // release entry
            NW_Mem_Free( entry );
            // remove it from the list
            NW_ADT_DynamicVector_RemoveAt( docRoot->loadList, index );
            // catch OOM only
            NW_THROW_ON( KBrsrOutOfMemory, status );
            }
          // jump to the next candidate
          }
        // last chunk, load complete
        else if( chunkIndex == -1 )
          {
          NW_HED_CompositeNode_t* compositeNode;

          // look up the child list to get the virtual content handler
          compositeNode = (NW_HED_CompositeNode_t*)
            (NW_Object_QueryAggregate( thisObj, &NW_HED_CompositeNode_Class));
          NW_ASSERT( compositeNode != NULL );

          virtualHandler = (NW_ImageCH_Epoc32ContentHandler_t*)
            NW_HED_CompositeNode_LookupChild( compositeNode, entry->clientData );

          if( virtualHandler )
            {
            status = NW_HED_DocumentNode_Initialize( NW_HED_DocumentNodeOf( virtualHandler ), KBrsrSuccess );
            // catch OOM only
            NW_THROW_ON( KBrsrOutOfMemory, status );
            }
          NW_Mem_Free( entry );
          // load complet. remove virtual item from the loadList
          status = NW_ADT_DynamicVector_RemoveAt( docRoot->loadList, index );
          // catch OOM only
          NW_THROW_ON( KBrsrOutOfMemory, status );
          }
        else
          {
          // only the first chunk and the load complete should
          // be passed to the function
          NW_ASSERT( NW_TRUE );
          }
        }
      }
    }
  NW_CATCH( status )
    {
    }
  NW_FINALLY
    {
    return status;
    }
  NW_END_TRY
  }

/* ------------------------------------------------------------------------- */
/* NW_HED_CompositeContentHandler_CreateContentHandler
 *
 * Creates non top level content handler for a given context
 * Parameters:
 *  image:     this object
 */
NW_HED_ContentHandler_t*
NW_HED_CompositeContentHandler_CreateContentHandler( NW_HED_ContentHandler_t* thisObj,
                                            NW_Uint8* contentTypeString, NW_HED_UrlRequest_t* urlRequest, 
                                            NW_Url_Resp_t* response, void* clientData )
           
  {
  NW_HED_ContentHandler_t* newContentHandler;
  NW_HED_DocumentRoot_t* docRoot;

  // parameter assertion block 
  NW_ASSERT( thisObj != NULL );
  NW_ASSERT( urlRequest != NULL );
  NW_ASSERT( contentTypeString != NULL );

  // it musn't be top level request
  NW_ASSERT( NW_HED_UrlRequest_IsTopLevelRequest( urlRequest ) != NW_TRUE );

  docRoot = (NW_HED_DocumentRoot_t*)
    _NW_HED_DocumentNode_GetRootNode( NW_HED_DocumentNodeOf( thisObj ) );

    newContentHandler = NW_HED_MimeTable_CreateContentHandler( docRoot->mimeTable, NW_HED_DocumentNodeOf( thisObj ), 
      contentTypeString, urlRequest, response, NW_FALSE );
  
  // if this is null we don't support the content-type of the response 
  if( newContentHandler != NULL )
    {
    TBrowserStatusCode status;
    // add content handler to the children list
    status = NW_HED_CompositeContentHandler_AddChild( NW_HED_CompositeContentHandlerOf( thisObj ), 
      NW_HED_DocumentNodeOf( newContentHandler ), clientData );   

    // the content handler initalized successfully set "contentHandler" 
    if( status != KBrsrSuccess )
      {
      // delete the bogus content handler
      NW_Object_Delete( newContentHandler );      
      newContentHandler = NULL;
      }
    }
  return newContentHandler;
  }


// check if any body part has the same url as the one passed in
// if it does, return true, set aBodyPart to the body part 
// else, return false
TBool
_NW_HED_CompositeContentHandler_IsUrlInMultipart( NW_HED_CompositeContentHandler_t* thisObj,
                                                  const NW_Ucs2* aUrl,
                                                  CBodyPart** aBodyPart )
    {
    TInt32 bodyPartLen;

    if( thisObj->iMultipart )
        {
        TInt size = thisObj->iBodyPartArray.Count();
        TInt i;
        TInt32 len = NW_Str_Strlen( aUrl );
        for (i = 0; i < size; i++)
            {
            CBodyPart* bodyPart = thisObj->iBodyPartArray[i];
            NW_Ucs2* bodyPartUrl = (NW_Ucs2*)( bodyPart->Url().Ptr() );

            if( bodyPartUrl )
                {
                bodyPartLen = NW_Str_Strlen( bodyPartUrl );

                if( bodyPartLen == len )
                    {
                    if( NW_Str_Strcmp( bodyPartUrl, aUrl ) == 0 )
                        {
                        //aBodyPart = &bodyPart;
                        *aBodyPart = bodyPart;
                        return ETrue;
                        }
                    // else: continue loop
                    }
                // else: continue loop
                }
            // else: continue loop
            }
        }
    // else: return false

    return EFalse;
    }

