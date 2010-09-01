/*
* Copyright (c) 2001 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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
  nwx_ui_callbacks_register.h

  Allows the user agent to register UI functions

  Original Version: Peter B
*/

#ifndef NWX_UI_CALLBACKS_REGISTER_H
#define NWX_UI_CALLBACKS_REGISTER_H

#include "nwx_generic_dlg.h"
#include "BrsrStatusCodes.h"

typedef void (*NW_UI_DialogPrompt_t)(NW_Dlg_Prompt_t* data,
                                    void* callbackCtx, 
                                    NW_Dlg_PromptCB_t callback);

typedef void (*NW_UI_DialogInputPrompt_t)(NW_Dlg_InputPrompt_t* data,
                                         void* callbackCtx, 
                                         NW_Dlg_InputPromptCB_t callback );

typedef void (*NW_UI_DialogListSelect_t)(NW_Dlg_ListSelect_t* data,
                                         void* callbackCtx, 
                                         NW_Dlg_ListSelectCB_t callback);

typedef struct {
  NW_UI_DialogPrompt_t      DialogPrompt;
  NW_UI_DialogInputPrompt_t DialogInputPrompt;
  NW_UI_DialogListSelect_t  DialogListSelect;
}UI_Callback_t;

#ifdef __cplusplus
extern "C" {
#endif
  TBrowserStatusCode NW_RegisterUICallbacks(const UI_Callback_t*);
#ifdef __cplusplus
}
#endif

#endif /* NWX_UI_CALLBACKS_REGISTER_H */


