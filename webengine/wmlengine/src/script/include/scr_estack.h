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
    $Workfile: scr_estack.h $

    Purpose:
        This file is used by the scr_estack.c.  It defines the prototypes 
        for all the functions used by the execution stack.

 */

#ifndef SCR_estack_H
#define SCR_estack_H

#include "scr_val.h"

typedef struct {
  val_t     *ops;
  NW_Int32  capacity;
  NW_Int32  size;
} estack_t;

NW_Bool create_estack(void);

val_t pop_estack(void);

void pop_estack_and_free(void);

void push_estack(val_t v);

void push_estack_and_free(val_t *vptr);

NW_Bool is_estack_empty(void);

void free_estack(estack_t *st);

#endif  /*SCR_COMMON_INFO_H*/
