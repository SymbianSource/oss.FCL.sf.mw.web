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


#ifndef _NW_XHTML_ContentHandler_h_
#define _NW_XHTML_ContentHandler_h_

#include "nw_adt_dynamicvector.h"
#include "nw_adt_map.h"
#include "nw_basicforms_xhtmlcontrolset.h"
#include "nw_css_processor.h"
#include "nw_fbox_formliaison.h"
#include "nw_hed_compositecontenthandler.h"
#include "nw_hed_domtree.h"
#include "nw_hed_domhelper.h"
#include "nw_markup_numbercollector.h"
#include "nw_markup_module.h"
#include "nw_system_timer.h"
#include "nw_tinydom_utils.h"
#include "nw_xhtml_elementhandler.h"
#include "nwx_http_header.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define NW_XHTML_ContentHandler_Msg_ShowFragment 1

extern NW_WBXML_Dictionary_t NW_XHTML_WBXMLDictionary;

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
class MHEDLoadObserver;
class CCSSVariableStyleSheet;
class MMessageListener;

typedef struct NW_XHTML_ContentHandler_Class_s NW_XHTML_ContentHandler_Class_t;

typedef enum {
  NW_XHTML_CONTENTHANDLER_INVALID,
  NW_XHTML_CONTENTHANDLER_XHTMLBASIC,
  _NW_XHTML_ContentHandler_h_TML, /* or XHTML. XHTMLb + any HTML ext. */
  NW_XHTML_CONTENTHANDLER_IMODEHTMLEXT /* i-mode HTML extensions. */
} _NW_XHTML_ContentHandler_h_tmlSubset_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ContentHandler_ClassPart_s {
  NW_Uint16 numModules;
  const NW_Markup_Module_t* const* modules;
} NW_XHTML_ContentHandler_ClassPart_t;

struct NW_XHTML_ContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
  NW_HED_CompositeContentHandler_ClassPart_t NW_HED_CompositeContentHandler;
  NW_XHTML_ContentHandler_ClassPart_t NW_XHTML_ContentHandler;
};

struct NW_XHTML_Form_Node_Context_s {
  NW_DOM_Node_t     *formNode;
  NW_FBox_FormLiaison_t *formLiaison;  
  };

class TExternalScriptNode  
  {
  public:
  NW_DOM_Node_t     *iScriptNode;
  NW_Uint16         iTransactionId;
  };

typedef struct NW_XHTML_Form_Node_Context_s  NW_XHTML_Form_Node_Context_t;

struct NW_XHTML_Script_Node_Context_s {
  NW_DOM_ElementNode_t  *scriptNode;
  NW_LMgr_Box_t         *box;  
  };

