/*
* Copyright (c) 2000-2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_MARKUP_WMLVALIDATORI_H
#define NW_MARKUP_WMLVALIDATORI_H

#include "NW_FBox_ValidatorI.h"
#include "nw_markup_wmlvalidator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_WmlValidator_Construct (NW_Object_Dynamic_t* dynamicObject,
                                   va_list* argp);

NW_MARKUP_EXPORT
void
_NW_Markup_WmlValidator_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_WmlValidator_Validate(NW_FBox_Validator_t* validator,
                                 const NW_Text_t* string);

NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_WmlValidator_GetFormat(NW_FBox_Validator_t* validator,
                                  const NW_Ucs2** format,
                                  NW_FBox_Validator_Mode_t* mode,
                                  NW_FBox_Validator_EmptyOk_t* emptyOk);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* NW_MARKUP_WMLVALIDATORI_H */
