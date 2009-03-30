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


#ifndef NW_HED_HISTORYVISITOR_H
#define NW_HED_HISTORYVISITOR_H

#include "nw_object_dynamic.h"
#include "nw_adt_dynamicvector.h"
#include "nw_hed_historystack.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_HistoryVisitor_Class_s NW_HED_HistoryVisitor_Class_t;
typedef struct NW_HED_HistoryVisitor_s NW_HED_HistoryVisitor_t;

/* ------------------------------------------------------------------------- *
   miscellaneous types
 * ------------------------------------------------------------------------- */
typedef enum NW_HED_HistoryVisitor_Location_e {
  NW_HED_HistoryVisitor_Loacation_AtBeginning,
  NW_HED_HistoryVisitor_Loacation_Current,
  NW_HED_HistoryVisitor_Loacation_AtEnd
} NW_HED_HistoryVisitor_Location_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_HistoryVisitor_ClassPart_s {
  void** unused;
} NW_HED_HistoryVisitor_ClassPart_t;

struct NW_HED_HistoryVisitor_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_HistoryVisitor_ClassPart_t NW_HED_HistoryVisitor;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_HistoryVisitor_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_HED_HistoryStack_t* stack;
  NW_HED_HistoryStack_Metric_t current;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_HistoryVisitor_GetClassPart(_object) \
  (NW_Object_GetClassPart (_object, NW_HED_HistoryVisitor))

#define NW_HED_HistoryVisitorOf(_object) \
  (NW_Object_Cast (_object, NW_HED_HistoryVisitor))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_HistoryVisitor_Class_t NW_HED_HistoryVisitor_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
const NW_HED_HistoryEntry_t* 
NW_HED_HistoryVisitor_Prev (NW_HED_HistoryVisitor_t* thisObj);

NW_HED_EXPORT
const NW_HED_HistoryEntry_t* 
NW_HED_HistoryVisitor_Next (NW_HED_HistoryVisitor_t* thisObj);

NW_HED_EXPORT
const NW_HED_HistoryEntry_t* 
NW_HED_HistoryVisitor_Current (NW_HED_HistoryVisitor_t* thisObj);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT
TBrowserStatusCode
NW_HED_HistoryVisitor_Initialize (NW_HED_HistoryVisitor_t* thisObj,
                                  NW_HED_HistoryStack_t* stack,
                                  NW_HED_HistoryVisitor_Location_t location);


NW_HED_EXPORT
NW_HED_HistoryVisitor_t*
NW_HED_HistoryVisitor_New (NW_HED_HistoryStack_t* stack,
                           NW_HED_HistoryVisitor_Location_t location);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_HED_HISTORYVISITOR_H */
