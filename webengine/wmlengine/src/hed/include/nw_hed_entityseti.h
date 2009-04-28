/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_HED_ENTITYSETI_H
#define NW_HED_ENTITYSETI_H

#include "nw_object_basei.h"
#include "nw_hed_entityset.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */
struct NW_HED_EntitySet_Entry_s {
  const char* name;
   NW_Ucs2 value;
};

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_Ucs2
_NW_HED_EntitySet_GetEntityChar (const NW_HED_EntitySet_t* entitySet,
                                 NW_Ucs2 *name);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_ENTITYSETI_H */