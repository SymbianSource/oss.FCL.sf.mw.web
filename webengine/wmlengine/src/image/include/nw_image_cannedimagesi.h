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


#ifndef NW_IMAGE_CANNEDIMAGESI_H
#define NW_IMAGE_CANNEDIMAGESI_H

#include "nw_object_dynamici.h"
#include "nw_image_cannedimages.h"
#include "nw_image_epoc32simpleimage.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

extern
TBrowserStatusCode
_NW_Image_CannedImages_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list *argp);

extern
void
_NW_Image_CannedImages_Destruct (NW_Object_Dynamic_t* dynamicObject);


/* ------------------------------------------------------------------------- *
   protected types
 * ------------------------------------------------------------------------- */
struct NW_Image_CannedImages_Entry_s {
  NW_Int8 imageId;
  const NW_Image_Epoc32Simple_t* image;
};

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGE_CANNEDIMAGESI_H */
