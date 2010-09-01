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
    $Workfile: wml_var.c $

    Purpose:

        Function definitions for WML variables and WML variable lists.
  
      Definition file for varibale parsing functions
*/

#include "wml_var.h"
#include "nwx_memseg.h"
#include "nwx_string.h"
#include "nw_nvpair.h"
#include "BrsrStatusCodes.h"

/* Forward declarations for static functions */
static void ClearNextVar(NW_Wml_Var_t *var);

/* 
 * n must be valid, v may be null
 *
 *
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - n is null
 *       KBrsrOutOfMemory
 */
static TBrowserStatusCode NewVar(const NW_Ucs2 *n, const NW_Ucs2 *v, NW_Mem_Segment_Id_t mem_scope, NW_Wml_Var_t **ret_var)
{
  if (n != NULL)
  {
    if  (!NW_CheckName(n)) {
      return KBrsrWmlbrowserBadContent;
    }
    *ret_var = (NW_Wml_Var_t *)NW_Mem_Segment_Malloc(sizeof(NW_Wml_Var_t), mem_scope);
    if (*ret_var != NULL)
    {
      (*ret_var)->name = NW_Str_SegNewcpy(n, mem_scope);
      if ((*ret_var)->name != NULL)
      {
        (*ret_var)->val = NULL;
        if (v != NULL)
        {
          (*ret_var)->val = NW_Str_SegNewcpy(v, mem_scope);
          if ((*ret_var)->val == NULL)
          {
            return KBrsrOutOfMemory;
          }
        }
        return KBrsrSuccess;
      }
    }
    return KBrsrOutOfMemory;
  }
  return KBrsrFailure;
}

/*
 *
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - str = null 
 *              - str = empty string
 *              - str parsing error
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_WmlVar_ParseList(NW_Ucs2 *str, NW_Int32  *size, NW_Mem_Segment_Id_t mem_scope, NW_Wml_Var_t **ret_var)
{
  NW_Int32  i = 0, pos = 0, varNo = 0, len;
  NW_Bool nameFlag = NW_TRUE;

  *ret_var = 0;

  if ((str == NULL) || (NW_Str_Strlen(str) == 0)) 
    return KBrsrFailure;

  len = NW_Str_Strlen(str);

  /* checking how many vars there are */
  for(i = 0, *size = 1; i < len; i++) {
    if(str[i] == '&') (*size)++;
  }

  *ret_var = (NW_Wml_Var_t *)NW_Mem_Segment_Malloc(sizeof(NW_Wml_Var_t) * NW_UINT32_CAST(*size), mem_scope);
  if (*ret_var == NULL)
    return KBrsrOutOfMemory;
  
  for(i = 0, pos = 0, varNo = 0; i < len + 1; i++)
  {
    if (str[i] == '=') 
    {
      /* Parsing error ??? */
      if (nameFlag == NW_FALSE)
        return KBrsrFailure;

      (*ret_var)[varNo].name = NW_Str_SegSubstr(str, NW_UINT32_CAST(pos), 
                                                NW_UINT32_CAST(i - pos), mem_scope);

      if ((*ret_var)[varNo].name == NULL) {
        return KBrsrOutOfMemory;
      }
      
      if (!NW_CheckName((*ret_var)[varNo].name)) {
        return KBrsrWmlbrowserBadContent;
      }
  
      pos = i + 1;
      nameFlag = NW_BOOL_CAST(!nameFlag);
    }

    if ((str[i] == '&') || (str[i] == '\0')) 
    {
      /* Parsing error ??? */
      if (nameFlag)
        return KBrsrFailure;

      (*ret_var)[varNo].val = NW_Str_SegSubstr(str, NW_UINT32_CAST(pos), 
                                               NW_UINT32_CAST(i - pos), mem_scope);

      if ((*ret_var)[varNo].val == NULL)
        return KBrsrOutOfMemory;
  
      pos = i + 1;
      nameFlag = NW_BOOL_CAST(!nameFlag);
      varNo++;
    }
  }
  return KBrsrSuccess;
}

NW_Wml_Var_t *NW_WmlVar_Get(NW_Wml_VarList_t *l, const NW_Ucs2 *name)
{
  NW_Wml_Var_t *v = 0;

  if (l == NULL)
    return NULL;

  v = l->next;
  while (v != NULL)  
  {
    if (NW_Str_Strcmp(v->name, name) == 0) 
      return v;
    v = v->next;
  }
  
  return NULL;
}

NW_Wml_Var_t *NW_WmlVar_Next(NW_Wml_VarList_t *l, NW_Wml_Var_t *current)
{
  NW_Wml_Var_t *v = 0;

  if (l == NULL)
    v = NULL;
  else if (current == NULL)
    v = l;
  else
    v = current->next;

  return v;
}

NW_Bool NW_WmlVar_Remove(NW_Wml_VarList_t *l, NW_Ucs2 *name)
{
  NW_Wml_Var_t *v = 0;
  NW_Bool res = NW_FALSE;
  
  if (l == NULL)
    return NW_FALSE;

  v = l;

  while ((v != NULL) && (v->next != NULL))  
  {
    if (NW_Str_Strcmp(v->next->name, name) == 0)
    {
      res = NW_TRUE;
      v->next = v->next->next;
    }
    v = v->next;
  }
  return res;
}

