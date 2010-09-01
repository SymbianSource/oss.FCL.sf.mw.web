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

/*! \file nwx_ui_callbacks_register.cpp
*/

/*
  nwx_ui_callbacks_register.cpp

  Implements the callback functions for the UI. These call the corresponding
  functions which have been registered by the user agent.

  Original Version: Peter B
*/

#include <e32def.h>
#include "nwx_defs.h"
#include "nwx_ui_callbacks_register.h"
#include "nwx_ctx.h"
#include "MVCShell.h"
#include "Epoc32DialogProvider.h"
#include "BrsrStatusCodes.h"
/************************************************************************
  Function: NW_UI_DialogPrompt
  Purpose: Displays a generic prompt Passes on the request to the 
           UI callback which has been registered by the user agent.

  Paramters:  data - Data to display in dialog
              callbackCtx - Data to pass onto callback function
              callback - Function to call on dialog completion

  Return Values: none
**************************************************************************/
void NW_UI_DialogPrompt(NW_Dlg_Prompt_t* data,
                        void* callbackCtx, 
                        NW_Dlg_PromptCB_t callback)
{
  LoaderServer_Ctx* context = (LoaderServer_Ctx*)NW_Ctx_Get(NW_CTX_URL_LOADER, 0);
  if (context == NULL || context->UICallbacks == NULL)
  {
    NW_ASSERT(0);
    return;
  }

  (context->UICallbacks->DialogPrompt)(data, callbackCtx, callback);
}

/************************************************************************
  Function: NW_UI_DialogInputPrompt
  Purpose: Displays a generic input prompt. Passes on the request to the 
           UI callback which has been registered by the user agent.

  Paramters:  data - Data to display in dialog
              callbackCtx - Data to pass onto callback function
              callback - Function to call on dialog completion

  Return Values: none
**************************************************************************/
void NW_UI_DialogInputPrompt(NW_Dlg_InputPrompt_t* data,
							               void* callbackCtx, 
                              NW_Dlg_InputPromptCB_t callback)
{
  LoaderServer_Ctx* context = (LoaderServer_Ctx*)NW_Ctx_Get(NW_CTX_URL_LOADER, 0);
  if (context == NULL || context->UICallbacks == NULL)
  {
    NW_ASSERT(0);
    return;
  }

  context->UICallbacks->DialogInputPrompt(data, callbackCtx, callback);
}

/************************************************************************
  Function: NW_UI_DialogListSelect
  Purpose: Displays a generic list box for selection. Passes on the request to the 
           UI callback which has been registered by the user agent.

  Paramters:  data - Data to display in dialog
              callbackCtx - Data to pass onto callback function
              callback - Function to call on dialog completion  

  Return Values: none
**************************************************************************/
void NW_UI_DialogListSelect(NW_Dlg_ListSelect_t* data,
							              void* callbackCtx, 
                            NW_Dlg_ListSelectCB_t callback)
{
  LoaderServer_Ctx* context = (LoaderServer_Ctx*)NW_Ctx_Get(NW_CTX_URL_LOADER, 0);
  if (context == NULL || context->UICallbacks == NULL)
  {
    NW_ASSERT(0);
    return;
  }

  context->UICallbacks->DialogListSelect(data, callbackCtx, callback);
}
/*! 
	\brief Registers UI callbacks. Ownership of callback structure remains <br>
	with user agent (it's not freed here or on shutdown by the context).
	\return The  results  are <br>
	KBrsrSuccess - success <br>
	KBrsrFailure - failed <br>
*/
extern "C" TBrowserStatusCode NW_RegisterUICallbacks(const UI_Callback_t* callbacks)
{
  LoaderServer_Ctx* context = (LoaderServer_Ctx*)NW_Ctx_Get(NW_CTX_URL_LOADER, 0);

  if (context == NULL || callbacks == NULL)
  {
    NW_ASSERT(0);
    return KBrsrFailure;
  }

  context->UICallbacks = callbacks;

  return KBrsrSuccess;
}


extern "C" TBrowserStatusCode NW_GUI_DisplayError(NW_Msg_t* msg)
{
  NW_REQUIRED_PARAM(msg);
  CShell* shellInstance = REINTERPRET_CAST(CShell*, (NW_Ctx_Get(NW_CTX_BROWSER_APP, 0)));  
  NW_ASSERT(shellInstance);
  MEpoc32DialogProvider& dialogProvider = shellInstance->DialogApi();
  TRAPD(err, dialogProvider.NotifyErrorL(msg->status));
	NW_Msg_Delete(msg);
  return KBrsrSuccess;
}
