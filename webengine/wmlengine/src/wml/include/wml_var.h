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
    $Workfile: wml_var.h $

    Purpose:

        Function and structure declaration for WML variables and WML variable
        lists. The core browser manages user defined variables while processing WML
        documents.
*/

#ifndef WML_VARS_H
#define WML_VARS_H

#include "nw_wml_var.h"
#include "BrsrStatusCodes.h"

/* Global function declarations */

TBrowserStatusCode NW_WmlVar_ParseList(NW_Ucs2 *str, NW_Int32  *size, NW_Mem_Segment_Id_t mem_scope, NW_Wml_Var_t **ret_var);

NW_Wml_VarList_t * NW_WmlVar_NewList(NW_Mem_Segment_Id_t mem_scope);

TBrowserStatusCode NW_WmlVar_AddToList(NW_Wml_VarList_t *l, const NW_Ucs2 *n, 
                                const NW_Ucs2 *v, NW_Mem_Segment_Id_t mem_scope);

TBrowserStatusCode NW_WmlVar_ReplaceValue(NW_Wml_Var_t *v, const NW_Ucs2 *value, NW_Mem_Segment_Id_t mem_scope);

NW_Wml_VarList_t * NW_WmlVar_DupList(NW_Wml_VarList_t *l, NW_Mem_Segment_Id_t mem_scope);


NW_Ucs2 *NW_WmlVar_GetValue(NW_Wml_VarList_t *l, const NW_Ucs2 *name);

NW_Bool NW_WmlVar_Replace(NW_Wml_VarList_t *l, NW_Wml_Var_t *param_var);

NW_Wml_Var_t *NW_WmlVar_Get(NW_Wml_VarList_t *l, const NW_Ucs2 *name);

NW_Wml_Var_t *NW_WmlVar_Next(NW_Wml_VarList_t *l, NW_Wml_Var_t *current);

NW_Bool NW_WmlVar_Remove(NW_Wml_VarList_t *l, NW_Ucs2 *name);

void NW_WmlVar_Clear(NW_Wml_VarList_t *l);

NW_Wml_Var_t *NW_WmlVar_PopFirst(NW_Wml_VarList_t *l);

void NW_WmlVar_AddVarToList(NW_Wml_VarList_t *l, NW_Wml_Var_t *v);

NW_Bool NW_WmlVar_IsListEmpty(NW_Wml_VarList_t *l);

#endif /* WML_VARS_H */
