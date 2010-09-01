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


#ifndef NW_LINEBOX_DEFN_H
#define NW_LINEBOX_DEFN_H

#include "nw_lmgr_box_defn.h"
#include "nw_adt_dynamicvector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Line boxes are currently implemented as if they were simply container boxes.
 * However, they have not been subclassed from container because they can, and
 * will eventually be, implemented in a much more light-weight manner
 */

typedef struct NW_LMgr_LineBox_s NW_LMgr_LineBox_t;



/* ------------------------------------------------------------------------- */
struct NW_LMgr_LineBox_s {
  NW_LMgr_Box_t super;

  /* member variables */
  NW_ADT_DynamicVector_t *line;
};
  
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 







































