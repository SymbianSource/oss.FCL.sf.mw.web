/*
* Copyright (c) 2000-2004 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include <fbs.h>

#include "nw_image_cannedimagesi.h"
#include "nw_image_epoc32cannedimage.h"
#include "nw_text_ascii.h"

#include "nw_browser_browserimages.h"
#include "BrsrStatusCodes.h"

static TBrowserStatusCode
NW_Image_CannedImages_CreateDictionary(NW_Image_CannedImages_t* thisObj);

/* ------------------------------------------------------------------------- *
   static data
 * ------------------------------------------------------------------------- */

static const
TUint NW_Image_CannedImages_Dictionary[NW_Image_NumberOfCannedImages*2]=
  {
  qgn_prop_wml_square,              // NW_Image_Square
  qgn_prop_wml_disc,                // NW_Image_Disc
  qgn_prop_wml_circle,              // NW_Image_Circle
  qgn_prop_wml_image_not_shown,     // NW_Image_Missing
  qgn_prop_wml_image_not_shown,     // NW_Image_Broken
  qgn_prop_wml_selectarrow,         // NW_Image_SelectArrow
  qgn_prop_wml_object,              // NW_Image_Object
  qgn_prop_wml_selectfile,          // NW_Image_SelectFile
  qgn_prop_wml_image_map,           // NW_Image_ImageMap
  /* masks */
  qgn_prop_wml_square_mask,
  qgn_prop_wml_disc_mask,
  qgn_prop_wml_circle_mask,
  qgn_prop_wml_image_not_shown_mask,
  qgn_prop_wml_image_not_shown_mask,
  qgn_prop_wml_selectarrow_mask,
  qgn_prop_wml_object_mask,
  qgn_prop_wml_selectfile_mask,
  qgn_prop_wml_image_map_mask
  };

/* ------------------------------------------------------------------------- */
const
NW_Image_CannedImages_Class_t NW_Image_CannedImages_Class = {
  { /* NW_Object_Core       */
    /* super                   */ &NW_Object_Dynamic_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Image_CannedImages_t),
    /* construct		       */ _NW_Image_CannedImages_Construct,
    /* destruct			       */ _NW_Image_CannedImages_Destruct
  },
  { /* NW_CannedImages      */
    /* numEntries           */ 0,
    /* entries              */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

TBrowserStatusCode
_NW_Image_CannedImages_Construct (NW_Object_Dynamic_t* dynamicObject,
                                  va_list *argp)
{
  NW_Image_CannedImages_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_Image_CannedImages_Class));
  NW_REQUIRED_PARAM (argp);


  /* for convenience */
  thisObj = NW_Image_CannedImagesOf (dynamicObject);

  /* load all images from the MBM file */
  NW_Image_CannedImages_CreateDictionary(thisObj);

  /* successful completion */
  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- */
void
_NW_Image_CannedImages_Destruct (NW_Object_Dynamic_t* dynamicObject)
{
  NW_Image_CannedImages_t* thisObj;
  NW_Uint8 ii;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (dynamicObject, &NW_Image_CannedImages_Class));

  /* for convenience */
  thisObj = NW_Image_CannedImagesOf (dynamicObject);

  /* deallocate our dynamic memory */

  /* remove all loaded images in the dictionary */
  for (ii=0; ii<NW_Image_NumberOfCannedImages; ii++)
  {
   if (thisObj->dictionary[ii]!=NULL)
   {
     NW_Object_Delete(thisObj->dictionary[ii]);
     thisObj->dictionary[ii]=NULL;
   }
  }
}


/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Image_AbstractImage_t*
NW_Image_CannedImages_GetImage (NW_Image_CannedImages_t* thisObj,
                                NW_Int8 imageId)
{
  const NW_Image_AbstractImage_t* image = NULL;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_Image_CannedImages_Class));
  NW_REQUIRED_PARAM(thisObj);

  if (imageId>NW_Image_Invalid && imageId<=NW_Image_NumberOfCannedImages)
      {
      /* imageId-1 because we're skipping NW_Image_Invalid */
      image=thisObj->dictionary[imageId-1];
      }

  /* return the image if found, otherwise NULL */
  return image;
}

