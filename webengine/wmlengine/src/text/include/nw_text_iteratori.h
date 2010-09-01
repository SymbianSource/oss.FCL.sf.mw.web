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


#ifndef _NW_Text_IteratorI_h_
#define _NW_Text_IteratorI_h_

#include "nw_object_dynamici.h"
#include "nw_text_iterator.h"
#include "NW_Text_Abstract.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_Iterator_Construct (NW_Object_Dynamic_t* dynamic,
                             va_list* argList);
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Text_IteratorI_h_ */
