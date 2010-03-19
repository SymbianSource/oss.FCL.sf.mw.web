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


/*
    $Workfile: nw_wml_core.h $

    Purpose:

        Class: WmlBrowser

        Main browser engine class declaration. The exteral environment messages to this
        engine through the constructor, Start, Event, and *Resp methods. The core browser
        object directs the URL Loader to get WML deck and card bytecode. It then processes
        the WML tags and interfaces with an external UI system. The core browser also 
        interfaces with the WML script engine.

*/

#ifndef NW_WML_CORE_H
#define NW_WML_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nwx_defs.h"

#include "nw_wml_defs.h"
#include "nw_wml_var.h"
#include "nw_wml_decoder.h"
#include "nw_wml_elements.h"
#include "nw_wml_list.h"
#include "nw_wml_api.h"
#include "nw_nvpair.h"

#include "nwx_mem.h"
#include "nwx_http_header.h"
#include "nwx_memseg.h"
#include "nwx_buffer.h"
#include "urlloader_urlresponse.h"
#include "BrsrStatusCodes.h"


#define MAX_NUMBER_INTRA_OEF 15 /* the value is so chosen that the call stack does not max out - Vishy*/


/*---------------------------------------------------------------------
 * NW_Wml_History_t -- history stack element -- holds information needed
 * to reconstruct the call to access a card we've been to recently.
 --------------------------------------------------------------------*/
typedef struct {
  NW_Ucs2             *req_url;         /* the one sent in the request */
} NW_Wml_History_t;


typedef struct
{
  NW_Ucs2           *load_url;
  NW_Ucs2           *load_url_params;
  NW_Http_Header_t  *load_header;
  NW_Bool           load_prev;
  NW_Uint32         timer_time;
  NW_Bool           timerProcessing;/* NW_TRUE: timer processing complete
                                       NW_FALSE: timer processing not complete */
} NW_Wml_Script_State_t;


typedef struct _NW_Wml_t   NW_Wml_t;
struct _NW_Wml_t {
  void                  *browser_app;       /* the user agent */
  NW_Wml_VarList_t      *var_list;
  NW_Wml_VarList_t      *param_list;
  NW_Wml_Task_e         curr_task;
  NW_Ucs2               *history_resp_url;
  NW_DeckDecoder_t      *decoder;
  NW_Wml_Element_t      **card_els;
  NW_Int16              prev_task_id;
  NW_Int16              do_type_prev_id;
  NW_Wml_Script_State_t script_state;
  NW_Bool               hasFocus;
  NW_Int16              outstandingLoadCount;
  NW_Mem_Segment_Id_t   mem_segment_general;
  NW_Mem_Segment_Id_t   mem_segment_card; /* DON'T ACCESS THIS FIELD DIRECTLY */
  const NW_WmlApi_t     *wml_api;    /* user agent call backs */
  NW_Uint16             IntraDeck_OefCounter; /* This counter is used to keep track of the number of 
                                              intra-deck OnEnterforward has occured continuously*/   
  NW_Bool               oefNavigation;    /* NW_TRUE = nav within oef/oeb navigation, NW_FALSE otherwise */
};

/* Type for emptyOk parameter to NW_WmlInput_ValidateInput(). */
typedef enum NW_WmlInput_EmptyOk_e {
  NW_WmlInput_EmptyOk_None,
  NW_WmlInput_EmptyOk_True,
  NW_WmlInput_EmptyOk_False
} NW_WmlInput_EmptyOk_t;


/* macros */
#define DISPLAY_API thisObj->wml_api->display
#define TIMER_API thisObj->wml_api->timer

/* interface from the user agent */
TBrowserStatusCode NW_Wml_Init(NW_Wml_t *that,
                        const NW_WmlApi_t *wmlapi,
                        void *usrAgent);

/* Use this function to init the wml 1.x interpreter 
from the HED Wml content handler */
TBrowserStatusCode NW_Wml_InitFromShell(NW_Wml_t *that,
                                const NW_Ucs2 *url,
                                const NW_WmlApi_t *wmlapi,
                                void *usrAgent);

TBrowserStatusCode NW_Wml_GetVersion(NW_Wml_t *that, NW_Uint8 *version);

TBrowserStatusCode NW_Wml_Start(NW_Wml_t* that, NW_Ucs2 *initialUrl);

TBrowserStatusCode NW_Wml_ReStart(NW_Wml_t *that, TBrowserStatusCode status);

TBrowserStatusCode NW_Wml_Exit(NW_Wml_t *that);

TBrowserStatusCode NW_Wml_GetAttribute(NW_Wml_t *that, NW_Uint16 el_id, NW_Wml_Attr_t tag, NW_Ucs2 **ret_string);

