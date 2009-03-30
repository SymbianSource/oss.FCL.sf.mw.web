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


#ifndef NW_IMAGECH_EPOC32CONTENTHANDLER_H
#define NW_IMAGECH_EPOC32CONTENTHANDLER_H

#include "nw_hed_contenthandler.h"
#include "nw_image_abstractimage.h"
#include "nw_image_iimageobserver.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  


/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_ImageCH_Epoc32ContentHandler_Class_s NW_ImageCH_Epoc32ContentHandler_Class_t;
typedef struct NW_ImageCH_Epoc32ContentHandler_s NW_ImageCH_Epoc32ContentHandler_t;

/* ------------------------------------------------------------------------- *
   method prototypes
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_ImageCH_Epoc32ContentHandler_ClassPart_s {
  void** unused;
} NW_ImageCH_Epoc32ContentHandler_ClassPart_t;

struct NW_ImageCH_Epoc32ContentHandler_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Base_ClassPart_t NW_Object_Base;
  NW_Object_Dynamic_ClassPart_t NW_Object_Dynamic;
  NW_HED_DocumentNode_ClassPart_t NW_HED_DocumentNode;
  NW_HED_ContentHandler_ClassPart_t NW_HED_ContentHandler;
  NW_ImageCH_Epoc32ContentHandler_ClassPart_t NW_ImageCH_Epoc32ContentHandler;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_ImageCH_Epoc32ContentHandler_s {
  NW_HED_ContentHandler_t super;

  /* member variables */
  NW_Image_AbstractImage_t* image;
  NW_Text_t* title;
  NW_HED_UrlRequest_LoadMode_t loadMode;
  NW_Ucs2* requestUrl;
  NW_Ucs2* altText;

  void* context;
  NW_Image_IImageObserver_t NW_Image_IImageObserver;
  NW_Uint8 noStore;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_ImageCH_Epoc32ContentHandler_GetClassPart(_object) \
  (NW_Object_GetClassPart ((_object), NW_ImageCH_Epoc32ContentHandler))

#define NW_ImageCH_Epoc32ContentHandlerOf(_object) \
  (NW_Object_Cast ((_object), NW_ImageCH_Epoc32ContentHandler))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
extern const NW_ImageCH_Epoc32ContentHandler_Class_t NW_ImageCH_Epoc32ContentHandler_Class;

extern
NW_HED_UrlRequest_LoadMode_t
NW_ImageCH_Epoc32ContentHandler_GetLoadMode( NW_ImageCH_Epoc32ContentHandler_t* thisObj );


/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */
extern
NW_HED_ContentHandler_t*
NW_ImageCH_Epoc32ContentHandler_NewBrokenImage( NW_HED_DocumentNode_t* parent, 
                                                NW_HED_UrlRequest_t* urlRequest,
                                                TBrowserStatusCode error );

extern
TBrowserStatusCode
NW_ImageCH_Epoc32ContentHandler_GetVirtualHandler( NW_HED_DocumentNode_t* parent, 
                                                   NW_HED_UrlRequest_t* urlRequest,
                                                   NW_ImageCH_Epoc32ContentHandler_t* imageCH,
                                                   NW_ImageCH_Epoc32ContentHandler_t** newVirtualImageCH );

extern
void
NW_ImageCH_Epoc32ContentHandler_ForceImageOpen( NW_ImageCH_Epoc32ContentHandler_t* imageCH );

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGECH_EPOC32CONTENTHANDLER_H */
