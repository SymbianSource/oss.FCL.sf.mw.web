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


#ifndef NW_HED_HISTORYSTACK_H
#define NW_HED_HISTORYSTACK_H

#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "nw_hed_historyentry.h"
#include "BrsrStatusCodes.h"

#include "HistoryStack.h"


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_HistoryStack_Class_s NW_HED_HistoryStack_Class_t;
typedef struct NW_HED_HistoryStack_s NW_HED_HistoryStack_t;
class CWmlControl;
/* ------------------------------------------------------------------------- *
   miscellaneous types
 * ------------------------------------------------------------------------- */
typedef NW_Int8 NW_HED_HistoryStack_Metric_t;

typedef enum NW_HED_HistoryStack_Direction_e {
  NW_HED_HistoryStack_Direction_Previous,
  NW_HED_HistoryStack_Direction_Current,
  NW_HED_HistoryStack_Direction_Next,
  NW_HED_HistoryStack_Direction_RestorePrevious,
  NW_HED_HistoryStack_Direction_RestoreNext
} NW_HED_HistoryStack_Direction_t;

typedef enum NW_HED_HistoryStack_Location_e {
  NW_HED_HistoryStack_Loacation_AtBeginning,
  NW_HED_HistoryStack_Loacation_BeforeCurrent,
  NW_HED_HistoryStack_Loacation_AtEnd
} NW_HED_HistoryStack_Location_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_HistoryStack_ClassPart_s {
  void** unused;
} NW_HED_HistoryStack_ClassPart_t;

struct NW_HED_HistoryStack_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_HistoryStack_ClassPart_t NW_HED_HistoryStack;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_HistoryStack_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_ADT_DynamicVector_t* vector;
  NW_HED_HistoryStack_Metric_t current;
  CWmlControl* iWmlCtrl;
  TInt iReason;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_HistoryStack_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_HistoryStack))

#define NW_HED_HistoryStackOf(_object) \
  (NW_Object_Cast (_object, NW_HED_HistoryStack))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_HistoryStack_Class_t NW_HED_HistoryStack_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_HED_HistoryEntry_t*
NW_HED_HistoryStack_DeleteEntry (NW_HED_HistoryStack_t* thisObj,
                                 NW_HED_HistoryStack_Metric_t index);


NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryStack_SetCurrent (NW_HED_HistoryStack_t* thisObj,
                                NW_HED_HistoryStack_Direction_t direction);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryStack_SetCurrentEntry(NW_HED_HistoryStack_t* thisObj,
                                    NW_HED_HistoryEntry_t* entry);

NW_HED_EXPORT
NW_HED_HistoryEntry_t*
NW_HED_HistoryStack_GetEntry (NW_HED_HistoryStack_t* thisObj,
                              NW_HED_HistoryStack_Direction_t direction);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryStack_PushEntry (NW_HED_HistoryStack_t* thisObj,
                               NW_HED_HistoryEntry_t* historyEntry);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryStack_InsertEntry (NW_HED_HistoryStack_t* thisObj,
                                 NW_HED_HistoryEntry_t* historyEntry,
                                 NW_HED_HistoryStack_Location_t location);

NW_HED_EXPORT
NW_HED_HistoryEntry_t*
NW_HED_HistoryStack_RemoveEntry (NW_HED_HistoryStack_t* thisObj,
                                 NW_HED_HistoryStack_Direction_t direction);

NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryStack_DeleteAll (NW_HED_HistoryStack_t* thisObj);

NW_HED_EXPORT
NW_Bool
NW_HED_HistoryStack_IsUrlinHistory (NW_HED_HistoryStack_t* thisObj, 
                                    NW_HED_UrlRequest_t* urlRequest);

NW_HED_EXPORT
NW_Uint8
NW_HED_HistoryStack_Length (NW_HED_HistoryStack_t* thisObj);

NW_HED_EXPORT
NW_HED_HistoryEntry_t*
NW_HED_HistoryStack_GetEntryByIndex(NW_HED_HistoryStack_t* thisObj,
                                    NW_Uint8 historyIndex);

NW_HED_EXPORT
NW_HED_HistoryStack_Metric_t
NW_HED_HistoryStack_GetIndex (NW_HED_HistoryStack_t* thisObj,
                              NW_HED_HistoryStack_Direction_t direction);

NW_HED_EXPORT
void
SetOssHistoryControl(NW_HED_HistoryStack_t* thisObj, CWmlControl* aWmlControl);

NW_HED_EXPORT
void
SetUrlRequestReason(NW_HED_HistoryStack_t* thisObj, TInt aReason);

NW_Bool
NW_HED_HistoryStack_ValidateEntry (const NW_HED_HistoryEntry_t* historyEntry);

NW_Int8
NW_HED_HistoryStack_GetIndexByEntry (NW_HED_HistoryStack_t* thisObj,
                                     NW_HED_HistoryEntry_t* historyEntry);

THistoryStackDirection KimonoToOssDirection(NW_HED_HistoryStack_Direction_t aDirection);

void
SetPossibleWmlOEB(NW_HED_HistoryStack_t* thisObj, NW_Bool flag);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
NW_HED_HistoryStack_t*
NW_HED_HistoryStack_New (void);

#endif /* NW_HED_HISTORYSTACK_H */
