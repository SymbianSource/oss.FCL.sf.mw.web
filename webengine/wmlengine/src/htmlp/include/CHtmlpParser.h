/*
* Copyright (c) 2000 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CHTMLP_PARSER_H
#define CHTMLP_PARSER_H

#include <e32base.h>
#include "nw_htmlp_lexer.h"
#include "BrsrStatusCodes.h"
#include "nwx_buffer.h"
#include <nw_encoder_wbxmlwriter.h>

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

//Non DTD elements are not found in the HTMLP dictionary and index is stored in
//the upper 16 bits.

#ifndef NW_HTMLP_DTD_ElementTableMask
#define NW_HTMLP_DTD_ElementTableMask 0xffff0000
#endif

//Initialize the Non DTD value as actual index will always be greated than this.

#ifndef NW_HTMLP_DTD_ElementTableInit
#define NW_HTMLP_DTD_ElementTableInit 0x00009999
#endif

  typedef struct {
  void* contentHandler;
  void* response;
} NW_Htmlp_CharsetConvContext;


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
typedef struct NW_HTMLP_EventCallbacks_s {
  TBrowserStatusCode (*beginDocumentCB) (NW_HTMLP_Lexer_t*, void*);
  
  TBrowserStatusCode (*endDocumentCB) (NW_HTMLP_Lexer_t*, TBrowserStatusCode e, void*);
  TBrowserStatusCode (*startTagCB) (NW_Uint8 byteCount, const NW_Uint8* pBuf,
                             void*, NW_Bool isLiteral);
  TBrowserStatusCode (*endTagCB) (NW_Uint8 asciiCharCount, const NW_Uint8* pBuf,
                           NW_Bool emptyTag, void*);
  TBrowserStatusCode (*contentCB) (NW_HTMLP_Lexer_t*, const NW_HTMLP_Interval_t*,
                            void*);
  TBrowserStatusCode (*cdataCB) (NW_HTMLP_Lexer_t*, const NW_HTMLP_Interval_t*,
                          void*);
  TBrowserStatusCode (*attributeStartCB) (void*);
  // cp_count to track the attribute code page switch count.
  TBrowserStatusCode (*attributeNameAndValueCB) (NW_HTMLP_Lexer_t*,
                                          const NW_HTMLP_Interval_t* pName,
                                          NW_Bool missingValue,
                                          const NW_HTMLP_Interval_t* pValue,
                                          void*,
                                          NW_Uint32* cp_count);
  TBrowserStatusCode (*attributesEndCB) (NW_Uint32 attributeCount, void*);
  // cp_count to track the attribute code page switch count.
  TBrowserStatusCode (*piFormCB) (NW_HTMLP_Lexer_t*,
                           const NW_HTMLP_Interval_t* pPITarget,
                           const NW_HTMLP_Interval_t* pArguments,
                           void*,
                           NW_Uint32* cp_count);
  void* pClientPointer; /* passed back in each callback */

  
  TBrowserStatusCode (*charsetConvertCallback) (void* context,
                                                NW_Uint32 charsetLength,
                                                 NW_Uint32 charsetOffset,
                                                 NW_Buffer_t* body, 
                                                 NW_Int32* numUnconvertible, 
                                                 NW_Int32* indexFirstUnconvertible, 
                                                 NW_Buffer_t** outBuf,
                                                 NW_Uint32 *selectedCharset);
  void* charsetContext;

} NW_HTMLP_EventCallbacks_t;

/* Added for handling special cases for  elements. It is good idea to document 
 * each special case here. In other words, this is small state machine to handle 
 * all such cases.
 *

 * 
 * 1) <script> : Consume all the contents inside the <script> element as it is.
 *               
 *
 * 2) <a>       : The <a> can have "href" and other attributes. If "href" attribute is
 *                present then allow different formats of attributes 
 *                e.g. href="www.nokia.com/" or 'www.nokia.com/' or www.nokia.com/ as 
 *                part of the attribute value. Note, when no quotes are there then also
 *                '/' is part of the attribute value.
 * 3) <title>    : If <title> is closed by the mis-spelled end tag. The syntax of the
 *                 <title> tag is "<title> contents </title>". If there is start tag
 *                 <title> is found then any closing tag after this can be assumed as
 *                 end </title> tag.
 *
 * 4) <base>     : The <base> tag has the "href" attribute only. If the value of the
 *                 "href" attribute is not enclosed in the quotes then in case of 
 *                  realtive URLs 
 *                 (e.g. <base href=http://uk.dir.yahoo.com/Product_Information_and_Reviews/>
 *                 The last "/" forword slash is treated as EMPTY end tag but this is not 
 *                 true. If "/" is dropped then relative URLs are not formed correctly
 *                 and page displaying error is obtained. For this case look for ">" 
 *                 only for end of tag.
 *
 *
 */

