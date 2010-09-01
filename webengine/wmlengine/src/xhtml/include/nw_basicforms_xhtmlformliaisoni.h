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


#ifndef NW_XHTML_XHTMLFORMLIAISONI_H
#define NW_XHTML_XHTMLFORMLIAISONI_H

#include "nw_evt_ecmaevent.h"

#include "nw_fbox_formliaisoni.h"
#include "nw_object_dynamici.h"
#include "nw_basicforms_xhtmlformliaison.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_Construct (NW_Object_Dynamic_t* dynamicObject,
                                 va_list* argp);

extern
void
_NW_XHTML_FormLiaison_Destruct (NW_Object_Dynamic_t* dynamicObject);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetWBXMLVersion(NW_FBox_FormLiaison_t*  thisObj,
                                      NW_Uint32*               wmlVersion);
extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_AddControl(NW_FBox_FormLiaison_t* thisObj,
                                 void* node);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_SetInitialStringValue(NW_FBox_FormLiaison_t* thisObj,
                                            void* node,
																						NW_Text_Length_t	maxChars);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_SetStringValue(NW_FBox_FormLiaison_t* thisObj,
                                     void* node,
                                     NW_Text_t* value);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_ValidateStringValue(NW_FBox_FormLiaison_t* thisObj,
                                          void* node,
                                          const NW_Text_t* value);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetStringValue(NW_FBox_FormLiaison_t* thisObj,
                                     void* node,
                                     NW_Text_t** valueOut,
                                     NW_Bool*  initialValueUsed);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetStringName(NW_FBox_FormLiaison_t* thisObj,
                                    void* node,
                                    NW_Ucs2** nameOut);

TBrowserStatusCode
_NW_XHTML_FormLiaison_GetStringTitle(NW_FBox_FormLiaison_t* thisObj,
                                    void* node,
                                    NW_Ucs2** titleOut);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_SetBoolValue(NW_FBox_FormLiaison_t* thisObj,
                                   void* node,
                                   NW_Bool *value);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_ResetBoolValue( NW_FBox_FormLiaison_t* thisObj,
                                      void* node,
                                      NW_Bool *value );

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_ToggleBoolValue(NW_FBox_FormLiaison_t* thisObj,
                                      void* node);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetBoolValue(NW_FBox_FormLiaison_t* thisObj,
                                   void* node,
                                   NW_Bool* valueOut);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_Reset(NW_FBox_FormLiaison_t* thisObj,
                            void* resetButtonNode);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_Submit(NW_FBox_FormLiaison_t* thisObj,
                             void* submitButtonNode);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_Focus( NW_FBox_FormLiaison_t* thisObj,
                             void* node );

extern
NW_Bool
_NW_XHTML_FormLiaison_IsOptionMultiple(NW_FBox_FormLiaison_t* thisObj,
                                       void* controlId);

extern
NW_Bool
_NW_XHTML_FormLiaison_IsOptionSelected(NW_FBox_FormLiaison_t* thisObj,
                                       void* controlId);

extern
NW_Bool
_NW_XHTML_FormLiaison_OptionHasOnPick(NW_FBox_FormLiaison_t* thisObj,
                                      void* controlId);

extern
NW_Bool
_NW_XHTML_FormLiaison_IsLocalNavOnPick(NW_FBox_FormLiaison_t* thisObj,
                                      void* elemID);
extern
NW_Bool
_NW_XHTML_FormLiaison_GetInitialValue (NW_FBox_FormLiaison_t* formLiaison,
																			 void* controlId);

extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_GetDocRoot(NW_FBox_FormLiaison_t  *thisObj,
                                    NW_HED_DocumentRoot_t **docRoot);
extern
TBrowserStatusCode
_NW_XHTML_FormLiaison_DelegateEcmaEvent(NW_FBox_FormLiaison_t *thisObj,
                                        void* eventNode,
                                        NW_ECMA_Evt_Type_t ecmaEvent);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_XHTMLFORMLIAISONI_H */
