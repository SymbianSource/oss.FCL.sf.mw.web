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
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_defs.h"
#include "nwx_string.h"
#include "scr_msgs.h"
#include "nwx_assert.h"
#include "BrsrStatusCodes.h"

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/
/*
**-------------------------------------------------------------------------
**  Macros
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

/*********************************************************
**  Name:   NW_StartScript_Req_New
**  Description:  Allocate and initialize a new script request.
**  Parameters:   *url      - pointer to the url
**                *referer  - pointer to the referer
**                *content  - pointer to the content
**                len       - length of the content
**                charset   - encoding  
**  Return Value: pointer to new script request or NULL
**********************************************************/
NW_StartScript_Req_t*
NW_StartScript_Req_New(NW_Ucs2 *url, NW_Ucs2 *postfields,
                       NW_Ucs2 *referer, NW_Byte *content,
                       const NW_Uint32 len, const NW_Http_CharSet_t charset)
{
  NW_StartScript_Req_t *req;

  NW_ASSERT(url != NULL);
  NW_ASSERT(content != NULL);
  NW_ASSERT(len > 0);
  NW_ASSERT(referer != NULL);
      
  /* script server's responsibility to free content, url and referer */
  req = (NW_StartScript_Req_t*) NW_Mem_Malloc(sizeof(NW_StartScript_Req_t));
  if (req != NULL) {
    req->url        = url;
    req->postfields = postfields;
    req->referer    = referer;
    req->content    = content;
    req->contentLen = len;
    req->charset    = charset;
    return req;
  }
  return NULL;
}

/*********************************************************
**  Name:   NW_StartScript_Req_Free
**  Description:  Deletes a script request.
**  Parameters:   *req - pointer to the request
**  Return Value: void
**********************************************************/
void NW_StartScript_Req_Free(NW_StartScript_Req_t* req)
{
  if (req != NULL) {
    req->url        = NULL;
    req->postfields = NULL;
    req->content    = NULL;
    req->referer    = NULL;
    NW_Mem_Free(req);
  }
  return;
}

/*********************************************************
**  Name:   NW_StartScript_Req_DeepFree
**  Description:  Deletes a script request and its contents.
**  Parameters:   *req - pointer to the request
**  Return Value: void
**********************************************************/
void NW_StartScript_Req_DeepFree(NW_StartScript_Req_t* req)
{
  if (req != NULL) {
    if(req->url != NULL) {
      NW_Mem_Free(req->url);
    }
    if(req->postfields != NULL) {
      NW_Mem_Free(req->postfields);
    }
    if(req->referer != NULL) {
      NW_Mem_Free(req->referer);
    }
    if (req->content != NULL) {
      NW_Mem_Free(req->content);
    }
    NW_Mem_Free(req);
  }
  return;
}

/*********************************************************
**  Name:   NW_Script_VarCons_New
**  Description:  Allocate and initialize a new variable request.
**  Parameters:   *name - pointer to the name
**  Return Value: pointer to new variable request or NULL
**********************************************************/
NW_ScriptServerVarCons_t* NW_Script_VarCons_New(const NW_Ucs2 *name)
{
  NW_ScriptServerVarCons_t *var;

  NW_ASSERT(name != NULL);

  var = (NW_ScriptServerVarCons_t*) NW_Mem_Malloc(sizeof(NW_ScriptServerVarCons_t));
  if (var != NULL) {
    var->name = NW_Str_Newcpy(name);
    if (var->name == NULL) {
      NW_Mem_Free(var);
      return NULL;
    }
  }

  return var;
}

/*********************************************************
**  Name:   NW_Script_VarCons_Free
**  Description:  Deletes a variable request
**  Parameters:   *var - pointer to the variable request
**  Return Value: void
**********************************************************/
void NW_Script_VarCons_Free(NW_ScriptServerVarCons_t *var)
{
  if (var != NULL) {
    /* note: NW_Str_Delete() handles NULL pointers */
    NW_Str_Delete(var->name);
    var->name = NULL;
    NW_Mem_Free(var);
  }
  return;
}

