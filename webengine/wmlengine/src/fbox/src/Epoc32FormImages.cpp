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
#include "nw_fbox_epoc32formimagesi.h"

#include "nw_fbox_checkbox.h"
#include "nw_fbox_radiobox.h"
#include "nw_fbox_optionbox.h"
#include "nw_image_epoc32simpleimage.h"

#include <e32base.h>
#include <fbs.h>
#include "nwx_buffer.h"
#include "nw_browser_browserimages.h"

/* ------------------------------------------------------------------------- */
const
NW_FBox_FormImages_Class_t NW_FBox_FormImages_Class = {
  { /* NW_Object_Core       */
    /* super                */ &NW_Object_Base_Class,
    /* queryInterface       */ _NW_Object_Base_QueryInterface
  },
  { /* NW_FBox_FormImages   */
    /* numCheckBoxEntries   */ 0,
    /* checkBoxEntries      */ NULL,
    /* numCheckBoxEntries   */ 0,
    /* checkBoxEntries      */ NULL
  }
};

/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
const NW_Image_AbstractImage_t*
NW_FBox_FormImages_GetImage(NW_FBox_FormImages_t* thisObj,
                            NW_Bool hasFocus,
                            NW_Bool state,
                            NW_FBox_FormBox_t* formBox)
    {
    NW_Int16 index;
    NW_Uint8 numEntries;
    const NW_Image_AbstractImage_t* image = NULL;
    const NW_FBox_FormImages_Entry_t* entry;

    /*lint --e{794} --e{550} Conceivable use of null pointer, Symbol not accessed */

    /* parameter assertion block */
    NW_ASSERT (thisObj != NULL);
    NW_ASSERT (NW_Object_IsInstanceOf (thisObj, &NW_FBox_FormImages_Class));

    numEntries = 0;
    entry = NULL;
    if (NW_Object_IsInstanceOf(formBox, &NW_FBox_RadioBox_Class))
        {
        numEntries = NW_FBox_FormImages_GetClassPart (thisObj).numRadioBoxEntries;
        entry = NW_FBox_FormImages_GetClassPart (thisObj).radioBoxEntries;
        }
    else if (NW_Object_IsInstanceOf(formBox, &NW_FBox_CheckBox_Class))
        {
        numEntries = NW_FBox_FormImages_GetClassPart (thisObj).numCheckBoxEntries;
        entry = NW_FBox_FormImages_GetClassPart (thisObj).checkBoxEntries;
        }

    /* iterate through the table looking for a match */
    for (index = numEntries; index >= 0; index--)
        {
        if ((hasFocus == entry->focus) &&  (state == entry->state))
            {
            break;
            }
        index++;
        entry++;
        }

    TFileName mbmDrive;
    Dll::FileName(mbmDrive);
    TParse parse;
    parse.Set(mbmDrive, NULL, NULL);
    mbmDrive = parse.Drive();

    HBufC* mbmFile = HBufC::New( KMaxFileName );

    if (mbmFile == NULL )
        {
        /* Could not allocate memory for mbmFile, return from function with NULL*/
        return NULL;
        }

    TPtr ptr(mbmFile->Des());

    ptr.Append(mbmDrive);

	TFileName KBrowserBitmapsFolder;
	//_LIT(KDC_APP_BITMAP_DIR,"\\resource\\apps\\localisablefiles\\"); 	//	Bitmap files (.mbm)

	KBrowserBitmapsFolder += KDC_APP_BITMAP_DIR;
	KBrowserBitmapsFolder += KBrowserBitmapsName;
	KBrowserBitmapsFolder.ZeroTerminate();

	ptr.Append(KBrowserBitmapsFolder);

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    TRAPD(error, bitmap = new (ELeave)CFbsBitmap());

    error = bitmap->Load(mbmFile->Des(), entry->imageId);

    if (error == KErrNone)
        {
        /* Load mask for the image */
        TRAP(error, mask = new (ELeave)CFbsBitmap());

        error = mask->Load(mbmFile->Des(), entry->maskId);
        if (error == KErrNone)
            {
            /* Image loaded succesfully, create Kimono image object to hold it */
            TSize size = bitmap->SizeInPixels();

            NW_GDI_Dimension3D_t gdiSize;
            gdiSize.width = (NW_GDI_Metric_t)size.iWidth;
            gdiSize.height = (NW_GDI_Metric_t)size.iHeight;
            gdiSize.depth = 1;

            /* TODO: Here we are creating image object, but who will DELETE it ? */
            const NW_Image_Epoc32Simple_t* simpleImage =
                NW_Image_Epoc32Simple_New(gdiSize, bitmap, mask, NW_TRUE, NW_FALSE,
                                          ERecognizedImage, NULL, 0, NULL);

            NW_ASSERT(simpleImage != NULL);

            image = NW_Image_AbstractImageOf(simpleImage);
            }
        else
            {
            delete mask;
            }
        }
    else
        {
        delete bitmap;
        }

    delete mbmFile;
    return image;
}
