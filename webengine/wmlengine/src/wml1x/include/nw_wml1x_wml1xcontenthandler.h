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

#ifndef NW_WML1X_CONTENTHANDLER_H
#define NW_WML1X_CONTENTHANDLER_H

#include "nw_hed_compositecontenthandler.h"
#include "nw_wmlscriptch_iwmlbrowserlib.h"
#include "nw_wmlscriptch_iwmlscriptlistener.h"
#include "nw_markup_numbercollector.h"

#include "nw_system_timer.h"
#include "nw_hed_domhelper.h"
#include "nw_hed_domtree.h"
#include "nw_text_ucs2.h"
#include "nw_adt_dynamicvector.h"
#include "nw_wml1x_wml1xformliaison.h"
#include <nw_tinydom_utils.h>
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Wml1x_ContentHandler_Class_s NW_Wml1x_ContentHandler_Class_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Wml1x_ContentHandler_ClassPart_s {
  void** unused;
} NW_Wml1x_ContentHandler_ClassPart_t;

struct NW_Wml1x_ContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
  NW_HED_CompositeContentHandler_ClassPart_t NW_HED_CompositeContentHandler;
  NW_Wml1x_ContentHandler_ClassPart_t NW_Wml1x_ContentHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Wml1x_ContentHandler_s {
  NW_HED_CompositeContentHandler_t super;

  /* implemented interfaces */
  NW_WmlsCh_IWmlBrowserLib_t      NW_WmlsCh_IWmlBrowserLib;
  NW_WmlsCh_IWmlScriptListener_t  NW_WmlsCh_IWmlScriptListener;

  /* aggregate(s) */
  NW_Markup_NumberCollector_t     NW_Markup_NumberCollector;

  /* member variables */
  NW_Text_t*                      title;
  NW_Text_t*                      currentUrl;
  NW_Wml_t                        wmlInterpreter;
  NW_LMgr_Box_t*                  currentBox;         /* box onto which further content is added as child(ren) */
  NW_Int16                        lastId;             /* next element Id to be assigned */
  NW_Bool                         isSuspended;        /* whether or not the content handler has been suspended */

  NW_System_Timer_t*              wmlTimer;

  NW_ADT_Map_t                    *optionMap;         /* maps option element ids to option element states */
  NW_Wml1x_FormLiaison_t          *formLiaison;       /* provides methods needed by FormBoxes */
  NW_Ds_DynamicArray_t            *optionItemList;    /* copy of the option list item pointers */

	NW_HED_Context_t*								contextSuspended;		/* copy of the context at the time of supend or 
	                                                       IntraPageNavigationStarted */
  NW_Bool                         saveddeck;          /* whether the content is retrieved from saved deck
                                                         used for setting only-cache mode for url request for embeded element loading */
  NW_Bool                         histload;          /* whether the content is retrieved as result of history load
                                                         used for setting histprev cache mode for url request for embeded element loading */
  NW_Bool                         reload;            /* whether the content is retrieved as result of reload
                                                         used for setting no cache mode for url request for embeded element loading */
  NW_Bool                         allowTimer;         /* Prevent RESTORE_TASK (Resume) from re-creating timers.*/
    };

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Wml1x_ContentHandler_ClassPartOf(_object) \
  (NW_Object_GetClassPart (_object, NW_Wml1x_ContentHandler))

#define NW_Wml1x_ContentHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_Wml1x_ContentHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Wml1x_ContentHandler_Class_t NW_Wml1x_ContentHandler_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode 
NW_Wml1x_ContentHandler_DeleteOptionItemList(NW_Ds_DynamicArray_t *itemList);

extern
NW_HED_DomHelper_t*
NW_Wml1x_ContentHandler_GetDomHelper (NW_Wml1x_ContentHandler_t* thisObj,
                                      NW_Wml_VarList_t* varList);
extern
NW_HED_DomTree_t*
NW_Wml1x_ContentHandler_GetDomTree (NW_Wml1x_ContentHandler_t *thisObj);

/* retrieve the boolean whether content is saved content */
extern
NW_Bool
NW_Wml1x_ContentHandler_IsSaveddeck (NW_Wml1x_ContentHandler_t * thisObj);

/* retrieve the boolean whether content is history load */
extern
NW_Bool
NW_Wml1x_ContentHandler_IsHistload (NW_Wml1x_ContentHandler_t * thisObj);

/* retrieve the boolean whether content is reload */
extern
NW_Bool
NW_Wml1x_ContentHandler_IsReload (NW_Wml1x_ContentHandler_t * thisObj);

/* set the boolean whether content is the saved content */
extern
TBrowserStatusCode
NW_Wml1x_ContentHandler_SetSaveddeck (NW_Wml1x_ContentHandler_t * thisObj, NW_Bool isSaveddeck);

/* set the boolean whether content is a history load */
extern
TBrowserStatusCode
NW_Wml1x_ContentHandler_SetHistload (NW_Wml1x_ContentHandler_t * thisObj, NW_Bool isHistload);

/* set the boolean whether content is a reload */
extern
TBrowserStatusCode
NW_Wml1x_ContentHandler_SetReload (NW_Wml1x_ContentHandler_t * thisObj, NW_Bool isReload);

extern
void AddImageProperties(NW_Wml1x_ContentHandler_t *thisObj,
                               NW_LMgr_Box_t *box,
                               NW_LMgr_PropertyValue_t value);

extern
TBrowserStatusCode LoadSrcImage( NW_Wml1x_ContentHandler_t* thisObj, NW_Uint16 elId );

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1X_CONTENTHANDLER_H */
