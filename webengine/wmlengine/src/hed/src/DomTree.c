/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nw_hed_domtree.h"
#include <nw_tinydom_utils.h>
#include "nw_object_exceptions.h"
#include "nwx_statuscodeconvert.h"
// #include <nw_string_string.h>

struct NW_HED_DomTree_s {
  Parser_t          domParser;
  NW_Buffer_t*      domBuffer;
};

/* 
 * 
 *
 *
 */

static TBrowserStatusCode
NW_HED_Write_WBXML_StrTbl(Parser_t *p, void* WBXMLEncStrTbl)
{
     TBrowserStatusCode status = KBrsrSuccess;
     NW_Uint32 stringTableByteCount = 0;
     NW_Encoder_StringTableIterator_t strTableIterator;
     NW_Status_t s = NW_STAT_FAILURE;
     NW_Uint8* pLiteralBuf;
     NW_String_t pLiteralStr;
     NW_Uint32 docTableIndex;
     NW_Encoder_StringTable_t* strTable = NULL;
     NW_WBXML_Document_t* doc = &(p->document);
     
     
         

     strTable = (NW_Encoder_StringTable_t*) WBXMLEncStrTbl;

     stringTableByteCount = NW_Encoder_StringTable_getTotalBytes(strTable);
     
     
     if(stringTableByteCount > 0)
     {
        /*The following function always return success so why bother to check? */

        NW_Encoder_StringTable_StringTableIterateInit(WBXMLEncStrTbl,&strTableIterator);


        s =  NW_Encoder_StringTable_StringTableIterateNext
                    (&strTableIterator,&stringTableByteCount,&pLiteralBuf);

        while(s == NW_STAT_WBXML_ITERATE_MORE)
        {

         pLiteralStr.length = stringTableByteCount;
         pLiteralStr.storage = pLiteralBuf;

         status = StatusCodeConvert( NW_WBXML_Document_putTableString(
                                      doc,&pLiteralStr,&docTableIndex) );

         if(status != KBrsrSuccess)
         {
          break;
         }

           //Just a sanity check

         /*
           if(WBXMLwriterPrevIndex != docTableIndex)
           {
            status = KBrsrFailure;
            break;
           }
         
             WBXMLwriterPrevIndex += pLiteralStr.length;
        */
        

           s =  NW_Encoder_StringTable_StringTableIterateNext
                    (&strTableIterator,&stringTableByteCount,&pLiteralBuf) ;

        }//end while

     }//end if(stringTableByteCount > 0)

     //Delete the tree

     NW_Encoder_StringTable_delete(WBXMLEncStrTbl);
     WBXMLEncStrTbl = NULL;
 return status;
}//end NW_HED_Write_WBXML_StrTbl()

/* ------------------------------------------------------------------------- */
NW_HED_DomTree_t* NW_HED_DomTree_New (NW_Buffer_t* domBuffer,
                                      NW_Http_CharSet_t charset,
                                      NW_Uint32 defaultPublicId,
                                      void* WBXMLEncStrTbl)
{
  NW_HED_DomTree_t* domTree;

  NW_ASSERT(domBuffer != NULL);
  NW_REQUIRED_PARAM(charset);

  NW_TRY(status) {
    /* create the dom-tree */
    domTree = (NW_HED_DomTree_t*) NW_Mem_Malloc(sizeof(NW_HED_DomTree_t));
    NW_THROW_OOM_ON_NULL(domTree, status);
    NW_Mem_memset(domTree, 0, sizeof(NW_HED_DomTree_t));

    /* init the parser */
    status = StatusCodeConvert(NW_TinyDom_ParserInitialize(&domTree->domParser, NULL, 
                                         0, defaultPublicId));
    NW_THROW_ON_ERROR(status);

    /*If the WBXML encoder table is present then write these contents to
     * extension table.
     */

    if(WBXMLEncStrTbl)
    {
     status = NW_HED_Write_WBXML_StrTbl(&domTree->domParser, WBXMLEncStrTbl);
     NW_THROW_ON_ERROR(status);
    }//end if(WBXMLEncStrTbl)

    /* build the dom-tree */
    status = StatusCodeConvert(NW_TinyDom_MakeDOMTree(&domTree->domParser, 
                                    domBuffer->data, domBuffer->length, NULL));
    NW_THROW_ON_ERROR(status);
    domTree->domBuffer = domBuffer; 
  }
  NW_CATCH(status) {
    if (domTree != NULL) {
      NW_TinyDom_ParserDelete(&domTree->domParser);
    }
    NW_Mem_Free(domTree);
    NW_Buffer_Free(domBuffer);
    domTree = NULL;
  }
  NW_FINALLY {
    return domTree;
  }
  NW_END_TRY
}

