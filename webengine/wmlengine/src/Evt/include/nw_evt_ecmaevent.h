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


#ifndef NW_ECMA_EVNTTYPE_H
#define NW_ECMA_EVNTTYPE_H



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum NW_ECMA_Evt_Type_e {
  NW_Ecma_Evt_None,
  NW_Ecma_Evt_OnFocus,
  NW_Ecma_Evt_OnChange, 
  NW_Ecma_Evt_OnClick, 
  NW_Ecma_Evt_OnLoad, 
  NW_Ecma_Evt_OnSubmit, 
  NW_Ecma_Evt_OnReset, 
  NW_Ecma_Evt_OnBlur,
  NW_Ecma_Evt_OnUnload,
  NW_Ecma_Evt_Timer,
  NW_Ecma_Evt_JavaScriptFunction,
} NW_ECMA_Evt_Type_t;
  


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_ECMA_EVNTTYPE_H */
