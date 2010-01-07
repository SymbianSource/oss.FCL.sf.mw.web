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
    $Workfile: nw_wml_list.h $

    Purpose:

        Declaration of template functions for creating, appending, accessing and 
        deleting generic singley linked lists.
*/

#ifndef NW_WML_LIST_H
#define NW_WML_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nwx_defs.h"
#include <nwx_status.h>
#include "nwx_memseg.h"

/* A single linked list with a dummy element first */

typedef struct _NW_Wml_Node_t NW_Wml_Node_t;
struct _NW_Wml_Node_t {
  NW_Wml_Node_t   *next;
  void            *data;
};

typedef struct {
  NW_Wml_Node_t       *first;
  NW_Wml_Node_t       *last;
  NW_Wml_Node_t       *cursor;
  NW_Mem_Segment_Id_t memSegment;
} NW_Wml_List_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NW_WML_LIST_H */
