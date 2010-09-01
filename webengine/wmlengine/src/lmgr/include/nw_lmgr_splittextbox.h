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


#ifndef NW_SPLITTEXTBOX_H
#define NW_SPLITTEXTBOX_H

#include "nw_lmgr_abstracttextbox.h"
#include "NW_Text_Abstract.h"

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* Text boxes hold text content.*/

typedef struct NW_LMgr_SplitTextBox_ClassPart_s {
  void** unused;
} NW_LMgr_SplitTextBox_ClassPart_t;

/* ------------------------------------------------------------------------- */
typedef struct NW_LMgr_SplitTextBox_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_LMgr_Box_ClassPart_t NW_LMgr_Box;
  NW_LMgr_MediaBox_ClassPart_t NW_LMgr_MediaBox;
  NW_LMgr_AbstractTextBox_ClassPart_t NW_LMgr_AbstractTextBox;
  NW_LMgr_SplitTextBox_ClassPart_t NW_LMgr_SplitTextBox;
} NW_LMgr_SplitTextBox_Class_t;

/* ------------------------------------------------------------------------- */

typedef struct NW_LMgr_SplitTextBox_s {
  NW_LMgr_AbstractTextBox_t super;
  NW_LMgr_AbstractTextBox_t *prevBox;
} NW_LMgr_SplitTextBox_t;

/* ------------------------------------------------------------------------- */
#define NW_LMgr_SplitTextBox_GetClassPart(_object) \
(NW_Object_GetClassPart(_object, NW_LMgr_SplitTextBox))


#define NW_LMgr_SplitTextBoxOf(_object) \
((NW_LMgr_SplitTextBox_t*) (_object))

/* ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_LMgr_SplitTextBox_Class_t NW_LMgr_SplitTextBox_Class;


/* ------------------------------------------------------------------------- *
   convenience methods
 * ------------------------------------------------------------------------- */
NW_LMgr_SplitTextBox_t*
NW_LMgr_SplitTextBox_New (NW_LMgr_AbstractTextBox_t* textBox,
                          NW_Text_Length_t length);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_SPLITTEXTBOX_H */
