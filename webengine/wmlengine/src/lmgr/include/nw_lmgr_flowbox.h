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


#ifndef NW_FLOWBOX_H
#define NW_FLOWBOX_H

#include "nw_adt_dynamicvector.h"
#include "nw_lmgr_formatbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* 
* Flow boxes manage flows. These are boxes within which other boxes
* can be laid out using some kind of flow: left-right flow, table
* flow, etc. Internally, flow box implementations call the "flowable"
* interface methods on objects to add them to the flow. One could
* actually further subclass to have flow-type flow boxes (LR, top-bottom,
* etc.) and other types (table, etc.).
  */
  
typedef struct NW_LMgr_FlowBox_s NW_LMgr_FlowBox_t;
  
/*
 * This method is used to glue an inline box into the flow. 
 * This may result in splitting the box over more than one line.
 */
  
typedef
TBrowserStatusCode
(*NW_LMgr_FlowBox_FlowInline_t)( NW_LMgr_FlowBox_t *flow, 
                                NW_LMgr_Box_t *box,
                                NW_LMgr_FormatContext_t *formatContext);
  
/* This method is used to put a block box into the flow */
  
typedef
TBrowserStatusCode
(*NW_LMgr_FlowBox_FlowBlock_t)( NW_LMgr_FlowBox_t *flow, 
                               NW_LMgr_Box_t *box,
                               NW_LMgr_FormatContext_t *formatContext);
  
typedef
TBrowserStatusCode
(*NW_LMgr_FlowBox_ListItem_t)( NW_LMgr_FlowBox_t* flow, 
                               NW_LMgr_Box_t *item,
                               NW_LMgr_FormatContext_t *context);
  
typedef
TBrowserStatusCode
(*NW_LMgr_FlowBox_LFloat_t)( NW_LMgr_FlowBox_t* flow, 
                             NW_LMgr_Box_t *box,
                             NW_LMgr_FormatContext_t *context);
  
/* This method moves the flow to a new line */
  
typedef
TBrowserStatusCode
(*NW_LMgr_FlowBox_NewLine_t)( NW_LMgr_FlowBox_t *flow,
                              NW_LMgr_FormatContext_t *context,
                              NW_GDI_Metric_t delta,
							  NW_Bool indentNewLine);
  
  
/* Class declarations -------------------------------------------------------*/
  
typedef struct NW_LMgr_FlowBox_ClassPart_s {
  void** unused;
} NW_LMgr_FlowBox_ClassPart_t;
  
typedef struct NW_LMgr_FlowBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_ContainerBox_ClassPart_t NW_LMgr_ContainerBox;
  NW_LMgr_FormatBox_ClassPart_t NW_LMgr_FormatBox;
  NW_LMgr_FlowBox_ClassPart_t NW_LMgr_FlowBox;
} NW_LMgr_FlowBox_Class_t;

/* ------------------------------------------------------------------------- */
struct NW_LMgr_FlowBox_s {
  NW_LMgr_FormatBox_t super;
};
  
/* ------------------------------------------------------------------------- */

#define NW_LMgr_FlowBox_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_FlowBox))

#define NW_LMgr_FlowBoxOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_FlowBox))

  /* ------------------------------------------------------------------------- */

#define NW_LMgr_FlowBox_FlowInline(_object, _box, _context) \
  (NW_LMgr_FlowBox_GetClassPart (_object).flowInline ((_object), (_box), \
                                                     (_context)))

#define NW_LMgr_FlowBox_FlowBlock(_object, _box, _context) \
  (NW_LMgr_FlowBox_GetClassPart (_object).flowBlock ((_object), (_box), \
                                                    (_context)))

#define NW_LMgr_FlowBox_ListItem(_object, _box, _context) \
  (NW_LMgr_FlowBox_GetClassPart (_object).listItem ((_object), (_box), (_context)))

#define NW_LMgr_FlowBox_LFloat(_object, _box, _context) \
  (NW_LMgr_FlowBox_GetClassPart (_object).lFloat ((_object), (_box), (_context)))

#define NW_LMgr_FlowBox_NewLine(_object, _context, _delta, _indentNewLine) \
  (NW_LMgr_FlowBox_GetClassPart (_object).newLine ((_object), (_context), (_delta), (_indentNewLine)))
  
/* ------------------------------------------------------------------------- */

NW_LMGR_EXPORT const NW_LMgr_FlowBox_Class_t NW_LMgr_FlowBox_Class;

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif 
