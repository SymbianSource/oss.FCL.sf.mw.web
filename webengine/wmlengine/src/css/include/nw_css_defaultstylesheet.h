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


#ifndef NW_CSS_DEFAULTSTYLESHEET_H
#define NW_CSS_DEFAULTSTYLESHEET_H

#include "nw_object_base.h"
#include "nw_lmgr_propertylist.h"
#include <nw_dom_element.h>
#include <nw_wbxml_dictionary.h>
#include "nw_evt_event.h"
#include "nw_lmgr_box.h"
#include "nw_hed_documentnode.h"
#include "nw_hed_domhelper.h"
#include "NW_CSS_EXPORT.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */

typedef struct NW_CSS_DefaultStyleSheet_Class_s NW_CSS_DefaultStyleSheet_Class_t;
typedef struct NW_CSS_DefaultStyleSheet_s NW_CSS_DefaultStyleSheet_t;

typedef struct NW_CSS_DefaultStyleSheet_PropEntry_s NW_CSS_DefaultStyleSheet_PropEntry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_CSS_DefaultStyleSheet_ClassPart_s {
  NW_Uint32 numPropertyVals;
  const NW_CSS_DefaultStyleSheet_PropEntry_t* propertyVals;
  NW_Uint32 numStringVals;
  const NW_Ucs2* const * stringVals;
} NW_CSS_DefaultStyleSheet_ClassPart_t;

struct NW_CSS_DefaultStyleSheet_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_CSS_DefaultStyleSheet_ClassPart_t NW_CSS_DefaultStyleSheet;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_CSS_DefaultStyleSheet_s {
  NW_Object_Core_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_CSS_DefaultStyleSheet_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_CSS_DefaultStyleSheet))


#define NW_CSS_DefaultStyleSheetOf(_object) \
  (NW_Object_Cast (_object, NW_CSS_DefaultStyleSheet))

 /* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_CSS_EXPORT const NW_CSS_DefaultStyleSheet_Class_t NW_CSS_DefaultStyleSheet_Class;
NW_CSS_EXPORT const NW_CSS_DefaultStyleSheet_t NW_CSS_DefaultStyleSheet;

/* -------------------------------------------------------------------------- *
    final methods
 * ---------------------------------------------------------------------------*/

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_CSS_DEFAULTSTYLESHEET_H */
