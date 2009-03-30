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
    $Workfile: wml_list.h $

    Purpose:

        Declaration of template functions for creating, appending, accessing and 
        deleting generic singley linked lists.
*/

#ifndef WML_LIST_H
#define WML_LIST_H

#include "nw_wml_list.h"
#include "BrsrStatusCodes.h"

/* Global function declarations */

NW_Wml_List_t *NW_WmlList_NewEmptyList(NW_Mem_Segment_Id_t memSegment);

TBrowserStatusCode NW_WmlList_Append(NW_Wml_List_t *l, void *el);

void *NW_WmlList_First(NW_Wml_List_t *l);

void *NW_WmlList_Next(NW_Wml_List_t *l);

NW_Bool NW_WmlList_IsLast(NW_Wml_List_t *l);

NW_Int32  NW_WmlList_Length(NW_Wml_List_t *l);

#if(0)
/* The following functions are not being used as of 7/31/2001. */
NW_Wml_List_t *NW_WmlList_New(void *el);
/* destroy the list not the content */ 
void NW_WmlList_Destroy(NW_Wml_List_t *l);

void *NW_WmlList_ToArray(NW_Wml_List_t *l, NW_Int32 el_size);
#endif

#endif
