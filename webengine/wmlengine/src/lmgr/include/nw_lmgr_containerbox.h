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


#ifndef NW_CONTAINERBOX_H
#define NW_CONTAINERBOX_H

#include "nw_lmgr_box.h"
#include "nw_lmgr_boxvisitor.h"
#include "nw_adt_dynamicvector.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  
/* Container boxes are boxes with children */
/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ContainerBox_Class_s NW_LMgr_ContainerBox_Class_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_ContainerBox_ClassPart_s {
  void** unused;
} NW_LMgr_ContainerBox_ClassPart_t;

struct NW_LMgr_ContainerBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
};

/* ------------------------------------------------------------------------- */
struct NW_LMgr_ContainerBox_s {
  NW_LMgr_Box_t super;

  /* member variables */
  NW_ADT_DynamicVector_t* children;
};

/* ------------------------------------------------------------------------- */

#define NW_LMgr_ContainerBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_ContainerBox))

#define NW_LMgr_ContainerBoxOf(object) \
  (NW_Object_Cast (object, NW_LMgr_ContainerBox))

/* Final Methods ---------------------------------------------------------- */
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_InsertChildAt (NW_LMgr_ContainerBox_t* container,
                                 NW_LMgr_Box_t* child,
                                 NW_ADT_Vector_Metric_t where);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_AddChild (NW_LMgr_ContainerBox_t* containerBox,
                               NW_LMgr_Box_t* child);

NW_LMGR_EXPORT
NW_ADT_Vector_Metric_t
NW_LMgr_ContainerBox_GetChildCount (NW_LMgr_ContainerBox_t* containerBox);

NW_LMGR_EXPORT
NW_LMgr_Box_t*
NW_LMgr_ContainerBox_GetChild (NW_LMgr_ContainerBox_t* containerBox,
                               NW_ADT_Vector_Metric_t index);

NW_LMGR_EXPORT
NW_LMgr_Box_t*
NW_LMgr_ContainerBox_SplitAtIndex (NW_LMgr_ContainerBox_t* container,
                                   NW_ADT_Vector_Metric_t index);


#define NW_LMgr_ContainerBox_GetChildren(_containerBox) \
  ((const NW_ADT_Vector_t*) (_containerBox)->children)

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_InsertChild (NW_LMgr_ContainerBox_t* container,
                                  NW_LMgr_Box_t* child,
                                  NW_LMgr_Box_t* where);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_InsertChildAft (NW_LMgr_ContainerBox_t* container,
                                     NW_LMgr_Box_t* child,
                                     NW_LMgr_Box_t* where);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_AppendChildrenOf (NW_LMgr_ContainerBox_t* container,
                                       NW_LMgr_ContainerBox_t* source);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_ReplaceChild (NW_LMgr_ContainerBox_t* container,
                                   NW_LMgr_Box_t* originalChild,
                                   NW_LMgr_Box_t* newChild);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_RemoveChild (NW_LMgr_ContainerBox_t* containerBox,
                                  NW_LMgr_Box_t* child);
NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_RemoveChildren (NW_LMgr_ContainerBox_t* thisObj);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_DeleteChildren (NW_LMgr_ContainerBox_t* thisObj);

NW_LMGR_EXPORT
NW_ADT_Vector_Metric_t
NW_LMgr_ContainerBox_GetChildIndex (NW_LMgr_ContainerBox_t* container,
                                    NW_LMgr_Box_t* child);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_ResizeToChildren(NW_LMgr_ContainerBox_t* container);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_Initialize(NW_LMgr_ContainerBox_t *container);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_InitializeSSL(NW_LMgr_ContainerBox_t *container);

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_ContainerBox_Collapse (NW_LMgr_ContainerBox_t* thisObj);

NW_Bool NW_LMgr_ContainerBox_IsLastChild(NW_LMgr_Box_t* box);

#define NW_LMgr_ContainerBox_GetBoxVisitor(_container) \
  (NW_LMgr_BoxVisitor_New (NW_LMgr_BoxOf(_container)))

/*-------------------------------------------------------------------------*/
NW_LMGR_EXPORT const NW_LMgr_ContainerBox_Class_t NW_LMgr_ContainerBox_Class;

/* ------------------------------------------------------------------------- *
  convenience functions
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_ContainerBox_t*
NW_LMgr_ContainerBox_New (NW_ADT_Vector_Metric_t numProperties);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_CONTAINERBOX_H */