/*********************************************************
**  Name:   NW_Script_VarInf_New
**  Description:  Allocate and initialize a new variable request.
**  Parameters:   *name - pointer to the name
**                *value - pointer to the value
**  Return Value: pointer to new variable request or NULL
**********************************************************/
NW_ScriptServerVarInf_t* NW_Script_VarInf_New(const NW_Ucs2 *name, 
                                                const NW_Ucs2 *value)
{
  NW_ScriptServerVarInf_t* varinf;
  
  NW_ASSERT(name != NULL);/* should have a name */
 
  varinf = (NW_ScriptServerVarInf_t*) NW_Mem_Malloc(sizeof(NW_ScriptServerVarInf_t));
  if (varinf != NULL) {
    varinf->name = NW_Str_Newcpy(name);
    if (varinf->name != NULL) {
      if(value == NULL) {
        varinf->value = NULL;
        return varinf;
      } else {
        varinf->value = NW_Str_Newcpy(value);
        if (varinf->value != NULL) {
          return varinf;
        }
      }
      NW_Mem_Free(varinf->name);
    }
    NW_Mem_Free(varinf);
  }

  return NULL;
}

/*********************************************************
**  Name:   NW_Script_VarCons_Free
**  Description:  Deletes a variable request
**  Parameters:   *varinf - pointer to the variable request
**  Return Value: void
**********************************************************/
void NW_Script_VarInf_Free(NW_ScriptServerVarInf_t *varinf)
{
  if (varinf != NULL) {
    /* note: NW_Str_Delete() handles NULL pointers */
    NW_Str_Delete(varinf->name);
    NW_Str_Delete(varinf->value);
    varinf->name  = NULL;
    varinf->value = NULL;
    NW_Mem_Free(varinf);
  }
  return;
}

/*********************************************************
**  Name:   NW_Script_VarInf_SetValue
**  Description:  Set a value on the variable
**  Parameters:   *varinf - pointer to the variable request
                  value - the value of the variable
**  Return Value: 
  KBrsrSuccess
  KBrsrOutOfMemory
**********************************************************/
TBrowserStatusCode NW_Script_VarInf_SetValue(NW_ScriptServerVarInf_t *varinf, 
                                      NW_Ucs2 *value)
{
  NW_ASSERT(varinf != NULL);
  NW_ASSERT(varinf->name != NULL);/* should have a name */
  
  /* delete any previous value */
  NW_Str_Delete(varinf->value);
  varinf->value = value;

  return KBrsrSuccess;
}

/*********************************************************
**  Name:   NW_Script_DlgPrompt_Req_New
**  Description:  Allocate and initialize a new prompt dialog.
**  Parameters:   *message - pointer to the message to display
**                *input - pointer to the input to display
**  Return Value: pointer to new prompt dialog or NULL
**********************************************************/
NW_ScriptServerDlgPromptReq_t* NW_Script_DlgPrompt_Req_New(const NW_Ucs2 *message,
                                                             const NW_Ucs2 *input)
{
  NW_ScriptServerDlgPromptReq_t *dlg;

  NW_ASSERT(message != NULL);
  NW_ASSERT(input != NULL);
  
  dlg = (NW_ScriptServerDlgPromptReq_t*) NW_Mem_Malloc(sizeof(NW_ScriptServerDlgPromptReq_t));
  if (dlg != NULL) {
    dlg->message = NW_Str_Newcpy(message);
    if (dlg->message != NULL) {
      dlg->input = NW_Str_Newcpy(input);
      if (dlg->input != NULL) {
        return dlg;
      }
      NW_Str_Delete(dlg->message);
    }
    NW_Mem_Free(dlg);
  }
 
  return NULL;
}

/*********************************************************
**  Name:   NW_Script_DlgPrompt_Req_Free
**  Description:  Deletes a dialog request
**  Parameters:   *dlg - pointer to the dialog request
**  Return Value: void
**********************************************************/
void NW_Script_DlgPrompt_Req_Free(NW_ScriptServerDlgPromptReq_t *dlg)
{
  if (dlg != NULL) {
    /* note: NW_Str_Delete() handles NULL pointers */
    NW_Str_Delete(dlg->message);
    NW_Str_Delete(dlg->input);
    dlg->message  = NULL;
    dlg->input    = NULL;
    NW_Mem_Free(dlg);
  }
  return;
}


