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


#ifndef NW_TEXTBOX_H
#define NW_TEXTBOX_H

#include "nw_lmgr_abstracttextbox.h"
#include "NW_Text_Abstract.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Text boxes hold text content.*/

typedef struct NW_LMgr_TextBox_ClassPart_s {
  void** unused;
} NW_LMgr_TextBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_TextBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_AbstractTextBox_ClassPart_t NW_LMgr_AbstractTextBox;
  NW_LMgr_TextBox_ClassPart_t NW_LMgr_TextBox;
} NW_LMgr_TextBox_Class_t;

/* ------------------------------------------------------------------------- */

typedef struct NW_LMgr_TextBox_s {
  NW_LMgr_AbstractTextBox_t super;
  NW_Text_t *text;
} NW_LMgr_TextBox_t;

/* ------------------------------------------------------------------------- */
#define NW_LMgr_TextBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_TextBox))


#define NW_LMgr_TextBoxOf(_object) \
((NW_LMgr_TextBox_t*) (_object))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_TextBox_Class_t NW_LMgr_TextBox_Class;

/* ------------------------------------------------------------------------- */
#define NW_LMgr_TextBox_SetText(_object, _text)\
  ((_object)->text = (_text))

#define NW_LMgr_TextBox_GetText(_object)\
  ((_object)->text)

NW_LMGR_EXPORT
TBrowserStatusCode
NW_LMgr_TextBox_Collapse (NW_LMgr_TextBox_t* thisObj);
/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT
NW_LMgr_TextBox_t*
NW_LMgr_TextBox_New (NW_ADT_Vector_Metric_t numProperties,
                     NW_Text_t* text);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_TEXTBOX_H */
