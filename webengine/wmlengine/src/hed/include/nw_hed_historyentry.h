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


#ifndef NW_HED_HISTORYENTRY_H
#define NW_HED_HISTORYENTRY_H

#include "nw_object_shared.h"
#include "nw_adt_dynamicvector.h"
#include "nw_hed_urlrequest.h"
#include "nw_hed_urlresponse.h"
#include "nw_gdi_types.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_HistoryEntry_Class_s NW_HED_HistoryEntry_Class_t;
typedef struct NW_HED_HistoryEntry_s NW_HED_HistoryEntry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_HistoryEntry_ClassPart_s {
  void** unused;
} NW_HED_HistoryEntry_ClassPart_t;

struct NW_HED_HistoryEntry_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_HistoryEntry_ClassPart_t NW_HED_HistoryEntry;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_HistoryEntry_s {
  NW_Object_Dynamic_t super;

  /* private don't touch */
  NW_Object_Shared_t* sharingObject;

  /* member variables */
  NW_HED_UrlRequest_t* urlRequest;
  NW_HED_UrlResponse_t* urlResponse;
  NW_Object_Shared_t* contentData;
  
  NW_Ucs2* pageTitle;
  NW_GDI_Point2D_t docPosition;
  NW_ADT_Vector_Metric_t activeBoxId;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_HistoryEntry_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_HistoryEntry))

#define NW_HED_HistoryEntryOf(_object) \
  (NW_Object_Cast (_object, NW_HED_HistoryEntry))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_HistoryEntry_Class_t NW_HED_HistoryEntry_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
#define NW_HED_HistoryEntry_GetUrlRequest(_historyEntry) \
  (NW_OBJECT_CONSTCAST(NW_HED_UrlRequest_t*) NW_HED_HistoryEntryOf(_historyEntry)->urlRequest)

#define NW_HED_HistoryEntry_GetUrlResponse(_historyEntry) \
  (NW_OBJECT_CONSTCAST(NW_HED_UrlResponse_t*) NW_HED_HistoryEntryOf(_historyEntry)->urlResponse)

#define NW_HED_HistoryEntry_GetPageTitle(_historyEntry) \
  ((const NW_Ucs2* const) NW_HED_HistoryEntryOf(_historyEntry)->pageTitle)

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryEntry_AdoptContentData (NW_HED_HistoryEntry_t* thisObj,
                                     NW_Object_Dynamic_t* contentData);

NW_HED_EXPORT
NW_Object_Dynamic_t*
NW_HED_HistoryEntry_OrphanContentData (NW_HED_HistoryEntry_t* thisObj);

NW_HED_EXPORT
NW_Object_Shared_t*
NW_HED_HistoryEntry_GetContentDataHolder (NW_HED_HistoryEntry_t* thisObj);

NW_HED_EXPORT
NW_HED_HistoryEntry_t*
NW_HED_HistoryEntry_Clone (const NW_HED_HistoryEntry_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryEntry_Reserve (NW_HED_HistoryEntry_t* thisObj);

NW_HED_EXPORT
NW_Bool
NW_HED_HistoryEntry_Release (NW_HED_HistoryEntry_t* thisObj);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryEntry_SetPageTitle(NW_HED_HistoryEntry_t* entry, NW_Ucs2* pageTitle);

NW_HED_EXPORT
void
NW_HED_HistoryEntry_SetDocPosition(NW_HED_HistoryEntry_t* entry, 
                                   NW_GDI_Point2D_t* position);

NW_HED_EXPORT
void
NW_HED_HistoryEntry_GetDocPosition(NW_HED_HistoryEntry_t* entry, 
                                   NW_GDI_Point2D_t* position);


#define NW_HED_HistoryEntry_SetActiveBoxId(_object, _id) \
  (NW_HED_HistoryEntryOf(_object)->activeBoxId = _id)

#define NW_HED_HistoryEntry_GetActiveBoxId(_historyEntry) \
  (NW_HED_HistoryEntryOf(_historyEntry)->activeBoxId)


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_HistoryEntry_t*
NW_HED_HistoryEntry_New (NW_HED_UrlRequest_t* urlRequest, NW_HED_UrlResponse_t* urlResponse);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_HISTORYENTRY_H */
