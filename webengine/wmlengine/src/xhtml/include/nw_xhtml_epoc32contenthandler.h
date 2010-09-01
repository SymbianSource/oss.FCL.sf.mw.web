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


#ifndef NW_XHTML_EPOC32CONTENTHANDLER_H
#define NW_XHTML_EPOC32CONTENTHANDLER_H

#include "nw_xhtml_xhtmlcontenthandler.h"
#include "nw_markup_imageviewer.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_Epoc32ContentHandler_Class_s NW_XHTML_Epoc32ContentHandler_Class_t;
typedef struct NW_XHTML_Epoc32ContentHandler_s NW_XHTML_Epoc32ContentHandler_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_XHTML_Epoc32ContentHandler_ClassPart_s {
  void** unused;
} NW_XHTML_Epoc32ContentHandler_ClassPart_t;

struct NW_XHTML_Epoc32ContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
  NW_HED_CompositeContentHandler_ClassPart_t NW_HED_CompositeContentHandler;
  NW_XHTML_ContentHandler_ClassPart_t NW_XHTML_ContentHandler;
  NW_XHTML_Epoc32ContentHandler_ClassPart_t NW_XHTML_Epoc32ContentHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_XHTML_Epoc32ContentHandler_s {
  NW_XHTML_ContentHandler_t super;

  /* member variables */
  /* aggregation(s) */
  NW_Markup_ImageViewer_t NW_Markup_ImageViewer;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_XHTML_Epoc32ContentHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart((_object), NW_XHTML_Epoc32ContentHandler))

#define NW_XHTML_Epoc32ContentHandlerOf(_object) \
  (NW_Object_Cast (_object, NW_XHTML_Epoc32ContentHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_XHTML_Epoc32ContentHandler_Class_t NW_XHTML_Epoc32ContentHandler_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode
NW_XHTML_Epoc32ContentHandler_SaveToFile(NW_XHTML_Epoc32ContentHandler_t *thisObj,
                                         NW_Ucs2* file);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_XHTML_CONTENTHANDLER_H */
