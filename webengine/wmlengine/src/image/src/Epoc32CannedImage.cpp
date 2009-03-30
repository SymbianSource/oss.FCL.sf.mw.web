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


#include <e32def.h>  // First to avoid NULL redefine warning (no #ifndef NULL).
#include "nw_image_epoc32cannedimagei.h"

#include "nw_gdi_types.h"

#include <fbs.h>
#include "BrsrStatusCodes.h"

/* ------------------------------------------------------------------------- *
   class definition
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const
NW_Image_Epoc32Canned_Class_t NW_Image_Epoc32Canned_Class = {
  { /* NW_Object_Core          */
    /* super                   */ &NW_Image_Epoc32Simple_Class,
    /* queryInterface          */ _NW_Object_Base_QueryInterface
  },
  { /* NW_Object_Base          */
    /* interfaceList           */ NULL
  },
  { /* NW_Object_Dynamic       */
    /* instanceSize            */ sizeof (NW_Image_Epoc32Canned_t),
    /* construct		       */ _NW_Image_Epoc32Simple_Construct,
    /* destruct			       */ _NW_Image_Epoc32Canned_Destruct
  },
  { /* NW_Image_Abstract       */
    /* getSize                 */ _NW_Image_Epoc32Simple_GetSize,
    /* draw                    */ _NW_Image_Epoc32Simple_Draw,
    /* drawScaled              */ _NW_Image_Epoc32Simple_DrawScaled,
    /* drawInRect              */ _NW_Image_Epoc32Simple_DrawInRect,
    /* incrementImage          */ _NW_Image_Epoc32Simple_IncrementImage,
    /* getLoopCount            */ _NW_Image_AbstractImage_GetLoopCount,
    /* setLoopCount            */ _NW_Image_AbstractImage_SetLoopCount,
    /* getDelay                */ _NW_Image_AbstractImage_GetDelay,
    /* isAnimated              */ _NW_Image_AbstractImage_IsAnimated,
    /* setImageObserver        */ NULL,
    /* removeImageObserver     */ NULL,
    /* imageOpened             */ NULL,
    /* imageSizeChanged        */ NULL,
    /* imageDecoded            */ NULL,
    /* imageOpenStarted        */ NULL,
    /* forceImageDecode        */ NULL
  },
  { /* NW_Image_Simple         */
    /* partialNextChunk        */ NULL
  },
  { /* NW_Image_Canned         */
    /* unused                  */ NW_Object_Unused
  }
};

/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */


/* ------------------------------------------------------------------------- */
void
_NW_Image_Epoc32Canned_Destruct (NW_Object_Dynamic_t* /*dynamicObject*/)
{
}


/* ------------------------------------------------------------------------- */
TBrowserStatusCode
NW_Image_Epoc32Canned_Destroy(NW_Image_AbstractImage_t* image)
{
  NW_Image_Epoc32Simple_t* thisObj;

  /* parameter assertion block */
  NW_ASSERT (NW_Object_IsInstanceOf (image, &NW_Image_Epoc32Simple_Class));

  /* for convenience */
  thisObj = NW_Image_Epoc32SimpleOf (image);

  delete (void*)thisObj->rawData;

  return KBrsrSuccess;
}

/* ------------------------------------------------------------------------- *
   convenience functions
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_Image_Epoc32Canned_t*
NW_Image_Epoc32Canned_New (NW_GDI_Dimension3D_t size, 
                           const void* data, 
                           const void* mask,
                           NW_Bool invertedMask,
                           NW_Bool isTransparent,
                           TImageType imageType,
                           const void* rawData,
                           NW_Int32 length)
{
  //lint -e{437} Passing struct to ellipsis
  return (NW_Image_Epoc32Canned_t*) NW_Object_New (&NW_Image_Epoc32Canned_Class, 
                                                   size, 
                                                   data, 
                                                   mask,
                                                   invertedMask,
                                                   isTransparent,
                                                   imageType,
                                                   rawData,
                                                   length, 
                                                   NULL);
}
