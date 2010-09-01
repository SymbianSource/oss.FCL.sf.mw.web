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


#ifndef NW_BROWSER_BROWSERMIMETABLE_H
#define NW_BROWSER_BROWSERMIMETABLE_H

#include "nw_hed_mimetable.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Browser_MimeTable_Class_s NW_Browser_MimeTable_Class_t;
typedef struct NW_Browser_MimeTable_s NW_Browser_MimeTable_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Browser_MimeTable_ClassPart_s {
  void** unused;
} NW_Browser_MimeTable_ClassPart_t;

struct NW_Browser_MimeTable_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_HED_MimeTable_ClassPart_t NW_HED_MimeTable;
  NW_Browser_MimeTable_ClassPart_t NW_Browser_MimeTable;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Browser_MimeTable_s {
  NW_HED_MimeTable_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Browser_MimeTable_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_Browser_MimeTable))

#define NW_Browser_MimeTableOf(_object) \
  (NW_Object_Cast (_object, NW_Browser_MimeTable))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Browser_MimeTable_Class_t NW_Browser_MimeTable_Class;
extern const NW_Browser_MimeTable_t NW_Browser_MimeTable;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_BROWSER_BROWSERMIMETABLEI_H */
