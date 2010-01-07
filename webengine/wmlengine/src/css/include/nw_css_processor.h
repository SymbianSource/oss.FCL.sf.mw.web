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


#ifndef NW_CSS_PROCESSOR_H
#define NW_CSS_PROCESSOR_H

#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).

#include "nw_object_dynamic.h"
#include "nw_hed_documentnode.h"
#include "nw_hed_domhelper.h"
#include "nw_text_ucs2.h"
#include <nw_dom_element.h>
#include "nw_lmgr_box.h"
#include "nw_hed_iloadrecipient.h"
#include "nw_image_iimageobserver.h"

#include "nw_css_export.h"
#include "nw_image_abstractimage.h"
#include "nwx_osu_file.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_CSS_Processor_Class_s NW_CSS_Processor_Class_t;
typedef struct NW_CSS_Processor_s NW_CSS_Processor_t;


typedef
TBrowserStatusCode  
(*NW_CSS_Processor_ApplyCSSCallbk_t)(void* contentHandler, 
                               NW_DOM_ElementNode_t* elementNode,
                               NW_LMgr_Box_t** box);

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_CSS_Processor_ClassPart_s {
  void** unused;
} NW_CSS_Processor_ClassPart_t;

struct NW_CSS_Processor_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_CSS_Processor_ClassPart_t NW_CSS_Processor;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_CSS_Processor_s 
{
  NW_Object_Dynamic_t super;

  /* interface implementations */
  NW_HED_ILoadRecipient_t NW_HED_ILoadRecipient;
  NW_Image_IImageObserver_t NW_Image_IImageObserver;

  /* member variables */
  NW_HED_DocumentNode_t* owner;
  void* imageList;
  void* iLoadList;
  NW_CSS_Processor_ApplyCSSCallbk_t applyCSSCallback;
  void* cssHandler;
  NW_ADT_DynamicVector_t* pendingLoads;
  NW_Bool isSavedDeck;
  NW_Bool isHistLoad;
  NW_Bool isReLoad;
  void* ruleList;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_CSS_Processor_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_CSS_Processor))


#define NW_CSS_ProcessorOf(_object) \
  (NW_Object_Cast (_object, NW_CSS_Processor))


/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_CSS_EXPORT const NW_CSS_Processor_Class_t NW_CSS_Processor_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

#define NW_CSS_Processor_SetOwner(_object, _owner) \
  ((NW_CSS_ProcessorOf(_object))->owner = (_owner))

#define NW_CSS_Processor_SetApplyCSSCallback(_object, _applyCSSCallback) \
  ((NW_CSS_ProcessorOf(_object))->applyCSSCallback = (_applyCSSCallback))

#define NW_CSS_Processor_GetApplyCSSCallback(_object) \
  ((NW_CSS_ProcessorOf(_object))->applyCSSCallback)

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* called by linkElementHandler or PiHandler to load external stylesheet 
  the encoding will be NULL, if there was not charset attribute found
  */
NW_CSS_EXPORT
TBrowserStatusCode
NW_CSS_Processor_Load(NW_CSS_Processor_t* thisObj, 
                      NW_Text_t* url, 
                      NW_Text_t* encoding,
                      NW_Bool isSavedDeck,
                      NW_Bool isHistLoad,
					  NW_Bool isReLoad);

/** Get images according to the CSS image list, mainly used to show image when user setting has
 *  auto image load disabled.
 *  Called by Epoc32ContentHandler_ImageViewer_load.
 */
NW_CSS_EXPORT
TBrowserStatusCode
NW_CSS_Processor_ShowImages(NW_CSS_Processor_t* thisObj);

/* called by NW_XHTML_ElementHandler_ApplyStyles - now */
NW_CSS_EXPORT
TBrowserStatusCode
NW_CSS_Processor_ApplyStyles(NW_CSS_Processor_t* thisObj,
                             NW_DOM_ElementNode_t* elementNode,
                             NW_LMgr_Box_t* box,
                             const NW_Evt_Event_t* event,
                             NW_HED_DomHelper_t* domHelper);


