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


#ifndef _NW_Markup_ImageViewerI_h_
#define _NW_Markup_ImageViewerI_h_

#include "nw_object_aggregatei.h"
#include "nw_hed_iimagevieweri.h"
#include "nw_markup_imageviewer.h"
#include "BrsrStatusCodes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* ------------------------------------------------------------------------- *
   protected global data
 * ------------------------------------------------------------------------- */
NW_MARKUP_EXPORT const NW_Object_Class_t* const _NW_Markup_ImageViewer_SecondaryList[];
NW_MARKUP_EXPORT const NW_HED_IImageViewer_Class_t NW_Markup_ImageViewer_IImageViewer_Class;

/* ------------------------------------------------------------------------- *
   virtual method implementation prototypes
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **

    @method:      NW_Markup_ImageViewer_Collect

    @synopsis:    Returns all text strings the may contain phone numbers.

    @scope:       public
    @access:      implementation

    @parameters:
       [in, out] NW_ADT_DynamicVector_t* dynamicVector
                  The dynamic vector into which all the phone numbers will be
                  placed. This is a vector of NW_Text_t*.

    @description: The function traverses the box tree, looks inside text and 
                  input boxes and makes text objects. The function attaches 
                  all created text objects to the dynamic vector. It is the caller's
                  responsibility to free the text objects.

                  Text from Password type input boxes is not passed to the caller.

                  Note that although the name of the function is ImageViewer
                  it in fact collects all text strings. The function can be
                  improved by introducing an algorithm of extructing numbers or
                  only strings that have numbers.

    @returns:     TBrowserStatusCode
       [KBrsrSuccess]
                  Successful completion.
     
       [KBrsrOutOfMemory]
                  An unrecoverable out of memory condition was reached.

       [KBrsrUnexpectedError]
                  An unexpected, unrecoverable and/or unhandled error was
                  encountered. Although the method will attempt to return
                  without disrupting the state of the system, this can not be
                  guaranteed.
 ** ----------------------------------------------------------------------- **/
NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_ImageViewer_IImageViewer_GetImage (NW_HED_IImageViewer_t* imageViewer,
                                              NW_ADT_DynamicVector_t* dynamicVector);


NW_MARKUP_EXPORT
TBrowserStatusCode
_NW_Markup_ImageViewer_IImageViewer_ShowImages (NW_HED_IImageViewer_t* imageViewer);

/* ------------------------------------------------------------------------- *
   protected methods
 * ------------------------------------------------------------------------- */
#define NW_Markup_ImageViewer_LoadImage(_imageViewer) \
  (NW_Object_Invoke (_imageViewer, NW_Markup_ImageViewer, loadImage) ( \
     NW_Markup_ImageViewerOf (_imageViewer)))

#define NW_Markup_ImageViewer_IsNoSave(_imageViewer, _imageBox) \
  (NW_Object_Invoke (_imageViewer, NW_Markup_ImageViewer, isNoSave) ( \
     NW_Markup_ImageViewerOf (_imageViewer), NW_LMgr_BoxOf(_imageBox)))

#define NW_Markup_ImageViewer_GetAltText(_imageViewer, _imageBox) \
  (NW_Object_Invoke (_imageViewer, NW_Markup_ImageViewer, getAltText) ( \
     NW_Markup_ImageViewerOf (_imageViewer), NW_LMgr_BoxOf(_imageBox)))

/* #define NW_Markup_ImageViewer_BoxIsValid(_numberCollector, _box) \
  (NW_Object_Invoke (_numberCollector, NW_Markup_ImageViewer, boxIsValid) ( \
     NW_Markup_ImageViewerOf (_numberCollector), NW_LMgr_BoxOf (_box)))*/

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif /* _NW_Markup_ImageViewerI_h_ */
