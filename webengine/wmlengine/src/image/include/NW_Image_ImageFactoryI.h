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


#ifndef NW_Image_ImageFactoryI_h
#define NW_Image_ImageFactoryI_h


#include "nw_object_corei.h"
#include "NW_Image_ImageFactory.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ------------------------------------------------------------------------- *
   protected data types
 * ------------------------------------------------------------------------- */
struct NW_Image_ImageFactory_MimeTableEntry_s {
  NW_Http_ContentTypeString_t contentTypeString;
  NW_Image_AbstractImage_Class_t* objClass;
};

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Image_ImageFactory_GetMimeTable(_imageFactory) \
  (NW_Image_ImageFactory_GetClassPart (_imageFactory).mimeTable)

#define NW_Image_ImageFactory_GetNumEntries(_imageFactory) \
  (NW_Image_ImageFactory_GetClassPart (_imageFactory).numEntries)

NW_IMAGE_EXPORT
NW_Image_AbstractImage_t*
_NW_Image_ImageFactory_CreateImage (const NW_Image_ImageFactory_t* imageFactory,
                                    NW_Http_ContentTypeString_t contentTypeString,
                                    NW_Buffer_t* imageData);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_Image_ImageFactoryI_h */
