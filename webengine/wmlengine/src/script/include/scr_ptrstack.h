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
    $Workfile: scr_ptrstack.h $

    Purpose:

       This file is used by the scr_ptrstack.c.  It defines the structure and
       the prototypes for all the functions used by the ptrstack.
 */
 
#ifndef SCR_PTRSTACK_H
#define SCR_PTRSTACK_H

#include "scr_defs.h"

typedef struct {
  void      **s;
  NW_Int32  size;
  NW_Int32  capacity;
} pstack_t;


NW_Bool is_pstack_empty(pstack_t *st);
NW_Bool is_pstack_full(pstack_t *st);
pstack_t *new_pstack(NW_Int32  cap);
void *pop_pstack(pstack_t *st);
void *peek_pstack(pstack_t *st);
void push_pstack(pstack_t *st, void *v);
void destroy_pstack(pstack_t *st);

void *el_at_pstack(pstack_t *st, NW_Int32  index);

#endif  /*SCR_PTRSTACK_H*/




