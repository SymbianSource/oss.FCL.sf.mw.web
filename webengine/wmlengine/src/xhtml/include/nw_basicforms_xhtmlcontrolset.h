/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef NW_XHTML_XHTMLCONTROLSET_H
#define NW_XHTML_XHTMLCONTROLSET_H

#include "nw_object_dynamic.h"
#include "nw_dom_node.h"
#include "nw_adt_dynamicvector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ControlSet_Class_s NW_XHTML_ControlSet_Class_t;
typedef struct NW_XHTML_ControlSet_s NW_XHTML_ControlSet_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_ControlSet_ClassPart_s {
  void** unused;
} NW_XHTML_ControlSet_ClassPart_t;

struct NW_XHTML_ControlSet_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_XHTML_ControlSet_ClassPart_t NW_XHTML_ControlSet;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_ControlSet_s {
  NW_Object_Dynamic_t super;

  /* member variables */
  NW_ADT_DynamicVector_t* valueVector; /* values of document controls */
  NW_ADT_DynamicVector_t* nodeVector; /* DOM nodes of document controls */
  NW_ADT_DynamicVector_t* boxVector; /* DOM nodes of document controls */
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ControlSet_ClassPartOf(_object) \
  (((NW_XHTML_ControlSet_Class_t*) \
   NW_Object_ClassOf(_object))->NW_XHTML_ControlSet)

#define NW_XHTML_ControlSetOf(_object) \
  ((NW_XHTML_ControlSet_t*) (_object))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_ControlSet_Class_t NW_XHTML_ControlSet_Class;

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_XHTML_ControlSet_AddNode (NW_XHTML_ControlSet_t* controlSet,
                              NW_DOM_ElementNode_t* node,
                              NW_ADT_Vector_Metric_t* index);

extern
TBrowserStatusCode
_NW_XHTML_ControlSet_SetValue (NW_XHTML_ControlSet_t* controlSet,
                               NW_DOM_ElementNode_t* node,
                               NW_Object_t* value);

extern
NW_Object_t*
_NW_XHTML_ControlSet_GetValue (NW_XHTML_ControlSet_t* controlSet,
                               NW_DOM_ElementNode_t* node);

extern
NW_DOM_ElementNode_t*
_NW_XHTML_ControlSet_GetControl (NW_XHTML_ControlSet_t* controlSet,
                                 NW_ADT_Vector_Metric_t index);

extern
NW_ADT_DynamicVector_t*
_NW_XHTML_ControlSet_GetValueVector (NW_XHTML_ControlSet_t* controlSet);

/* ------------------------------------------------------------------------- *
   public prototypes
 * ------------------------------------------------------------------------- */
#define NW_XHTML_ControlSet_AddNode(_controlSet, _node, _index) \
  (_NW_XHTML_ControlSet_AddNode ( \
     NW_XHTML_ControlSetOf (_controlSet), (_node), (_index)))

#define NW_XHTML_ControlSet_SetValue(_controlSet, _node, _value) \
  (_NW_XHTML_ControlSet_SetValue ( \
     NW_XHTML_ControlSetOf (_controlSet), (_node), (_value)))

#define NW_XHTML_ControlSet_GetValue(_controlSet, _node) \
  (_NW_XHTML_ControlSet_GetValue ( \
     NW_XHTML_ControlSetOf (_controlSet), (_node)))

#define NW_XHTML_ControlSet_GetControl(_controlSet, _index) \
  (_NW_XHTML_ControlSet_GetControl ( \
     NW_XHTML_ControlSetOf (_controlSet), (_index)))

#define NW_XHTML_ControlSet_GetValueVector(_controlSet) \
  (_NW_XHTML_ControlSet_GetValueVector ( \
     NW_XHTML_ControlSetOf (_controlSet)))

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
NW_XHTML_ControlSet_t*
NW_XHTML_ControlSet_New (NW_ADT_DynamicVector_t* valueVector);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Tempest_XHTML_XHTMControlSet_h */
