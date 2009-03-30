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


#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// This enum allows the BrowserUI to display the image content-types
// that are supported and displayable by the browser, but not recognized by
// the SymbianOS.  These are mostly image data with no indication of their
// content-type in their data, i.e. GIF files have "GIF89" as the first
// few bytes in the data.
// ERecognizedImage:  SymbianOS recognizes the image content-type and no
// special handling is required.
// EUnsupportedImage:  The browser and SymbianOS doesn't support this
// image and it will not display it.
// EWbmpImage, EOtaImage, etc: These are images that we support and display, 
// but SymbianOS doesn't recognize, so we set this marker for special handling.
typedef enum
    {
    ERecognizedImage = 0,
    EUnsupportedImage,
    EWbmpImage,
    EOtaImage
    } TImageType;


TImageType GetRecognizedImageType(const char* aContentTypeString);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */


#endif /* IMAGEUTILS_H */
