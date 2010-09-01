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
    $Workfile: nw_wml_var.h $

    Purpose:

        Function and structure declaration for WML variables and WML variable
        lists. The core browser manages user defined variables while processing WML
        documents.
*/

#ifndef NW_WML_VARS_H
#define NW_WML_VARS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nwx_defs.h"
#include "nwx_memseg.h"
#include <nwx_status.h>

typedef struct _NW_Wml_Var_t NW_Wml_Var_t;
struct _NW_Wml_Var_t {
  NW_Wml_Var_t  *next;
  NW_Ucs2     *name;
  NW_Ucs2     *val;
};

typedef NW_Wml_Var_t NW_Wml_VarList_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_WML_VARS_H */
