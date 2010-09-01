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


#ifndef _NW_HED_IImageViewer_h_
#define _NW_HED_IImageViewer_h_

#include "nw_object_interface.h"
#include "nw_adt_dynamicvector.h"
#include "NW_HED_EXPORT.h"
#include "BrsrStatusCodes.h"
#include "ImageUtils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @class:       NW_HED_IImageViewer

    @scope:       public

    @description: An interface typically implemented by content handlers.
                  Used by the NW_HED_DocumentRoot to retrieve the list of
                  images embedded in the document tree.

                  The NW_HED_DocumentRoot will traverse the document tree
                  querying each content handler in the tree for an
                  implementation of the this interface. If found, the document
                  root will invoke this method to retrieve the phone numbers
                  associated with that content handler.

                  How a particular content handler decides to implement this
                  interface is not specified.
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   forward declarations
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_IImageViewer_Class_s NW_HED_IImageViewer_Class_t;
typedef struct NW_HED_IImageViewer_s NW_HED_IImageViewer_t;

/* ------------------------------------------------------------------------- *
   virtual method type definitions
 * ------------------------------------------------------------------------- */
typedef
TBrowserStatusCode
(*NW_HED_IImageViewer_GetImage_t) (NW_HED_IImageViewer_t* imageViewer,
                                    NW_ADT_DynamicVector_t* dynamicVector);

typedef
TBrowserStatusCode
(*NW_HED_IImageViewer_ShowImages_t) (NW_HED_IImageViewer_t* imageViewer);

/* ------------------------------------------------------------------------- *
   data type
 * ------------------------------------------------------------------------- */

typedef struct NW_HED_ViewImageList_Entry_s
{
  NW_Byte* rawData;
  NW_Uint32 rawDataLen;
  NW_Ucs2* url;
  NW_Ucs2* altText;
  TImageType imageType;
  NW_Ucs2* contentType;
}NW_HED_ViewImageList_Entry_t;

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */
typedef struct NW_HED_IImageViewer_ClassPart_s {
  NW_HED_IImageViewer_GetImage_t getImage;
  NW_HED_IImageViewer_ShowImages_t showImages;
} NW_HED_IImageViewer_ClassPart_t;

struct NW_HED_IImageViewer_Class_s {
  NW_Object_Core_ClassPart_t NW_Object_Core;
  NW_Object_Interface_ClassPart_t NW_Object_Interface;
  NW_HED_IImageViewer_ClassPart_t NW_HED_IImageViewer;
};

/* ------------------------------------------------------------------------- *
   object definiton
 * ------------------------------------------------------------------------- */
struct NW_HED_IImageViewer_s {
  NW_Object_Interface_t super;
};

/* ------------------------------------------------------------------------- *
   convenience macros
 * ------------------------------------------------------------------------- */
#define NW_HED_IImageViewer_GetClassPart(_object) \
  (NW_Object_GetClassPart(_object, NW_HED_IImageViewer))

#define NW_HED_IImageViewerOf(_object) \
  (NW_Object_Cast (_object, NW_HED_IImageViewer))

/* ------------------------------------------------------------------------- *
   global static data
 * ------------------------------------------------------------------------- */
NW_HED_EXPORT const NW_HED_IImageViewer_Class_t NW_HED_IImageViewer_Class;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @method:      NW_HED_IImageViewer_GetImage

    @synopsis:    Returns the imageData

    @scope:       public
    @access:      virtual

    @parameters:
       [in, out]  NW_ADT_DynamicVector_t* dynamicVector
                  The dynamic vector into which all the phone numbers will be
                  placed. This is a vector of Image Data
      
    @description: Retrieves, by any mechanism necessary, all image data
                  associated with the interface implementer. The image entries
                  are added to the 'dynamicVector'.
                 
    @returns:     TBrowserStatusCode
                  On error, it is not the responsibility of the implementer
                  to clean up the out parameters. The only guarantee that the
                  implementer must fullfill is that the parameters are left
                  in a consistent state. This simply means that the entries of
                  the 'dynamicVector' must still be valid NW_Text_t*'s.

       [KBrsrSuccess]
                  Successful completion; 'dynamicVector' will have had all
                  phone numbers appendeded.

       [KBrsrOutOfMemory]
                  Not enough memory available to complete operation.

       [KBrsrUnexpectedError]
                  An unexpected error occurred.
 ** ----------------------------------------------------------------------- **/
#define NW_HED_IImageViewer_GetImage(_imageViewer, _dynamicVector) \
  (NW_Object_Invoke (_imageViewer, NW_HED_IImageViewer, getImage) ( \
     NW_HED_IImageViewerOf (_imageViewer), (_dynamicVector)))


#define NW_HED_IImageViewer_ShowImages(_imageViewer) \
  (NW_Object_Invoke (_imageViewer, NW_HED_IImageViewer, showImages) ( \
     NW_HED_IImageViewerOf (_imageViewer)))

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_HED_IImageViewer_h_ */
