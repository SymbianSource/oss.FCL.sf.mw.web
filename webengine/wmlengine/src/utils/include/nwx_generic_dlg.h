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


/*****************************************************************

  Subsystem Name: Messaging

  Version: V1.0

  Description: Defines the platform-independent interface to the 
               message dispatching API for all platforms.

  Implementation Notes:
    This portion of the message dispatching API is intended to be
    completely portable.

******************************************************************/
#ifndef NWX_GENERIC_DLG_H
#define NWX_GENERIC_DLG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/

#include "nwx_defs.h"
#include "nwx_osu.h"
#include "nwx_psq.h"
#include "nwx_msg_api.h"
#include "BrsrStatusCodes.h"

/* Return values */
#define DIALOG_FAIL                 0
#define DIALOG_OK                   1
#define DIALOG_YES_PRESSED          2
#define DIALOG_NO_PRESSED           3
#define DIALOG_ROLLER_PRESSED       4
#define DIALOG_SEND_PRESSED         5
#define DIALOG_END_PRESSED          6
#define DIALOG_HASH_PRESSED         7
#define DIALOG_HASH_RELEASED        8
#ifdef WAP_FEA_WALLET_IN_USE
#define DIALOG_WALLET_SELECTED      9
#define DIALOG_START_WALLET_OPT    10
#endif

  
  /* These index numbers must correspond to the strings in the UI */

typedef enum {
  NULL_BUTTON_INDEX,
  BACK_BUTTON_INDEX,
  SELECT_BUTTON_INDEX,
  OK_BUTTON_INDEX,
  CANCEL_BUTTON_INDEX,
  SIGN_BUTTON_INDEX,
  ENTER_BUTTON_INDEX,
  OPTIONS_BUTTON_INDEX
} NW_DialogButtons_t;

typedef enum {
  DIALOG_CONFIRM_YES,
  DIALOG_CONFIRM_NO,
  DIALOG_CONFIRM_END
} NW_Dlg_Confirm_Result_t;

/*
** Type Definitions
*/ 

enum {
  UI_OPTION_NONE,
  UI_OPTION_TEXT_TO_SIGN,
  UI_OPTION_DISPLAY_CERT_DETAILS,
  UI_OPTION_DISPLAY_SECURITY_SETTINGS,
  UI_OPTION_ACCEPT_SERVER_CERT,
  UI_OPTION_ACCEPT_CLIENT_AUTH,
  UI_OPTION_CERT_LIST_SIGN_TEXT,
  UI_OPTION_SELECT_CERT_HANDSHAKE,
  UI_OPTION_ENTER_PIN_SIGN_TEXT,
  UI_OPTION_ENTER_PIN_MODIFY,
  UI_OPTION_ENTER_PIN_UNBLOCK,
  UI_OPTION_ENTER_NEW_PIN,
  UI_OPTION_ENTER_NEW_PIN_REPEAT,
  UI_OPTION_ENTER_NEW_PIN_RETRY,
  UI_OPTION_ENTER_PIN_ENABLE,
  UI_OPTION_ENTER_PIN_DISABLE,
  UI_OPTION_ENTER_PIN_HANDSHAKE,
  UI_OPTION_ENTER_PIN_HANDSHAKE_RETRY,
  UI_OPTION_ENTER_PIN_CERT_RESP,
  UI_OPTION_ENTER_PIN_SIGNED_CERT,
  UI_OPTION_ENTER_PIN_HASHED_CERT,
  UI_OPTION_ENTER_PIN_DELETE,
  UI_OPTION_SAVE_CERT,
  UI_OPTION_SAVE_SIGNED_CERT,
  UI_OPTION_SAVE_HASHED_CERT,
  UI_OPTION_ENTER_HASH,
  UI_OPTION_ENTER_HASH_ERROR,
  UI_OPTION_SELECT_WIM_SAVE_CERT,
  UI_OPTION_SELECT_WIM_SAVE_HASHED_CERT,
  UI_OPTION_SELECT_WIM_SAVE_SIGNED_CERT
};


typedef struct {
    const NW_Ucs2* title;
    const NW_Ucs2* prompt;
    NW_Uint8 uiOptions;
    NW_Uint16 itemsCount;
    NW_Ucs2** items;
    NW_DialogButtons_t button0;
    NW_DialogButtons_t button1;
    NW_Uint8 selectedItem;
    NW_Uint8 selectedButton;
    void* context;
}NW_Dlg_ListSelect_t ;

typedef struct {
    const NW_Ucs2* title;
    const NW_Ucs2* prompt;
    NW_Uint8 uiOptions;
    NW_DialogButtons_t button0;
    NW_DialogButtons_t button1;
    NW_Uint8 selectedButton;
    void* context;
}NW_Dlg_Prompt_t ;