TBrowserStatusCode
NW_CSS_Processor_ResolveURL (NW_CSS_Processor_t* thisObj,
                             NW_Text_t* url,
                             NW_Text_t** retUrl,
                             TInt aBufferIndex);

/* called by NW_CSS_DeclarationListHandle_ApplyStyles */
NW_CSS_EXPORT
TBrowserStatusCode
NW_CSS_Processor_GetImage(NW_CSS_Processor_t* thisObj, 
                          NW_Text_t* url, 
                          NW_LMgr_Box_t* box,
                          NW_LMgr_PropertyName_t propName,
                          NW_Image_AbstractImage_t** image, 
                          TInt aBufferIndex);

/* invoked by styleElementHandler */
TBrowserStatusCode NW_CSS_Processor_ProcessBuffer(NW_CSS_Processor_t* thisObj,
                       NW_Byte* data, NW_Uint32 length, NW_Uint32 encoding, NW_Bool doNotDelete);

TBrowserStatusCode
NW_CSS_Processor_SetImage(NW_CSS_Processor_t* thisObj,
                          NW_Byte* data,
                          NW_Uint32 length,
                          NW_Http_ContentTypeString_t contentTypeString,
                          NW_Text_t* url);

TBrowserStatusCode
NW_CSS_Processor_ProcessLoadRequest(NW_CSS_Processor_t* thisObj, 
                                    NW_Text_t* url,
                                    NW_Uint8 loadType, 
                                    NW_Bool isSavedDeck,
                                    NW_Bool isHistLoad,
									NW_Bool isReLoad,
                                    TInt aCssBufferIndex);

/* invoked after the XHTML document has been displayed */
NW_CSS_EXPORT
TBrowserStatusCode
NW_CSS_Processor_ApplyStyleSheet(NW_CSS_Processor_t* thisObj);

/* invoked after the XHTML contenthandler suspend */
NW_CSS_EXPORT
void
NW_CSS_Processor_Suspend(NW_CSS_Processor_t* thisObj);

NW_CSS_EXPORT
TBrowserStatusCode
NW_CSS_Processor_ApplyStyleAttribute(NW_CSS_Processor_t* thisObj, NW_Text_t* styleVal, NW_LMgr_Box_t* box);

NW_CSS_EXPORT
NW_Uint32
NW_CSS_Processor_GetNumCSSImages(NW_CSS_Processor_t* thisObj);

NW_CSS_EXPORT
NW_Uint32
NW_CSS_Processor_GetNumStyleSheets(NW_CSS_Processor_t* thisObj);

NW_CSS_EXPORT
TBrowserStatusCode
NW_CSS_Processor_ProcessPendingLoads(NW_CSS_Processor_t* thisObj);

NW_CSS_EXPORT
TBrowserStatusCode
NW_CSS_Processor_WriteMultipartSegment(NW_CSS_Processor_t* thisObj, 
                                       NW_Osu_File_t fh,
                                       NW_Uint32 numCSSImages,
                                       NW_Uint32 numStyleSheets);

NW_CSS_EXPORT
void
NW_CSS_Processor_SetDictionary(NW_CSS_Processor_t* thisObj, NW_WBXML_Dictionary_t* dictionary);

#ifdef _BROWSER_ENG_DEBUG

NW_CSS_EXPORT
void NW_CSS_Processor_PrintProperties(NW_LMgr_Box_t* box);

#endif /* _BROWSER_ENG_DEBUG */


/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */
NW_CSS_EXPORT
NW_CSS_Processor_t*
NW_CSS_Processor_New();

/* Based on the displayVal, replaces the box type */
TBrowserStatusCode
NW_CSS_Processor_HandleDisplayVal (NW_LMgr_Box_t** box, NW_LMgr_PropertyValue_t *displayVal);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_CSS_PROCESSOR_H */