/** ----------------------------------------------------------------------- **
    @enum:        NW_HTMLP_ElementContentType

    @synopsis:    Special case handling cases

    @scope:       public

 ** ----------------------------------------------------------------------- **/
typedef enum NW_HTMLP_SPL_Elem_Type_e {
   NW_HTMLP_SPL_NONE,           //No special handling required
   NW_HTMLP_SPL_SCRIPT,         //To handle script
   NW_HTMLP_SPL_NOSCRIPT,       //To handle noscript
   NW_HTMLP_SPL_ANCHOR_ATTR,    //To handle attr val inside <a>
   NW_HTMLP_SPL_META,           //To handle meta element
   NW_HTMLP_SPL_BODY,           //To handle body start  
   NW_HTMLP_SPL_TITLE,          //To handle mis-spelled </title>
   NW_HTMLP_SPL_BASE_ATTR       //To handle "href" attr val inside <base>
} NW_HTMLP_SPL_Elem_Type_t;


/** ----------------------------------------------------------------------- **
    @struct:    NW_HTMLP_SPL_Elem_Handling

    @synopsis:    Handles special cases for the element tags

    @scope:       public

    @description: Handles special cases for the element tags
 ** ----------------------------------------------------------------------- **/

typedef struct NW_HTMLP_SPL_Elem_Handling_s
{
	NW_Bool splHandle; 
    NW_Ucs2 tagName[128];
	NW_HTMLP_SPL_Elem_Type_t type;
}NW_HTMLP_SPL_Elem_Handling_t;

typedef struct NW_HTMLP_ElementParseState_s {
  NW_HTMLP_ElementTableIndex_t* pOpenElementStack;

  /* points to stack top, needs to be int so -1 indicates empty */
  NW_Int32 openElementStackPointer;

  /* size in entries when originally malloc'ed, int32 like SP */
  NW_Int32 openElementStackCapacity;

  /* true only when in open element with content type tag PCDATA */
  NW_Bool inPCDATA;

  /* read position at end of element start, used to skip over newlines
     after element start */
  NW_Uint32 readPosition;

} NW_HTMLP_ElementParseState_t;


/** ----------------------------------------------------------------------- **
    @class:       CHtmlpParser

    @synopsis:    parser of a document or segment of a document.

    @description: parser of a document or segment of a document.
 ** ----------------------------------------------------------------------- **/
class CHtmlpParser : public CBase
{
public:

  /** ----------------------------------------------------------------------- **
    @function:    NewL
    @synopsis:    public constructor to create a new CHtmlpParser object.
    @description: public constructor to create a new CHtmlpParser object.
    @returns:     newly created CHtmlpParser object.
  ** ----------------------------------------------------------------------- **/
  static CHtmlpParser* NewL(
    NW_Uint32 inputByteCount, 
    NW_Uint8* pInputBuf, 
    NW_HTMLP_ElementTableIndex_t elementCount,
    NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
    NW_Uint32 encoding,
    NW_Bool consumeSpaces,
    void * parser,
    NW_Bool isScript);

  /** ----------------------------------------------------------------------- **
    @function:    ~CHtmlpParser
    @synopsis:    public destructor.
    @description: public destructor.
  ** ----------------------------------------------------------------------- **/
  ~CHtmlpParser();

  /** ----------------------------------------------------------------------- **
    @function:    NW_HTMLP_Parse
    @synopsis:    Initiates parse of document.
    @description: Initiates parse of document.
    @returns:     TBrowserStatusCode
                  Status of operation.
                   [KBrsrSuccess] Buffer parsed.
                   [KBrsrFailure] Parse failed.
                   [KBrsrOutOfMemory] Ran out of memory parsing the document.
  ** ----------------------------------------------------------------------- **/
  TBrowserStatusCode NW_HTMLP_Parse(NW_Bool isFirstSegment, NW_Bool docComplete, NW_Bool needCharsetDetect);

  NW_Buffer_t* getLastTextBuf();

  NW_Int32 getLastValid();

  NW_Uint32 getCodePageSwitchCount();

  NW_Uint32 getElementCount();

  NW_Buffer_t* getPreviousValidOutput();

