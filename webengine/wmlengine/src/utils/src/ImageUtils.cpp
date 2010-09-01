/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  String related utility methods.
*
*/

// INCLUDE FILES
#include <e32base.h>

#include "ImageUtils.h"
#include "nwx_http_defs.h"
#include "nwx_string.h"

extern const unsigned char HTTP_image_x_ota_bitmap_string[];
extern const unsigned char HTTP_image_vnd_nokia_ota_bitmap_string[];

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ImageUtils::GetRecognizedImageType
//
// This method determines if the content-type of the image is recognizable by
// the SymbianOS.  Some image types don't contain their content-type in the
// data, so we manually recognize them.
// -----------------------------------------------------------------------------
TImageType GetRecognizedImageType(const char* aContentTypeString)
    {
    // We default to assuming the SymbianOS (and BrowserUI) can recognize and
    // display the image.
    TImageType imageType = ERecognizedImage;

    // Find any images that require special handling, i.e. images that can't
    // be recognized but can be displayed.
    if (0 == NW_Asc_stricmp((char*)HTTP_image_vnd_wap_wbmp_string,
                            aContentTypeString))
        {
        // We have a Wbmp image (image/vnd.wap.wbmp)
        imageType = EWbmpImage;
        }
    else if (0 == NW_Asc_stricmp((char*)HTTP_image_vnd_nokia_ota_bitmap_string,
                                 aContentTypeString))
        {
        // We have an Ota image (image/vnd.nokia.ota-bitmap)
        imageType = EOtaImage;
        }
    else if (0 == NW_Asc_stricmp((char*)HTTP_image_x_ota_bitmap_string,
                                 aContentTypeString))
        {
        // We have an Ota image (image/x-ota-bitmap)
        imageType = EOtaImage;
        }

    return imageType;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