TBrowserStatusCode NW_Wml_GetElementType(NW_Wml_t *that, NW_Uint16 el_id, NW_Wml_ElType_e *el_type, NW_Wml_Element_t **el);

TBrowserStatusCode NW_Wml_GetDefault(NW_Wml_t *that, NW_Uint16 el_id, NW_Ucs2 **ret_string);

TBrowserStatusCode NW_Wml_GUIEvent(NW_Wml_t* that, NW_Uint16 id, void *value);

TBrowserStatusCode NW_Wml_GetCurrURL(NW_Wml_t *that, NW_Ucs2 **ret_string);

TBrowserStatusCode NW_Wml_GetCurrReqURL(NW_Wml_t *that, NW_Ucs2 **ret_string);

TBrowserStatusCode NW_Wml_GetPrevId(NW_Wml_t *that, NW_Int16 *id);

NW_Bool NW_WmlInput_ValidateValue(NW_Ucs2* input_value, 
                                  NW_Ucs2* format, 
                                  NW_WmlInput_EmptyOk_t emptyok, 
                                  NW_Bool partial_validation_ok, 
                                  NW_Uint32 docPublicId);

/* Returns the document public ID */
TBrowserStatusCode NW_Wml_GetPublicId(NW_Wml_t *that, NW_Uint32 *publicId);

/* Note: The NW_Wml_GetImage function is obsolete on Rainbow 2.0 and is only provided for
 compatability with WAVE. To load images use the image loader. */

/* helper function to retrieve the absolute url for the image element SRC*/
TBrowserStatusCode NW_Wml_GetImageUrl(NW_Wml_t *that, NW_Uint16 imgel_id, NW_Ucs2 **ret_string);

/* helper function to retrieve the absolute url for the image element with LOCALSRC */
TBrowserStatusCode NW_Wml_GetLocalImageUrl(NW_Wml_t *that, NW_Uint16 imgel_id, NW_Ucs2 **ret_string);

TBrowserStatusCode NW_Wml_Timeout(void *that);

/* user agents should call this function when they gain focus */
TBrowserStatusCode NW_Wml_GainFocus(NW_Wml_t *that);

/* user agents should call this function when they loose focus */
TBrowserStatusCode NW_Wml_LoseFocus(NW_Wml_t *that);

/* interface from the script interpreter */
TBrowserStatusCode NW_Wml_GetVar(NW_Wml_t* that, const NW_Ucs2 *var_name, NW_Ucs2 **ret_string);

NW_Wml_Var_t *NW_Wml_NextVar(NW_Wml_t* thisObj, NW_Wml_Var_t *current); 

TBrowserStatusCode NW_Wml_SetVar(NW_Wml_t *that, const NW_Ucs2 *varName, const NW_Ucs2 *varValue);

void NW_Wml_UnsetVar(NW_Wml_t *that, NW_Ucs2 *varName);

void NW_Wml_LoadPrev(NW_Wml_t *that);

void NW_Wml_NewContext(NW_Wml_t *that);

/* does a newcontext but leaves the top history entry intact. */
void NW_Wml_NewContextFromScript(NW_Wml_t *that);

void NW_Wml_LoadUrl(NW_Wml_t *that, NW_Ucs2 *url, NW_Ucs2 *param, NW_Http_Header_t *header);

TBrowserStatusCode NW_Wml_DisplayCard(NW_Wml_t* thisObj, NW_Bool cancelRequest);

TBrowserStatusCode NW_Wml_LoadWmlResp(NW_Wml_t *thisObj, NW_Buffer_t* responseBuffer, NW_Bool contentWasPlainText, NW_Url_Resp_t* response);

void NW_Wml_ClearTaskVariables(NW_Wml_t *that);

TBrowserStatusCode NW_Wml_ApplyTaskVariables(NW_Wml_t *that);

TBrowserStatusCode NW_Wml_HasOptionIntrinsicEvents(NW_Wml_t *that, NW_Wml_Element_t* option_el, NW_Bool execute);
TBrowserStatusCode NW_Wml_HasOnpick(NW_Wml_t *that, NW_Uint16 el_id );

NW_Mem_Segment_Id_t NW_Wml_GetCardSegment(NW_Wml_t *thisObj);
TBrowserStatusCode NW_Wml_FreeCardSegment(NW_Wml_t *that);

TBrowserStatusCode NW_Wml_HandleIntraDocRequest (NW_Wml_t *thisObj, NW_Wml_Task_e task, 
                                          NW_Bool historicRequest, const NW_Ucs2 *url);

TBrowserStatusCode NW_Wml_CancelAllLoads (NW_Wml_t *thisObj);

void NW_Wml_HandleError (NW_Wml_t *thisObj, NW_Int16 errorClass, NW_Int16 error);

TBrowserStatusCode NW_Wml_ProcessSetvarElements(NW_Wml_t *thisObj);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_WML_CORE_H */
