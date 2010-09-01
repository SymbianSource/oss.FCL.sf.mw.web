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


#ifndef NW_XHTML_XHTMLFORMLIAISON_H
#define NW_XHTML_XHTMLFORMLIAISON_H

#include "BrsrStatusCodes.h"
#include "nw_fbox_formliaison.h"
#include "nw_xhtml_xhtmlcontenthandler.h"
#include "NW_FBox_Validator.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_FormLiaison_Class_s NW_XHTML_FormLiaison_Class_t;
typedef struct NW_XHTML_FormLiaison_s NW_XHTML_FormLiaison_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_FormLiaison_ClassPart_s {
  void** unused;
} NW_XHTML_FormLiaison_ClassPart_t;

struct NW_XHTML_FormLiaison_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_FormLiaison_ClassPart_t NW_FBox_FormLiaison;
  NW_XHTML_FormLiaison_ClassPart_t NW_XHTML_FormLiaison;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_FormLiaison_s {
  /* TODO: remove value list from class hierarchy */
  NW_FBox_FormLiaison_t super;

  /* member variables */
  NW_ADT_Map_t* validatorMap;
  NW_DOM_ElementNode_t* formNode;
  NW_ADT_Vector_Metric_t numControls; /* number of controls in this form */
  NW_ADT_Vector_Metric_t firstControl; /* index of the first control of form */
  NW_XHTML_ContentHandler_t* contentHandler;
  NW_XHTML_ControlSet_t* controlSet; /* ptr to all controls in the document */
  NW_Text_t*             formName;
  NW_Text_t*             formAction; 
  NW_Text_t*             formMethod;
  NW_Text_t*             formEnctype;
  NW_Text_t*             formId;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_FormLiaison_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_XHTML_FormLiaison))

#define NW_XHTML_FormLiaisonOf(object) \
  (NW_Object_Cast (object, NW_XHTML_FormLiaison))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_FormLiaison_Class_t NW_XHTML_FormLiaison_Class;

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
NW_XHTML_FormLiaison_RegisterValidator (NW_XHTML_FormLiaison_t* thisObj,
                                         void* node,
                                        NW_FBox_Validator_t* validator);

extern
TBrowserStatusCode
NW_XHTML_FormLiaison_AddBox (NW_FBox_FormLiaison_t* formLiaison,
                             NW_LMgr_Box_t* box, NW_DOM_ElementNode_t* elementNode);


extern
TBrowserStatusCode
NW_XHTML_FormLiaison_GetAction(NW_FBox_FormLiaison_t* formLiaison,
                              NW_Ucs2** formAction);

extern
TBrowserStatusCode
NW_XHTML_FormLiaison_SetAction (NW_XHTML_FormLiaison_t* formLiaison,
                                const NW_Ucs2* formAction);

extern
TBrowserStatusCode
NW_XHTML_FormLiaison_SetMethod (NW_XHTML_FormLiaison_t* formLiaison,
                                const NW_Ucs2* formMethod);

extern
TBrowserStatusCode
NW_XHTML_FormLiaison_SetFormName (NW_XHTML_FormLiaison_t* formLiaison,
                                const NW_Ucs2* formName);

extern
TBrowserStatusCode
NW_XHTML_FormLiaison_SetEnctype (NW_XHTML_FormLiaison_t* formLiaison,
                                const NW_Ucs2* formEnctype);
extern
NW_Object_t*
NW_XHTML_FormLiaison_GetInitialValue (NW_XHTML_FormLiaison_t* thisObj,
																			NW_DOM_ElementNode_t* node);

extern
TBrowserStatusCode
NW_XHTML_FormLiaison_Submit(NW_FBox_FormLiaison_t* thisObj,
																			void* node);
extern
TBrowserStatusCode
NW_XHTML_FormLiaison_Reset(NW_FBox_FormLiaison_t* thisObj,
																			void* node);




/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
NW_XHTML_FormLiaison_t*
NW_XHTML_FormLiaison_New (NW_DOM_ElementNode_t* formNode,
                          NW_XHTML_ContentHandler_t* contentHandler,
                          NW_XHTML_ControlSet_t* controlSet);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_XHTMLFORMLIAISON_H */
