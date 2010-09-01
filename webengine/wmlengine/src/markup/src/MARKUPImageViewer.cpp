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


#include "nw_markup_imagevieweri.h"
#include "nw_hed_compositecontenthandler.h"
#include "nw_imagech_epoc32contenthandler.h"
#include "nw_image_epoc32simpleimage.h"
#include "nw_image_virtualimage.h"
#include "BrsrStatusCodes.h"
#include "nwx_string.h"

 /* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/** ----------------------------------------------------------------------- **
    @method:      NW_Markup_ImageViewer_GetText

    @synopsis:    Returns all phone numbers associated with the implementer.

    @scope:       private
    @access:      final

    @parameters:
       [in, out] NW_LMgr_Box_t* box
                  Pointer to a box object from the BoxTree.

       [out] NW_Text_t** text
                  Pointer to store the text object pointer.

       [out] NW_Bool* isCopy
                  Pointer to store a Bool flag indicating that the returned
                  text object has to be freed if not needed.

    @description: The function returns a pointer to a text object from input
                  and text boxes.

                  checks if the current box is a text box or input box. A
                  text object is extructed and its pointer is returned.
                  Exception: Input boxes of the Password type are ignored
                  for security reasons.

    @returns:     NW_Bool
       [NW_TRUE]
                  When a text object is returned.

       [NW_FALSE]
                  When text object is not found. NULL is returned in place
                  of the pointer to a text object
 ** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Markup_ImageViewer_Class_t NW_Markup_ImageViewer_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Object_Aggregate_Class,
    /* querySecondary          */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary     */
    /* offset                  */ 0
  },
  { /* NW_Object_Aggregate     */
    /* secondaryList           */ _NW_Markup_ImageViewer_SecondaryList,
    /* construct               */ NULL,
    /* destruct                */ NULL
  },
  { /* NW_Markup_ImageViewer   */
    /* loadImage               */ NULL,
    /*IsNoSave                 */ NULL,
    /* getAltText              */ NULL
  }
};

/* ------------------------------------------------------------------------- */
const
NW_Object_Class_t* const _NW_Markup_ImageViewer_SecondaryList[] = {
  &NW_Markup_ImageViewer_IImageViewer_Class,
  NULL
};

/* ------------------------------------------------------------------------- */
const NW_HED_IImageViewer_Class_t NW_Markup_ImageViewer_IImageViewer_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_HED_IImageViewer_Class,
    /* querySecondary          */ _NW_Object_Core_QuerySecondary
  },
  { /* NW_Object_Secondary     */
    /* offset                  */ offsetof (NW_Markup_ImageViewer_t, NW_HED_IImageViewer)
  },
  { /* NW_HED_IImageViewer */
    /* getImage                */ _NW_Markup_ImageViewer_IImageViewer_GetImage,
    /* showImages              */ _NW_Markup_ImageViewer_IImageViewer_ShowImages
  }
};

/* ------------------------------------------------------------------------- *
   NW_Markup_ImageViewer methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Markup_ImageViewer_LoadImage(NW_Markup_ImageViewer_t* imageViewer)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
                                     &NW_Markup_ImageViewer_Class));

  NW_REQUIRED_PARAM(imageViewer);
  
  /* default implementation is that all boxes are valid */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
NW_Bool
_NW_Markup_ImageViewer_IsNoSave(NW_Markup_ImageViewer_t* imageViewer,
                                NW_ImageCH_Epoc32ContentHandler_t* imageCH)
{
  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
                                     &NW_Markup_ImageViewer_Class));

  NW_REQUIRED_PARAM(imageViewer);
  NW_REQUIRED_PARAM(imageCH);
  
  /* default implementation is that all images are save */
  return NW_FALSE;
}


