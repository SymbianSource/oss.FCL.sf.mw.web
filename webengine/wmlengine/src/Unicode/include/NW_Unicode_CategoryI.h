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


#ifndef _NW_Unicode_CategoryI_h_
#define _NW_Unicode_CategoryI_h_




#include "nw_object_corei.h"
#include "NW_Unicode_Category.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */
struct NW_Unicode_Category_TableEntry_s {
  NW_Ucs4 start;
  NW_Ucs4 end;
  NW_Uint32 increment;
};

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Unicode_Category_GetTableSize(_class) \
  ((const NW_Uint16) \
   ((NW_Unicode_Category_Class_t*) (_class))->NW_Unicode_Category.tableSize)

#define NW_Unicode_Category_GetTable(_class) \
  ((const NW_Unicode_Category_TableEntry_t* const) \
   ((NW_Unicode_Category_Class_t*) (_class))->NW_Unicode_Category.table)

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Unicode_CategoryI_h_ */
