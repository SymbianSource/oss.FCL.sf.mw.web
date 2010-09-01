/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class represents the marker box
*
*/

#ifndef _LMgrMarkerTextTEXT_H
#define _LMgrMarkerTextTEXT_H

#include "nw_lmgr_textbox.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */



typedef struct LMgrMarkerText_ClassPart_s {
  void** unused;
} LMgrMarkerText_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct LMgrMarkerText_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_AbstractTextBox_ClassPart_t NW_LMgr_AbstractTextBox;
  NW_LMgr_TextBox_ClassPart_t NW_LMgr_TextBox;
  LMgrMarkerText_ClassPart_t LMgrMarkerText;
} LMgrMarkerText_Class_t;

/* ------------------------------------------------------------------------- */

typedef struct LMgrMarkerText_s 
{
  NW_LMgr_TextBox_t super;
} LMgrMarkerText_t;

/* ------------------------------------------------------------------------- */
#define LMgrMarkerText_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, LMgrMarkerText))


#define LMgrMarkerTextOf(_object) \
((LMgrMarkerText_t*) (_object))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const LMgrMarkerText_Class_t LMgrMarkerText_Class;

/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
LMgrMarkerText_t*
LMgrMarkerText_New (NW_ADT_Vector_Metric_t numProperties,
                     NW_Text_t* aText);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXTBOX_H */