/* ------------------------------------------------------------------------- *
   NW_HED_IImageViewer methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Markup_ImageViewer_IImageViewer_GetImage(NW_HED_IImageViewer_t* imageViewer,
                                             NW_ADT_DynamicVector_t* dynamicVector)
    {
    NW_Markup_ImageViewer_t* thisObj;
    NW_HED_CompositeNode_t* compositeNode;
    NW_LMgr_Box_t* imgBox = NULL;
    NW_ADT_Iterator_t* iter = NULL;
    NW_HED_ContentHandler_t* childNode = NULL;
    
    /* parameter assertion block */
    NW_ASSERT(NW_Object_IsInstanceOf(imageViewer, &NW_Markup_ImageViewer_IImageViewer_Class));
    NW_ASSERT(NW_Object_IsInstanceOf(dynamicVector, &NW_ADT_DynamicVector_Class));
    
    /* for convenience */
    thisObj = (NW_Markup_ImageViewer_t*)NW_Object_Interface_GetImplementer(imageViewer);
    NW_ASSERT(NW_Object_IsInstanceOf(thisObj, &NW_Markup_ImageViewer_Class));
    
    /* for convenience */
    NW_HED_CompositeContentHandler_t* compositeCH = (NW_HED_CompositeContentHandler_t*)NW_Object_Aggregate_GetAggregator(thisObj);
    NW_ASSERT(NW_Object_IsInstanceOf(compositeCH, &NW_HED_CompositeContentHandler_Class));
    
    compositeNode = (NW_HED_CompositeNode_t*)NW_Object_QuerySecondary(thisObj, &NW_HED_CompositeNode_Class);
    NW_ASSERT(NW_Object_IsInstanceOf(compositeNode, &NW_HED_CompositeNode_Class));
    
    NW_TRY(status)
        {
        /* Create a child iterator */
        status = NW_HED_CompositeNode_GetChildren(compositeNode, &iter);
        NW_THROW_ON_ERROR(status);
        
        /* Loop through the child content handlers; each contatins an image. */
        while (NW_ADT_Iterator_HasMoreElements(iter))
            {
            NW_ImageCH_Epoc32ContentHandler_t* imageCH = NULL;
            NW_Image_Epoc32Simple_t* image = NULL;
            NW_HED_ViewImageList_Entry_t* entry;
            
            status = NW_ADT_Iterator_GetNextElement(iter, &childNode);
            if (status != KBrsrSuccess)
                {
                break;
                }
            
            if (!NW_Object_IsClass(childNode, &NW_ImageCH_Epoc32ContentHandler_Class))
                {
                continue;
                }

            imageCH = NW_ImageCH_Epoc32ContentHandlerOf(childNode);
            
            if (imageCH->image == NULL)
                {
                continue;
                }

            if (NW_Object_IsInstanceOf(imageCH->image, &NW_Image_Virtual_Class))
                {
                continue;
                }
            
            imgBox = NW_HED_ContentHandlerOf(imageCH)->boxTree;
            if (imgBox == NULL)
                {
                continue;
                }

            if (NW_Markup_ImageViewer_IsNoSave(thisObj, imgBox))
                {
                continue;
                }
            
            // check if http cache-control header value "no-store" used, and continue if it's specified.
            if (imageCH->noStore)       // if cache-control:no-store directive used in HTTP headers, do not include the image.
                {
                continue;
                }

            image = NW_Image_Epoc32SimpleOf(imageCH->image);
            
            /* Save the text object in the dynamicVector */
            entry = (NW_HED_ViewImageList_Entry_t*)NW_ADT_DynamicVector_InsertAt(dynamicVector, NULL,
                                                                                 NW_ADT_Vector_AtEnd);
            NW_THROW_OOM_ON_NULL(entry, status);

            entry->rawData = (NW_Byte*)image->rawData;
            entry->rawDataLen = image->rawDataLength;
            entry->url = imageCH->requestUrl;
            entry->imageType = image->imageType;
            NW_Ucs2* contentType = NW_HED_ContentHandlerOf(imageCH)->response->contentTypeString16;
            if (!contentType)
                {
                contentType = NW_Str_CvtFromAscii((const char*)NW_HED_ContentHandlerOf(imageCH)->response->contentTypeString);
                // NW_Url_Resp_s takes care of cleaning up the contentTypeString16
                NW_HED_ContentHandlerOf(imageCH)->response->contentTypeString16 = contentType;
                }
			         entry->contentType = contentType;
            
            if (imageCH->altText == NULL)
                {
                imageCH->altText = NW_Markup_ImageViewer_GetAltText(thisObj, imgBox);
                }

            entry->altText = imageCH->altText;
            }
        }  
    NW_CATCH (status)
        {
        } 
    NW_FINALLY
        {
        NW_Object_Delete (iter);
        return status;
        }
    NW_END_TRY
    
    }

/* ------------------------------------------------------------------------- */
TBrowserStatusCode
_NW_Markup_ImageViewer_IImageViewer_ShowImages(NW_HED_IImageViewer_t* imageViewer)
{
  NW_Markup_ImageViewer_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (imageViewer,
                                     &NW_Markup_ImageViewer_IImageViewer_Class));
  /* for convenience */
  thisObj = (NW_Markup_ImageViewer_t*) NW_Object_Interface_GetImplementer (imageViewer);
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Markup_ImageViewer_Class));

  return NW_Markup_ImageViewer_LoadImage(thisObj);
}