/* ------------------------------------------------------------------------- */
TBrowserStatusCode NW_HED_DomTree_Append (NW_HED_DomTree_t** ppDomTree,
                                      NW_Buffer_t* domBuffer,
                                      NW_Http_CharSet_t charset,
                                      NW_Uint32 defaultPublicId,
                                      NW_Uint32 cp_count,
                                      NW_Int32 lastValid,
                                      void* WBXMLEncStrTbl)
{
  NW_HED_DomTree_t* domTree;
  
  NW_ASSERT(domBuffer != NULL);
  NW_ASSERT(ppDomTree != NULL);
  NW_REQUIRED_PARAM(charset);

  domTree = *ppDomTree;

  NW_TRY(status) {
    if (domTree == NULL)
    {
      /* create the dom-tree */
      domTree = (NW_HED_DomTree_t*) NW_Mem_Malloc(sizeof(NW_HED_DomTree_t));
      NW_THROW_OOM_ON_NULL(domTree, status);
      NW_Mem_memset(domTree, 0, sizeof(NW_HED_DomTree_t));
      
      /* init the parser */
      status = StatusCodeConvert(NW_TinyDom_ParserInitialize(&domTree->domParser, NULL, 
        0, defaultPublicId));
      NW_THROW_ON_ERROR(status);
      
    }

    /*If the WBXML encoder table is present then write these contents to
     * extension table.
     */

    if(WBXMLEncStrTbl)
    {
     status = NW_HED_Write_WBXML_StrTbl(&domTree->domParser,WBXMLEncStrTbl);
     NW_THROW_ON_ERROR(status);
    }//end if(WBXMLEncStrTbl)

    /* build the dom-tree */

    status = StatusCodeConvert(NW_TinyDom_AppendDOMTree(&domTree->domParser, 
                                    domBuffer->data, domBuffer->length, cp_count, lastValid, NULL));
    NW_THROW_ON_ERROR(status);
    if (domTree->domBuffer != domBuffer)
        {
        NW_Buffer_Free(domTree->domBuffer);
        }
    domTree->domBuffer = domBuffer;
    *ppDomTree = domTree;
  }
  NW_CATCH(status) {
    if (domTree != NULL) {
      NW_TinyDom_ParserDelete(&domTree->domParser);
    }
    NW_Mem_Free(domTree);
    NW_Buffer_Free(domBuffer);
    domTree = NULL;
    *ppDomTree = domTree;
  }
  NW_FINALLY {
    return status;
  }
  NW_END_TRY
}


/* ------------------------------------------------------------------------- */
void NW_HED_DomTree_Delete (NW_HED_DomTree_t* thisObj)
{
  if (thisObj != NULL) {
    NW_TinyDom_ParserDelete(&thisObj->domParser);

    if (thisObj->domBuffer != NULL) {
      NW_Buffer_Free (thisObj->domBuffer);
    }

    NW_Mem_Free (thisObj);
  }
}

/* ------------------------------------------------------------------------- */
NW_DOM_Node_t* NW_HED_DomTree_GetRootNode (const NW_HED_DomTree_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  return thisObj->domParser.tinyParser.dom_tree->root_node;
}

/* ------------------------------------------------------------------------- */
NW_Uint32 NW_HED_DomTree_GetPublicId (const NW_HED_DomTree_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  return NW_DOM_DocumentNode_getPublicIdAsNumber (
      NW_HED_DomTree_GetRootNode (thisObj));
}

/* ------------------------------------------------------------------------- */
NW_Uint8 NW_HED_DomTree_GetVersion (const NW_HED_DomTree_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  return NW_DOM_DocumentNode_getVersion (
      NW_HED_DomTree_GetRootNode (thisObj));
}

/* ------------------------------------------------------------------------- */
NW_Uint32 NW_HED_DomTree_GetCharEncoding (const NW_HED_DomTree_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  return NW_DOM_DocumentNode_getCharacterEncoding (
      NW_HED_DomTree_GetRootNode (thisObj));
}

/* ------------------------------------------------------------------------- */
NW_Buffer_t* NW_HED_DomTree_GetDomBuffer (const NW_HED_DomTree_t* thisObj)
{
  NW_ASSERT (thisObj != NULL);

  return thisObj->domBuffer;
}

/* ------------------------------------------------------------------------- */
NW_Buffer_t* NW_HED_DomTree_OrphanDomBuffer (NW_HED_DomTree_t* thisObj)
{
  NW_Buffer_t* buff;

  NW_ASSERT (thisObj != NULL);

  buff = thisObj->domBuffer;
  thisObj->domBuffer = NULL;

  return buff;
}