  NW_HTMLP_EventCallbacks_t * getEventCallbacks();

  NW_HTMLP_Lexer_t* getLexer();

  TBrowserStatusCode appendVisitedHeadText(NW_Byte* new_data, NW_Uint32 new_len);

  NW_Buffer_t* getVisitedHeadText();

// For WML purposes, return the status of checking the attribute or element name
// in the appropriate dictionary.
  TBrowserStatusCode NW_HTMLP_ValidateWMLAttribute(NW_HTMLP_Lexer_t* pL,const NW_HTMLP_Interval_t* pI_name,void* pV);

  NW_Bool iRestarted;
  NW_Bool iIsHtml;
  NW_WBXML_Writer_t * iWbxmlWriter; 
  NW_Int32 iOrigEncoding;
  NW_Buffer_t* iLeftBytes; // single byte left from last chunk, for (multibyte encodings) 
  NW_Bool iIsScript;

  NW_Bool iTrackingUnTerminatedComment;

private: 

  /* the lexer */
  NW_HTMLP_Lexer_t* iLexer;

  /* Callback structure and parser state.*/
  NW_HTMLP_EventCallbacks_t* iCBs;

  NW_HTMLP_SPL_Elem_Handling_t* iSPLElemHandling;

  NW_HTMLP_ElementParseState_t* iElementParseState;

  NW_HTMLP_ElementParseState_t* iLastValidStack;

  /* last text content parsed by parser for previous segment */
  NW_Buffer_t* iLastTextBuf;

  /* point to right after the last trustable item in the generated output WBXML buffer */
  NW_Int32 iLastValid;

  /* previously processed segements */
 /* NW_Uint8* iPreviousOutputBuffer;*/

  NW_Buffer_t* iPreviousValidOutput;

  NW_Bool iConsumeSpaces;

  NW_Int32 iLastTextBegin;   /* offset of '<' within consumeContent or begining of the last piece text if there is no '<' */

  // members related to code page switches
  // iCurrentXX records the current state that wbxml writer passed, incld those that may be discarded from wbxml buffer later
  // iValidXX records the state that is valid, never includes those cp switches that may be discarded later  
  NW_Uint8 iCurrentTagCP;
  NW_Uint8 iValidTagCP;

  NW_Uint8 iCurrentAttrCP;
  NW_Uint8 iValidAttrCP;

  NW_Uint32 iValidCPCount;  
  NW_Uint32 iCurrentCPCount; 
  // end of code page switch members

  NW_Buffer_t* iVisitedHeadText;

  NW_Int32 iLastScriptStart;

  NW_Bool iNeedCharsetDetect;

  NW_Bool iHasForm;
  NW_Bool iWithinNoscript;

  NW_Bool firstSegment;
  NW_Bool htmlTagFound;
  NW_Bool iDocComplete;

  /* private constructor, called from NewL*/
  CHtmlpParser( NW_Bool consumeSpaces);
  void ConstructL(NW_Uint32 inputByteCount, 
                  NW_Uint8* pInputBuf, 
                  NW_HTMLP_ElementTableIndex_t elementCount,
                  NW_HTMLP_ElementDescriptionConst_t* pElementDictionary,
                  NW_Uint32 encoding, 
                  NW_Bool isScript);

