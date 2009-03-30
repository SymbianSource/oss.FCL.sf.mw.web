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


#ifndef NW_IMAGECH_WBMPIMAGEI_H
#define NW_IMAGECH_WBMPIMAGEI_H

#include "nw_image_abstractimagei.h"
#include "nw_imagech_wbmpimage.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */
extern
TBrowserStatusCode
_NW_ImageCH_WBMP_Construct (NW_Object_Dynamic_t* dynamicObject,
                          va_list *argp);

extern
void
_NW_ImageCH_WBMP_Destruct (NW_Object_Dynamic_t* dynamicObject);

extern
TBrowserStatusCode
_NW_ImageCH_WBMP_GetSize (const NW_Image_AbstractImage_t* image,
                        NW_GDI_Dimension3D_t* size);

extern
TBrowserStatusCode
_NW_ImageCH_WBMP_Draw ( NW_Image_AbstractImage_t* image,
                       CGDIDeviceContext* deviceContext,
                       const NW_GDI_Point2D_t* location);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* NW_IMAGECH_WBMPIMAGEI_H */
