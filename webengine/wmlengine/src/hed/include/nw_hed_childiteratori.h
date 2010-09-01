/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _NW_HED_ChildIteratorI_h_
#define _NW_HED_ChildIteratorI_h_

#include "nw_adt_mapiteratori.h"
#include "nw_hed_childiterator.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
void**
_NW_HED_ChildIterator_NextElement (NW_ADT_Iterator_t* iterator);

NW_HED_EXPORT
NW_Uint32
_NW_HED_ChildIterator_GetElementSize (NW_ADT_Iterator_t* iterator);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_ChildIteratorI_h_ */