  TBrowserStatusCode NW_HTMLP_IfExistsConsumeKeywordCase(
    NW_HTMLP_Interval_t* pI,
    NW_Uint32 asciiCharCount,
    const NW_Uint8* pKeyword,
    NW_Bool CaseSensitive,
    NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_IfExistsConsumeKeyword(
    NW_HTMLP_Interval_t* pI,
    NW_Uint32 asciiCharCount,
    const NW_Uint8* pKeyword,
    NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_SkipSpace();

  TBrowserStatusCode NW_HTMLP_SkipJunk();

  TBrowserStatusCode NW_HTMLP_SkipCRLF();
  
  TBrowserStatusCode NW_HTMLP_ParseName(NW_Bool* pMatch, NW_HTMLP_Interval_t* pI);
  
  TBrowserStatusCode NW_HTMLP_FinishComment(NW_Bool* pMatch, NW_Bool imodeComment,
    NW_Bool looseComment);
  
  TBrowserStatusCode NW_HTMLP_FinishCdata(NW_HTMLP_Interval_t* pInterval, NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_FinishDoctype(NW_Bool* pMatch, NW_HTMLP_Interval_t* doctypeInterval);
  
  TBrowserStatusCode NW_HTMLP_IfLegalConsumeComment(NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_IfLegalConsumePi(NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_IfLegalConsumeCdata(NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_HandleSpecialCases(NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_IfLegalConsumeDoctype(NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_ConsumeContent(NW_Bool* pMatch);

  
  /* handle attributes */
  void NW_HTMLP_SPL_Elem_Handle_Attr(NW_HTMLP_Interval_t* pI_name, NW_Bool* splAttrFound);
  
  TBrowserStatusCode NW_HTMLP_CheckTagEndOrSpace (NW_Bool* aMatch);

  TBrowserStatusCode NW_HTMLP_CheckTagEnd (NW_Bool* aMatch);

  TBrowserStatusCode NW_HTMLP_AttributeValueConsume(NW_Bool* pMissingValue,
    NW_HTMLP_Interval_t* pI,
    NW_Bool* pMatch,NW_Bool splAttrFound);

  TBrowserStatusCode NW_HTMLP_AttributeNameAndValueConsume(NW_HTMLP_Interval_t* pI_name,
    NW_Bool* pMissingValue,
    NW_HTMLP_Interval_t* pI_attvalue,
    NW_Bool* pMatch,
    NW_HTMLP_SPL_Elem_Handling_t* splElem);

  
  TBrowserStatusCode NW_HTMLP_IfExistsConsumeMinEmptyElementSyntax(NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_IfLegalConsumeAttributes(
    NW_Bool* pMatch,
    NW_Bool* pIsEmptyTagEnd,
    const struct NW_HTMLP_EventCallbacks_s* pCB,
    NW_HTMLP_SPL_Elem_Handling_t* splElem);
  
  NW_HTMLP_ElementDescriptionConst_t* NW_HTMLP_ElementFindTagFromDoc(
    NW_HTMLP_Interval_t* pI,
    NW_HTMLP_ElementTableIndex_t* pIndex);
  
  TBrowserStatusCode NW_HTMLP_IfLegalConsumeElementEnd(NW_Bool* pMatch);
  
  TBrowserStatusCode NW_HTMLP_IfLegalConsumeElementStart(NW_Bool* pMatch);

  TBrowserStatusCode NW_HTMLP_ElementParseState_ExplicitClosing(
    NW_HTMLP_ElementTableIndex_t index,
    NW_HTMLP_ElementDescriptionConst_t* pElement);
  
  TBrowserStatusCode NW_HTMLP_ElementParseState_ImpliedClosings(NW_HTMLP_ElementDescriptionConst_t* pElement);

  TBrowserStatusCode NW_HTMLP_ElementParseState_CloseAll();
  
  void NW_HTMLP_SPL_Elem_Initialize();

  void NW_HTMLP_SPL_Elem_Setup(NW_HTMLP_ElementTableIndex_t elementIndex, NW_Bool findCharset);
  /* element stack operations */
  
  TBrowserStatusCode NW_HTMLP_ElementParseState_New();
  
  TBrowserStatusCode NW_HTMLP_ElementParseState_Push(NW_HTMLP_ElementTableIndex_t index);
  
  NW_HTMLP_ElementTableIndex_t NW_HTMLP_ElementParseState_Pop();
  
  void NW_HTMLP_ElementParseState_Delete(NW_HTMLP_ElementParseState_t** ppS);

  TBrowserStatusCode NW_HTMLP_BR_SPL_Handle();

  TBrowserStatusCode NW_HTMLP_ElementParseState_Clone(NW_HTMLP_ElementParseState_t** ppS);

  TBrowserStatusCode setValidMarks();

  TBrowserStatusCode commitValidMarks();
  
  TBrowserStatusCode NW_HTMLP_SPL_Elem_Handle_xml_charset();

  TBrowserStatusCode NW_HTMLP_SPL_Elem_Handle_Meta(NW_HTMLP_Interval_t* pI_name, 
										                               NW_HTMLP_Interval_t* pI_attvalue,
                                                   NW_HTMLP_SPL_Elem_Handling_t* splElem);

  TBrowserStatusCode NW_HTMLP_SPL_Elem_Handle_BodyStart(NW_HTMLP_ElementDescriptionConst_t* pElement);

  TBrowserStatusCode NW_HTMLP_SPL_Elem_Before_HTML_tag(NW_HTMLP_Lexer_t* pL, const NW_HTMLP_Interval_t* pI_content, 
                                                     NW_Bool* onlySplChar);

  void updateCurrentCP();
  void updateCurrentCP(NW_Uint32 switches);
};


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

/* CHTMLP_PARSER_H */
#endif