typedef struct NW_XHTML_Script_Node_Context_s NW_XHTML_Script_Node_Context_t;
/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_ContentHandler_s {
  NW_HED_CompositeContentHandler_t super;

  /* aggregation(s) */
  NW_Markup_NumberCollector_t NW_Markup_NumberCollector;

  /* member variables */
  NW_Text_t* title;
  NW_ADT_DynamicVector_t* nodeContexts;

  /* Holds a map of CBase derived ElementHandler instances.  The map uses
     the associated element-node pointer as a key.
  */  
  NW_ADT_Map_t* iElementHandlerMap;

  NW_HED_DomTree_t* domTree;
  NW_HED_DomHelper_t domHelper;

  NW_DOM_Node_t     *baseElement;

  NW_Text_t* fragment;

  /* Holds the content-location in the header from the response*/ 
  NW_Ucs2 *contentLocation;

  NW_CSS_Processor_t* processor;

  NW_Object_Shared_t* historyContentData;
  NW_ADT_DynamicVector_t* controlValueSet; /* form control values */
  NW_XHTML_ControlSet_t* controlSet; /* form control values and DOM nodes */
  NW_Bool isLoading; /* to check the load status */

  /* HTML subset implemented in this version. */
  _NW_XHTML_ContentHandler_h_tmlSubset_t htmlSubset;
  
  /* whether the xhtml content is retrieved from saved deck */
  NW_Bool saveddeck;
  NW_Bool histload;
  /* whether the xhtml content is reloaded*/
  NW_Bool reload;

  /* Support for META HTTP-EQUIV="refresh" */
  NW_Bool metaRefreshSet;
  NW_Int32 metaRefreshSeconds;
  NW_Uint8 metaRefreshLoadReason;
  NW_Text_t* metaRefreshResolvedUrl;
  NW_System_Timer_t* metaRefreshTimer;

  /* flags used to decide which element to ignore when both body and */
  /* frameset elements are in the same page*/
  NW_Bool ignoreFramesetElement;
  NW_Bool ignoreBodyElement; 

  // a map structure of map elements.
  void* imageMapLookupTable;

  /*
  <noscript> element and its children should not be rendered when :
  1)	scripting is enabled 
  3)	an supported <script> type "e.g. "javascript" was encountered 
  skipToNode is used to point to the next sibling node of the <noscript>
  element and in the content handler initialize the <noscript> node and its children are
  skipped by skipping upto the skipToNode.
  skipToNode is initialized in <noscript> element handler, when ignoreNoScriptElement is TRUE.
  This is also used to skip over any embedded elements in a valid object tag once a element
  is selected.
  */
  NW_DOM_ElementNode_t* skipToNode;
  // is set to TRUE in the script element handler , if the script content is supported
  NW_Bool ignoreNoScriptElement; 


  // last dom node of the last chunk
  NW_DOM_Node_t* lastInitializedDomNode;
  NW_DOM_Node_t* prevInitializedDomNode;

  void* htmlpParser;
  
  // flag to control intialize element handler
  NW_Bool stopInitEH;
  NW_Bool lastChunk;
  // indicates that script is being executed on
  // the content. Ecma script element handler turns this flag on and off.
  NW_Bool iScriptIsExecuted;

  // indicates that DocumentRoot has been resumed after suspending.
  // E.g. if we click on image link while browsing, the  DocumentRoot will be suspended
  // & unknown CH will handle browsing. After that  the  DocumentRoot will be resumed.
  // In this case we don't need to execute script. In order to handle this situation this flag was introduced.
  NW_Bool iScriptIsResumed;

  // the flag is set after the formatting is complete. 
  // the flag is used to notify ecmaengine of the document load complete 
  NW_Bool formattingComplete;

  // incremental rendering does not support normal mode so
  // whenever a html page is fetched, first we switch to small 
  // screen mode and stay in that mode until the page
  // loaded completly.
  // However if the original mode was normal we need to
  // switch back.
  // this flag is set to true if we need to switch back
  NW_Bool switchBackToNormalLayout;

  NW_Bool scriptLoadComplete;

  NW_Bool iScriptCancelled;

  NW_Bool metaNoCache;

  NW_Bool metaCacheExpires;

  NW_Ucs2* metaExpiresDate;

  void* scriptLoadTransactionIds;

  // list - contains script nodes that are not initialized 
  // when external script loads are pending
  void* scriptNodes;

  void* formNodes;

  /* interface reference */
  MMessageListener* iMessageListener;

  CCSSVariableStyleSheet* variableStyleSheet;
  //
  NW_Bool iBodyIsPresent;

  NW_DOM_Node_t* iBodyNode;

};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ContentHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_XHTML_ContentHandler))

#define NW_XHTML_ContentHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_XHTML_ContentHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_ContentHandler_Class_t NW_XHTML_ContentHandler_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ContentHandler_SetTitle(_xhtmlContentHandler, _title) \
  (((_xhtmlContentHandler)->title = (_title)), KBrsrSuccess)

#define NW_XHTML_ContentHandler_GetTitle(_xhtmlContentHandler) \
  ((const NW_Text_t const*) (_xhtmlContentHandler)->title)

#define NW_XHTML_ContentHandler_GetVariableStyleSheet(_xhtmlContentHandler) \
  ((CCSSVariableStyleSheet*) (_xhtmlContentHandler)->variableStyleSheet)

#define NW_XHTML_ContentHandler_SetBaseElement(_xhtmlContentHandler, _baseElement) \
  ((((NW_XHTML_ContentHandler_t*)_xhtmlContentHandler)->baseElement = (_baseElement)), KBrsrSuccess)

#define NW_XHTML_ContentHandler_GetBaseElement(_xhtmlContentHandler) \
  ((NW_DOM_Node_t*) ((NW_XHTML_ContentHandler_t*)_xhtmlContentHandler)->baseElement)

#define NW_XHTML_ContentHandler_GetContentLocation(_xhtmlContentHandler) \
  ((NW_Ucs2*) ((NW_XHTML_ContentHandler_t*)_xhtmlContentHandler)->contentLocation)

#define NW_XHTML_ContentHandler_SetNodeContext(_xhtmlContentHandler, _node, _context) \
  (NW_ADT_Map_Set ((_xhtmlContentHandler)->nodeContexts, (_node), (_context)))

#define NW_XHTML_ContentHandler_GetCSSProcessor(_xhtmlContentHandler) \
  ((_xhtmlContentHandler)->processor)

