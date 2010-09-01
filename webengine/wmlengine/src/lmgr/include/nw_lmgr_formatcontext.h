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


#ifndef NW_FORMATCONTEXT_H
#define NW_FORMATCONTEXT_H

#include "nw_lmgr_box_defn.h"
#include "nw_lmgr_linebox_defn.h"
//#include "nw_lmgr_box.h"
#include "nw_gdi_types.h"
#include "nw_object_dynamic.h"
#include "NW_Text_Abstract.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */




/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_FormatContext_Class_s NW_LMgr_FormatContext_Class_t;
typedef struct NW_LMgr_FormatContext_s NW_LMgr_FormatContext_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_LMgr_FormatContext_ClassPart_s {
  void** unused;
} NW_LMgr_FormatContext_ClassPart_t;

struct NW_LMgr_FormatContext_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_FormatContext_ClassPart_t NW_LMgr_FormatContext;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_LMgr_FormatContext_s {
  NW_Object_Dynamic_t super;

  /* Member variables */
  NW_Uint8  flags;
  NW_Uint32 listCount;                    /* Current ordered list item */
  NW_Uint32 itemsHandled;                 /* The number of list items handled */
  NW_GDI_Metric_t x;                      /* Layout the next element   */    
  NW_GDI_Metric_t y;                      /* starting at x,y (offsets) */
  NW_LMgr_LineBox_t* currentLine;         /* Current line buffer */     
  NW_Uint16 lineNumber;
  NW_GDI_Metric_t startMargin;            /* Current margin where formatting starts */
  NW_GDI_Metric_t endMargin;              /* Current margin where formatting breaks */
  NW_GDI_Metric_t lineStart;              /* Where the current line starts */
  NW_GDI_Metric_t lineEnd;                /* Where the current line ends */
  NW_GDI_Metric_t pendingMargin;          /* Pending vertical margin */
  NW_GDI_Metric_t pendingInlineMargin;    /* Pending inline left margin */
  NW_Bool pendingWhitespace;              /* Any pending WS? */
  NW_GDI_FlowDirection_t direction;
  NW_ADT_DynamicVector_t *placedFloats;
  NW_ADT_DynamicVector_t *pendingFloats;

  NW_Bool formatChildren; /* is NW_TRUE if the box has the children to be formatted*/
  NW_LMgr_Box_t* formatBox;/* the formatbox to be used to format the children*/
  NW_LMgr_FormatContext_t* newFormatContext;/* new formatcontext to be used to format the children*/
  NW_Uint8  referenceCount;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_LMgr_FormatContext_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_LMgr_FormatContext))

#define NW_LMgr_FormatContextOf(_object) \
  (NW_Object_Cast (_object, NW_LMgr_FormatContext))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_FormatContext_Class_t NW_LMgr_FormatContext_Class;

#define NW_LMgr_FlowCtxt_Flags_trailingWS          (NW_Uint8)1

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

NW_LMGR_EXPORT
NW_LMgr_FormatContext_t * 
NW_LMgr_FormatContext_New(void);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_FORMATCONTEXT_H */
