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


#ifndef NW_IMAGECH_EPOC32CONTENTHANDLERI_H
#define NW_IMAGECH_EPOC32CONTENTHANDLERI_H

#include "nw_hed_contenthandleri.h"
#include "nw_image_abstractimage.h"
#include "nw_image_iimageobserveri.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   protected static data
 * ------------------------------------------------------------------------- */
NW_LMGR_EXPORT const NW_Object_Class_t* const _NW_ImageCH_Epoc32ContentHandler_InterfaceList[];
NW_LMGR_EXPORT const NW_Image_IImageObserver_Class_t _NW_ImageCH_Epoc32_ImageObserver_Class;

/* ------------------------------------------------------------------------- *
   method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_Construct( NW_Object_Dynamic_t* dynamicObject,
                                           va_list* argp );

extern
void
_NW_ImageCH_Epoc32ContentHandler_Destruct( NW_Object_Dynamic_t* dynamicObject );

extern
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_Initialize( NW_HED_DocumentNode_t* documentNode,
                                             TBrowserStatusCode aInitStatus );

extern
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_GetBoxTree( NW_HED_DocumentNode_t* documentNode,
                                       NW_LMgr_Box_t** boxTree );

extern
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_PartialNextChunk( NW_HED_ContentHandler_t* contentHandler,
                                         NW_Int32 chunkIndex,
                                         NW_Url_Resp_t* response,
                                         void* context);
extern
const NW_Text_t*
_NW_ImageCH_Epoc32ContentHandler_GetTitle( NW_HED_ContentHandler_t* contentHandler );

extern
TBrowserStatusCode
_NW_ImageCH_Epoc32ContentHandler_CreateBoxTree( NW_HED_ContentHandler_t* contentHandler,
                                                NW_LMgr_Box_t** boxTree);

extern
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageOpened( NW_Image_IImageObserver_t* imageObserver,
                                                             NW_Image_AbstractImage_t* abstractImage,
                                                             NW_Bool failed,
                                                             NW_Int16 suppressDupReformats );

extern
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_SizeChanged( NW_Image_IImageObserver_t* imageObserver,
                                                             NW_Image_AbstractImage_t* abstractImage );

extern
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageOpeningStarted( NW_Image_IImageObserver_t* imageObserver,
                                                                     NW_Image_AbstractImage_t* abstractImage );

extern
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageDecoded( NW_Image_IImageObserver_t* imageObserver,
                                                              NW_Image_AbstractImage_t* abstractImage,
                                                              NW_Bool failed);

extern
void
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_ImageDestroyed( NW_Image_IImageObserver_t* imageObserver,
                                                                NW_Image_AbstractImage_t* abstractImage );

extern
NW_Bool
_NW_ImageCH_Epoc32ContentHandler_IImageObserver_IsVisible( NW_Image_IImageObserver_t* aImageObserver);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGECH_EPOC32CONTENTHANDLERI_H */