NW_Ucs2 *NW_WmlVar_GetValue(NW_Wml_VarList_t *l, const NW_Ucs2 *name)
{
  NW_Wml_Var_t *v = NW_WmlVar_Get(l, name);
  
  /* Treat variables as not set if variable 
     is not defined, or is set to empty string.
     See section 10.3 of the WML 1.1 Spec. */

  if ( ( v != NULL ) &&
       ( v->val != NULL ) &&
       ( v->val[0] != 0 ) )
  {
    return v->val;
  }
  else
  {
    return NULL;
  }
}

NW_Bool NW_WmlVar_Replace(NW_Wml_VarList_t *l, NW_Wml_Var_t *param_var)
{
  NW_Wml_Var_t *list_var = NW_WmlVar_Get(l, param_var->name);
  
  if (list_var != NULL)
  { 
    list_var->val = param_var->val;
    return NW_TRUE;
  }
  else 
    return NW_FALSE;
}

void NW_WmlVar_Clear(NW_Wml_VarList_t *l)
{
  if (l == NULL)
    return;

  if (l->next != NULL)
    ClearNextVar(l->next);
  l->next = NULL;
}

static void ClearNextVar(NW_Wml_Var_t *var)
{
  if (var->next)
    ClearNextVar(var->next);
  var->val  = 0;
  var->next = 0;
  var->name = 0;
}

/*
 * RETURN: *var_list or 0 if malloc fails
 */
NW_Wml_VarList_t * NW_WmlVar_NewList(NW_Mem_Segment_Id_t mem_scope)
{
  NW_Wml_VarList_t *ret_list;

  ret_list = (NW_Wml_VarList_t *)NW_Mem_Segment_Malloc(sizeof(NW_Wml_VarList_t), mem_scope);
  if (ret_list == NULL)
    return NULL;
  
  ret_list->name = ret_list->val = 0; 
  ret_list->next = 0;

  return ret_list;
}

NW_Wml_Var_t *NW_WmlVar_PopFirst(NW_Wml_VarList_t *l) 
{
  NW_Wml_Var_t *var = l;
  NW_Wml_Var_t *prev_var = l;

  if (NW_WmlVar_IsListEmpty(l)) return 0;

  /* note that the "first" var, that is the first one put in the
     list, will be the last actual variable in the linked list */
  while(var->next) {
    prev_var = var;
    var = var->next;
  }
  
  prev_var->next = 0;
  return var;
}

void NW_WmlVar_AddVarToList(NW_Wml_VarList_t *l, NW_Wml_Var_t *v)
{
  if (!l)
    return;

  v->next = l->next;
  l->next = v;
}

NW_Bool NW_WmlVar_IsListEmpty(NW_Wml_VarList_t *l)
{
  if (!l)
    return NW_TRUE;

  return NW_BOOL_CAST(l->next == 0);
}

/*
 * RETURN: KBrsrSuccess
 *       KBrsrFailure - if name is null
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_WmlVar_AddToList(NW_Wml_VarList_t *l, const NW_Ucs2 *name, const NW_Ucs2 *value, NW_Mem_Segment_Id_t mem_scope)
{
  TBrowserStatusCode status;
  NW_Wml_Var_t *v = 0;

  status = NewVar(name, value, mem_scope, &v);
  if (status == KBrsrSuccess && v)
  {
      v->next = l->next;
      l->next = v;
  }
  return status;
}

/*
 * RETURN: KBrsrSuccess
 *       KBrsrOutOfMemory
 */
TBrowserStatusCode NW_WmlVar_ReplaceValue(NW_Wml_Var_t *v, const NW_Ucs2 *value, NW_Mem_Segment_Id_t mem_scope)
{
   if (!value) {
    v->val = NULL;
    return KBrsrSuccess;
  }

  if (v->val != NULL && (NW_Str_Strlen(value) <= NW_Str_Strlen(v->val))) {
    NW_Str_Strcpy(v->val, value);
    return KBrsrSuccess;
  }

  if (mem_scope == NW_MEM_SEGMENT_MANUAL) {
    NW_Str_Delete(v->val);
  }
  v->val = NW_Str_SegNewcpy(value, mem_scope);
  if (!v->val) {
    return KBrsrOutOfMemory;
  }

  return KBrsrSuccess;

}

/*
 * RETURN: *var_list or 0 if malloc fails
 */
NW_Wml_VarList_t *NW_WmlVar_DupList(NW_Wml_VarList_t *l, NW_Mem_Segment_Id_t mem_scope)
{
  NW_Wml_Var_t *v = 0;
  NW_Wml_VarList_t *ret_list;

  ret_list = NW_WmlVar_NewList(mem_scope);
  if (ret_list == NULL) {
    return NULL;
  }
  
  while ((v = NW_WmlVar_PopFirst(l)) != NULL)
  {
    TBrowserStatusCode status = NW_WmlVar_AddToList(ret_list, v->name, v->val, mem_scope);
    /* memory already allocated will be freed when segment is freed. */
    if (status == KBrsrOutOfMemory) {
      return NULL;
    }
    NW_ASSERT(status == KBrsrSuccess);
  }

  return ret_list;
}

