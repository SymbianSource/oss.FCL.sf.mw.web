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


#ifndef NW_Text_AbstractI_h
#define NW_Text_AbstractI_h

#include "nw_object_dynamici.h"
#include "NW_Text_Abstract.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Text_Abstract_GetStorage(_abstractText) \
  ((void* ) NW_Text_AbstractOf (_abstractText)->storage)

#define NW_Text_Abstract_GetStorageSize(_abstractText) \
  (NW_Object_Invoke (_abstractText, NW_Text_Abstract, getStorageSize) ( \
     NW_Text_AbstractOf (_abstractText)))

#define NW_Text_Abstract_CalcCharacterCount(_abstractText) \
  (_NW_Text_Abstract_CalcCharacterCount (NW_Text_AbstractOf (_abstractText)))

#define NW_Text_Abstract_SetStorage(_abstractText, _storage, _characterCount, _flags) \
  (_NW_Text_Abstract_SetStorage (NW_Text_AbstractOf (_abstractText), \
                                 (_storage), (_characterCount), (_flags)))

/* ------------------------------------------------------------------------- *
   protected method prototypes
 * ------------------------------------------------------------------------- */
NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_Abstract_Construct (NW_Object_Dynamic_t* dynamicObject,
                             va_list* argList);

NW_TEXT_EXPORT
void
_NW_Text_Abstract_Destruct (NW_Object_Dynamic_t* dynamicObject);

NW_TEXT_EXPORT
TBrowserStatusCode
_NW_Text_Abstract_CalcCharacterCount (NW_Text_Abstract_t* thisObj);

NW_TEXT_EXPORT
NW_Ucs2*
_NW_Text_Abstract_GetUCS2Buffer (const NW_Text_Abstract_t* thisObj,
                                 NW_Uint16 flags,
                                 NW_Text_Length_t* charCount,
                                 NW_Bool* freeNeeded);
NW_Text_Length_t
_NW_Text_Abstract_GetSpaceAfter (const NW_Text_Abstract_t* thisObj,
                                 NW_Text_Length_t index);
#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_Text_AbstractI_h */