TBrowserStatusCode
NW_Image_CannedImages_CreateDictionary(NW_Image_CannedImages_t* thisObj)
{
  TFileName mbmDrive;
  HBufC* mbmFile = NULL;
  CFbsBitmap* bitmap = NULL;
  CFbsBitmap* mask = NULL;
  TParse parse;
  NW_Uint8 ii;

  /* get the drive letter - MBM is on the same drive as this DLL */
  Dll::FileName( mbmDrive );
  parse.Set( mbmDrive, NULL, NULL );
  mbmDrive = parse.Drive();

  NW_TRY(status)
  {
    /* allocate memory for MBM file name */
    mbmFile = HBufC::New( KMaxFileName );
    NW_THROW_OOM_ON_NULL(mbmFile, status);

    /* assemble the MBM file name */
    TPtr ptr( mbmFile->Des() );
    ptr.Append( mbmDrive );

	TFileName KBrowserBitmapsFolder;
	//_LIT(KDC_APP_BITMAP_DIR,"\\resource\\apps\\localisablefiles\\"); 	//	Bitmap files (.mbm)

	KBrowserBitmapsFolder += KDC_APP_BITMAP_DIR;
	KBrowserBitmapsFolder += KBrowserBitmapsName;
	KBrowserBitmapsFolder.ZeroTerminate();

    ptr.Append( KBrowserBitmapsFolder );

    /* load canned images from MBM file into the dictionary */
    for (ii=0; ii<NW_Image_NumberOfCannedImages; ii++)
    {
      TInt error;

      /* allocate a bitmap object */
      bitmap = NULL;
      bitmap = new CFbsBitmap();
      NW_THROW_OOM_ON_NULL(bitmap, status);

      /* load an image from the MBM file */
      error = bitmap->Load( mbmFile->Des(),
                                 NW_Image_CannedImages_Dictionary[ii] );
      if (error!=KErrNone)
      {
        NW_THROW_STATUS(status, KBrsrOutOfMemory);
      }

      /* load the mask if it exist */
      mask = NULL;
      if (NW_Image_CannedImages_Dictionary[ii+NW_Image_NumberOfCannedImages])
      {
        /* allocate a mask object */
        mask = new CFbsBitmap();
        NW_THROW_OOM_ON_NULL(mask, status);

        /* load an image from the MBM file */
        error = mask->Load( mbmFile->Des(),
          NW_Image_CannedImages_Dictionary[ii+NW_Image_NumberOfCannedImages] );
        if (error!=KErrNone)
        {
          NW_THROW_STATUS(status, KBrsrOutOfMemory);
        }
      }

      /* create KM image object to hold the loaded image */
      NW_GDI_Dimension3D_t gdiSize;
      TSize size = bitmap->SizeInPixels();

      gdiSize.width = (NW_GDI_Metric_t)size.iWidth;
      gdiSize.height = (NW_GDI_Metric_t)size.iHeight;
      gdiSize.depth = 1;

      NW_Image_Epoc32Canned_t* simpleImage =
          NW_Image_Epoc32Canned_New (gdiSize,
                                     bitmap,
                                     mask,
                                     NW_TRUE,
                                     NW_FALSE,
                                     ERecognizedImage,
                                     NULL,
                                     0);
      NW_THROW_OOM_ON_NULL(simpleImage, status);

      thisObj->dictionary[ii] = NW_Image_AbstractImageOf( simpleImage );
    }
  }
  NW_CATCH(status)
  {
    delete bitmap;
    /* remove all loaded images in the dictionary */
    for (ii=0; ii<NW_Image_NumberOfCannedImages; ii++)
    {
     if (thisObj->dictionary[ii]!=NULL)
     {
       NW_Image_Epoc32Canned_Destroy(thisObj->dictionary[ii]);
       NW_Object_Delete(thisObj->dictionary[ii]);
       thisObj->dictionary[ii]=NULL;
     }
    }
  }
  NW_FINALLY
  {
    /* Delete folder buffer */
    delete mbmFile;

    return status;
  }
  NW_END_TRY
}

NW_Image_CannedImages_t*
NW_Image_CannedImages_New()
{
  return (NW_Image_CannedImages_t*)
      NW_Object_New (&NW_Image_CannedImages_Class);
}
