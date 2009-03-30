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

    @description: HTML Parser  

 ** ----------------------------------------------------------------------- **/
#ifndef NW_HTMLP_PARSER_H
#define NW_HTMLP_PARSER_H


#include "nw_htmlp_lexer.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Next two govern allocation/reallocation of the open element stack. */
#ifndef NW_HTMLP_OPEN_ELEMENT_INITIAL_STACK_CAPACITY
#define NW_HTMLP_OPEN_ELEMENT_INITIAL_STACK_CAPACITY 16
#endif
#ifndef NW_HTMLP_OPEN_ELEMENT_STACK_GROW_BY
#define NW_HTMLP_OPEN_ELEMENT_STACK_GROW_BY 8
#endif

/* RME Documentation tools do not support callback syntax. */
/* * ----------------------------------------------------------------------- **
    @struct:      NW_HTMLP_EventCallbacks

    @synopsis:    default

    @scope:       public
    @variables:
       TBrowserStatusCode (*beginDocumentCB) (NW_HTMLP_Lexer_t*, void*)
                  default

       TBrowserStatusCode (*endDocumentCB) (NW_HTMLP_Lexer_t*, NW_Bool failed, void*)
                  default

       TBrowserStatusCode (*startTagCB) (NW_Uint8 asciiCharCount, const NW_Uint8* pBuf,
                                  void*)
                  default

       TBrowserStatusCode (*endTagCB) (NW_Uint8 asciiCharCount, const NW_Uint8* pBuf,
                                NW_Bool emptyTag, void*)
                  default

       TBrowserStatusCode (*contentCB) (NW_HTMLP_Lexer_t*, const NW_HTMLP_Interval_t*,
                                 void*)
                  default

       TBrowserStatusCode (*cdataCB) (NW_HTMLP_Lexer_t*, const NW_HTMLP_Interval_t*,
                               void*)
                  default

       TBrowserStatusCode (*attributeStartCB) (void*)
                  default

       TBrowserStatusCode (*attributeNameAndValueCB) (NW_HTMLP_Lexer_t*,
                                               const NW_HTMLP_Interval_t* pName,
                                               NW_Bool missingValue,
                                               const NW_HTMLP_Interval_t* pValue,
                                               void*)
                  default

       TBrowserStatusCode (*attributesEndCB) (NW_Uint32 attributeCount, void*)
                  default

       void* pClientPointer
                  Passed back in each callback.

    @description: default
 ** ----------------------------------------------------------------------- **/
struct NW_HTMLP_EventCallbacks_s {
  TBrowserStatusCode (*beginDocumentCB) (NW_HTMLP_Lexer_t*, void*);
  TBrowserStatusCode (*endDocumentCB) (NW_HTMLP_Lexer_t*, NW_Bool failed, void*);
  TBrowserStatusCode (*startTagCB) (NW_Uint8 asciiCharCount, const NW_Uint8* pBuf,
                             void*);
  TBrowserStatusCode (*endTagCB) (NW_Uint8 asciiCharCount, const NW_Uint8* pBuf,
                           NW_Bool emptyTag, void*);
  TBrowserStatusCode (*contentCB) (NW_HTMLP_Lexer_t*, const NW_HTMLP_Interval_t*,
                            void*);
  TBrowserStatusCode (*cdataCB) (NW_HTMLP_Lexer_t*, const NW_HTMLP_Interval_t*,
                          void*);
  TBrowserStatusCode (*attributeStartCB) (void*);
  TBrowserStatusCode (*attributeNameAndValueCB) (NW_HTMLP_Lexer_t*,
                                          const NW_HTMLP_Interval_t* pName,
                                          NW_Bool missingValue,
                                          const NW_HTMLP_Interval_t* pValue,
                                          void*);
  TBrowserStatusCode (*attributesEndCB) (NW_Uint32 attributeCount, void*);
  TBrowserStatusCode (*piFormCB) (NW_HTMLP_Lexer_t*,
                           const NW_HTMLP_Interval_t* pPITarget,
                           const NW_HTMLP_Interval_t* pArguments,
                           void*);
  void* pClientPointer; /* passed back in each callback */
};


/** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Parse

    @synopsis:    Initiates parse of document.

    @scope:       public

    @parameters:
       [in] NW_HTMLP_Lexer_t* lexer
                  The lexer.

       [in] const struct NW_HTMLP_EventCallbacks_s* pCB
                  Callback structure and parser state.

    @description: Initiates parse of document.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Buffer parsed.

       [KBrsrFailure]
                  Parse failed.

       [KBrsrOutOfMemory]
                  Ran out of memory parsing the document.

 ** ----------------------------------------------------------------------- **/
TBrowserStatusCode NW_HTMLP_Parse(NW_HTMLP_Lexer_t* lexer,
                           const struct NW_HTMLP_EventCallbacks_s* pCB,
                           NW_Bool consumeSpaces);

/* Added for handling special cases for  elements. It is good idea to document 
 * each special case here. In other words, this is small state machine to handle 
 * all such cases.
 *

 * 
 * 1) <script> : If in <script> tag comments ("<!--") occurs then don't allow loose comment
 *               closing (">") but look for ("-->") for closing comments.
 *
 * 2) <a>       : The <a> can have "href" and other attributes. If "href" attribute is
 *                present then allow different formats of attributes 
 *                e.g. href="www.nokia.com/" or 'www.nokia.com/' or www.nokia.com/ as 
 *                part of the attribute value. Note, when no quotes are there then also
 *                '/' is part of the attribute value.
 */

/** ----------------------------------------------------------------------- **
    @enum:        NW_HTMLP_ElementContentType

    @synopsis:    Special case handling cases

    @scope:       public

 ** ----------------------------------------------------------------------- **/
typedef enum NW_HTMLP_SPL_Elem_Type_e {
   NW_HTMLP_SPL_NONE, //No special handling required
   NW_HTMLP_SPL_SCRIPT_COMMENT, //To handle comments inside script
   NW_HTMLP_SPL_ANCHOR_ATTR     //To handle attr val inside <a>
} NW_HTMLP_SPL_Elem_Type_t;

/** ----------------------------------------------------------------------- **
    @struct:    NW_HTMLP_SPL_Elem_Handling

    @synopsis:    Handles special cases for the element tags

    @scope:       public

    @parameters:
       [in] NW_Bool splHandle
                  The element required special treatment or not.

       [in] NW_Uint8 tagName[128]  
						Tag name

       [in] NW_HTMLP_SPL_Elem_Type_t 
						Flag for handling special case in the Element

    @description: Initiates parse of document.

    @returns:     TBrowserStatusCode
                  Status of operation.

       [KBrsrSuccess]
                  Buffer parsed.

       [KBrsrFailure]
                  Parse failed.

       [KBrsrOutOfMemory]
                  Ran out of memory parsing the document.

 ** ----------------------------------------------------------------------- **/

typedef struct NW_HTMLP_SPL_Elem_Handling_s
{
	NW_Bool splHandle; 
    NW_Ucs2 tagName[128];
	NW_HTMLP_SPL_Elem_Type_t type;
}NW_HTMLP_SPL_Elem_Handling_t;


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

/* NW_HTMLP_PARSER_H */
#endif
