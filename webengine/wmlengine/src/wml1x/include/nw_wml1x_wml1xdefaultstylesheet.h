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


#ifndef NW_WML1X_WML1XDEFAULTSTYLESHEET_H
#define NW_WML1X_WML1XDEFAULTSTYLESHEET_H

#include "nw_lmgr_box.h"
#include "nw_lmgr_activecontainerbox.h"
#include "nw_lmgr_propertylist.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* Includes */

void NW_Wml1x_ParagraphDefaultStyle (NW_LMgr_Property_t     *prop,
                                     NW_LMgr_Box_t *box);

void NW_Wml1x_BigDefaultStyle(NW_LMgr_Property_t* prop, 
                         NW_LMgr_ContainerBox_t* containerBox);

void NW_Wml1x_SmallDefaultStyle(NW_LMgr_Property_t* prop,
                           NW_LMgr_ContainerBox_t* containerBox);

void NW_Wml1x_StrongDefaultStyle(NW_LMgr_Property_t* prop,
                            NW_LMgr_ContainerBox_t* containerBox);

void NW_Wml1x_BoldDefaultStyle(NW_LMgr_Property_t* prop,
                          NW_LMgr_ContainerBox_t* containerBox);

void NW_Wml1x_EmphasisDefaultStyle(NW_LMgr_Property_t* prop,
                              NW_LMgr_ContainerBox_t* containerBox);

void NW_Wml1x_ItalicDefaultStyle(NW_LMgr_Property_t* prop,
                            NW_LMgr_ContainerBox_t* containerBox);

void NW_Wml1x_UnderlineDefaultStyle(NW_LMgr_Property_t* prop,
                               NW_LMgr_ContainerBox_t* containerBox);


void NW_Wml1x_DefaultStyle(NW_LMgr_Property_t* prop,
                           NW_LMgr_ContainerBox_t* containerBox);

void NW_Wml1x_ImageWithinAnchorDefaultStyle(NW_LMgr_Property_t* prop,
                                            NW_LMgr_Box_t* box);

void NW_Wml1x_DefaultCardStyle(NW_LMgr_Property_t* prop,
                           NW_LMgr_ContainerBox_t* containerBox);

TBrowserStatusCode NW_Wml1x_AnchorDefaultStyle(NW_LMgr_Property_t* prop,
                                        NW_LMgr_ActiveContainerBox_t* containerBox,
                                        NW_Bool inCache);

void NW_Wml1x_InputDefaultStyle (NW_LMgr_Box_t *inputBox);

void NW_Wml1x_SelectDefaultStyle (NW_LMgr_Box_t *selectBox);

void NW_Wml1x_FieldsetDefaultStyle(NW_LMgr_Box_t* box);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif
