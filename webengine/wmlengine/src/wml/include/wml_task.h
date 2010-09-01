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
    $Workfile: wml_url_utils.h $

    Purpose:

        Class: WmlBrowser

        WML interpreter various functions decalrations. All functions are
        used by this component only.

*/

#ifndef WML_TASK_H
#define WML_TASK_H

#include "wml_core.h"

#include "nwx_mem.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WAE_TIMER_USE_DEFAULT,
    WAE_TIMER_TIMEOUT,
    WAE_TIMER_FROM_TIMER
} NW_Wml_TimerBehavior_t;


/*
 * wml_bind.c
 */
TBrowserStatusCode NW_Wml_ApplyContainedInternalVariables(NW_Wml_t* thisObj, NW_Wml_Element_t* container);

/*
 * wml_head.c
 */
TBrowserStatusCode NW_Wml_AccessControl(NW_Wml_t *thisObj, NW_Ucs2 *url);

TBrowserStatusCode NW_Wml_HandleMetaElements(NW_Wml_t *thisObj);


/*
 * wml_input_validation.c
 */
NW_Bool NW_WmlInput_ValidateFormat(const NW_Ucs2 * format);


/*
 * wml_intrinsic_event.c
 */
TBrowserStatusCode NW_Wml_HandleIntrinsicEvents(NW_Wml_t *obj, NW_Wml_Intrinsic_e);


/*
 * wml_option_event.c
 */
TBrowserStatusCode NW_Wml_HandleOptionEvent(NW_Wml_t *obj, NW_Wml_Element_t* el);

/*
** wml_event.c
*/
TBrowserStatusCode NW_Wml_HandleInputEvent(NW_Wml_t          *thisObj, 
                                    NW_Wml_Element_t  *el,
                                    NW_Ucs2           *value, 
                                    NW_Bool           setFlag,
                                    NW_Bool           *validateOK);

/*
 * wml_query_service.c
 */
NW_Int32  NW_Wml_FindEl(NW_Wml_t *thisObj, NW_Uint16 id);

/*
 * wml_ref_model.c
 */
TBrowserStatusCode NW_Wml_HandleTaskEvent(NW_Wml_t *thisObj, NW_Wml_Element_t* el);

TBrowserStatusCode NW_Wml_ProcessGo(NW_Wml_t *obj, NW_Ucs2 *response_url);

TBrowserStatusCode NW_Wml_ProcessPrev(NW_Wml_t *obj, NW_Ucs2 *response_url);

TBrowserStatusCode NW_Wml_ProcessRefresh(NW_Wml_t *obj);

/*
 * wml_select_ui_state.c
 */
TBrowserStatusCode NW_Wml_SetCardUIState(NW_Wml_t *thisObj);

TBrowserStatusCode NW_WmlInput_InitOptionValues(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el);


/*
 * wml_select_validation.c
 */
TBrowserStatusCode NW_WmlSelect_ValidateName_Value(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* iname_var_value, NW_Wml_Select_Type_e type, NW_Mem_Segment_Id_t mem_scope, NW_Ucs2 **ret_string);

TBrowserStatusCode NW_WmlSelect_ValidateIName_Value(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Ucs2* name_var_value, NW_Wml_Select_Type_e type, NW_Mem_Segment_Id_t mem_scope, NW_Ucs2 **ret_string);

TBrowserStatusCode NW_WmlSelect_GetFirstOptionValue(NW_Wml_t* thisObj, NW_Wml_Element_t* select_el, NW_Mem_Segment_Id_t mem_scope, NW_Ucs2 **ret_string);


/*
 * wml_timer.c
 */
TBrowserStatusCode NW_Wml_SetTimerKey(NW_Wml_t* thisObj, NW_Wml_TimerBehavior_t behavior, NW_Uint32 *ret_time);

TBrowserStatusCode NW_Wml_HandleTimer(NW_Wml_t *thisObj);

void NW_Wml_SetTimerProcessing(NW_Wml_t* thisObj, NW_Bool timerProcessing);

NW_Bool NW_Wml_IsTimerProcessingComplete(NW_Wml_t* thisObj);



/*
 * wml_url_navigation.c
 */
TBrowserStatusCode NW_Wml_FetchUrlGeneric(
                            NW_Wml_t *obj, 
                            NW_Ucs2 *url_param,
                            NW_Bool is_method_post,
                            NW_Http_Header_t *header,
                            NW_NVPair_t *postfields,
                            NW_Ucs2 *enctype);
TBrowserStatusCode NW_Wml_FetchUrl(NW_Wml_t *obj, NW_Ucs2 *url, NW_Wml_Element_t* go_el);
TBrowserStatusCode NW_Wml_FetchPrevUrl(NW_Wml_t *obj);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* WML_TASK_H */