#define NW_XHTML_ContentHandler_RemoveNodeContext(_xhtmlContentHandler, _node) \
  (NW_ADT_Map_Remove ((_xhtmlContentHandler)->nodeContexts, (_node)))


/* extern
 * _NW_XHTML_ContentHandler_h_tmlSubset_t 
 * NW_XHTML_ContentHandler_GetHtmlSubset (NW_XHTML_ContentHandler_t* contentHandle);
 */
#define NW_XHTML_ContentHandler_GetHtmlSubset(_xhtmlContentHandler) \
  ((_xhtmlContentHandler)->htmlSubset)

extern
const NW_XHTML_ElementHandler_t*
NW_XHTML_ContentHandler_GetElementHandler (NW_XHTML_ContentHandler_t* contentHandler,
                                           NW_DOM_ElementNode_t* domElement);

extern
NW_HED_DocumentNode_t*
NW_XHTML_ContentHandler_MapElementNode (NW_XHTML_ContentHandler_t* contentHandler,
                                        NW_DOM_ElementNode_t* domElement);

extern
TBrowserStatusCode 
NW_XHTML_GetDOMAttribute (NW_XHTML_ContentHandler_t* contentHandler,
                          NW_DOM_ElementNode_t* elementNode,
                          const NW_Uint16 token, NW_Text_t** text_t);

extern
NW_Text_t*
NW_XHTML_GetDOMTextNodeData(NW_XHTML_ContentHandler_t* thisObj,
                            NW_DOM_TextNode_t* textNode);

extern
NW_CSS_Processor_t*
NW_XHTML_ContentHandler_CreateCSSProcessor(NW_XHTML_ContentHandler_t* contentHandler);

extern
NW_WBXML_Dictionary_t*
NW_XHTML_ContentHandler_GetDictionary(NW_XHTML_ContentHandler_t* contentHandler);

extern
TBrowserStatusCode
NW_XHTML_ContentHandler_AddFormLiaison(NW_XHTML_ContentHandler_t* contentHandler,
                                       NW_DOM_ElementNode_t* formNode);

extern
NW_FBox_FormLiaison_t*
NW_XHTML_ContentHandler_GetFormLiaison(NW_XHTML_ContentHandler_t* contentHandler,
                                       NW_DOM_ElementNode_t* formNode);


extern
TBrowserStatusCode
NW_XHTML_ContentHandler_StartLoad (NW_XHTML_ContentHandler_t* thisObj,
                                   const NW_Text_t* url,
                                   NW_Uint8 reason,
                                   void* clientData, 
                                   NW_Uint8 type);
extern
TBrowserStatusCode
NW_XHTML_ContentHandler_StartRequest (NW_XHTML_ContentHandler_t* thisObj,
                                      NW_HED_UrlRequest_t* urlRequest,
                                      void* clientData );

extern
TBrowserStatusCode
NW_XHTML_ContentHandler_StartLoad_tId (NW_XHTML_ContentHandler_t* thisObj,
                                   const NW_Text_t* url,
                                   NW_Uint8 reason,
                                   void* clientData, 
                                   NW_Uint8 type,
                                   NW_Uint16* aTransationId);
extern
TBrowserStatusCode
NW_XHTML_ContentHandler_StartRequest_tId (NW_XHTML_ContentHandler_t* thisObj,
                                      NW_HED_UrlRequest_t* urlRequest,
                                      void* clientData ,
                                      NW_Uint16* aTransationId);

extern
TBrowserStatusCode
NW_XHTML_ContentHandler_SetLoadObserver (NW_XHTML_ContentHandler_t* thisObj,
                                         NW_Uint16 aTransationId,
                                         MHEDLoadObserver* aLoadObserver);

/* returns true if the xhtml content is retrieved from saved deck, otherwise false.
 * used for cache-only loading for embeded elements in saved file 
 */
extern
NW_Bool
NW_XHTML_ContentHandler_IsSaveddeck(const NW_XHTML_ContentHandler_t* thisObj);

/* returns true if the xhtml content is history load, otherwise false.
 * used for cache-only loading for embeded elements in saved file 
 */
extern
NW_Bool
NW_XHTML_ContentHandler_IsHistoryLoad(const NW_XHTML_ContentHandler_t* thisObj);


/* returns true if the xhtml content is reload, otherwise false.
 */
extern
NW_Bool
NW_XHTML_ContentHandler_IsReLoad(const NW_XHTML_ContentHandler_t* thisObj);

/* set the saveddeck boolean member 
 */
