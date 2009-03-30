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


#ifndef NW_CSS_DEFAULTSTYLESHEETI_H
#define NW_CSS_DEFAULTSTYLESHEETI_H

#include "nw_object_basei.h"
#include "nw_css_defaultstylesheet.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */

struct NW_CSS_DefaultStyleSheet_PropEntry_s
{
  NW_Uint16 elementToken;
  NW_LMgr_PropertyName_t name;
  NW_Float32 val;
  NW_LMgr_PropertyValueType_t valType;
  NW_Byte* pattern;
};


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_CSS_DEFAULTSTYLESHEETI_H */
