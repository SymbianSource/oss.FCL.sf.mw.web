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


#ifndef NW_WML1X_WML1XFORMLIAISON_H
#define NW_WML1X_WML1XFORMLIAISON_H

#include "nw_fbox_formliaison.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Wml1x_FormLiaison_Class_s NW_Wml1x_FormLiaison_Class_t;
typedef struct NW_Wml1x_FormLiaison_s NW_Wml1x_FormLiaison_t; 

typedef struct NW_Wml1x_ContentHandler_s NW_Wml1x_ContentHandler_t; 

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Wml1x_FormLiaison_ClassPart_s {
  void** unused;
} NW_Wml1x_FormLiaison_ClassPart_t;

struct NW_Wml1x_FormLiaison_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_FBox_FormLiaison_ClassPart_t NW_FBox_FormLiaison;
  NW_Wml1x_FormLiaison_ClassPart_t NW_Wml1x_FormLiaison;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Wml1x_FormLiaison_s {
  NW_FBox_FormLiaison_t super;

  /* member variables */
  NW_Wml1x_ContentHandler_t *contentHandler;
  NW_Text_t* lastStringValue;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Wml1x_FormLiaison_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_Wml1x_FormLiaison))

#define NW_Wml1x_FormLiaisonOf(object) \
  (NW_Object_Cast (object, NW_Wml1x_FormLiaison))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Wml1x_FormLiaison_Class_t NW_Wml1x_FormLiaison_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

extern
NW_Wml1x_FormLiaison_t*
NW_Wml1x_FormLiaison_New (NW_Wml1x_ContentHandler_t *contentHandler);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1X_WML1XFORMLIAISON_H */
