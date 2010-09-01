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


#ifndef NW_WML1X_WML1XFORMLIAISONI_H
#define NW_WML1X_WML1XFORMLIAISONI_H

#include "nw_evt_ecmaevent.h"

#include "nw_fbox_formliaisoni.h"
#include "nw_object_dynamici.h"
#include "nw_wml1x_wml1xformliaison.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_Construct (NW_Object_Dynamic_t  *dynamicObject,
                                 va_list              *argp);

extern
void
_NW_Wml1x_FormLiaison_Destruct(NW_Object_Dynamic_t* dynamicObject);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetWBXMLVersion(NW_FBox_FormLiaison_t   *thisObj,
                                      NW_Uint32                *version);
extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_SetInitialStringValue(NW_FBox_FormLiaison_t   *thisObj,
                                            void                    *elemId,
																						NW_Text_Length_t				maxChars);
extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_SetStringValue(NW_FBox_FormLiaison_t  *thisObj,
                                    void                    *elemId,
                                    NW_Text_t               *value);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_ValidateStringValue (NW_FBox_FormLiaison_t* thisObj,
                                           void* elemId,
                                           const NW_Text_t* value);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetStringValue(NW_FBox_FormLiaison_t  *thisObj,
                                    void                    *elemId,
                                    NW_Text_t              **valueOut,
                                    NW_Bool                 *initialValueUsed);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetStringName(NW_FBox_FormLiaison_t  *thisObj,
                                   void                    *elemId,
                                   NW_Ucs2              **nameOut);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetStringTitle(NW_FBox_FormLiaison_t  *thisObj,
                                   void                    *elemId,
                                   NW_Ucs2              **titleOut);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_SetBoolValue(NW_FBox_FormLiaison_t  *thisObj,
                                  void                    *elemId,
                                  NW_Bool                 *value);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_ToggleBoolValue(NW_FBox_FormLiaison_t *thisObj,
                                      void                  *elemId);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetBoolValue(NW_FBox_FormLiaison_t  *thisObj,
                                  void                    *elemId,
                                  NW_Bool                 *valueOut);

extern
NW_Bool
_NW_Wml1x_FormLiaison_OptionHasOnPick(NW_FBox_FormLiaison_t  *thisObj,
                                      void                    *elemId);

extern
NW_Bool
_NW_Wml1x_FormLiaison_IsLocalNavOnPick(NW_FBox_FormLiaison_t  *thisObj,
                                      void                    *elemId);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_GetDocRoot(NW_FBox_FormLiaison_t  *thisObj,
                                    NW_HED_DocumentRoot_t **docRoot);

extern
TBrowserStatusCode
_NW_Wml1x_FormLiaison_DelegateEcmaEvent(NW_FBox_FormLiaison_t *thisObj,
                                        void* eventNode,
                                        NW_ECMA_Evt_Type_t ecmaEvent);



#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1X_WML1XFORMLIAISONI_H */
