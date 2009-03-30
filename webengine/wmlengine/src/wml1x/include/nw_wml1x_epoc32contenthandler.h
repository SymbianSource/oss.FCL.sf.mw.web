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


#ifndef NW_WML1X_EPOC32CONTENTHANDLER_H
#define NW_WML1X_EPOC32CONTENTHANDLER_H

#include "nw_wml1x_wml1xcontenthandler.h"
#include "nw_markup_imageviewer.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_Wml1x_Epoc32ContentHandler_Class_s NW_Wml1x_Epoc32ContentHandler_Class_t;
typedef struct NW_Wml1x_Epoc32ContentHandler_s NW_Wml1x_Epoc32ContentHandler_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_Wml1x_Epoc32ContentHandler_ClassPart_s {
  void** unused;
} NW_Wml1x_Epoc32ContentHandler_ClassPart_t;

struct NW_Wml1x_Epoc32ContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
  NW_HED_CompositeContentHandler_ClassPart_t NW_HED_CompositeContentHandler;
  NW_Wml1x_ContentHandler_ClassPart_t NW_Wml1x_ContentHandler;
  NW_Wml1x_Epoc32ContentHandler_ClassPart_t NW_Wml1x_Epoc32ContentHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_Wml1x_Epoc32ContentHandler_s {
  NW_Wml1x_ContentHandler_t super;

  /* aggregation(s) */
  NW_Markup_ImageViewer_t NW_Markup_ImageViewer;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_Wml1x_Epoc32ContentHandler_ClassPartOf(_object) \
  (NW_Object_GetClassPart (_object, NW_Wml1x_Epoc32ContentHandler))

#define NW_Wml1x_Epoc32ContentHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_Wml1x_Epoc32ContentHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_Wml1x_Epoc32ContentHandler_Class_t NW_Wml1x_Epoc32ContentHandler_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode
NW_Wml1x_Epoc32ContentHandler_SaveToFile(NW_Wml1x_Epoc32ContentHandler_t *thisObj,
                                         NW_Ucs2* file);

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_WML1X_EPOC32CONTENTHANDLER_H */