/*********************************************************
**  Name:   NW_Script_Finish_Req_New
**  Description:  Create new a finish request
**  Parameters:   
**  Return Value: NW_ScriptServerFinishReq_t* - pointer to
                  a new finish request
**********************************************************/
NW_ScriptServerFinishReq_t* NW_Script_Finish_Req_New(TBrowserStatusCode status, 
                                                     const NW_Ucs2 *message)
{
  NW_ScriptServerFinishReq_t *data = NULL;

  data = (NW_ScriptServerFinishReq_t*) NW_Mem_Malloc(sizeof(NW_ScriptServerFinishReq_t));
  if (data != NULL) {
    data->status  = status;
    data->message = NULL;
    if (message != NULL) {
      data->message = NW_Str_Newcpy(message);
    }
  }
  return data;
}

/*********************************************************
**  Name:   NW_Script_Finish_Req_Free
**  Description:  Deletes a finish request
**  Parameters:   *data - pointer to the finish request
**  Return Value: void
**********************************************************/
void NW_Script_Finish_Req_Free(NW_ScriptServerFinishReq_t *data)
{
  if (data != NULL) {
    /* note: NW_Str_Delete() handles NULL pointers */
    NW_Str_Delete(data->message);
    data->message = NULL;
    NW_Mem_Free(data);
  }
  return;
}

/*********************************************************
**  Name:   NW_Script_Finish_Req_New
**  Description:  Create a new confirm request
**  Parameters:   message - message to be displayed to the user
**                yesStr  - ok string
**                noStr   - no string  
**  Return Value: NW_ScriptServerConfirmReq_t* - pointer to
                  a new confirm request
**********************************************************/
NW_ScriptServerConfirmReq_t* NW_Script_Confirm_Req_New(const NW_Ucs2 *message,
                                                     const NW_Ucs2 *yesStr,
                                                     const NW_Ucs2 *noStr)
{
  NW_ScriptServerConfirmReq_t *data = NULL;

  data = (NW_ScriptServerConfirmReq_t*) NW_Mem_Malloc(sizeof(NW_ScriptServerConfirmReq_t));
  if (data != NULL) {
    data->message = NULL; 
    data->yesStr  = NULL;
    data->noStr   = NULL;

    if (message != NULL) {
      data->message = NW_Str_Newcpy(message);
      if (data->message == NULL) {
        NW_Mem_Free(data);
        return NULL;
      }
    }
    
    if (yesStr != NULL) {
      data->yesStr = NW_Str_Newcpy(yesStr);
      if (data->yesStr == NULL) {
        /* note: NW_Str_Delete() handles NULL pointers */
        NW_Str_Delete(data->message);
        data->message = NULL;
        NW_Mem_Free(data);
        return NULL;
      }
    }

    if (noStr != NULL) {
      data->noStr = NW_Str_Newcpy(noStr);
      if (data->noStr == NULL) {
        /* note: NW_Str_Delete() handles NULL pointers */
        NW_Str_Delete(data->message);
        NW_Str_Delete(data->yesStr);
        data->message = NULL;
        data->yesStr  = NULL;
        NW_Mem_Free(data);
        return NULL;
      }
    }
  }
  return data;
}

/*********************************************************
**  Name:   NW_Script_Confirm_Req_Free
**  Description:  Deletes a confirm request
**  Parameters:   *data - pointer to the confirm request
**  Return Value: void
**********************************************************/
void NW_Script_Confirm_Req_Free(NW_ScriptServerConfirmReq_t *data)
{
  if (data != NULL) {
    /* note: NW_Str_Delete() handles NULL pointers */
    NW_Str_Delete(data->message);
    NW_Str_Delete(data->yesStr);
    NW_Str_Delete(data->noStr);
    data->message = NULL;
    data->yesStr  = NULL;
    data->noStr   = NULL;
    NW_Mem_Free(data);
  }
  return;
}