typedef struct {
    const NW_Ucs2* title;
    const NW_Ucs2* prompt;
    NW_Ucs2* format;
    const NW_Ucs2* emptyOKStr;
    NW_Uint8 uiOptions;
    NW_Bool shouldHide;
    NW_Bool isNumber;
    NW_DialogButtons_t button0;
    NW_DialogButtons_t button1;
    NW_Uint16 maxInputLen;
    NW_Ucs2* input;
    NW_Uint8 selectedButton;
    void* context;
}NW_Dlg_InputPrompt_t;


typedef struct NW_Dlg_SelectItemData_s NW_Dlg_SelectItemData_t;
typedef struct NW_Dlg_SelectData_s NW_Dlg_SelectData_t;

struct NW_Dlg_SelectItemData_s{
  NW_Ucs2* text;
  NW_Bool isSelected;
  NW_Bool isOptGroup;           
  NW_Bool hasOnPick; 
  NW_Bool isLocalNavOnpick;
  void* context;
};

struct NW_Dlg_SelectData_s{
  NW_Ucs2* title;
  void* options; /* pointer to NW_ADT_Vector_t */
  NW_Bool multiple;
  NW_Bool isCanceled; /* if user aborts select, we want to know about it */
};

/**
 * The callback to call when done displaying a generic list dialog.
**/
typedef TBrowserStatusCode (NW_Dlg_ListSelectCB_t)   (NW_Dlg_ListSelect_t *data, void* callbackCtx);

/**
 * The callback to call when done displaying a generic prompt dialog.
**/
typedef TBrowserStatusCode (NW_Dlg_PromptCB_t)   (NW_Dlg_Prompt_t *data, void* callbackCtx);

/**
 * The callback to call when done displaying a generic input prompt dialog.
**/
typedef TBrowserStatusCode (NW_Dlg_InputPromptCB_t)   (NW_Dlg_InputPrompt_t *data, void* callbackCtx);

/**
 * The callback to call when done displaying a generic input confirm dialog.
**/
typedef TBrowserStatusCode (NW_Dlg_ConfirmCB_t)  (void *callback_ctx, TBrowserStatusCode status, NW_Dlg_Confirm_Result_t result);


/**
 * The callback to call when done displaying a generic select dialog.
**/
typedef TBrowserStatusCode (NW_Dlg_SelectCB_t)   
                    (NW_Dlg_SelectData_t *data, void* callbackCtx, NW_Bool selectionChanged);

/**
 * The callback to call when done displaying a generic details dialog.
**/
typedef TBrowserStatusCode (NW_Dlg_DetailsDlgCB_t)   (void* callbackCtx);



void NW_UI_DialogListSelect(NW_Dlg_ListSelect_t *data, 
                            void* callbackCtx,
                            NW_Dlg_ListSelectCB_t callback);

void NW_UI_DialogPrompt(NW_Dlg_Prompt_t *data, 
                        void* callbackCtx,
                        NW_Dlg_PromptCB_t callback);

void NW_UI_DialogInputPrompt(NW_Dlg_InputPrompt_t *data, 
                             void* callbackCtx,
                             NW_Dlg_InputPromptCB_t callback);

void NW_UI_DialogSelectOption(NW_Dlg_SelectData_t *data, 
                              void* callbackCtx, 
                              NW_Dlg_SelectCB_t callback);

void NW_UI_DetailsDialogHashkeyPressed(NW_Ucs2* input, 
                                       void* callbackCtx, 
                                       NW_Dlg_DetailsDlgCB_t callback);

/*
** Global Function Declarations
*/
NW_Dlg_ListSelect_t* NW_Dlg_ListSelectInit(const NW_Ucs2* title,const NW_Ucs2* prompt, NW_Uint8 uiOptions,
                                           NW_Uint16 itemsCount, NW_Ucs2 **items,
                                           NW_DialogButtons_t button0, NW_DialogButtons_t button1,
                                           NW_Uint8 selectedItem, NW_Uint8 selectedButton,void* context);

NW_Dlg_Prompt_t* NW_Dlg_PromptInit(const NW_Ucs2* title,const NW_Ucs2* prompt, NW_Uint8 uiOptions,
                                   NW_DialogButtons_t button0, NW_DialogButtons_t button1,
                                   NW_Uint8 selectedButton,void* context);

NW_Dlg_InputPrompt_t* NW_Dlg_InputPromptInit(const NW_Ucs2* title, const NW_Ucs2* prompt, 
                                             const NW_Ucs2* format, const NW_Ucs2* emptyOKStr,
                                             NW_Uint8 uiOptions, NW_Bool shouldHide, NW_Bool isNumber, 
                                             NW_DialogButtons_t button0,
                                             NW_DialogButtons_t button1, NW_Uint16 maxInputLen,
                                             NW_Ucs2* input, NW_Uint8 selectedButton,void* context);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* NWX_GENERIC_DLG_H */