extern
TBrowserStatusCode
NW_XHTML_ContentHandler_SetSaveddeck(NW_XHTML_ContentHandler_t *thisObj, NW_Bool isSaveddeck);

/* set the history load boolean member 
 */
extern
TBrowserStatusCode
NW_XHTML_ContentHandler_SetHistoryLoad(NW_XHTML_ContentHandler_t *thisObj, NW_Bool isHistLoad);


/* set the reload boolean member 
 */
extern
TBrowserStatusCode
NW_XHTML_ContentHandler_SetReLoad(NW_XHTML_ContentHandler_t *thisObj, NW_Bool isReLoad);

/* Initialize the META refresh. If and only if returns success, the content
 * handler object will take ownership of and assume responsibility for
 * destructing the resolvedUrl object.  Once initialized, the content handler
 * object performs the META refresh when needed.
 */
extern
TBrowserStatusCode
NW_XHTML_ContentHandler_MetaRefreshInitialize(NW_XHTML_ContentHandler_t *thisObj,
                                              NW_Int32 seconds,
                                              NW_Text_t* resolvedUrl,
                                              NW_Uint8 loadReason);
extern
void
NW_XHTML_ContentHandler_SetImageMapLookupTable(NW_XHTML_ContentHandler_t *thisObj,
                                               void* mapList);


extern
void*
NW_XHTML_ContentHandler_GetImageMapLookupTable(NW_XHTML_ContentHandler_t *thisObj);

extern
void
NW_XHTML_ContentHandler_SetReferrerUrl(NW_XHTML_ContentHandler_t *thisObj);



extern
TBrowserStatusCode
NW_XHTML_ContentHandler_AddToPendingScriptTids(NW_XHTML_ContentHandler_t *thisObj,const TExternalScriptNode& aScriptNodeId );


extern
NW_Uint8
NW_XHTML_ContentHandler_PendingScriptLoadCount(NW_XHTML_ContentHandler_t *thisObj);

extern
CBase*
NW_XHTML_ContentHandler_GetElementHandlerInstance(NW_XHTML_ContentHandler_t *thisObj,
        NW_DOM_ElementNode_t* elementNode);

extern
TBrowserStatusCode
NW_XHTML_ContentHandler_SetElementHandlerInstance(NW_XHTML_ContentHandler_t *thisObj,
        NW_DOM_ElementNode_t* elementNode, CBase* elementHandler);

extern
TBrowserStatusCode
NW_XHTML_ContentHandler_AddFormNodes(NW_XHTML_ContentHandler_t *thisObj, NW_DOM_ElementNode_t* elementNode);

extern
NW_Bool
NW_XHTML_ContentHandler_IsScriptLoadPending(NW_XHTML_ContentHandler_t *thisObj, NW_DOM_ElementNode_t* aElementNode) ;
NW_DOM_ElementNode_t*
NW_XHTML_ContentHandler_GetFormNode(NW_XHTML_ContentHandler_t *thisObj, NW_DOM_ElementNode_t* elementNode);

extern
TBrowserStatusCode
NW_XHTML_ContentHandler_ShowFragment (NW_XHTML_ContentHandler_t* thisObj, const NW_Text_t* fragment);

extern
NW_Bool
NW_XHTML_ContentHandler_AllowableStylesheet(const NW_Ucs2* media, NW_Bool* isHandheld);

extern
void NW_XHTML_ContentHandler_EnableMobileProfileMode(NW_XHTML_ContentHandler_t* thisObj);

extern
TBrowserStatusCode
NW_XHTML_ContentHandler_LoadStylesheet(NW_XHTML_ContentHandler_t* thisObj,
    const NW_Ucs2* type, const NW_Ucs2* relation, const NW_Ucs2* media,
    const NW_Ucs2* url, const NW_Ucs2* charset, const NW_Ucs2* alternate);

TBrowserStatusCode
NW_XHTML_ContentHandler_WindowOpen(NW_XHTML_ContentHandler_t* thisObj,
                                   NW_Text_t* url, NW_Text_t* target);


TBrowserStatusCode
NW_XHTML_ContentHandler_WindowOpenAndPost(NW_XHTML_ContentHandler_t* thisObj,
    NW_Text_t* url, NW_Text_t* target, NW_Uint8* contentType, 
    NW_Uint32 contentTypeLength, NW_Uint8* postData, NW_Uint32 postDataLength,
    NW_Uint8* boundary);

void
NW_XHTML_ContentHandler_GetBaseTagTarget (NW_XHTML_ContentHandler_t* contentHandler,
                                          NW_Text_t** targetVal);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_XHTML_ContentHandler_h_ */
